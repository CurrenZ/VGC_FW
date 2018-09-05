#include <FSRKEY.h>
#include <Keyboard.h>

#define FSR A0
#define FSR_LITE_THD 150
#define FSR_HARD_THD 400

FSRKEY x('x');  // x is mapped to the reading when harder force is applied
FSRKEY o('o');  // o is mapped to the reading when lighter force is applied
int fsr = 0;
char currentKey = 0;

bool continueMode = false;

void setup() {
  Serial.begin(115200);
  pinMode(FSR, INPUT);
}

void loop() {
  int tmp = analogRead(FSR);
  // if harder key is pressed, fsr reading updates only when it is fully released
  if (!continueMode) if (x.is_pressed) fsr = (tmp == 0)? 0:fsr;
  else if (!x.is_pressed) fsr = tmp;
  x.key_release();
  o.key_release();
  if (fsr < FSR_LITE_THD) currentKey = 0;
  if (fsr >= FSR_LITE_THD) currentKey = o.self;
  if (fsr >= FSR_HARD_THD) currentKey = x.self;
  if (currentKey == x.self) x.key_press();
  else if (currentKey == o.self) o.key_press();
  Serial.print(tmp);
  Serial.print("\t");
  Serial.print(fsr);
  Serial.print("\t");
  Serial.println(currentKey);
}
