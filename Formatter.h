#pragma once
#include <map>
#include "Layer.h"
#include "rapidxml.hpp"

class ImageFormatter {
private:
	static std::map<std::string, ImageFormatter*> formatMap;

public:
	static void addFormat(std::string fileType, ImageFormatter *formatter);
	static ImageFormatter* getFormatter(std::string fileType);

	virtual Layer* load(const std::string& path) = 0;
	virtual void save(const std::string& path) = 0;

};

class BMPFormatter : public ImageFormatter {
public:
	Layer* load(const std::string& path) override;
	void save(const std::string& path) override;

};

class PAMFormatter : public ImageFormatter {
public:
	Layer* load(const std::string& path) override;
	void save(const std::string& path) override;

};

class ProjectFormatter {
private:
	static std::map<std::string, ProjectFormatter*> formatMap;

public:
	static void addFormat(std::string fileType, ProjectFormatter *formatter);
	static ProjectFormatter* getFormatter(std::string fileType);

	virtual void load(const std::string& path) = 0;
	virtual void save(const std::string& path) = 0;

};

class DRFormatter : public ProjectFormatter {
private:
	rapidxml::xml_document<> doc;

	void appendXMLRectangle(rapidxml::xml_node<>& node, const Rectangle& rect);
	void appendXMLSelection(rapidxml::xml_node<>& node, Selection *s);
	void appendXMLSelection(rapidxml::xml_node<>& node, std::pair<std::string, Selection*> s);
	void appendXMLDoneOperation(rapidxml::xml_node<>& node, Layer::DoneOperation *o);
	void appendXMLLayer(rapidxml::xml_node<>& node, Layer *l);

	Layer* convertXMLtoLayer(rapidxml::xml_node<>& node);
public:
	void load(const std::string& path) override;
	void save(const std::string& path) override;

};

class CompositeOperation;

class OperationFormatter {
	static std::map<std::string, OperationFormatter*> formatMap;
public:
	static void addFormat(std::string fileType, OperationFormatter *formatter);
	static OperationFormatter* getFormatter(std::string fileType);

	virtual CompositeOperation* load(const std::string& path) = 0;
	virtual void save(CompositeOperation *operation, const std::string& path) = 0;
};

class FUNFormatter : public OperationFormatter{
private:
	rapidxml::xml_document<> doc;

public:
	CompositeOperation* load(const std::string& path) override;
	void save(CompositeOperation *operation, const std::string& path) override;
};