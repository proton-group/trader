/*
FILE:
HH:MM:SS DD:MM:YYYY X.XXXX
HH:MM:SS DD:MM:YYYY X.XXXX
HH:MM:SS DD:MM:YYYY X.XXXX
HH:MM:SS DD:MM:YYYY X.XXXX
HH:MM:SS DD:MM:YYYY X.XXXX
*/
#ifndef _COTERSPARSER_HPP_
#define _COTERSPARSER_HPP_
#include <istream>
#include "financial_structurs.hpp"
#include "exchange.hpp"
class CotersParser
{
    std::istream& _is;
public:
    CotersParser(std::istream& is): _is(is) {};
    void set_coters_on_exchange(Exchange& market, const Security_id& id);
};

#endif