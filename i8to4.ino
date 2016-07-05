//8to4.ino
/*
Copyright (c) 2016 Ivan Prokofyev

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// http://licenseit.ru/wiki/index.php/MIT_License
/*
- 8 входов по 2 на канал.
- 4 выхода на каждый канал.
- в канале:

1.  первый вход канала управляется моностабильной кнопкой, при нажатии на вход подается минус, после этого состояние выхода канала меняется на противоположный.

2.  второй вход канала управляется нормально разомкнутым сухим контактом, состояние второго входа копируется на выход канала.

* входы одного канала не конкурируют между собой.

* предусмотрен ручной режим, переход происходит при удержании нажатой любой кнопки в течении 2 сек (регулируется в коде, при изменении значения необходима новая компиляция и загрузка кода в МК), возврат в смешанный режим по такому же удержанию кнопки.

* если питание будет потеряно во время работы МК в ручном режиме, то после перезапуска его выводы 4, 5, 6, 7 будут выставлены в состояния, которые были до отключения питания, ручной режим работы будет продолжен.

* если питание будет потеряно в смешанном режиме, то при его восстановлении произойдет опрос выводов 8, 9, 10, 11 и в соответствии с их состояниями будут выставлены состояния управляющих выводов 4, 5, 6, 7.

Предполагается работа с [Relay Shield](http://amperka.ru/product/arduino-relay-shield) и с
[Iskra Neo](http://wiki.amperka.ru/_media/%D0%BF%D1%80%D0%BE%D0%B4%D1%83%D0%BA%D1%82%D1%8B:iskra-neo:iskra_neo_pinout.png).

Для компиляции следует использовать [Arduino IDE от Arduino.org](http://www.arduino.org/downloads).

Библиотеку EEPROM от Arduino.org заменить на такую же от Arduino.сс, просто в каталоге установки Arduino IDE от Arduino.сс найти эту библиотеку, скопировать и вставить взамен такой же, в каталоге установки Arduino IDE от Arduino.org.

К выводам 0, 1, 2, 3 следует подсоединить моностабильные кнопки, которые обеспечат замыкание на GND.

К выводам 8, 9, 10, 11 следует подсоединить сухие контакты, которые также обеспечат замыкание на GND.

Скетч можно забрать прямым копипастом - [8to4.ino](https://github.com/itechnocrat/8to4/blob/master/8to4.ino) или забрать [архив](https://github.com/itechnocrat/8to4/archive/master.zip), вверху зеленая кнопка Clone or download.

Проблема:  
Не реализована возможность опроса выводов 8, 9, 10, 11 сразу после возврата из ручного режима.
*/

#include <Bounce2.h>
// https://github.com/thomasfredericks/Bounce2/wiki

#include <PMButton.h>
// https://github.com/JCWentzel/PolymorphicButtons

#include <EEPROM.h>
// https://www.arduino.cc/en/Reference/EEPROM

#define input_1 0 // 0 пин - вход 1го канала
#define input_2 1 // 1 пин - вход 2го канала
#define input_3 2 // 2 пин - вход 3го канала
#define input_4 3 // 3 пин - вход 4го канала
#define input_5 8 // 8 пин - вход 5го канала
#define input_6 9 // 9 пин - вход 6го канала
#define input_7 10 // 10 пин - вход 7го канала
#define input_8 11 // 11 пин - вход 8го канала

#define out_1 4 // 4 пин - выход 1го канала
#define out_2 5 // 5 пин - выход 2го канала
#define out_3 6 // 6 пин - выход 3го канала
#define out_4 7 // 7 пин - выход 4го канала

#ifndef bool
#define bool boolean
#endif

bool value_out_1; // значение выхода 1го канала
bool value_out_2; // значение выхода 2го канала
bool value_out_3; // значение выхода 3го канала
bool value_out_4; // значение выхода 4го канала

bool first_entry; // флаг первого прохождения кода

bool manual_mode = false;

byte all_state; // для упакованного значения состояний

// создание экземпляров обработчика дребезга, по одному на вход:
Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();
Bounce debouncer3 = Bounce();
Bounce debouncer4 = Bounce();
Bounce debouncer5 = Bounce();
Bounce debouncer6 = Bounce();
Bounce debouncer7 = Bounce();
Bounce debouncer8 = Bounce();

// делаем кнопки многофункциональными
PMButton button1(input_1);
PMButton button2(input_2);
PMButton button3(input_3);
PMButton button4(input_4);

void setup() {
  // назначение выходов:
  pinMode(out_1, OUTPUT);
  pinMode(out_2, OUTPUT);
  pinMode(out_3, OUTPUT);
  pinMode(out_4, OUTPUT);

  // установка режимов работы выводов как входы
  button1.begin();
  button2.begin();
  button3.begin();
  button4.begin();
  button1.holdTime(2000); // длительность нажатия для перехода в ручной режим и обратно ms

  // pinMode(input_1, INPUT_PULLUP);
  // pinMode(input_2, INPUT_PULLUP);
  // pinMode(input_3, INPUT_PULLUP);
  // pinMode(input_4, INPUT_PULLUP);
  pinMode(input_5, INPUT_PULLUP); // входы для выходов таймера:
  pinMode(input_6, INPUT_PULLUP);
  pinMode(input_7, INPUT_PULLUP);
  pinMode(input_8, INPUT_PULLUP);

  // к каждому обработчику дребезга привязывается вход
  debouncer1.attach(input_1); // входы для управляющих кнопок:
  debouncer2.attach(input_2);
  debouncer3.attach(input_3);
  debouncer4.attach(input_4);
  debouncer5.attach(input_5); // входы для выходов таймера:
  debouncer6.attach(input_6);
  debouncer7.attach(input_7);
  debouncer8.attach(input_8);

  // установка значения для подавления дребезга
  debouncer1.interval(10);
  debouncer2.interval(10);
  debouncer3.interval(10);
  debouncer4.interval(10);
  debouncer5.interval(10);
  debouncer6.interval(10);
  debouncer7.interval(10);
  debouncer8.interval(10);

  // считать первый байт EEPROM,
  // распаковать и разложить по переменным
  all_state = EEPROM.read(0);
  value_out_4 = bitRead(all_state, 0);
  value_out_3 = bitRead(all_state, 1);
  value_out_2 = bitRead(all_state, 2);
  value_out_1 = bitRead(all_state, 3);
  manual_mode = bitRead(all_state, 4);

  if(manual_mode){
    digitalWrite(out_1, value_out_1);
    digitalWrite(out_2, value_out_2);
    digitalWrite(out_3, value_out_3);
    digitalWrite(out_4, value_out_4);
  }

  first_entry = true; // если первый проход после старта, то надо один раз проверить состояние вторых входов каждого канала
}

void loop() {
// ------------------------------------------------ 1 канал -------------------------------------------------
// обработка 1 кнопки
  if ( debouncer1.update() ) { // update() возвращает 1 если было ИЗМЕНЕНИЕ состояния входа
      if ( !debouncer1.read() ) { // если ИЗМЕНЕНИЕ к нажатию, то
         digitalWrite( out_1, revers(&value_out_1) ); // меняем состояние выхода на противоположное
    }
  }

  button1.checkSwitch();
  if( button1.held() ){
    manual_mode = !manual_mode;
  }

// обработка 1 выхода таймера
  if ( (debouncer5.update() || first_entry) && !manual_mode) {
    value_out_1 = !debouncer5.read();
    digitalWrite( out_1, value_out_1 );
  }
// ---------------------------------------------------------------------------------------------------------------

// ------------------------------------------------ 2 канал -------------------------------------------------
// обработка 2 кнопки
  if ( debouncer2.update() ) { 
    if ( !debouncer2.read() ) {
      digitalWrite( out_2, revers(&value_out_2) );
     }
   }

  button2.checkSwitch();
  if( button2.held() ){
    manual_mode = !manual_mode;
  }

// обработка 2 выхода таймера
  if ( (debouncer6.update() || first_entry) && !manual_mode) {
    value_out_2 = !debouncer6.read();
    digitalWrite( out_2, value_out_2 );
  }
// ---------------------------------------------------------------------------------------------------------------

// ------------------------------------------------ 3 канал -------------------------------------------------
// обработка 3 кнопки
  if ( debouncer3.update() ) {
    if ( !debouncer3.read() ) {
       digitalWrite( out_3, revers(&value_out_3) );
     }
   }

  button3.checkSwitch();
  if( button3.held() ){
    manual_mode = !manual_mode;
  }

// обработка 3 выхода таймера
  if ( (debouncer7.update() || first_entry) && !manual_mode) {
    value_out_3 = !debouncer7.read();
    digitalWrite( out_3, value_out_3 );
  }
// ---------------------------------------------------------------------------------------------------------------

// ------------------------------------------------ 4 канал -------------------------------------------------
// обработка 4 кнопки
  if ( debouncer4.update() ) {
    if ( !debouncer4.read() ) {
       digitalWrite( out_4, revers(&value_out_4) );
     }
   }

  button4.checkSwitch();
  if( button4.held() ){
    manual_mode = !manual_mode;
  }

// обработка 4 выхода таймера
  if ( (debouncer8.update() || first_entry) && !manual_mode) {
    value_out_4 = !debouncer8.read();
    digitalWrite( out_4, value_out_4 );
  }
// ---------------------------------------------------------------------------------------------------------------

  // упаковать значения выходов,
  // если значение отличное от уже имеющегося в EEPROM, то
  // записать значение в EEPROM
  all_state = (manual_mode << 4) | (value_out_1 << 3) | (value_out_2 << 2) | (value_out_3 << 1) | value_out_4;
  EEPROM.update(0, all_state);

  if(first_entry) first_entry = false;

} //loop

bool revers(bool *point2value_out){
  return (*point2value_out) = !(*point2value_out);
}