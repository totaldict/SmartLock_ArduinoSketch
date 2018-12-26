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

byte hexToByte (String StrControlHex) {         //0 : 255 (1 байт)

  uint8_t  HEX16 = 0;   // число 16-е из символа
  uint8_t  exp16 = 1;   // степень числа 16
  uint8_t  decBy = 0;   // число 10-е расчитанное без знака

  StrControlHex.remove(0, 2);           //отрезаем управляющие символы (2шт)
  int i = StrControlHex.indexOf('|');   //определяем дилнну строки
  if (i == -1) return 0;                          //фигня пришла а не байт
  if (i > 2)   return 0;                             //фигня пришла а не байт
  StrControlHex.remove(i, 1);           //отрезаем управляющие символы ('|')

  for (int j = StrControlHex.length() - 1; j >= 0; j--) {

    HEX16 = StrControlHex.charAt(j);

    if (HEX16 >= 48 && HEX16 <= 57) HEX16 = map(HEX16, 48, 57, 0, 9);
    if (HEX16 >= 65 && HEX16 <= 70) HEX16 = map(HEX16, 65, 70, 10, 15);
    if (HEX16 >= 97 && HEX16 <= 102) HEX16 = map(HEX16, 97, 102, 10, 15);

    decBy = decBy + HEX16 * exp16;
    exp16 = exp16 * 16;

  }
  return decBy;                         //возвращаем десятичное число 1 байт без знака
}

void loop() {
  delay(500);         //задержка 0.5 сек.
  Serial.write("ok\n");

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
      servo.write(hexToByte(StrControl));
      break;
  }
  }
}
