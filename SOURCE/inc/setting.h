#ifndef _SETTING_H
#define _SETTING_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "in.h"
#include "adc.h"
#include "gidrolock.h"
#include "heating.h"
#include "fan.h"
#include "Ee24cxx.h"
#include "rtc.h"
#include "ds18b20.h"
#include "dht22.h"
#include "nrf24.h"
#include "rs485.h"
#include "w25qxx.h"
#include "ili9488.h"
#include "beep.h"

/* Define --------------------------------------------------------------------*/
#define HW_BUILD                         "0.05"      // "0.04" 0x302E3035
#define SW_BUILD                         "1.00"      // "1.00" 0x312E3033
#define STATUS                           'D'         // 0x44 -default "D", 
#define DEVICE_NUMBER                    0x00        // Device number
#define DEVICE_TYPE                      0x05        // Device type  0x07 = Core, 0x06 = Water, 0x05 = Power/Timer start,0x04 = SW
#define RTC_CALIBRATION                  0x00        // RTC CalibrationPpm
#define LCD_ROTATION                     0x09        // 0x27 Rotation_270, 0x18 Rotation_180, 0x09 Rotation_90, 0x00 Rotation_0
#define TIME_ZONE                        0x00        //

#define RF24_SP_PW                       0X00
#define RF24_CH                          0X20
#define RF24_TX_ADR                      0x7E7E7E7E  //
#define RF24_RX0_ADR                     0x7E7E7E7E  //
#define RF24_RX1_ADR                     0x2C2C2C2C  //
#define RF24_RX2_ADR                     0x3C        //
#define RF24_RX3_ADR                     0x4C        //
#define RF24_RX4_ADR                     0x5C        //
#define RF24_RX5_ADR                     0x6C        //

#define SENSOR_ON_OFF                    0x0F
#define CALIB_SENSOR1                    0x00
#define CALIB_SENSOR2                    0x00
#define CALIB_SENSOR3                    0x00
#define CALIB_SENSOR4                    0x00
#define CALIB_PRESSURE1                  0x00
#define CALIB_PRESSURE2                  0x00
#define CALIB_PRESSURE3                  0x00
#define CALIB_PRESSURE4                  0x00
#define ALARM_SENSOR1                    0xF0
#define ALARM_SENSOR2                    0xF0
#define ALARM_SENSOR3                    0xF0
#define ALARM_SENSOR4                    0xF0

#define CALIB_POWER_V                    0x00

#define FAN_ON_OFF                       0x01
#define SENSOR_INTRV                     0x0258 // 60sec * 10Hz
#define MAX_JOB                          0xD2F0 // 90min * 60sec * 10Hz
#define INTERVAL_HUM                     0x2328 // 15min * 60sec * 10Hz
#define DELAY_HUM                        0x1770 // 10min * 60sec * 10Hz
#define MAX_TEMPR                        0x015E // 35 * 10
#define MAX_HUM                          0x0320 // 80 * 10
#define GIST_TEMPR                       0x001E // 3 * 10
#define GIST_HUM                         0x001E // 3 * 10

#define HEAT_ON_OFF                      0x1F 
#define HEAT_TEMPER_R1                   0x1E   // 30
#define HEAT_TEMPER_R2                   0x1E   // 30
#define HEAT_TEMPER_R3                   0x1E   // 30
#define HEAT_TEMPER_R4                   0x1E   // 30
#define HEAT_TEMPER_R5                   0x1E   // 30
#define HEAT_JOB_DEFAULT_DELAY           0x64   // 10sec * 10Hz
#define HEAT_JOB_INTERVAL                0x0258 // 60sec * 10Hz
#define HEAT_SENSOR_INTERVAL             0x0258 // 60sec * 10Hz
#define HEAT_TOP_TEMPERATURE             0x01F4 // 50 * 10
#define HEAT_GIST_TEMPERATURE            0x03   // 0.3 * 10
#define HEAT_STEP_DELAY                  0x02   // 0.2sec * 10Hz
#define HEAT_MAX_DELAY                   0xC8   // 20sec * 10Hz


//#define HEATING_MAX_DEVICES              0x05
//#define DS18B20_MAX_DEVICES              0x08

#define EEPROM_BUFF                      0x80

/* Define --------------------------------------------------------------------*/
#define CanModeNormal                    0x00
#define CanModeLoopBack                  CAN1->BTR |= CAN_BTR_LBKM
#define CanModeSilent                    CAN1->BTR |= CAN_BTR_SILM
#define CanModeSilentLoopBack            CAN1->BTR |= (CAN_BTR_SILM | CAN_BTR_LBKM)

#define CAN_SPEED                        0x001C0004  //0x00050004 // 1000 Kb
                                                     //0x001C0004 // 500 Kb
                                                     //0x001C0009 // 250 Kb
                                                     //0x001C0013 // 125 Kb
                                                     //0x001C0018 // 100 Kb

#define RS485_SPEED                      0x208D      //0x0057       //921600
                                                     //0x00A0       //500000
                                                     //0x00AE       //460800
                                                     //0x0140       //250000
                                                     //0x015B       //230400
                                                     //0x0000       //125000
                                                     //0x02B6       //115200
                                                     //0x056D       //57600
                                                     //0x0823       //38400
                                                     //0x208D       //9600

#define USART_2_3_SPEED_1152             0x015B      //0x002B       //921600
#define USART_2_3_SPEED_96               0x1047      //0x0050       //500000
                                                     //0x0057       //460800
                                                     //0x00A0       //250000
                                                     //0x00AE       //230400
                                                     //0x0000       //125000
                                                     //0x015B       //115200
                                                     //0x02B6       //57600
                                                     //0x0412       //38400
                                                     //0x0823       //19200
                                                     //0x1047       //9600

/* Define --------------------------------------------------------------------*/
#define ADDR_DATE_BUILD                  0x00 // 0x00-0x03
#define ADDR_STATUS                      0x04
#define ADDR_DEVICE_NUMBER               0x05
#define ADDR_DEVICE_TYPE                 0x06
#define ADDR_RTC_CALIBRATION             0x07
#define ADDR_TIME_ZONE                   0x08
#define ADDR_LCD_ROTATION                0x09
#define ADDR_CAN_SPEED                   0x0A // 0x0A-0x0D
#define ADDR_RS485_SPEED                 0x0E // 0x0E-0x0F
#define ADDR________X                    0x10 // 0x10-0x1D
#define ADDR_RF24_SP_PW                  0x1E
#define ADDR_RF24_CH                     0x1F
#define ADDR_RF24_TX_ADR                 0x20 // 0x20-0x23
#define ADDR_RF24_RX0_ADR                0x24 // 0x24-0x27
#define ADDR_RF24_RX1_ADR                0x28 // 0x28-0x2B
#define ADDR_RF24_RX2_ADR                0x2C
#define ADDR_RF24_RX3_ADR                0x2D
#define ADDR_RF24_RX4_ADR                0x2E
#define ADDR_RF24_RX5_ADR                0x2F
/* Define --------------------------------------------------------------------*/
#define ADDR_SENSOR_ON_OFF               0x30
#define ADDR_CALIB_SENSOR1               0x31
#define ADDR_CALIB_SENSOR2               0x32
#define ADDR_CALIB_SENSOR3               0x33
#define ADDR_CALIB_SENSOR4               0x34
#define ADDR_CALIB_PRESSURE1             0x35
#define ADDR_CALIB_PRESSURE2             0x36
#define ADDR_CALIB_PRESSURE3             0x37
#define ADDR_CALIB_PRESSURE4             0x38
#define ADDR_ALARM_SENSOR1               0x39
#define ADDR_ALARM_SENSOR2               0x3A
#define ADDR_ALARM_SENSOR3               0x3B
#define ADDR_ALARM_SENSOR4               0x3C
#define ADDR_________X                   0x3D // 0x3D-0x3E
#define ADDR_CALIB_POWER_V               0x3F
#define ADDR_FAN_ON_OFF                  0x40
#define ADDR_SENSOR_INTRV                0x41 // 0x41-0x42
#define ADDR_MAX_JOB                     0x43 // 0x43-0x44
#define ADDR_INTERVAL_HUM                0x45 // 0x45-0x46
#define ADDR_DELAY_HUM                   0x47 // 0x47-0x48
#define ADDR_MAX_TEMPR                   0x49 // 0x49-0x4A
#define ADDR_MAX_HUM                     0x4B // 0x4B-0x4C
#define ADDR_GIST_TEMPR                  0x4D
#define ADDR_GIST_HUM                    0x4E
#define ADDR_HEAT_ON_OFF                 0x4F
#define ADDR_HEAT_TEMPER_R1              0x50
#define ADDR_HEAT_TEMPER_R2              0x51
#define ADDR_HEAT_TEMPER_R3              0x52
#define ADDR_HEAT_TEMPER_R4              0x53
#define ADDR_HEAT_TEMPER_R5              0x54
#define ADDR_HEAT_JOB_DEFAULT_DELAY      0x55 // 0x55-0x56
#define ADDR_HEAT_JOB_INTERVAL           0x57 // 0x57-0x58
#define ADDR_HEAT_SENSOR_INTERVAL        0x59 // 0x59-0x5A
#define ADDR_HEAT_TOP_TEMPERATURE        0x5B // 0x5B-0x5C
#define ADDR_HEAT_GIST_TEMPERATURE       0x5D
#define ADDR_HEAT_STEP_DELAY             0x5E
#define ADDR_HEAT_MAX_DELAY              0x5F

/* Define --------------------------------------------------------------------*/
#define PRIORITY_DHT22_DMA               0x0F
#define PRIORITY_RTC                     0x0E
#define PRIORITY_RS485                   0x01
#define PRIORITY_GUI_UPDATE              0x00
#define PRIORITY_HEATING                 0x00
#define PRIORITY_DS18B20                 0x00
#define PRIORITY_DHT22_TIM               0x00
#define PRIORITY_ADC                     0x00
#define PRIORITY_CAN_TX                  0x00
#define PRIORITY_CAN_RX0                 0x00
#define PRIORITY_CAN_RX1                 0x00
#define PRIORITY_CAN_ERROR               0x00

/* Define --------------------------------------------------------------------*/
struct settingsInitTypeDef{
  uint32_t dateBuild;
  uint8_t  number;
  uint8_t  type;
  uint8_t  rtcCalib;
  uint32_t canSpeed;
  uint16_t rs485Speed;
  uint8_t  rotation;
  int8_t   timeZone;
  uint16_t canDevice;
  
  
  // LCD
  uint16_t maxX;
  uint16_t maxY;
  //RF24L01
  uint8_t  rf24SpeedPower;
  uint8_t  rf24Ch;
  uint32_t rf24Tx;
  uint32_t rf24Rx0;
  uint32_t rf24Rx1;
  uint8_t  rf24Rx2;
  uint8_t  rf24Rx3;
  uint8_t  rf24Rx4;
  uint8_t  rf24Rx5;
  //ADC
  uint8_t  sensorOnOff;
  int8_t   calibSensor1;
  int8_t   calibSensor2;
  int8_t   calibSensor3;
  int8_t   calibSensor4;
  int8_t   calibPressure1;
  int8_t   calibPressure2;
  int8_t   calibPressure3;
  int8_t   calibPressure4;
  uint8_t  alarmSensor1;
  uint8_t  alarmSensor2;
  uint8_t  alarmSensor3;
  uint8_t  alarmSensor4;
  int8_t   calibPowerV;
  uint8_t  onAlarm;
  //FAN
  _Bool    fanOnOff;
  uint16_t fanSensorInterval;
  uint16_t fanMaxJob;
  uint16_t fanIntervalHum;
  uint16_t fanDelayHum;
  uint16_t fanMaxTemperature;
  uint16_t fanMaxHumidity;
  uint8_t  fanGistTemperature;
  uint8_t  fanGistHumidity;
  //HEATING
  uint8_t  heatOnOff;
  uint16_t heatJobDefDelay;
  uint16_t heatJobInterval;
  uint16_t heatSensorInterval;
  uint16_t heatTopTemperature;
  uint8_t  heatGistTemperature;
  uint8_t  heatStepDelay;
  uint8_t  heatMaxDelay;
  
  
  
  
  
};

extern struct settingsInitTypeDef settings;

uint32_t GetTick(void);
void DelayMs(uint32_t ms);

void ReadConfig(void);
void CounterToBuffer(uint32_t counter, uint8_t* buff);
uint32_t BufferToCounter(uint8_t* buff);
void Setting(void);

#endif /* _SETTING_H */
