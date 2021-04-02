#ifndef WINDOW_H
#define WINDOW_H

#include "sdk.h"

extern sf::RenderWindow* SFWINDOW;
extern tgui::Gui* TGUI;

#define DEFAULT_WINDOW_SIZE_X 1280
#define DEFAULT_WINDOW_SIZE_Y 720

void Init();
void Update();

#endif