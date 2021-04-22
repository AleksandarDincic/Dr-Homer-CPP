#pragma once
#include "Formatter.h"

class BMPFormatter : public Formatter {
public:
	fileType getType() const override { return IMAGE; }
	Layer* load(std::string path) const override;
	void save(std::string path) const override;
};