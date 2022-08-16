#ifndef _FINANCIAL_STRUCTURS_HPP
#define _FINANCIAL_STRUCTURS_HPP

#include <ctime>
#include <string>
#include <memory>
enum Watch {second = 1, minut = 60, hour = 60*minut, day = 24*hour};
enum class Fiat {USD, EUR};

struct Coter
{
    std::time_t date;
    double value;
};

class Security_id
{
    std::string _id;
    public:
    Security_id() = default;
    Security_id(const std::string& id) : _id(id) {};
    virtual bool operator==(const Security_id& second) const;
    virtual std::string operator()() const;
    virtual ~Security_id() = default;
};

class Position_id
{
    int _id;
    public:
    Position_id() = default;
    Position_id(int id) : _id(id) {};
    int operator()() const;
    virtual ~Position_id() = default;
    virtual bool operator==(const Position_id& second) const; 
    virtual bool operator<(const Position_id& second) const;
};

class Wallet
{   
protected:
    double balance;
    double _cost;
public:
    virtual Security_id get_security_id() = 0;
    virtual ~Wallet() = default;
    void buy_security(double value);
    void sell_security(double value);
    double& get_balance();
    double get_balance() const;
    class invalid_balance_value : public std::exception {};
};

class USD_Wallet final : public Wallet
{
public:
    USD_Wallet(double value)
    {
        if (value < 0)
        {
            throw invalid_balance_value();
        }
        balance = value;
    }
    Security_id get_security_id() override;
};

class EURO_Wallet final : public Wallet
{
public:
    EURO_Wallet(double value)
    {
        if (value < 0)
        {
            throw invalid_balance_value();
        }
        balance = value;
    }
    Security_id get_security_id() override;
};

struct Position
{
    int credit_lever;
    double margin;
    Position() = default;
    Position(Position& pos);
    std::shared_ptr<Wallet> first_security;
    std::shared_ptr<Wallet> second_security;
    Position& operator=(Position pos);
};

#endif