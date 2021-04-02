#ifndef PROGRAM_H
#define PROGRAM_H

#include "sdk.h"

struct gridPoint {
	gridPoint(float x, float y) {
		this->pos.x = x;
		this->pos.y = y;
	}
	sf::Vector2f pos;
};

struct pointSet {
	pointSet(sf::Color c = sf::Color::Red, sf::Vector2f initPos = sf::Vector2f(-999, -999)) {
		this->clr = c;
		if (initPos != sf::Vector2f(-999, -999)) {
			AddPoint(initPos.x, initPos.y);
		}
	}
	void AddPoint(float x, float y) {
		this->points.push_back(gridPoint(x, y));
	}
	std::vector<gridPoint> points;
	sf::Color clr;
};

void FixCalcs();
void AddPointSetToList(pointSet ps);
void AddNewPointSetWithoutFirstPoint();
void DeletePointSetFromList(int index);
void UpdateEvents();
void DrawMousePos();
void InitWidgets();
void ClearOutputTextBox();
void UpdateWidgets();
void DrawGridBackground();
void DrawGridLines();
void DragGrid();
void PlotPointToGrid(sf::Vector2f point, sf::Vector2i size, sf::Color clr);
sf::Vector2f CalculatePointToScreenGridPos(sf::Vector2f point);
void DrawPointsFromSetList();
void UpdatePointSetList();
sf::Vector2f ClosestSelectedPointSetPointToMouse();
void ResetGridView();

#endif

