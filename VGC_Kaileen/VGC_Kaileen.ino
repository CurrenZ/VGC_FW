/*******************************************************
 * Author: Current Zeng, Makers Making Change, <keranz@neilsquire.ca>
 * Data created: 9/5/2018
 * 
 * This file is part of Video Game Controller.
 * Inputs are two FSR buttons and one joystick
 *******************************************************/

#include <FSRKEY.h>
#include <Keyboard.h>

// pins mapping
#define FSR1 A0      // FSR buttons
#define FSR2 A1
#define JOY_V A2     // Joystick vertical
#define JOY_H A3     // Joystick horizontal
#define JOY_PRESS 8  // Joystick press
#define POT1 A4      // Potential meters
#define POT2 A5
#define RED1 2       // LEDs
#define WHT1 3
#define BLU1 4
#define RED2 5
#define WHT2 6
#define BLU2 7

#define FSR_HARD_MAX 550 // used to limit adjustment of potentiometers
#define FSR_HARD_MIN 300
#define JOY_THD 150  // THD means threshold
#define JOY_TOR 10   // TOR means tolerance

FSRKEY x('x');   // button 1, light press
FSRKEY r('r');   // button 1, hard press
FSRKEY c('c');   // button 2, light press
FSRKEY z('z');   // button 2, hard press
FSRKEY up('w');  // joystick up
FSRKEY dn('s');  // joystick down
FSRKEY lt('a');  // joystick left
FSRKEY rt('d');  // joystick right
FSRKEY esc(KEY_ESC);  // joystick press

int fsr1 = 0;
int fsr2 = 0;

// Red White and Blue LEDs turn on at light, medium and hard press region respectively
// These parameters will be adjusted in setup() according to potentiometers' reading
int FSR1_LITE_THD = 100;
int FSR1_HARD_THD = 400;
int FSR1_MID_THD = 250;
int FSR2_LITE_THD = 100;
int FSR2_HARD_THD = 400;
int FSR2_MID_THD = 250;

// bias for later compensation, will be updated in setup()
long fsr1Bias = 0;
long fsr2Bias = 0;

// joystick readings at nomal positions, will be updated in setup()
long joyVNom = 0;
long joyHNom = 0;

// the key to be sent
char currentKey1 = 0;
char currentKey2 = 0;
char currentKeyJ = 0;

// preventing light key strokes after releasing hard key
// change to true if this feature is not needed
const bool fsr1ContinueMode = false;
const bool fsr2ContinueMode = false;

void setup(){
	Serial.begin(115200);
	Keyboard.begin();
	pinMode(FSR1, INPUT);
	pinMode(FSR2, INPUT);
	pinMode(JOY_V, INPUT);
	pinMode(JOY_H, INPUT);
	pinMode(JOY_PRESS, INPUT);
	pinMode(POT1, INPUT);
	pinMode(POT2, INPUT);
	pinMode(RED1, OUTPUT);
	pinMode(WHT1, OUTPUT);
	pinMode(BLU1, OUTPUT);
	pinMode(RED2, OUTPUT);
	pinMode(WHT2, OUTPUT);
	pinMode(BLU2, OUTPUT);

	// initializing the device
  // calculating rest FSRs bias and joystick nom for later compensation
  for (int i = 0; i < 100; i ++){
    fsr1Bias += analogRead(FSR1);
    fsr2Bias += analogRead(FSR2);
    joyVNom += analogRead(JOY_V);
    joyHNom += analogRead(JOY_H);
  }
  fsr1Bias /= 100;
  fsr2Bias /= 100;
  joyVNom /= 100;
  joyHNom /= 100;

  // adjusting FSRs' threasholds by reading voltage of potentiometers
  adjustFSRTHD(limit(analogRead(POT1)), &FSR1_LITE_THD, &FSR1_MID_THD, &FSR1_HARD_THD);
  adjustFSRTHD(limit(analogRead(POT2)), &FSR2_LITE_THD, &FSR2_MID_THD, &FSR2_HARD_THD);

  // flickering LEDs, nothing but a cool indicator
	for (int i = 0; i < 5; i ++){
		digitalWrite(RED1, HIGH);
		digitalWrite(RED2, HIGH);
		delay(100);
		digitalWrite(RED1, LOW);
		digitalWrite(RED2, LOW);
		digitalWrite(WHT1, HIGH);
		digitalWrite(WHT2, HIGH);
		delay(100);
		digitalWrite(WHT1, LOW);
		digitalWrite(WHT2, LOW);
		digitalWrite(BLU1, HIGH);
		digitalWrite(BLU2, HIGH);
		delay(100);
		digitalWrite(BLU1, LOW);
		digitalWrite(BLU2, LOW);
	}
}

void loop(){
  // updating compensated fsr readings
  int fsr1_tmp = analogRead(FSR1) - fsr1Bias;
  int fsr2_tmp = analogRead(FSR2) - fsr2Bias;
  int joyv = analogRead(JOY_V);
  int joyh = analogRead(JOY_H);
  int joyp = analogRead(JOY_PRESS);
  
  // when continue mode is set to false, these four line will be executed 
  // if harder key is pressed, fsr reading updates only when it is fully released
  if (!fsr1ContinueMode && r.is_pressed) fsr1 = (fsr1_tmp <= (FSR1_LITE_THD >> 1))? 0:fsr1;
  else fsr1 = fsr1_tmp;
  if (!fsr2ContinueMode && z.is_pressed) fsr2 = (fsr2_tmp <= (FSR2_LITE_THD >> 1))? 0:fsr2;
  else fsr2 = fsr2_tmp;

  makeLEDsDecisions(RED1, WHT1, BLU1, fsr1_tmp, FSR1_LITE_THD, FSR1_MID_THD, FSR1_HARD_THD);
  makeLEDsDecisions(RED2, WHT2, BLU2, fsr2_tmp, FSR2_LITE_THD, FSR2_MID_THD, FSR2_HARD_THD);
  
  // release key if it was pressed and now released
  if (fsr1 < FSR1_LITE_THD) x.key_release();
  if (fsr1 < FSR1_HARD_THD) r.key_release();
  if (fsr2 < FSR2_LITE_THD) c.key_release();
  if (fsr2 < FSR2_HARD_THD) z.key_release();
  if (abs(joyv - joyVNom) <= JOY_TOR){
    up.key_release();
    dn.key_release();
  }
  if (abs(joyh - joyHNom) <= JOY_TOR){
    lt.key_release();
    rt.key_release();
  }
  if (joyp != 0) esc.key_release();

  // key decisions and strokes send of FSR buttons
  makeCharDecisions(x.self, r.self, fsr1, &currentKey1, FSR1_LITE_THD, FSR1_HARD_THD);
  makeCharDecisions(c.self, z.self, fsr2, &currentKey2, FSR2_LITE_THD, FSR2_HARD_THD);
  sendKeyStrokes(currentKey1, currentKey2);

  // key decisions and strokes send of joystick
  if (joyv > (int(joyVNom) + JOY_THD)) up.key_press();
  if (joyv < (int(joyVNom) - JOY_THD)) dn.key_press();
  if (joyh > (int(joyHNom) + JOY_THD)) lt.key_press();
  if (joyh < (int(joyHNom) - JOY_THD)) rt.key_press();
  // horizontal and vertical joystick movments may affect joystick push
  // note that this phenomenon may only apply to the specific joystick we picked
  if ((joyp == 0) && (joyh == joyHNom) && (joyv == joyVNom)) esc.key_press();
}

void adjustFSRTHD(int ppp, int* l, int* m, int* h){
  *h = ppp;
  *l = *h >> 2;
  *m = *l + ((*h - *l) >> 1);
}

void makeLEDsDecisions(int red, int wht, int blu, int fff, int l, int m, int h){
  if (fff <= l){
    digitalWrite(red, LOW);
    digitalWrite(wht, LOW);
    digitalWrite(blu, LOW);
  }
  else if (fff > l && fff <= m){
    digitalWrite(red, LOW);
    digitalWrite(wht, LOW);
    digitalWrite(blu, HIGH);
  }
  else if (fff > m && fff <= h){
    digitalWrite(red, LOW);
    digitalWrite(wht, HIGH);
    digitalWrite(blu, LOW);
  }
  else if (fff > h){
    digitalWrite(red, HIGH);
    digitalWrite(wht, LOW);
    digitalWrite(blu, LOW);
  }
}

void makeCharDecisions(char lite, char hard, int fff, char* curKey, int l, int h){
  if (fff < l) *curKey = 0;
  if (fff >= l) *curKey = lite;
  if (fff >= h) *curKey = hard;
}

void sendKeyStrokes(char curK1, char curK2){
  if (curK1 == x.self) {
    x.key_press();
    r.key_release();
  }
  else if (curK1 == r.self){
    x.key_release();
    r.key_press();
  } 

  if (curK2 == c.self){
    c.key_press();
    z.key_release();
  } 
  else if (curK2 == z.self){
    c.key_release();
    z.key_press();
  } 
}

int limit(int ppp){
  if (ppp >= FSR_HARD_MAX) return FSR_HARD_MAX;
  if (ppp <= FSR_HARD_MIN) return FSR_HARD_MIN;
  return ppp;
}
