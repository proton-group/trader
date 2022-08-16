#include "financial_structurs.hpp"

Security_id USD_Wallet::get_security_id()
{
    return Security_id("USD");
}

Security_id EURO_Wallet::get_security_id()
{
    return Security_id("EURO");
}

void Wallet::buy_security(double value)
{
    balance += value;
}

std::string Security_id::operator()() const
{
    return _id;
}

void Wallet::sell_security(double value)
{
    balance -= value;
    if (balance < 0)
    {
        throw invalid_balance_value();
    }
}
double Wallet::get_balance() const
{
    return balance;
}

double& Wallet::get_balance()
{
    return balance;
}

bool Security_id::operator==(const Security_id& second) const
{
    return second._id == _id;
}

int Position_id::operator()() const
{
    return _id;
}

bool Position_id::operator==(const Position_id& second) const
{
    return _id == second();
}

bool Position_id::operator<(const Position_id& second) const
{
    return _id < second();
}

Position& Position::operator=(Position pos)
{
    credit_lever = pos.credit_lever;
    margin = pos.margin;
    first_security = std::move(pos.first_security);
    second_security = std::move(pos.second_security);
    return *this;
}

Position::Position(Position& pos)
{
    credit_lever = pos.credit_lever;
    margin = pos.margin;
    first_security = std::move(pos.first_security);
    second_security = std::move(pos.second_security);
}