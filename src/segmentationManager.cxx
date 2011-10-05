/**
 * \file segmentationManager.cxx
 * \brief Gestione segmentazione
 * \author ICAR-CNR Napoli
 */

#include "segmentationManager.h"
#include "segmentationFilter.h"
#include "wxVtkViewer2d.h"
#include "wxConnectedThresholdDialog.h"
#include "wxNeighborhoodConnectedDialog.h"
#include "wxConfidenceConnectedDialog.h"
#include "wx2dGui.h"
#include <vtkImageFlip.h>
#include <wx/busyinfo.h>

// Costruttore 
segmentationManager::segmentationManager(viewerHandler* viewerHandler)
{
	_viewerHandler = viewerHandler;
	_idData = -1;
	_idViewer = -1;
	_verFlip = false;
	_horFlip = false;
	_x = -1;
	_y = -1;
	_pixelValue = 0;
}


// Distruttore 
segmentationManager::~segmentationManager()
{
	_viewerHandler = 0;
}


void segmentationManager::initializeSegmentationManager()
{
	_idViewer = _viewerHandler->getActiveViewer();
	_idData = _viewerHandler->getViewer(_idViewer)->getActiveIdSingleData();

	int rows = atoi(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getHeaderEntry(13).c_str());
	int columns = atoi(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getHeaderEntry(14).c_str());
	_x = ((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getCurrentX();
	_y = (((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getCurrentY());
	if (_horFlip)
		_x = columns - 1 - _x; 
	if (_verFlip)
		_y = rows - 1 - _y; 

	infoFilter infoFilter(_idData, _viewerHandler->getDataHandler(), _viewerHandler->get2dViewer(_idViewer)->getCurrentSlice());
	_pixelValue = (short)infoFilter.getPixelValue(_x,_y);
	
	_seed = infoFilter.getXY(_x,_y);
	_x = ((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getCurrentX();
	_y = rows -1 - (((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getCurrentY());
}

bool segmentationManager::computeConnectedThresholdAlgorithm()
{
	
	wxWindow* window = ((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getWxWindow();
	while(!(window->IsTopLevel())){
		if(window->GetParent())
			window = window->GetParent(); 			
	}
	wxConnectedThresholdDialog* inputDialog = new wxConnectedThresholdDialog(window);
	inputDialog->InitializeDialog(_x,_y, _pixelValue);
	if(inputDialog->ShowModal()==wxID_OK){
		
		short lowerThreshold = inputDialog->GetLowerThreshold();
		short upperThreshold = inputDialog->GetUpperThreshold();
		inputDialog->Destroy();

		wxWindowDisabler disabler;
		wxBusyInfo wait("Please wait, segmenting...");
		wxBusyCursor cursor;
		
		segmentationFilter segFilter(_idData, _viewerHandler->getDataHandler());
		segFilter.computeRegionGrowing(_seed, lowerThreshold, upperThreshold, 0, 0, 0, 0, segmentationFilter::connectedThreshold);
		((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->resetVtkData();
		if(_verFlip)
			computeFlipping(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getVtkImage(),'y');
		if(_horFlip)
			computeFlipping(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getVtkImage(),'x');
		((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->updateViewer();
		((wx2dGui*)window)->onAll((wxCommandEvent)NULL);
		((wx2dGui*)window)->Raise();
	return true;
	}
	else {
		inputDialog->Destroy();
		((wx2dGui*)window)->onAll((wxCommandEvent)NULL);
		return false;
	}

}

bool segmentationManager::computeNeighborhoodConnectedAlgorithm()
{
	wxWindow* window = ((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getWxWindow();
	while(!(window->IsTopLevel())){
		if(window->GetParent())
			window = window->GetParent(); 			
	}
	wxNeighborhoodConnectedDialog* inputDialog = new wxNeighborhoodConnectedDialog(window);
	inputDialog->InitializeDialog(_x,_y, _pixelValue);
	if(inputDialog->ShowModal()==wxID_OK){

		short lowerThreshold = inputDialog->GetLowerThreshold();
		short upperThreshold = inputDialog->GetUpperThreshold();
		float radiusX = inputDialog->GetRadiusX();
		float radiusY = inputDialog->GetRadiusY();
		inputDialog->Destroy();

		wxWindowDisabler disabler;
		wxBusyInfo wait("Please wait, segmenting...");
		wxBusyCursor cursor;

		segmentationFilter segFilter(_idData, _viewerHandler->getDataHandler());
		segFilter.computeRegionGrowing(_seed, lowerThreshold, upperThreshold, radiusX, radiusY, 0, 0, segmentationFilter::neighborhoodConnected);
		((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->resetVtkData();
		if(_verFlip)
			computeFlipping(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getVtkImage(),'y');
		if(_horFlip)
			computeFlipping(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getVtkImage(),'x');
		((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->updateViewer();
		((wx2dGui*)window)->onAll((wxCommandEvent)NULL);
		((wx2dGui*)window)->Raise();
	return true;
	}
	else {
		inputDialog->Destroy();
		((wx2dGui*)window)->onAll((wxCommandEvent)NULL);
		return false;
	}
	
}
	
bool segmentationManager::computeConfidenceConnectedAlgorithm()
{
	wxWindow* window = ((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getWxWindow();
	while(!(window->IsTopLevel())){
		if(window->GetParent())
			window = window->GetParent(); 			
	}
	wxConfidenceConnectedDialog* inputDialog = new wxConfidenceConnectedDialog(window);
	inputDialog->InitializeDialog(_x,_y, _pixelValue);
	if(inputDialog->ShowModal()==wxID_OK){
		float radius = inputDialog->GetRadius();
		float multiplier = inputDialog->GetMultiplier();
		int iterations = inputDialog->GetIterations();
		inputDialog->Destroy();
		wxWindowDisabler disabler;
		wxBusyInfo wait("Please wait, segmenting...");
		wxBusyCursor cursor;

		segmentationFilter segFilter(_idData, _viewerHandler->getDataHandler());
		segFilter.computeRegionGrowing(_seed, 0, 0, radius, 0, multiplier, iterations, segmentationFilter::confidenceConnected);
		((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->resetVtkData();
		if(_verFlip)
			computeFlipping(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getVtkImage(),'y');
		if(_horFlip)
			computeFlipping(((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->getItkVtkData()->getVtkImage(),'x');
		((wxVtkViewer2d*)(_viewerHandler->getViewer(_idViewer)->getViewerDrawing()))->updateViewer();
		((wx2dGui*)window)->onAll((wxCommandEvent)NULL);
		((wx2dGui*)window)->Raise();
		return true;
	}
	else {
		inputDialog->Destroy();
		((wx2dGui*)window)->onAll((wxCommandEvent)NULL);
		return false;
	}
}

void segmentationManager::computeFlipping(vtkImageData* image,char axis) 
{ 
	if (axis == 'x' || axis == 'y' || axis == 'z')
	{
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
			vtkFlip->SetInput(image);
			vtkFlip->SetInformationInput(image);
			vtkFlip->SetFilteredAxes(intAxis);
			vtkFlip->SetInterpolationModeToCubic();
			vtkFlip->InterpolateOn();
			vtkFlip->Update();	
			image->DeepCopy(vtkFlip->GetOutput());					
			vtkFlip->Delete();
		}
	}
}
