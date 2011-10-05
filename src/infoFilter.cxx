/**
 * \file infoFilter.cxx
 * \brief File per la gestione delle informazioni sulle immagini
 * \author ICAR-CNR Napoli
 */

#include "infoFilter.h"

infoFilter::infoFilter(unsigned int idData, dataHandler *dataHandler, int sliceNumber) : itkVtkFilter(idData, dataHandler) {
	_sliceNumber = sliceNumber;
}


infoFilter::~infoFilter() {
}


ImageType::PointType infoFilter::getImageOrigin() {
	if (isValidData()) {
		if(!getItkVtkData()->getRgb()) return getItkVtkData()->getItkImage()->GetOrigin();
		else return getItkVtkData()->getItkRgbImage()->GetOrigin();
	}
	return NULL;
}


ImageType::SpacingType infoFilter::getSpacingType() {
	if(!getItkVtkData()->getRgb()) return getItkVtkData()->getItkImage()->GetSpacing();
	else return getItkVtkData()->getItkRgbImage()->GetSpacing();
}


double infoFilter::getCurrentX(double xPos) {
	if (isValidData()) {
		return (xPos - getImageOrigin()[0])/getSpacingType()[0];
	}
	return 0;
}


double infoFilter::getCurrentY(double yPos) {
	if (isValidData()) {
		return (yPos - getImageOrigin()[1])/getSpacingType()[1];
	}
	return 0;
}


PixelType infoFilter::getPixelValue(double x, double y) {
	if (isValidData()) {
		ImageType::IndexType pixelIndex;
		pixelIndex[0] = x;
		pixelIndex[1] = y;
		ImageType::SizeType size = getItkVtkData()->getItkImage()->GetLargestPossibleRegion().GetSize();
		if (size[2] == 1)
			pixelIndex[2] =  0;
		else
			pixelIndex[2] = _sliceNumber;
		PixelType pixelValue = 0;
		if (x >= 0 && x < size[0] && y >= 0 && y < size[1])
			pixelValue = getItkVtkData()->getItkImage()->GetPixel(pixelIndex);
		return pixelValue;
	}
	return NULL;
}

RGBPixelType infoFilter::getPixelValueRgb(double x, double y) {
	RGBPixelType pixelValueRgb;
	if (isValidData()) {
		RGBImageType::IndexType pixelIndex;
		pixelIndex[0] = x;
		pixelIndex[1] = y;
		RGBImageType::SizeType size = getItkVtkData()->getItkRgbImage()->GetLargestPossibleRegion().GetSize();
		if (size[2] == 1)
			pixelIndex[2] =  0;
		else
			pixelIndex[2] = _sliceNumber;
		if (x >= 0 && x <= size[0] && y >= 0 && y <= size[1])
			pixelValueRgb = getItkVtkData()->getItkRgbImage()->GetPixel(pixelIndex);
		return pixelValueRgb;
	}
	else {
		pixelValueRgb.SetRed(0);
		pixelValueRgb.SetGreen(0);
		pixelValueRgb.SetBlue(0);
	}
	return pixelValueRgb;
}

ImageType::IndexType infoFilter::getXY(double x, double y) {
	ImageType::IndexType seed;
	seed[0] = 0;
	seed[1] = 0;
	seed[2] = 0;
	if (isValidData()) {
		seed[0] = (long)x;
		seed[1] = (long)y;
		seed[2] = _sliceNumber;
	}
	return seed;
}