#ifndef FSRKEY_h
#define FSRKEY_h

#include <Keyboard.h>

class FSRKEY{
public:
	FSRKEY();
	FSRKEY(char _theKey);
	char self = 0;
	char lower_case = 0;
	char upper_case = 0;
	bool is_pressed = false;
	bool is_letter = false;
	void key_press();
	void key_release();
};

#endif