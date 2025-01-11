#include "../RTC.tpp"
#include "../REG/BM8563.hpp"
namespace HAL
{
    namespace RTC
    {
        class BM8563 : public Base<class BM8563>
        {
            friend class Base<class BM8563>;
        }
    }
}