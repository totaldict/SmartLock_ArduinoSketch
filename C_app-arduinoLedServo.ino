const char StaPack = '#';              // Признак начала пакета данных
const char EndPack = '|';                                  // Признак окончания пакета данных
const uint16_t TimeOut = 500;                       // Отвалились через это время mls

#include <Servo.h>      //используем библиотеку для работы с сервоприводом
Servo servo;                  //объявляем переменную servo типа Servo

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  servo.attach(10);     //сервопривод на порту 10
  servo.write(0);
}

byte Degr (String StrControl) {         //0 : 255 (1 байт)

  StrControl.remove(0, 2);           //отрезаем управляющие символы (2шт)
  int i = StrControl.indexOf('|');   //определяем дилинну строки
  if (i == -1) return 0;                          //фигня пришла а не байт
  if (i > 2)   return 0;                             //фигня пришла а не байт
  StrControl.remove(i, 1);           //отрезаем управляющие символы ('|')

  int decBy=StrControl.toInt();     //перевод строки градусов в int
  
  return decBy;                         //возвращаем десятичное число
}

void loop() {
  delay(500);         //задержка 0.5 сек.
  Serial.write("ok\n");             //статус состояния постоянно отправляем что ОК

clearPack:                                // переходим сюда если приняли мусор /отвалились по таймауту/

  char   IncomChar;
  String StrControl = "";

  while (Serial.available() > 0) {            // выгребаем все байты которые пришли в буфер Serial

    IncomChar = Serial.read();

    if (IncomChar == StaPack) {           // пришел символ начала посылки данных
      StrControl += IncomChar;             // плюсуем пришедший символ к строке
      unsigned long currentTime = millis(); // записываем текущее время

      ReceptionPacket:                    // начало приема пакета

      if (Serial.available() > 0) {
        IncomChar = Serial.read();
        StrControl += IncomChar;          // плюсуем пришедший символ к строке
        if (IncomChar == EndPack) break;  // вываливаемся из цикла выгребания байт если приняли весь пакет
      }

      if (millis() - currentTime >= TimeOut) goto clearPack;  // отвалились по TimeOut

      goto ReceptionPacket;               // переход в начало приема пакета

    }

  }                                       // конец цикла по выгребанию байт

  if (StrControl != "") {

  //Serial.println(StrControl);           //печатаем принятый пакет отладка

  switch (StrControl.charAt(1)) {       //применяем полученую строку
    case 'w':
      digitalWrite(13, 0);
      break;
    case 'x':
      digitalWrite(13, 1);
      break;
    case 'A':
      servo.write(Degr(StrControl));
      break;
    case 'O':           //открывает на 5 сек замок, основной принцип открытия
      servo.write(90);
      delay(5000);
      servo.write(0);
      break;
  }
  }
}
