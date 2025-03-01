#include "../REG/DS3234.hpp"
#include "../RTC.tpp"
namespace HAL::RTC
{
    class DS3234 : public Base<class DS3234>
    {
        friend class Base<class DS3234>;
    }
} // namespace HAL::RTC