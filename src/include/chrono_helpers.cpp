#include "chrono_helpers.hpp"
namespace s21{

std::string getTodayDate() {
    static std::string cached_date;
    static std::chrono::sys_days cached_day;
    
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::floor<std::chrono::days>(now);
    
    if (cached_date.empty() || today != cached_day) {
        cached_day = today;
        std::time_t tt = std::chrono::system_clock::to_time_t(today);
        std::tm* utc_tm = std::gmtime(&tt);  // <-- БЕЗ TZ
        
        char buffer[11];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", utc_tm);
        cached_date = std::string(buffer);
    }
    
    return cached_date;
}
}