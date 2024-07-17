int trig = 8;
int echo = 7;

float speed1 = 0.034; //in cm/micro second
float time1;
int distance;
int pos = 0;
int delay1 = 0.01;
int buzzer = 2;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(trig ,OUTPUT);
pinMode(echo , INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(trig , LOW);
delay (delay1);
digitalWrite(trig , HIGH);
delay (delay1);
digitalWrite(trig , LOW);

time1 = pulseIn (echo , HIGH);   //in microsec
time1 = time1/2;

distance = speed1 * time1;
if (distance <= 12 and distance > 5 ){
  tone(buzzer)
  }
else if (distance <= 5){
  tone(buzzer)
  }

}
