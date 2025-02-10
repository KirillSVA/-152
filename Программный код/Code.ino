#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
Servo disp;

int lcd_num;
int ready;
int max;
int order[4]{ 0, 0, 0, 0 };
int orderone[4]{ 0, 0, 0, 0 };
const byte ROWS = 2;
const byte COLS = 2;
char hexaKeys[ROWS][COLS] = {
  { '0', '1' },
  { '2', '3' }
};
byte rowPins[ROWS] = { A0, A1 };  // цифровые выводы строк
byte colPins[COLS] = { A2, A3 };
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), colPins, rowPins, COLS, ROWS);

// Создаем объект класса LiquidCrystal_I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t pinSensor = 1;  //0,1,2                                        // Определяем номер вывода Arduino, к которому подключён датчик расхода воды.
uint8_t intSensor;
uint32_t varTime;        // Объявляем переменную для хранения времени последнего расчёта.
float varQ;              // Объявляем переменную для хранения рассчитанной скорости потока воды (л/с).
float varV;              // Объявляем переменную для хранения рассчитанного объема воды (л).
volatile uint16_t varF;  // Объявляем переменную для хранения частоты импульсов (Гц).
#define C0 3             //Clapan orange pin
#define C1 5             //Clapan gaze pin
#define C2 4             //Clapan mint pin
#define CS 6             //Clapan Micser pin
#define table_servo 7    //Пин жёлтого моToрчика на дискеы
#define DIS 8            //servo pin
#define ID1 9            //1 InfDat
#define ID2 10           //2 InfDat
#define ID3 11           //3 InfDat
#define ID4 12           //4 InfDat
#define IDO 13           //OUT InfDat

void Order(int n, int k) {
  order[n] = order[n] + k;
  if (order[n] == -1) { order[n] = 6; }
  if (order[n] == 7) { order[n] = 0; }
}
void LCD(int n, int k) {
  lcd.clear();
  lcd.setCursor(0, 0);  // Устанавливаем положение курсора, 0-й столбец, 0-я строка
  switch (n) {
    case 0: lcd.print("Press button to"); break;
    case 1: lcd.print("Glass 1"); break;
    case 2: lcd.print("Glass 2"); break;
    case 3: lcd.print("Glass 3"); break;
    case 4: lcd.print("Glass 4"); break;
    case 5: lcd.print("Start preparing"); break;
    case 6: lcd.print("    Progress    "); break;
    case 7: lcd.print("Issuing an order"); break;
    case 8: lcd.print("     Paused     "); break;
  }
  lcd.setCursor(0, 1);
  switch (k) {
    case -1: lcd.print("start! ^._.^"); break;
    case 0: lcd.print(" "); break;
    case 1: lcd.print("Soda water"); break;
    case 2: lcd.print("Mint syrup"); break;
    case 3: lcd.print("Orange juice"); break;
    case 4: lcd.print("Mint Lemonade"); break;
    case 5: lcd.print("Orange Lemonade"); break;
    case 6: lcd.print("Triple Lemonade"); break;
    case 7: lcd.print("your order"); break;
    case 8: lcd.print((String) "     " + ready + " of " + max + "     "); break;
    case 9: lcd.print(" Press to start "); break;
  }
}

void funCountInt() {
  varF++;
}  // Определяем функцию, которая будет приращать частоту импульсов.

void SensorPreparetion(uint8_t n) {
  pinSensor = n;
  pinMode(pinSensor, INPUT);
  intSensor = digitalPinToInterrupt(pinSensor);                     // Определяем номер прерывания который использует вывод pinSensor.
  attachInterrupt(intSensor, funCountInt, RISING);                  // Назначаем функцию funCountInt как обработчик прерываний intSensor при каждом выполнении условия RISING - переход от 0 к 1.
  if (intSensor < 0) { Serial.print("Указан вывод без EXT INT"); }  // Выводим сообщение о том, что датчик подключён к выводу не поддерживающему внешнее прерывание.
  varTime = 0;
  varQ = 0;
  varV = 0;
  varF = 0;  // Обнуляем все переменные.
}
void StartOut(int n) {
  Serial.begin(9600);
  bool stay = false;
  digitalWrite(CS, HIGH);
  disp.write(90);
  Serial.println(digitalRead(IDO));
  while (n > 0) {
    if (digitalRead(IDO) == 0) {
      if (stay == false) { n--; }
      stay = true;
      digitalWrite(table_servo, HIGH);
    } else {
      stay = false;
      digitalWrite(table_servo, LOW);
    }
  }
}


void Drow_new(int n) {
  if (n == 6) {
    delay(5000);
    digitalWrite(C0, LOW);
    delay(4800);
    digitalWrite(C0, HIGH);
    delay(5000);
    digitalWrite(C1, LOW);
    delay(4800);
    digitalWrite(C1, HIGH);
    delay(5000);
    digitalWrite(C2, LOW);
    delay(4800);
    digitalWrite(C2, HIGH);
    delay(3000);
    digitalWrite(CS, LOW);
    delay(4000);
    digitalWrite(CS, HIGH);
  }
}
int o;
int t = 280;  //325
int dT = 247;
void ToStartPos() {
  delay(1000);
  int l = 1;
  while (l == 1) {
    l = 1;
    for (int i = 0; i < 4; i++) {
      l = (l + digitalRead(IDO)) / 2;
    }
  }
  digitalWrite(table_servo, LOW);
  //delay(1848);
  l = 1;
  while (l == 1) {
    l = 1;
    for (int i = 0; i < 4; i++) {
      l = (l + digitalRead(ID2)) / 2;
    }
  }
  delay(130);
  digitalWrite(table_servo, HIGH);
}

void DelayOut(int p) {
  digitalWrite(table_servo, LOW);
  delay(t + p);
  digitalWrite(table_servo, HIGH);
  o = 0;
  delay(1000);
  while (o == 0) {
    o = 1;
    for (int i = 0; i < 5; i++) {
      o = (o + digitalRead(IDO)) / 2;
    }
  }
  t = dT;
}
void StartOut(int i1, int i2, int i3, int i4) {
  if (i1 == 0) {
    ready = 0;
    digitalWrite(table_servo, LOW);
    delay(800);
    digitalWrite(table_servo, HIGH);
    ready++;
    LCD(7, 8);
  } else {
    t += dT;
  }
  if (i2 == 0) {
    DelayOut(-30);
    ready++;
    LCD(7, 8);
  } else {
    t += dT;
  }
  if (i3 == 0) {
    DelayOut(-10);
    ready++;
    LCD(7, 8);
  } else {
    t += dT;
  }
  if (i4 == 0) {
    DelayOut(-15);
    ready++;
    LCD(7, 8);
  } else {
    t += dT;
  }

}

void Start() {
  max = 4;
  ready = 0;
  orderone[0] = 0;
  orderone[1] = 0;
  orderone[2] = 0;
  orderone[3] = 0;
  for (int i = 0; i < 4; i++) {
    if (order[i] == 0) {
      max--;
      orderone[i] = 1;
    }
  }
  LCD(6, 8);
  for (int i = 0; i < max; i++) {
    switch (i) {
      case 0: disp.write(125); break;
      case 1: disp.write(90); break;
      case 2: disp.write(55); break;
      case 3: disp.write(25); break;
    }
    delay(1000);
    Drow_new(order[i]);
    ready++;
    LCD(6, 8);
  }
  disp.write(90);
  StartOut(orderone[0], orderone[1], orderone[2], orderone[3]);
  LCD(0, -1);
  lcd_num = 0;
}


void setup() {                //
  Serial.begin(9600);         // Инициируем передачу данных в монитор последовательного порта.
  pinMode(pinSensor, INPUT);  // Конфигурируем вывод к которому подключён датчик, как вход.
  intSensor = digitalPinToInterrupt(pinSensor);
  pinMode(ID1, INPUT);
  pinMode(ID2, INPUT);
  pinMode(ID3, INPUT);
  pinMode(ID4, INPUT);
  pinMode(IDO, INPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C0, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(table_servo, OUTPUT);
  digitalWrite(table_servo, HIGH);
  digitalWrite(C1, HIGH);
  digitalWrite(C2, HIGH);
  digitalWrite(C0, HIGH);
  digitalWrite(CS, HIGH);
  disp.attach(DIS);
  disp.write(90);
  SensorPreparetion(1);

  lcd.init();       // Инициализация и начало работы с дисплеем
  lcd.backlight();  // Включаем подсветку
  LCD(0, -1);
  lcd_num = 0;

}  //
   //
void loop() {                              //
                                           //   Если прошла 1 секунда:                                    //
  char customKey = customKeypad.getKey();  // вывод значения кнопки
  if (customKey) {
    Serial.println(lcd_num);
    Serial.println(customKey);
    switch (lcd_num) {
      case 0:
        LCD(1, 0);
        lcd_num = 1;
        break;
      case 1:
        switch (customKey) {
          case '0':
            LCD(1, order[0]);
            lcd_num = 10;
            break;
          case '1':
            LCD(0, -1);
            lcd_num = 0;
            break;
          case '2':
            LCD(5, 7);
            lcd_num = 5;
            break;
          case '3':
            LCD(2, order[1]);
            lcd_num = 2;
            break;
        }
        break;
      case 2:
        switch (customKey) {
          case '0':
            LCD(2, order[1]);
            lcd_num = 20;
            break;
          case '1':
            LCD(0, -1);
            lcd_num = 0;
            break;
          case '2':
            LCD(1, order[0]);
            lcd_num = 1;
            break;
          case '3':
            LCD(3, order[2]);
            lcd_num = 3;
            break;
        }
        break;
      case 3:
        switch (customKey) {
          case '0':
            LCD(3, order[2]);
            lcd_num = 30;
            break;
          case '1':
            LCD(0, -1);
            lcd_num = 0;
            break;
          case '2':
            LCD(2, order[1]);
            lcd_num = 2;
            break;
          case '3':
            LCD(4, order[3]);
            lcd_num = 4;
            break;
        }
        break;
      case 4:
        switch (customKey) {
          case '0':
            LCD(4, order[3]);
            lcd_num = 40;
            break;
          case '1':
            LCD(0, -1);
            lcd_num = 0;
            break;
          case '2':
            LCD(3, order[2]);
            lcd_num = 3;
            break;
          case '3':
            LCD(5, 7);
            lcd_num = 5;
            break;
        }
        break;
      case 5:
        switch (customKey) {
          case '0':
            lcd_num = 6;
            Start();
            break;
          case '1':
            LCD(0, -1);
            lcd_num = 0;
            break;
          case '2':
            LCD(4, order[3]);
            lcd_num = 4;
            break;
          case '3':
            LCD(1, order[0]);
            lcd_num = 1;
            break;
        }
        break;

      case 10:
        switch (customKey) {
          case '0':
            LCD(1, order[0]);
            lcd_num = 1;
            break;
          case '1':
            LCD(1, order[0]);
            lcd_num = 1;
            break;
          case '2':
            Order(0, -1);
            LCD(1, order[0]);
            lcd_num = 10;
            break;
          case '3':
            Order(0, 1);
            LCD(1, order[0]);
            lcd_num = 10;
            break;
        }
        break;

      case 20:
        switch (customKey) {
          case '0':
            LCD(2, order[1]);
            lcd_num = 2;
            break;
          case '1':
            LCD(2, order[1]);
            lcd_num = 2;
            break;
          case '2':
            Order(1, -1);
            LCD(2, order[1]);
            lcd_num = 20;
            break;
          case '3':
            Order(1, 1);
            LCD(2, order[1]);
            lcd_num = 20;
            break;
        }
        break;

      case 30:
        switch (customKey) {
          case '0':
            LCD(3, order[2]);
            lcd_num = 3;
            break;
          case '1':
            LCD(3, order[2]);
            lcd_num = 3;
            break;
          case '2':
            Order(3, -1);
            LCD(3, order[2]);
            lcd_num = 30;
            break;
          case '3':
            Order(3, 1);
            LCD(3, order[2]);
            lcd_num = 30;
            break;
        }
        break;

      case 40:
        switch (customKey) {
          case '0':
            LCD(4, order[3]);
            lcd_num = 4;
            break;
          case '1':
            LCD(4, order[3]);
            lcd_num = 4;
            break;
          case '2':
            Order(4, -1);
            LCD(4, order[3]);
            lcd_num = 40;
            break;
          case '3':
            Order(4, 1);
            LCD(4, order[3]);
            lcd_num = 40;
            break;
        }
        break;
    }
  }
}


/*
bool test = true;
void loop(){
  if(test){
    Drow_new(0);
    test = false;
  }
}
*/