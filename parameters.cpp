#include "parameters.h"

namespace Sndspec {

std::vector<std::string> Parameters::getInputFiles() const
{
	return inputFiles;
}

void Parameters::setInputFiles(const std::vector<std::string> &value)
{
	inputFiles = value;
}

std::string Parameters::getOutputPath() const
{
	return outputPath;
}

void Parameters::setOutputPath(const std::string &value)
{
	outputPath = value;
}

int Parameters::getImgWidth() const
{
	return imgWidth;
}

void Parameters::setImgWidth(int value)
{
	imgWidth = value;
}

int Parameters::getIngHeight() const
{
	return ingHeight;
}

void Parameters::setIngHeight(int value)
{
	ingHeight = value;
}

double Parameters::getDynRange() const
{
	return dynRange;
}

void Parameters::setDynRange(double value)
{
	dynRange = value;
}

int Parameters::getFftSize() const
{
	return fftSize;
}

void Parameters::setFftSize(int value)
{
	fftSize = value;
}

} // namespace Sndspec

