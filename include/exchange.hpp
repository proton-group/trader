#ifndef _EXCHANGE_HPP
#define _EXCHANGE_HPP
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include "financial_structurs.hpp"

struct posid_comp
{
    bool operator()(const Position_id& f, const Position_id& s) const
    {
        return f < s;
    }
};

struct coter_hash
{
    size_t operator()(const Security_id& id) const
    {
        std::hash<std::string> hasher;
        return hasher(id());
    }
};

struct coter_equal
{
    bool operator()(const Security_id& f, const Security_id& s) const
    {
        return f == s;
    }
};

class Exchange
{
public:
    Exchange() = default;
    virtual ~Exchange() = default;
    void add_indirect_coter(const std::time_t date, const Security_id& s_id, const double value);

    Position_id add_position(std::shared_ptr<Wallet> first_security, std::shared_ptr<Wallet> second_security, int credit_lever, int sec_credit_selecter); //return position id;
    double close_position(const Position_id& id); //return balance in usd (without credit)

    double get_firstsec_balance(const Position_id& id) const; //return current balance in usd;
    double get_secondsec_balance(const Position_id& id) const; //return current balance in euro;
    bool buy_secondsec(const Position_id& id, double value); //TODO: positionid struct
    bool buy_firstsec(const Position_id& id, double value);

    bool is_closed(const Position_id& id) const;

    std::vector<Coter>::iterator cotersList_begin(const Security_id& id);
    std::vector<Coter>::iterator cotersList_end(const Security_id& id);
    //std::insert_iterator<std::vector<Coter>> cortersList_insert_iter();

    //Position iterators for work with real exchange API
    std::map<Position_id, Position, posid_comp>::iterator positionList_begin();
    std::map<Position_id, Position, posid_comp>::iterator positionList_end();

    class incorrect_margin_or_credit_lever : public std::exception {};
    class exchange_rate_not_set : public std::exception {};
private:
    std::unordered_map<Security_id, std::vector<Coter>, coter_hash, coter_equal> cotersHistory;
    std::map<Position_id, Position, posid_comp> posList;
};

#endif