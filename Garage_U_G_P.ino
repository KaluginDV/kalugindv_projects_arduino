#include <Bounce.h>
#include <math.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>

#define DHTPIN_U 2 // DHT-11 с улицы
#define DHTPIN_G 3 // DHT-11 в гараже
#define DHTPIN_P 4 // DHT-11 в подвале
#define DL_18B20 5 // Dallas 18B20
#define VEN_U 6 // Включение - выключение вентилятора с улицы в гараж
#define VEN_G 7 // Включение - выключение вентилятора из гаража в подвал
#define VEN_P 8 // Включение - выключение вентилятора из подвала на улицу
#define OBOGREV 9 // Включение - выключение оборгева
#define BUT_LEFT 11 // Кнопка левая
#define BUT_RIGH 10 // Кнопка правая
#define BUZER 12 // Пищалка
#define DHTTYPE DHT11 // тип датчика влажности для библиотеки <DHT.h>

DHT dht_U(DHTPIN_U, DHTTYPE); //сделали датчик для улицы
DHT dht_G(DHTPIN_G, DHTTYPE); //сделали датчик для гаража
DHT dht_P(DHTPIN_P, DHTTYPE); //сделали датчик для подвала
OneWire ds(DL_18B20); // //сделали датчик для гаража калибровочный
Bounce bouncer_BUT_LEFT = Bounce( BUT_LEFT, 1); //Создали левую кнопку
Bounce bouncer_BUT_RIGH = Bounce( BUT_RIGH, 1); //Создали правую кнопку

LiquidCrystal_I2C lcd(0x27, 16, 2); //

int h_U, t_U , h_G ; // переменные для влажности
int t_G, h_P , t_P ; // переменные для температуры

int matrix_h_U[17]; int matrix_t_U[17]; //массивы для улицы
int matrix_h_G[17]; int matrix_t_G[17]; //массивы для гаража
int matrix_h_P[17]; int matrix_t_P[17]; //массивы для подваля

int secundy = -1; int i = 1; int tackt = 0;

const float a = 17.27;
const float b = 237.7;

double c_G, c_P, c_U, vl_P, vl_G;
int Tochka_Rosy_G, Tochka_Rosy_P, Tochka_Rosy_U;

//---------------------------------------------------------------------------------------------------------

void produvka(void)//принудительная продувка 600 сек по нажатию правой кнопки!
{
  lcd.clear();
  int door = 600;
  while (door != 0)
  {
    digitalWrite(VEN_U, LOW); digitalWrite(VEN_G, LOW); digitalWrite(VEN_P, LOW); digitalWrite(OBOGREV, LOW);
    lcd.setCursor(0, 0); lcd.print("PRODUVKA 600 Sec");
    lcd.setCursor(0, 1); lcd.print("OcTalocb "); lcd.print(door); lcd.print(" Sec");
    delay(1000);
    door--;
  }
  digitalWrite(VEN_U, HIGH); digitalWrite(VEN_G, HIGH); digitalWrite(VEN_P, HIGH); digitalWrite(OBOGREV, HIGH);
  lcd.clear();
}

//-----------------------------------------------------------------------------------------------------------------------------

void matrix_init (void) //заполяем массивы нулями
{ int i_matrix = 0;
  for (i_matrix = 0; i_matrix < 60; i_matrix++)
  {
    matrix_h_U[i_matrix] = 0; matrix_t_U[i_matrix] = 0;
    matrix_h_G[i_matrix] = 0; matrix_t_G[i_matrix] = 0;
    matrix_h_P[i_matrix] = 0; matrix_t_P[i_matrix] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------------

void datchiki(void)//принятие решения на вкл или выкл релюшек
{
  t_U = dht_U.readTemperature() - 14;
  h_U = dht_U.readHumidity() - 2;
  t_G = dht_G.readTemperature() - 1;
  h_G = dht_G.readHumidity();
  t_P = dht_P.readTemperature();
  h_P = dht_P.readHumidity();
  
  vl_G = double(h_G) / 100;
  vl_P = double(h_P) / 100;

  c_G = ((a * t_G) / (b + t_G)) + log(vl_G);
  Tochka_Rosy_G = int((b * c_G) / (a - c_G));
  c_P = ((a * t_P) / (b + t_P)) + log(vl_P);
  Tochka_Rosy_P = int((b * c_P) / (a - c_P));

  int Tochka_Rosy_G_min = Tochka_Rosy_G - 2;
  int Tochka_Rosy_G_max = Tochka_Rosy_G + 2;
  int Tochka_Rosy_P_min = Tochka_Rosy_P - 2;
  int Tochka_Rosy_P_max = Tochka_Rosy_P + 2;

  if (t_U < 0) {
    digitalWrite(VEN_U, HIGH);
  }
  else
  {
    if (t_G > Tochka_Rosy_G_min && t_G < Tochka_Rosy_G_max) digitalWrite(VEN_U, HIGH); else
    {
      if (h_U > 50) digitalWrite(VEN_U, HIGH); else digitalWrite(VEN_U, LOW);
    };
  };
  if (t_G < 0) {
    digitalWrite(VEN_G, HIGH);
  }
  else
  {
    if (t_P > Tochka_Rosy_P_min && t_P < Tochka_Rosy_P_max) digitalWrite(VEN_P, HIGH); else
    {
      if (h_G > 50) digitalWrite(VEN_G, HIGH); else digitalWrite(VEN_G, LOW);
    };
  };
  if (t_P < 0) digitalWrite(VEN_P, HIGH);
  else
  {
    if (h_P > 50) digitalWrite(VEN_P, LOW); else digitalWrite(VEN_P, HIGH);
  };
  if (t_P < 0) digitalWrite(OBOGREV, LOW);
  else
  {
    if (h_P > 60) digitalWrite(OBOGREV, LOW); else digitalWrite(OBOGREV, HIGH);
  };
}

//--------------------------------------------------------------------------------------------------------------------------------

void svode (void) // сводная таблица с температурами и влажностями со всех трех DHT
{
  //-------------------------
  t_U = dht_U.readTemperature() - 14;
  h_U = dht_U.readHumidity() - 2;
  
  lcd.setCursor(0, 0); lcd.print("T:="); lcd.print(t_U); lcd.print(" ");
  lcd.setCursor(0, 1); lcd.print("H:="); lcd.print(h_U); lcd.print(" ");
  //-------------------------
  t_G = dht_G.readTemperature() - 1;
  h_G = dht_G.readHumidity();

  lcd.setCursor(5, 0); lcd.print("="); lcd.print(t_G); lcd.print(" ");
  lcd.setCursor(5, 1); lcd.print("="); lcd.print(h_G); lcd.print(" ");
  //--------------------------
  t_P = dht_P.readTemperature();
  h_P = dht_P.readHumidity();

  lcd.setCursor(8, 0); lcd.print("="); lcd.print(t_P); lcd.print("Tg="); lcd.print(Tochka_Rosy_G); lcd.print(" ");
  lcd.setCursor(8, 1); lcd.print("="); lcd.print(h_P); lcd.print("Tp="); lcd.print(Tochka_Rosy_P); lcd.print(" ");
  //--------------------------
  delay(5000); lcd.clear(); //рисуется 5 сек и перетирает экран
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void menu_U (void)//получение влажности и температуры с улици
{
  t_U = dht_U.readTemperature() - 14;
  h_U = dht_U.readHumidity() - 2;
  lcd.setCursor(0, 0); lcd.print("OUTDOR:H="); lcd.print(h_U); lcd.print(" T="); lcd.print(t_U);

  int a = 0;
  for (-1; a < 16; a++)
  { lcd.setCursor(a, 1);
    int s = (matrix_h_U[a] / 10);
    switch (s)
    { case 0: lcd.print("\0") ; break;
      case 1: lcd.print("\1") ; break;
      case 2: lcd.print("\1") ; break;
      case 3: lcd.print("\2") ; break;
      case 4: lcd.print("\3") ; break;
      case 5: lcd.print("\4") ; break;
      case 6: lcd.print("\5") ; break;
      case 7: lcd.print("\6") ; break;
      case 8: lcd.print("\7") ; break;
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void menu_G(void)//получение влажности и температуры из гаража
{
  t_G = dht_G.readTemperature() - 1;
  h_G = dht_G.readHumidity();

  lcd.setCursor(0, 0); lcd.print("GARAGE:H="); lcd.print(h_G); lcd.print(" T="); lcd.print(t_G);

  int a = 0;
  for (-1; a < 16; a++)
  { lcd.setCursor(a, 1);
    int s = int(matrix_h_G[a] / 10);
    switch (s)
    { case 0: lcd.print("\0") ; break;
      case 1: lcd.print("\1") ; break;
      case 2: lcd.print("\1") ; break;
      case 3: lcd.print("\2") ; break;
      case 4: lcd.print("\3") ; break;
      case 5: lcd.print("\4") ; break;
      case 6: lcd.print("\5") ; break;
      case 7: lcd.print("\6") ; break;
      case 8: lcd.print("\7") ; break;
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void menu_P (void) //получение влажности и температуры из подвала
{
  t_P = dht_P.readTemperature();
  h_P = dht_P.readHumidity();

  lcd.setCursor(0, 0); lcd.print("BASEME:H="); lcd.print(h_P); lcd.print(" T="); lcd.print(t_P);

  int a = 0;
  for (-1; a < 16; a++)
  {
    lcd.setCursor(a, 1);
    int s = int(matrix_h_P[a] / 10);
    switch (s) {
      case 0: lcd.print("\0") ; break;
      case 1: lcd.print("\1") ; break;
      case 2: lcd.print("\1") ; break;
      case 3: lcd.print("\2") ; break;
      case 4: lcd.print("\3") ; break;
      case 5: lcd.print("\4") ; break;
      case 6: lcd.print("\5") ; break;
      case 7: lcd.print("\6") ; break;
      case 8: lcd.print("\7") ; break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

void opros_datchikov (void)
{
  //------------------------------------
  lcd.setCursor(0, 0); lcd.print("T:");
  t_U = dht_U.readTemperature() - 14;
  h_U = dht_U.readHumidity() - 2;

  if (isnan(t_U) || isnan(h_U)) {
    lcd.print("Fail");
  }  else {
    lcd.print(t_U);
    lcd.setCursor(0, 1);
    lcd.print("H:");
    lcd.print(h_U);
  } ;
  //-----------------------------------
  lcd.setCursor(4, 0); lcd.print("\4");
  t_G = dht_G.readTemperature() - 1;
  h_G = dht_G.readHumidity();

  if (isnan(t_G) || isnan(h_G)) {
    lcd.print("Fail");
  }  else {
    lcd.print(t_G);
    lcd.setCursor(4, 1);
    lcd.print("\4");
    lcd.print(h_G);
  } ;
  //-----------------------------------
  lcd.setCursor(7, 0); lcd.print("\4");
  t_P = dht_P.readTemperature();
  h_P = dht_P.readHumidity();

  if (isnan(t_P) || isnan(h_P)) {
    lcd.print("Fail");
  }  else {
    lcd.print(t_P);
    lcd.setCursor(7, 1);
    lcd.print("\4");
    lcd.print(h_P);
  } ;
  //------------------------------------
  byte i; byte present = 0; byte type_s; byte data[12]; byte addr[8]; float celsius;
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  switch (addr[0]) {
    case 0x10: type_s = 1; break;  case 0x28:  type_s = 0; break;  case 0x22:  type_s = 0; break; return;
  } ds.reset(); ds.select(addr); ds.write(0x44, 1); delay(1000);
  present = ds.reset(); ds.select(addr); ds.write(0xBE);

  for ( i = 0; i < 9; i++) {
    data = ds.read();
  } int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3;
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  else if (cfg == 0x20) raw = raw & ~3; else if (cfg == 0x40) raw = raw & ~1;
  }
  celsius = (float)raw / 16.0;
  lcd.setCursor(10, 0); lcd.print("\4"); lcd.print(celsius);
  lcd.setCursor(10, 1); lcd.print("\4"); lcd.print("\6"); lcd.print("\4"); lcd.print("\2"); lcd.print("\5"); lcd.print("\7");
  //---------------------------------------
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup (void)
{
  dht_U.begin(); delay(1000);
  dht_G.begin(); delay(1000);
  dht_P.begin(); delay(1000);

  lcd.init();
  lcd.backlight(); delay(200);
  lcd.noBacklight();

  pinMode(VEN_U, OUTPUT); digitalWrite(VEN_U, HIGH);
  pinMode(VEN_G, OUTPUT); digitalWrite(VEN_G, HIGH);
  pinMode(VEN_P, OUTPUT); digitalWrite(VEN_P, HIGH);
  pinMode(OBOGREV, OUTPUT); digitalWrite(OBOGREV, HIGH);
  pinMode(BUT_LEFT, INPUT);
  pinMode(BUT_RIGH, INPUT);

  matrix_init();//заполняем нулями

  byte linia_0 [8] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111,}; lcd.createChar(0, linia_0);
  byte linia_15[8] = { B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111,}; lcd.createChar(1, linia_15);
  byte linia_30[8] = { B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111,}; lcd.createChar(2, linia_30);
  byte linia_45[8] = { B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111,}; lcd.createChar(3, linia_45);
  byte linia_60[8] = { B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111,}; lcd.createChar(4, linia_60);
  byte linia_75[8] = { B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111,}; lcd.createChar(5, linia_75);
  byte linia_90[8] = { B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111,}; lcd.createChar(6, linia_90);
  byte linia_100[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111,}; lcd.createChar(7, linia_100);

  opros_datchikov();
  delay(4000);

  lcd.clear();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop (void)//вечный цикл в петле
{
  datchiki(); //переключаем релюшки в зависимости от показаний датчиков
  bouncer_BUT_LEFT.update ( ); int value_BUT_LEFT = bouncer_BUT_LEFT.read();
  bouncer_BUT_RIGH.update ( ); int value_BUT_RIGH = bouncer_BUT_RIGH.read();
  if ( value_BUT_LEFT == HIGH) {
    i++;
    if (i > 4) i = 1;
  }
  if ( value_BUT_RIGH == HIGH) {
    produvka();
  } else {
    delay(1);
  }
  switch (i)
  {
    case 1: menu_U() ; break;
    case 2: menu_G() ; break;
    case 3: menu_P() ; break;
    case 4: {
        svode() ;
        delay(5000);
        i = 1;
      } break;
  }
  tackt++;

  //запись в массивы температуры и влажности
  if (tackt >= 115) // время между циклами записи в массивы порядка 4 минут
  { tackt = 0; secundy++; if (secundy > 15)secundy = 0;
    matrix_h_U[secundy] = h_U; matrix_t_U[secundy] = t_U;
    matrix_h_G[secundy] = h_G; matrix_t_G[secundy] = t_G;
    matrix_h_P[secundy] = h_P; matrix_t_P[secundy] = t_P;
    tone(12, 2000, 150);
  } // Попискивание при записи очередных элементов массива */
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
