#include "../REG/DS3231.hpp"
#include "../RTC.tpp"
namespace HAL::RTC
{
    class DS3231 : public Base<class DS3231>
    {
        friend class Base<class DS3231>;
    }
} // namespace HAL::RTC