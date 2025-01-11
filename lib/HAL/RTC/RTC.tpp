namespace HAL
{
    namespace RTC
    {
        template <class Driver>
        class Base
        {
            void Bcd2asc(void)
            {
                uint8_t i, j;
                for (j = 0, i = 0; i < 7; i++)
                {
                    asc[j++] =
                        (trdata[i] & 0xf0) >> 4 | 0x30; /*格式为: 秒 分 时 日 月 星期 年 */
                    asc[j++] = (trdata[i] & 0x0f) | 0x30;
                }
            }

            uint8_t Bcd2ToByte(uint8_t Value)
            {
                uint8_t tmp = 0;
                tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
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
        };
    }
}