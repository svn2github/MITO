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

vtkActor *surfaceRenderingFilter::compute(float contourValue) {
	if (isValidData()) {
		vtkContourFilter *skinExtractor = vtkContourFilter::New();
		skinExtractor->SetInput(getItkVtkData()->getVtkImage());
		skinExtractor->SetValue(0, contourValue);
		
		vtkDecimatePro* isoDeci = vtkDecimatePro::New();
		isoDeci->SetInput( skinExtractor->GetOutput());
		isoDeci->SetTargetReduction(0.5);
		isoDeci->SetPreserveTopology( TRUE);

		vtkSmoothPolyDataFilter *smoother = vtkSmoothPolyDataFilter::New();
		smoother->SetInput(isoDeci->GetOutput());
		smoother->SetNumberOfIterations(20);
		
		vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();
		skinNormals->SetInput(smoother->GetOutput());
		skinNormals->SetFeatureAngle(120.0);

		vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();
		skinMapper->SetInput(skinNormals->GetOutput());
		skinMapper->ScalarVisibilityOff();
		
		vtkActor *actor = vtkActor::New();
		actor->SetMapper(skinMapper);
		actor->GetProperty()->SetDiffuseColor(1, 1, 1);
		actor->GetProperty()->SetSpecular(.3);
		actor->GetProperty()->SetSpecularPower(20);
		
		isoDeci->Delete();
		skinMapper->Delete();
		skinNormals->Delete();
		smoother->Delete();
		skinExtractor->Delete();

		return actor;
	}
	else return NULL;
}