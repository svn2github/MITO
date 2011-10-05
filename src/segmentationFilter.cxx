/**
 * \file segmentationFilter.cxx
 * \brief File per la definizione dei dati in ITK e in VTK
 * \author ICAR-CNR Napoli
 */

#include "segmentationFilter.h"
#include <itkCastImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkNeighborhoodConnectedImageFilter.h>
#include <itkConfidenceConnectedImageFilter.h>


segmentationFilter::segmentationFilter(unsigned int idData, dataHandler *dataHandler)
	: itkVtkFilter(idData, dataHandler) {
}


segmentationFilter::~segmentationFilter() {
}


void segmentationFilter::computeRegionGrowing(ImageType::IndexType xy, PixelType seedLowerLimit, PixelType seedUpperLimit, float xRadius, float yRadius, float multiplier, int numOfIterations, algoType algoType_) {
	if (isValidData()) {
		ImageType::Pointer outFilterSegmentation = ImageType::New();
		ImageType::IndexType seed;
		seed[0] = xy[0];
		seed[1] = xy[1];
		seed[2] = xy[2];
		
		
		typedef itk::ConnectedThresholdImageFilter<ImageType, ImageType> ConnectedFilterType ;
		typedef itk::NeighborhoodConnectedImageFilter<ImageType, ImageType> NeighborhoodConnectedFilter;
		typedef itk::ConfidenceConnectedImageFilter<ImageType, ImageType> ConfidenceConnectedFilter;
		typedef unsigned char OutputSegPixelType;
		typedef itk::Image< OutputSegPixelType, 3 > OutputSegImageType;
		typedef itk::CastImageFilter< ImageType, OutputSegImageType > CastingFilterType;
		
		ConnectedFilterType::Pointer connectedThresholdPointer = ConnectedFilterType::New();
		NeighborhoodConnectedFilter::Pointer neighborhoodConnectedPointer = NeighborhoodConnectedFilter::New();
		ConfidenceConnectedFilter::Pointer confidenceConnectedPointer = ConfidenceConnectedFilter::New();
		CastingFilterType::Pointer caster = CastingFilterType::New();

		switch (algoType_) {
			case connectedThreshold:

				connectedThresholdPointer->SetInput(getItkVtkData()->getItkImage());
				connectedThresholdPointer->SetLower(seedLowerLimit);
				connectedThresholdPointer->SetUpper(seedUpperLimit);
				connectedThresholdPointer->SetSeed(seed);
    			connectedThresholdPointer->SetReplaceValue(255);
				caster->SetInput(connectedThresholdPointer->GetOutput());	
				caster->Update();
				outFilterSegmentation = connectedThresholdPointer->GetOutput();	
				break;
			case neighborhoodConnected:
				ImageType::SizeType radiusSize;
				radiusSize[0] = xRadius;
				radiusSize[1] = yRadius;
				radiusSize[2] = 0;
				neighborhoodConnectedPointer->SetInput(getItkVtkData()->getItkImage());
				neighborhoodConnectedPointer->SetLower(seedLowerLimit);
				neighborhoodConnectedPointer->SetUpper(seedUpperLimit);
				neighborhoodConnectedPointer->SetRadius(radiusSize);
				neighborhoodConnectedPointer->SetSeed(seed);
				neighborhoodConnectedPointer->SetReplaceValue(255);
				caster->SetInput(neighborhoodConnectedPointer->GetOutput());	
				caster->Update();
				outFilterSegmentation = neighborhoodConnectedPointer->GetOutput();
				break;
			case confidenceConnected:
				confidenceConnectedPointer->SetInput(getItkVtkData()->getItkImage());
	    		confidenceConnectedPointer->SetMultiplier(multiplier);
				confidenceConnectedPointer->SetNumberOfIterations(numOfIterations);
	    		confidenceConnectedPointer->SetSeed(seed);
    			confidenceConnectedPointer->SetReplaceValue(255);
				confidenceConnectedPointer->SetInitialNeighborhoodRadius(xRadius);
				caster->SetInput(confidenceConnectedPointer->GetOutput());	
				caster->Update();
				outFilterSegmentation = confidenceConnectedPointer->GetOutput();
				break;
			default:
				return;
		}
		
		ImageType::IndexType pixelIndex;
		PixelType pixelValue;
		ImageType::SizeType size = getItkVtkData()->getItkImage()->GetLargestPossibleRegion().GetSize(); 
		int minPixelValue = getItkVtkData()->getMinPixelValue();
		if(size[2]==1) {
            for (int x=0;x<size[0];x++) {
				for (int y=0;y<size[1];y++) {   
					pixelIndex[0] = x;
					pixelIndex[1] = y;
					pixelIndex[2] = 0;
					if(caster->GetOutput()->GetPixel(pixelIndex) == 255)
						pixelValue = getItkVtkData()->getItkImage()->GetPixel(pixelIndex);
					else
						pixelValue = minPixelValue;
					
					outFilterSegmentation->SetPixel(pixelIndex, pixelValue);
				}
			}
		}
		else {
			for(int z=0;z<size[2];z++) {
				for (int x=0;x<size[0];x++) {  
					for (int y=0;y<size[1];y++) {   
						pixelIndex[0] = x;
						pixelIndex[1] = y;
						pixelIndex[2] = z;
						if(caster->GetOutput()->GetPixel(pixelIndex) == 255)
							pixelValue = getItkVtkData()->getItkImage()->GetPixel(pixelIndex);
						else
						pixelValue = minPixelValue;

						outFilterSegmentation->SetPixel(pixelIndex, pixelValue);

					}
				}
			}
		}
		update(outFilterSegmentation);
	}
}