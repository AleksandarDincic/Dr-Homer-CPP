#pragma once
#include <map>
#include "Layer.h"
#include "Operation.h"

class Selection;

class Image {
private:
	static Image* image;

	int width, height;
	std::vector<Layer*> layers;
	std::vector<Operation*> operations;
	std::map<std::string, Selection*> selections;
	std::map<std::string, CompositeOperation*> compositeOperations;

	Image() : width(0), height(0) {}

	void resize(Layer *l);
public:
	~Image();

	static Image* getImage();
	static void loadImage(const std::string& path);
	static void deleteImage() { delete image; image = nullptr; }
	
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	Layer& getLayer(int pos) const { return *layers[pos]; };
	const std::map<std::string, Selection*>& getSelections() const { return selections; }
	const std::map<std::string, CompositeOperation*>& getCompositeOperations() const { return compositeOperations; }
	CompositeOperation* getCompositeOperation(const std::string& name);
	
	void addLayer(Layer *l) { if (l) { resize(l); layers.insert(layers.begin(), l); } }
	void addLayerBottom(Layer *l) { if (l) { resize(l); layers.push_back(l); } }
	void addLayer(int width, int height);
	void addLayer(std::string path);

	void setLayerOpacity(int pos, int opacity);
	void setLayerActive(int pos, bool active);
	void setLayerVisible(int pos, bool visible);
	void deleteLayer(int pos);

	Layer* importLayer(const std::string& path);

	void addOperation(Operation* operation) { operations.push_back(operation); }
	
	void operate();
	void clearOperations();

	void addSelection(std::vector<Rectangle>& rects, std::string name);
	void setSelectionActive(const std::string& name, bool active);
	void deleteSelection(const std::string& name);

	void printCompositeOperations();
	void saveCompositeOperation(const std::string& name);
	void saveCompositeOperation(CompositeOperation *o);
	void deleteCompositeOperation(const std::string& name);
	void importCompositeOperation(const std::string& path, bool deleteOld);
	void exportCompositeOperation(const std::string& name, const std::string& path);
	
	void Export(std::string path);
	void saveProject(const std::string& path);

	Pixel getPixel(int width, int height);

	auto begin() { return layers.begin(); }
	auto end() { return layers.end(); }

	friend std::ostream& operator<<(std::ostream& o, const Image& i);
};