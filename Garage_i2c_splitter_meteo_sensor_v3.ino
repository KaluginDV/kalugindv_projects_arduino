#include <TroykaI2CHub.h>           // библиотека для работы с I²C хабом
#include <TroykaMeteoSensor.h>      // библиотека для работы с метеосенсором SHT
#include <LiquidCrystal_1602_RUS.h> // библиотека для работы с дисплеем (с поддержкой кирилицы)

TroykaMeteoSensor meteoSensor;      // создаём объект для работы с датчиком
TroykaI2CHub splitter;              // создаём объект для работы с хабом // адрес по умолчанию 0x70
LiquidCrystal_1602_RUS lcd(8, 9, 4, 5, 6, 7 );// инициализация дисплея (с поддержкой кирилицы)

float h_G, h_P;                     // переменные для влажности
float t_G, t_P;                     // переменные для температуры

void setup() {

  Serial.begin(115200);             // открываем последовательный порт
  while (!Serial) {                 // ждём открытия порта
  }
  Serial.println("Serial init OK"); // печатаем сообщение об успешной инициализации Serial-порта
  splitter.begin();                 // начало работы с I²C хабом

  Serial.println("Splitter init OK");
  meteoSensor.begin();              // начало работы с датчиком

  lcd.begin(16, 2);                 // установка параметров дисплея - 16 символов, 2 строки
  lcd.clear();

  // выводим приветствие на дисплей
  lcd.setCursor(3, 0);
  lcd.print(L"ГАРАЖНЫЙ");
  lcd.setCursor(3, 1);
  lcd.print(L"ТЕРМОМЕТР");

  delay(1000);                      // ждём одну секунду
}

void loop() {

  datchiki(); //переключаем релюшки в зависимости от показаний датчиков
 
  // вывод на дисплей
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t_G);
  lcd.print(" H:");
  lcd.print(h_G);

  // вывод на дисплей
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(t_P);
  lcd.print(" H:");
  lcd.print(h_P);

  // ждём одну секунду
  delay(1000);

}

void datchiki(void)
{
  // переключаем по очереди каналы
  splitter.setBusChannel(0);
  startMeteoScan(0);
  splitter.setBusChannel(3);
  startMeteoScan(3);
}

void startMeteoScan(int i) {
  // считываем данные с датчика
  int stateSensor = meteoSensor.read();
  // проверяем состояние данных
  switch (stateSensor) {
    case SHT_OK:
      if (i == 0) {
        t_G = meteoSensor.getTemperatureC();
      }
      else {
        t_P = meteoSensor.getTemperatureC();
      }
      if (i == 0) {
        h_G = meteoSensor.getHumidity();
      }
      else {
        h_P = meteoSensor.getHumidity();
      }
      break;
    // ошибка данных или сенсор не подключён
    case SHT_ERROR_DATA:
      Serial.println("Data error or sensor not connected");
      break;
    // ошибка контрольной суммы
    case SHT_ERROR_CHECKSUM:
      Serial.println("Checksum error");
      break;
  }

}
