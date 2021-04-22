#include <vector>
#include <iostream>
#include <cmath>
#include "Image.h"
#include "Operation.h"
#include "Exceptions.h"

std::map<std::string, Operation*> Operation::basicOperationMap;

void BasicOperation::operateLayer(Layer * l, const std::vector<Selection *>& s) const
{
	int width = l->getWidth();
	int height = l->getHeight();
	Layer *argumentLayer = aware() ? new Layer(*l) : l;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (!s.size() || std::any_of(s.cbegin(), s.cend(),
				[i, j](Selection* s) { return  s->inSelection(j, i); })) {
				(*l)[i][j] = operatePixel(argumentLayer, j, i);
			}
	if (aware()) delete argumentLayer;
}

void CompositeOperation::clear()
{
	for (Operation *o : operations) {
		delete o;
	}
	operations.clear();
}

void CompositeOperation::copy(const CompositeOperation & o)
{
	name = o.name;
	for (Operation *o : o.operations) {
		addOperation(o);
	}
}

void CompositeOperation::move(CompositeOperation & o)
{
	name = o.name;
	operations = o.operations;
	o.operations.clear();
}

void CompositeOperation::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	rapidxml::xml_node<> *operationsNode = doc.allocate_node(rapidxml::node_element, "operations");

	for (Operation* o : operations)
		o->appendOperationXML(doc, *operationsNode);

	currentNode.append_node(operationsNode);
}

void CompositeOperation::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* operationsNode = node->first_node("operations");

	for (rapidxml::xml_node<>* childNode = operationsNode->first_node("operation"); childNode; childNode = childNode->next_sibling("operation")) {
		Operation *o = Operation::convertXMLtoOperation(childNode);

		operations.push_back(o);
	}
}

bool CompositeOperation::aware() const
{
	return std::any_of(operations.begin(), operations.end(), [](Operation* o) {
		return o->aware();
	});
}

void CompositeOperation::operateLayer(Layer * l, const std::vector<Selection *>& s) const
{
	for (Operation *o : operations) {
		o->operateLayer(l, s);
	}
}

Pixel Add::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	return Pixel(tempPixel.getR() + paramR, tempPixel.getG() + paramG,
		tempPixel.getB() + paramB, tempPixel.getA());
}

void Add::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Add::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void Add::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel Sub::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	return Pixel(tempPixel.getR() - paramR, tempPixel.getG() - paramG,
		tempPixel.getB() - paramB, tempPixel.getA());
}

void Sub::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Sub::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void Sub::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel Greyscale::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int avg = (tempPixel.getR() + tempPixel.getG() + tempPixel.getB()) / 3;
	return Pixel(avg,avg,avg,tempPixel.getA());
}

Pixel Invert::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	return Pixel(255-tempPixel.getR(),255-tempPixel.getG(),255-tempPixel.getB(),tempPixel.getA());
}

Pixel Median::operatePixel(Layer * l, int x, int y) const
{
	std::vector<Pixel> includedPixels;
	includedPixels.push_back((*l)[y][x]);
	bool notBottom = y > 0, notTop = y < Image::getImage()->getHeight() - 1;
	bool notLeft = x > 0, notRight = x < Image::getImage()->getWidth() - 1;
	if (notBottom) {
		includedPixels.push_back((*l)[y - 1][x]);
		if (notLeft)
			includedPixels.push_back((*l)[y - 1][x - 1]);
		if (notRight)
			includedPixels.push_back((*l)[y - 1][x + 1]);
	}
	if(notLeft)
		includedPixels.push_back((*l)[y][x - 1]);
	if (notRight)
		includedPixels.push_back((*l)[y][x + 1]);
	if (notTop) {
		includedPixels.push_back((*l)[y + 1][x]);
		if (notLeft)
			includedPixels.push_back((*l)[y + 1][x - 1]);
		if (notRight)
			includedPixels.push_back((*l)[y + 1][x + 1]);
	}
	int avgR = 0, avgG = 0, avgB = 0;
	for (Pixel p : includedPixels) {
		avgR += p.getR();
		avgG += p.getG();
		avgB += p.getB();
	}
	avgR /= includedPixels.size();
	avgG /= includedPixels.size();
	avgB /= includedPixels.size();
	return Pixel(avgR,avgG,avgB,includedPixels[0].getA());
}

Pixel Mul::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	return Pixel(tempPixel.getR() * paramR, tempPixel.getG() * paramG,
		tempPixel.getB() * paramB, tempPixel.getA());
}

void Mul::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Mul::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void Mul::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel Div::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	return Pixel(tempPixel.getR() / paramR, tempPixel.getG() / paramG,
		tempPixel.getB() / paramB, tempPixel.getA());
}

void Div::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Div::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void Div::setParams(std::vector<double> params)
{
	if (params[0] == 0 || params[1] == 0 || params[2] == 0)
		throw BadInputException("Zero division");
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel Fill::operatePixel(Layer * l, int x, int y) const
{
	return Pixel(paramR, paramG, paramB, paramA);
}

void Fill::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramA);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Fill::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramAStr = paramNode->value();
	paramA = std::stoi(paramAStr);
}

void Fill::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2], paramA = params[3] > 255? 255 : (params[3] < 0 ? 0 : params[3]);
}

Pixel BlackWhite::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int avg = (tempPixel.getR() + tempPixel.getG() + tempPixel.getB()) / 3;
	avg = avg < 127 ? 0 : 255;
	return Pixel(avg, avg, avg, tempPixel.getA());
}

Pixel InverseDiv::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int newParamR = tempPixel.getR() == 0 ? 255 : paramR / tempPixel.getR();
	int newParamG = tempPixel.getG() == 0 ? 255 : paramG / tempPixel.getG();
	int newParamB = tempPixel.getB() == 0 ? 255 : paramB / tempPixel.getB();
	return Pixel(newParamR, newParamG, newParamB, tempPixel.getA());
}

void InverseDiv::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void InverseDiv::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void InverseDiv::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel InverseSub::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	return Pixel(paramR - tempPixel.getR(), paramG - tempPixel.getG(),
		paramB - tempPixel.getB(), tempPixel.getA());
}

void InverseSub::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void InverseSub::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void InverseSub::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramG = params[2];
}

Pixel Power::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int newParamR = 0, newParamG = 0, newParamB = 0;
	
	if (tempPixel.getR() < 0 && (floor(paramR) != ceil(paramR)))
		newParamR = 0;
	else if (tempPixel.getR() == 0 && paramR == 0)
		newParamR = 1;
	else if (tempPixel.getR() == 0 && paramR < 0)
		newParamR = 255;
	else newParamR = pow(tempPixel.getR(), paramR);


	if (tempPixel.getG() < 0 && (floor(paramG) != ceil(paramG)))
		newParamG = 0;
	else if (tempPixel.getG() == 0 && paramG == 0)
		newParamG = 1;
	else if (tempPixel.getG() == 0 && paramG < 0)
		newParamG = 255;
	else newParamG = pow(tempPixel.getG(), paramG);


	if (tempPixel.getB() < 0 && (floor(paramB) != ceil(paramB)))
		newParamB = 0;
	else if (tempPixel.getB() == 0 && paramB == 0)
		newParamB = 1;
	else if (tempPixel.getB() == 0 && paramB < 0)
		newParamB = 255;
	else newParamB = pow(tempPixel.getB(), paramB);

	return Pixel(newParamR, newParamG, newParamB, tempPixel.getA());
}

void Power::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Power::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stod(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stod(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stod(paramBStr);
}

void Power::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel Log::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int newParamR = 0, newParamG = 0, newParamB = 0;

	if (tempPixel.getR() == 0)
		newParamR = 0;
	else if (tempPixel.getR() < 0)
		newParamR = tempPixel.getR();
	else newParamR = log(tempPixel.getR()) / log(paramR);

	if (tempPixel.getG() == 0)
		newParamG = 0;
	else if (tempPixel.getG() < 0)
		newParamG = tempPixel.getG();
	else newParamG = log(tempPixel.getG()) / log(paramG);

	if (tempPixel.getB() == 0)
		newParamB = 0;
	else if (tempPixel.getB() < 0)
		newParamB = tempPixel.getB();
	else newParamB = log(tempPixel.getB()) / log(paramB);
	
	return Pixel(newParamR, newParamG, newParamB, tempPixel.getA());
}

void Log::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Log::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stod(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stod(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stod(paramBStr);
}

void Log::setParams(std::vector<double> params)
{
	if (params[0] < 0 || params[1] < 0 || params[2] < 0 || params[0] == 1 || params[1] == 1 || params[2] == 1)
		throw BadInputException("Invalid logarithm base entered");
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel Min::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int newR = tempPixel.getR() > paramR ? paramR : tempPixel.getR();
	int newG = tempPixel.getG() > paramG ? paramG : tempPixel.getG();
	int newB = tempPixel.getB() > paramB ? paramB : tempPixel.getB();
	return Pixel(newR, newG, newB, tempPixel.getA());
}

void Min::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Min::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void Min::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2];
}

Pixel Max::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int newR = tempPixel.getR() < paramR ? paramR : tempPixel.getR();
	int newG = tempPixel.getG() < paramG ? paramG : tempPixel.getG();
	int newB = tempPixel.getB() < paramB ? paramB : tempPixel.getB();
	return Pixel(newR, newG, newB, tempPixel.getA());
}

void Max::appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	std::string paramStr;
	const char *paramChar;
	rapidxml::xml_node<> *paramNode;

	paramStr = std::to_string(paramR);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramG);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);

	paramStr = std::to_string(paramB);
	paramChar = doc.allocate_string(paramStr.c_str());
	paramNode = doc.allocate_node(rapidxml::node_element, "param", paramChar);
	currentNode.append_node(paramNode);
}

void Max::convertXMLtoParams(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* paramNode = node->first_node("param");
	std::string paramRStr = paramNode->value();
	paramR = std::stoi(paramRStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramGStr = paramNode->value();
	paramG = std::stoi(paramGStr);

	paramNode = paramNode->next_sibling("param");
	std::string paramBStr = paramNode->value();
	paramB = std::stoi(paramBStr);
}

void Max::setParams(std::vector<double> params)
{
	paramR = params[0], paramG = params[1], paramB = params[2];
}

void Operation::addOperation(const std::string & name, Operation * o)
{
	if (basicOperationMap.find(name) == basicOperationMap.end()) {
		basicOperationMap[name] = o;
	}
	else throw BadInputException("Operation with that name already exists");
}

void Operation::printBasicOperations()
{
	for (std::pair<std::string, Operation*> p : basicOperationMap)
		std::cout << p.second->getName() << ", ";
}

Operation * Operation::convertXMLtoOperation(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* nameNode = node->first_node("name");
	std::string name = nameNode->value();

	Operation *operation = getOperation(name);
	if (operation) {
		operation = operation->clone();
	}
	else {
		operation = new CompositeOperation(name);
	}

	operation->convertXMLtoParams(node);
	return operation;
}

Operation * Operation::getOperation(const std::string& name)
{
	if (basicOperationMap.find(name) != basicOperationMap.end())
		return basicOperationMap[name];
	else return nullptr;
}

Pixel Abs::operatePixel(Layer * l, int x, int y) const
{
	Pixel tempPixel = (*l)[y][x];
	int tempR = tempPixel.getR() < 0 ? -tempPixel.getR() : tempPixel.getR();
	int tempG = tempPixel.getG() < 0 ? -tempPixel.getG() : tempPixel.getG();
	int tempB = tempPixel.getB() < 0 ? -tempPixel.getB() : tempPixel.getB();
	return Pixel(tempR, tempG, tempB, tempPixel.getA());
}

void Operation::appendOperationXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode)
{
	rapidxml::xml_node<> *operationNode = doc.allocate_node(rapidxml::node_element, "operation");

	char* name = doc.allocate_string(getName().c_str());
	rapidxml::xml_node<> *nameNode = doc.allocate_node(rapidxml::node_element, "name", name);
	operationNode->append_node(nameNode);

	appendParamsXML(doc, *operationNode);

	currentNode.append_node(operationNode);
}
