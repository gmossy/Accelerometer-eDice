// Arduino Dice
// by Glenn Mossy Dec 13, 2018
// thingiverse: 
// github:  http://www.github.com/gmossy
// add interupts http://gammon.com.au/interrupts
/* Thingiverse case:
 * https://www.thingiverse.com/thing:972220
 */
//Dice functions "sides" derived from edice @ thing:972220.
//Some parts derived from Gus @ pimylifeup.com
//Accelerometer code derived from http://crackeconcept.blogspot.com/2014/11/using-accelerometer-with-ardiuno-with.html
//
// for the MMA7361 Accelerometer
// https://moderndevice.com/product/3-axis-accelerometer-module/
#include <math.h>  
#include <EEPROM.h>   // EEPROM is used to store the calibrated values for X,Y,Z.  Run the calibration sketch on each different Arduino board. 
//
//Pin to to turn dice on & off, or roll the die
// assumes a pull-down button, normally low.
int buttonPin   = 2;
//intialize pushbutton button1State
int button1State     = 0;
int lastButton1State = 0;

//piezo buzzer pin
int buzzPin     = 10;

//LED pin assignments for DICE 
int bottomLeft  = 3;
int middleLeft  = 4;
int upperLeft   = 5;
int middle      = 6;
int bottomRight = 7;
int middleRight = 8;
int upperRight  = 9;

//Pin assignments for accelerometer
const int X_Pin = 0;  // arduino adc pin for x 
const int Y_Pin = 1;  // arduino adc pin for y 
const int Z_Pin = 2;  // arduino adc pin for z 

//delcare an array to store values read from the accelerometer
// Global Variables
// for axis values x, y and z, and tilt
float tiltx, tilty, tiltz;
float AccelerometerArray[7];
float AccelerometerArray2[7];
// Configure the die shaking behavior
int Accelerometer_state = 0;   // 0 is no shaking, 1 is light shaking,  2 is heavy shaking.
int shakeTimeMin = 50;        // initialize die light shaking time 
int shakeTimeMax = 1000;       // initialize die max shaking time
float shakeForce = 2.0;        // Force of the die shaking in X, Y, or Z, 0-3

// x 493 to 161  midpoint (hi+low)/2 = 327  mid - low =  166
// y 518 to 184  midpoint (hi+low)/2 = 351  mid - low =  167
// z 473 to  147 midpoint (hi+low)/2 = 310  mid - low =  163
// X 159 ,501  Y  182 ,521 Z 142 ,485

//initialize the random No.
long randNumber; 
int randSeedAnalogPin = A5;  // nothing should be attached to pin A5 or A7
//Declare an array
int dieArray[6] = {1,2,3,4,5,6};

//animation speed
int roll_time = 75;

//initialize a time to use to clear the die display
static unsigned long startTime;
static unsigned long nowTime;  
int period = 15000;

void setup(){
  pinMode(bottomLeft,  OUTPUT);
  pinMode(middleLeft,  OUTPUT);
  pinMode(upperLeft,   OUTPUT);
  pinMode(middle,      OUTPUT);
  pinMode(bottomRight, OUTPUT);
  pinMode(middleRight, OUTPUT);
  pinMode(upperRight,  OUTPUT);
  pinMode(buttonPin,    INPUT);      //set button pin as input
  pinMode(buzzPin,     OUTPUT);      //set buzzer pin as output
  
  Serial.begin(9600);
  static unsigned long startTime = 0;
  static unsigned long nowTime = millis();
  randomSeed(analogRead(randSeedAnalogPin));
  // Lets animate the dice roll....
  //animated_roll();
  //delay(3000);
  Accelerometer_state = 0;
  button1State = 0;
  clearAll();         // turn off the leds to save battery
}
 void loop(){
   Accelerometer_state = 0;
   button1State = 0;
  
 //sample the accelerometer
    readAccelerometer();
    // save the read sample accelerometer data array
    AccelerometerArray[0] = tiltx;
    AccelerometerArray[1] = tilty;
    AccelerometerArray[2] = tiltz;

    Serial.println(" tiltx, tilty, tilyz ");
    for (int ia = 0; ia <= 2; ia = ia + 1) {
      Serial.print("\n"); 
      Serial.print(AccelerometerArray[ia]);
      Serial.print("\t"); 
    } //end for
   // Accelerometer_state = 0; 
    float shakeX = 100 * AccelerometerArray[0] - 100 * AccelerometerArray2[0];
    float shakeY = 100 * AccelerometerArray[1] - 100 * AccelerometerArray2[1];
    float shakeZ = 100 * AccelerometerArray[2] - 100 * AccelerometerArray2[2];
    if ((abs(shakeX) > shakeForce ||  abs(shakeY) > shakeForce || abs(shakeZ) > shakeForce ) && ( abs(shakeX) > 3.0 && abs(shakeY) > 3.0 )) {
           Accelerometer_state = 1;
           Serial.print("Accelerometer_state = ");
           Serial.println(Accelerometer_state);
           Serial.print("shakeX = ");
           Serial.println(shakeX);
           Serial.print("shakeY = ");
           Serial.println(shakeY);
           Serial.print("shakeZ = ");
           Serial.println(shakeZ);
           Serial.print("\t"); 
    } else { Accelerometer_state = 0; }
    
            Serial.println("nowTime"); 
            Serial.println(nowTime); 
            Serial.println("millis - current"); 
            Serial.println(millis()); 
    if (millis() - nowTime > shakeTimeMin && millis() - nowTime < shakeTimeMax ) {
      Serial.print("light die shaking:"); 
      Serial.println(millis() - nowTime); 
      Accelerometer_state = 1;
      } 
      else if (millis()-nowTime > shakeTimeMax)
      {
      Serial.println("heavy die shaking"); 
      Accelerometer_state = 1;       // change this state to 2, after testing, and can increase animation roll time also. 
     } else {
     Accelerometer_state = 0;   // die has not been shakin for the min time
     }  // endif

     /*
    Serial.print(tiltx); // use "tiltx", "tilty" etc in your program
    Serial.print("\t");  // "\t" is tab character
    Serial.print(tilty); 
    Serial.print("\t"); 
    Serial.println(tiltz); 
    */
    delay(1000);           // delay for serial monitor only

 // reset the button1State
 // bool status_button;
 // int  button_delay;
  Serial.print ("button1State: ");
  Serial.println(button1State);
  //Read our button if high then run dice
  if ((digitalRead(buttonPin) == HIGH && button1State == 0)  || Accelerometer_state == 1 ) {
    delay(5);  //button debounce
    led13();   //show on led13 that we pressed the button
    button1State = 1;
  // setting the timer startTime, for clearing the die display, beginning at the press of the button
    startTime = millis();  //reset the timer
 /*
 // Handle a long press to provide a way to turn off the leds to save battery
    while (status_button == HIGH){
    button_delay++;
    delay(200);
    status_button = digitalRead(buttonPin);
    if(button_delay == 10){ //no need to wait for user to release
         clearAll();
         break;
      }        
     }
    if(button_delay < 2) { //short press of button does the normal dice roll
          // Do the dice roll
 */    
    randNumber = random(1, 7);
    Serial.print(" Button pressed:= " );
    Serial.print(button1State);
    Serial.print(" Die Shows:= " );
    Serial.println(randNumber);

    // do an animated roll and set the die
    animated_roll();
    delay(shakeTimeMax);    // let the die come to rest
 //   }
    //reseed the random number
    Serial.print("reseeding the random and resetting the button1State");
    randomSeed(analogRead(randSeedAnalogPin));
    delay(500);        //wait a little for the seeding and to prevent cheating
    button1State = 0;  //reset the button1State
  }
    // add a clearAll(); with a timer, to save battery, after the period expires, and dice has not been rolled.
    /*
    Serial.print("timer: ");
    Serial.println(startTime);
    Serial.println(nowTime);
    Serial.println(millis());
    */
   if (millis() - startTime > period)
    {
        // timer has expired
        Serial.print("clearing the die");
        Serial.print("\t");
        clearAll();
        startTime = millis();  //reset the starttime
    } // end of timer

    //sample the accelerometer
    readAccelerometer();
    // save the read sample accelerometer data array
    AccelerometerArray2[0] = tiltx;
    AccelerometerArray2[1] = tilty;
    AccelerometerArray2[2] = tiltz;
    Serial.print("fetch 2nd sample ");
    Serial.println(" tiltx, tilty, tilyz ");
    for (int ia = 0; ia <= 2; ia = ia + 1) {
      Serial.print("\n"); 
      Serial.print(AccelerometerArray[ia]);
      Serial.print("\t"); 
    } //end for
    delay (500);  // delay for serial monitor only
    Accelerometer_state = 0;  // reset the state of the acclerometer
    nowTime = millis();  //   current time for state of the acclerometer
    delay(5);
}
//Create a function for each of our "sides" of the die
 void six()
{
  digitalWrite(bottomLeft, HIGH);
  digitalWrite(middleLeft, HIGH);
  digitalWrite(upperLeft, HIGH);
  digitalWrite(bottomRight, HIGH);
  digitalWrite(middleRight, HIGH);
  digitalWrite(upperRight, HIGH);
}
void five()
{
  digitalWrite(upperLeft, HIGH);
  digitalWrite(bottomLeft, HIGH);
  digitalWrite(middle, HIGH);
  digitalWrite(upperRight, HIGH);
  digitalWrite(bottomRight, HIGH);
}
void four()
{
  digitalWrite(upperLeft, HIGH);
  digitalWrite(bottomLeft, HIGH);
  digitalWrite(upperRight, HIGH);
  digitalWrite(bottomRight, HIGH);
}
void three()
{
  digitalWrite(upperLeft, HIGH);
  digitalWrite(middle, HIGH);
  digitalWrite(bottomRight, HIGH);
}
void two()
{
  digitalWrite(bottomRight, HIGH);
  digitalWrite(upperLeft, HIGH);
}
void one(){
  digitalWrite(middle, HIGH);
}
void clearAll(){
  digitalWrite(bottomLeft, LOW);
  digitalWrite(middleLeft, LOW);
  digitalWrite(upperLeft, LOW);
  digitalWrite(middle,LOW);
  digitalWrite(bottomRight, LOW);
  digitalWrite(middleRight, LOW);
  digitalWrite(upperRight, LOW);
}
bool check_button(int buttonPin)
{
  button1State = digitalRead(buttonPin);
  if (button1State != lastButton1State && button1State == HIGH) {
    lastButton1State = button1State;
    return true;
  }
  lastButton1State = button1State;
  return false;
}
void dice_set(int diceSet)
{
// set a dice value
    switch (diceSet) {
    case 1:
       one();
           Serial.print("switchcase ");
           Serial.print("diceSet ");
           Serial.println(diceSet);
           Serial.print("\t");  // "\t" is tab character
      break;
    case 2:
      two();
           Serial.print("switchcase ");
           Serial.print("diceSet ");
           Serial.println(diceSet);
           Serial.print("\t");  // "\t" is tab character
      break;
    case 3:
       three();
           Serial.print("switchcase ");
           Serial.print("diceSet ");
           Serial.println(diceSet);
           Serial.print("\t");  // "\t" is tab character
      break;
    case 4:
      four();
           Serial.print("switchcase ");
           Serial.print("diceSet ");
           Serial.println(diceSet);
           Serial.print("\t");  // "\t" is tab character
      break;
    case 5:
       five();
           Serial.print("switchcase ");
           Serial.print("diceSet ");
           Serial.println(diceSet);
            Serial.print("\t");  // "\t" is tab character
      break;
    case 6:
      six();
           Serial.print("switchcase ");
           Serial.print("diceSet ");
           Serial.println(diceSet);
           Serial.print("\t");  // "\t" is tab character
      break;
    default:
           Serial.print("switchcase ");
           Serial.print("default diceSet ");
           Serial.println(diceSet);
           Serial.print("\t");  // "\t" is tab character
      break;
    } 
}
void led13() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(300);                        // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(200);                        // wait for a second
}
void beep(){
        //buzzer beeps
        digitalWrite(buzzPin, HIGH);
        delay(2);
        digitalWrite(buzzPin, LOW);
        delay(5);
        digitalWrite(buzzPin, HIGH);
        delay(5);
        digitalWrite(buzzPin, LOW);
}
void animated_roll()
{
  //roll_time = 25;
  roll_time = random(20,51);
  for (int i = random(15, 21); i > 0; i--) {
    clearAll();
    dice_set(random(1, 7));
    delay(roll_time);
    Serial.print("roll_time:= ");
    Serial.println(i);
    //buzzer beeps
        digitalWrite(buzzPin, HIGH);
        delay(2);
        digitalWrite(buzzPin, LOW);
        delay(5);
        digitalWrite(buzzPin, HIGH);
        delay(5);
        digitalWrite(buzzPin, LOW);
    if (i > 7 && roll_time < 50)  {
      roll_time = roll_time + i / 2;
    }
    else   {
      roll_time = roll_time + 10;
      if (i < 2)  {
        //buzzer beeps
        digitalWrite(buzzPin, HIGH);
        delay(2);
        digitalWrite(buzzPin, LOW);
      }
    }
  }
} 
void  readAccelerometer(){
   int xMid, yMid, zMid, xRng, yRng, zRng;
   int xr = 0, yr = 0, zr = 0;    // x raw etc
   float x = 0, y = 0, z = 0;
    xr = analogRead(X_Pin);      // read analog pin for x raw
  //      Serial.print(xr );       
  //          Serial.print("\t"); 
    yr = analogRead(Y_Pin);      // read analog pin for y raw
   //     Serial.print(yr ); 
   //        Serial.print("\t");      
    zr = analogRead(Z_Pin);      // read analog pin for z raw   
   //     Serial.println(zr );              
   //
    // read calibration values from EEprom - won't work until calibrated
    xMid = EEPROM.read(1000) << 8; 
    xMid = xMid | EEPROM.read(1001);
    xRng = EEPROM.read(1002) << 8;
    xRng = xRng | EEPROM.read(1003);
    yMid = EEPROM.read(1004) << 8; 
    yMid = yMid | EEPROM.read(1005);
    yRng = EEPROM.read(1006) << 8;
    yRng = yRng | EEPROM.read(1007);
    zMid = EEPROM.read(1008) << 8; 
    zMid = zMid | EEPROM.read(1009);
    zRng = EEPROM.read(1010) << 8;
    zRng = zRng | EEPROM.read(1011);

    // print calibration values. x, y, z pairs
    // they should look like ~300 ~160  ~300 ~160   ~300 ~160   
  /*  
Serial.println("Calibratrion constants: xMid, xRng, yMid, yRng, zMid, zRng");
    Serial.print(xMid);
    Serial.print("\t"); 
    Serial.print(xRng); 
    Serial.print("\t\t");
    Serial.print(yMid);
    Serial.print("\t"); 
    Serial.print(yRng); 
    Serial.print("\t\t");
    Serial.print(zMid); 
    Serial.print("\t"); 
    Serial.println(zRng);
    delay(2000);  
   */
    //
    x = (xr - xMid) / (float)xRng;
  //      Serial.print("x");
  //      Serial.print(x); 
  //      Serial.print("\t");  
    y = (yr - yMid) / (float)yRng;
  //      Serial.print("y");
  //      Serial.print(y);
  //      Serial.print("\t");  
    z = (zr - zMid) / (float)zRng;
   //     Serial.print("z");
   //     Serial.print(z);
   //     Serial.print("\t"); 
  /*
    The acceleration is a 3D vector, which has a length (or magnitude) and points in some direction.
    So, if you want to know about changes in acceleration you have to consider changes in both the length and the direction.
     The (magnitude), length L = sqrt(x*x + y*y + z*z)
     The direction cosines in x, y, and z (which give the information about where the vector is pointing) are given by
     dcx = x/L
     dcy = y/L
     dcz = z/L
 */
    tiltx = atan2( sqrt((y*y) + (z*z)), x);
    tilty = atan2( sqrt((x*x) + (z*z)), y);
    tiltz = atan2( sqrt((y*y) + (x*x)), z);
}
