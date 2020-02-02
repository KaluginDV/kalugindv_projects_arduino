// библиотека для работы с I²C хабом
#include <TroykaI2CHub.h>

// библиотека для работы с метеосенсором
#include <TroykaMeteoSensor.h>

// библиотека для работы с дисплеем
#include <LiquidCrystal.h>

// создаём объект для работы с датчиком
TroykaMeteoSensor meteoSensor;

// создаём объект для работы с хабом
// адрес по умолчанию 0x70
TroykaI2CHub splitter;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

float h_G, h_P; // переменные для влажности
float t_G, t_P; // переменные для температуры

void setup() {
  // открываем последовательный порт
  Serial.begin(115200);
  // ждём открытия порта
  while(!Serial) {
  }
  // печатаем сообщение об успешной инициализации Serial-порта
  Serial.println("Serial init OK");
  // начало работы с I²C хабом
  splitter.begin();
  Serial.println("Splitter init OK");

  // начало работы с датчиком
  meteoSensor.begin();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print(" - VAULT-TEC -");
  lcd.setCursor(0, 1);
  lcd.print("  Please wait...  "); 
  
  // ждём одну секунду
  delay(1000);
}

void loop() {
  lcd.clear();
  // счётчик цикла
  byte i;
  i = 0;
    // переключаем по очереди каналы
    splitter.setBusChannel(i);
    // выводим номер канала
    Serial.print("Set channel ");
    Serial.print(i);
    Serial.println(":");
    startMeteoScan(i);
    // печатаем о завершении процесса
    Serial.println("Done");
    // вывод на дисплей
    lcd.print("T:");
    lcd.print(t_G);
    lcd.print(" H:");
    lcd.print(h_G);

      i = 3;
    // переключаем по очереди каналы
    splitter.setBusChannel(i);
    // выводим номер канала
    Serial.print("Set channel ");
    Serial.print(i);
    Serial.println(":");
    startMeteoScan(i);
    // печатаем о завершении процесса
    Serial.println("Done");
    // вывод на дисплей
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(t_P);
    lcd.print(" H:");
    lcd.print(h_P);
    // ждём одну секунду
    delay(10000);

}

void startMeteoScan(int i) {
  // считываем данные с датчика
  int stateSensor = meteoSensor.read();
  // проверяем состояние данных
  switch (stateSensor) {
    case SHT_OK:
      // выводим показания влажности и температуры
      Serial.println("Data sensor is OK");
      Serial.print("Temperature = ");
      if (i == 0){
        t_G = meteoSensor.getTemperatureC();  
        Serial.print(t_G);
      }
      else{
        t_P = meteoSensor.getTemperatureC();
        Serial.print(t_P);
      }  
      Serial.println(" C \t");
      Serial.print("Temperature = ");
      Serial.print(meteoSensor.getTemperatureK());
      Serial.println(" K \t");
      Serial.print("Temperature = ");
      Serial.print(meteoSensor.getTemperatureF());
      Serial.println(" F \t");
      Serial.print("Humidity = ");
      if (i == 0){
        h_G = meteoSensor.getHumidity();   
        Serial.print(h_G);
      }
      else{
        h_P = meteoSensor.getHumidity(); 
        Serial.print(h_P);
      }         
      Serial.println(" %\r\n");
      break;
    // ошибка данных или сенсор не подключён
    case SHT_ERROR_DATA:
      Serial.println("Data error or sensor not connected");  
      if (i == 0){
        h_G = 0; 
        t_G = 0;  
      }
      else{
        h_P = 0;
        t_P = 0; 
      }       
      break; 
    // ошибка контрольной суммы
    case SHT_ERROR_CHECKSUM:
      Serial.println("Checksum error");
      break;
  }

}
