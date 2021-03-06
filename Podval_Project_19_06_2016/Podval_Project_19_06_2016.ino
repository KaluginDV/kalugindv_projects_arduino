//На данный момент работают только два термпературных датчика
//В планах добавить реле на включение провертривания помещения в зависимости от разницы тимператур
//добавить третий уличный датчик температуры
// добавить датчик влажности для подвала

// Подключаем библитеотеку для подлючения датчиков на одной шине
#include <OneWire.h>
// Подключаем библиотеку работы с температурными датчиками
#include <DallasTemperature.h>
// Обязательно подключаем стандартную библиотеку LiquidCrystal
//#include <LiquidCrystal.h>
#include <LiquidCrystal_1602_RUS.h>

OneWire oneWire(2); // вход датчиков 18b20
DallasTemperature ds(&oneWire);
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal_1602_RUS lcd(8, 9, 4, 5, 6, 7 );

DeviceAddress sensor1 = {0x28, 0x55, 0x58, 0x8, 0x0, 0x0, 0x80, 0xE0};
DeviceAddress sensor2 = {0x28, 0xFF, 0x55, 0x90, 0x81, 0x15, 0x3, 0xD6};

float minTemp1;
float maxTemp1;
float minTemp2;
float maxTemp2;

float sens1temp;
float sens2temp;

void setup() {

  ds.begin();
  lcd.begin(16, 2);
  lcd.clear();

  ds.requestTemperatures();
  minTemp1 = ds.getTempC(sensor1);
  maxTemp1 = ds.getTempC(sensor1);
  minTemp2 = ds.getTempC(sensor2);
  maxTemp2 = ds.getTempC(sensor2);
  lcd.setCursor(2, 0);
  lcd.print(L"ГАРАЖНЫЙ");

  delay(100);

  lcd.setCursor(4, 1);
  lcd.print(L"ТЕРМОМЕТР");

  delay(5000);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(L"ХОРОШЕГО");
  delay(100);
  lcd.setCursor(4, 1);
  lcd.print(L"ВАМ ДНЯ!");

  delay(5000);

}

void loop() {

  ds.requestTemperatures(); // считываем температуру с датчиков

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(L"ГАРАЖ");
  lcd.print(":   ");
  sens1temp = ds.getTempC(sensor1); // отправляем температуру
  lcd.print(sens1temp);
  lcd.print("C");

  delay(100);

  lcd.setCursor(0, 1);
  lcd.print(L"ПОДВАЛ");
  lcd.print(":  ");
  sens2temp = ds.getTempC(sensor2); // отправляем температуру
  lcd.print(sens2temp);
  lcd.print("C");

  if (sens1temp >= maxTemp1) {
    maxTemp1 = sens1temp;
  }

  if (sens2temp >= maxTemp2) {
    maxTemp2 = sens2temp;
  }

  if (sens1temp < minTemp1 && sens1temp > -127) {
    minTemp1 = sens1temp;
  }

  if (sens2temp < minTemp2 && sens1temp > -127) {
    minTemp2 = sens2temp;
  }

  delay(10000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(L"МАКСИМУМ");

  delay(100);

  lcd.setCursor(0, 1);
  lcd.print(L"Г");
  lcd.print(":");
  lcd.print(maxTemp1);
  lcd.print(" ");

  delay(100);

  lcd.print(L"П");
  lcd.print(":");
  lcd.print(maxTemp2);

  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(L"МИНИМУМ");

  delay(100);

  lcd.setCursor(0, 1);
  lcd.print(L"Г");
  lcd.print(":");
  lcd.print(minTemp1);
  lcd.print(" ");

  delay(100);

  lcd.print(L"П");
  lcd.print(":");
  lcd.print(minTemp2);

  delay(5000);

  Time();

}

//Подсчет времени наработки после включения
void Time()
{
  for (int i = 0; i < 60; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(L"ВРЕМЯ РАБОТЫ");
    lcd.setCursor(0, 1);
    int time = millis() / 1000;
    if (time / 60 / 60 < 10) {
      lcd.print ("0");
    }
    lcd.print (time / 60 / 60);
    lcd.print (":");
    if (time / 60 % 60 < 10) {
      lcd.print ("0");
    }
    lcd.print ((time / 60) % 60);
    lcd.print (":");
    if (time % 60 < 10) {
      lcd.print ("0");
    }
    lcd.print (time % 60);
    delay(100);
  }
  //delay(3000);
}
