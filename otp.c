// нужно ввести CRC настроек. а то каким-то образом on_delay_step стал 5 вместо 0.25
// это неправильное значение даже сохранилось в EEPROM

#include <Ethernet.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <MemoryFree.h>
//#include <utility/w5100.h>

typedef uint8_t SensorAddress[8]; // определяем тип данных для адреса одного датчика

// настройки ethernet
byte mac[] = {0x1E, 0x2D, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(8, 8, 8, 8);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
char server[] = "www.xxxxxxxx.ru";
EthernetClient client;


#define RELAY_PIN  2      // номер выхода,подключенного к реле

OneWire ds1(7); // Создаем объект OneWire для шины 1-Wire на 7-пине
OneWire ds2(8); // Создаем объект OneWire для шины 1-Wire на 7-пине
OneWire ds3(6); // Создаем объект OneWire для шины 1-Wire на 7-пине
OneWire ds4(9); // Создаем объект OneWire для шины 1-Wire на 7-пине
byte temper_data[2]; // Место для значения температуры

// настройки по умолчанию ШИМ-алгоритма управления термоприводом
float temp_ideal=30; // целевая температура по умолчанию
float temp_gist=0.35; // гистерезис. греть/охлаждать перестаем на 0.3 градуса раньше
float on_delay_default=10;  // 10 секунд работаем (питание на клапан), 40 секунд отдыхаем
float sum_delay=50; // общее время работы с клапаном 50 секунд. если будем дольше включать - то уменьшим время выключения
float temp_delay=120; //  каждые 120 секунд определяем - надо ли изменить продолжительность питания/отдыха
float diff_delay=120; //  динамику изменения температуры: t 2x сек назад, t 1x сек назад, t сейчас
float on_delay_step=0.5; //  шаг изменения ширины импульса (за раз изменяем на 1 секунду)
float max_temp=50; //  если температура выше критической - сбрасываем настройку $on_delay
float max_on_delay=20; //  не греем клапан дольше 20 сек
// инициализируем переменные для работы ШИМ-алгоритма управления термоприводом
float temp_now=0; // температура в этот конкретный момент
float temp_old=0; // старая температура
unsigned long temp_old_time=0; // момент сохранения старой температуры в temp_old

float temp_before1=0; // температура temp_delay секунд назад
float temp_before2=0; // температура 2 х temp_delay секунд назад
float on_delay=0; // время работы с клапаном
int target_direction=0; //  направление к целевой температуре. +1 значит хотим повысить, -1 значит хотим понизить. 0 - ничего не хотим
int alarm_frost=0; //  один раз при перегреве снижаем $on_delay
int hot_stopper=0; //  один раз если температура резко снижается больше 1.5 градусов за 4 минуты - увеличиваем $on_delay (пытаемся замедлить снижение, чтобы клапан не закрылся целиком)
int grow_chance_count=0; //  счетчик шансов на рост. когда температура сама растет - просто ждем, вдруг дорастёт сама
                         //  но если 3 раза подряд роста до целевой температуры не произошло - значит нужно
                         //  усилить воздействие (ширину импульса)
int freeze_counter=0;    // пока идёт обратный отсчет freeze_counter (до нуля) мы не пытаемся влиять на клапан, а просто наблюдаем (кроме аварийных случаев)
bool relay_status=false;
String info=""; // для каждого сеанса связи с сервером собираем всякую отладочную инфу и отправляем.
float temp_outdoor=0; // температура на улице
float temp_home1=0; // температура дома
float temp_home2=0; // температура дома
float temp_home3=0; // температура дома
float temp_home4=0; // температура дома

long bigtimer; // таймер для контроля периодичности выполнения команд. будем раз в секунду добавлять +1. если будет расходиться с реальным временем - значит где-то зависания.
int need_grow_timer=0; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
                     // если (таймер > 10 x temp_delay && on_delay < on_delay_default) тогда on_delay = on_delay_default 
                     // для случаев когда после сильного падения on_delay (например, до нуля) замучаешься ждать роста
                     // ведь с шагом 0.25 необходимо 12 / 0.25 * 2мин = 96 минут

int FindStrInArray(char *mArray,int mlength, String marker) {
 int posi=-1;
 //Serial.println("marker:"+marker);
 //Serial.print("size:");
 //Serial.println(marker.length());
 for(int i=0;i<mlength-marker.length();i++)
  {
   int f=0;
   //Serial.print("pos:"); Serial.print(i); Serial.print(" symbol:"); Serial.println(mArray[i]);
   for(int j=0;j<marker.length()-1;j++)
    {
     if (mArray[i+j]==marker[j]) 
      { 
       f=1; 
       //Serial.print("совпал символ на поз"); Serial.print(i+j); Serial.print(" маркер:"); Serial.print(marker[j]);  Serial.print(" symbol:"); Serial.println(mArray[i+j]); 
      }
     else 
      {
       if (f==1) f=0;
       break; 
      }
    } 
   if (f==1) 
    {  
     posi=i; 
     break; 
    }
  } 
 return(posi); 
}

String GetStrFromArrayBetweenMarkers(char *mArray,int ArrayLength,String marker1, String marker2) {
 int posi1,posi2;
 posi1=FindStrInArray(mArray,ArrayLength,marker1);
 if (posi1==-1) { return("error1"); }
 posi2=FindStrInArray(mArray,ArrayLength,marker2);
 if (posi2==-1) return("error2");

 String res="";
 for (int i=posi1+marker1.length(); i<posi2; i++)
  res=res+mArray[i];

 return(res);
}

float StringToFloat(String value) {
 float floatVar;
 char floatbufVar[32];
 value.toCharArray(floatbufVar,sizeof(floatbufVar));
 floatVar=atof(floatbufVar);  
 return(floatVar);
}

// this method makes a HTTP connection to the server:
bool httpRequest(String request,String request2,bool fOnlySend) {
  //client.setTimeout(2000);
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.print("GET /");
    client.print(request);
    client.print(request2);
    client.println(" HTTP/1.1");
    client.println("Host: <a href="http://www.xxxxxxxxxxx.ru" title="www.xxxxxxxxxxx.ru"" rel="nofollow">www.xxxxxxxxxxx.ru"</a>);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    return(true);
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    return(false);
  }
 if (fOnlySend) client.stop(); 
}

float HttpGet() {
 char mGet[200];
 int mGetSize=0; int f=1; char c2=' '; bool f_start=false;
 //client.setTimeout(2000);
 long startOfHttpResponseTime = millis();
 while ((long)(millis() - startOfHttpResponseTime) < 1000) {
  if (client.available()) 
   {
    char c = client.read(); 
    if (!f_start) {
     if (c2!='P'||c!='H') {
      c2=c;
      continue;
     }
     f_start=true;
    } 
    Serial.print(c);
    mGet[mGetSize]=c;
    
    mGetSize++;
    if (mGetSize>sizeof(mGet)-1) break;
   }
  else break;
 }
 //client.stop();
 //Serial.print("buffer size:");
 //Serial.println(mGetSize);
 //Serial.println(mGet);
 if (mGetSize==0) return(-1);
 //Serial.println(mGetSize);

 // получаем настройки с сервера
 String res;
 
 Serial.println(temp_ideal);
 float last_temp_ideal=temp_ideal;
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st1%","en1%"); if (res!="") temp_ideal=StringToFloat(res);
 // если на сервере изменили temp_ideal - то нужно срочно начать реагировать
 if (last_temp_ideal!=temp_ideal) { on_delay=on_delay_default; freeze_counter=0; }
 
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st2%","en2%"); if (res!="") temp_gist=StringToFloat(res);
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st3%","en3%"); if (res!="") sum_delay=StringToFloat(res);
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st4%","en4%"); if (res!="") temp_delay=StringToFloat(res);
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st5%","en5%"); if (res!="") diff_delay=StringToFloat(res);
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st6%","en6%"); if (res!="") on_delay_step=StringToFloat(res);
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st7%","en7%"); if (res!="") max_temp=StringToFloat(res);
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st8%","en8%"); if (res!="") max_on_delay=StringToFloat(res);

 float last_on_delay_default=on_delay_default;
 res=GetStrFromArrayBetweenMarkers(mGet,mGetSize,"st9%","en9%"); if (res!="") on_delay_default=StringToFloat(res);
 // если на сервере изменили on_delay по умолчанию - то нужно обновить on_delay. вдруг удаленное управление при аварийной ситуации
 if (last_on_delay_default!=on_delay_default) { on_delay=on_delay_default; info=info+"def_on_delay_updated,"; }

 WriteParamsToEEPROM(); // полученные настройки запишем в ПЗУ

 Serial.println(temp_ideal);
 Serial.println(temp_gist);
 Serial.println(on_delay_step);

 Serial.println("загрузили настройки");
 //Serial.println(res);
 //Serial.println(resfloat);
 float resfloat=1;
 return(resfloat);
}

float GetParamsFromDB() {
 Serial.println("подключаемся к серверу");
 if (httpRequest("ard.php?action=get_params","",false)) { // успешно отправили запрос
  Serial.println("запрашиваем настройки");
  delay(500);
  Serial.println(HttpGet());
  Serial.println(sum_delay);
 }
}
 
void EEPROM_float_write(int addr, float val) // запись в ЕЕПРОМ
{
  byte *x = (byte *)&val;
  for(byte i = 0; i < 4; i++) {
   if (EEPROM.read(i+addr)!=x[i]) EEPROM.write(i+addr, x[i]); // записываем только если изменилось - ведь ячейки ПЗУ не вечны
  }
}
 
float EEPROM_float_read(int addr) // чтение из ЕЕПРОМ
{   
  byte x[4];
  for(byte i = 0; i < 4; i++) x[i] = EEPROM.read(i+addr);
  float *y = (float *)&x;
  return y[0];
}

void ReadParamsFromEEPROM() // читаем все настройки из ПЗУ при старте
{
 // по адресу 0 находится флаг. =1 можно читать. =255 не читать. если ПЗУ пустое (новая ардуино) то по адресу 0 будет лежать 255.
 if (EEPROM.read(0)!=1) return;
 
 float val;
 val=-1; val=EEPROM_float_read(0*4+1); if (val!=-1) { temp_ideal=val; Serial.println(val); }
 val=-1; val=EEPROM_float_read(1*4+1); if (val!=-1) temp_gist=val;
 Serial.println(val);
 val=-1; val=EEPROM_float_read(2*4+1); if (val!=-1) sum_delay=val;
 Serial.println(val);
 val=-1; val=EEPROM_float_read(3*4+1); if (val!=-1) temp_delay=val;
 Serial.println(val);
 val=-1; val=EEPROM_float_read(4*4+1); if (val!=-1) diff_delay=val;
 Serial.println(val);
 val=-1; val=EEPROM_float_read(5*4+1); if (val!=-1) on_delay_step=val;
 Serial.println(val);
 val=-1; val=EEPROM_float_read(6*4+1); if (val!=-1) max_temp=val;
 Serial.println(val);
 val=-1; val=EEPROM_float_read(7*4+1); if (val!=-1) max_on_delay=val;
 Serial.println(val);
 val=-1; val=EEPROM_float_read(8*4+1); if (val!=-1) on_delay_default=val;
 Serial.println(val);
}

void WriteParamsToEEPROM() // сохраняем все настройки в ПЗУ
{
 EEPROM_float_write(0*4+1,temp_ideal);
 EEPROM_float_write(1*4+1,temp_gist);
 EEPROM_float_write(2*4+1,sum_delay);
 EEPROM_float_write(3*4+1,temp_delay);
 EEPROM_float_write(4*4+1,diff_delay);
 EEPROM_float_write(5*4+1,on_delay_step);
 EEPROM_float_write(6*4+1,max_temp);
 EEPROM_float_write(7*4+1,max_on_delay);
 EEPROM_float_write(8*4+1,on_delay_default);
 
 if (EEPROM.read(0)!=1) EEPROM.write(0, 1); // ставим флаг = 1 (можно читать настройки из EEPROM )
}

void initEthernet() {
 Serial.println("инициализируем ethernet");
 // start the Ethernet connection using a fixed IP address and DNS server:
 //W5100.setRetransmissionCount(2);
 Ethernet.begin(mac, ip, myDns,gateway,subnet);
 //Ethernet.begin(mac);
 // print the Ethernet board/shield's IP address:
 Serial.print("My IP address: ");
 Serial.println(Ethernet.localIP());
}

int8_t sensorAmount(OneWire *ds) {
 SensorAddress sAddress;
 (ds)->reset_search();
 int8_t sensors = 0;
 while ((ds)->search(sAddress)) // получили адрес датчика    
  if ( (ds)->crc8(sAddress,7)==sAddress[7] ) {
   info=info+";+"; 
   for (int i=0;i<8;i++)
    info=info+String(sAddress[i])+":";
   info=info+";"; 
   sensors++;
  }
 return sensors;
}

void setup() {
 bigtimer=0;
 pinMode(RELAY_PIN, OUTPUT);
 digitalWrite(RELAY_PIN, HIGH); // на всякий случай сразу выключаем реле

 // start serial port:
 Serial.begin(9600);
 while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
 }

 Serial.print("free memory:"); Serial.println(freeMemory());

 ReadParamsFromEEPROM(); // загружаем параметры из ПЗУ (пока с инетом не соединились надо работать по старым настройкам
 on_delay=on_delay_default; // начинаем работать по настройкам. если в ПЗУ сбой - по настройкам из шапки программы.
 
 // при старте инициализируем ethernet и загружаем настройки с сервера
 delay(1000); // give the ethernet module time to boot up:
 initEthernet();
 GetParamsFromDB();

 // нужно узнать адреса всех подключенных датчиков
 info=""; 

 // переменные датчика температуры
 {
 SensorAddress sAddress;
 int8_t sensors = 0;
 //OneWire ds1(7); // Создаем объект OneWire для шины 1-Wire на 7-пине
 //OneWire ds2(8); // Создаем объект OneWire для шины 1-Wire на 8-пине
 //OneWire ds3(6); // Создаем объект OneWire для шины 1-Wire на 6-пине
 
 ds1.reset_search();
 while (ds1.search(sAddress)) // получили адрес датчика    
  if ( ds1.crc8(sAddress,7)==sAddress[7] ) {
   info=info+";+"; 
   for (int i=0;i<8;i++)
    info=info+String(sAddress[i])+":";
   info=info+";"; 
   sensors++;
  }
 info=info+"Wire1count:"+String(sensors);
 httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=2&info=",info , true);
 info="";
 delay(2000);

 ds2.reset_search();
 sensors = 0;
 while (ds2.search(sAddress)) // получили адрес датчика    
  if ( ds2.crc8(sAddress,7)==sAddress[7] ) {
   info=info+";+"; 
   for (int i=0;i<8;i++)
    info=info+String(sAddress[i])+":";
   info=info+";"; 
   sensors++;
  }
 info=info+"Wire2count:"+String(sensors);
 httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=2&info=",info , true);
 info="";
 delay(2000);

 ds3.reset_search();
 sensors = 0;
 while (ds3.search(sAddress)) // получили адрес датчика    
  if ( ds3.crc8(sAddress,7)==sAddress[7] ) {
   info=info+";+"; 
   for (int i=0;i<8;i++)
    info=info+String(sAddress[i])+":";
   info=info+";"; 
   sensors++;
  }
 info=info+"Wire3count:"+String(sensors);
 httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=2&info=",info , true  );
 info="";
 delay(2000);

 ds4.reset_search();
 sensors = 0;
 while (ds4.search(sAddress)) // получили адрес датчика    
  if ( ds4.crc8(sAddress,7)==sAddress[7] ) {
   info=info+";+"; 
   for (int i=0;i<8;i++)
    info=info+String(sAddress[i])+":";
   info=info+";"; 
   sensors++;
  }
 info=info+"Wire4count:"+String(sensors);
 httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=2&info=",info , true  );
 delay(2000);

 // проверяем, сколько символов помещается в буфер GET-запроса
 //delay(2000);
 //httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=",String(freeMemory()), true);
 //delay(2000);
 //httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=33&info=A","", true);
 //delay(2000);
 //httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=34","&info=A", true);
 //delay(2000);

 info="";
 delay(2000);

// info="";
// int SensorCount=sensorAmount(&ds1); Serial.print("Wire1 - total devices found: "+String(SensorCount)); info=info+"Wire1count:"+String(SensorCount);
// httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=2&info=",info , true);
// info="";
// delay(2000);
// SensorCount=sensorAmount(&ds2); Serial.print("Wire2 - total devices found: "+String(SensorCount)); info=info+"Wire2count:"+String(SensorCount);
// httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=2&info=",info , true);
// info="";
// delay(2000);
// SensorCount=sensorAmount(&ds3); Serial.print("Wire3 - total devices found: "+String(SensorCount)); info=info+"Wire3count:"+String(SensorCount);
// httpRequest("ard.php?sensor_count=-1&sensor_num=3&sensor_3=2&info=",info , true);
// info="";
// delay(2000);


 Serial.print("free memory:"); Serial.println(freeMemory());
 }
}

void loop() {
 // 1 - даём импульсы ШИМ на термопривод (реле через пин7)
 perSHIM(on_delay*1000,sum_delay*1000-on_delay*1000);
 // 2 - каждые X секунды измеряем температуру
 perReadTemperature(3000);
 // 3 - каждые X секунд отправляем температуру на сервер (для анализа лога)
 perSendTemp(30000);
 // 4 - каждые 5 минут переинициализируем ethernet (вдруг обрыв или глюки) и загружаем настройки с сервера (вдруг изменились?)
 perInitEthernet(300000);
 // 5 - каждую секунду вызываем функцию анализа и принятия решения
 perAnalyze(1000);
 //Serial.print("free memory:"); Serial.println(freeMemory());
}

void perSHIM(unsigned long interval_on,unsigned long interval_off){
 static unsigned long prevTime = 0; // время когда последний раз переключали реле
 unsigned long interval;
 if (relay_status) interval=interval_off; // реле включено, значит отсчитываем миллисекунды до выключения реле
 else interval=interval_on; // реле выключено, значит отсчитываем миллисекунды до включения реле
 if(millis() - prevTime > interval ) {
  Serial.println("--");
  prevTime = millis();  //
  if (relay_status)
   {
    Serial.println("off");
    //digitalWrite(LED_BUILTIN,HIGH); 
    digitalWrite(RELAY_PIN,LOW); 
   } 
  else
   {
    Serial.println("on");
    //digitalWrite(LED_BUILTIN,LOW); 
    digitalWrite(RELAY_PIN,HIGH); 
   } 
  relay_status=!relay_status; 
 }
} 

void perReadTemperature(unsigned long interval){
 static unsigned long prevTime = 0; // время когда последний раз читали температуру
 if(millis() - prevTime > interval ) {
  prevTime = millis();  //

  float temperature;
  int LowByte,HighByte,TReading,SignBit;

  ds1.reset(); // Теперь готовимся получить значение измеренной температуры
  ds1.write(0xCC); 
  ds1.write(0xBE); // Просим передать нам значение регистров со значением температуры
  // Получаем и считываем ответ
  temper_data[0]=ds1.read(); 
  temper_data[1]=ds1.read(); 
  //temperature =  ((temper_data[1] << 8) | temper_data[0]) * 0.0625;
  LowByte = temper_data[0];
  HighByte = temper_data[1];
  TReading = (HighByte << 8) | LowByte;
  SignBit = TReading & 0x8000;  // проверяем значение в самом старшем бите
  if (SignBit) // если значение отрицательное
   {
    TReading = (TReading ^ 0xffff) + 1; 
   }
  temperature=float(TReading)*0.0625;    // умножаем на (100 * 0.0625) или 6.25
  if (SignBit) // если значение отрицательное
   temperature=-temperature;
  //temperature=5;
  
  //LowByte = temper_data[0];
  //HighByte = temper_data[1];  
  //temperature =  ((temper_data[1] << 8) | temper_data[0]) * 0.0625;
  //TReading = (temper_data[1] << 8) + temper_data[0]; // склеиваем
  //SignBit = TReading & 0x8000;  // test most sig bit
  //if (SignBit) // negative
  // TReading = (TReading ^ 0xffff) + 1; // 2 comp
  //temperature = TReading * 0.0625;    // multiply by (100 * 0.0625) or 6.25  
  //if (SignBit) temperature=-temperature;
  // Формируем итоговое значение: 
  //    - сперва "склеиваем" значение, 
  //    - затем умножаем его на коэффициент, соответсвующий разрешающей способности (для 12 бит по умолчанию - это 0,0625)
  //float temperature =  ((temper_data[1] << 8) | temper_data[0]) * 0.0625;
  // Выводим полученное значение температуры в монитор порта
  //Serial.println("подача ТП:"+String(temperature));
  temp_now=temperature;

  ds2.reset(); // Теперь готовимся получить значение измеренной температуры
  ds2.write(0xCC); 
  ds2.write(0xBE); // Просим передать нам значение регистров со значением температуры
  temper_data[0]=ds2.read(); 
  temper_data[1]=ds2.read(); 
  LowByte = temper_data[0];
  HighByte = temper_data[1];
  TReading = (HighByte << 8) | LowByte;
  SignBit = TReading & 0x8000;  // проверяем значение в самом старшем бите
  if (SignBit) // если значение отрицательное
   {
    TReading = (TReading ^ 0xffff) + 1; 
   }
  temperature=float(TReading)*0.0625;    // умножаем на (100 * 0.0625) или 6.25
  if (SignBit) // если значение отрицательное
   temperature=temperature*-1;
  //temperature=5;
  temp_outdoor=temperature;


//; 40:255:64:139:33:23:4:48:;; 40:255:198:229:33:23:4:226:;; 40:255:153:141:33:23:3:23:;Wire3count:3
  SensorAddress sAddress;
  sAddress[0]=40; sAddress[1]=255; sAddress[2]=64; sAddress[3]=139; sAddress[4]=33; sAddress[5]=23; sAddress[6]=4; sAddress[7]=48;
  ds3.reset(); // Теперь готовимся получить значение измеренной температуры
  ds3.select(sAddress); 
  ds3.write(0xBE); // Просим передать нам значение регистров со значением температуры
  temper_data[0]=ds3.read(); 
  temper_data[1]=ds3.read(); 
  LowByte = temper_data[0]; HighByte = temper_data[1];
  TReading = (HighByte << 8) | LowByte;
  SignBit = TReading & 0x8000;  // проверяем значение в самом старшем бите
  if (SignBit) TReading = (TReading ^ 0xffff) + 1; // если значение отрицательное
  temperature=float(TReading)*0.0625;    // умножаем на (100 * 0.0625) или 6.25
  if (SignBit) temperature=-temperature; // если значение отрицательное   
  temp_home1=temperature;

  sAddress[0]=40; sAddress[1]=255; sAddress[2]=198; sAddress[3]=229; sAddress[4]=33; sAddress[5]=23; sAddress[6]=4; sAddress[7]=226;
  ds3.reset(); // Теперь готовимся получить значение измеренной температуры
  ds3.select(sAddress); 
  ds3.write(0xBE); // Просим передать нам значение регистров со значением температуры
  temper_data[0]=ds3.read(); 
  temper_data[1]=ds3.read(); 
  LowByte = temper_data[0]; HighByte = temper_data[1];
  TReading = (HighByte << 8) | LowByte;
  SignBit = TReading & 0x8000;  // проверяем значение в самом старшем бите
  if (SignBit) TReading = (TReading ^ 0xffff) + 1; // если значение отрицательное
  temperature=float(TReading)*0.0625;    // умножаем на (100 * 0.0625) или 6.25
  if (SignBit) temperature=-temperature; // если значение отрицательное   
  temp_home2=temperature;
  
  sAddress[0]=40; sAddress[1]=255; sAddress[2]=153; sAddress[3]=141; sAddress[4]=33; sAddress[5]=23; sAddress[6]=3; sAddress[7]=23;
  ds3.reset(); // Теперь готовимся получить значение измеренной температуры
  ds3.select(sAddress); 
  ds3.write(0xBE); // Просим передать нам значение регистров со значением температуры
  temper_data[0]=ds3.read(); 
  temper_data[1]=ds3.read(); 
  LowByte = temper_data[0]; HighByte = temper_data[1];
  TReading = (HighByte << 8) | LowByte;
  SignBit = TReading & 0x8000;  // проверяем значение в самом старшем бите
  if (SignBit) TReading = (TReading ^ 0xffff) + 1; // если значение отрицательное
  temperature=float(TReading)*0.0625;    // умножаем на (100 * 0.0625) или 6.25
  if (SignBit) temperature=-temperature; // если значение отрицательное   
  temp_home3=temperature;

  ds4.reset(); // Теперь готовимся получить значение измеренной температуры
  ds4.write(0xCC); 
  ds4.write(0xBE); // Просим передать нам значение регистров со значением температуры
  temper_data[0]=ds4.read(); 
  temper_data[1]=ds4.read(); 
  LowByte = temper_data[0]; HighByte = temper_data[1];
  TReading = (HighByte << 8) | LowByte;
  SignBit = TReading & 0x8000;  // проверяем значение в самом старшем бите
  if (SignBit) TReading = (TReading ^ 0xffff) + 1; // если значение отрицательное
  temperature=float(TReading)*0.0625;    // умножаем на (100 * 0.0625) или 6.25
  if (SignBit) temperature=-temperature; // если значение отрицательное   
  temp_home4=temperature;

  ds1.reset(); // Теперь готовимся получить значение измеренной температуры
  ds1.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
  ds1.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  ds2.reset(); // Теперь готовимся получить значение измеренной температуры
  ds2.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
  ds2.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  ds3.reset(); // Теперь готовимся получить значение измеренной температуры
  ds3.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. Пусть все датчики измеряют
  ds3.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  ds4.reset(); // Теперь готовимся получить значение измеренной температуры
  ds4.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. Пусть все датчики измеряют
  ds4.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  // Микросхема измеряет температуру, а мы пока занимаемся другими делами
 }
} 

void perSendTemp(unsigned long interval){
 static unsigned long prevTime = 0; // время когда последний раз 
 if(millis() - prevTime > interval ) {
  prevTime = millis();  //
  Serial.println("подключаемся к серверу");
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.print("GET /");
    client.print("ard.php?sensor_count=7");
    client.print("&sensor_1=");
    client.print(temp_now);
    client.print("&sensor_2=");
    client.print(on_delay);
    client.print("&sensor_3=");
    client.print(temp_outdoor);
    client.print("&sensor_4=");
    client.print(temp_home1);
    client.print("&sensor_5=");
    client.print(temp_home2);
    client.print("&sensor_6=");
    client.print(temp_home3);
    client.print("&sensor_7=");
    client.print(temp_home4);
    client.print("&info=");
    client.print(info);
    client.println(" HTTP/1.1");
    client.println("Host: <a href="http://www.xxxxxxx.ru" title="www.xxxxxxx.ru"" rel="nofollow">www.xxxxxxx.ru"</a>);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
  info="";
 }
} 

void perInitEthernet(unsigned long interval){
 static unsigned long prevTime = 0; // время когда последний раз 
 if(millis() - prevTime > interval ) {
  prevTime = millis();  //
  initEthernet();
  GetParamsFromDB();
 }
} 

void perAnalyze(unsigned long interval){
 static unsigned long prevTime = 0; // время когда последний раз 
 
 if(millis() - prevTime > interval ) {
  prevTime = millis();  //
  /////////////////////////////////////////////////////////////////
  // обрабатываем
  /////////////////////////////////////////////////////////////////
  bigtimer++;
 
  if (temp_old_time==0) {
   temp_old=temp_now; // через temp_delay секунд эта температура станет полезна
   temp_old_time=millis();
  }
  else if (millis()-temp_old_time>diff_delay*1000) { // этот момент настал
    temp_before2=temp_before1;
    temp_before1=temp_old;
    temp_old_time=0;
  }
  
  // управляем воздействием в зависимости от текущей температуры
    
  if (freeze_counter>0) { // между точками воздействия реагируем на аварийные ситуации 
   freeze_counter--;
   // вдруг мы пытаемся нагревать, а температура превысила уставку?
   // нужно срочно остудить на одну ступень, а не ждать кучу времени ($freeze_counter) пока перегреется
   // но только один раз
   if (temp_now>temp_ideal+temp_gist && target_direction==1 && alarm_frost==0) {
    target_direction=-1;
    grow_chance_count=0; // обнуляем счетчик шансов на прогрев (перегрелись, куда еще :)      
    alarm_frost++;
    on_delay=on_delay-on_delay_step;  
   } 

   if (hot_stopper<1 && temp_before2>temp_before1 && temp_before1>temp_now && temp_before2-temp_now>1.5) { // резкое падение нужно один раз замедлить
    on_delay=on_delay+on_delay_step;  
    hot_stopper++;
   } 

  } 
  else {
   // противоаварийный сброс настроек, если температура выше критической 4 минуты подряд
   if (temp_now>max_temp && temp_before1>max_temp && temp_before2>max_temp) 
    { on_delay=on_delay_default; }
   // если температура выше критической и on_delay на максимуме - сбрасываем настройки, что-то резко изменилось
   if (temp_now>max_temp && on_delay>=(max_on_delay-on_delay_step)) 
    { on_delay=on_delay_default; }

   // а если температура = установке $temp_ideal, тогда вообще ничего делать не нужно
   // тогда остановим $freeze_counter, пока температура не выйдет за пределы

   if (temp_now<(temp_ideal-temp_gist)) { // если температура ниже заданной
    need_grow_timer++; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
                     // если (таймер > 10 x temp_delay && on_delay < on_delay_default) тогда on_delay = on_delay_default 
                     // для случаев когда после сильного падения on_delay (например, до нуля) замучаешься ждать роста
                     // ведь с шагом 0.25 необходимо 12 / 0.25 * 2мин = 96 минут
    if (need_grow_timer > 10*temp_delay && on_delay < on_delay_default) on_delay=on_delay_default;
    // нужно сильнее открывать клапан (увеличить on_delay)
    hot_stopper=0; // обнуляем счетчик попыток замедлить падение температуры
    target_direction=1; 
    // минуточку, может быть температура уже растёт? зачем еще сильнее открывать клапан?
    if (temp_before2<temp_before1 && temp_before1<temp_now ) 
     {
      if (grow_chance_count<4) grow_chance_count++; // температура и так растет. даём шанс
      else if (temp_now-temp_before2>=1.5) //  шансы на рост уже исчерпаны. но вдруг температура уже растёт хорошими темпами?
       ; // растёт хорошо! 1.5 градуса за 4 минуты, будем ждать сколько потребуется!
      else // рост слишком медленный, увеличиваем ширину импульса на термопривод
       on_delay=on_delay+on_delay_step; 
     }
    else  
     {
      on_delay=on_delay+on_delay_step;
      // если до уставки осталось более 3 градусов - ускоряем разогрев (увеличим on_delay еще на ступень on_delay_step)
      if ( (temp_ideal-temp_gist)-temp_now > 3 ) on_delay=on_delay+on_delay_step;
     } 
    if (on_delay>max_on_delay) on_delay=max_on_delay; // не позволяем подавать питания на термопривод импульсами дольше чем max_on_delay

    freeze_counter=temp_delay*2; // термопривод долго реагирует, не торопимся
   }
   else if (temp_now>(temp_ideal+temp_gist)) { // нужно слабее открывать клапан (увеличить on_delay)
    target_direction=-1;
    need_grow_timer=0; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
    alarm_frost=0; // обнуляем счетчик аварийных охлаждений
    grow_chance_count=0; // обнуляем счетчик шансов на прогрев (перегрелись, куда еще :)
    // минуточку, может быть температура уже падает? зачем еще сильнее прикрывать клапан?
    if (temp_before2>temp_before1 && temp_before1>temp_now) // уже падает
     { // если падает и мы недалеко от целевой температуры - лучше немного подождать, чем вызвать резкое неконтролируемое падение
      if ( (temp_now-(temp_ideal+temp_gist))<4 ) { on_delay=on_delay; } // просто ждём 

      // хотя температура и падает, но у нас сильный перегрев (на 10 градусов) - нужно прикрыть клапан
      if ( (temp_before2-(temp_ideal+temp_gist))>10&&
           (temp_before1-(temp_ideal+temp_gist))>10&&
           (temp_now-(temp_ideal+temp_gist))>10 )
            { // горячее на 10 градусов уже 4 минуты
              // нужно дважды уменьшить ширину импульса
              if (on_delay<on_delay_step*3) on_delay=on_delay_step; else { on_delay=on_delay-2*on_delay_step; }
            }
     }
    else // температура не падает, уменьшаем ширину импульса на термопривод
     if (on_delay<on_delay_step*2) on_delay=on_delay_step; else { on_delay=on_delay-on_delay_step; }
    freeze_counter=temp_delay; 
   }
   else { // температура в пределах гистерезиса
    target_direction=0; // никуда не дергаемся, тепло, хорошо :)
    need_grow_timer=0; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
    alarm_frost=0; // обнуляем счетчик аварийных охлаждений
    grow_chance_count=0; // обнуляем счетчик шансов на прогрев (вышли на режим, всё норм)
    hot_stopper=0; // обнуляем счетчик попыток замедлить падение температуры
   }
  }

  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
 }
} 


