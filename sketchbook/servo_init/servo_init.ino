#include "Arduino.h"
#include <Servo.h>

Servo myservo;
Servo yourservo;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(9);
  yourservo.attach(6);
}

void loop() {
  // put your main code here, to run repeatedly:

}
