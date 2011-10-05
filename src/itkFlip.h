#ifndef _itkFlip_h
#define _itkFlip_h

#include <itkFlipImageFilter.h>

class itkFlip {

public:
	inline ImageType::Pointer computeFlipping(ImageType::Pointer itkImage) {
		typedef itk::FlipImageFilter < ImageType > FilterType;
		FilterType::Pointer filter = FilterType::New();
		typedef FilterType::FlipAxesArrayType FlipAxesArrayType;
		FlipAxesArrayType flipArray;
		flipArray[0]=0;
		flipArray[1]=1;
		flipArray[2]=0;
		filter->SetFlipAxes( flipArray );
		filter->SetInput(itkImage);
		filter->Update();
		return filter->GetOutput();
	}

	inline RGBImageType::Pointer computeFlipping(RGBImageType::Pointer itkRgbImage) {
		typedef itk::FlipImageFilter < RGBImageType > FilterType;
		FilterType::Pointer filter = FilterType::New();
		typedef FilterType::FlipAxesArrayType FlipAxesArrayType;
		FlipAxesArrayType flipArray;
		flipArray[0]=0;
		flipArray[1]=1;
		flipArray[2]=0;
		filter->SetFlipAxes( flipArray );
		filter->SetInput(itkRgbImage);
		filter->Update();
		return filter->GetOutput();
	}

};

#endif