#include <Servo.h>


Servo myservo;
int val=0;
void setup() {
  // put your setup code here, to run once:
  myservo.attach(9);

}

void loop() {
  delay(30);
  if(val == 90)
  {
    val =0;
  }
  // put your main code here, to run repeatedly:
  myservo.write(val);
  if(val == 0)
  {
    delay(5000);
  }
  
  val++; 
  


}
