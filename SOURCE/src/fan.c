#include "fan.h"

struct fanInitTypeDef fan;

void FanAnalyze(void){ // 10Hz
  if(0x00 == fan.sensorTimer){
    fan.sensorTimer = settings.fanSensorInterval;
    for(uint8_t i = 0x09; i > 0x00 ; i--){
      fan.oldTemperature[i] = fan.oldTemperature[i - 0x01];
      fan.oldHumidity[i] = fan.oldHumidity[i - 0x01];
    }
    if(0xFFFF != dht22.humidity || 0xFFFF != dht22.temperature){
      fan.oldTemperature[0x00] = dht22.temperature;
      fan.oldHumidity[0x00] = dht22.humidity;
    }
  }
  if(settings.fanOnOff){
    if((fan.oldHumidity[0x00] - fan.oldHumidity[0x09]) > settings.fanGistHumidity || fan.oldHumidity[0x00] > settings.fanMaxHumidity){
      fan.jobHum = true;
    }
  }
  if((fan.jobHum && fan.timer > (settings.fanIntervalHum - settings.fanDelayHum)) || fan.job){ FAN_ON; }else{ FAN_OFF; fan.jobHum = false;}
  if(0x00 == fan.timer){ fan.timer = settings.fanIntervalHum; }
  if(0x00 == fan.jobTimer) {fan.job = false;}
  if(fan.job){ fan.jobTimer--; }else{ fan.jobTimer = settings.fanMaxJob; }
  fan.timer--;
  fan.sensorTimer--;
}
void FanInit(void){
  FAN_OFF;
  GPIOE->CRL = (GPIOE->CRL & (~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6))) | (GPIO_CRL_MODE6 | GPIO_CRL_CNF6_0); // FAN
  
  #if defined(DEBUG)
    printf("< OK >    Initialization FAN\r\n");
  #endif
}
