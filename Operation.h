#pragma once
#include <vector>
#include <map>
#include "Layer.h"
#include "Selection.h"
#include "rapidxml.hpp"

class Operation {
private:
	static std::map<std::string, Operation*> basicOperationMap;
protected:
	virtual Pixel operatePixel(Layer *l, int x, int y) const = 0;
public:
	static void addOperation(const std::string& name, Operation* o);
	static void printBasicOperations();
	static Operation* getOperation(const std::string& name);
	static Operation* convertXMLtoOperation(rapidxml::xml_node<>* node);

	void appendOperationXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode);
	virtual void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) {}
	virtual void convertXMLtoParams(rapidxml::xml_node<>* node) {}

	virtual std::string getName() const = 0;
	virtual bool aware() const = 0;
	virtual void operateLayer(Layer* l, const std::vector<Selection *>& s) const = 0;
	virtual int numOfParams() const = 0;
	virtual void setParams(std::vector<double> params) = 0;
	virtual Operation* clone() const = 0;
	virtual ~Operation() {}
};

class BasicOperation : public Operation {
public:
	void operateLayer(Layer *l, const std::vector<Selection *>& s) const override;
	virtual ~BasicOperation() {}
};

class CompositeOperation : public Operation {
private:
	std::vector<Operation*> operations;
	std::string name;
	void clear();
	void copy(const CompositeOperation& o);
	void move(CompositeOperation& o);
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override { return (*l)[y][x]; }
public:
	std::string getName() const override { return name; }
	bool aware() const override;
	void operateLayer(Layer *l, const std::vector<Selection *>& s) const override;
	void addOperation(Operation* o) { operations.push_back(o->clone()); }
	void setParams(std::vector<double> params) override {}
	int numOfParams() const override { return 0; }
	CompositeOperation* clone() const override { return new CompositeOperation(*this); }

	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;

	CompositeOperation(std::string name) : name(name) {}
	CompositeOperation(const CompositeOperation& o) { copy(o); }
	CompositeOperation(CompositeOperation&& o) { move(o); }
	~CompositeOperation() {	clear(); }
};

class Add : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Add(int paramR = 0, int paramG = 0, int paramB = 0) : paramR(paramR), paramG(paramG), paramB(paramB) {}

	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "+"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Add* clone() const override { return new Add(*this); }
};

class Sub : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Sub(int paramR = 0, int paramG = 0, int paramB = 0) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "-"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Sub* clone() const override { return new Sub(*this); }
};

class InverseSub : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	InverseSub(int paramR = 0, int paramG = 0, int paramB = 0) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "i-"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	InverseSub* clone() const override { return new InverseSub(*this); }
};

class Mul : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Mul(int paramR = 1, int paramG = 1, int paramB = 1) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "*"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Mul* clone() const override { return new Mul(*this); }
};

class Power : public BasicOperation {
private:
	double paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Power(double paramR = 1.0, double paramG = 1.0, double paramB = 1.0) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "pow"; };
	bool aware() const override { return false; }	
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Power* clone() const override { return new Power(*this); }
};

class Log : public BasicOperation {
private:
	double paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Log(double paramR = 10.0, double paramG = 10.0, double paramB = 10.0) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "log"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Log* clone() const override { return new Log(*this); }
};

class Div : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Div(int paramR = 1, int paramG = 1, int paramB = 1) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "/"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Div* clone() const override { return new Div(*this); }
};

//baci exception u SetParams() ako je 0
class InverseDiv : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	InverseDiv(int paramR = 1, int paramG = 1, int paramB = 1) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "i/"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	InverseDiv* clone() const override { return new InverseDiv(*this); }
};

class Min : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Min(int paramR = 255, int paramG = 255, int paramB = 255) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "min"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Min* clone() const override { return new Min(*this); }
};

class Max : public BasicOperation {
private:
	int paramR, paramG, paramB;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Max(int paramR = 0, int paramG = 0, int paramB = 0) : paramR(paramR), paramG(paramG), paramB(paramB) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "max"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 3; }
	Max* clone() const override { return new Max(*this); }
};


class Fill : public BasicOperation {
private:
	int paramR, paramG, paramB, paramA;
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Fill(int paramR = 0, int paramG = 0, int paramB = 0, int paramA = 0) : paramR(paramR), paramG(paramG), paramB(paramB), paramA(paramA) {}
	
	void appendParamsXML(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& currentNode) override;
	void convertXMLtoParams(rapidxml::xml_node<>* node) override;
	
	std::string getName() const override { return "fill"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override;
	int numOfParams() const override { return 4; }
	Fill* clone() const override { return new Fill(*this); }
};

class Greyscale : public BasicOperation {
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Greyscale() {}
	
	std::string getName() const override { return "greyscale"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override {}
	int numOfParams() const override { return 0; }
	Greyscale* clone() const override { return new Greyscale(*this); }
};

class BlackWhite : public BasicOperation {
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	BlackWhite() {}
	
	std::string getName() const override { return "blackwhite"; };
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override {}
	int numOfParams() const override { return 0; }
	BlackWhite* clone() const override { return new BlackWhite(*this); }
};

class Invert: public BasicOperation {
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Invert() {}
	
	std::string getName() const override { return "invert"; }
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override {}
	int numOfParams() const override { return 0; }
	Invert* clone() const override { return new Invert(*this); }
};

class Median : public BasicOperation {
protected:
	Pixel operatePixel(Layer* l, int x, int y) const override;
public:
	Median() {}
	
	std::string getName() const override { return "median"; }
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override {}
	int numOfParams() const override { return 0; }
	Median* clone() const override { return new Median(*this); }
};

class Abs : public BasicOperation {
protected:
	Pixel operatePixel(Layer *l, int x, int y) const override;
public:
	Abs() {}
	
	std::string getName() const override { return "abs"; }
	bool aware() const override { return false; }
	void setParams(std::vector<double> params) override {}	
	int numOfParams() const override { return 0; }
	Abs* clone() const override { return new Abs(*this); }
};