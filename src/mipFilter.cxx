/**
 * \file mipFilter.cxx
 * \brief File per la creazione e la gestione della tecnica MIP (Maximum Intensity Projection)
 * \author ICAR-CNR Napoli
 */

#include "mipFilter.h"
#include "colorTransferTable.h"
#include <vtkImageShiftScale.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>

mipFilter::mipFilter(unsigned int idData, dataHandler *dataHandler) : itkVtkFilter(idData,dataHandler) {
}

mipFilter::~mipFilter(){
}

vtkVolume* mipFilter::compute(long wl, long ww, int clut, int minPixel, bool gaussFilter) {
	//Funzione utilizzata per realizzare la correzione dell'immagine e per convertire il tipo
	vtkImageShiftScale *shift = vtkImageShiftScale::New();

	//Filtro di smoothing
	vtkImageGaussianSmooth *gauss = NULL;

	// Inizializzo la variabile
	int shiftValue = 0;
	
	//Controllo se esistono pixel a valore negativo
	if( minPixel < 0 ) 
		shiftValue =(-1)*(minPixel);
	
	if(isValidData()){
		//Shift dell'immagine per renderla compatibile all'operazione di RayCasting
		//definendola di tipo unsigned short
		shift->SetInput(getItkVtkData()->getVtkImage());
		shift->SetShift(shiftValue);
		shift->SetScale(1);
		shift->ClampOverflowOn();
		shift->SetOutputScalarTypeToUnsignedShort();

		//Verifico l'attivazione del filtro Gaussiano per migliorare immagini deteriorate
		if(gaussFilter){
			gauss = vtkImageGaussianSmooth::New();
			//Filtro di tipo gaussiano. 
			gauss->SetInput(shift->GetOutput());
			gauss->SetStandardDeviation(1.0);
			gauss->SetRadiusFactor(10.0);
		}

		//Metodo per settare il valore per l'opacità
		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();

		//opacityTransferFunction->AddPoint(minPixel, (wl-ww/2)/256);
		//opacityTransferFunction->AddPoint(maxPixel, (wl+ww/2)/256);

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
		
		/*//Metodo per settare il valore per il colore (grigio)
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
		colorTransferFunction->AddRGBPoint(minPixel, (wl-ww/2)/256, (wl-ww/2)/256, (wl-ww/2)/256);
		colorTransferFunction->AddRGBPoint(maxPixel, (wl+ww/2)/256, (wl+ww/2)/256, (wl+ww/2)/256);*/

		colorTransferFunction->BuildFunctionFromTable(shiftValue + wl-ww/2, shiftValue + wl+ww/2, 255, (double*)&table);
		
		//Creo un set di proprietà per il volume con varie opzioni 
		vtkVolumeProperty *mipprop = vtkVolumeProperty::New();
		mipprop->SetScalarOpacity(opacityTransferFunction);       
		mipprop->SetColor(colorTransferFunction);
		mipprop->ShadeOn();
		mipprop->SetInterpolationTypeToLinear(); 

		//Creo la funzione per realizzare il Mip utilizzando RayCasting attraverso il valore Scalare
		vtkVolumeRayCastMIPFunction *MIPFunction1 = vtkVolumeRayCastMIPFunction::New();
		MIPFunction1->SetMaximizeMethodToScalarValue();

		vtkVolumeRayCastMapper *rayCastMapper= vtkVolumeRayCastMapper::New();
		rayCastMapper->SetMinimumImageSampleDistance(1.5);
		rayCastMapper->SetSampleDistance(1.5);

		//Determino il tipo di immagine , filtrata o non , da dare come input al RayCast .
		if(gaussFilter) {
			rayCastMapper->SetInputConnection(gauss->GetOutputPort());
		} else {
			rayCastMapper->SetInputConnection(shift->GetOutputPort());
		}
		//Setto il tipo di RayCast da utilizzare
		rayCastMapper->SetVolumeRayCastFunction(MIPFunction1);

		vtkVolume *volume = vtkVolume::New();
		volume->SetMapper( rayCastMapper );
		volume->SetProperty(mipprop);

		shift->Delete();
		mipprop->Delete();
		if(gauss) gauss->Delete();
		MIPFunction1->Delete();

		rayCastMapper->Delete();		
		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();

		return volume;
	}
	else return NULL;
}


vtkVolume* mipFilter::computeRgb(long wl, long ww, int clut, bool gaussFilter) {
	//Filtro di smoothing
	vtkImageGaussianSmooth *gauss = NULL;
	
	if(isValidData()){
		getItkVtkData()->getVtkImage()->SetScalarTypeToUnsignedChar();

		//Verifico l'attivazione del filtro Gaussiano per migliorare immagini deteriorate
		if(gaussFilter){
			gauss = vtkImageGaussianSmooth::New();
			//Filtro di tipo gaussiano. 
			gauss->SetInput(getItkVtkData()->getVtkImage());
			gauss->SetStandardDeviation(1.0);
			gauss->SetRadiusFactor(10.0);
		}

		//Metodo per settare il valore per l'opacità
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
		volumeProperty->ShadeOn();

		//Creo la funzione per realizzare il Mip utilizzando RayCasting attraverso il valore Scalare
		/*vtkVolumeRayCastMIPFunction *MIPFunction = vtkVolumeRayCastMIPFunction::New();
		MIPFunction->SetMaximizeMethodToScalarValue();*/

		vtkFixedPointVolumeRayCastMapper *rayCastMapper= vtkFixedPointVolumeRayCastMapper::New();
		float LOD = 3.5;
		rayCastMapper->SetMinimumImageSampleDistance(LOD);
		rayCastMapper->SetMaximumImageSampleDistance(3*LOD);

		//Determino il tipo di immagine, filtrata o no, da dare come input al RayCast
		if(gaussFilter) {
			rayCastMapper->SetInputConnection(gauss->GetOutputPort());
		} else {
			rayCastMapper->SetInput(getItkVtkData()->getVtkImage());
		}
		//Setto il tipo di RayCast da utilizzare
		//rayCastMapper->SetVolumeRayCastFunction(MIPFunction);
		rayCastMapper->SetBlendModeToMaximumIntensity();

		vtkVolume *volume = vtkVolume::New();
		volume->SetMapper(rayCastMapper);
		volume->SetProperty(volumeProperty);

		volumeProperty->Delete();
		if(gauss) gauss->Delete();
		//MIPFunction->Delete();

		rayCastMapper->Delete();		
		red->Delete();
		green->Delete();
		blue->Delete();
		opacityTransferFunction->Delete();
		colorTransferFunction->Delete();

		return volume;
	}
	else return NULL;
}