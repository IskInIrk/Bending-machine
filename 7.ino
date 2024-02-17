#include "GyverEncoder.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ENCODER_2_CLK_PIN 19
#define ENCODER_2_DT_PIN 18
#define ENCODER_1_CLK_PIN 2   
#define ENCODER_1_DT_PIN 3

#include "GyverButton.h"

#define START_BUTTON_PIN 4
#define STOP_BUTTON_PIN 5
#define KONSTOP_BUTTON_PIN 6
#define KONUP_BUTTON_PIN 8

GButton butt1(START_BUTTON_PIN);
GButton butt2(STOP_BUTTON_PIN);
GButton butt3(KONSTOP_BUTTON_PIN);
GButton butt4(KONUP_BUTTON_PIN);

#define START_RELAY_PIN 9
#define STOP_RELAY_PIN 10
#define UP_RELAY_PIN 11
#define DOWN_RELAY_PIN 12


Encoder enc1(ENCODER_1_CLK_PIN, ENCODER_1_DT_PIN, ENC_NO_BUTTON, TYPE2);
Encoder enc2(ENCODER_2_CLK_PIN, ENCODER_2_DT_PIN, ENC_NO_BUTTON, TYPE2);

volatile int enc1Value = 0;
volatile int enc2Value = 0;
volatile int y = 0;
volatile int value1; 
volatile int value2=1;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Angle:");
  lcd.print(enc1Value);
  attachInterrupt(digitalPinToInterrupt(ENCODER_1_CLK_PIN), isrEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_1_DT_PIN), isrEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2_CLK_PIN), isrEncoder2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2_DT_PIN), isrEncoder2, CHANGE);
  pinMode(START_RELAY_PIN, OUTPUT);
  pinMode(STOP_RELAY_PIN, OUTPUT);
  pinMode(UP_RELAY_PIN, OUTPUT);
  pinMode(DOWN_RELAY_PIN, OUTPUT);

  digitalWrite(START_RELAY_PIN, HIGH);
  digitalWrite(STOP_RELAY_PIN, HIGH);
  digitalWrite(UP_RELAY_PIN, HIGH);
  digitalWrite(DOWN_RELAY_PIN, HIGH);

  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(KONSTOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(KONUP_BUTTON_PIN, INPUT_PULLUP);
  butt1.setTickMode(AUTO);
  butt2.setTickMode(AUTO);
  butt3.setTickMode(AUTO);
  butt4.setTickMode(AUTO);

}

void loop() {
  updateDisplay();
  startButtonPressed();
  stopButtonPressed();
  konstopButtonPressed();
  
}

void isrEncoder1() {
  enc1.tick();
  if (enc1.isRight()) {
    enc1Value++;                // Увеличиваем значение при повороте направо
    y = (enc1Value + 1)*3.4  ;  // Пересчитываем z
  }
  if (enc1.isLeft()) {
    if (enc1Value > 0) {          // Проверяем, что значение больше нуля перед уменьшением
      enc1Value--;                // Уменьшаем значение при повороте налево, если оно больше нуля
      y = (enc1Value - 1)*3.4 ;  // Пересчитываем z

    }
  }
}

void isrEncoder2() {
  enc2.tick();
  if (enc2.isTurn())
   {
    enc2Value++;  // Увеличиваем значение enc2Value при каждом обороте энкодера
    if (digitalRead(START_RELAY_PIN) == LOW && enc2Value == y) {
      digitalWrite(START_RELAY_PIN, HIGH);
      digitalWrite(STOP_RELAY_PIN, LOW);
      enc2Value = 0;
    }
    if (digitalRead(STOP_RELAY_PIN) == LOW && enc2Value == y) {
      digitalWrite(STOP_RELAY_PIN, HIGH);
      enc2Value = 0;
    }
    else if (digitalRead(DOWN_RELAY_PIN) == LOW  && butt4.isHold() == false ){
    digitalWrite(DOWN_RELAY_PIN, HIGH);
    }
    else if (digitalRead(STOP_RELAY_PIN) == HIGH && digitalRead(START_RELAY_PIN) == HIGH && butt4.isHold() == true ){
    digitalWrite(DOWN_RELAY_PIN, LOW);
    value2=0;
    
   }
  }

  
}

void updateDisplay() {
   lcd.setCursor(0, 0);
  lcd.setCursor(0, 1);
  lcd.print("Angle:");
  lcd.setCursor(6, 1);

 char buffer[5];                      // буфер для хранения строки, включая завершающий символ null
 sprintf(buffer, "%03d", enc1Value);  // преобразуем число в строку с фиксированной длиной в 5 символов
 lcd.setCursor(6, 1);                 // Устанавливаем курсор в позицию (6, 1)
 lcd.print(buffer);                   // Печатаем значение enc11
}

void startButtonPressed() {
  if (butt1.isClick() && enc1Value > 1 && butt4.isHold() == true) {
    digitalWrite(START_RELAY_PIN, LOW);
    enc2Value = 0;
  }
  isrEncoder2();
}

void konstopButtonPressed() {
  if (butt3.isHold() == true ){
  digitalWrite(STOP_RELAY_PIN, HIGH);
  enc2Value = 0;
  }
}

void stopButtonPressed() {

if (butt2.isHold() == true && butt4.isHold() == true && value2== 0 ) {//
  digitalWrite(UP_RELAY_PIN, HIGH);
  digitalWrite(DOWN_RELAY_PIN, HIGH);
  value1=1;
} 
else if  (butt2.isRelease()  ) {
  if (value2==1){
    value2=0;
  }
  else if (value2 ==0){
    value2=1;
  }
 }
else if (butt2.isHold() == true && value2 == 1  ) {//
  digitalWrite(UP_RELAY_PIN, HIGH);
  digitalWrite(DOWN_RELAY_PIN, LOW);

} 
else if (butt2.isHold() == true && value2 == 0 ) {//
  digitalWrite(UP_RELAY_PIN, LOW);
  digitalWrite(DOWN_RELAY_PIN, HIGH);
} 

else  if (butt2.isHold() == false ) {//Все выключено 
  
  digitalWrite(UP_RELAY_PIN, HIGH);
  digitalWrite(DOWN_RELAY_PIN, HIGH);

} 
}
