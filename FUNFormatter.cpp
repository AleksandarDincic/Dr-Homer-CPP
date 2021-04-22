#include <fstream>
#include "Image.h"
#include "Formatter.h"
#include "Operation.h"
#include "Exceptions.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

using namespace rapidxml;

CompositeOperation * FUNFormatter::load(const std::string & path)
{
	doc.remove_all_attributes();
	doc.remove_all_nodes();
	doc.clear();

	file<> xmlFile(path.c_str());
	doc.parse<0>(xmlFile.data());

	xml_node<> *node = doc.first_node("compositeOperation");
	//exception!!

	xml_node<> *nameNode = node->first_node("name");
	//exception!!

	CompositeOperation* importedOperation = new CompositeOperation(nameNode->value());
	
	importedOperation->convertXMLtoParams(node);

	return importedOperation;
}

void FUNFormatter::save(CompositeOperation *operation, const std::string& path) {
	std::ofstream FILE(path);
	doc.remove_all_attributes();
	doc.remove_all_nodes();
	doc.clear();

	xml_node<> *node = doc.allocate_node(node_element, "compositeOperation");
	
	char *name = doc.allocate_string(operation->getName().c_str());
	xml_node<> *nameNode = doc.allocate_node(node_element, "name", name);
	
	node->append_node(nameNode);

	operation->appendParamsXML(doc, *node);

	doc.append_node(node);

	FILE << doc;

	FILE.close();
}