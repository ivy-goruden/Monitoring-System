#include "chrono_helpers.hpp"
namespace s21{

std::string getTodayDate(){
    auto now = std::chrono::system_clock::now();
    
    // 2. Truncate to days precision (UTC)
    auto today = std::chrono::floor<std::chrono::days>(now);
    
    // 3. Convert to calendar date
    std::chrono::year_month_day today_date{today};
    int year = static_cast<int>(today_date.year());
    unsigned int month = static_cast<unsigned int>(today_date.month());
    unsigned int day = static_cast<unsigned int>(today_date.day());
    return std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
}
}