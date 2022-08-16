#include "trader.hpp"
#include "cotersParser.hpp"
#include <iostream>
#include <fstream>
// main period balance

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "invalid input\n";
        return -1;
    }
    std::ifstream cotersfile;
    CotersParser parser(cotersfile);
    Exchange market;
    Trader bot;

    bot.set_analysing_period(std::stoi(argv[1]));
    bot.set_balance(std::stoi(argv[2]));

    try
    {
        char cotersfilename[40];
        while(1)
        {
            std::cout << "input coters file or end:\n";
            std::cin >> cotersfilename;
            if(cotersfilename[0] == 'e' && cotersfilename[1] == 'n' && cotersfilename[2] == 'd')
            {
                break;
            }
            std::cout << "\n";
            cotersfile.open(cotersfilename);
            parser.set_coters_on_exchange(market, Security_id("USD"));
            bot.coters_analysing(market.cotersList_begin(Security_id("USD")), market.cotersList_end(Security_id("USD")));
            bot.manage_positions_on_forex(market);
        }
    }
    catch(const Exchange::incorrect_margin_or_credit_lever& e)
    {
        std::cerr << "margin or credit_lever incorrect"<< '\n';
    }
    catch(const Exchange::exchange_rate_not_set& e)
    {
        std::cerr << "coters not set"<< '\n';
    }
    bot.close_positions(market);
    std::cout << bot.get_balance() << std::endl;
    
    return 0;
}