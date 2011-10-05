/**
 * \file volumeRenderingFilter.cxx
 * \brief File per la realizzazione del volume rendering
 * \author ICAR-CNR Napoli
 */

#include "volumeRenderingFilter.h"
#include "colorTransferTable.h"
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkImageShiftScale.h>
#include <vtkPointData.h>
#include <vtkDataSetWriter.h>
#include <vtkDataObjectWriter.h>

#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolumeTextureMapper2D.h>
//#include <vtkVolumeRayCastIsosurfaceFunction.h>

volumeRenderingFilter::volumeRenderingFilter(unsigned int idData, dataHandler *dataHandler) : itkVtkFilter(idData, dataHandler) {
}

volumeRenderingFilter::~volumeRenderingFilter() {
}

// usa vtkVolumeRayCastMapper
vtkVolume *volumeRenderingFilter::compute(long wl, long ww, int clut, int minPixel) {
	if (isValidData()) {
		getItkVtkData()->setVtkActor(0);

		/*vtkDataSetWriter *dataSetWriter = vtkDataSetWriter::New();
		dataSetWriter->SetInput(getItkVtkData()->getVtkImage());
		dataSetWriter->SetFileName("volumeNoShift.vtk");
		dataSetWriter->Write();*/

		int shiftValue = 0;
		if( minPixel < 0 ) 
			shiftValue =(-1)*(minPixel);

		vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
		shiftScale->SetInput(getItkVtkData()->getVtkImage());
		shiftScale->SetScale(1);
		shiftScale->SetShift(shiftValue);
		shiftScale->ClampOverflowOn();
		shiftScale->SetOutputScalarTypeToUnsignedShort();

		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();

		/*double alpha[256];
		for(int i = 0; i < 256; i++)
		{
			alpha[i] = i / 255.;
		}*/


		float start = shiftValue + wl - ww/2, end = shiftValue + wl + ww/2;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		double table[256][3];
		if(clut == 0) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = i / 255.;
				table[i][1] = i / 255.;
				table[i][2] = i / 255.;
			}
		}
		else if(clut == 1) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = BlackBody_Red[i] / 255.;
				table[i][1] = BlackBody_Green[i] / 255.;
				table[i][2] = BlackBody_Blue[i] / 255.;
			}
		}
		else if(clut == 2) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Cardiac_Red[i] / 255.;
				table[i][1] = Cardiac_Green[i] / 255.;
				table[i][2] = Cardiac_Blue[i] / 255.;
			}
		}
		else if(clut == 3) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Flow_Red[i] / 255.;
				table[i][1] = Flow_Green[i] / 255.;
				table[i][2] = Flow_Blue[i] / 255.;
			}
		}
		else if(clut == 4) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GEColor_Red[i] / 255.;
				table[i][1] = GEColor_Green[i] / 255.;
				table[i][2] = GEColor_Blue[i] / 255.;
			}
		}
		else if(clut == 5) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GrainRainbow_Red[i] / 255.;
				table[i][1] = GrainRainbow_Green[i] / 255.;
				table[i][2] = GrainRainbow_Blue[i] / 255.;
			}
		}
		else if(clut == 6) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = HotIron_Red[i] / 255.;
				table[i][1] = HotIron_Green[i] / 255.;
				table[i][2] = HotIron_Blue[i] / 255.;
			}
		}
		else if(clut == 7) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = NIH_Red[i] / 255.;
				table[i][1] = NIH_Green[i] / 255.;
				table[i][2] = NIH_Blue[i] / 255.;
			}
		}
		else if(clut == 8) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Spectrum_Red[i] / 255.;
				table[i][1] = Spectrum_Green[i] / 255.;
				table[i][2] = Spectrum_Blue[i] / 255.;
			}
		}
		else if(clut == 9) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRBones_Red[i] / 255.;
				table[i][1] = VRBones_Green[i] / 255.;
				table[i][2] = VRBones_Blue[i] / 255.;
			}
		}
		else if(clut == 10) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRMusclesBones_Red[i] / 255.;
				table[i][1] = VRMusclesBones_Green[i] / 255.;
				table[i][2] = VRMusclesBones_Blue[i] / 255.;
			}
		}
		else if(clut == 11) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRRedVessels_Red[i] / 255.;
				table[i][1] = VRRedVessels_Green[i] / 255.;
				table[i][2] = VRRedVessels_Blue[i] / 255.;
			}
		}

		//opacityTransferFunction->BuildFunctionFromTable(wl-ww/2, wl+ww/2, 255, (double*)&alpha);
		colorTransferFunction->BuildFunctionFromTable(shiftValue + wl-ww/2, shiftValue + wl+ww/2, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetInterpolationTypeToLinear();
		
		vtkVolume *volume = vtkVolume::New();
		volume->SetProperty(volumeProperty);
		
		//alpha compositing technique
		vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
		
		//isosurfaces on the volume
		//vtkVolumeRayCastIsosurfaceFunction *compositeFunction = vtkVolumeRayCastIsosurfaceFunction::New();
		//compositeFunction->SetIsoValue(100);

		vtkVolumeRayCastMapper *volumeRaycastMapper = vtkVolumeRayCastMapper::New();

		volumeRaycastMapper->SetMinimumImageSampleDistance(1.5);
		volumeRaycastMapper->SetSampleDistance(1.5);
		//volumeRaycastMapper->SetMinimumImageSampleDistance(1);
		//volumeRaycastMapper->SetMaximumImageSampleDistance(1);
		//volumeRaycastMapper->SetSampleDistance(1);

		volumeRaycastMapper->SetVolumeRayCastFunction(compositeFunction);

		// se lo si attiva INTERMIX A OFF allora il cursore non interseca il volume
		//volumeRaycastMapper->IntermixIntersectingGeometryOff();

		volumeRaycastMapper->SetInput(shiftScale->GetOutput());
		volume->SetMapper(volumeRaycastMapper);
		
		volumeRaycastMapper->Delete();
		compositeFunction->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
		shiftScale->Delete();

		return volume;
	}
	else return NULL;
}

// usa vtkFixedPointVolumeRayCastMapper (più performante)
vtkVolume *volumeRenderingFilter::computeRgb(long wl, long ww, int clut) {
	if (isValidData()) {
		getItkVtkData()->setVtkActor(0);

		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
		float start = wl - ww/2, end = wl + ww/2;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		double tableRed[256][3];
		double tableGreen[256][3];
		double tableBlue[256][3];
		double table[256][3];

		if(clut == 0) {
			for(int i = 0; i < 256; i++)
			{
				tableRed[i][0] = i / 255.;
				tableRed[i][1] = 0.;
				tableRed[i][2] = 0.;
			}
			for(int i = 0; i < 256; i++)
			{
				tableGreen[i][0] = 0.;
				tableGreen[i][1] = i / 255.;
				tableGreen[i][2] = 0.;
			}
			for(int i = 0; i < 256; i++)
			{
				tableBlue[i][0] = 0.;
				tableBlue[i][1] = 0.;
				tableBlue[i][2] = i / 255.;
			}
		}
		else if(clut == 1) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = BlackBody_Red[i] / 255.;
				table[i][1] = BlackBody_Green[i] / 255.;
				table[i][2] = BlackBody_Blue[i] / 255.;
			}
		}
		else if(clut == 2) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Cardiac_Red[i] / 255.;
				table[i][1] = Cardiac_Green[i] / 255.;
				table[i][2] = Cardiac_Blue[i] / 255.;
			}
		}
		else if(clut == 3) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Flow_Red[i] / 255.;
				table[i][1] = Flow_Green[i] / 255.;
				table[i][2] = Flow_Blue[i] / 255.;
			}
		}
		else if(clut == 4) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GEColor_Red[i] / 255.;
				table[i][1] = GEColor_Green[i] / 255.;
				table[i][2] = GEColor_Blue[i] / 255.;
			}
		}
		else if(clut == 5) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GrainRainbow_Red[i] / 255.;
				table[i][1] = GrainRainbow_Green[i] / 255.;
				table[i][2] = GrainRainbow_Blue[i] / 255.;
			}
		}
		else if(clut == 6) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = HotIron_Red[i] / 255.;
				table[i][1] = HotIron_Green[i] / 255.;
				table[i][2] = HotIron_Blue[i] / 255.;
			}
		}
		else if(clut == 7) {
			for(int i = 0; i < 256; i++)
			{
				tableRed[i][0] = NIH_Red[i] / 255.;
				tableRed[i][1] = NIH_Green[i] / 255.;
				tableRed[i][2] = NIH_Blue[i] / 255.;
			}
		}
		else if(clut == 8) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Spectrum_Red[i] / 255.;
				table[i][1] = Spectrum_Green[i] / 255.;
				table[i][2] = Spectrum_Blue[i] / 255.;
			}
		}
		else if(clut == 9) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRBones_Red[i] / 255.;
				table[i][1] = VRBones_Green[i] / 255.;
				table[i][2] = VRBones_Blue[i] / 255.;
			}
		}
		else if(clut == 10) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRMusclesBones_Red[i] / 255.;
				table[i][1] = VRMusclesBones_Green[i] / 255.;
				table[i][2] = VRMusclesBones_Blue[i] / 255.;
			}
		}
		else if(clut == 11) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRRedVessels_Red[i] / 255.;
				table[i][1] = VRRedVessels_Green[i] / 255.;
				table[i][2] = VRRedVessels_Blue[i] / 255.;
			}
		}

		
		vtkColorTransferFunction *red = vtkColorTransferFunction::New();
		red->BuildFunctionFromTable(wl-ww/2, wl+ww/2, 255, (double*)&tableRed);

		vtkColorTransferFunction *green = vtkColorTransferFunction::New();
		green->BuildFunctionFromTable(wl-ww/2, wl+ww/2, 255, (double*)&tableGreen);

		vtkColorTransferFunction *blue = vtkColorTransferFunction::New();
		blue->BuildFunctionFromTable(wl-ww/2, wl+ww/2, 255, (double*)&tableBlue);

		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
		colorTransferFunction->BuildFunctionFromTable(wl-ww/2, wl+ww/2, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->IndependentComponentsOn();
	
		if(clut == 0) {
			volumeProperty->SetColor(0, red);
			volumeProperty->SetColor(1, green);
			volumeProperty->SetColor(2, blue);
		}
		else {
			volumeProperty->SetColor(0, colorTransferFunction);
			volumeProperty->SetColor(1, colorTransferFunction);
			volumeProperty->SetColor(2, colorTransferFunction);
		}

		volumeProperty->SetScalarOpacity(0, opacityTransferFunction);
		volumeProperty->SetScalarOpacity(1, opacityTransferFunction);
		volumeProperty->SetScalarOpacity(2, opacityTransferFunction);
		
		volumeProperty->SetComponentWeight(3, 0);
		volumeProperty->SetInterpolationTypeToLinear();
		
		vtkVolume *volume = vtkVolume::New();
		
		vtkFixedPointVolumeRayCastMapper *volumeRaycastMapper = vtkFixedPointVolumeRayCastMapper::New();
		float LOD = 3.5;
		volumeRaycastMapper->SetMinimumImageSampleDistance(LOD);
		volumeRaycastMapper->SetMaximumImageSampleDistance(3*LOD);

		getItkVtkData()->getVtkImage()->SetScalarTypeToUnsignedChar();
		volumeRaycastMapper->SetInput(getItkVtkData()->getVtkImage());
		volumeRaycastMapper->SetBlendModeToComposite();
		volume->SetMapper(volumeRaycastMapper);
		volume->SetProperty(volumeProperty);
		
		volumeRaycastMapper->Delete();
		volumeProperty->Delete();
		opacityTransferFunction->Delete();
		colorTransferFunction->Delete();
		red->Delete();
		green->Delete();
		blue->Delete();

		return volume;
	}
	else return NULL;
}

// usa vtkFixedPointVolumeRayCastMapper
vtkVolume *volumeRenderingFilter::computeFixedPoint(long wl, long ww, int clut, int minPixel) {
	vtkVolume *volume = NULL;

	if (isValidData()) {
		getItkVtkData()->setVtkActor(0);

		int shiftValue = 0;
		if( minPixel < 0 ) 
			shiftValue =(-1)*(minPixel);

		vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
		shiftScale->SetInput(getItkVtkData()->getVtkImage());
		shiftScale->SetScale(1);
		shiftScale->SetShift(shiftValue);
		shiftScale->ClampOverflowOn();
		shiftScale->SetOutputScalarTypeToUnsignedShort();

		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();

		float start = shiftValue + wl - ww/2, end = shiftValue + wl + ww/2;
		opacityTransferFunction->AddPoint(start, 0);		
		opacityTransferFunction->AddPoint(end, 1);
		

		double table[256][3];
		if(clut == 0) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = i / 255.;
				table[i][1] = i / 255.;
				table[i][2] = i / 255.;
			}
		}
		else if(clut == 1) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = BlackBody_Red[i] / 255.;
				table[i][1] = BlackBody_Green[i] / 255.;
				table[i][2] = BlackBody_Blue[i] / 255.;
			}
		}
		else if(clut == 2) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Cardiac_Red[i] / 255.;
				table[i][1] = Cardiac_Green[i] / 255.;
				table[i][2] = Cardiac_Blue[i] / 255.;
			}
		}
		else if(clut == 3) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Flow_Red[i] / 255.;
				table[i][1] = Flow_Green[i] / 255.;
				table[i][2] = Flow_Blue[i] / 255.;
			}
		}
		else if(clut == 4) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GEColor_Red[i] / 255.;
				table[i][1] = GEColor_Green[i] / 255.;
				table[i][2] = GEColor_Blue[i] / 255.;
			}
		}
		else if(clut == 5) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GrainRainbow_Red[i] / 255.;
				table[i][1] = GrainRainbow_Green[i] / 255.;
				table[i][2] = GrainRainbow_Blue[i] / 255.;
			}
		}
		else if(clut == 6) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = HotIron_Red[i] / 255.;
				table[i][1] = HotIron_Green[i] / 255.;
				table[i][2] = HotIron_Blue[i] / 255.;
			}
		}
		else if(clut == 7) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = NIH_Red[i] / 255.;
				table[i][1] = NIH_Green[i] / 255.;
				table[i][2] = NIH_Blue[i] / 255.;
			}
		}
		else if(clut == 8) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Spectrum_Red[i] / 255.;
				table[i][1] = Spectrum_Green[i] / 255.;
				table[i][2] = Spectrum_Blue[i] / 255.;
			}
		}
		else if(clut == 9) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRBones_Red[i] / 255.;
				table[i][1] = VRBones_Green[i] / 255.;
				table[i][2] = VRBones_Blue[i] / 255.;
			}
		}
		else if(clut == 10) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRMusclesBones_Red[i] / 255.;
				table[i][1] = VRMusclesBones_Green[i] / 255.;
				table[i][2] = VRMusclesBones_Blue[i] / 255.;
			}
		}
		else if(clut == 11) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRRedVessels_Red[i] / 255.;
				table[i][1] = VRRedVessels_Green[i] / 255.;
				table[i][2] = VRRedVessels_Blue[i] / 255.;
			}
		}

		
		colorTransferFunction->BuildFunctionFromTable(shiftValue + wl-ww/2, shiftValue + wl+ww/2, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetInterpolationTypeToLinear();

		vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();
		mapper->SetInput(shiftScale->GetOutput());
		mapper->SetBlendModeToComposite();

		volume = vtkVolume::New();
		volume->SetProperty(volumeProperty);
		volume->SetMapper(mapper);
		
		mapper->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
	}

	return volume;
}

// usa vtkVolumeTextureMapper3D (textures 3D)
vtkVolume *volumeRenderingFilter::computeTextures3D(long wl, long ww, int clut, int minPixel, float sampleDistance) {
	vtkVolume *volume = NULL;

	if (isValidData()) {
		getItkVtkData()->setVtkActor(0);

		int shiftValue = 0;
		if( minPixel < 0 ) 
			shiftValue =(-1)*(minPixel);

		vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
		shiftScale->SetInput(getItkVtkData()->getVtkImage());
		shiftScale->SetScale(1);
		shiftScale->SetShift(shiftValue);
		shiftScale->ClampOverflowOn();
		shiftScale->SetOutputScalarTypeToUnsignedShort();

		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();

		float start = shiftValue + wl - ww/2, end = shiftValue + wl + ww/2;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		double table[256][3];
		if(clut == 0) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = i / 255.;
				table[i][1] = i / 255.;
				table[i][2] = i / 255.;
			}
		}
		else if(clut == 1) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = BlackBody_Red[i] / 255.;
				table[i][1] = BlackBody_Green[i] / 255.;
				table[i][2] = BlackBody_Blue[i] / 255.;
			}
		}
		else if(clut == 2) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Cardiac_Red[i] / 255.;
				table[i][1] = Cardiac_Green[i] / 255.;
				table[i][2] = Cardiac_Blue[i] / 255.;
			}
		}
		else if(clut == 3) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Flow_Red[i] / 255.;
				table[i][1] = Flow_Green[i] / 255.;
				table[i][2] = Flow_Blue[i] / 255.;
			}
		}
		else if(clut == 4) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GEColor_Red[i] / 255.;
				table[i][1] = GEColor_Green[i] / 255.;
				table[i][2] = GEColor_Blue[i] / 255.;
			}
		}
		else if(clut == 5) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GrainRainbow_Red[i] / 255.;
				table[i][1] = GrainRainbow_Green[i] / 255.;
				table[i][2] = GrainRainbow_Blue[i] / 255.;
			}
		}
		else if(clut == 6) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = HotIron_Red[i] / 255.;
				table[i][1] = HotIron_Green[i] / 255.;
				table[i][2] = HotIron_Blue[i] / 255.;
			}
		}
		else if(clut == 7) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = NIH_Red[i] / 255.;
				table[i][1] = NIH_Green[i] / 255.;
				table[i][2] = NIH_Blue[i] / 255.;
			}
		}
		else if(clut == 8) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Spectrum_Red[i] / 255.;
				table[i][1] = Spectrum_Green[i] / 255.;
				table[i][2] = Spectrum_Blue[i] / 255.;
			}
		}
		else if(clut == 9) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRBones_Red[i] / 255.;
				table[i][1] = VRBones_Green[i] / 255.;
				table[i][2] = VRBones_Blue[i] / 255.;
			}
		}
		else if(clut == 10) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRMusclesBones_Red[i] / 255.;
				table[i][1] = VRMusclesBones_Green[i] / 255.;
				table[i][2] = VRMusclesBones_Blue[i] / 255.;
			}
		}
		else if(clut == 11) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRRedVessels_Red[i] / 255.;
				table[i][1] = VRRedVessels_Green[i] / 255.;
				table[i][2] = VRRedVessels_Blue[i] / 255.;
			}
		}

		
		colorTransferFunction->BuildFunctionFromTable(shiftValue + wl-ww/2, shiftValue + wl+ww/2, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetInterpolationTypeToLinear();
		
		vtkVolumeTextureMapper3D *volume3DTextureMapper = vtkVolumeTextureMapper3D::New();
		volume3DTextureMapper->SetPreferredMethodToNVidia();
		volume3DTextureMapper->SetSampleDistance(sampleDistance);
		volume3DTextureMapper->SetInput(shiftScale->GetOutput());
		
		volume = vtkVolume::New();
		volume->SetProperty(volumeProperty);
		volume->SetMapper(volume3DTextureMapper);
		
		volume3DTextureMapper->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
		shiftScale->Delete();
	}
	
	return volume;
}

// usa vtkVolumeTextureMapper2D (textures 2D)
vtkVolume *volumeRenderingFilter::computeTextures2D(long wl, long ww, int clut, int minPixel) {
	vtkVolume *volume = NULL;
	
	if (isValidData()) {
		getItkVtkData()->setVtkActor(0);

		int shiftValue = 0;
		if( minPixel < 0 ) 
			shiftValue =(-1)*(minPixel);

		vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
		shiftScale->SetInput(getItkVtkData()->getVtkImage());
		shiftScale->SetScale(1);
		shiftScale->SetShift(shiftValue);
		shiftScale->ClampOverflowOn();
		shiftScale->SetOutputScalarTypeToUnsignedShort();

		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();

		float start = shiftValue + wl - ww/2, end = shiftValue + wl + ww/2;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		double table[256][3];
		if(clut == 0) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = i / 255.;
				table[i][1] = i / 255.;
				table[i][2] = i / 255.;
			}
		}
		else if(clut == 1) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = BlackBody_Red[i] / 255.;
				table[i][1] = BlackBody_Green[i] / 255.;
				table[i][2] = BlackBody_Blue[i] / 255.;
			}
		}
		else if(clut == 2) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Cardiac_Red[i] / 255.;
				table[i][1] = Cardiac_Green[i] / 255.;
				table[i][2] = Cardiac_Blue[i] / 255.;
			}
		}
		else if(clut == 3) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Flow_Red[i] / 255.;
				table[i][1] = Flow_Green[i] / 255.;
				table[i][2] = Flow_Blue[i] / 255.;
			}
		}
		else if(clut == 4) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GEColor_Red[i] / 255.;
				table[i][1] = GEColor_Green[i] / 255.;
				table[i][2] = GEColor_Blue[i] / 255.;
			}
		}
		else if(clut == 5) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GrainRainbow_Red[i] / 255.;
				table[i][1] = GrainRainbow_Green[i] / 255.;
				table[i][2] = GrainRainbow_Blue[i] / 255.;
			}
		}
		else if(clut == 6) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = HotIron_Red[i] / 255.;
				table[i][1] = HotIron_Green[i] / 255.;
				table[i][2] = HotIron_Blue[i] / 255.;
			}
		}
		else if(clut == 7) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = NIH_Red[i] / 255.;
				table[i][1] = NIH_Green[i] / 255.;
				table[i][2] = NIH_Blue[i] / 255.;
			}
		}
		else if(clut == 8) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Spectrum_Red[i] / 255.;
				table[i][1] = Spectrum_Green[i] / 255.;
				table[i][2] = Spectrum_Blue[i] / 255.;
			}
		}
		else if(clut == 9) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRBones_Red[i] / 255.;
				table[i][1] = VRBones_Green[i] / 255.;
				table[i][2] = VRBones_Blue[i] / 255.;
			}
		}
		else if(clut == 10) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRMusclesBones_Red[i] / 255.;
				table[i][1] = VRMusclesBones_Green[i] / 255.;
				table[i][2] = VRMusclesBones_Blue[i] / 255.;
			}
		}
		else if(clut == 11) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRRedVessels_Red[i] / 255.;
				table[i][1] = VRRedVessels_Green[i] / 255.;
				table[i][2] = VRRedVessels_Blue[i] / 255.;
			}
		}

		
		colorTransferFunction->BuildFunctionFromTable(shiftValue + wl-ww/2, shiftValue + wl+ww/2, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetInterpolationTypeToLinear();
		
	
		vtkVolumeTextureMapper2D *volume2DTextureMapper = vtkVolumeTextureMapper2D::New();
		volume2DTextureMapper->SetInput(shiftScale->GetOutput());
		
		volume = vtkVolume::New();
		volume->SetProperty(volumeProperty);
		volume->SetMapper(volume2DTextureMapper);

		volume2DTextureMapper->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
		shiftScale->Delete();
	}
	
	return volume;
}

// controlla se la scheda grafica supporta il 3D texture mapping
bool volumeRenderingFilter::is3DtextureSupported() {
	bool texture3DSupported = false;
	if (isValidData()) {
		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();

		float start = 0, end = 1;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		double table[256][3];
		for(int i = 0; i < 256; i++)
		{
			table[i][0] = i / 255.;
			table[i][1] = i / 255.;
			table[i][2] = i / 255.;
		}
		
		//opacityTransferFunction->BuildFunctionFromTable(wl-ww/2, wl+ww/2, 255, (double*)&alpha);
		colorTransferFunction->BuildFunctionFromTable(0, 1, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetInterpolationTypeToLinear();
		
		vtkVolumeTextureMapper3D *volume3DTextureMapper = vtkVolumeTextureMapper3D::New();
		volume3DTextureMapper->SetSampleDistance(0.25);
		volume3DTextureMapper->SetInput(getItkVtkData()->getVtkImage());
		
		// controlla se il 3d texture mapping del particolare set di dati è supportato dalla scheda video
		if (volume3DTextureMapper->IsRenderSupported(volumeProperty))
			texture3DSupported = true;
		
		volume3DTextureMapper->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
	}
	
	return texture3DSupported;
}

