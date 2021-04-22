#include "Layer.h"
#include "Operation.h"
#include <iostream>
#include <string>

void Layer::resize(int width, int height)
{
	if (width > 0) {
		for (std::vector<Pixel>& rows : pixels) {
			rows.resize(width, Pixel());
		}
	}
	if (height > 0) {
		pixels.resize(height, std::vector<Pixel>(getWidth(), Pixel()));
	}
}

void Layer::addOperation(Operation * o, const std::vector<Selection*>& selections)
{
	doneOperations.push_back(new DoneOperation(o, selections, getWidth(), getHeight()));
}

void Layer::clamp()
{
	int height = getHeight();
	int width = getWidth();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			(*this)[i][j].clamp();
		}
	}
}

std::ostream & operator<<(std::ostream & os, const Layer & l)
{
	os << (l.getActive() ? "Active - " : "Inactive - ") << (l.getVisible() ? "Visible - " : "Invisible - ") << "Opacity " << l.opacity << " (";
	for (Layer::DoneOperation* o : l.doneOperations) {
		os << o->getOperation()->getName() << ", ";
	}
	return os << ")";
}

Layer::DoneOperation::DoneOperation(Operation * operation, const std::vector<Selection*>& selections, int clampWidth, int clampHeight) : operation(operation->clone()) {
	for (Selection* s : selections) {
		Selection *newSelection = new Selection(*s);
		newSelection->clamp(clampWidth, clampHeight);
		this->selections.push_back(newSelection);
	}
}
