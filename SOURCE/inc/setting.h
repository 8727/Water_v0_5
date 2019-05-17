#ifndef _SETTING_H
#define _SETTING_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
// #include "Ee24cxx.h"
// #include "rtc.h"
// #include "ili9488.h"
// #include "w25qxx.h"
// #include "xpt2046.h"
// #include "rs485.h"
// #include "dht22.h"
// #include "ds18b20.h"
// #include "ws2811.h"
// #include "lighting.h"
// #include "timers.h"


/* Define --------------------------------------------------------------------*/
#define NAME_BUILD              "v0.05"     // 0x09, 0x0A, 0x0B, 0x0C  // v0.05
#define DEVICE_NUMBER           0x01        // Device number
#define RTC_CALIBRATION         0x00        // RTC CalibrationPpm
#define CALIB_POWER_V           0x00        // CALIB_POWER_V




#define CAN_SPEED               0x001C0004  //0x00050004 // 1000 Kb
                                            //0x001C0004 // 500 Kb
                                            //0x001C0009 // 250 Kb
                                            //0x001C0013 // 125 Kb
                                            //0x001C0018 // 100 Kb
#define CanModeNormal           0x00
#define CanModeLoopBack         CAN1->BTR |= CAN_BTR_LBKM
#define CanModeSilent           CAN1->BTR |= CAN_BTR_SILM
#define CanModeSilentLoopBack   CAN1->BTR |= (CAN_BTR_SILM | CAN_BTR_LBKM)
                                            
#define RS485_SPEED             0x0140      //0x0057       //921600
                                            //0x00A0       //500000
                                            //0x00AE       //460800
                                            //0x0140       //250000
                                            //0x015B       //230400
                                            //0x0000       //125000
                                            //0x02B6       //115200
                                            //0x056D       //57600
                                            //0x0823       //38400
                                            //0x208D       //9600
//#define USART_2_3_SPEED                     0x002B       //921600
//#define USART_2_3_SPEED                     0x0050       //500000
//#define USART_2_3_SPEED                     0x0057       //460800
//#define USART_2_3_SPEED                     0x00A0       //250000
//#define USART_2_3_SPEED                     0x00AE       //230400
//#define USART_2_3_SPEED                     0x0000       //125000
#define USART_2_3_SPEED_1152                  0x015B       //115200
//#define USART_2_3_SPEED_1152                0x015A       //115200
//#define USART_2_3_SPEED                     0x02B6       //57600
//#define USART_2_3_SPEED                     0x0412       //38400
//#define USART_2_3_SPEED                     0x0823       //19200
#define USART_2_3_SPEED_96                    0x1047       //9600





#define EEPROM_BUILD_DATE       0x00        // 0x00, 0x01, 0x02, 0x03  BUILD_DATE
#define EEPROM_STATUS           0x04        // Status
#define EEPROM_DEVICE_N         0x05        // Device number
#define EEPROM_BUILD_TYPE       0x06        // Device type, S = switch, W = water
#define EEPROM_CALIBRATION      0x07        // RTC CalibrationPpm

#define EEPROM_NAME_BUILD       0x01        // 0x08, 0x09, 0x0A, 0x0B, 0x0C  // NAME_BUILD

#define EEPROM_CALIB_POWER_V    0x0F        // CALIB_POWER_V

#define EEPROM_CAN_SPEED        0x03        // 0x18, 0x19, 0x1A, 0x1B
                                            // 0x1C, 0x1D, 0x1E, 0x1F
#define EEPROM_RS485_SPEED      0x20        // 0x21
 




#define PRIORITY_DHT22_DMA      0x0F
#define PRIORITY_ADC            0x0E
#define PRIORITY_RTC            0x0C
#define PRIORITY_XPT2046_LIGHT  0x0D
#define PRIORITY_BRIGHET        0x0A
#define PRIORITY_RS485          0x01
#define PRIORITY_DS18B20        0x00
#define PRIORITY_DHT22_TIM      0x00
#define PRIORITY_CAN_TX         0x00
#define PRIORITY_CAN_RX0        0x00
#define PRIORITY_CAN_RX1        0x00
#define PRIORITY_CAN_ERROR      0x00
#define PRIORITY_GUI            0x00



struct settingsInitTypeDef{
  uint8_t  type;
  uint8_t  number;
  uint32_t date;
  uint16_t canDevice;
  uint8_t  calibration;
  int8_t   calibPowerV;
  uint32_t canSpeed;
  uint16_t rs485Speed;
  // LCD
  uint8_t rotation;
  uint16_t maxX;
  uint16_t maxY;
  uint8_t ax;
  uint8_t ay;
  int16_t bx;
  int16_t by;
  
  
  
  uint8_t brightnessW[LEDS_W];
  uint8_t effectRGB;
  
};
extern struct settingsInitTypeDef settings;

uint32_t GetTick(void);
void DelayMs(uint32_t ms);

void Start(void);
void ReadConfig(void);
void CounterToBuffer(uint32_t counter, uint8_t* buff);
uint32_t BufferToCounter(uint8_t* buff);
void Setting(void);

#endif /* _SETTING_H */
