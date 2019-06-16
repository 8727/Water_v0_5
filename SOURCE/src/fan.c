#include "fan.h"

struct fanInitTypeDef fan;

void FanAnalyze(void){
  uint8_t i;
  if(fan.sensorCounter > fan.sensorInterval){
    fan.sensorCounter = 0x00;
    for(i = 0x09; i > 0x00 ; i--){
      fan.oldTemperature[i] = fan.oldTemperature[i - 0x01];
      fan.oldHumidity[i] = fan.oldHumidity[i - 0x01];
    }
    if(0xFFFF != dht22.humidity || 0xFFFF != dht22.temperature)
    fan.oldTemperature[0x00] = dht22.temperature;
    fan.oldHumidity[0x00] = dht22.humidity;
  }
  if((fan.oldHumidity[0x00] - fan.oldHumidity[0x09]) > fan.gistHumidity || fan.oldHumidity[0x00] > fan.maxHumidity){
    fan.status = 0x01;
  }
  if(0x01 == fan.status && fan.counter < fan.delay){ FAN_ON; }else{ FAN_OFF; fan.status = 0x00;}
  fan.counter++;
  if(fan.counter > fan.interval){ fan.counter = 0x00; }
  fan.sensorCounter++;
}

void FanInit(void){
  GPIOE->CRL = (GPIOE->CRL & (~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6))) | (GPIO_CRL_MODE6 | GPIO_CRL_CNF6_0); // FAN
  
}
