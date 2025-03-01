#include "../REG/PCF8563.hpp"
#include "../RTC.tpp"
namespace HAL::RTC
{
    class PCF8563 : public Base<class PCF8563>
    {
        friend class Base<class PCF8563>;
    };
} // namespace HAL::RTC