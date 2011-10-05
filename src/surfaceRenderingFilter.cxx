/**
 * \file surfaceRenderingFilter.cxx
 * \brief File per la realizzazione del surface rendering
 * \author ICAR-CNR Napoli
 */

#include "surfaceRenderingFilter.h"
#include <vtkDecimatePro.h> 
#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkSmoothPolyDataFilter.h>

surfaceRenderingFilter::surfaceRenderingFilter(unsigned int idData, dataHandler *dataHandler) : itkVtkFilter(idData, dataHandler) {
}

surfaceRenderingFilter::~surfaceRenderingFilter() {
}

vtkActor *surfaceRenderingFilter::compute(float contourValue, double decimateValue, double smoothValue, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower, double* colourAmbient, double* colourDiffuse, double* colourSpecular ) {
	if (isValidData()) {
		vtkContourFilter *skinExtractor = vtkContourFilter::New();
		skinExtractor->SetInput(getItkVtkData()->getVtkImage());
		skinExtractor->SetValue(0, contourValue);
		
		vtkDecimatePro* isoDeci = vtkDecimatePro::New();
		isoDeci->SetInput( skinExtractor->GetOutput());
		isoDeci->SetTargetReduction(decimateValue);
		isoDeci->SetPreserveTopology(true);

		vtkSmoothPolyDataFilter *smoother = vtkSmoothPolyDataFilter::New();
		smoother->SetInput(isoDeci->GetOutput());
		smoother->SetNumberOfIterations(smoothValue);
		
		vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();
		skinNormals->SetInput(smoother->GetOutput());
		skinNormals->SetFeatureAngle(120.0);

		vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();
		skinMapper->SetInput(skinNormals->GetOutput());
		skinMapper->ScalarVisibilityOff();
		
		vtkActor *actor = vtkActor::New();
		actor->SetMapper(skinMapper);
		actor->GetProperty()->ShadingOn();
		actor->GetProperty()->SetAmbientColor(colourAmbient[0], colourAmbient[1], colourAmbient[2]);
		actor->GetProperty()->SetDiffuseColor(colourDiffuse[0], colourDiffuse[1], colourDiffuse[2]);
		actor->GetProperty()->SetSpecularColor(colourSpecular[0], colourSpecular[1], colourSpecular[2]);
		actor->GetProperty()->SetAmbient(shadingAmbient);
		actor->GetProperty()->SetDiffuse(shadingDiffuse);
		actor->GetProperty()->SetSpecular(shadingSpecular);
		actor->GetProperty()->SetSpecularPower(shadingSpecularPower);
		
		isoDeci->Delete();
		skinMapper->Delete();
		skinNormals->Delete();
		smoother->Delete();
		skinExtractor->Delete();

		return actor;
	}
	else return NULL;
}