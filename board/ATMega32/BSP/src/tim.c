#include "iom32.h"
#include "tim.h"

void timer1(void)
{
    TCCR1B = 0X00;
    TCNT1 = 0;
    OCR1B = 800;
    TIMSK |= (1 << OCIE1B) | (1 << TOIE1);
    TCCR1A = (1 << COM1B1) | (1 << WGM10) | (1 << WGM11);
    TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
}
