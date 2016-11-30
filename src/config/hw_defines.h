/*
 * hw_defines.h
 *
 * Created: 10.05.2016 13:01:23
 *  Author: chrivieh
 */ 


#ifndef HW_DEFINES_H_
#define HW_DEFINES_H_

#include "defines.h"

#define Select_IMU_XM() (PORTC.OUTCLR = IMU_CS_XM_bm)
#define Deselect_IMU_XM() (PORTC.OUTSET = IMU_CS_XM_bm)

#define Select_IMU_G() (PORTC.OUTCLR = IMU_CS_G_bm)
#define Deselect_IMU_G() (PORTC.OUTSET = IMU_CS_G_bm)

#endif /* HW_DEFINES_H_ */