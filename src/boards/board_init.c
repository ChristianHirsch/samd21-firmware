/**
 * \file
 *
 * \brief SAM D21 Xplained Pro board initialization
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//#include <compiler.h>
#include <board.h>
#include <conf_board.h>
#include <port.h>
#include "zero.h"

// Constants for Clock generators
#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)
#define GENERIC_CLOCK_GENERATOR_XOSC32K   (1u)
#define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u) /*  Initialized at reset for WDT */
#define GENERIC_CLOCK_GENERATOR_OSC8M     (3u)
// Constants for Clock multiplexers
#define GENERIC_CLOCK_MULTIPLEXER_DFLL48M (0u)

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{

	// init system clock

	/* Set 1 Flash Wait State for 48MHz, cf tables 20.9 and 35.27 in SAMD21 Datasheet */
	NVMCTRL->CTRLB.bit.RWS = NVMCTRL_CTRLB_RWS_HALF_Val;

	/* Turn on the digital interface clock */
	PM->APBAMASK.reg |= PM_APBAMASK_GCLK;

	/* ----------------------------------------------------------------------------------------------
	 * 1) Enable XOSC32K clock (External on-board 32.768Hz oscillator)
	 */
	SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_STARTUP( 0x6u ) | /* cf table 15.10 of product datasheet in chapter 15.8.6 */
		SYSCTRL_XOSC32K_XTALEN | SYSCTRL_XOSC32K_EN32K;
    SYSCTRL->XOSC32K.reg |= SYSCTRL_XOSC32K_ENABLE;
//	SYSCTRL->XOSC32K.bit.EN32K = 1; /* separate call, as described in chapter 15.6.3 */
//	SYSCTRL->XOSC32K.bit.ENABLE = 1; /* separate call, as described in chapter 15.6.3 */

//    SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE; /*  separate call, as described in chapter 15.6.3 */

	while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_XOSC32KRDY) == 0 )
	{
		/* Wait for oscillator stabilization */
	}

	/* Software reset the module to ensure it is re-initialized correctly */
	/* Note: Due to synchronization, there is a delay from writing CTRL.SWRST until the reset is complete.
	 * CTRL.SWRST and STATUS.SYNCBUSY will both be cleared when the reset is complete, as described in chapter 13.8.1
	 */
	GCLK->CTRL.reg = GCLK_CTRL_SWRST;

	while ( (GCLK->CTRL.reg & GCLK_CTRL_SWRST) && (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) )
	{
		/* Wait for reset to complete */
	}

	/* ----------------------------------------------------------------------------------------------
	 * 2) Put XOSC32K as source of Generic Clock Generator 1
	 */
	GCLK->GENDIV.reg = GCLK_GENDIV_ID( GENERIC_CLOCK_GENERATOR_XOSC32K ); // Generic Clock Generator 1

	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	{
		/* Wait for synchronization */
	}

	/* Write Generic Clock Generator 1 configuration */
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID( GENERIC_CLOCK_GENERATOR_XOSC32K ) | // Generic Clock Generator 1
		GCLK_GENCTRL_SRC_XOSC32K | // Selected source is External 32KHz Oscillator
		//                      GCLK_GENCTRL_OE | // Output clock to a pin for tests
		GCLK_GENCTRL_GENEN;

	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	{
		/* Wait for synchronization */
	}

	/* ----------------------------------------------------------------------------------------------
	 * 3) Put Generic Clock Generator 1 as source for Generic Clock Multiplexer 0 (DFLL48M reference)
	 */
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GENERIC_CLOCK_MULTIPLEXER_DFLL48M ) | // Generic Clock Multiplexer 0
		GCLK_CLKCTRL_GEN_GCLK1 | // Generic Clock Generator 1 is source
		GCLK_CLKCTRL_CLKEN;

	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	{
		/* Wait for synchronization */
	}

	/* ----------------------------------------------------------------------------------------------
	 * 4) Enable DFLL48M clock
	 */

	/* DFLL Configuration in Closed Loop mode, cf product datasheet chapter 15.6.7.1 - Closed-Loop Operation */

	/* Remove the OnDemand mode, Bug http://avr32.icgroup.norway.atmel.com/bugzilla/show_bug.cgi?id=9905 */
	SYSCTRL->DFLLCTRL.bit.ONDEMAND = 0;

	while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
	{
		/* Wait for synchronization */
	}

	SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP( 31 ) | // Coarse step is 31, half of the max value
		SYSCTRL_DFLLMUL_FSTEP( 511 ) | // Fine step is 511, half of the max value
		SYSCTRL_DFLLMUL_MUL( (VARIANT_MCK/VARIANT_MAINOSC) ); // External 32KHz is the reference

	while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
	{
		/* Wait for synchronization */
	}

	/* Write full configuration to DFLL control register */
	SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_MODE | /* Enable the closed loop mode */
		SYSCTRL_DFLLCTRL_WAITLOCK |
		SYSCTRL_DFLLCTRL_QLDIS; /* Disable Quick lock */

	while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
	{
		/* Wait for synchronization */
	}

	/* Enable the DFLL */
	SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ENABLE;

	while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLLCKC) == 0 ||
			(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLLCKF) == 0 )
	{
		/* Wait for locks flags */
	}

	while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
	{
		/* Wait for synchronization */
	}

	/* ----------------------------------------------------------------------------------------------
	 * 5) Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
	 */
	GCLK->GENDIV.reg = GCLK_GENDIV_ID( GENERIC_CLOCK_GENERATOR_MAIN ); // Generic Clock Generator 0

	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	{
		/* Wait for synchronization */
	}

	/* Write Generic Clock Generator 0 configuration */
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID( GENERIC_CLOCK_GENERATOR_MAIN ) | // Generic Clock Generator 0
		GCLK_GENCTRL_SRC_DFLL48M | // Selected source is DFLL 48MHz
		//                      GCLK_GENCTRL_OE | // Output clock to a pin for tests
		GCLK_GENCTRL_IDC | // Set 50/50 duty cycle
		GCLK_GENCTRL_GENEN;

	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	{
		/* Wait for synchronization */
	}

#if 0
	/* ----------------------------------------------------------------------------------------------
	 * 6) Modify PRESCaler value of OSC8M to have 8MHz
	 */
	SYSCTRL->OSC8M.bit.PRESC = SYSCTRL_OSC8M_PRESC_1_Val;
	SYSCTRL->OSC8M.bit.ONDEMAND = 0;

	/* ----------------------------------------------------------------------------------------------
	 * 7) Put OSC8M as source for Generic Clock Generator 3
	 */
	GCLK->GENDIV.reg = GCLK_GENDIV_ID( GENERIC_CLOCK_GENERATOR_OSC8M ); // Generic Clock Generator 3

	/* Write Generic Clock Generator 3 configuration */
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID( GENERIC_CLOCK_GENERATOR_OSC8M ) | // Generic Clock Generator 3
		GCLK_GENCTRL_SRC_OSC8M | // Selected source is RC OSC 8MHz (already enabled at reset)
		//                      GCLK_GENCTRL_OE | // Output clock to a pin for tests
		GCLK_GENCTRL_GENEN;

	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
	{
		/* Wait for synchronization */
	}
#endif //0

	/*
	 * Now that all system clocks are configured, we can set CPU and APBx BUS clocks.
	 * These values are normally the ones present after Reset.
	 */
	PM->CPUSEL.reg  = PM_CPUSEL_CPUDIV_DIV1;
	PM->APBASEL.reg = PM_APBASEL_APBADIV_DIV1_Val;
	PM->APBBSEL.reg = PM_APBBSEL_APBBDIV_DIV1_Val;
	PM->APBCSEL.reg = PM_APBCSEL_APBCDIV_DIV1_Val;

    /* ------------------------------------------------- */

	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Configure LEDs as outputs, turn them off */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_0_PIN, &pin_conf);
	port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);


	//	/* Set buttons as inputs */
	//	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	//	pin_conf.input_pull = PORT_PIN_PULL_UP;
	//	port_pin_set_config(BUTTON_0_PIN, &pin_conf);
	//	
	//#ifdef CONF_BOARD_AT86RFX
	//	port_get_config_defaults(&pin_conf);
	//	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	//	port_pin_set_config(AT86RFX_SPI_SCK, &pin_conf);
	//	port_pin_set_config(AT86RFX_SPI_MOSI, &pin_conf);
	//	port_pin_set_config(AT86RFX_SPI_CS, &pin_conf);
	//	port_pin_set_config(AT86RFX_RST_PIN, &pin_conf);
	//	port_pin_set_config(AT86RFX_SLP_PIN, &pin_conf);
	//	port_pin_set_output_level(AT86RFX_SPI_SCK, true);
	//	port_pin_set_output_level(AT86RFX_SPI_MOSI, true);
	//	port_pin_set_output_level(AT86RFX_SPI_CS, true);
	//	port_pin_set_output_level(AT86RFX_RST_PIN, true);
	//	port_pin_set_output_level(AT86RFX_SLP_PIN, true);
	//	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	//	port_pin_set_config(AT86RFX_SPI_MISO, &pin_conf);
	//#endif	
}
