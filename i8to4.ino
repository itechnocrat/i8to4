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
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, output OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// http://licenseit.ru/wiki/index.php/MIT_License
/**
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
// #ifndef bool
// #define bool boolean
// #endif

int input[] = {0,1,2,3,8,9,10,11};
int output[] = {4,5,6,7};
int j;
bool first_entry; // флаг первого прохождения кода
bool value_input;
Bounce *ptr_debouncer[8];

void setup() {

  for(int j=0; j<4; j++){
    pinMode(output[j], OUTPUT);
  }

  for(int j=0; j<8; j++){
    pinMode(input[j], INPUT_PULLUP);
  }

  for(int j=0; j<8; j++){
    ptr_debouncer[j] = new  Bounce(); // наплодим восемь экземпляров обработчика
    (*ptr_debouncer[j]).attach(input[j]); // привяжем каждый на один вход
    (*ptr_debouncer[j]).interval(10); // зададим параметр для подавления дребезга
  }

  first_entry = true; // если первый проход после старта, то надо один раз проверить состояние вторых входов каждого канала

  j = 0;
}

void loop() {

  if( (*ptr_debouncer[j]).update() || first_entry ) {
    value_input = (*ptr_debouncer[j]).read();
    if ( j < 4 ) {
      if ( !value_input ) {
        digitalWrite( output[j], !digitalRead( output[j] ) );
      }
    }
      if ( j > 3 ) {
        digitalWrite( output[j-4], !value_input );
    }
  }
  
  if ( j < 7 ) j ++;
  else {
    j = 0;
    if (first_entry) {
      first_entry = false;
    }
  }

} //loop
