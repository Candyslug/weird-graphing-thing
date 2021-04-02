#include "Interpreter.h"
#include "Window.h"



enum Token_Value {
	NAME, NUMBER, END,
	PLUS = '+', MINUS = '-', MUL = '*', DIV = '/',
	PRINT = ';', ASSIGN = '=', LP = '(', RP = ')'
};

#define math_pi 3.1415926535897
#define math_e  2.7182818284590


std::string HandleInputString() {
	std::string s = TGUI->get<tgui::TextBox>("InputTextBox")->getText();
	std::string ret;
	if (s.size() > 0) {
		for (int i = 0; i < s.size(); i++) {
			char curChar = s[i];
			if (curChar != ' ') {
				if (curChar == '\n') {
					ret += ";";
				}
				else {
					ret += curChar;
				}
			}
		}
	}
	std::cout << "======== ret:\n" << ret << std::endl;
	std::cout << "========\n";
	return ret;
}



int InterpInput() {

	HandleInputString();
	//return Y position for every x position in the equation
	int y = 0;



	return y;
}




