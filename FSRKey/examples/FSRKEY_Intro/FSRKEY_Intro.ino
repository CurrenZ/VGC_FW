#include <FSRKEY.h>

FSRKEY a('a');

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.print(a.self);
  Serial.print("\t");
  Serial.print(a.lower_case);
  Serial.print("\t");
  Serial.print(a.upper_case);
  Serial.print("\t");
  Serial.print(a.is_pressed);
  Serial.print("\t");
  Serial.print(a.is_letter);
  Serial.print("\n");

}