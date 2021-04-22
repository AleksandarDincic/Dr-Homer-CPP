#include <string>
#include "Menu.h"
#include "Operation.h"
#include "Formatter.h"
#include "Exceptions.h"

bool Menu::initialized = false;

void Menu::initialize()
{
	if (!initialized) {
		ImageFormatter::addFormat("bmp", new BMPFormatter());
		ImageFormatter::addFormat("pam", new PAMFormatter());
		ProjectFormatter::addFormat("dr", new DRFormatter());
		OperationFormatter::addFormat("fun", new FUNFormatter());
		Operation::addOperation(Add().getName(), new Add());
		Operation::addOperation(Sub().getName(), new Sub());
		Operation::addOperation(InverseSub().getName(), new InverseSub());
		Operation::addOperation(Mul().getName(), new Mul());
		Operation::addOperation(Div().getName(), new Div());
		Operation::addOperation(InverseDiv().getName(), new InverseDiv());
		Operation::addOperation(Power().getName(), new Power());
		Operation::addOperation(Log().getName(), new Log());
		Operation::addOperation(Abs().getName(), new Abs());
		Operation::addOperation(Min().getName(), new Min());
		Operation::addOperation(Max().getName(), new Max());
		Operation::addOperation(Invert().getName(), new Invert());
		Operation::addOperation(Greyscale().getName(), new Greyscale());
		Operation::addOperation(BlackWhite().getName(), new BlackWhite());
		Operation::addOperation(Median().getName(), new Median());
		Operation::addOperation(Fill().getName(), new Fill());
		initialized = true;
	}
}

void Menu::loadImage()
{
	std::string path;
	std::cout << "Enter path to file: ";
	std::cin >> path;

	Image::loadImage(path);
	image = image->getImage();
	message = "Project loaded";
}

void Menu::refreshMenu()
{
	system("cls");

	std::cout << "=======Dr. Homer Image Editor=======" << std::endl;
	if (image)
		std::cout << *image << std::endl;

	std::cout << message << std::endl << std::endl;

	std::cout << "Select a command:" << std::endl;
	if (!editMode) {
		std::cout << "1. New project" << std::endl;
		std::cout << "2. Open project" << std::endl;
		std::cout << "3. Exit" << std::endl;
	}
	else {
		std::cout << "1.  Add empty layer" << std::endl;
		std::cout << "2.  Add layer from image" << std::endl;
		std::cout << "3.  Delete layer" << std::endl;
		std::cout << "4.  Set layer opacity" << std::endl;
		std::cout << "5.  Set layer active" << std::endl;
		std::cout << "6.  Set layer visible" << std::endl;
		std::cout << "7.  Add selection" << std::endl;
		std::cout << "8.  Set selection active" << std::endl;
		std::cout << "9.  Delete selection" << std::endl;
		std::cout << "10. Add operation" << std::endl;
		std::cout << "11. Clear operations" << std::endl;
		std::cout << "12. Apply operations" << std::endl;
		std::cout << "13. Save as composite operation" << std::endl;
		std::cout << "14. Import composite operation" << std::endl;
		std::cout << "15. Export composite operation" << std::endl;
		std::cout << "16. Save project" << std::endl;
		std::cout << "17. Export image" << std::endl;
		std::cout << "18. Exit" << std::endl;
	}
}

void Menu::addLayer()
{
	std::string widthStr, heightStr;
	int width, height;

	std::cout << "Enter the new layer's width: ";
	std::cin >> widthStr;
	width = std::stoi(widthStr);

	std::cout << "Enter the new layer's height: ";
	std::cin >> heightStr;
	height = std::stoi(heightStr);

	image->addLayer(width, height);

	unsaved = true;
	message = "Layer added";
}

void Menu::deleteLayer()
{
	std::string layerNumStr;
	int layerNum;

	std::cout << "Enter layer number: ";
	std::cin >> layerNumStr;
	layerNum = std::stoi(layerNumStr);

	image->deleteLayer(layerNum - 1);

	unsaved = true;
	message = "Layer deleted";
}

void Menu::setLayerOpacity()
{
	std::string layerNumStr, opacityStr;
	int layerNum, opacity;

	std::cout << "Enter layer number: ";
	std::cin >> layerNumStr;
	layerNum = std::stoi(layerNumStr);

	std::cout << "Enter layer opacity (0-100, will be clamped): ";
	std::cin >> opacityStr;
	opacity = std::stoi(opacityStr);

	image->setLayerOpacity(layerNum - 1, opacity);

	unsaved = true;
	message = "Opacity changed";
}

void Menu::setLayerActive()
{
	std::string layerNumStr, activeStr;
	int layerNum, active;

	std::cout << "Enter layer number: ";
	std::cin >> layerNumStr;
	layerNum = std::stoi(layerNumStr);

	std::cout << "Enter 0 for inactive, any other number for active: ";
	std::cin >> activeStr;
	active = std::stoi(activeStr);

	image->setLayerActive(layerNum - 1, active);

	unsaved = true;
	message = "Layer active state changed";
}

void Menu::setLayerVisible()
{
	std::string layerNumStr, visibleStr;
	int layerNum, visible;

	std::cout << "Enter layer number: ";
	std::cin >> layerNumStr;
	layerNum = std::stoi(layerNumStr);
	
	std::cout << "Enter 0 for invisible, any other number for visible: ";
	std::cin >> visibleStr;
	visible = std::stoi(visibleStr);

	image->setLayerVisible(layerNum - 1, visible);

	unsaved = true;
	message = "Layer visibility changed";
}

void Menu::addSelection()
{
	std::string name;
	std::vector<Rectangle> rects;
	
	std::cout << "Enter selection name :";
	std::cin >> name;
	
	rects = inputRects();
	image->addSelection(rects, name);

	unsaved = true;
	message = "Selection added";
}

void Menu::setSelectionActive()
{
	std::string name, activeStr;
	int active;

	std::cout << "Enter selection name :";
	std::cin >> name;
	
	std::cout << "Enter 0 for inactive, any other number for active:";
	std::cin >> activeStr;
	active = std::stoi(activeStr);
	
	image->setSelectionActive(name, active);

	unsaved = true;
	message = "Selection active state changed";
}

void Menu::deleteSelection()
{
	std::string name;
	
	std::cout << "Enter selection name :";
	std::cin >> name;
	
	image->deleteSelection(name);
	
	unsaved = true;
	message = "Selection deleted";
}

void Menu::addOperation()
{
	Operation* operation;
	std::string name;
	std::vector<double> params;

	std::cout << "Insert operation name. Available operations :" << std::endl;
	Operation::printBasicOperations();
	image->printCompositeOperations();
	std::cout << std::endl;
	std::cin >> name;
	
	operation = Operation::getOperation(name);
	if (!operation) {
		operation = image->getCompositeOperation(name);
		if(!operation) throw BadInputException("Operation with that name doesn't exist");
	}

	operation = operation->clone();
	
	std::cout << "Operation has " << operation->numOfParams() << " parameters" << std::endl;
	for (int i = 0; i < operation->numOfParams(); i++) {
		std::string paramStr;
		double param;
		
		std::cout << "Enter parameter " << (i + 1) << ": ";
		std::cin >> paramStr;
		param = std::stod(paramStr);

		params.push_back(param);
	}
	
	operation->setParams(params);
	image->addOperation(operation);
	
	unsaved = true;
	message = "Operation added";
}

std::vector<Rectangle> Menu::inputRects()
{
	std::vector<Rectangle> rects;
	std::cout << "Enter the properties of each rectangle. Entering an invalid number at any point will stop the input of rectangles" << std::endl;
	while (true) {
		std::string xStr, yStr, widthStr, heightStr;
		int x, y, width, height;

		std::cout << "Enter the x-position of rectangle: ";
		std::cin >> xStr;
		x = std::stoi(xStr);
		if (x < 0) break;

		std::cout << "Enter the y-position of rectangle: ";
		std::cin >> yStr;
		y = std::stoi(yStr);
		if (y < 0) break;

		std::cout << "Enter the width of rectangle: ";
		std::cin >> widthStr;
		width = std::stoi(widthStr);
		if (width <= 0) break;

		std::cout << "Enter the height of rectangle: ";
		std::cin >> heightStr;
		height = std::stoi(heightStr);
		if (height <= 0) break;

		rects.push_back(Rectangle(x, y, width, height));
	}
	return rects;
}

void Menu::addLayerFromPath()
{
	std::string path;

	std::cout << "Enter path to image: ";
	std::cin >> path;
	
	image->addLayer(path);

	unsaved = true;
	message = "Layer added";
}

void Menu::start()
{
	while (running) {
		refreshMenu();
		inputChoice();
	}
}


void Menu::inputChoice() {
	std::string choiceStr;
	std::cin >> choiceStr;
	int choice;
	
	try {
		choice = std::stoi(choiceStr);
		if (!editMode)
			switch (choice) {
			case 1:
				image = Image::getImage();
				editMode = true;
				break;
			case 2:
				loadImage();
				editMode = true;
				break;
			case 3:
				running = false;
				break;
			default:
				message = "Unknown command";
				break;
			}
		else
			switch (choice) {
			case 1:
				addLayer();
				break;
			case 2:
				addLayerFromPath();
				break;
			case 3:
				deleteLayer();
				break;
			case 4:
				setLayerOpacity();
				break;
			case 5:
				setLayerActive();
				break;
			case 6:
				setLayerVisible();
				break;
			case 7:
				addSelection();
				break;
			case 8:
				setSelectionActive();
				break;
			case 9:
				deleteSelection();
				break;
			case 10:
				addOperation();
				break;
			case 11:
				clearOperations();
				break;
			case 12:
				operate();
				break;
			case 13:
				saveCompositeOperation();
				break;
			case 14:
				importCompositeOperation();
				break;
			case 15:
				exportCompositeOperation();
				break;
			case 16:
				saveProject();
				break;
			case 17:
				exportImage();
				break;
			case 18:
				quit();
				break;
			case 98:
				(new FUNFormatter())->load("C:\\Users\\adinc\\source\\repos\\POOP_Projekat\\petar.fun");
				break;
			case 99:
				image->Export("C:\\Users\\adinc\\source\\repos\\POOP_Projekat\\Debug\\ajmo.bmp");
				break;
			default:
				message = "Unknown command";
				break;
			}
	}
	catch (std::invalid_argument e){
		message = "Invalid input";
	}
	catch (std::out_of_range e){
		message = "Input out of range";
	}
	catch (BadFormatException e) {
		message = e.getMessage();
	}
	catch (BadInputException e) {
		message = e.getMessage();
	}
	catch (BadPathException e) {
		message = e.getMessage();
	}
	catch (...) {
		message = "Unknown error";
	}
}

void Menu::clearOperations()
{
	image->clearOperations();

	unsaved = true;
	message = "Operations cleared";
}

void Menu::operate()
{
	image->operate();
	image->clearOperations();

	unsaved = true;
	message = "Operations applied";
}

void Menu::saveCompositeOperation()
{
	std::string name;

	std::cout << "Enter composite operation name: ";
	std::cin >> name;
	
	image->saveCompositeOperation(name);
	image->clearOperations();

	unsaved = true;
	message = "Composite operation saved";
}

void Menu::importCompositeOperation()
{
	std::string path, deleteOldStr;
	int deleteOld;

	std::cout << "Enter file path:";
	std::cin >> path;

	std::cout << "Would you like to replace composite operation with same name if already loaded?" << std::endl;
	std::cout << "Enter 0 for no, any other number for yes: ";
	std::cin >> deleteOldStr;
	deleteOld = std::stoi(deleteOldStr);

	image->importCompositeOperation(path, deleteOld);

	unsaved = true;
	message = "Composite operation loaded";
}

void Menu::exportCompositeOperation()
{
	std::string name, path;

	std::cout << "Enter composite operation name: ";
	std::cin >> name;
	
	std::cout << "Enter file path: ";
	std::cin >> path;

	image->exportCompositeOperation(name, path);

	unsaved = true;
	message = "Composite operation exported ";
}

void Menu::saveProject()
{
	std::string path;
	std::cout << "Enter path to file: ";
	std::cin >> path;
	image->saveProject(path);

	unsaved = false;
	message = "Project saved ";
}

void Menu::exportImage()
{
	std::string path;
	std::cout << "Enter path to file: ";
	std::cin >> path;
	image->Export(path);

	unsaved = false;
	message = "Image exported";
}

void Menu::quit()
{
	std::string choiceStr;
	int choice = 1;

	if (unsaved) {
		std::cout << "There are unsaved changes, would you like to exit without saving?" << std::endl;
		std::cout << "Enter 0 for no, or any other number for yes: ";
		std::cin >> choiceStr;
		choice = std::stoi(choiceStr);
	}
	if (choice) {
		std::cout << "Would you like to export before exiting?" << std::endl;
		std::cout << "Enter 0 for no, or any other number for yes: ";
		std::cin >> choiceStr;
		choice = std::stoi(choiceStr);

		if (choice) exportImage();

		image = nullptr;
		Image::deleteImage();
		editMode = false;
	}
}
