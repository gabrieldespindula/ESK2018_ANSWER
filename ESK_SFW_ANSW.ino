//Emirates Skills 2018, example program
#include <LiquidCrystal.h>
#include "BasicStepperDriver.h"
//Emirates Skills 2018, example program
#include <LiquidCrystal.h>
#include "BasicStepperDriver.h"
#include "Ultrasonic.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 120

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 8

// All the wires needed for full functionality
#define DIR 12
#define STEP 13

//buzzer definition
#define BUZZ  A1

// define some values used by the panel and buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//define ultrasonic
#define TRIGPIN 2
#define ECHOPIN 3 

//define states
#define WAITING 0
#define SELECTION_AUTO  1
#define SELECTION_MANU  2
#define ULTRA_TEST  3
#define MOTOR_TEST  4
#define MANUAL_MODE 5
#define AUTO_MODE 6

// select the motor pins
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
//ultrsonic start
Ultrasonic ultrasonic(TRIGPIN,ECHOPIN);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

int distance;
int set=10;
int state=ULTRA_TEST;
float angle;
long timer;
void setup()
{
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
 lcd.print("Push the buttons"); // print a simple message
 stepper.begin(RPM, MICROSTEPS);
 pinMode(BUZZ, OUTPUT);
 Serial.begin(9600);

  lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
  lcd.print("  ANSWER CODE   ");
  lcd.setCursor(0,1);
  lcd.print(" E-SKILLS 2018 ");
  delay(2000);
}

int motor_steps=0;
int motor_dir=0;
int step_move(){
  if(motor_dir) motor_steps++;
  else motor_steps--;
  if(motor_steps<0 || motor_steps>200) motor_dir=!motor_dir;
  return (motor_dir*2)-1;
}

void auto_screen(){
  lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
  lcd.print("AUTO   ");
  lcd.print(angle);
  lcd.print("deg  ");
  lcd.setCursor(0,1);
  lcd.print("DIST:");
  if(distance<50){
    lcd.print(distance);
  }
  else{
    lcd.print("OVRNG  ");
  }
  lcd.print(" SET:");
  lcd.print(set);
  lcd.print("   ");
  if(distance<set && distance>0)  digitalWrite(BUZZ, HIGH);
  else  digitalWrite(BUZZ, LOW);
}
void manual_screen(){
  lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
  lcd.print("MANUAL  ");
  lcd.print(angle);
  lcd.print("deg  ");
  lcd.setCursor(0,1);
  lcd.setCursor(0,1);
  lcd.print("DIST:");
  if(distance<50){
    lcd.print(distance);
  }
  else{
    lcd.print("OVRNG  ");
  }
  lcd.print(" SET:");
  lcd.print(set);
  lcd.print("   ");
  if(distance<set && distance>0)  digitalWrite(BUZZ, HIGH);
  else  digitalWrite(BUZZ, LOW);
}
void loop()
{
    distance = ultrasonic.Ranging(CM);
    angle=0.225*motor_steps;
    switch(state){
      case ULTRA_TEST:{
        lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
        lcd.print("ULTRASONIC TEST ");
        lcd.setCursor(0,1);
        lcd.print("Distance: ");
        if(distance<50){
          lcd.print(distance);
          lcd.print("cm  ");
        }
        else{
          lcd.print("OVRNG  ");
        }
        if(distance<10 && distance>0)  digitalWrite(BUZZ, HIGH);
        else  digitalWrite(BUZZ, LOW);
        if(read_LCD_buttons()!=btnNONE){
          state = MOTOR_TEST;
          delay(10);
          while(read_LCD_buttons()!=btnNONE);
        }
        break;  
      }
      case MOTOR_TEST:{
        if(motor_steps<200)  stepper.move(step_move());
        lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
        lcd.print("   MOTOR TEST   ");
        lcd.setCursor(0,1);
        lcd.print("                ");
        if(read_LCD_buttons()!=btnNONE && motor_steps==200){
          state = WAITING;
          delay(10);
          while(read_LCD_buttons()!=btnNONE);
        }
        break;
      }
      case WAITING:  {
        lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
        lcd.print("EMIRATES SKILLS ");
        lcd.setCursor(0,1);
        lcd.print("     RADAR      "); 
        if(read_LCD_buttons()==btnSELECT){
          state = SELECTION_AUTO;
          timer=millis();
          delay(10);
          while(read_LCD_buttons()==btnSELECT);
        }
        break;
      }
      case SELECTION_AUTO:{
        lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
        lcd.print("-> AUTO SCAN    ");
        lcd.setCursor(0,1);
        lcd.print("   MANUAL SCAN  "); 
        if(read_LCD_buttons()==btnSELECT){
          state = AUTO_MODE;
          set=10;
        }
        if(read_LCD_buttons()==btnDOWN){
          state = SELECTION_MANU;
          timer=millis();
        }
        if(timer+3000<millis()){
          state = WAITING;
        }
        delay(10);
        while(read_LCD_buttons()!=btnNONE);
        break;
      }
      case SELECTION_MANU:{
        lcd.setCursor(0,0);            // move cursor to second line "1" and 9 spaces over
        lcd.print("   AUTO SCAN    ");
        lcd.setCursor(0,1);
        lcd.print("-> MANUAL SCAN  "); 
        if(read_LCD_buttons()==btnSELECT){
          state = MANUAL_MODE;
          set=10;
        }
        if(read_LCD_buttons()==btnUP){
          state = SELECTION_AUTO;
          timer=millis();
        }
        if(timer+3000<millis()){
          state = WAITING;
        }
        delay(10);
        while(read_LCD_buttons()!=btnNONE);
        break;
      }
      case MANUAL_MODE:{
        manual_screen(); 
        if(read_LCD_buttons()==btnUP){
          set++;
          if(set>35)  set=35;
        }
        if(read_LCD_buttons()==btnDOWN){
          set--;
          if(set<5)  set=5;
        }
        if(read_LCD_buttons()==btnSELECT){
          state = SELECTION_MANU;
          timer=millis();
        }
        if(read_LCD_buttons()==btnLEFT){
          motor_steps++;
          if(motor_steps>200) motor_steps=200;
          else stepper.move(1);
          delay(100);
        }
        if(read_LCD_buttons()==btnRIGHT ){
          motor_steps--;
          if(motor_steps<0) motor_steps=0;
          else stepper.move(-1);
          delay(100);
        }
        delay(10);
        while(read_LCD_buttons()!=btnNONE && read_LCD_buttons()!=btnRIGHT && read_LCD_buttons()!=btnLEFT);
        break;
      }
      case AUTO_MODE:{
        stepper.move(step_move());
        auto_screen(); 
        if(read_LCD_buttons()==btnUP){
          set++;
          if(set>35)  set=35;
        }
        if(read_LCD_buttons()==btnDOWN){
          set--;
          if(set<5)  set=5;
        }
        if(read_LCD_buttons()==btnSELECT){
          state = SELECTION_AUTO;
          timer=millis();
        }
        delay(10);
        while(read_LCD_buttons()!=btnNONE);
        break;
      }
    }
}

