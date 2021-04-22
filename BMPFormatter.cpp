#include <fstream>
#include "Image.h"
#include "Formatter.h"
#include "Exceptions.h"

Layer * BMPFormatter::load(const std::string& path)
{
	char header[14];
	char dibStart[4];

	std::ifstream FILE(path, std::ifstream::binary | std::ifstream::in);
	if (!FILE.is_open()) throw BadPathException("File does not exist");

	FILE.read(header, 14);
	FILE.read(dibStart, 4);

	unsigned dibSize = (int)(unsigned char)dibStart[0] + ((int)(unsigned char)dibStart[1] << 8) +
		((int)(unsigned char)dibStart[2] << 16) + ((int)(unsigned char)dibStart[3] << 24);

	char *dib = new char[dibSize - 4];

	FILE.read(dib, dibSize - 4);

	unsigned int imageWidth = (int)(unsigned char)dib[0] + ((int)(unsigned char)dib[1] << 8) +
		((int)(unsigned char)dib[2] << 16) + ((int)(unsigned char)dib[3] << 24);

	unsigned int imageHeight = (int)(unsigned char)dib[4] + ((int)(unsigned char)dib[5] << 8) +
		((int)(unsigned char)dib[6] << 16) + ((int)(unsigned char)dib[7] << 24);

	unsigned int pixelSize = (int)(unsigned char)dib[10] + ((int)(unsigned char)dib[11] << 8);
	if (pixelSize != 24 && pixelSize != 32)
		throw BadFormatException("Only RGB and RGBA BMP files are supported");

	Layer *l = new Layer(imageWidth, imageHeight, path);
	
	unsigned pixelByteSize = pixelSize / 8;
	unsigned rowSize = (pixelSize * imageWidth + 31) / 32 * 4;

	char *pixelBuffer = new char[pixelByteSize];

	int compression = (int)(unsigned char)dib[12] + ((int)(unsigned char)dib[13] << 8) +
		((int)(unsigned char)dib[14] << 16) + ((int)(unsigned char)dib[15] << 24);

	if (compression == 3) {
		unsigned int redMask = (int)(unsigned char)dib[36] + ((int)(unsigned char)dib[37] << 8) +
			((int)(unsigned char)dib[38] << 16) + ((int)(unsigned char)dib[39] << 24);
		unsigned int greenMask = (int)(unsigned char)dib[40] + ((int)(unsigned char)dib[41] << 8) +
			((int)(unsigned char)dib[42] << 16) + ((int)(unsigned char)dib[43] << 24);
		unsigned int blueMask = (int)(unsigned char)dib[44] + ((int)(unsigned char)dib[45] << 8) +
			((int)(unsigned char)dib[46] << 16) + ((int)(unsigned char)dib[47] << 24);
		unsigned int alphaMask = (int)(unsigned char)dib[48] + ((int)(unsigned char)dib[49] << 8) +
			((int)(unsigned char)dib[50] << 16) + ((int)(unsigned char)dib[51] << 24);

		for (int i = 0; i < imageHeight; i++) {
			for (int j = 0; j < imageWidth; j++) {
				unsigned int tempMask;
				FILE.read(pixelBuffer, pixelByteSize);
				unsigned int readPixel = (int)(unsigned char)pixelBuffer[0] + ((int)(unsigned char)pixelBuffer[1] << 8) +
					((int)(unsigned char)pixelBuffer[2] << 16) + ((int)(unsigned char)pixelBuffer[3] << 24);
				
				unsigned int readRed = readPixel & redMask;
				tempMask = redMask;
				while (!(tempMask & 1)) { readRed >>= 1, tempMask >>= 1; }

				unsigned int readGreen = readPixel & greenMask;
				tempMask = greenMask;
				while (!(tempMask & 1)) { readGreen >>= 1, tempMask >>= 1; }

				unsigned int readBlue = readPixel & blueMask;
				tempMask = blueMask;
				while (!(tempMask & 1)) { readBlue >>= 1, tempMask >>= 1; }

				unsigned int readAlpha = readPixel & alphaMask;
				tempMask = alphaMask;
				while (!(tempMask & 1)) { readAlpha >>= 1, tempMask >>= 1; }

				(*l)[i][j] = Pixel(readRed,readGreen,readBlue,readAlpha);
			}
			for (int j = 0; j < rowSize - pixelByteSize * imageWidth; j++)
				FILE.read(pixelBuffer, 1);
		}
	
	}
	else {
		for (int i = 0; i < imageHeight; i++) {
			for (int j = 0; j < imageWidth; j++) {
				FILE.read(pixelBuffer, pixelByteSize);
				(*l)[i][j] = Pixel((unsigned char)pixelBuffer[2],
					(unsigned char)pixelBuffer[1], (unsigned char)pixelBuffer[0], 255);
			}
			for (int j = 0; j < rowSize - pixelByteSize * imageWidth; j++)
				FILE.read(pixelBuffer, 1);
		}
	}

	FILE.close();
	return l;
}

void BMPFormatter::save(const std::string& path)
{
	std::ofstream FILE(path, std::ofstream::binary | std::ofstream::out);
	Image *i = Image::getImage();

	unsigned int bmpSize = i->getWidth()*i->getHeight() * 4 + 70;
	
	char header[14];

	//BM
	header[0] = 66, header[1] = 77; 
	//velicina bitmape u bajtovima
	header[2] = bmpSize & 0xFF, header[3] = bmpSize >> 8 & 0xFF, header[4] = bmpSize >> 16 & 0xFF,
		header[5] = bmpSize >> 24 & 0xFF; 
	//djubre
	header[6] = header[7] = header[8] = header[9] = 0;
	//offset gde pocinju pikseli -> fiksno
	header[10] = 70, header[11] = header[12] = header[13] = 0;

	FILE.write(header, 14);

	char dib[56];
	
	//velicina DIB -> fiksno
	dib[0] = 56, dib[1] = dib[2] = dib[3] = 0;
	
	//sirina slike
	dib[4] = ((unsigned)i->getWidth()) & 0xFF, dib[5] = ((unsigned)i->getWidth()) >> 8 & 0xFF,
		dib[6] = ((unsigned)i->getWidth()) >> 16 & 0xFF, dib[7] = ((unsigned)i->getWidth()) >> 24 & 0xFF;

	//visina slike
	dib[8] = ((unsigned)i->getHeight()) & 0xFF, dib[9] = ((unsigned)i->getHeight()) >> 8 & 0xFF,
		dib[10] = ((unsigned)i->getHeight()) >> 16 & 0xFF, dib[11] = ((unsigned)i->getHeight()) >> 24 & 0xFF;

	//plane neki -> fiksno
	dib[12] = 1, dib[13] = 0;

	//broj bita po pikselu
	dib[14] = 32, dib[15] = 0;

	//BI_BITFIELDS
	dib[16] = 3, dib[17] = dib[18] = dib[19] = 0;

	//velica dela s bitovima
	dib[20] = (bmpSize-70) & 0xFF, dib[21] = (bmpSize - 70) >> 8 & 0xFF,
		dib[22] = (bmpSize - 70) >> 16 & 0xFF, dib[23] = (bmpSize - 70) >> 24 & 0xFF;

	//rezolucija neka -> fiksno
	dib[24] = 0x13, dib[25] = 0x0B, dib[26] = dib[27] = 0;

	//rezolucija opet
	dib[28] = 0x13, dib[29] = 0x0B, dib[30] = dib[31] = 0;

	//neke boje -> fiksno
	dib[32] = dib[33] = dib[34] = dib[35] = 0;

	//neke boje bitne -> fiksno
	dib[36] = dib[37] = dib[38] = dib[39] = 0;

	//red maska
	dib[40] = dib[41] = 0, dib[42] = 0xFF, dib[43] = 0;

	//green maska
	dib[44] = 0, dib[45] = 0xFF, dib[46] = dib[47] = 0;
	
	//blue maska
	dib[48] = 0xFF, dib[49] = dib[50] = dib[51] = 0;

	//alpha maska
	dib[52] = dib[53] = dib[54] =  0, dib[55] = 0xFF;

	FILE.write(dib, 56);

	char pixelBuffer[4];

	for (int j = 0; j < i->getHeight(); j++) {
		for (int k = 0; k < i->getWidth(); k++) {
			Pixel tempPixel = i->getPixel(k, j);
			pixelBuffer[0] = tempPixel.getB();
			pixelBuffer[1] = tempPixel.getG();
			pixelBuffer[2] = tempPixel.getR();
			pixelBuffer[3] = tempPixel.getA();

			FILE.write(pixelBuffer, 4);
		}
	}

	FILE.close();
}
