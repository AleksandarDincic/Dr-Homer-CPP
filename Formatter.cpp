#include "Formatter.h"

std::map<std::string, ImageFormatter*> ImageFormatter::formatMap;
std::map<std::string, OperationFormatter*> OperationFormatter::formatMap;
std::map<std::string, ProjectFormatter*> ProjectFormatter::formatMap;

void ImageFormatter::addFormat(std::string fileType, ImageFormatter * formatter)
{
	if (formatMap.find(fileType) == formatMap.end()) {
		formatMap[fileType] = formatter;
	}
	// u suprotnom exception
}

ImageFormatter * ImageFormatter::getFormatter(std::string fileType)
{
	if (formatMap.find(fileType) != formatMap.end())
		return formatMap[fileType];
	return nullptr;
}

void OperationFormatter::addFormat(std::string fileType, OperationFormatter * formatter)
{
	if (formatMap.find(fileType) == formatMap.end()) {
		formatMap[fileType] = formatter;
	}
}

OperationFormatter * OperationFormatter::getFormatter(std::string fileType)
{
	if (formatMap.find(fileType) != formatMap.end())
		return formatMap[fileType];
	return nullptr;
}

void ProjectFormatter::addFormat(std::string fileType, ProjectFormatter * formatter)
{
	if (formatMap.find(fileType) == formatMap.end()) {
		formatMap[fileType] = formatter;
	}
}

ProjectFormatter * ProjectFormatter::getFormatter(std::string fileType)
{
	if (formatMap.find(fileType) != formatMap.end())
		return formatMap[fileType];
	return nullptr;
}
