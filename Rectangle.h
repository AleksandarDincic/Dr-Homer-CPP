#pragma once

class Rectangle {
private:
	int x, y, width, height;
public:
	Rectangle(int x, int y, int width, int height) :
		x(x), y(y), width(width), height(height) {}

	int getX() const { return x; }
	int getY() const { return y; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }

	bool inRect(int x, int y) const {
		return (x >= this->x && x < this->x + width) && (y <= this->y && y > this->y - height);
	}
	void clamp(int width, int height) {
		if (x > width - 1 || y < 0) { x = y = -1; this->width = this->height = 0; }
		if (this->width > width - x) { this->width = width - x;	}
		if (y > height - 1) { this->height -= y - height + 1; y = height - 1; }
	}
};