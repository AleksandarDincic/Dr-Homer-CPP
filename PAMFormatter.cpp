#include <fstream>
#include <iostream>
#include "Image.h"
#include "Formatter.h"
#include "Exceptions.h"

Layer * PAMFormatter::load(const std::string& path)
{
	std::ifstream FILE(path, std::ifstream::binary | std::ifstream::in);
	if (!FILE.is_open()) throw BadPathException("File does not exist");
	char HDR[9]; 	
	FILE.read(HDR, 3); //P7\n

	FILE.read(HDR, 6); //WIDTH_

	int imageWidth = 0;
	char numChar;

	FILE.read(&numChar, 1);
	while (numChar != 10) {
		imageWidth *= 10;
		imageWidth += numChar - '0';
		FILE.read(&numChar, 1);
	}

	FILE.read(HDR, 7); //HEIGHT_

	int imageHeight = 0;

	FILE.read(&numChar, 1);
	while (numChar != 10) {
		imageHeight *= 10;
		imageHeight += numChar - '0';
		FILE.read(&numChar, 1);
	}

	FILE.read(HDR, 6); //DEPTH_

	int depth;
	FILE.read(&numChar, 1);
	depth = numChar - '0';

	FILE.read(&numChar, 1); //\n

	FILE.read(HDR, 7); //MAXVAL_

	int maxval = 0;

	FILE.read(&numChar, 1);
	while (numChar != 10) {
		maxval *= 10;
		maxval += numChar - '0';
		FILE.read(&numChar, 1);
	}

	if (maxval > 255) throw BadFormatException("Only PAM files with colors in range 0-255 are supported");

	FILE.read(HDR, 9); //TUPLTYPE_

	char tuple[20];
	int tupleCNT = 0;

	FILE.read(&numChar, 1);
	while (numChar != 10) {
		tuple[tupleCNT++] = numChar;
		FILE.read(&numChar, 1);
	}
	tuple[tupleCNT] = 0;

	std::string tupleStr = tuple;

	FILE.read(HDR, 7); //ENDHDR\n

	Layer *l = new Layer(imageWidth, imageHeight, path);

	if (tupleStr == "RGB_ALPHA") {
		int r, g, b, a;
		for (int i = imageHeight - 1; i >=0; i--) {
			for (int j = 0; j < imageWidth; j++) {
				FILE.read(&numChar, 1);
				r = (unsigned char)numChar; 
				FILE.read(&numChar, 1);
				g = (unsigned char)numChar;
				FILE.read(&numChar, 1);
				b = (unsigned char)numChar;
				FILE.read(&numChar, 1);
				a = (unsigned char)numChar;

				(*l)[i][j] = Pixel(r, g, b, a);
			}
		}
	}
	else if (tupleStr == "RGB") {
		int r, g, b;
		for (int i = imageHeight - 1; i >= 0; i--) {
			for (int j = 0; j < imageWidth; j++) {
				FILE.read(&numChar, 1);
				r = (unsigned char)numChar;
				FILE.read(&numChar, 1);
				g = (unsigned char)numChar;
				FILE.read(&numChar, 1);
				b = (unsigned char)numChar;

				(*l)[i][j] = Pixel(r, g, b, 255);
			}
		}
	}
	else throw BadFormatException("Only RGB and RGBA PAM files are supported");

	FILE.close();
	return l;
}

void PAMFormatter::save(const std::string& path)
{
	std::ofstream FILE(path, std::ofstream::binary | std::ofstream::out);
	Image *i = Image::getImage();

	char HDR[25];

	//P7\n
	HDR[0] = 80, HDR[1] = 55, HDR[2] = 10;
	FILE.write(HDR, 3);

	int width = i->getWidth();
	int height = i->getHeight();

	//WIDTH_
	HDR[0] = 87, HDR[1] = 73, HDR[2] = 68, HDR[3] = 84, HDR[4] = 72, HDR[5] = 32;
	FILE.write(HDR, 6);
	
	int digitCounter = 0;

	while (width > 0) {
		HDR[digitCounter++] = '0' + width % 10;
		width /= 10;
	}
	for (int ii = 0, j = digitCounter - 1; ii < j; ii++, j--) {
		char t = HDR[ii];
		HDR[ii] = HDR[j];
		HDR[j] = t;
	}

	FILE.write(HDR, digitCounter);

	HDR[0] = 10;
	FILE.write(HDR, 1);

	//HEIGHT_
	HDR[0] = 72, HDR[1] = 69, HDR[2] = 73, HDR[3] = 71, HDR[4] = 72, HDR[5] = 84, HDR[6] = 32;
	FILE.write(HDR, 7);

	digitCounter = 0;

	while (height > 0) {
		HDR[digitCounter++] = '0' + height % 10;
		height /= 10;
	}
	for (int ii = 0, j = digitCounter - 1; ii < j; ii++, j--) {
		char t = HDR[ii];
		HDR[ii] = HDR[j];
		HDR[j] = t;
	}

	FILE.write(HDR, digitCounter);

	HDR[0] = 10;
	FILE.write(HDR, 1);

	//DEPTH_4\n
	HDR[0] = 68, HDR[1] = 69, HDR[2] = 80, HDR[3] = 84, HDR[4] = 72, HDR[5] = 32, HDR[6]= '0' + 4, HDR[7]=10;
	FILE.write(HDR, 8);

	//MAXVAL_255\n
	HDR[0] = 77, HDR[1] = 65, HDR[2] = 88, HDR[3] = 86, HDR[4] = 65, HDR[5] = 76, HDR[6] = 32, 
		HDR[7] = '0' + 2, HDR[8] = '0' + 5, HDR[9] = '0' + 5, HDR[10] = 10;
	FILE.write(HDR, 11);

	//TUPLTYPE_RGB__ALPHA\n
	HDR[0] = 84, HDR[1] = 85, HDR[2] = 80, HDR[3] = 76, HDR[4] = 84, HDR[5] = 89, HDR[6] = 80, HDR[7] = 69, HDR[8] = 32,
		HDR[9] = 82, HDR[10] = 71, HDR[11] = 66, HDR[12] = 95, HDR[13] = 65, HDR[14] = 76, HDR[15] = 80, HDR[16] = 72, HDR[17] = 65, HDR[18] = 10;

	FILE.write(HDR, 19);

	//ENDHDR\n
	HDR[0] = 69, HDR[1] = 78, HDR[2] = 68, HDR[3] = 72, HDR[4] = 68, HDR[5] = 82, HDR[6] = 10;
	FILE.write(HDR, 7);

	for (int j = i->getHeight() - 1; j >= 0; j--) {
		for (int k = 0; k < i->getWidth(); k++) {
			Pixel tempPixel = i->getPixel(k, j);
			HDR[0] = tempPixel.getR();
			HDR[1] = tempPixel.getG();
			HDR[2] = tempPixel.getB();
			HDR[3] = tempPixel.getA();

			FILE.write(HDR, 4);
		}
	}

	FILE.close();
}
