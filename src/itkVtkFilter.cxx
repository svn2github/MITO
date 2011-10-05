/**
 * \file itkVtkFilter.cxx
 * \brief File per la gestione dei dati in ITK e in VTK
 * \author ICAR-CNR Napoli
 */

#include "itkVtkFilter.h"

itkVtkFilter::itkVtkFilter(unsigned int idData, dataHandler *dataHandler) {
	_idData = idData;
	_dataHandler = dataHandler;

	_itkVtkData = (itkVtkData*)dataHandler->getData(_idData);

	if (_itkVtkData)
		_isValidData = true;
	else
		_isValidData = false;
}

itkVtkFilter::~itkVtkFilter() {
}


void itkVtkFilter::update(itk::ImageSource<ImageType>::Pointer p) {
	if (isValidData()) {
		p->Update();
		getItkVtkData()->setItkImage(p->GetOutput());
		getItkVtkData()->getItkVtkConnector()->SetInput(getItkVtkData()->getItkImage());
		getItkVtkData()->setVtkImage(getItkVtkData()->getItkVtkConnector()->GetOutput());
	}
		
}


void itkVtkFilter::update(ImageType::Pointer p) {
	if (isValidData()) {
		getItkVtkData()->setItkImage(p);
		getItkVtkData()->getItkVtkConnector()->SetInput(p);
		getItkVtkData()->setVtkImage(getItkVtkData()->getItkVtkConnector()->GetOutput());
	}
}


void itkVtkFilter::update(itk::ImageSource<RGBImageType>::Pointer p) {
	if (isValidData()) {
		p->Update();
		getItkVtkData()->setItkRgbImage(p->GetOutput());
		getItkVtkData()->getItkVtkRgbConnector()->SetInput(getItkVtkData()->getItkRgbImage());
		getItkVtkData()->setVtkImage(getItkVtkData()->getItkVtkRgbConnector()->GetOutput());
	}
}


void itkVtkFilter::update(RGBImageType::Pointer p) {
	if (isValidData()) {
		getItkVtkData()->setItkRgbImage(p);
		getItkVtkData()->getItkVtkRgbConnector()->SetInput(p);
		getItkVtkData()->setVtkImage(getItkVtkData()->getItkVtkRgbConnector()->GetOutput());
	}
}