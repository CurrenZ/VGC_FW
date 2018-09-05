#include "FSRKEY.h"

FSRKEY::FSRKEY(){
}

FSRKEY::FSRKEY(char theKey){
	self = theKey;
	lower_case = theKey;
	upper_case = theKey;
	if (self >= 65 && self <= 90){
		lower_case = self + 32;
		is_letter = true;
	}
	else if (self >= 97 && self <= 122){
		upper_case = self - 32;
		is_letter = true;
	}
}

void FSRKEY::key_press(){
	if (!is_pressed){
		Keyboard.press(self);
		is_pressed = true;
	}
}

void FSRKEY::key_release(){
	if (is_pressed){
		Keyboard.release(self);
		is_pressed = false;
	}
}