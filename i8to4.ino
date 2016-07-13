//8to4.ino
//v.0.0.2
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

* при потере питания и после его восстановлении произойдет опрос выводов 8, 9, 10, 11 и в соответствии с их состояниями будут выставлены состояния управляющих выводов 4, 5, 6, 7.

Предполагается работа с [Relay Shield](http://amperka.ru/product/arduino-relay-shield) и с
[Iskra Neo](http://wiki.amperka.ru/_media/%D0%BF%D1%80%D0%BE%D0%B4%D1%83%D0%BA%D1%82%D1%8B:iskra-neo:iskra_neo_pinout.png).

Для компиляции следует использовать [Arduino IDE от Arduino.org](http://www.arduino.org/downloads).

К выводам 0, 1, 2, 3 следует подсоединить моностабильные кнопки, которые обеспечат замыкание на GND.

К выводам 8, 9, 10, 11 следует подсоединить сухие контакты, которые также обеспечат замыкание на GND.

Скетч можно забрать прямым копипастом - [i8to4.ino](https://github.com/itechnocrat/i8to4/blob/master/i8to4.ino) или получить с [архивом](https://github.com/itechnocrat/i8to4/archive/master.zip), вверху зеленая кнопка Clone or download.
*/

#include <Bounce2.h>
// https://github.com/thomasfredericks/Bounce2/wiki

// #define input_1 0 // 0 пин - 1ый вход 1го канала
// #define input_2 1 // 1 пин - 1ый вход 2го канала
// #define input_3 2 // 2 пин - 1ый вход 3го канала
// #define input_4 3 // 3 пин - 1ый вход 4го канала
// #define input_5 8 // 8 пин - 2ый вход 1го канала
// #define input_6 9 // 9 пин - 2ый вход 2го канала
// #define input_7 10 // 10 пин - 2ый вход 3го канала
// #define input_8 11 // 11 пин - 2ый вход 4го канала

int input[8] = {0,1,2,3,8,9,10,11};

// #define out_1 4 // 4 пин - выход 1го канала
// #define out_2 5 // 5 пин - выход 2го канала
// #define out_3 6 // 6 пин - выход 3го канала
// #define out_4 7 // 7 пин - выход 4го канала

int out[4] = {4,5,6,7};

// #ifndef bool
// #define bool boolean
// #endif

// bool value_out_1; // значение выхода 1го канала
// bool value_out_2; // значение выхода 2го канала
// bool value_out_3; // значение выхода 3го канала
// bool value_out_4; // значение выхода 4го канала

bool value_out[4] = {0,0,0,0};

bool first_entry; // флаг первого прохождения кода

// создание экземпляров обработчика дребезга, по одному на вход:
// Bounce debouncer1 = Bounce();
// Bounce debouncer2 = Bounce();
// Bounce debouncer3 = Bounce();
// Bounce debouncer4 = Bounce();
// Bounce debouncer5 = Bounce();
// Bounce debouncer6 = Bounce();
// Bounce debouncer7 = Bounce();
// Bounce debouncer8 = Bounce();

  // назначение выходов:
  // pinMode(out_1, OUTPUT);
  // pinMode(out_2, OUTPUT);
  // pinMode(out_3, OUTPUT);
  // pinMode(out_4, OUTPUT);



  // pinMode(input_1, INPUT_PULLUP);
  // pinMode(input_2, INPUT_PULLUP);
  // pinMode(input_3, INPUT_PULLUP);
  // pinMode(input_4, INPUT_PULLUP);
  // pinMode(input_5, INPUT_PULLUP); // входы для выходов таймера:
  // pinMode(input_6, INPUT_PULLUP);
  // pinMode(input_7, INPUT_PULLUP);
  // pinMode(input_8, INPUT_PULLUP);

Bounce *ptr_debouncer[8];

void setup() {

  for(int j=0; j<4; j++){
    pinMode(out[j], OUTPUT);
  }

  for(int j=0; j<8; j++){
    pinMode(input[j], INPUT_PULLUP);
  }

  for(int j=0; j<8; j++){
    ptr_debouncer[j] = new  Bounce();
    (*ptr_debouncer[j]).attach(input[j]);
    (*ptr_debouncer[j]).interval(10);
  }

  // к каждому обработчику дребезга привязывается вход
  // debouncer1.attach(input_1); // входы для управляющих кнопок:
  // debouncer2.attach(input_2);
  // debouncer3.attach(input_3);
  // debouncer4.attach(input_4);
  // debouncer5.attach(input_5); // входы для выходов таймера:
  // debouncer6.attach(input_6);
  // debouncer7.attach(input_7);
  // debouncer8.attach(input_8);

  // установка значения для подавления дребезга
  // debouncer1.interval(10);
  // debouncer2.interval(10);
  // debouncer3.interval(10);
  // debouncer4.interval(10);
  // debouncer5.interval(10);
  // debouncer6.interval(10);
  // debouncer7.interval(10);
  // debouncer8.interval(10);

  first_entry = true; // если первый проход после старта, то надо один раз проверить состояние вторых входов каждого канала
}

void loop() {
// ------------------------------------------------ 1 канал -------------------------------------------------
// обработка 1 кнопки
//   if ( debouncer1.update() ) { // update() возвращает 1 если было ИЗМЕНЕНИЕ состояния входа
//       if ( !debouncer1.read() ) { // если ИЗМЕНЕНИЕ к нажатию, то
//          digitalWrite( out_1, revers(&value_out_1) ); // меняем состояние выхода на противоположное
//     }
//   }

// // обработка 1 выхода таймера
//   if ( debouncer5.update() || first_entry ) {
//     value_out_1 = !debouncer5.read();
//     digitalWrite( out_1, value_out_1 );
//   }
// // ---------------------------------------------------------------------------------------------------------------

// // ------------------------------------------------ 2 канал -------------------------------------------------
// // обработка 2 кнопки
//   if ( debouncer2.update() ) { 
//     if ( !debouncer2.read() ) {
//       digitalWrite( out_2, revers(&value_out_2) );
//      }
//    }

// // обработка 2 выхода таймера
//   if ( debouncer6.update() || first_entry ) {
//     value_out_2 = !debouncer6.read();
//     digitalWrite( out_2, value_out_2 );
//   }
// // ---------------------------------------------------------------------------------------------------------------

// // ------------------------------------------------ 3 канал -------------------------------------------------
// // обработка 3 кнопки
//   if ( debouncer3.update() ) {
//     if ( !debouncer3.read() ) {
//        digitalWrite( out_3, revers(&value_out_3) );
//      }
//    }

// // обработка 3 выхода таймера
//   if ( debouncer7.update() || first_entry ) {
//     value_out_3 = !debouncer7.read();
//     digitalWrite( out_3, value_out_3 );
//   }
// // ---------------------------------------------------------------------------------------------------------------

// // ------------------------------------------------ 4 канал -------------------------------------------------
// // обработка 4 кнопки
//   if ( debouncer4.update() ) {
//     if ( !debouncer4.read() ) {
//        digitalWrite( out_4, revers(&value_out_4) );
//      }
//    }

// // обработка 4 выхода таймера
//   if ( debouncer8.update() || first_entry ) {
//     value_out_4 = !debouncer8.read();
//     digitalWrite( out_4, value_out_4 );
//   }
// // ---------------------------------------------------------------------------------------------------------------

//   if(first_entry) first_entry = false;

// } //loop

// bool revers(bool *point2value_out){
//   return (*point2value_out) = !(*point2value_out);
}