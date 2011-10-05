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
#include <vtkRenderer.h>
#include <vtkGPUVolumeRayCastMapper.h>

#include <vtkGPUInfo.h>
#include <vtkGPUInfoList.h>

volumeRenderingFilter::volumeRenderingFilter(unsigned int idData, dataHandler *dataHandler) : itkVtkFilter(idData, dataHandler) {
}

volumeRenderingFilter::~volumeRenderingFilter() {
}

// usa vtkVolumeRayCastMapper
vtkVolume *volumeRenderingFilter::compute(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower) {
	if (isValidData()) {
		//getItkVtkData()->setVtkActor(0);
		getItkVtkData()->setVtkVolume(0);

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

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetInterpolationTypeToLinear();

		volumeProperty->IndependentComponentsOff();

		float start = shiftValue + wl - ww/2, end = shiftValue + wl + ww/2;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		colorTransferFunction->BuildFunctionFromTable(start, end, 255, (double*)&table);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetScalarOpacity(opacityTransferFunction);

		if (shading) {
			volumeProperty->ShadeOn();
			volumeProperty->SetAmbient(shadingAmbient);
			volumeProperty->SetDiffuse(shadingDiffuse);
			volumeProperty->SetSpecular(shadingSpecular);
			volumeProperty->SetSpecularPower(shadingSpecularPower);
		}
		else {
			volumeProperty->ShadeOff();
		}

		vtkVolume *volume = vtkVolume::New();
		volume->SetProperty(volumeProperty);
		
		vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
		vtkVolumeRayCastMapper *volumeRaycastMapper = vtkVolumeRayCastMapper::New();

		volumeRaycastMapper->AutoAdjustSampleDistancesOn();
		volumeRaycastMapper->SetSampleDistance(sampleDistance);
		volumeRaycastMapper->SetImageSampleDistance(1.0);
		volumeRaycastMapper->SetMinimumImageSampleDistance(1.0);
		volumeRaycastMapper->SetMaximumImageSampleDistance(16.0);
		volumeRaycastMapper->SetVolumeRayCastFunction(compositeFunction);
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

// usa vtkFixedPointVolumeRayCastMapper (più performante e buona per RGB)
vtkVolume *volumeRenderingFilter::computeRgb(long wl, long ww, int clut, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower) {
	if (isValidData()) {
		//getItkVtkData()->setVtkActor(0);
		getItkVtkData()->setVtkVolume(0);

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

		if (shading) {
			volumeProperty->ShadeOn();
			volumeProperty->SetAmbient(shadingAmbient);
			volumeProperty->SetDiffuse(shadingDiffuse);
			volumeProperty->SetSpecular(shadingSpecular);
			volumeProperty->SetSpecularPower(shadingSpecularPower);
		}
		else {
			volumeProperty->ShadeOff();
		}

		vtkVolume *volume = vtkVolume::New();
		
		vtkFixedPointVolumeRayCastMapper *volumeRaycastMapper = vtkFixedPointVolumeRayCastMapper::New();
		//volumeRaycastMapper->AutoAdjustSampleDistancesOff();
		volumeRaycastMapper->AutoAdjustSampleDistancesOn();
		volumeRaycastMapper->SetSampleDistance(sampleDistance);
		volumeRaycastMapper->SetImageSampleDistance(1.0);
		volumeRaycastMapper->SetMinimumImageSampleDistance(1.0);
		volumeRaycastMapper->SetMaximumImageSampleDistance(16.0);

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
vtkVolume *volumeRenderingFilter::computeFixedPoint(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower) {
	vtkVolume *volume = NULL;

	if (isValidData()) {
		//getItkVtkData()->setVtkActor(0);
		getItkVtkData()->setVtkVolume(0);

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

		if (shading) {
			volumeProperty->ShadeOn();
			volumeProperty->SetAmbient(shadingAmbient);
			volumeProperty->SetDiffuse(shadingDiffuse);
			volumeProperty->SetSpecular(shadingSpecular);
			volumeProperty->SetSpecularPower(shadingSpecularPower);
		}
		else {
			volumeProperty->ShadeOff();
		}

		vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();

		//mapper->AutoAdjustSampleDistancesOff();
		mapper->AutoAdjustSampleDistancesOn();
		mapper->SetSampleDistance(sampleDistance);
		mapper->SetImageSampleDistance(1.0);
		mapper->SetMinimumImageSampleDistance(1.0);
		mapper->SetMaximumImageSampleDistance(16.0);

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
vtkVolume *volumeRenderingFilter::computeTextures3D(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower) {
	vtkVolume *volume = NULL;

	if (isValidData()) {
		//getItkVtkData()->setVtkActor(0);
		getItkVtkData()->setVtkVolume(0);

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

		if (shading) {
			volumeProperty->ShadeOn();
			volumeProperty->SetAmbient(shadingAmbient);
			volumeProperty->SetDiffuse(shadingDiffuse);
			volumeProperty->SetSpecular(shadingSpecular);
			volumeProperty->SetSpecularPower(shadingSpecularPower);
		}
		else {
			volumeProperty->ShadeOff();
		}

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
vtkVolume *volumeRenderingFilter::computeTextures2D(long wl, long ww, int clut, int minPixel, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower) {
	vtkVolume *volume = NULL;
	
	if (isValidData()) {
		//getItkVtkData()->setVtkActor(0);
		getItkVtkData()->setVtkVolume(0);

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
		
		if (shading) {
			volumeProperty->ShadeOn();
			volumeProperty->SetAmbient(shadingAmbient);
			volumeProperty->SetDiffuse(shadingDiffuse);
			volumeProperty->SetSpecular(shadingSpecular);
			volumeProperty->SetSpecularPower(shadingSpecularPower);
		}
		else {
			volumeProperty->ShadeOff();
		}
	
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


// usa vtkGPUVolumeRayCastMapper
vtkVolume *volumeRenderingFilter::computeGPU(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower, int availableVRAM) {
	vtkVolume *volume = NULL;

	if (isValidData()) {
		//getItkVtkData()->setVtkActor(0);
		getItkVtkData()->setVtkVolume(0);

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

		if (shading) {
			volumeProperty->ShadeOn();
			volumeProperty->SetAmbient(shadingAmbient);
			volumeProperty->SetDiffuse(shadingDiffuse);
			volumeProperty->SetSpecular(shadingSpecular);
			volumeProperty->SetSpecularPower(shadingSpecularPower);
		}
		else {
			volumeProperty->ShadeOff();
		}

		vtkGPUVolumeRayCastMapper *volumeGPUMapper = vtkGPUVolumeRayCastMapper::New();
		/*
		vtkIdType maxMemoryInBytes;
		maxMemoryInBytes = volumeGPUMapper->GetMaxMemoryInBytes();
		maxMemoryInBytes = dedicatedVideoMemory;
		volumeGPUMapper->SetMaxMemoryInBytes(maxMemoryInBytes);
		*/

		//volumeGPUMapper->setOpacityPiecewiseFunction(opacityTransferFunction);
		//volumeGPUMapper->setColorTransferFunction(colorTransferFunction);
		//volumeGPUMapper->SetPreferredMethodToNVidia();
		//volumeGPUMapper->AutoAdjustSampleDistancesOff();

		//volumeGPUMapper->AutoAdjustSampleDistancesOff();
		volumeGPUMapper->AutoAdjustSampleDistancesOff();
		volumeGPUMapper->SetSampleDistance(sampleDistance);
		volumeGPUMapper->SetImageSampleDistance(1.0);
		volumeGPUMapper->SetMinimumImageSampleDistance(1.0);
		volumeGPUMapper->SetMaximumImageSampleDistance(16.0);

		vtkIdType VRAM = availableVRAM*1024*1024;
		volumeGPUMapper->SetMaxMemoryInBytes( VRAM );
		volumeGPUMapper->SetMaxMemoryFraction(0.75);

		volumeGPUMapper->SetInput(shiftScale->GetOutput());
		
		volume = vtkVolume::New();
		volume->SetProperty(volumeProperty);
		volume->SetMapper(volumeGPUMapper);
		
		volumeGPUMapper->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
		shiftScale->Delete();
	}
	
	return volume;
}

// controlla se la scheda grafica supporta il 3D texture mapping
bool volumeRenderingFilter::is3DtextureSupported(vtkRenderer *renderer) {
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
		colorTransferFunction->BuildFunctionFromTable(0, 1, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetInterpolationTypeToLinear();
		
		vtkVolumeTextureMapper3D *volume3DTextureMapper = vtkVolumeTextureMapper3D::New();
		volume3DTextureMapper->SetInput(getItkVtkData()->getVtkImage());
		
		// controlla se il 3d texture mapping del particolare set di dati è supportato dalla scheda video
		if (volume3DTextureMapper->IsRenderSupported(volumeProperty, renderer))
			texture3DSupported = true;
		
		volume3DTextureMapper->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
	}
	
	return texture3DSupported;
}

// controlla se la scheda grafica supporta il raycasting su GPU
bool volumeRenderingFilter::isGPUraycastingSupported(vtkRenderWindow *renderWindow) {
	bool GPUraycastingSupported = false;
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
		colorTransferFunction->BuildFunctionFromTable(0, 1, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetInterpolationTypeToLinear();
		
		vtkGPUVolumeRayCastMapper *volumeGPUMapper = vtkGPUVolumeRayCastMapper::New();
		volumeGPUMapper->SetInput(getItkVtkData()->getVtkImage());
		
		// controlla se il 3d texture mapping del particolare set di dati è supportato dalla scheda video
		if ( volumeGPUMapper->IsRenderSupported(renderWindow, volumeProperty) )
			GPUraycastingSupported = true;
		
		volumeGPUMapper->Delete();
		volumeProperty->Delete();
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
	}
	
	return GPUraycastingSupported;
}

int volumeRenderingFilter::computeAvailableGPUMemory() 
{
	// calcolo dedicatedVideoMemory in MegaBytes
	int dedicatedVideoMemory = 0;
	vtkGPUInfoList *l=vtkGPUInfoList::New();
	l->Probe();
	int c=l->GetNumberOfGPUs();
	int i=0;
	while(i<c)
	{
		vtkGPUInfo *info=l->GetGPUInfo(i);
		vtkIdType value;
		value=info->GetDedicatedVideoMemory();
		dedicatedVideoMemory = (int)(dedicatedVideoMemory + value/1024/1024); // converto da bytes a MegaBytes
		++i;
	}
	l->Delete();

	return dedicatedVideoMemory;
}