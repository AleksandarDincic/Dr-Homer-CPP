#include <iostream>
#include <string>
#include "Menu.h"
#include "Formatter.h"
#include "Exceptions.h"


int main(int argc, const char* argv[]) {
	Menu::initialize();
	if (argc == 3) {
		try {
			Image *i = Image::getImage();
			i->addLayer(argv[1]);
			FUNFormatter formatter;
			CompositeOperation *o = formatter.load(argv[2]);
			i->addOperation(o);
			i->operate();
			i->Export(argv[1]);
			exit(0);
		}
		catch (BadFormatException e) {
			std::cout << e.getMessage();
		}
		catch (BadInputException e) {
			std::cout << e.getMessage();
		}
		catch (BadPathException e) {
			std::cout << e.getMessage();
		}
		catch (...) {
			std::cout << "Unknown error";
		}

		try {
			Image *i = Image::getImage();
			i->loadImage(argv[1]);
			FUNFormatter formatter;
			CompositeOperation *o = formatter.load(argv[2]);
			i->addOperation(o);
			i->operate();
			for (Layer* l : *i) {
				l->setVisible(false);
			}
			for (Layer* l : *i) {
				l->setVisible(true);
				i->Export(l->getPath());
				l->setVisible(false);
			}
			exit(0);
		}
		catch (BadFormatException e) {
			std::cout << e.getMessage();
			exit(1);
		}
		catch (BadInputException e) {
			std::cout << e.getMessage();
			exit(1);
		}
		catch (BadPathException e) {
			std::cout << e.getMessage();
			exit(1);
		}
		catch (...) {
			std::cout << "Unknown error";
			exit(1);
		}
	}
	else {
		Menu m;
		m.start();
	}
	return 0;
}