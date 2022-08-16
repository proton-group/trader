#include "trader.hpp"
namespace
{
    bool lower(Coter a, Coter b)
    {
        return a.value < b.value;
    }
    bool greater(Coter a, Coter b)
    {
        return a.value > b.value;
    }
    double stochastic_oscillator(double price, double low_price, double high_price)
    {
        const int average = 50;
        if (high_price != low_price)
        {
            return (price - low_price)/(high_price-low_price)*100;
        }
        return average;
    }

    template<typename iterator>
    double low_border(time_t interval, iterator cotersBegin, iterator cotersEnd)
    {
        return std::min_element(std::find_if(cotersBegin, cotersEnd, [&](Coter cur){return cur.date >= cotersEnd->date - interval;}), cotersEnd, lower) -> value;
    }
    template<typename iterator>
    double high_border(time_t interval, iterator cotersBegin, iterator cotersEnd)
    {
        return std::max_element(std::find_if(cotersBegin, cotersEnd, [&](Coter cur){return cur.date >= cotersEnd->date - interval;}), cotersEnd, lower) -> value;
    }
}

double Trader::get_stochastic() const
{
    return stochastic_value;
}

double Trader::get_high_price() const
{
    return high_price;
}

double Trader::get_low_price() const
{
    return low_price;
}

double Trader::get_price() const
{
    return price;
}

template<>
void Trader::coters_analysing(std::vector<Coter>::iterator cotersBegin, std::vector<Coter>::iterator cotersEnd)
{
    high_price = high_border(_period, cotersBegin, cotersEnd);
    low_price = low_border(_period, cotersBegin, cotersEnd);
    Coter test1 = *(--cotersEnd);
    Coter test2 = *(cotersBegin);
    price = (--cotersEnd)->value;
    stochastic_value = stochastic_oscillator(cotersEnd->value, low_price, high_price);
    volatility = high_price - low_price;
    return;
}

void Trader::manage_positions_on_forex(Exchange& market)
{
    // релазиовать удаление закрытых счетов и докуп
    const int oversold_indicator = 20;
    const int overbuy_indicator = 80;
    const float buy_border = 1.01;
    const float sold_border = 0.99;
    const int credit_margin_call_salfety_level = 20;
    //(credit_lever * deposit) + volatility < deposite / 2
    const int credit_lever = std::clamp(base_credit_lever, 1, (int)(credit_margin_call_salfety_level/(abs(price - volatility)/price)));
    if (balance_usd > deposit/2)
    {
        bool check_longPosBuyingStatus = stochastic_value <= oversold_indicator && price < low_price*buy_border;
        bool check_shortPosBuyingStatus = stochastic_value >= overbuy_indicator && price > high_price*sold_border;
        if(check_longPosBuyingStatus || check_shortPosBuyingStatus)
        {
            std::shared_ptr<USD_Wallet> usd = std::make_shared<USD_Wallet>(balance_usd - deposit/2);
            std::shared_ptr<EURO_Wallet> euro = std::make_shared<EURO_Wallet>(0);
            if(check_longPosBuyingStatus)
            {
                my_positions.push_back(market.add_position(usd, euro, std::clamp(credit_lever, 1, credit_lever_limit), 1));
                market.buy_secondsec(my_positions.back(), balance_usd - deposit/2);
            }
            if(check_shortPosBuyingStatus)
            {
                my_positions.push_back(market.add_position(usd, euro, std::clamp(credit_lever, 1, credit_lever_limit), 2));
                market.buy_firstsec(my_positions.back(), balance_usd - deposit/2);
            }
            balance_usd -= deposit/2;
        }
    }
    if(balance_usd > 0)
    {
        bool check_shortPosSellingStatus = stochastic_value <= oversold_indicator || price < low_price*buy_border;
        bool check_longPosSellingStatus = stochastic_value >= overbuy_indicator || price > high_price*sold_border;
        if(check_longPosSellingStatus || check_shortPosSellingStatus)
        {
            for_each(my_positions.begin(), my_positions.end(), 
            [&](Position_id id)
            {
                if(check_shortPosSellingStatus && market.get_firstsec_balance(id) > 0)
                {
                    balance_usd += market.close_position(id);
                }
                if(check_longPosSellingStatus && market.get_secondsec_balance(id) > 0)
                {
                    balance_usd += market.close_position(id);
                }
            });
        }
    }
    for(std::vector<Position_id>::iterator it = my_positions.begin(); it != my_positions.end();)
    {
        if (market.is_closed(*it)) {
            it = my_positions.erase(it);
        } else {
            ++it;
        }
    }
    return;
}

void Trader::set_analysing_period(std::time_t period)
{
    _period = period; 
}

void Trader::set_balance(double balance)
{
    balance_usd = balance;
    deposit = balance;
}

void Trader::set_credit_lever_limit(int limit)
{
    credit_lever_limit = limit;
}
void Trader::set_base_credit_lever(int lever)
{
    base_credit_lever = lever;
}

void Trader::close_positions(Exchange& market)
{
    std::for_each(my_positions.begin(), my_positions.end(), [&](Position_id id){balance_usd += market.close_position(id);});
    return;
}

double Trader::get_balance() const
{
    return balance_usd;
}

std::vector<Position_id>::iterator Trader::my_positions_begin()
{
    return my_positions.begin();
}

std::vector<Position_id>::iterator Trader::my_positions_end()
{
    return my_positions.end();
}