// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3
#include <Keypad.h>               //Include Keypad Library
#include <ezButton.h>
#include <Stepper.h>
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // Change to (0x27,20,4) for 20x4 LCD.

ezButton limitSwitchI(10);  // create ezButton object that attach to pin 10;
ezButton limitSwitchF(13);  // create ezButton object that attach to pin 13;

int state;

const int dirPin = 11;
const int stepPin = 12;
float P;
long second = 0;
#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

const int ROWS= 4; //four rows
const int COLS = 3; //three columns

char customKey;
char keys[ROWS][COLS]= {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};   //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3}; //connect to the column pinouts of the keypad


int Input;

//Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 


String inputString;
long inputInt;
int cond=0;
//long Input;
//long userInput;



void setup()
{  
  Serial.begin(9600);
  //Input=MultiDIGIT();
  limitSwitchI.setDebounceTime(50); // set debounce time to 50 millisecond
  limitSwitchF.setDebounceTime(50); // set debounce time to 50 millisecond
  //Serial.println(String("##UserInp=")+ Input + "Watts");
  
  emon1.voltage(0, 562.5, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(1, 10.5);       // Current: input pin, calibration.
  inputString.reserve(10); // maximum number of digit for a number is 10, change if needed
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
}

void loop()
{
  while(cond==0){
    for(int x = 0; x < 5; x++) {
    digitalWrite(dirPin, LOW);
    // Set motor direction anti-clockwise
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
    Serial.println("Bwdctr");
    } 
    //state = limitSwitchI.getState();
    //limitSwitchI.loop(); // MUST call the loop() function first
    backrotation();
//    if(limitSwitchI.isPressed()){
//      cond=1;
//      Serial.println("Switch Closed");
//      //Serial.println("The limit switch: UNTOUCHED -> TOUCHED");
//    }
//
//    if(limitSwitchI.isReleased()){
//      //Serial.println("BACK ROTATION");
//      for(int x = 0; x < 5; x++) {
//        digitalWrite(dirPin, LOW);
//        // Set motor direction anti-clockwise
//        digitalWrite(stepPin, HIGH);
//        delayMicroseconds(2000);
//        digitalWrite(stepPin, LOW);
//        delayMicroseconds(2000);
//        Serial.println("BD");
//      }
//    }
  } 
  while(cond==1){
    Input=MultiDIGIT();
    Serial.println(String("##UserInp=")+ Input + "Watts");
  }
  while(cond==2){
    sensor();
    steppercontrol();
    limitSwitchI.loop(); // MUST call the loop() function first
    limitSwitchF.loop(); // MUST call the loop() function first
    if(limitSwitchF.isPressed()){
      cond=0;
      second=0;
      Serial.println("FSwitch Closed");
    }
    
  }
 
}
int sensor(){
  
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  float realPower       = emon1.realPower;        //extract Real Power into variable
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float Irms            = emon1.Irms;             //extract Irms into Variable

  P=supplyVoltage * Irms;
  
  Serial.println(String("Voltage=")+ supplyVoltage + "Voltage");
  Serial.println(String("Curent=")+ Irms + "Amp");
  
  Serial.println(String("Power=")+ P + "Watts");
  
  Serial.println(String("UserInp=")+ Input + "Watts");
  lcd.setCursor(0, 0); // Set the cursor on the third column and first row.
  lcd.print(String("UserInp=")+ Input + "Watts"); // Print the string "Hello World!"
  lcd.setCursor(0, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
  lcd.print(String("Power=")+ P + "Watts");
}
int MultiDIGIT()
{
  while( 1)
  {
    customKey = customKeypad.getKey();
    if(customKey >= '0' && customKey <= '9')
    {
      second = second * 10 + (customKey - '0');
      Serial.println(second);
    }

    if(customKey == '#'){
      cond=2;
      return second;
      break;  //return second;
    }
  }
}
//
void bwdctr(){
  for(int x = 0; x < 200; x++) {
    digitalWrite(dirPin, LOW);
    // Set motor direction anti-clockwise
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
    Serial.println("Bwdctr");
    } 
}
//

 //Stepper control begins
int steppercontrol()
{
  
  if(P < Input-1 ){
    //rotate stepper +ve i.e. clockwise
    for(int x = 0; x < 50; x++) {
    digitalWrite(dirPin, HIGH);
    // Set motor direction clockwise
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(10000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(10000);
  }
  }
  else if(P > Input+1 ){
    //rotate stepper -ve i.e. anti-clockwise
    for(int x = 0; x < 50; x++) {
    digitalWrite(dirPin, LOW);
    // Set motor direction anti-clockwise
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
  }
  
  delay(1000);
  }

int backrotation()
{
  //int state;
  limitSwitchI.loop(); // MUST call the loop() function first
  //Serial.println(String("State=")+ state);
  if(limitSwitchI.isPressed()){
    cond=1;
    Serial.println("Switch Closed");
    }
  if(limitSwitchI.isReleased()){
    bwdctr();
    Serial.println("BACK ROTATION");
    }
  return 0;
}
