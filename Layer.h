#pragma once
#include <vector>
#include "Pixel.h"
#include "Selection.h"

class Operation;

class Layer {
public:
	class DoneOperation {
	private:
		Operation *operation;
		std::vector<Selection*> selections;
	public:
		DoneOperation(Operation *operation, const std::vector<Selection*>& selections, int clampWidth, int clampHeight);
		~DoneOperation() { delete operation; for (Selection* s : selections) delete s; }

		Operation* getOperation() const { return operation; }
		const std::vector<Selection*>&  getSelections() const { return selections; }
	};

private:
	std::vector<std::vector<Pixel>> pixels;
	std::vector<DoneOperation *> doneOperations;
	int opacity;
	bool active, visible;
	std::string path;
public:
	Layer(int width, int height, const std::string& path = "") :
		pixels(height, std::vector <Pixel>(width, Pixel())), opacity(100), active(true), visible(true), path(path) {}
	~Layer() { for (DoneOperation* o : doneOperations) delete o; }
	
	void resize(int width = -1, int height = -1);

	bool getActive() const { return active; }
	bool getVisible() const { return visible; }
	int getOpacity() const { return opacity; }
	int getHeight() const { return pixels.size(); }
	int getWidth() const { return pixels[0].size(); }
	const std::string& getPath() const { return path; }
	const std::vector<DoneOperation*>& getDoneOperations() const { return doneOperations; }


	void setActive(bool active) { this->active = active; }
	void setVisible(bool visible) { this->visible = visible; }
	void setOpacity(int opacity) { this->opacity = opacity >= 100 ? 100 : (opacity <= 0 ? 0 : opacity); }


	void addOperation(Operation *o, const std::vector<Selection *>& selections);
	void clamp();

	std::vector<Pixel>& operator[](int i) { return pixels[i]; }
	friend std::ostream& operator<<(std::ostream& os, const Layer& l);
};