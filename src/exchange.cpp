#include "exchange.hpp"

void Exchange::add_indirect_coter(const std::time_t date, const Security_id& s_id, const double value)
{
    const double marginCall_border = 0.7;
    if(cotersHistory.find(s_id) == cotersHistory.end())
    {
        std::vector<Coter> clist;
        cotersHistory[s_id] = clist;
    }
    std::vector<Coter>& cotersList = cotersHistory[s_id];
    if(cotersList.begin() != cotersList.end())
    {
        if(cotersList.back().date >= date)
        {
            return;
        }
    }
    Coter fresh = {date, value};
    cotersList.push_back(fresh);
    for(auto cpos = posList.begin(); cpos != posList.end();)
    {
        const auto& [p_id, pos] = *cpos;
        double fcost = cotersHistory.at(pos.first_security->get_security_id()).back().value;
        double scost = cotersHistory.at(pos.second_security->get_security_id()).back().value;
        if(pos.first_security->get_security_id() == s_id || pos.second_security->get_security_id() == s_id)
        {
            if(pos.margin*pos.credit_lever - (pos.first_security->get_balance() * fcost + pos.second_security->get_balance() * scost) >= pos.margin * marginCall_border)
            {
                cpos = posList.erase(cpos);
            }
            else
            {
                ++cpos;
            }
        }
    }
    return;
}

Position_id Exchange::add_position(std::shared_ptr<Wallet> first_security, std::shared_ptr<Wallet> second_security, const int credit_lever, const int sec_credit_selecter) //uniptr
{
    if(sec_credit_selecter != 1 && sec_credit_selecter != 2)
    {
        throw incorrect_margin_or_credit_lever();
    }
    const auto& margin = first_security->get_balance();
    if(margin < 0 || credit_lever < 1)
    {
        throw incorrect_margin_or_credit_lever();
    }
    std::vector<Coter> cotersList;
    if(sec_credit_selecter == 1)
    {
        if(cotersHistory.find(first_security->get_security_id()) == cotersHistory.end())
        {
            throw exchange_rate_not_set();
        }
        cotersList = cotersHistory.at(first_security->get_security_id());
  
    }
    if (sec_credit_selecter == 2)
    {
        if(cotersHistory.find(second_security->get_security_id()) == cotersHistory.end())
        {
            throw exchange_rate_not_set();
        }
        cotersList = cotersHistory.at(second_security->get_security_id());
    }
    Position newpos;
    newpos.margin = margin;
    newpos.credit_lever = credit_lever;
    newpos.first_security = std::move(first_security);
    newpos.second_security = std::move(second_security);
    double lastCost = cotersList.back().value;
    if(cotersList.begin() != cotersList.end())
    {
        if(sec_credit_selecter == 2)
        {
            newpos.second_security->get_balance() = credit_lever*margin/lastCost;
        }
        if(sec_credit_selecter == 1)
        {
            newpos.first_security->get_balance() = credit_lever*margin/lastCost;
        }
    }
    else
    {
        throw exchange_rate_not_set();
    }
    auto newid = Position_id(posList.size());
    posList[newid] = newpos;
    return newid;
}

double Exchange::close_position(const Position_id& id)
{
    if(is_closed(id))
    {
        return 0;
    }

    double scost = cotersHistory.at(posList[id].second_security->get_security_id()).back().value;
    double fcost = cotersHistory.at(posList[id].first_security->get_security_id()).back().value;

    double balance = (get_firstsec_balance(id)*fcost + get_secondsec_balance(id) * scost) - posList[id].margin * (posList[id].credit_lever - 1); 
    posList.erase(id);
    return balance;
}

static bool buy(const Position_id& id, double value, double cost, const Exchange& ex, std::shared_ptr<Wallet>& selsec, std::shared_ptr<Wallet>& buysec)
{
    if(ex.is_closed(id) == false)
    {
        buysec->buy_security(value);
        selsec->sell_security(value * cost);
    }
    return false;
}

bool Exchange::buy_secondsec(const Position_id& id, double value)
{
    auto& buysec = posList[id].second_security;
    auto& selsec = posList[id].first_security;
    //if there are not enough funds for the operation - return false;
    return buy(id, value, (cotersHistory.at(buysec->get_security_id())).back().value / (cotersHistory.at(selsec->get_security_id())).back().value, *this, selsec, buysec);
}

bool Exchange::buy_firstsec(const Position_id& id, double value)
{
    auto& selsec = posList[id].second_security;
    auto& buysec = posList[id].first_security;
    //if there are not enough funds for the operation - return false;
    return buy(id, value, (cotersHistory.at(buysec->get_security_id())).back().value / (cotersHistory.at(selsec->get_security_id())).back().value, *this, selsec, buysec);
}

double Exchange::get_firstsec_balance(const Position_id& id) const
{
    if(is_closed(id) != false)
    {
        return 0;
    }
    return posList.at(id).first_security->get_balance();
 
}

double Exchange::get_secondsec_balance(const Position_id& id) const
{
    if(is_closed(id) != false)
    {
        return 0;
    }
    return posList.at(id()).second_security->get_balance();

}
bool Exchange::is_closed(const Position_id& id) const
{
    return posList.find(id) == posList.end();
}

std::vector<Coter>::iterator Exchange::cotersList_begin(const Security_id& id)
{
    return (cotersHistory.at(id)).begin();
}

std::vector<Coter>::iterator Exchange::cotersList_end(const Security_id& id)
{
    return (cotersHistory.at(id)).end();
}

std::map<Position_id, Position, posid_comp>::iterator Exchange::positionList_begin()
{
    return posList.begin();
}
std::map<Position_id, Position, posid_comp>::iterator Exchange::positionList_end()
{
    return posList.end();
}