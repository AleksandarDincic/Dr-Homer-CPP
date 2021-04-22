#pragma once

class Pixel {
private:
	int r, g, b, a;
public:
	Pixel(int r = 0, int g = 0, int b = 0, int a = 0) :
		r(r), g(g), b(b), a(a) {}

	int getR() const { return r; };
	int getG() const { return g; };
	int getB() const { return b; };
	int getA() const { return a; };
	
	void clamp() {
		r = r >= 255 ? 255 : (r <= 0 ? 0 : r);
		g = g >= 255 ? 255 : (g <= 0 ? 0 : g);
		b = b >= 255 ? 255 : (b <= 0 ? 0 : b);
	}
};