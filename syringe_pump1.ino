#include <Arduino.h>
#include <EasyDriver.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
//AH_EasyDriver(int RES, int DIR, int STEP);
// RES -> RESOLUTION per full revolve
// DIR -> DIRECTION PIN
// STEP -> STEPPING PIN
int dirPin = 5;                  // output pin for stepper motor direction
int stepPin = 3;                 // output pin for the pin used to step the motor
int hallSensorPin = 7;
int state =0;
int rePin = 4;
int forPin = 6;
int interrupt = 2;
int forVal, reVal;
AH_EasyDriver stepper(200,dirPin,stepPin);   // Initialization
const double pi = 3.14159;
double r, t_vol, ff, s, etime;
float rpm, rev;
char input;
 //true==backward 
 //false== forward

  /*****************LCD****************/
 const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
long starttime=0;
String med;
int runTime;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
  /************************************/
void setup() {
  /*****************LCD****************/
  Serial.begin(9600);
  start();
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Auto-Syringe");
  delay(300);
  lcd.setCursor(2,1);
  lcd.print("ENGR 359");
  delay(300);
  lcd.clear();
  /************************************/
  /***********Syringe Pump*************/
  pinMode(hallSensorPin, INPUT);
//  stepper.setSpeedRPM(20);          // RPM , rotations per minute
//  stepper.setSpeedHz(100);           // Hz, steps per second
  pinMode(forPin, INPUT_PULLUP);
  pinMode(rePin, INPUT_PULLUP);
  pinMode(interrupt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallSensorPin),stopcode,CHANGE);
  /************************************/
}
void loop() {
  /***********Syringe Pump*************/
  state = digitalRead(hallSensorPin);
  reVal = digitalRead(rePin);
  forVal = digitalRead(forPin);
  input = Serial.read();
  if(state==0){
    
  }
  if(input=='i'){
    UserInput();
  }
  if(input=='g'){
    UserGo();
  }
  if(reVal == 0){
    stepper.setSpeedRPM(150);
    stepper.stepMotor(true, 10);
  }
  if(forVal == 0){
   stepper.setSpeedRPM(100);
   stepper.stepMotor(false, 10);
  }
  if(input == 'f' && state == 1){
    forward(3.443);
  }
  
  /************************************/
  /*****************LCD****************/
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print("H:");
  // print the number of seconds since reset:
  lcd.print(millis() / 3600000);

  lcd.setCursor(5, 1);
  lcd.print(" M:");
  lcd.print(millis()/60000);

  float seconds = (millis()-starttime)%60000;
  lcd.setCursor(10,1);
  lcd.print("  S:");
  lcd.print(seconds);

  lcd.setCursor(0,0);
  lcd.print("DRUG CODE: ------");
  /************************************/
}

  /****************FUNCTIONS********************/
void start(){
  Serial.println("=================Hi=================");
}
void forward(float rot){
  starttime=millis();
  stepper.setSpeedRPM(8);
   s=micros();
  //revolve(float, bool)
  stepper.revolve (rot,false);             //true==backward false== forward
  etime = micros()-s;
  Serial.println((etime/1000000));
  //delay(10000000);
}
void setForward(int rot){
  stepper.setSpeedRPM(40);
  stepper.revolve (rot,false);
}
void stopcode()
{
  stepper.disableDriver();
}
void UserInput(){
  /*************USERINPUT****************/
  Serial.print("MED!?: ");
  while(Serial.available()==0){}
  med = Serial.readString();
  Serial.print("MED: "); Serial.println(med);
  Serial.print("Enter radius(mm): ");
  while(Serial.available()==0){}
  r=Serial.parseFloat(); Serial.println();
  Serial.print("radius: "); Serial.println(r);

  Serial.print("Enter Flow Rate(ul/min): ");
  while(Serial.available()==0){}
  ff=Serial.parseFloat(); Serial.println();
  Serial.print("Flow Rate: "); Serial.println(ff);

  Serial.print("Enter Totol Volume(ul^3(ml)): ");
  while(Serial.available()==0){}
  t_vol=Serial.parseFloat(); Serial.println();
  Serial.print("Total Volume: "); Serial.println(t_vol);
  rpm = ff/(pi*(pow(r,2)));
  rev = t_vol/(4*pi*(pow(r,2)));
  runTime = t_vol/ff;
  /************************************/
}
void UserGo(){
  Serial.println("GO");
  lcd.setCursor(0, 1);
  lcd.print("H:");
  // print the number of seconds since reset:
  lcd.print(millis() / 3600000);

  lcd.setCursor(5, 1);
  lcd.print(" M:");
  lcd.print("0");

  float seconds = (millis()-starttime)%60000;
  lcd.setCursor(10,1);
  lcd.print("  S:");
  lcd.print(runTime);

  lcd.setCursor(0,0);
  lcd.print("DRUG CODE:");
  lcd.print(med);
  
  stepper.setSpeedRPM(rpm);
  stepper.revolve (rev,false);
}

