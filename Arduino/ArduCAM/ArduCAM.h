/*
  ArduCAM.h - Arduino library support for CMOS Image Sensor
  Copyright (C)2011-2015 ArduCAM.com. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ArduCAM.com. You can find the latest version of the library at
  http://www.ArduCAM.com

  Now supported controllers:
		- OV7670
		- MT9D111
		- OV7675
		- OV2640
		- OV3640
		- OV5642
		- OV5640
		- OV7660
		- OV7725
		- MT9M112		
		- MT9V111
		- OV5640		
		- MT9M001			
		- MT9T112
		- MT9D112
				
	We will add support for many other sensors in next release.
	
  Supported MCU platform
 		-	Theoretically support all Arduino families
  		-	Arduino UNO R3			(Tested)
  		-	Arduino MEGA2560 R3		(Tested)
  		-	Arduino Leonardo R3		(Tested)
  		-	Arduino Nano			(Tested)
  		-	Arduino DUE				(Tested)
  		- Arduino Yun				(Tested)  		
  		-	Raspberry Pi			(Tested)
  		- ESP8266-12				(Tested)  		
		* Feather M0                (Tested with OV5642)

  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.ArduCAM.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*------------------------------------
	Revision History:
	2012/09/20 	V1.0.0	by Lee	first release 
	2012/10/23  V1.0.1  by Lee  Resolved some timing issue for the Read/Write Register	
	2012/11/29	V1.1.0	by Lee  Add support for MT9D111 sensor
	2012/12/13	V1.2.0	by Lee	Add support for OV7675 sensor
	2012/12/28  V1.3.0	by Lee	Add support for OV2640,OV3640,OV5642 sensors
	2013/02/26	V2.0.0	by Lee	New Rev.B shield hardware, add support for FIFO control 
															and support Mega1280/2560 boards 
	2013/05/28	V2.1.0	by Lee	Add support all drawing functions derived from UTFT library 			
	2013/08/24	V3.0.0	by Lee	Support ArudCAM shield Rev.C hardware, features SPI interface and low power mode.
								Support almost all series of Arduino boards including DUE.	
	2014/03/09  V3.1.0  by Lee  Add the more impressive example sketches. 
								Optimise the OV5642 settings, improve image quality.
								Add live preview before JPEG capture.
								Add play back photos one by one	after BMP capture.
	2014/05/01  V3.1.1  by Lee  Minor changes to add support Arduino IDE for linux distributions.	
	2014/09/30  V3.2.0  by Lee  Improvement on OV5642 camera dirver.			
	2014/10/06  V3.3.0  by Lee  Add OV7660,OV7725 camera support.			
	2015/02/27  V3.4.0  by Lee  Add the support for Arduino Yun board, update the latest UTFT library for ArduCAM.			
	2015/06/09  V3.4.1  by Lee	Minor changes and add some comments		
	2015/06/19  V3.4.2  by Lee	Add support for MT9M112 camera.			
	2015/06/20  V3.4.3  by Lee	Add support for MT9V111 camera.			
	2015/06/22  V3.4.4  by Lee	Add support for OV5640 camera.										
	2015/06/22  V3.4.5  by Lee	Add support for MT9M001 camera.		
	2015/08/05  V3.4.6  by Lee	Add support for MT9T112 camera.	
	2015/08/08  V3.4.7  by Lee	Add support for MT9D112 camera.							
	2015/09/20  V3.4.8  by Lee	Add support for ESP8266 processor.	
	2016/02/03	V3.4.9	by Lee	Add support for Arduino ZERO board.
	2016/06/07  V3.5.0  by Lee	Add support for OV5642_CAM_BIT_ROTATION_FIXED.
	2016/06/14  V3.5.1  by Lee	Add support for ArduCAM-Mini-5MP-Plus OV5640_CAM.	
	2016/09/29	V3.5.2	by Lee	Optimize the OV5642 register settings		
	2016/10/05	V4.0.0	by Lee	Add support for second generation of ArduCAM shield V2, ArduCAM-Mini-5MP-Plus(OV5642/OV5640).				
	2016/10/28  V4.0.1  by Lee	Add support for Raspberry Pi
	2017/04/27  V4.1.0  by Lee	Add support for OV2640/OV5640/OV5642 functions.
	2017/07/07  V4.1.0  by Lee	Add support for ArduCAM_ESP32 paltform
	2017/07/25  V4.1.1  by Lee	Add support for MT9V034
	2017/11/27  V4.1.2  by Max      Add support for Feather M0
	2018/10/15  V4.1.2  by Lee      Add support for NRF52
	2018/10/15  V4.1.2  by Lee      Add support for TEENSYDUINO
--------------------------------------*/

#ifndef ArduCAM_H
#define ArduCAM_H

#include "memorysaver.h"
#include <pins_arduino.h>
#include <Wire.h>
#include "Arduino.h"
#include "ArduCAM.h"
#include <SPI.h>


#if defined (__AVR__)
#define cbi(reg, bitmask) *reg &= ~bitmask
#define sbi(reg, bitmask) *reg |= bitmask
#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);
#define cport(port, data) port &= data
#define sport(port, data) port |= data
#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) pgm_read_byte(&cfont.font[x])  
#define regtype volatile uint8_t
#define regsize uint8_t
#endif

#if defined(__SAM3X8E__)

#define cbi(reg, bitmask) *reg &= ~bitmask
#define sbi(reg, bitmask) *reg |= bitmask

#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);

#define cport(port, data) port &= data
#define sport(port, data) port |= data

#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) cfont.font[x]  

#define regtype volatile uint32_t
#define regsize uint32_t

#define PROGMEM

#define pgm_read_byte(x)        (*((char *)x))
#define pgm_read_word(x)        ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
#define pgm_read_byte_near(x)   (*((char *)x))
#define pgm_read_byte_far(x)    (*((char *)x))
#define pgm_read_word_near(x)   ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x)))
#define pgm_read_word_far(x)    ( ((*((unsigned char *)x + 1)) << 8) + (*((unsigned char *)x))))
#define PSTR(x)  x
#if defined F
	#undef F
#endif
#define F(X) (X)	
#endif	

#if defined(ESP8266)
	#define cbi(reg, bitmask) digitalWrite(bitmask, LOW)
	#define sbi(reg, bitmask) digitalWrite(bitmask, HIGH)
	#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
	#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);
	
	#define cport(port, data) port &= data
	#define sport(port, data) port |= data
	
	#define swap(type, i, j) {type t = i; i = j; j = t;}
	
	#define fontbyte(x) cfont.font[x]  
	
	#define regtype volatile uint32_t
	#define regsize uint32_t
#endif	

#if defined(__SAMD51__) || defined(__SAMD21G18A__)
	#define Serial SERIAL_PORT_USBVIRTUAL

	#define cbi(reg, bitmask) *reg &= ~bitmask
	#define sbi(reg, bitmask) *reg |= bitmask

	#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
	#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);

	#define cport(port, data) port &= data
	#define sport(port, data) port |= data

	#define swap(type, i, j) {type t = i; i = j; j = t;}
	#define fontbyte(x) cfont.font[x]  

	#define regtype volatile uint32_t
	#define regsize uint32_t
#endif

#if defined(ESP32)
	#define cbi(reg, bitmask) digitalWrite(bitmask, LOW)
	#define sbi(reg, bitmask) digitalWrite(bitmask, HIGH)
	#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
	#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);
	
	#define cport(port, data) port &= data
	#define sport(port, data) port |= data
	
	#define swap(type, i, j) {type t = i; i = j; j = t;}
	
	#define fontbyte(x) cfont.font[x]  
	
	#define regtype volatile uint32_t
	#define regsize uint32_t
#endif

#if defined(__CPU_ARC__)
	#define cbi(reg, bitmask) *reg &= ~bitmask
	#define sbi(reg, bitmask) *reg |= bitmask
	#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
	#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);
	#define cport(port, data) port &= data
	#define sport(port, data) port |= data
	#define swap(type, i, j) {type t = i; i = j; j = t;}
	#define fontbyte(x) pgm_read_byte(&cfont.font[x])  
	#define regtype volatile uint32_t
	#define regsize uint32_t
#endif

#if defined (RASPBERRY_PI)
	#define regtype volatile uint32_t
	#define regsize uint32_t 
	#define byte uint8_t
	#define cbi(reg, bitmask) digitalWrite(bitmask, LOW)
  #define sbi(reg, bitmask) digitalWrite(bitmask, HIGH)
  #define PROGMEM
	
	#define PSTR(x)  x
	#if defined F
	#undef F
	#endif
	#define F(X) (X)
#endif

#if defined(ARDUINO_ARCH_NRF52)
    #define cbi(reg, bitmask) digitalWrite(bitmask, LOW)
	#define sbi(reg, bitmask) digitalWrite(bitmask, HIGH)
	#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
	#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);
	
	#define cport(port, data) port &= data
	#define sport(port, data) port |= data
	
	#define swap(type, i, j) {type t = i; i = j; j = t;}
	
	#define fontbyte(x) cfont.font[x]  
	
	#define regtype volatile uint32_t
	#define regsize uint32_t

#endif

#if defined(TEENSYDUINO)
 #define cbi(reg, bitmask) digitalWriteFast(bitmask, LOW)
 #define sbi(reg, bitmask) digitalWriteFast(bitmask, HIGH)
#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);
 #define cport(port, data) port &= data
#define sport(port, data) port |= data
 #define swap(type, i, j) {type t = i; i = j; j = t;}
 #define fontbyte(x) cfont.font[x]  
 #define regtype volatile uint8_t
#define regsize uint8_t
 #endif

#if defined(NRF52840_XXAA)

 #define cbi(reg, bitmask) digitalWrite(bitmask, LOW)
 #define sbi(reg, bitmask) digitalWrite(bitmask, HIGH)

#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);

#define cport(port, data) port &= data
#define sport(port, data) port |= data

#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) cfont.font[x]  

#define regtype volatile uint32_t
#define regsize uint32_t

#define PROGMEM

#if defined F
	#undef F
#endif
#define F(X) (X)
#endif


/****************************************************/
/* Sensor related definition 												*/
/****************************************************/
#define BMP 	0
#define JPEG	1
#define RAW	  2

#define OV7670		0	
#define MT9D111_A	1
#define OV7675		2
#define OV5642		3
#define OV3640  	4
#define OV2640  	5
#define OV9655		6
#define MT9M112		7
#define OV7725		8
#define OV7660		9
#define MT9M001 	10
#define OV5640 		11
#define MT9D111_B	12
#define OV9650		13
#define MT9V111		14
#define MT9T112		15
#define MT9D112		16
#define MT9V034 	17
#define MT9M034   18

#define OV2640_160x120 		0	//160x120
#define OV2640_176x144 		1	//176x144
#define OV2640_320x240 		2	//320x240
#define OV2640_352x288 		3	//352x288
#define OV2640_640x480		4	//640x480
#define OV2640_800x600 		5	//800x600
#define OV2640_1024x768		6	//1024x768
#define OV2640_1280x1024	7	//1280x1024
#define OV2640_1600x1200	8	//1600x1200



#define OV3640_176x144 		0	//176x144
#define OV3640_320x240 		1	//320x240
#define OV3640_352x288 		2	//352x288
#define OV3640_640x480		3	//640x480
#define OV3640_800x600 		4	//800x600
#define OV3640_1024x768		5 //1024x768
#define OV3640_1280x960	  	6	//1280x960
#define OV3640_1600x1200	7	//1600x1200
#define OV3640_2048x1536	8	//2048x1536


#define OV5642_320x240 		0	//320x240
#define OV5642_640x480		1	//640x480
#define OV5642_1024x768		2	//1024x768
#define OV5642_1280x960 	3	//1280x960
#define OV5642_1600x1200	4	//1600x1200
#define OV5642_2048x1536	5	//2048x1536
#define OV5642_2592x1944	6	//2592x1944
#define OV5642_1920x1080  	7


#define OV5640_320x240 		0	//320x240 
#define OV5640_352x288		1	//352x288
#define OV5640_640x480 	  	2	//640x480
#define OV5640_800x480	  	3	//800x480
#define OV5640_1024x768	  	4	//1024x768
#define OV5640_1280x960	  	5	//1280x960	
#define OV5640_1600x1200	6	 //1600x1200
#define OV5640_2048x1536	7  //2048x1536
#define OV5640_2592x1944	8	 //2592x1944



//Light Mode

#define Auto                 0
#define Sunny                1
#define Cloudy               2
#define Office               3
#define Home                 4

#define Advanced_AWB         0
#define Simple_AWB           1
#define Manual_day           2
#define Manual_A             3
#define Manual_cwf           4
#define Manual_cloudy        5



//Color Saturation 

#define Saturation4          0
#define Saturation3          1
#define Saturation2          2
#define Saturation1          3
#define Saturation0          4
#define Saturation_1         5
#define Saturation_2         6
#define Saturation_3         7
#define Saturation_4         8

//Brightness

#define Brightness4          0
#define Brightness3          1
#define Brightness2          2
#define Brightness1          3
#define Brightness0          4
#define Brightness_1         5
#define Brightness_2         6
#define Brightness_3         7
#define Brightness_4         8


//Contrast

#define Contrast4            0
#define Contrast3            1
#define Contrast2            2
#define Contrast1            3
#define Contrast0            4
#define Contrast_1           5
#define Contrast_2           6
#define Contrast_3           7
#define Contrast_4           8



#define degree_180            0
#define degree_150            1
#define degree_120            2
#define degree_90             3
#define degree_60             4
#define degree_30             5
#define degree_0              6
#define degree30              7
#define degree60              8
#define degree90              9
#define degree120             10
#define degree150             11



//Special effects

#define Antique                      0
#define Bluish                       1
#define Greenish                     2
#define Reddish                      3
#define BW                           4
#define Negative                     5
#define BWnegative                   6
#define Normal                       7
#define Sepia                        8
#define Overexposure                 9
#define Solarize                     10
#define  Blueish                     11
#define Yellowish                    12

#define Exposure_17_EV                    0
#define Exposure_13_EV                    1
#define Exposure_10_EV                    2
#define Exposure_07_EV                    3
#define Exposure_03_EV                    4
#define Exposure_default                  5
#define Exposure03_EV                     6
#define Exposure07_EV                     7
#define Exposure10_EV                     8
#define Exposure13_EV                     9
#define Exposure17_EV                     10

#define Auto_Sharpness_default              0
#define Auto_Sharpness1                     1
#define Auto_Sharpness2                     2
#define Manual_Sharpnessoff                 3
#define Manual_Sharpness1                   4
#define Manual_Sharpness2                   5
#define Manual_Sharpness3                   6
#define Manual_Sharpness4                   7
#define Manual_Sharpness5                   8



#define Sharpness1                         0
#define Sharpness2                         1
#define Sharpness3                         2
#define Sharpness4                         3
#define Sharpness5                         4
#define Sharpness6                         5
#define Sharpness7                         6
#define Sharpness8                         7
#define Sharpness_auto                       8




#define EV3                                 0
#define EV2                                 1
#define EV1                                 2
#define EV0                                 3
#define EV_1                                4
#define EV_2                                5
#define EV_3                                6

#define MIRROR                              0
#define FLIP                                1
#define MIRROR_FLIP                         2




#define high_quality                         0
#define default_quality                      1
#define low_quality                          2

#define Color_bar                      0
#define Color_square                   1
#define BW_square                      2
#define DLI                            3


#define Night_Mode_On                  0
#define Night_Mode_Off                 1

#define Off                            0
#define Manual_50HZ                    1
#define Manual_60HZ                    2
#define Auto_Detection                 3

/****************************************************/
/* I2C Control Definition 													*/
/****************************************************/
#define I2C_ADDR_8BIT 0
#define I2C_ADDR_16BIT 1
#define I2C_REG_8BIT 0
#define I2C_REG_16BIT 1
#define I2C_DAT_8BIT 0
#define I2C_DAT_16BIT 1

/* Register initialization tables for SENSORs */
/* Terminating list entry for reg */
#define SENSOR_REG_TERM_8BIT                0xFF
#define SENSOR_REG_TERM_16BIT               0xFFFF
/* Terminating list entry for val */
#define SENSOR_VAL_TERM_8BIT                0xFF
#define SENSOR_VAL_TERM_16BIT               0xFFFF

//Define maximum frame buffer size
#if (defined OV2640_MINI_2MP)
#define MAX_FIFO_SIZE		0x5FFFF			//384KByte
#elif (defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined ARDUCAM_SHIELD_REVC)
#define MAX_FIFO_SIZE		0x7FFFF			//512KByte
#else
#define MAX_FIFO_SIZE		0x7FFFFF		//8MByte
#endif 

/****************************************************/
/* ArduChip registers definition 											*/
/****************************************************/
#define RWBIT									0x80  //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1       	0x00  //TEST register

#if !(defined OV2640_MINI_2MP)
	#define ARDUCHIP_FRAMES			  0x01  //FRAME control register, Bit[2:0] = Number of frames to be captured																		//On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full
#endif

#define ARDUCHIP_MODE      		0x02  //Mode register
#define MCU2LCD_MODE       		0x00
#define CAM2LCD_MODE       		0x01
#define LCD2MCU_MODE       		0x02

#define ARDUCHIP_TIM       		0x03  //Timming control
#if !(defined OV2640_MINI_2MP)
	#define HREF_LEVEL_MASK    		0x01  //0 = High active , 		1 = Low active
	#define VSYNC_LEVEL_MASK   		0x02  //0 = High active , 		1 = Low active
	#define LCD_BKEN_MASK      		0x04  //0 = Enable, 					1 = Disable
	#if (defined ARDUCAM_SHIELD_V2)
		#define PCLK_REVERSE_MASK  	0x08  //0 = Normal PCLK, 		1 = REVERSED PCLK
	#else
		#define PCLK_DELAY_MASK  		0x08  //0 = data no delay,		1 = data delayed one PCLK
	#endif
	//#define MODE_MASK          		0x10  //0 = LCD mode, 				1 = FIFO mode
#endif
//#define FIFO_PWRDN_MASK	   		0x20  	//0 = Normal operation, 1 = FIFO power down
//#define LOW_POWER_MODE			  0x40  	//0 = Normal mode, 			1 = Low power mode

#define ARDUCHIP_FIFO      		0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK    		0x01
#define FIFO_START_MASK    		0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20

#define ARDUCHIP_GPIO			  0x06  //GPIO Write Register
#if !(defined (ARDUCAM_SHIELD_V2) || defined (ARDUCAM_SHIELD_REVC))
#define GPIO_RESET_MASK			0x01  //0 = Sensor reset,							1 =  Sensor normal operation
#define GPIO_PWDN_MASK			0x02  //0 = Sensor normal operation, 	1 = Sensor standby
#define GPIO_PWREN_MASK			0x04	//0 = Sensor LDO disable, 			1 = sensor LDO enable
#endif

#define BURST_FIFO_READ			0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ		0x3D  //Single FIFO read operation

#define ARDUCHIP_REV       		0x40  //ArduCHIP revision
#define VER_LOW_MASK       		0x3F
#define VER_HIGH_MASK      		0xC0

#define ARDUCHIP_TRIG      		0x41  //Trigger source
#define VSYNC_MASK         		0x01
#define SHUTTER_MASK       		0x02
#define CAP_DONE_MASK      		0x08

#define FIFO_SIZE1				0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3				0x44  //Camera write FIFO size[18:16]


/****************************************************/


/****************************************************************/
/* define a structure for sensor register initialization values */
/****************************************************************/
struct sensor_reg {
	uint16_t reg;
	uint16_t val;
};

#if ( defined(OV5642_CAM) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP_PLUS) )	
#include "ov5642_regs.h"
#endif


/****************************************************************/
/* define a structure for sensor register initialization values */
/****************************************************************/

class ArduCAM 
{
	public:
	ArduCAM(byte model ,int CS)
	{
#if defined (RASPBERRY_PI)
		if (CS >= 0)
		{
			B_CS = CS;
		}
#else
#if (defined(ESP8266)||defined(ESP32)||defined(TEENSYDUINO) ||defined(NRF52840_XXAA))
		B_CS = CS;
#else
		P_CS = portOutputRegister(digitalPinToPort(CS));
		B_CS = digitalPinToBitMask(CS);
#endif
#endif
#if defined (RASPBERRY_PI)
		// pinMode(CS, OUTPUT);
#else
		pinMode(CS, OUTPUT);
		sbi(P_CS, B_CS);
#endif
		sensor_model = model;
		switch (sensor_model)
		{
		case OV7660:
		case OV7670:
		case OV7675:
		case OV7725:
#if defined (RASPBERRY_PI)
			sensor_addr = 0x21;
#else
			sensor_addr = 0x42;
#endif		
			break;
		case MT9D111_A: //Standard MT9D111 module
			sensor_addr = 0xba;
			break;
		case MT9D111_B: //Flex MT9D111 AF module
			sensor_addr = 0x90;
			break;
		case MT9M112:
#if defined (RASPBERRY_PI)
			sensor_addr = 0x5d;
#else
			sensor_addr = 0x90;
#endif
			break;
		case MT9M001:
			sensor_addr = 0xba;
			break;
		case MT9V034:
			sensor_addr = 0x90;
			break;
		case MT9M034:
			sensor_addr = 0x20;// 7 bits
			break;
		case OV3640:
		case OV5640:
		case OV5642:
		case MT9T112:
		case MT9D112:
#if defined (RASPBERRY_PI)
			sensor_addr = 0x3c;
#else
			sensor_addr = 0x78;
#endif
			break;
		case OV2640:
		case OV9650:
		case OV9655:
#if defined (RASPBERRY_PI)
			sensor_addr = 0x30;
#else
			sensor_addr = 0x60;
#endif
			break;
		default:
#if defined (RASPBERRY_PI)
			sensor_addr = 0x21;
#else
			sensor_addr = 0x42;
#endif
			break;
		}
#if defined (RASPBERRY_PI)
		// initialize i2c:
		if (!arducam_i2c_init(sensor_addr)) {
			printf("ERROR: I2C init failed\n");
		}
#endif
	}
	void InitCAM(void)
	{

		switch (sensor_model)
		{
		case OV5642:
		{
#if ( defined(OV5642_CAM) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || defined(OV5642_MINI_5MP_PLUS) )
			wrSensorReg16_8(0x3008, 0x80);
			if (m_fmt == RAW) {
				//Init and set 640x480;
				wrSensorRegs16_8(OV5642_1280x960_RAW);
				wrSensorRegs16_8(OV5642_640x480_RAW);
			}
			else {
				wrSensorRegs16_8(OV5642_QVGA_Preview);
#if defined (RASPBERRY_PI) 
				arducam_delay_ms(100);
#else
				delay(100);
#endif
				if (m_fmt == JPEG)
				{
#if defined (RASPBERRY_PI) 
					arducam_delay_ms(100);
#else
					delay(100);
#endif
					wrSensorRegs16_8(OV5642_JPEG_Capture_QSXGA);
					wrSensorRegs16_8(ov5642_320x240);
#if defined (RASPBERRY_PI) 
					arducam_delay_ms(100);
#else
					delay(100);
#endif
					wrSensorReg16_8(0x3818, 0xa8);
					wrSensorReg16_8(0x3621, 0x10);
					wrSensorReg16_8(0x3801, 0xb0);
#if (defined(OV5642_MINI_5MP_PLUS) || (defined ARDUCAM_SHIELD_V2))
					wrSensorReg16_8(0x4407, 0x04);
#else
					wrSensorReg16_8(0x4407, 0x0C);
#endif
					wrSensorReg16_8(0x5888, 0x00);
				}
				else
				{

					byte reg_val;
					wrSensorReg16_8(0x4740, 0x21);
					wrSensorReg16_8(0x501e, 0x2a);
					wrSensorReg16_8(0x5002, 0xf8);
					wrSensorReg16_8(0x501f, 0x01);
					wrSensorReg16_8(0x4300, 0x61);
					rdSensorReg16_8(0x3818, &reg_val);
					wrSensorReg16_8(0x3818, (reg_val | 0x60) & 0xff);
					rdSensorReg16_8(0x3621, &reg_val);
					wrSensorReg16_8(0x3621, reg_val & 0xdf);
				}
			}

#endif
			break;
		}
		}
	}
	
	void CS_HIGH(void)
	{
		sbi(P_CS, B_CS);
	}
	void CS_LOW(void)
	{
		cbi(P_CS, B_CS);
	}
	
	void flush_fifo(void)
	{
		write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
	}
	void start_capture(void)
	{
		write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
	}
	void clear_fifo_flag(void)
	{
		write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
	}
	uint8_t read_fifo(void) {
		uint8_t data;
		data = bus_read(SINGLE_FIFO_READ);
		return data;
	}
	
	uint8_t read_reg(uint8_t addr)
	{
		uint8_t data;
#if defined (RASPBERRY_PI)
		data = bus_read(addr);
#else
		data = bus_read(addr & 0x7F);
#endif
		return data;
	}
	void write_reg(uint8_t addr, uint8_t data)
	{
#if defined (RASPBERRY_PI)
		bus_write(addr, data);
#else
		bus_write(addr | 0x80, data);
#endif  
	}
	
	uint32_t read_fifo_length(void)
	{
		uint32_t len1, len2, len3, length = 0;
		len1 = read_reg(FIFO_SIZE1);
		len2 = read_reg(FIFO_SIZE2);
		len3 = read_reg(FIFO_SIZE3) & 0x7f;
		length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
		return length;
	}
	void set_fifo_burst(void)
	{
#if defined (RASPBERRY_PI)
		transfer(BURST_FIFO_READ);
#else
		SPI.transfer(BURST_FIFO_READ);
#endif

	}
	
	void set_bit(uint8_t addr, uint8_t bit) {
		uint8_t temp;
		temp = read_reg(addr);
		write_reg(addr, temp | bit);
	}
	void clear_bit(uint8_t addr, uint8_t bit) {
		uint8_t temp;
		temp = read_reg(addr);
		write_reg(addr, temp & (~bit));
	}
	uint8_t get_bit(uint8_t addr, uint8_t bit) {
		uint8_t temp;
		temp = read_reg(addr);
		temp = temp & bit;
		return temp;
	}
	void set_mode(uint8_t mode)
	{
		switch (mode)
		{
		case MCU2LCD_MODE:
			write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
			break;
		case CAM2LCD_MODE:
			write_reg(ARDUCHIP_MODE, CAM2LCD_MODE);
			break;
		case LCD2MCU_MODE:
			write_reg(ARDUCHIP_MODE, LCD2MCU_MODE);
			break;
		default:
			write_reg(ARDUCHIP_MODE, MCU2LCD_MODE);
			break;
		}
	}

	uint8_t bus_write(int address, int value)
  {
	  cbi(P_CS, B_CS);
#if defined (RASPBERRY_PI)
	  arducam_spi_write(address | 0x80, value);
#else
	  SPI.transfer(address);
	  SPI.transfer(value);
#endif
	  sbi(P_CS, B_CS);
	  return 1;
  }
	uint8_t bus_read(int address)
	{
		uint8_t value;
		cbi(P_CS, B_CS);
#if defined (RASPBERRY_PI)
		value = arducam_spi_read(address & 0x7F);
		sbi(P_CS, B_CS);
		return value;
#else
#if (defined(ESP8266) || defined(__arm__) ||defined(TEENSYDUINO))
#if defined(OV5642_MINI_5MP)
		SPI.transfer(address);
		value = SPI.transfer(0x00);
		// correction for bit rotation from readback
		value = (byte)(value >> 1) | (value << 7);
		// take the SS pin high to de-select the chip:
		sbi(P_CS, B_CS);
		return value;
#else
		SPI.transfer(address);
		value = SPI.transfer(0x00);
		// take the SS pin high to de-select the chip:
		sbi(P_CS, B_CS);
		return value;
#endif
#else
		SPI.transfer(address);
		value = SPI.transfer(0x00);
		// take the SS pin high to de-select the chip:
		sbi(P_CS, B_CS);
		return value;
#endif
#endif
	}
  
	// Write 8 bit values to 8 bit register address
	int wrSensorRegs8_8(const struct sensor_reg* reglist)
	{
#if defined (RASPBERRY_PI)
		arducam_i2c_write_regs(reglist);
#else
		int err = 0;
		uint16_t reg_addr = 0;
		uint16_t reg_val = 0;
		const struct sensor_reg* next = reglist;
		while ((reg_addr != 0xff) | (reg_val != 0xff))
		{
			reg_addr = pgm_read_word(&next->reg);
			reg_val = pgm_read_word(&next->val);
			err = wrSensorReg8_8(reg_addr, reg_val);
			next++;
#if (defined(ESP8266)||defined(ESP32)||defined(TEENSYDUINO))
			yield();
#endif
		}
#endif  
		return 1;
	}
	
	// Write 16 bit values to 8 bit register address
	int wrSensorRegs8_16(const struct sensor_reg* reglist)
	{
#if defined (RASPBERRY_PI)
		arducam_i2c_write_regs16(reglist);
#else
		int err = 0;
		unsigned int reg_addr, reg_val;
		const struct sensor_reg* next = reglist;

		while ((reg_addr != 0xff) | (reg_val != 0xffff))
		{
#if defined (RASPBERRY_PI)
			reg_addr = next->reg;
			reg_val = next->val;
#else
			reg_addr = pgm_read_word(&next->reg);
			reg_val = pgm_read_word(&next->val);
#endif
			err = wrSensorReg8_16(reg_addr, reg_val);
			//  if (!err)
			//return err;
			next++;
#if (defined(ESP8266)||defined(ESP32)||defined(TEENSYDUINO))
			yield();
#endif
		}
#endif
		return 1;
	}
	
	// Write 8 bit values to 16 bit register address
	int wrSensorRegs16_8(const struct sensor_reg* reglist)
	{
#if defined (RASPBERRY_PI)
		arducam_i2c_write_word_regs(reglist);
#else
		int err = 0;
		unsigned int reg_addr;
		unsigned char reg_val;
		const struct sensor_reg* next = reglist;

		while ((reg_addr != 0xffff) | (reg_val != 0xff))
		{

#if defined (RASPBERRY_PI)
			reg_addr = next->reg;
			reg_val = next->val;
#else
			reg_addr = pgm_read_word(&next->reg);
			reg_val = pgm_read_word(&next->val);
#endif
			err = wrSensorReg16_8(reg_addr, reg_val);
			//if (!err)
			//return err;
			next++;
#if (defined(ESP8266)||defined(ESP32)||defined(TEENSYDUINO))
			yield();
#endif
		}
#endif
		return 1;
	}
	
  // Write 16 bit values to 16 bit register address
	int wrSensorRegs16_16(const struct sensor_reg* reglist)
	{
#if defined (RASPBERRY_PI)
#else
		int err = 0;
		unsigned int reg_addr, reg_val;
		const struct sensor_reg* next = reglist;
		reg_addr = pgm_read_word(&next->reg);
		reg_val = pgm_read_word(&next->val);
		while ((reg_addr != 0xffff) | (reg_val != 0xffff))
		{
			err = wrSensorReg16_16(reg_addr, reg_val);
			//if (!err)
			//   return err;
			next++;
			reg_addr = pgm_read_word(&next->reg);
			reg_val = pgm_read_word(&next->val);
#if (defined(ESP8266)||defined(ESP32)||defined(TEENSYDUINO))
			yield();
#endif
		}
#endif
		return 1;
	}
	

	// Read/write 8 bit value to/from 8 bit register address	
	byte wrSensorReg8_8(int regID, int regDat)
	{
#if defined (RASPBERRY_PI)
		arducam_i2c_write(regID, regDat);
#else
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID & 0x00FF);
		Wire.write(regDat & 0x00FF);
		if (Wire.endTransmission())
		{
			return 0;
		}
		delay(1);
#endif
		return 1;

	}
	byte rdSensorReg8_8(uint8_t regID, uint8_t* regDat)
	{
#if defined (RASPBERRY_PI) 
		arducam_i2c_read(regID, regDat);
#else
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID & 0x00FF);
		Wire.endTransmission();

		Wire.requestFrom((sensor_addr >> 1), 1);
		if (Wire.available())
			* regDat = Wire.read();
		delay(1);
#endif
		return 1;

	}
	
	// Read/write 16 bit value to/from 8 bit register address
	byte wrSensorReg8_16(int regID, int regDat)
	{
#if defined (RASPBERRY_PI) 
		arducam_i2c_write16(regID, regDat);
#else
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID & 0x00FF);

		Wire.write(regDat >> 8);            // sends data byte, MSB first
		Wire.write(regDat & 0x00FF);
		if (Wire.endTransmission())
		{
			return 0;
		}
		delay(1);
#endif
		return 1;
	}
	byte rdSensorReg8_16(uint8_t regID, uint16_t* regDat)
	{
#if defined (RASPBERRY_PI) 
		arducam_i2c_read16(regID, regDat);
#else
		uint8_t temp;
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID);
		Wire.endTransmission();

		Wire.requestFrom((sensor_addr >> 1), 2);
		if (Wire.available())
		{
			temp = Wire.read();
			*regDat = (temp << 8) | Wire.read();
		}
		delay(1);
#endif
		return 1;
	}
	
	// Read/write 8 bit value to/from 16 bit register address
	byte wrSensorReg16_8(int regID, int regDat)
	{
#if defined (RASPBERRY_PI) 
		arducam_i2c_word_write(regID, regDat);
		arducam_delay_ms(1);
#else
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID >> 8);            // sends instruction byte, MSB first
		Wire.write(regID & 0x00FF);
		Wire.write(regDat & 0x00FF);
		if (Wire.endTransmission())
		{
			return 0;
		}
		delay(1);
#endif
		return 1;
	}
	byte rdSensorReg16_8(uint16_t regID, uint8_t* regDat)
	{
#if defined (RASPBERRY_PI) 
		arducam_i2c_word_read(regID, regDat);
#else
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID >> 8);
		Wire.write(regID & 0x00FF);
		Wire.endTransmission();
		Wire.requestFrom((sensor_addr >> 1), 1);
		if (Wire.available())
		{
			*regDat = Wire.read();
		}
		delay(1);
#endif  
		return 1;
	}
	
	// Read/write 16 bit value to/from 16 bit register address
	byte wrSensorReg16_16(int regID, int regDat)
	{
#if defined (RASPBERRY_PI)
#else
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID >> 8);            // sends instruction byte, MSB first
		Wire.write(regID & 0x00FF);
		Wire.write(regDat >> 8);            // sends data byte, MSB first
		Wire.write(regDat & 0x00FF);
		if (Wire.endTransmission())
		{
			return 0;
		}
		delay(1);
#endif
		return (1);
	}
	byte rdSensorReg16_16(uint16_t regID, uint16_t* regDat)
	{
#if defined (RASPBERRY_PI)
#else
		uint16_t temp;
		Wire.beginTransmission(sensor_addr >> 1);
		Wire.write(regID >> 8);
		Wire.write(regID & 0x00FF);
		Wire.endTransmission();
		Wire.requestFrom((sensor_addr >> 1), 2);
		if (Wire.available())
		{
			temp = Wire.read();
			*regDat = (temp << 8) | Wire.read();
		}
		delay(1);
#endif 
		return (1);
	}


	void OV5642_set_JPEG_size(uint8_t size)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)
		uint8_t reg_val;

		switch (size)
		{
		case OV5642_320x240:
			wrSensorRegs16_8(ov5642_320x240);
			break;
		case OV5642_640x480:
			wrSensorRegs16_8(ov5642_640x480);
			break;
		case OV5642_1024x768:
			wrSensorRegs16_8(ov5642_1024x768);
			break;
		case OV5642_1280x960:
			wrSensorRegs16_8(ov5642_1280x960);
			break;
		case OV5642_1600x1200:
			wrSensorRegs16_8(ov5642_1600x1200);
			break;
		case OV5642_2048x1536:
			wrSensorRegs16_8(ov5642_2048x1536);
			break;
		case OV5642_2592x1944:
			wrSensorRegs16_8(ov5642_2592x1944);
			break;
		default:
			wrSensorRegs16_8(ov5642_320x240);
			break;
		}
#endif
	}
	
	void OV5642_set_RAW_size (uint8_t size)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)		
		switch (size)
		{
		case OV5642_640x480:
			wrSensorRegs16_8(OV5642_1280x960_RAW);
			wrSensorRegs16_8(OV5642_640x480_RAW);
			break;
		case OV5642_1280x960:
			wrSensorRegs16_8(OV5642_1280x960_RAW);
			break;
		case OV5642_1920x1080:
			wrSensorRegs16_8(ov5642_RAW);
			wrSensorRegs16_8(OV5642_1920x1080_RAW);
			break;
		case OV5642_2592x1944:
			wrSensorRegs16_8(ov5642_RAW);
			break;
		}
#endif			
	}
	
	void OV5642_set_Light_Mode(uint8_t Light_Mode)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)
		switch (Light_Mode)
		{

		case Advanced_AWB:
			wrSensorReg16_8(0x3406, 0x0);
			wrSensorReg16_8(0x5192, 0x04);
			wrSensorReg16_8(0x5191, 0xf8);
			wrSensorReg16_8(0x518d, 0x26);
			wrSensorReg16_8(0x518f, 0x42);
			wrSensorReg16_8(0x518e, 0x2b);
			wrSensorReg16_8(0x5190, 0x42);
			wrSensorReg16_8(0x518b, 0xd0);
			wrSensorReg16_8(0x518c, 0xbd);
			wrSensorReg16_8(0x5187, 0x18);
			wrSensorReg16_8(0x5188, 0x18);
			wrSensorReg16_8(0x5189, 0x56);
			wrSensorReg16_8(0x518a, 0x5c);
			wrSensorReg16_8(0x5186, 0x1c);
			wrSensorReg16_8(0x5181, 0x50);
			wrSensorReg16_8(0x5184, 0x20);
			wrSensorReg16_8(0x5182, 0x11);
			wrSensorReg16_8(0x5183, 0x0);
			break;
		case Simple_AWB:
			wrSensorReg16_8(0x3406, 0x00);
			wrSensorReg16_8(0x5183, 0x80);
			wrSensorReg16_8(0x5191, 0xff);
			wrSensorReg16_8(0x5192, 0x00);
			break;
		case Manual_day:
			wrSensorReg16_8(0x3406, 0x1);
			wrSensorReg16_8(0x3400, 0x7);
			wrSensorReg16_8(0x3401, 0x32);
			wrSensorReg16_8(0x3402, 0x4);
			wrSensorReg16_8(0x3403, 0x0);
			wrSensorReg16_8(0x3404, 0x5);
			wrSensorReg16_8(0x3405, 0x36);
			break;
		case Manual_A:
			wrSensorReg16_8(0x3406, 0x1);
			wrSensorReg16_8(0x3400, 0x4);
			wrSensorReg16_8(0x3401, 0x88);
			wrSensorReg16_8(0x3402, 0x4);
			wrSensorReg16_8(0x3403, 0x0);
			wrSensorReg16_8(0x3404, 0x8);
			wrSensorReg16_8(0x3405, 0xb6);
			break;
		case Manual_cwf:
			wrSensorReg16_8(0x3406, 0x1);
			wrSensorReg16_8(0x3400, 0x6);
			wrSensorReg16_8(0x3401, 0x13);
			wrSensorReg16_8(0x3402, 0x4);
			wrSensorReg16_8(0x3403, 0x0);
			wrSensorReg16_8(0x3404, 0x7);
			wrSensorReg16_8(0x3405, 0xe2);
			break;
		case Manual_cloudy:
			wrSensorReg16_8(0x3406, 0x1);
			wrSensorReg16_8(0x3400, 0x7);
			wrSensorReg16_8(0x3401, 0x88);
			wrSensorReg16_8(0x3402, 0x4);
			wrSensorReg16_8(0x3403, 0x0);
			wrSensorReg16_8(0x3404, 0x5);
			wrSensorReg16_8(0x3405, 0x0);
			break;
		default:
			break;
		}
#endif
	}
	void OV5642_set_Color_Saturation(uint8_t Color_Saturation)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)

		switch (Color_Saturation)
		{
		case Saturation4:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x80);
			wrSensorReg16_8(0x5584, 0x80);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation3:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x70);
			wrSensorReg16_8(0x5584, 0x70);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation2:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x60);
			wrSensorReg16_8(0x5584, 0x60);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation1:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x50);
			wrSensorReg16_8(0x5584, 0x50);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation0:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x40);
			wrSensorReg16_8(0x5584, 0x40);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation_1:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x30);
			wrSensorReg16_8(0x5584, 0x30);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation_2:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x20);
			wrSensorReg16_8(0x5584, 0x20);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation_3:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x10);
			wrSensorReg16_8(0x5584, 0x10);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		case Saturation_4:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5583, 0x00);
			wrSensorReg16_8(0x5584, 0x00);
			wrSensorReg16_8(0x5580, 0x02);
			break;
		}
#endif	
	}
	void OV5642_set_Brightness(uint8_t Brightness)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)

		switch (Brightness)
		{
		case Brightness4:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x40);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Brightness3:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x30);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Brightness2:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x20);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Brightness1:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x10);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Brightness0:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x00);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Brightness_1:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x10);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x08);
			break;
		case Brightness_2:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x20);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x08);
			break;
		case Brightness_3:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x30);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x08);
			break;
		case Brightness_4:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5589, 0x40);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x558a, 0x08);
			break;
		}
#endif	

	}
	void OV5642_set_Contrast(uint8_t Contrast)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		switch (Contrast)
		{
		case Contrast4:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x30);
			wrSensorReg16_8(0x5588, 0x30);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Contrast3:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x2c);
			wrSensorReg16_8(0x5588, 0x2c);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Contrast2:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x28);
			wrSensorReg16_8(0x5588, 0x28);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Contrast1:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x24);
			wrSensorReg16_8(0x5588, 0x24);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Contrast0:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x20);
			wrSensorReg16_8(0x5588, 0x20);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Contrast_1:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x1C);
			wrSensorReg16_8(0x5588, 0x1C);
			wrSensorReg16_8(0x558a, 0x1C);
			break;
		case Contrast_2:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x18);
			wrSensorReg16_8(0x5588, 0x18);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Contrast_3:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x14);
			wrSensorReg16_8(0x5588, 0x14);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		case Contrast_4:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x04);
			wrSensorReg16_8(0x5587, 0x10);
			wrSensorReg16_8(0x5588, 0x10);
			wrSensorReg16_8(0x558a, 0x00);
			break;
		}
#endif		
	}
	void OV5642_set_Special_effects(uint8_t Special_effect)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		switch (Special_effect)
		{
		case Bluish:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x18);
			wrSensorReg16_8(0x5585, 0xa0);
			wrSensorReg16_8(0x5586, 0x40);
			break;
		case Greenish:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x18);
			wrSensorReg16_8(0x5585, 0x60);
			wrSensorReg16_8(0x5586, 0x60);
			break;
		case Reddish:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x18);
			wrSensorReg16_8(0x5585, 0x80);
			wrSensorReg16_8(0x5586, 0xc0);
			break;
		case BW:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x18);
			wrSensorReg16_8(0x5585, 0x80);
			wrSensorReg16_8(0x5586, 0x80);
			break;
		case Negative:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x40);
			break;

		case Sepia:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x18);
			wrSensorReg16_8(0x5585, 0x40);
			wrSensorReg16_8(0x5586, 0xa0);
			break;
		case Normal:
			wrSensorReg16_8(0x5001, 0x7f);
			wrSensorReg16_8(0x5580, 0x00);
			break;
		}
#endif
	}
	
	void OV5642_set_hue(uint8_t degree)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		switch (degree)
		{
		case degree_180:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x80);
			wrSensorReg16_8(0x5582, 0x00);
			wrSensorReg16_8(0x558a, 0x32);
			break;
		case degree_150:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x6f);
			wrSensorReg16_8(0x5582, 0x40);
			wrSensorReg16_8(0x558a, 0x32);
			break;
		case degree_120:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x40);
			wrSensorReg16_8(0x5582, 0x6f);
			wrSensorReg16_8(0x558a, 0x32);
			break;
		case degree_90:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x00);
			wrSensorReg16_8(0x5582, 0x80);
			wrSensorReg16_8(0x558a, 0x02);
			break;
		case degree_60:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x40);
			wrSensorReg16_8(0x5582, 0x6f);
			wrSensorReg16_8(0x558a, 0x02);
			break;
		case degree_30:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x6f);
			wrSensorReg16_8(0x5582, 0x40);
			wrSensorReg16_8(0x558a, 0x02);
			break;
		case degree_0:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x80);
			wrSensorReg16_8(0x5582, 0x00);
			wrSensorReg16_8(0x558a, 0x01);
			break;
		case degree30:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x6f);
			wrSensorReg16_8(0x5582, 0x40);
			wrSensorReg16_8(0x558a, 0x01);
			break;
		case degree60:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x40);
			wrSensorReg16_8(0x5582, 0x6f);
			wrSensorReg16_8(0x558a, 0x01);
			break;
		case degree90:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x00);
			wrSensorReg16_8(0x5582, 0x80);
			wrSensorReg16_8(0x558a, 0x31);
			break;
		case degree120:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x40);
			wrSensorReg16_8(0x5582, 0x6f);
			wrSensorReg16_8(0x558a, 0x31);
			break;
		case degree150:
			wrSensorReg16_8(0x5001, 0xff);
			wrSensorReg16_8(0x5580, 0x01);
			wrSensorReg16_8(0x5581, 0x6f);
			wrSensorReg16_8(0x5582, 0x40);
			wrSensorReg16_8(0x558a, 0x31);
			break;
		}
#endif	

	}
	void OV5642_set_Exposure_level(uint8_t level)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		switch (level)
		{
		case Exposure_17_EV:
			wrSensorReg16_8(0x3a0f, 0x10);
			wrSensorReg16_8(0x3a10, 0x08);
			wrSensorReg16_8(0x3a1b, 0x10);
			wrSensorReg16_8(0x3a1e, 0x08);
			wrSensorReg16_8(0x3a11, 0x20);
			wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case Exposure_13_EV:
			wrSensorReg16_8(0x3a0f, 0x18);
			wrSensorReg16_8(0x3a10, 0x10);
			wrSensorReg16_8(0x3a1b, 0x18);
			wrSensorReg16_8(0x3a1e, 0x10);
			wrSensorReg16_8(0x3a11, 0x30);
			wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case Exposure_10_EV:
			wrSensorReg16_8(0x3a0f, 0x20);
			wrSensorReg16_8(0x3a10, 0x18);
			wrSensorReg16_8(0x3a11, 0x41);
			wrSensorReg16_8(0x3a1b, 0x20);
			wrSensorReg16_8(0x3a1e, 0x18);
			wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case Exposure_07_EV:
			wrSensorReg16_8(0x3a0f, 0x28);
			wrSensorReg16_8(0x3a10, 0x20);
			wrSensorReg16_8(0x3a11, 0x51);
			wrSensorReg16_8(0x3a1b, 0x28);
			wrSensorReg16_8(0x3a1e, 0x20);
			wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case Exposure_03_EV:
			wrSensorReg16_8(0x3a0f, 0x30);
			wrSensorReg16_8(0x3a10, 0x28);
			wrSensorReg16_8(0x3a11, 0x61);
			wrSensorReg16_8(0x3a1b, 0x30);
			wrSensorReg16_8(0x3a1e, 0x28);
			wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case Exposure_default:
			wrSensorReg16_8(0x3a0f, 0x38);
			wrSensorReg16_8(0x3a10, 0x30);
			wrSensorReg16_8(0x3a11, 0x61);
			wrSensorReg16_8(0x3a1b, 0x38);
			wrSensorReg16_8(0x3a1e, 0x30);
			wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case Exposure03_EV:
			wrSensorReg16_8(0x3a0f, 0x40);
			wrSensorReg16_8(0x3a10, 0x38);
			wrSensorReg16_8(0x3a11, 0x71);
			wrSensorReg16_8(0x3a1b, 0x40);
			wrSensorReg16_8(0x3a1e, 0x38);
			wrSensorReg16_8(0x3a1f, 0x10);
			break;
		case Exposure07_EV:
			wrSensorReg16_8(0x3a0f, 0x48);
			wrSensorReg16_8(0x3a10, 0x40);
			wrSensorReg16_8(0x3a11, 0x80);
			wrSensorReg16_8(0x3a1b, 0x48);
			wrSensorReg16_8(0x3a1e, 0x40);
			wrSensorReg16_8(0x3a1f, 0x20);
			break;
		case Exposure10_EV:
			wrSensorReg16_8(0x3a0f, 0x50);
			wrSensorReg16_8(0x3a10, 0x48);
			wrSensorReg16_8(0x3a11, 0x90);
			wrSensorReg16_8(0x3a1b, 0x50);
			wrSensorReg16_8(0x3a1e, 0x48);
			wrSensorReg16_8(0x3a1f, 0x20);
			break;
		case Exposure13_EV:
			wrSensorReg16_8(0x3a0f, 0x58);
			wrSensorReg16_8(0x3a10, 0x50);
			wrSensorReg16_8(0x3a11, 0x91);
			wrSensorReg16_8(0x3a1b, 0x58);
			wrSensorReg16_8(0x3a1e, 0x50);
			wrSensorReg16_8(0x3a1f, 0x20);
			break;
		case Exposure17_EV:
			wrSensorReg16_8(0x3a0f, 0x60);
			wrSensorReg16_8(0x3a10, 0x58);
			wrSensorReg16_8(0x3a11, 0xa0);
			wrSensorReg16_8(0x3a1b, 0x60);
			wrSensorReg16_8(0x3a1e, 0x58);
			wrSensorReg16_8(0x3a1f, 0x20);
			break;
		}
#endif	
	}
	void OV5642_set_Sharpness(uint8_t Sharpness)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		switch (Sharpness)
		{
		case Auto_Sharpness_default:
			wrSensorReg16_8(0x530A, 0x00);
			wrSensorReg16_8(0x530c, 0x0);
			wrSensorReg16_8(0x530d, 0xc);
			wrSensorReg16_8(0x5312, 0x40);
			break;
		case Auto_Sharpness1:
			wrSensorReg16_8(0x530A, 0x00);
			wrSensorReg16_8(0x530c, 0x4);
			wrSensorReg16_8(0x530d, 0x18);
			wrSensorReg16_8(0x5312, 0x20);
			break;
		case Auto_Sharpness2:
			wrSensorReg16_8(0x530A, 0x00);
			wrSensorReg16_8(0x530c, 0x8);
			wrSensorReg16_8(0x530d, 0x30);
			wrSensorReg16_8(0x5312, 0x10);
			break;
		case Manual_Sharpnessoff:
			wrSensorReg16_8(0x530A, 0x08);
			wrSensorReg16_8(0x531e, 0x00);
			wrSensorReg16_8(0x531f, 0x00);
			break;
		case Manual_Sharpness1:
			wrSensorReg16_8(0x530A, 0x08);
			wrSensorReg16_8(0x531e, 0x04);
			wrSensorReg16_8(0x531f, 0x04);
			break;
		case Manual_Sharpness2:
			wrSensorReg16_8(0x530A, 0x08);
			wrSensorReg16_8(0x531e, 0x08);
			wrSensorReg16_8(0x531f, 0x08);
			break;
		case Manual_Sharpness3:
			wrSensorReg16_8(0x530A, 0x08);
			wrSensorReg16_8(0x531e, 0x0c);
			wrSensorReg16_8(0x531f, 0x0c);
			break;
		case Manual_Sharpness4:
			wrSensorReg16_8(0x530A, 0x08);
			wrSensorReg16_8(0x531e, 0x0f);
			wrSensorReg16_8(0x531f, 0x0f);
			break;
		case Manual_Sharpness5:
			wrSensorReg16_8(0x530A, 0x08);
			wrSensorReg16_8(0x531e, 0x1f);
			wrSensorReg16_8(0x531f, 0x1f);
			break;
		}
#endif
	}
	void OV5642_set_Mirror_Flip(uint8_t Mirror_Flip)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		uint8_t reg_val;
		switch (Mirror_Flip)
		{
		case MIRROR:
			rdSensorReg16_8(0x3818, &reg_val);
			reg_val = reg_val | 0x00;
			reg_val = reg_val & 0x9F;
			wrSensorReg16_8(0x3818, reg_val);
			rdSensorReg16_8(0x3621, &reg_val);
			reg_val = reg_val | 0x20;
			wrSensorReg16_8(0x3621, reg_val);

			break;
		case FLIP:
			rdSensorReg16_8(0x3818, &reg_val);
			reg_val = reg_val | 0x20;
			reg_val = reg_val & 0xbF;
			wrSensorReg16_8(0x3818, reg_val);
			rdSensorReg16_8(0x3621, &reg_val);
			reg_val = reg_val | 0x20;
			wrSensorReg16_8(0x3621, reg_val);
			break;
		case MIRROR_FLIP:
			rdSensorReg16_8(0x3818, &reg_val);
			reg_val = reg_val | 0x60;
			reg_val = reg_val & 0xFF;
			wrSensorReg16_8(0x3818, reg_val);
			rdSensorReg16_8(0x3621, &reg_val);
			reg_val = reg_val & 0xdf;
			wrSensorReg16_8(0x3621, reg_val);
			break;
		case Normal:
			rdSensorReg16_8(0x3818, &reg_val);
			reg_val = reg_val | 0x40;
			reg_val = reg_val & 0xdF;
			wrSensorReg16_8(0x3818, reg_val);
			rdSensorReg16_8(0x3621, &reg_val);
			reg_val = reg_val & 0xdf;
			wrSensorReg16_8(0x3621, reg_val);
			break;
		}
#endif
	}
	void OV5642_set_Compress_quality(uint8_t quality)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		switch (quality)
		{
		case high_quality:
			wrSensorReg16_8(0x4407, 0x02);
			break;
		case default_quality:
			wrSensorReg16_8(0x4407, 0x04);
			break;
		case low_quality:
			wrSensorReg16_8(0x4407, 0x08);
			break;
		}
#endif
	}
	void OV5642_Test_Pattern(uint8_t Pattern)
	{
#if defined(OV5642_CAM) || defined(OV5642_CAM_BIT_ROTATION_FIXED)|| defined(OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_PLUS)	
		switch (Pattern)
		{
		case Color_bar:
			wrSensorReg16_8(0x503d, 0x80);
			wrSensorReg16_8(0x503e, 0x00);
			break;
		case Color_square:
			wrSensorReg16_8(0x503d, 0x85);
			wrSensorReg16_8(0x503e, 0x12);
			break;
		case BW_square:
			wrSensorReg16_8(0x503d, 0x85);
			wrSensorReg16_8(0x503e, 0x1a);
			break;
		case DLI:
			wrSensorReg16_8(0x4741, 0x4);
			break;
		}
#endif
	}
   
	void set_format(byte fmt)
	{
		if (fmt == BMP)
			m_fmt = BMP;
		else if (fmt == RAW)
			m_fmt = RAW;
		else
			m_fmt = JPEG;
	}
	
  protected:
	regtype *P_CS;
	regsize B_CS;
	byte m_fmt;
	byte sensor_model;
	byte sensor_addr;
};

#endif