#include "exchange.hpp"
#include <iostream>
#include "cotersParser.hpp"
#include "trader.hpp"
#include <gtest/gtest.h>

TEST(exchange, basic)
{
    Exchange market;
    std::shared_ptr<USD_Wallet> usd = std::make_shared<USD_Wallet>(100);
    std::shared_ptr<EURO_Wallet> euro = std::make_shared<EURO_Wallet>(0);
    market.add_indirect_coter(1, usd->get_security_id(), 1);
    market.add_indirect_coter(1, euro->get_security_id(), 10);
    Position_id pos1 = market.add_position(usd, euro, 10, 1);
    ASSERT_TRUE(market.get_firstsec_balance(pos1) == 1000);
    market.buy_secondsec(pos1, 100);
    ASSERT_TRUE(market.get_firstsec_balance(pos1) == 0);
    ASSERT_TRUE(market.get_secondsec_balance(pos1) == 100);
    market.add_indirect_coter(2, euro->get_security_id(), 9);
    ASSERT_TRUE(market.is_closed(pos1) == true);
}

TEST(exchange, zero_position)
{
    Exchange market;
    std::shared_ptr<USD_Wallet> usd = std::make_shared<USD_Wallet>(0);
    std::shared_ptr<EURO_Wallet> euro = std::make_shared<EURO_Wallet>(0);
    market.add_indirect_coter(1, usd->get_security_id(), 1);
    market.add_indirect_coter(1, euro->get_security_id(), 10);
    Position_id pos1 = market.add_position(usd, euro, 1, 1);
    ASSERT_TRUE(market.get_firstsec_balance(pos1) == 0);
    ASSERT_TRUE(market.get_secondsec_balance(pos1) == 0);
    market.add_indirect_coter(2, euro->get_security_id(), 10);
    ASSERT_TRUE(market.is_closed(pos1) == true);
}

TEST(exchange, exceptions)
{
    Exchange market;
    std::shared_ptr<USD_Wallet> usd = std::make_shared<USD_Wallet>(100);
    std::shared_ptr<EURO_Wallet> euro = std::make_shared<EURO_Wallet>(0);
    try
    {
        Position_id pos2 = market.add_position(usd, euro, 10, 2);
    }
    catch(const Exchange::exchange_rate_not_set& e)
    {
        std::cout << "First exception catched" << '\n';
    }
    market.add_indirect_coter(1, usd->get_security_id(),1);
    market.add_indirect_coter(1, euro->get_security_id(),10);
    std::shared_ptr<USD_Wallet> usd2 = std::make_shared<USD_Wallet>(100);
    std::shared_ptr<EURO_Wallet> euro2 = std::make_shared<EURO_Wallet>(0);
    try
    {
        Position_id pos2 = market.add_position(usd2, euro2, 0, 1);
    }
    catch(const Exchange::incorrect_margin_or_credit_lever& e)
    {
        std::cout << "Second exception catched" << '\n';
    }
    
}

TEST(exchange, balanceOut)
{
    Exchange market;
    std::shared_ptr<USD_Wallet> usd = std::make_shared<USD_Wallet>(100);
    std::shared_ptr<EURO_Wallet> euro = std::make_shared<EURO_Wallet>(0);
    market.add_indirect_coter(1, euro->get_security_id(), 10);
    market.add_indirect_coter(1, usd->get_security_id(), 1);
    Position_id pos1 = market.add_position(usd, euro, 10, 1);
    ASSERT_TRUE(market.get_firstsec_balance(pos1) == 1000);
    market.buy_secondsec(pos1, 100);
    ASSERT_TRUE(market.get_firstsec_balance(pos1) == 0);
    ASSERT_TRUE(market.get_secondsec_balance(pos1) == 100);
    market.add_indirect_coter(2, euro->get_security_id(), 10.5);
    ASSERT_TRUE(market.is_closed(pos1) == false);
    market.buy_firstsec(pos1, 1050);
    ASSERT_TRUE(market.get_firstsec_balance(pos1) == 1050);
    ASSERT_TRUE(market.get_secondsec_balance(pos1) == 0);
    ASSERT_TRUE(market.is_closed(pos1) == false);
    ASSERT_TRUE(market.close_position(pos1) == 150);
    ASSERT_TRUE(market.is_closed(pos1) == true);
}

TEST(trader, shortPosition)
{
    std::stringstream teststring;
    teststring << "00:00:00 27.02.2022 0.9151\n";	
    teststring << "00:00:00 12.04.2022 0.9234\n";
    teststring << "00:00:00 13.04.2022 0.9184\n";
    teststring << "00:00:00 14.04.2022 0.9233\n";
    teststring << "00:00:00 15.04.2022 0.9251\n";
    teststring << "00:00:00 18.04.2022 0.9273\n";
    teststring << "00:00:00 19.04.2022 0.9268\n";
    teststring << "00:00:00 20.04.2022 0.9212\n";
    teststring << "00:00:00 21.04.2022 0.9225\n";
    teststring << "00:00:00 22.04.2022 0.9263\n";
    teststring << "00:00:00 25.04.2022 0.9333\n";
    teststring << "00:00:00 26.04.2022 0.9399\n";
    teststring << "00:00:00 27.04.2022 0.9471\n";
    teststring << "00:00:00 28.04.2022 0.9522\n";
    teststring << "00:00:00 29.04.2022 0.9483\n";
    teststring << "00:00:00 02.05.2022 0.9518\n";
    teststring << "00:00:00 03.05.2022 0.9501\n";
    CotersParser parser(teststring);
    Exchange market;
    parser.set_coters_on_exchange(market, Security_id("USD"));
    market.add_indirect_coter(0, Security_id("EURO"), 1);
    Trader bot;
    const int deposit = 100;
    bot.set_balance(deposit);
    bot.set_base_credit_lever(0);
    bot.set_analysing_period(365 * Watch::day);
    bot.coters_analysing<std::vector<Coter>::iterator>(market.cotersList_begin(Security_id("USD")), market.cotersList_end(Security_id("USD"))); //для определенных валют
    bot.manage_positions_on_forex(market);
    std::stringstream teststring2;
    teststring2 << "00:00:00 04.05.2022 0.9412\n";
    teststring2 << "00:00:00 05.05.2022 0.9484\n";
    teststring2 << "00:00:00 06.05.2022 0.9474\n";
    teststring2 << "00:00:00 09.05.2022 0.9471\n";
    teststring2 << "00:00:00 10.05.2022 0.9496\n";
    teststring2 << "00:00:00 11.05.2022 0.9210\n";
    teststring2 << "00:00:00 12.05.2022 0.9240\n";
    CotersParser parser2(teststring2);
    parser2.set_coters_on_exchange(market, Security_id("USD"));
    bot.coters_analysing<std::vector<Coter>::iterator>(market.cotersList_begin(Security_id("USD")), market.cotersList_end(Security_id("USD")));
    bot.manage_positions_on_forex(market);
    bot.close_positions(market);
    ASSERT_TRUE(bot.get_balance() > deposit);
}

TEST(trader, longPosition)
{
    std::stringstream teststring;
    teststring << "00:00:00 27.02.2022 0.9263\n"; 
    teststring << "00:00:00 12.04.2022 0.9333\n";
    teststring << "00:00:00 13.04.2022 0.9399\n";
    teststring << "00:00:00 14.04.2022 0.9471\n";
    teststring << "00:00:00 15.04.2022 0.9522\n";
    teststring << "00:00:00 18.04.2022 0.9483\n";
    teststring << "00:00:00 19.04.2022 0.9518\n";
    teststring << "00:00:00 21.04.2022 0.9501\n";
    teststring << "00:00:00 22.04.2022 0.9151\n";	
    teststring << "00:00:00 25.04.2022 0.9234\n";
    teststring << "00:00:00 26.04.2022 0.9184\n";
    teststring << "00:00:00 27.04.2022 0.9233\n";
    teststring << "00:00:00 28.04.2022 0.9251\n";
    teststring << "00:00:00 29.04.2022 0.9273\n";
    teststring << "00:00:00 02.05.2022 0.9268\n";
    teststring << "00:00:00 03.05.2022 0.9212\n";
    CotersParser parser(teststring);
    Exchange market;
    parser.set_coters_on_exchange(market, Security_id("USD"));
    market.add_indirect_coter(0, Security_id("EURO"), 1);
    Trader bot;
    const int deposit = 100; //in EURO but course is usd/euro
    bot.set_balance(deposit);
    bot.set_base_credit_lever(0);
    bot.set_analysing_period(365 * Watch::day);
    bot.coters_analysing<std::vector<Coter>::iterator>(market.cotersList_begin(Security_id("USD")), market.cotersList_end(Security_id("USD")));
    bot.manage_positions_on_forex(market);
    std::stringstream teststring2;
    teststring2 << "00:00:00 04.05.2022 0.9412\n";
    teststring2 << "00:00:00 05.05.2022 0.9484\n";
    teststring2 << "00:00:00 06.05.2022 0.9474\n";
    teststring2 << "00:00:00 09.05.2022 0.9471\n";
    teststring2 << "00:00:00 10.05.2022 0.9496\n";
    CotersParser parser2(teststring2);
    parser2.set_coters_on_exchange(market, Security_id("USD"));
    bot.coters_analysing<std::vector<Coter>::iterator>(market.cotersList_begin(Security_id("USD")), market.cotersList_end(Security_id("USD")));
    bot.manage_positions_on_forex(market);
    bot.close_positions(market);
    ASSERT_TRUE(bot.get_balance() > deposit);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}