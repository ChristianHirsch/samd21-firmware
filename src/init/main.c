/*
 * main.c
 *
 * Created: 29.04.2016 16:04:42
 *  Author: chrivieh
 */ 

//#include "FreeRTOS.h"
//#include "task.h"

//#include <asf.h>
#include <port.h>
#include "boards/zero.h"

extern void system_init(void);

int main(void)
{
    system_init();

    uint32_t speed = 0xffff;
    uint32_t perc = 0;

    while(1)
    {
        uint32_t value = speed * perc / 100;

        LED_Off( LED0_PIN );
        for(uint32_t i=0; i < value; i++)
        {
            __asm__ __volatile__("nop");
        }

        LED_On( LED0_PIN );
        for(uint32_t i=0; i < speed - value; i++)
        {
            __asm__ __volatile__("nop");
        }

        perc++;
        perc %= 101;
    }
	while(1)  /* never reach this point */;

	return 0;
}
