#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include "Rectangle.h"

class Selection {
private:
	std::vector<Rectangle> rectangles;
	bool active;
public:
	Selection(std::vector<Rectangle>& rectangles) :
		rectangles(rectangles), active(true) {}
	bool getActive() const { return active; }
	void setActive(bool active) { this->active = active; }
	bool inSelection(int x, int y) {
		return std::any_of(rectangles.begin(), rectangles.end(), [x,y](Rectangle r) {
			return r.inRect(x, y);
		});
	}
	void clamp(int width, int height) {
		for (Rectangle &r : rectangles) {
			r.clamp(width, height);
		}
	}
	auto begin() { return rectangles.begin(); }
	auto end() { return rectangles.end(); }

	friend std::ostream& operator<<(std::ostream& os, const Selection& s) {
		return os << (s.active ? "Active" : "Inactive");
	}
};