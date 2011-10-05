/**
 * \file wxVtkViewer3d.cxx
 * \brief File per la gestione del visualizzatore 3D
 * \author ICAR-CNR Napoli
 */

#include "wxVtkViewer3d.h"
#include "vtkTextProperty.h"
#include "vtkMitoCustomCamera.h"


wxVtkViewer3d::wxVtkViewer3d(wxWindow *parent, wxWindowID id) : wxVtkViewer(parent,parent, id) {
	_ren = vtkRenderer::New();

	// add the custom camera
	vtkMitoCustomCamera* camera = vtkMitoCustomCamera::New();
    _ren->SetActiveCamera(camera);

	_rWin = vtkRenderWindow::New();

	// render window stereoscopica
	_rWin->StereoCapableWindowOn();
	_rWin->StereoRenderOn();

	_rWin->AddRenderer(_ren);
	_iren->SetRenderWindow(_rWin);
	_iren->Initialize();

	// eyeAngle inizialmente posto a 0
	_ren->GetActiveCamera()->SetEyeAngle(0);
	_rWin->SetStereoTypeToCrystalEyes();

	_ren->Render();
	_itkVtkData = NULL;
	_leftTopInfo = vtkTextActor::New();
	_algorithmType = undefined;
}

wxVtkViewer3d::wxVtkViewer3d(wxWindow *parent, wxWindow *gui, wxWindowID id) : wxVtkViewer(parent, gui, id) {
	_ren = vtkRenderer::New();

	// add the custom camera
	vtkMitoCustomCamera* camera = vtkMitoCustomCamera::New();
    _ren->SetActiveCamera(camera);

	_rWin = vtkRenderWindow::New();
	
	// render window stereoscopica
	_rWin->StereoCapableWindowOn();
	_rWin->StereoRenderOn();
	
	_rWin->AddRenderer(_ren);
	_iren->SetRenderWindow(_rWin);

	// eyeAngle inizialmente posto a 0
	_ren->GetActiveCamera()->SetEyeAngle(0);
	_rWin->SetStereoTypeToCrystalEyes();

	_ren->Render();
	_iren->Initialize();
	_itkVtkData = NULL;
	_leftTopInfo = vtkTextActor::New();
}

wxVtkViewer3d::~wxVtkViewer3d() {
	_ren->RemoveAllViewProps();
	
	if (_itkVtkData->getVtkActor()) {
		_ren->RemoveActor(_itkVtkData->getVtkActor());
		_itkVtkData->setVtkActor(NULL);
	}
	if (_itkVtkData->getVtkVolume()) {
		_ren->RemoveVolume(_itkVtkData->getVtkVolume());
		_itkVtkData->setVtkVolume(NULL);
	}
	_leftTopInfo->Delete();
	_rWin->Delete();
	_ren->Delete();
}

void wxVtkViewer3d::Delete(){
	delete this;
}

void wxVtkViewer3d::setTextActor() {
	_leftTopInfo->SetVisibility(1);
		
	vtkTextProperty * _leftTopInfoProps = _leftTopInfo->GetTextProperty();
	_leftTopInfoProps->SetColor(1.0, 1.0, 1.0);
	//_leftTopInfoProps->ShadowOn();
	_leftTopInfoProps->ItalicOn();
	_leftTopInfoProps->BoldOff();
	_leftTopInfoProps->SetFontSize(12);
	_leftTopInfoProps->SetVerticalJustificationToTop();
	_leftTopInfo->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	_leftTopInfo->GetPositionCoordinate()->SetValue(0.005,0.97);
	_ren->AddActor(_leftTopInfo);
}

void wxVtkViewer3d::setText() {
	_leftTopInfo->GetTextProperty()->SetColor(0,0,0);
	const char _leftTopInfoText[10]=".";
	long wl = _iren->getWl();
	long ww = _iren->getWw();
	//sprintf(_leftTopInfoText,"/n");
	_leftTopInfo->SetInput(_leftTopInfoText);

	/*
	char _leftTopInfoText[300];
	long wl = _iren->getWl();
	long ww = _iren->getWw();
	sprintf(_leftTopInfoText,"WL: %d WW: %d",wl,ww);
	_leftTopInfo->SetInput(_leftTopInfoText);
	*/
	
}


void wxVtkViewer3d::showLeftTopInfo()
{
	//_leftTopInfo->VisibilityOn();
}
	
void wxVtkViewer3d::hideLeftTopInfo()
{
	//_leftTopInfo->VisibilityOff();
}

void wxVtkViewer3d::showItkVtkData(itkVtkData *itkVtkData) {
	_itkVtkData = itkVtkData;
	setTextActor();
	if (_itkVtkData) {
		if (_itkVtkData->getVtkActor())
			_ren->AddActor(_itkVtkData->getVtkActor());
		if (_itkVtkData->getVtkVolume())
			_ren->AddVolume(_itkVtkData->getVtkVolume());
		wxKeyEvent event;
		event.m_keyCode = 114; 
		_iren->OnKeyDown(event);
	}
	else
		_ren->RemoveAllViewProps();
}

void wxVtkViewer3d::removeSingleAppData() {
	_itkVtkData = NULL;
	_ren->RemoveAllViewProps();
}

void wxVtkViewer3d::updateViewer() {
	_iren->Render();
	
	this->getWxWindow()->SetFocus();
	//return true;

	// aggiunta per prova
	//_iren->Enable();
}

algorithmType3d wxVtkViewer3d::getAlgorithm() {
	return _algorithmType;
}

void wxVtkViewer3d::setAlgorithm(algorithmType3d algorithmType) {
	_algorithmType = algorithmType;
}