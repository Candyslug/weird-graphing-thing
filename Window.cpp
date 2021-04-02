#include "Window.h"
#include "Program.h"
#include "Interpreter.h"

sf::RenderWindow* SFWINDOW;
tgui::Gui* TGUI;

void Init() {
	SFWINDOW = new sf::RenderWindow(sf::VideoMode(
		DEFAULT_WINDOW_SIZE_X,
		DEFAULT_WINDOW_SIZE_Y),
		"wipp",
		sf::Style::Titlebar | sf::Style::Close);
	TGUI = new tgui::Gui(*SFWINDOW);
}

void Update() {
	InitWidgets();
	while (SFWINDOW->isOpen()) {
		UpdateEvents();
		//=======================================//

		//Draw Functions
		SFWINDOW->clear(sf::Color(200, 200, 200, 255));
		FixCalcs();
		DrawGridBackground();
		DrawGridLines();
		DrawMousePos();
		DragGrid();	
		UpdateWidgets();
		DrawPointsFromSetList();
	
		//=======================================//
		TGUI->draw();
		SFWINDOW->display();
	}
}