#include <regex>
#include "Image.h"
#include "Formatter.h"
#include "Selection.h"
#include "Exceptions.h"

Image* Image::image = nullptr;

Pixel Image::getPixel(int width, int height)
{
	int  tempRed = 0, tempGreen = 0, tempBlue = 0;
	double tempAlpha = 0;
	for (Layer *l : layers) {
		if (l->getVisible()) {
			double opacity = l->getOpacity() / 100.0;
			Pixel tempPixel = (*l)[height][width];
			double doubleA = tempPixel.getA() * opacity / 255.0;
			tempAlpha += (1 - tempAlpha) * doubleA;
		}
	}
	double temperAlpha = 0;
	for (Layer *l : layers) {
		if (l->getVisible()) {
			double opacity = l->getOpacity() / 100.0;
			Pixel tempPixel = (*l)[height][width];
			double doubleA = tempPixel.getA() * opacity / 255.0;
			tempRed += (1 - temperAlpha) * doubleA / tempAlpha * tempPixel.getR();
			tempGreen += (1 - temperAlpha) * doubleA / tempAlpha * tempPixel.getG();
			tempBlue += (1 - temperAlpha) * doubleA / tempAlpha * tempPixel.getB();
			temperAlpha += (1 - temperAlpha) * doubleA;
		}
	}
	return Pixel(tempRed, tempGreen, tempBlue, tempAlpha*255);
}

void Image::resize(Layer *l)
{
	if (height < l->getHeight() || width < l->getWidth()) {
		for (Layer *ll : layers) {
			ll->resize(width < l->getWidth() ? l->getWidth() : -1, height < l->getHeight() ? l->getHeight() : -1);
		}
		height = height < l->getHeight() ? l->getHeight() : height;
		width = width < l->getWidth() ? l->getWidth() : width;
	}
	l->resize(width > l->getWidth() ? width : -1, height > l->getHeight() ? height : -1);
}

Image::~Image()
{
	for (Layer* l : layers) {
		delete l;
	}
	for (Operation* o : operations) {
		delete o;
	}
	for (std::pair<std::string, Selection*> s : selections) {
		delete s.second;
	}
	for (std::pair<std::string, CompositeOperation*> o : compositeOperations) {
		delete o.second;
	}
}

Image * Image::getImage() {
	if (image == nullptr)
		image = new Image();
	return image;
}

void Image::loadImage(const std::string & path) try
{
	ProjectFormatter* reader;

	std::regex rx("[^\.]*\.(.*)");
	std::smatch extensionMatch;
	if (std::regex_match(path, extensionMatch, rx)) {
		std::string extension = extensionMatch.str(1);
		if ((reader = ProjectFormatter::getFormatter(extension))) {
			reader->load(path);
		}
		else throw BadFormatException("Not a project format");
	}
	else throw BadPathException("Invalid path");
}
catch (BadFormatException e) {
	deleteImage();
	throw e;
}
catch (BadPathException e) {
	deleteImage();
	throw e;
}

CompositeOperation * Image::getCompositeOperation(const std::string & name)
{
	if (compositeOperations.find(name) != compositeOperations.end())
		return compositeOperations[name];
	else return nullptr;
}

void Image::setLayerOpacity(int pos, int opacity)
{
	if (layers.size() == 0 || pos < 0 || pos > layers.size() - 1)
		throw BadInputException("Layer index out of bounds");
	getLayer(pos).setOpacity(opacity);
}

void Image::setLayerActive(int pos, bool active)
{
	if (layers.size() == 0 || pos < 0 || pos > layers.size() - 1)
		throw BadInputException("Layer index out of bounds");
	getLayer(pos).setActive(active);
}

void Image::setLayerVisible(int pos, bool visible)
{
	if (layers.size() == 0 || pos < 0 || pos > layers.size() - 1)
		throw BadInputException("Layer index out of bounds");
	getLayer(pos).setVisible(visible);
}

void Image::setSelectionActive(const std::string& name, bool active)
{
	if (selections.find(name) != selections.end()) {
		selections[name]->setActive(active);
	}
	else throw BadInputException("Selection with that name doesn't exist");
}

void Image::addLayer(int width, int height)
{
	if (width <= 0 || height <= 0) throw BadInputException("Layer dimensions must be positive");
	Layer *tempLayer = new Layer(width, height);
	resize(tempLayer);
	layers.insert(layers.begin(), tempLayer);
}

void Image::addLayer(std::string path) {
	ImageFormatter* reader;

	std::regex rx("[^\.]*\.(.*)");
	std::smatch extensionMatch;
	if (std::regex_match(path, extensionMatch, rx)) {
		std::string extension = extensionMatch.str(1);
		if ((reader = ImageFormatter::getFormatter(extension))) {
			Layer *tempLayer = reader->load(path);
			resize(tempLayer);
			layers.insert(layers.begin(), tempLayer);
		}
		else throw BadFormatException("Not an image format");
	}
	else throw BadPathException("Invalid path");
}

Layer * Image::importLayer(const std::string & path)
{
	ImageFormatter* reader;

	std::regex rx("[^\.]*\.(.*)");
	std::smatch extensionMatch;
	if (std::regex_match(path, extensionMatch, rx)) {
		std::string extension = extensionMatch.str(1);
		if ((reader = ImageFormatter::getFormatter(extension))) {
			Layer *tempLayer = reader->load(path);
			return tempLayer;
		}
	}
	return nullptr;
}

void Image::addSelection(std::vector<Rectangle>& rects, std::string name)
{
	if (selections.find(name) == selections.end()) {
		selections[name] = new Selection(rects);
	}
	else throw BadInputException("Selection with that name already exists");
}

void Image::operate()
{
	std::vector<Selection *> activeSelections;
	for (std::pair<std::string, Selection*> p : selections) {
		if (p.second->getActive())
			activeSelections.push_back(p.second);
	}

	for (Layer *l : layers)
		if (l->getActive()) {
			for (Operation *o : operations) {
				o->operateLayer(l, activeSelections);
				l->addOperation(o, activeSelections);
				l->clamp();
			}
		}
}

void Image::deleteLayer(int pos)
{
	if (layers.size() == 0 || pos < 0 || pos > layers.size() - 1) 
		throw BadInputException("Layer index out of bounds");
	Layer *tempLayer = layers[pos];
	layers.erase(layers.begin() + pos);
	delete tempLayer;
	if (!layers.size())
		width = height = 0;
}

void Image::deleteSelection(const std::string & name)
{
	if (selections.find(name) != selections.end()) {
		selections.erase(name);
	}
	else throw BadInputException("Selection with that name doesn't exist");
}

void Image::clearOperations()
{
	for (Operation *o : operations)
		delete o;
	operations.clear();
}

void Image::printCompositeOperations()
{
	for (std::pair<std::string, Operation*> p : compositeOperations)
		std::cout << p.second->getName() << ", ";
}

void Image::saveCompositeOperation(const std::string & name)
{
	if (!Operation::getOperation(name) && compositeOperations.find(name) == compositeOperations.end()) {
		CompositeOperation* newOperation = new CompositeOperation(name);
		for (Operation *o : operations) {
			newOperation->addOperation(o);
		}
		compositeOperations[name] = newOperation;
	}
	else throw BadInputException("Operation with that name already exists");
}

void Image::saveCompositeOperation(CompositeOperation * o)
{
	if (!Operation::getOperation(o->getName()) && compositeOperations.find(o->getName()) == compositeOperations.end()) {
		compositeOperations[o->getName()] = o;
	}
	else throw BadInputException("Operation with that name already exists");
}

void Image::deleteCompositeOperation(const std::string & name)
{
	if (compositeOperations.find(name) != compositeOperations.end()) {
		compositeOperations.erase(name);
	}
	else throw BadInputException("Operation with that name doesn't exist");
}

void Image::importCompositeOperation(const std::string & path, bool deleteOld)
{
	OperationFormatter* reader;
	CompositeOperation *operation;

	std::regex rx("[^\.]*\.(.*)");
	std::smatch extensionMatch;
	if (std::regex_match(path, extensionMatch, rx)) {
		std::string extension = extensionMatch.str(1);
		if (reader = OperationFormatter::getFormatter(extension)) {
			operation = reader->load(path);
		}
		else throw BadFormatException("Format doesn't exist");
	}
	else throw BadPathException("Invalid path to file");

	if (compositeOperations.find(operation->getName()) != compositeOperations.end()) {
		if (deleteOld) {
			delete compositeOperations[operation->getName()];
			compositeOperations.erase(operation->getName());
		}
		else throw BadInputException("Operation with that name already exists");
	}

	compositeOperations[operation->getName()] = operation;
}

void Image::exportCompositeOperation(const std::string & name, const std::string & path)
{
	OperationFormatter* writer;

	if (compositeOperations.find(name) != compositeOperations.end()) {
		CompositeOperation *savedOperation = compositeOperations[name];

		std::regex rx("[^\.]*\.(.*)");
		std::smatch extensionMatch;
		if (std::regex_match(path, extensionMatch, rx)) {
			std::string extension = extensionMatch.str(1);
			if (writer = OperationFormatter::getFormatter(extension)) {
				writer->save(savedOperation,path);
			}
			else throw BadFormatException("Format doesn't exist");
		}
		else throw BadPathException("Invalid path to file");
	}
	else throw BadInputException("Operation with that name doesn't exist");
}

void Image::Export(std::string path)
{
	ImageFormatter* writer;

	std::regex rx("[^\.]*\.(.*)");
	std::smatch extensionMatch;
	if (std::regex_match(path, extensionMatch, rx)) {
		std::string extension = extensionMatch.str(1);
		if (writer = ImageFormatter::getFormatter(extension)) {
			writer->save(path);
		}
		else throw BadFormatException("Format doesn't exist");
	}
	else throw BadPathException("Invalid path to file");
}

void Image::saveProject(const std::string & path)
{
	ProjectFormatter* writer;

	std::regex rx("[^\.]*\.(.*)");
	std::smatch extensionMatch;
	if (std::regex_match(path, extensionMatch, rx)) {
		std::string extension = extensionMatch.str(1);
		if (writer = ProjectFormatter::getFormatter(extension)) {
			writer->save(path);
		}
		else throw BadFormatException("Format doesn't exist");
	}
	else throw BadPathException("Invalid path to file");
}

std::ostream & operator<<(std::ostream & o, const Image & i)
{
	o << "IMAGE:" << std::endl;
	o << "Size: " << i.width << 'x' << i.height << std::endl;
	o << "Layers:" << std::endl;
	int counter = 1;
	for (Layer *l : i.layers)
		o << counter++ << ". - " << *l << std::endl;
	//ovde layeri
	o << "Selections:" << std::endl;
	for (std::pair<std::string,Selection*> s : i.selections) {
		o << s.first << " - " << *s.second << std::endl;
	}
	//ovde selekcije
	o << "Operations:" << std::endl;
	for (Operation * op : i.operations) {
		o << op->getName() << " -> ";
	}
	//ovde operacije
	return o;
}
