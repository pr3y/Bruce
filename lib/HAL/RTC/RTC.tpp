namespace HAL::RTC
{
    template <class Driver>
    class Base
    {
        void Bcd2asc(void)
        {
            uint8_t i, j;
            for (j = 0, i = 0; i < 7; i++)
            {
                a   sc[j++] =
                    (trdata[i] & 0xf0) >> 4 | 0x30; /*格式为: 秒 分 时 日 月 星期 年 */
                asc[j++] = (trdata[i] & 0x0f) | 0x30;
            }
        }

        uint8_t Bcd2ToByte(uint8_t Value)
        {
            uint8_t tmp = 0;
            tmp         = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
            return (tmp + (Value & (uint8_t)0x0F));
        }

        uint8_t ByteToBcd2(uint8_t Value)
        {
            uint8_t bcdhigh = 0;

            while (Value >= 10)
            {
                bcdhigh++;
                Value -= 10;
            }

            return ((uint8_t)(bcdhigh << 4) | Value);
        }

        uint8_t BcdToUint8(uint8_t val)
        {
            return val - 6 * (val >> 4);
        }

        uint8_t Uint8ToBcd(uint8_t val)
        {
            return val + 6 * (val / 10);
        }

        uint8_t BcdToBin24Hour(uint8_t bcdHour)
        {
            uint8_t hour;
            if (bcdHour & 0x40)
            {
                // 12 hour mode, convert to 24
                bool isPm = ((bcdHour & 0x20) != 0);

                hour = BcdToUint8(bcdHour & 0x1f);
                if (isPm)
                {
                    hour += 12;
                }
            }
            else
            {
                hour = BcdToUint8(bcdHour);
            }
            return hour;
        }
    };
} // namespace HAL::RTC