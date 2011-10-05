/**
 * \file flipFilter.cxx
 * \brief File per la realizzazione del flip delle immagini
 * \author ICAR-CNR Napoli
 */

#include "flipFilter.h"


flipFilter::flipFilter(unsigned int idData, dataHandler* dataHandler) : itkVtkFilter(idData, dataHandler) {
}


flipFilter::~flipFilter() {
}


void flipFilter::computeITK(char axis) {
	if (isValidData()) {
		typedef itk::FlipImageFilter<ImageType> FilterType;
		typedef FilterType::FlipAxesArrayType FlipAxesArrayType;

		FilterType::Pointer filter = FilterType::New();
		FlipAxesArrayType flipArray;
		
		typedef itk::FlipImageFilter < RGBImageType > RGBFilterType;
		typedef RGBFilterType::FlipAxesArrayType RGBFlipAxesArrayType;

		if(!getItkVtkData()->getRgb()) {
			FilterType::Pointer filter = FilterType::New();
			FlipAxesArrayType flipArray;

			switch (axis) {
				case 'x':
					flipArray[0]=1;
					flipArray[1]=0;
					flipArray[2]=0;
					break;
				case 'y':
					flipArray[0]=0;
					flipArray[1]=1;
					flipArray[2]=0;
					break;
				case 'z':
					flipArray[0]=0;
					flipArray[1]=0;
					flipArray[2]=1;
				default:
					flipArray[0]=0;
					flipArray[1]=0;
					flipArray[2]=0;
			}
			filter->SetFlipAxes( flipArray );
	  
			filter->SetInput(getItkVtkData()->getItkImage());
			filter->Update();
			update(filter->GetOutput());
		}
		else {
			RGBFilterType::Pointer filterRgb = RGBFilterType::New();
			RGBFlipAxesArrayType flipArrayRgb;

			switch (axis) {
				case 'x':
					flipArrayRgb[0]=1;
					flipArrayRgb[1]=0;
					flipArrayRgb[2]=0;
					break;
				case 'y':
					flipArrayRgb[0]=0;
					flipArrayRgb[1]=1;
					flipArrayRgb[2]=0;
					break;
				case 'z':
					flipArrayRgb[0]=0;
					flipArrayRgb[1]=0;
					flipArrayRgb[2]=1;
				default:
					flipArrayRgb[0]=0;
					flipArrayRgb[1]=0;
					flipArrayRgb[2]=0;
			}	
			filterRgb->SetFlipAxes( flipArrayRgb );

			filterRgb->SetInput(getItkVtkData()->getItkRgbImage());
			filterRgb->Update();
			update(filterRgb->GetOutput());
		}
	}
}


void flipFilter::computeVTK(char axis)
{
	if (isValidData()) {
	    if (axis == 'x' || axis == 'y' || axis == 'z') {
			int intAxis = -1;
			switch (axis) {
				case 'x':
					intAxis = 0;
				break;
				case 'y':
					intAxis = 1;
				break;
				case 'z':
					intAxis = 2;
				default:
					intAxis = -1;
			}	
			
			if (intAxis >= 0)
			{
				vtkImageFlip *vtkFlip = vtkImageFlip::New();
				vtkFlip->SetInput(getItkVtkData()->getVtkImage());
				vtkFlip->SetInformationInput(getItkVtkData()->getVtkImage());
				vtkFlip->SetFilteredAxes(intAxis);
				vtkFlip->SetInterpolationModeToCubic();
				vtkFlip->InterpolateOn();
				vtkFlip->Update();	
				(getItkVtkData()->getVtkImage())->DeepCopy(vtkFlip->GetOutput());					
				vtkFlip->Delete();
			}
		}
	}
}
