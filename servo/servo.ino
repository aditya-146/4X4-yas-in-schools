#include <Servo.h>

int pos;
int angle;
int buz = 13;

Servo myservo;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

myservo.attach(9);
pinMode(buz ,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(buz , 1);





myservo.write(180);
delay(1000);
myservo.write(90);
delay(1000);
//myservo.write(0);
//delay(100);
}
 
     
