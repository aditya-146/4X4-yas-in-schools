


int light_sensor =  A0;
int led1 = 3;
int led2 = 4; 
float lrd ;
float res = 3300;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(light_sensor , INPUT);
pinMode(led1 , OUTPUT);
pinMode(led2 , OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:


lrd =  analogRead(light_sensor);
Serial.println(lrd);
if (lrd < 25){
  digitalWrite (led1 , 1);
  digitalWrite (led2 , 1);
  }
else {
 digitalWrite(led1 , 0);
 digitalWrite((led2 , 0); 
  } 
}
