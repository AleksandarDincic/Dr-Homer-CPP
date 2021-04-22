#include <fstream>
#include "Formatter.h"
#include "Image.h"
#include "Exceptions.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

using namespace rapidxml;

void DRFormatter::appendXMLRectangle(rapidxml::xml_node<>& node, const Rectangle & rect)
{
	xml_node<> *rectangleNode = doc.allocate_node(node_element, "rectangle");

	std::string numStr = std::to_string(rect.getX());
	char *numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *xNode = doc.allocate_node(node_element, "x", numChar);
	rectangleNode->append_node(xNode);

	numStr = std::to_string(rect.getY());
	numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *yNode = doc.allocate_node(node_element, "y", numChar);
	rectangleNode->append_node(yNode);

	numStr = std::to_string(rect.getWidth());
	numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *widthNode = doc.allocate_node(node_element, "width", numChar);
	rectangleNode->append_node(widthNode);

	numStr = std::to_string(rect.getHeight());
	numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *heightNode = doc.allocate_node(node_element, "height", numChar);
	rectangleNode->append_node(heightNode);

	node.append_node(rectangleNode);
}

void DRFormatter::appendXMLSelection(rapidxml::xml_node<>& node, Selection * s)
{
	xml_node<> *selectionNode = doc.allocate_node(node_element, "selection");

	for (Rectangle r : *s) {
		appendXMLRectangle(*selectionNode, r);
	}

	node.append_node(selectionNode);
}

void DRFormatter::appendXMLSelection(rapidxml::xml_node<>& node, std::pair<std::string, Selection*> s)
{
	xml_node<> *selectionNode = doc.allocate_node(node_element, "selection");

	char* name = doc.allocate_string(s.first.c_str());
	xml_node<> *nameNode = doc.allocate_node(node_element, "name", name);
	selectionNode->append_node(nameNode);

	std::string activeStr = std::to_string(s.second->getActive());
	char *activeChar = doc.allocate_string(activeStr.c_str());
	xml_node<> *activeNode = doc.allocate_node(node_element, "active", activeChar);
	selectionNode->append_node(activeNode);

	for (Rectangle r : *(s.second)) {
		appendXMLRectangle(*selectionNode, r);
	}

	node.append_node(selectionNode);
}

void DRFormatter::appendXMLDoneOperation(rapidxml::xml_node<>& node, Layer::DoneOperation * o)
{
	xml_node<> *doneOperationNode = doc.allocate_node(node_element, "doneOperation");

	o->getOperation()->appendOperationXML(doc, *doneOperationNode);

	const std::vector<Selection*>& selections = o->getSelections();

	xml_node<> *selectionsNode = doc.allocate_node(node_element, "selections");

	for (Selection* s : selections) {
		appendXMLSelection(*selectionsNode, s);
	}

	doneOperationNode->append_node(selectionsNode);

	node.append_node(doneOperationNode);
}

void DRFormatter::appendXMLLayer(rapidxml::xml_node<>& node, Layer * l)
{
	xml_node<> *layerNode = doc.allocate_node(node_element, "layer");

	char* pathChar = doc.allocate_string(l->getPath().c_str());
	xml_node<> *pathNode = doc.allocate_node(node_element, "path", pathChar);
	layerNode->append_node(pathNode);

	std::string numStr = std::to_string(l->getActive());
	char* numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *activeNode = doc.allocate_node(node_element, "active", numChar);
	layerNode->append_node(activeNode);

	numStr = std::to_string(l->getVisible());
	numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *visibleNode = doc.allocate_node(node_element, "visible", numChar);
	layerNode->append_node(visibleNode);

	numStr = std::to_string(l->getOpacity());
	numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *opacityNode = doc.allocate_node(node_element, "opacity", numChar);
	layerNode->append_node(opacityNode);

	const std::vector<Layer::DoneOperation*>& doneOperations = l->getDoneOperations();

	xml_node<> *doneOperationsNode = doc.allocate_node(node_element, "doneOperations");

	for (Layer::DoneOperation* o : doneOperations)
		appendXMLDoneOperation(*doneOperationsNode, o);

	layerNode->append_node(doneOperationsNode);

	node.append_node(layerNode);
}

Layer * DRFormatter::convertXMLtoLayer(rapidxml::xml_node<>& node)
{
	Image *image = Image::getImage();

	xml_node<> *pathNode = node.first_node("path");
	std::string path = pathNode->value();
	
	Layer *l;
	if(path != "")
		l = image->importLayer(path);
	else l = new Layer(image->getWidth(), image->getHeight());

	if (l) {
		xml_node<> *activeNode = pathNode->next_sibling("active");
		std::string activeStr = activeNode->value();
		bool active = std::stoi(activeStr);
		l->setActive(active);

		xml_node<> *visibleNode = activeNode->next_sibling("visible");
		std::string visibleStr = visibleNode->value();
		bool visible = std::stoi(visibleStr);
		l->setVisible(visible);

		xml_node<> *opacityNode = visibleNode->next_sibling("opacity");
		std::string opacityStr = opacityNode->value();
		int opacity = std::stoi(opacityStr);
		l->setOpacity(opacity);

		xml_node<> *doneOperationsNode = opacityNode->next_sibling("doneOperations");

		for (xml_node<> *childNode = doneOperationsNode->first_node("doneOperation"); childNode; childNode = childNode->next_sibling("doneOperation")) {
			xml_node<> *operationNode = childNode->first_node("operation");
			Operation *o = Operation::convertXMLtoOperation(operationNode);

			std::vector<Selection*> selections;

			xml_node<> *selectionsNode = operationNode->next_sibling("selections");
			for (xml_node<> *childerNode = selectionsNode->first_node("selection"); childerNode; childerNode = childerNode->next_sibling("selection")) {
				std::vector<Rectangle> rects;

				for (xml_node<> *childestNode = childerNode->first_node("rectangle"); childestNode; childestNode = childestNode->next_sibling("rectangle")) {
					xml_node<> *xNode = childestNode->first_node("x");
					std::string xStr = xNode->value();
					int x = std::stoi(xStr);

					xml_node<> *yNode = xNode->next_sibling("y");
					std::string yStr = yNode->value();
					int y = std::stoi(yStr);

					xml_node<> *widthNode = yNode->next_sibling("width");
					std::string widthStr = widthNode->value();
					int width = std::stoi(widthStr);

					xml_node<> *heightNode = widthNode->next_sibling("height");
					std::string heightStr =	heightNode->value();
					int height = std::stoi(heightStr);

					rects.push_back(Rectangle(x, y, width, height));
				}

				selections.push_back(new Selection(rects));
			}

			o->operateLayer(l, selections);
			l->clamp();
			l->addOperation(o, selections);
		}

		return l;
	}
	return nullptr;
}

void DRFormatter::save(const std::string& path) {
	Image *image = Image::getImage();
	std::ofstream FILE(path);
	
	doc.remove_all_attributes();
	doc.remove_all_nodes();
	doc.clear();

	xml_node<> *node = doc.allocate_node(node_element, "image");

	std::string numStr = std::to_string(image->getWidth());
	char *numChar = doc.allocate_string(numStr.c_str());
	xml_node<> *numNode = doc.allocate_node(node_element, "width", numChar);
	node->append_node(numNode);

	numStr = std::to_string(image->getHeight());
	numChar = doc.allocate_string(numStr.c_str());
	numNode = doc.allocate_node(node_element, "height", numChar);
	node->append_node(numNode);

	xml_node<> *layersNode = doc.allocate_node(node_element, "layers");

	for (Layer* l : *image) {
		appendXMLLayer(*layersNode, l);
	}

	node->append_node(layersNode);

	//selekcije

	xml_node<> *selectionsNode = doc.allocate_node(node_element, "selections");
	
	const std::map<std::string, Selection*>& selections = image->getSelections();

	for (std::pair<std::string, Selection*> s : selections) {
		appendXMLSelection(*selectionsNode, s);
	}

	node->append_node(selectionsNode);

	//kompozitne

	xml_node<> *compositeNode = doc.allocate_node(node_element, "compositeOperations");

	const std::map<std::string, CompositeOperation*>& compositeOperations = image->getCompositeOperations();

	for (std::pair<std::string, CompositeOperation*> o : compositeOperations) {
		o.second->appendOperationXML(doc, *compositeNode);
	}

	node->append_node(compositeNode);

	doc.append_node(node);

	FILE << doc;

	FILE.close();
}


void DRFormatter::load(const std::string& path) {
	doc.remove_all_attributes();
	doc.remove_all_nodes();
	doc.clear();

	Image* image = Image::getImage();

	file<> xmlFile(path.c_str());
	doc.parse<0>(xmlFile.data());

	xml_node<> *imageNode = doc.first_node("image");
	
	xml_node<> *widthNode = imageNode->first_node("width");
	std::string widthStr = widthNode->value();
	int width = std::stoi(widthStr);

	xml_node<> *heightNode = widthNode->next_sibling("height");
	std::string heightStr = heightNode->value();
	int height = std::stoi(heightStr);


	//layeri

	image->addLayer(width, height);

	xml_node<> *layersNode = heightNode->next_sibling("layers");

	for (xml_node<> *childNode = layersNode->first_node("layer"); childNode; childNode = childNode->next_sibling("layer")) {
		Layer *l = convertXMLtoLayer(*childNode);
		image->addLayerBottom(l);
	}

	image->deleteLayer(0);

	//selekcije

	xml_node<> *selectionsNode = layersNode->next_sibling("selections");
	for (xml_node<> *childNode = selectionsNode->first_node("selection"); childNode; childNode= childNode->next_sibling("selection")) {
		xml_node<> *nameNode = childNode->first_node("name");
		std::string name = nameNode->value();

		xml_node<> *activeNode = nameNode->next_sibling("active");
		std::string activeStr = activeNode->value();
		bool active = std::stoi(activeStr);

		std::vector<Rectangle> rects;

		for (xml_node<> *childerNode = activeNode->next_sibling("rectangle"); childerNode; childerNode = childerNode->next_sibling("rectangle")) {
			xml_node<> *xNode = childerNode->first_node("x");
			std::string xStr = xNode->value();
			int x = std::stoi(xStr);

			xml_node<> *yNode = xNode->next_sibling("y");
			std::string yStr = yNode->value();
			int y = std::stoi(yStr);

			xml_node<> *widthNode = yNode->next_sibling("width");
			std::string widthStr = widthNode->value();
			int width = std::stoi(widthStr);

			xml_node<> *heightNode = widthNode->next_sibling("height");
			std::string heightStr = heightNode->value();
			int height = std::stoi(heightStr);

			rects.push_back(Rectangle(x, y, width, height));
		}

		image->addSelection(rects, name);
		image->setSelectionActive(name, active);
	}

	//add operations
	
	xml_node<> *operationsNode = selectionsNode->next_sibling("compositeOperations");
	for (xml_node<> *childNode = operationsNode->first_node("operation"); childNode; childNode = childNode->next_sibling("operation")) {
		Operation *o = Operation::convertXMLtoOperation(childNode);
		image->saveCompositeOperation((CompositeOperation *)o);
	}
}
