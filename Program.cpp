#include "Program.h"
#include "Window.h"
#include "Interpreter.h"

namespace g {
	sf::Vector2i curCenterPos = sf::Vector2i(0, 0);
	sf::Vector2f fixedCenterPos;

	int zoomScale = 10;
	int lineStep = 1;
	sf::Vector2i dragDelta;
	sf::Vector2f ratioCalc;
	sf::Vector2f posOnGridCalc;

	sf::Vector2i amountOfSquaresOnGrid;
	sf::Vector2i nearestPointToMouse;

	int curSelectedPointSet;

	sf::Vector2f mousePosOnGrid;

	namespace t {
		sf::Vector2f size = sf::Vector2f(880 - 20, 680 - 20);
		sf::Vector2f position = sf::Vector2f(390, 20);
		sf::IntRect area = sf::IntRect(position.x, position.y, size.x, size.y);

		float sizeRatioXY = size.x / size.y;
		float sizeRatioYX = size.y / size.x;
	}
}

std::vector<pointSet> ListOfPointSets;

void AddNewPointSet(sf::Vector2f firstPoint) {
	AddPointSetToList(pointSet(sf::Color::Blue, firstPoint));
	TGUI->get<tgui::ListBox>("PointsSetListBox")->setSelectedItemByIndex(ListOfPointSets.size() - 1);
}

void AddNewPointSetWithoutFirstPoint() {
	AddPointSetToList(pointSet(sf::Color::Blue));
	TGUI->get<tgui::ListBox>("PointsSetListBox")->setSelectedItemByIndex(ListOfPointSets.size() - 1);
}

void AddPointSetToList(pointSet ps) {
	ListOfPointSets.push_back(ps);
	char c[128];
	sprintf(c, "%i: RGBA: (%i, %i, %i, %i)", (int)ListOfPointSets.size(),
		ps.clr.r,
		ps.clr.g,
		ps.clr.b,
		ps.clr.a);
	TGUI->get<tgui::ListBox>("PointsSetListBox")->addItem(c);
}

void DeletePointSetFromList(int index) {
	ListOfPointSets.erase(ListOfPointSets.begin() + index);
	TGUI->get<tgui::ListBox>("PointsSetListBox")->deselectItem();
	UpdatePointSetList();
}

void func() {//Debug Test
	pointSet lol(sf::Color::Red);
	lol.AddPoint(5, 5);
	lol.AddPoint(1, 5);
	lol.AddPoint(1, 1);
	lol.AddPoint(-3, -2);
	AddPointSetToList(lol);

	pointSet lol2(sf::Color::Blue);
	lol2.points.push_back(gridPoint(1, 5));
	lol2.points.push_back(gridPoint(6, 2));
	lol2.points.push_back(gridPoint(1, 3));
	lol2.points.push_back(gridPoint(7, 2));
	AddPointSetToList(lol2);
}

pointSet GetSelectedPointSetFromListBox() {
	return ListOfPointSets[TGUI->get<tgui::ListBox>("PointsSetListBox")->getSelectedItemIndex()];
}

void UpdateEvents() {
	sf::Event event;
	while (SFWINDOW->pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			SFWINDOW->close();
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Key::Escape) {
				TGUI->get<tgui::ListBox>("PointsSetListBox")->deselectItem();
			}
			if (event.key.code == sf::Keyboard::Key::Delete) {
				//if (TGUI->get<tgui::ListBox>("PointsSetListBox")->isFocused()) {
				if (TGUI->get<tgui::ListBox>("PointsSetListBox")->getSelectedItemIndex() != -1) {
					DeletePointSetFromList(TGUI->get<tgui::ListBox>("PointsSetListBox")->getSelectedItemIndex());
				}
				//}
			}
			if (event.key.code == sf::Keyboard::Key::Up) {
				ResetGridView();
			}
		}
		if (event.type == sf::Event::MouseWheelScrolled) {
			if (event.mouseWheelScroll.delta > 0 && g::zoomScale > 5) g::zoomScale -= 1;
			if (event.mouseWheelScroll.delta < 0) g::zoomScale += 1;
		}
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Button::Left) {
				if (g::t::area.intersects(sf::IntRect(
					sf::Mouse::getPosition(*SFWINDOW).x,
					sf::Mouse::getPosition(*SFWINDOW).y, 2, 2))) {
					//if (!TGUI->get<tgui::ListBox>("PointsSetListBox")->isFocused()) {
					/*if (TGUI->get<tgui::ListBox>("PointsSetListBox")->getSelectedItemIndex() == -1) {
						AddNewPointSet(g::mousePosOnGrid);
					}
					else */if (g::curSelectedPointSet != -1) {
						ListOfPointSets[g::curSelectedPointSet].AddPoint(g::mousePosOnGrid.x, g::mousePosOnGrid.y);
					}
					//}
				}
			}
			if (event.mouseButton.button == sf::Mouse::Button::Left) {
				if (g::t::area.intersects(sf::IntRect(
					sf::Mouse::getPosition(*SFWINDOW).x,
					sf::Mouse::getPosition(*SFWINDOW).y, 2, 2))) {

				}
			}
		}
		TGUI->handleEvent(event);
	}
}

void DrawMousePos() {
	char MPt[64];
	if (g::t::area.intersects(sf::IntRect(
		sf::Mouse::getPosition(*SFWINDOW).x, sf::Mouse::getPosition(*SFWINDOW).y, 2, 2))) {
		sprintf_s(MPt, "Cursor Pos: (%2.3f, %2.3f) (grid)",
			g::mousePosOnGrid.x,
			g::mousePosOnGrid.y);
	}
	else {
		sprintf_s(MPt, "Cursor Pos: (%i, %i) (window)",
			sf::Mouse::getPosition(*SFWINDOW).x, sf::Mouse::getPosition(*SFWINDOW).y);
	}
	TGUI->get<tgui::Label>("MousePosLabel")->setText(MPt);
	TGUI->get<tgui::Label>("MousePosLabel")->setPosition(
		g::t::area.left + g::t::area.width - TGUI->get<tgui::Label>("MousePosLabel")->getSize().x,
		g::t::area.top + g::t::area.height + 10);

	if (TGUI->get<tgui::ListBox>("PointsSetListBox")->getSelectedItemIndex() != -1) {
		sf::VertexArray lineP(sf::LinesStrip, 2);
		lineP[0].position = sf::Vector2f(sf::Mouse::getPosition(*SFWINDOW).x, sf::Mouse::getPosition(*SFWINDOW).y);
		if (ListOfPointSets[TGUI->get<tgui::ListBox>("PointsSetListBox")->getSelectedItemIndex()].points.size() > 0) {
			lineP[1].position = CalculatePointToScreenGridPos(
				ListOfPointSets[g::curSelectedPointSet].points
				[ListOfPointSets[g::curSelectedPointSet].points.size() - 1].pos);
		}
		lineP[0].color =
			sf::Color(ListOfPointSets[g::curSelectedPointSet].clr.r,
				ListOfPointSets[g::curSelectedPointSet].clr.g,
				ListOfPointSets[g::curSelectedPointSet].clr.b,
				ListOfPointSets[g::curSelectedPointSet].clr.a / 3.f);
		lineP[1].color =
			sf::Color(ListOfPointSets[g::curSelectedPointSet].clr.r,
				ListOfPointSets[g::curSelectedPointSet].clr.g,
				ListOfPointSets[g::curSelectedPointSet].clr.b,
				ListOfPointSets[g::curSelectedPointSet].clr.a / 3.f);
		if (lineP[1].position != sf::Vector2f(0, 0) && (g::t::area.intersects(sf::IntRect(
			sf::Mouse::getPosition(*SFWINDOW).x, sf::Mouse::getPosition(*SFWINDOW).y, 2, 2))))
			SFWINDOW->draw(lineP);
	}
}

void InitWidgets() { //Function only runs once.
	tgui::Label::Ptr MousePosLabel = tgui::Label::create();
	TGUI->add(MousePosLabel, "MousePosLabel");
	MousePosLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);

	tgui::Label::Ptr ZoomAndPosLabel = tgui::Label::create();
	TGUI->add(ZoomAndPosLabel, "ZoomAndPosLabel");
	ZoomAndPosLabel->setPosition(g::t::position.x, g::t::position.y + g::t::size.y + 10);

	tgui::Label::Ptr PointsSetListBoxLabel = tgui::Label::create();
	TGUI->add(PointsSetListBoxLabel, "PointsSetListBoxLabel");
	PointsSetListBoxLabel->setPosition(20, 485);
	PointsSetListBoxLabel->setText("Point Sets");

	tgui::Label::Ptr SelectedPointLabel = tgui::Label::create();
	TGUI->add(SelectedPointLabel, "SelectedPointLabel");
	SelectedPointLabel->setPosition(20, 685);

	tgui::ListBox::Ptr PointsSetListBox = tgui::ListBox::create();
	TGUI->add(PointsSetListBox, "PointsSetListBox");
	PointsSetListBox->setPosition(20, 510);
	PointsSetListBox->setSize(340, 170);

	tgui::Label::Ptr PointLabel = tgui::Label::create();
	TGUI->add(PointLabel, "PointPosLabel");
	PointLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);

	tgui::Button::Ptr AddNewPointSetButton = tgui::Button::create();
	TGUI->add(AddNewPointSetButton, "AddNewPointSetButton");
	AddNewPointSetButton->setPosition(260, 480);
	AddNewPointSetButton->setSize(100, 23);
	AddNewPointSetButton->setText("New Point Set");
	AddNewPointSetButton->connect("pressed", [&]() { AddNewPointSetWithoutFirstPoint(); });

	tgui::Button::Ptr ResetViewButton = tgui::Button::create();
	TGUI->add(ResetViewButton, "ResetViewButton");
	ResetViewButton->setPosition(820, 690);
	ResetViewButton->setSize(150, 20);
	ResetViewButton->setText("Reset View (UpArrow)");
	ResetViewButton->connect("pressed", [&]() { ResetGridView(); });

	tgui::Label::Ptr InputLabel = tgui::Label::create();
	TGUI->add(InputLabel, "InputLabel");
	InputLabel->setPosition("20, 20");
	InputLabel->setText("Input Commands            Variables: y, x\n");

	tgui::ListBox::Ptr InputCommandsList = tgui::ListBox::create();
	TGUI->add(InputCommandsList, "InputCommandsList");
	InputCommandsList->setPosition(310, 40);
	InputCommandsList->setSize(55, 155);
	InputCommandsList->addItem("cos(?)");
	InputCommandsList->addItem("sin(?)");
	InputCommandsList->addItem("tan(?)");
	InputCommandsList->addItem("pi");
	InputCommandsList->addItem("e");
	InputCommandsList->setFocusable(false);

	tgui::TextBox::Ptr InputTextBox = tgui::TextBox::create();
	TGUI->add(InputTextBox, "InputTextBox");
	InputTextBox->setPosition(20, 40);
	InputTextBox->addText("y = x + 2");
	InputTextBox->setSize(280, 155);

	tgui::Button::Ptr InputButton = tgui::Button::create();
	TGUI->add(InputButton, "InputButton");
	InputButton->setPosition(20, 200);
	InputButton->setSize(150, 20);
	InputButton->setText("Interpret Input");
	InputButton->connect("pressed", [&]() { InterpInput(); });

	tgui::Label::Ptr OutputLabel = tgui::Label::create();
	TGUI->add(OutputLabel, "OutputLabel");
	OutputLabel->setPosition(20, 230);
	OutputLabel->setText("Output");

	tgui::TextBox::Ptr OutputTextBox = tgui::TextBox::create();
	TGUI->add(OutputTextBox, "OutputTextBox");
	OutputTextBox->setPosition(20, 250);
	OutputTextBox->setSize(345, 155);
	OutputTextBox->setFocusable(false);

	tgui::Button::Ptr OutputButton = tgui::Button::create();
	TGUI->add(OutputButton, "OutputButton");
	OutputButton->setPosition(20, 410);
	OutputButton->setSize(345, 20);
	OutputButton->setText("Clear Output");
	OutputButton->connect("pressed", [&]() { ClearOutputTextBox(); });

}

void ClearOutputTextBox() {
	TGUI->get<tgui::TextBox>("OutputTextBox")->setText("");
}

void UpdateWidgets() {
	char zoomStr[64];
	sprintf(zoomStr, "Zoom: (%2.3f, %2.3f)            CurCenterPos: (%2.3f, %2.3f)",
		(float)g::zoomScale,
		(float)g::zoomScale * (float)g::t::sizeRatioYX,
		g::fixedCenterPos.x,
		g::fixedCenterPos.y);
	TGUI->get<tgui::Label>("ZoomAndPosLabel")->setText(zoomStr);

	char pointStr[124];
	if (g::curSelectedPointSet == -1) {
		sprintf(pointStr, "Selected Point Set Index: Null", g::curSelectedPointSet);
	}
	else {
		sprintf_s(pointStr, "Selected Point Set Index: %i", g::curSelectedPointSet);
	}
	TGUI->get<tgui::Label>("SelectedPointLabel")->setText(pointStr);
}

void FixCalcs() {
	g::fixedCenterPos.x = (float)g::curCenterPos.x / g::ratioCalc.x;
	g::fixedCenterPos.y = (float)g::curCenterPos.y / g::ratioCalc.y;

	g::ratioCalc.x = -(g::t::size.x / g::zoomScale);
	g::ratioCalc.y = -(g::t::size.y / g::zoomScale) * g::t::sizeRatioYX;

	g::amountOfSquaresOnGrid.x = g::t::size.x / g::zoomScale;
	g::amountOfSquaresOnGrid.y = g::t::size.y / g::zoomScale;

	g::mousePosOnGrid.x =
		(g::t::size.x / 2.f / g::ratioCalc.x) +
		g::fixedCenterPos.x - (sf::Mouse::getPosition(*SFWINDOW).x - g::t::position.x) / g::ratioCalc.x;
	g::mousePosOnGrid.y =
		g::t::sizeRatioYX * g::t::sizeRatioYX * ((g::t::size.y / 2.f / g::ratioCalc.y) +
			g::fixedCenterPos.y - (sf::Mouse::getPosition(*SFWINDOW).y - g::t::position.y) / g::ratioCalc.y);

	g::curSelectedPointSet = TGUI->get<tgui::ListBox>("PointsSetListBox")->getSelectedItemIndex();
}

void DrawGridBackground() {
	sf::RectangleShape gridBG;
	gridBG.setOutlineThickness(2.f);
	gridBG.setOutlineColor(sf::Color::Black);
	gridBG.setSize(g::t::size);
	gridBG.setPosition(g::t::position);
	SFWINDOW->draw(gridBG);
}

void DrawGridLines() {

	sf::RectangleShape VerticalLine;
	VerticalLine.setSize(sf::Vector2f(2, g::t::size.y));
	for (int a = 0 - g::zoomScale * g::zoomScale; a < g::zoomScale * g::zoomScale; a++) {
		int pos = g::t::position.x + (a * (g::t::size.x / g::zoomScale)) + (g::t::size.x / 2);
		if (pos + g::curCenterPos.x < g::t::position.x) continue;
		if (pos + g::curCenterPos.x < g::t::position.x) continue;
		else if (pos + g::curCenterPos.x > g::t::position.x + g::t::size.x) continue;
		if (a == 0) {
			VerticalLine.setSize(sf::Vector2f(2, g::t::size.y));
			VerticalLine.setFillColor(sf::Color(0, 0, 0, 255));
		}
		else if (a % 5 == 0) {
			VerticalLine.setSize(sf::Vector2f(1, g::t::size.y));
			VerticalLine.setFillColor(sf::Color(50, 50, 50, 255));
		}
		else {
			VerticalLine.setSize(sf::Vector2f(1, g::t::size.y));
			VerticalLine.setFillColor(sf::Color(170, 170, 170, 255));
		}
		VerticalLine.setPosition(sf::Vector2f(pos + g::curCenterPos.x, g::t::position.y));
		SFWINDOW->draw(VerticalLine);
	}
	//==========================================================//
	sf::RectangleShape HorizontalLine;
	HorizontalLine.setSize(sf::Vector2f(g::t::size.x, 2));
	for (int b = 0 - g::zoomScale * g::zoomScale; b < g::zoomScale * g::zoomScale; b++) {
		int pos = g::t::position.y + (b * (((g::t::size.y * g::t::sizeRatioXY) / g::zoomScale))) + (g::t::size.y / 2);
		if (pos + g::curCenterPos.y < g::t::position.y) continue;
		else if (pos + g::curCenterPos.y > g::t::position.y + g::t::size.y) continue;
		if (b == 0) {
			HorizontalLine.setSize(sf::Vector2f(g::t::size.x, 2));
			HorizontalLine.setFillColor(sf::Color(0, 0, 0, 255));
		}
		else if (b % 5 == 0) {
			HorizontalLine.setSize(sf::Vector2f(g::t::size.x, 1));
			HorizontalLine.setFillColor(sf::Color(50, 50, 50, 255));
		}
		else {
			HorizontalLine.setSize(sf::Vector2f(g::t::size.x, 1));
			HorizontalLine.setFillColor(sf::Color(170, 170, 170, 255));
		}
		HorizontalLine.setPosition(sf::Vector2f(g::t::position.x, pos + g::curCenterPos.y));
		SFWINDOW->draw(HorizontalLine);
	}
}

void DragGrid() {
	if (g::t::area.intersects(sf::IntRect(
		sf::Mouse::getPosition(*SFWINDOW).x,
		sf::Mouse::getPosition(*SFWINDOW).y, 2, 2))) {

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
			if (g::dragDelta == sf::Vector2i(0, 0)) {
				g::dragDelta = (sf::Vector2i)g::t::position + sf::Mouse::getPosition(*SFWINDOW) - g::curCenterPos;
			}
			g::curCenterPos = (sf::Vector2i)g::t::position + sf::Mouse::getPosition(*SFWINDOW) - g::dragDelta;
		}
		else {
			g::dragDelta = sf::Vector2i(0, 0);
		}
	}

	/*if (g::curCenterPos.x < CalculatePointToScreenGridPos(sf::Vector2f(-200, 0)).x) {
		g::curCenterPos.x = CalculatePointToScreenGridPos(sf::Vector2f(-200, 0)).x;
	}
	if(g::curCenterPos.x > CalculatePointToScreenGridPos(sf::Vector2f(0, 200)).x) {
		g::curCenterPos.x = CalculatePointToScreenGridPos(sf::Vector2f(0, 200)).x;
	}

	if (g::curCenterPos.y < CalculatePointToScreenGridPos(sf::Vector2f(0, -200)).y) {
		g::curCenterPos.y = CalculatePointToScreenGridPos(sf::Vector2f(0, -200)).y;
	}
	if (g::curCenterPos.y > CalculatePointToScreenGridPos(sf::Vector2f(0, 200)).y) {
		g::curCenterPos.y = CalculatePointToScreenGridPos(sf::Vector2f(0, 200)).y;
	}*/
}

void PlotPointToGrid(sf::Vector2f point, sf::Vector2i size, sf::Color clr) {
	sf::RectangleShape p;
	p.setFillColor(clr);
	p.setSize(sf::Vector2f(size.x, size.y));
	p.setPosition(point);
	if (g::t::area.intersects(sf::IntRect(point.x - 1, point.y - 1, 2, 2))) {
		SFWINDOW->draw(p);
		TGUI->get<tgui::Label>("PointPosLabel")->setPosition(point.x, point.y + 15);
	}
}

sf::Vector2f CalculatePointToScreenGridPos(sf::Vector2f point) {
	sf::Vector2f ret;
	ret.x = (float)g::t::position.x + (float)(point.x * (g::t::size.x / g::zoomScale)) + (float)(g::t::size.x / 2) + (float)g::curCenterPos.x;
	ret.y = (float)g::t::position.y + (float)(point.y * (((g::t::size.y * g::t::sizeRatioXY) / g::zoomScale))) + (float)(g::t::size.y / 2) + (float)g::curCenterPos.y;
	return ret;
}

void DrawPointsFromSetList() {
	for (int i = 0; i < ListOfPointSets.size(); i++) {
		for (int a = 0; a < ListOfPointSets[i].points.size(); a++) {
			sf::Vector2f point = CalculatePointToScreenGridPos(ListOfPointSets[i].points[a].pos);
			sf::VertexArray line(sf::LinesStrip, 2);
			if (a < ListOfPointSets[i].points.size() - 1) {

				//==========//
				//Doesnt work
				/*
				NOTE:
				try gridX/gridY + first point / second point * something(maybe where the line meets'
				the end of the grid on grid POS.
				*/


				if (point.x < g::t::position.x) {

				}


				sf::Vector2f newPoint;
				if (point.x < g::t::position.x) newPoint.x = g::t::position.x;
				if (point.y < g::t::position.y) newPoint.y = g::t::position.y;
				if (point.x > g::t::position.x + g::t::size.x) newPoint.x = g::t::position.x + g::t::size.x;
				if (point.y > g::t::position.y + g::t::size.y) newPoint.y = g::t::position.y + g::t::size.y;
				//==========//



				line[0].position = point;
				line[1].position = CalculatePointToScreenGridPos(ListOfPointSets[i].points[a + 1].pos);
				line[0].color = ListOfPointSets[i].clr;
				line[1].color = ListOfPointSets[i].clr;
				SFWINDOW->draw(line);
			}
			PlotPointToGrid(point - sf::Vector2f(2, 2), sf::Vector2i(5, 5), ListOfPointSets[i].clr);
		}
	}
}

void UpdatePointSetList() {
	TGUI->get<tgui::ListBox>("PointsSetListBox")->removeAllItems();
	for (int i = 0; i < ListOfPointSets.size(); i++) {
		char c[256];
		sprintf(c, "%i: RGBA: (%i, %i, %i, %i)", i,
			ListOfPointSets[i].clr.r,
			ListOfPointSets[i].clr.g,
			ListOfPointSets[i].clr.b,
			ListOfPointSets[i].clr.a);
		TGUI->get<tgui::ListBox>("PointsSetListBox")->addItem(c);
	}
}

sf::Vector2f ClosestSelectedPointSetPointToMouse()
{
	static int closestDist;
	/*for (int i = 0; i < ListOfPointSets[g::curSelectedPointSet].points.size(); i++) {
		if ()
	}*/
	return sf::Vector2f();
}

void ResetGridView() {
	g::zoomScale = 10;
	g::curCenterPos = sf::Vector2i(0, 0);
}
