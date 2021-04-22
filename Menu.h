#pragma once
#include <iostream>
#include "Image.h"

class Menu {
private:
	static bool initialized; 
	
	bool running, editMode, unsaved;
	std::string message;
	Image* image;
	
	void initImage() { image = Image::getImage(); editMode = true; }
	
	void loadImage();
	void refreshMenu();
	void addLayer();
	void deleteLayer();
	void setLayerOpacity();
	void setLayerActive();
	void setLayerVisible();
	void addSelection();
	void setSelectionActive();
	void deleteSelection();
	void addOperation();
	std::vector<Rectangle> inputRects();
	void addLayerFromPath();
	void inputChoice();
	void clearOperations();
	void operate();
	void saveCompositeOperation();
	void importCompositeOperation();
	void exportCompositeOperation();
	void saveProject();
	void exportImage();
	void quit();
public:
	static void initialize();

	void start();
	Menu() : running(true), editMode(false), unsaved(false), message(""), image(nullptr) {}
};