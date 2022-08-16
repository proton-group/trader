// [project trader]
// designed by Anatoly Alexandrov
// License:
// You can view, modify, compile and run the program. It is forbidden to use the program or source code or parts thereof for monetary gain.

#ifndef _TRADER_HPP
#define _TRADER_HPP
#include "exchange.hpp"
#include <vector>

class Trader
{
public:
    void manage_positions_on_forex(Exchange& market);

    template<typename iterator>
    void coters_analysing(iterator cotersBegin, iterator cotersEnd); //как определить шаблон внутри класса в cpp, а не файле tpp? В tpp неправильно, ибо есть нешаблонные методы.
    
    void set_analysing_period(std::time_t period);
    void set_balance(double balance);
    void set_credit_lever_limit(int limit);
    void set_base_credit_lever(int lever);
    double get_balance() const;
    void close_positions(Exchange& market);
    double get_stochastic() const;
    double get_high_price() const;
    double get_low_price() const;
    double get_price() const;

    std::vector<Position_id>::iterator my_positions_begin();
    std::vector<Position_id>::iterator my_positions_end();
private:
    double deposit;
    double balance_usd;
    double stochastic_value;
    double high_price;
    double low_price;
    double price;
    double volatility;
    int credit_lever_limit{1};
    int base_credit_lever{1};
    std::time_t _period{Watch::hour};
    std::vector<Position_id> my_positions;
};
#endif