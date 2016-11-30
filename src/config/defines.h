/*
 * defines.h
 *
 * Created: 10.05.2016 13:02:45
 *  Author: chrivieh
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

#define LED1_bm			PIN4_bm		//PORTA
#define LED2_bm			PIN3_bm		//PORTA
#define SW_bm			PIN7_bm		//PORTA
#define LEDPORT			PORTA
#define SWPORT			PORTA
#define VPORT			PORTA

#define V_CHG_bm		PIN5_bm		//PORTA
#define V_PGOOD_bm		PIN6_bm		//PORTA

#define RF_CS_bm		PIN1_bm		//PORTB
#define RF_RST_bm		PIN3_bm		//PORTD
#define RF_INT_bm		PIN4_bm		//PORTD
#define RF_INT_vect		PORTD_INT0_vect

#define IMU_CS_G_bm		PIN2_bm		//PORTC
#define IMU_CS_XM_bm	PIN3_bm		//PORTC
#define IMU_INT_G_bm	PIN2_bm		//PORTB
#define IMU_DRDY_G_bm	PIN3_bm		//PORTB
#define IMU_INT1_XM_bm	PIN0_bm		//PORTC
#define IMU_INT2_XM_bm	PIN1_bm		//PORTC
#define IMU_INT_G_PORT	PORTB
#define IMU_INT_XM_PORT	PORTC

#define BAR_CS_bm		PIN4_bm		//PORTC
#define BAR_INT_bm		PIN2_bm		//PORTD
#define BAR_INT_PORT	PORTD

#define MOT1_bm			PIN2_bm		//PORTE
#define MOT2_bm			PIN1_bm		//PORTE
#define MOT3_bm			PIN0_bm		//PORTE
#define MOT4_bm			PIN3_bm		//PORTE

#endif /* DEFINES_H_ */