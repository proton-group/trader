#include "cotersParser.hpp"

namespace
{
    time_t month_to_days(int month)
    {
        const int size = 7;
        int full[size] = {1, 3, 5, 7, 8, 10, 12};
        if(std::find(full, full+size-1, month) != full+size-1)
        {
            return 31;
        }
        if(month == 2)
        {
            return 28;
        }
        return 30;
    }
    time_t month_to_posix(int month)
    {
        time_t seconds = 0;
        for(int i = 1; i < month; i++)
        {
            seconds += month_to_days(i) * Watch::day;
        }
        return seconds;
    }
}
void CotersParser::set_coters_on_exchange(Exchange& market, const Security_id& id)
{
    const int unix_epoch = 1970;
    const time_t year = 365 * Watch::day;
    time_t date;
    double value;
    std::string cotertext;
    while(std::getline(_is, cotertext))
    {
        date = std::stof(cotertext.substr(0, 2)) * Watch::hour + std::stof(cotertext.substr(3, 2)) * Watch::minut + std::stof(cotertext.substr(6, 2));
        date += std::stoi(cotertext.substr(9, 2)) * Watch::day + month_to_posix(std::stoi(cotertext.substr(12, 2))) + (std::stoi(cotertext.substr(15, 4)) - unix_epoch) * year;
        value = std::stof(cotertext.substr(20, 6));
        market.add_indirect_coter(date, id, value);
    }
    return;
}