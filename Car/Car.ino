int a1 = 2;
int a2 = 3;
int b1 = 5;
int b2 = 6;
int ena = 4;
int enb = 7;



void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode (ena , OUTPUT);
pinMode (enb , OUTPUT);
pinMode (a1 , OUTPUT);
pinMode (a2 , OUTPUT);
pinMode (b1 , OUTPUT);
pinMode (b2 , OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:


  


digitalWrite(ena , HIGH);
digitalWrite(enb , HIGH);
analogWrite(a1 , 0);
analogWrite(b1 , 0);
analogWrite(a2 , 0);
analogWrite(b2 , 0);

 
}
