/**
 * \file wxVtkViewer2d.cxx
 * \brief File per la gestione del visualizzatore 2D
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "wxVtkViewer2d.h"
#include <vtkTextActor.h> 
#include <vtkTextProperty.h>
#include <vtkImageMapToWindowLevelColors.h>



wxVtkViewer2d::wxVtkViewer2d(wxWindow *parent, wxWindow *gui, wxWindowID id) : wxVtkViewer(parent, gui, id),
	vtkImageViewer2() {	
	_iren->SetRenderWindow(RenderWindow);
	SetupInteractor(_iren);
	//SetColorLevel(450);
	//SetColorWindow(980);
	_iren->Initialize();
	_itkVtkData = NULL;
	_coordinates =  vtkCoordinate::New();
	_leftTopInfo = vtkTextActor::New();
	_rightTopInfo = vtkTextActor::New();
	_leftBottomInfo = vtkTextActor::New();
	_rightBottomInfo = vtkTextActor::New();
	_RoiLengthAreaVolume = NULL; 
	_RoiAngle = NULL; 
	_verFlip = false;
	_horFlip = false;
}

wxVtkViewer2d::~wxVtkViewer2d() {
	_coordinates->Delete();
	_leftTopInfo->Delete();
	_rightTopInfo->Delete();
	_leftBottomInfo->Delete();
	_rightBottomInfo->Delete();
	if(_RoiLengthAreaVolume)
		_RoiLengthAreaVolume->Delete();
	if(_RoiAngle)
		_RoiAngle->Delete();
	SetInput(NULL);
	_itkVtkData = 0;
}	

void wxVtkViewer2d::Delete() {
	vtkImageViewer2::Delete();
}

void wxVtkViewer2d::updateViewer() {
	SetInput(NULL);
	SetInput(_itkVtkData->getVtkImage());
	_iren->Render();
}

void wxVtkViewer2d::showItkVtkData(itkVtkData *itkVtkData) {
	_itkVtkData = itkVtkData;

	if (_itkVtkData) {
		SetInput(_itkVtkData->getVtkImage());
	}
	else
		SetInput(NULL);
	setTextActor();
	_iren->Render();

	Renderer->ResetCamera();
	nextSlice();
	prevSlice();
	initialZoom = Renderer->GetActiveCamera()->GetParallelScale(); 

	SetColorLevel(itkVtkData->getWl());
	SetColorWindow(itkVtkData->getWw());
}

void wxVtkViewer2d::removeSingleAppData() {
	_itkVtkData = NULL;
	SetInput(NULL);
}

void wxVtkViewer2d::nextSlice() {
	int zSlice = GetSlice()+1;
	if (zSlice > GetSliceMax())
		zSlice = GetSliceMin();
	SetSlice(zSlice);
	updateViewer();
}

void wxVtkViewer2d::prevSlice() {
	int zSlice = GetSlice()-1;
	if (zSlice < GetSliceMin())
		zSlice = GetSliceMax();
	SetSlice(zSlice);
	updateViewer();
}

void wxVtkViewer2d::setSlice(int zSlice) {
	SetSlice(zSlice);
	updateViewer();
}

double wxVtkViewer2d::getXPos(long mouseXPos, long mouseYPos) {
	_coordinates->SetCoordinateSystemToDisplay();
	_coordinates->SetValue(mouseXPos, mouseYPos);
	double *b = _coordinates->GetComputedWorldValue(Renderer);
	return b[0];
}

double wxVtkViewer2d::getYPos(long mouseXPos, long mouseYPos) {
	wxRect r;
	r = _iren->GetRect();
	_coordinates->SetCoordinateSystemToDisplay();
	_coordinates->SetValue(mouseXPos, r.height - mouseYPos);
	double *b = _coordinates->GetComputedWorldValue(Renderer);
	return b[1];
}

void wxVtkViewer2d::setRoiLengthAreaVolumeActor(wxMouseEvent& event)
{
	_RoiLengthAreaVolume = vtkTextActor::New();
	_RoiLengthAreaVolume->SetVisibility(1);
	vtkTextProperty * _RoiLengthAreaVolumeProps = _RoiLengthAreaVolume->GetTextProperty();
	_RoiLengthAreaVolumeProps->SetColor(1.0, 1.0, 1.0);
    //_RoiLengthAreaVolumeProps->ShadowOn();
    _RoiLengthAreaVolumeProps->ItalicOn();
    _RoiLengthAreaVolumeProps->BoldOff();
    _RoiLengthAreaVolumeProps->SetFontSize(12);
	_RoiLengthAreaVolume->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
	wxRect r;
	r = _iren->GetRect();	
	_RoiLengthAreaVolume->GetPositionCoordinate()->SetValue(event.GetX()+7,r.height-event.GetY()+7);
	Renderer->AddActor(_RoiLengthAreaVolume);
}

void wxVtkViewer2d::setRoiLengthAreaVolume(wxMouseEvent& event, float length,float area,float volume){

	if (_RoiLengthAreaVolume==NULL)
		setRoiLengthAreaVolumeActor(event);
	char _RoiLengthAreaVolumeText[100];
	if (area!=0)
		sprintf(_RoiLengthAreaVolumeText,"Length: %g cm\nArea: %g cm2\nVolume: %g cm3", length, area, volume );
	else 
		sprintf(_RoiLengthAreaVolumeText,"Length: %g cm", length);
	_RoiLengthAreaVolume->SetInput(_RoiLengthAreaVolumeText);
}

void wxVtkViewer2d::removeRoiLengthAreaVolume(){
	if(_RoiLengthAreaVolume){
		Renderer->RemoveActor(_RoiLengthAreaVolume);
		_RoiLengthAreaVolume->Delete();
		_RoiLengthAreaVolume=NULL;
	}
}
void wxVtkViewer2d::removeRoiAngle(){
	if(_RoiAngle){
		Renderer->RemoveActor(_RoiAngle);
		_RoiAngle->Delete();
		_RoiAngle=NULL;
	}
}

void wxVtkViewer2d::setRoiAngleActor(wxMouseEvent& event)
{
	_RoiAngle = vtkTextActor::New();
	_RoiAngle->SetVisibility(1);
	vtkTextProperty * _RoiAngleProps = _RoiAngle->GetTextProperty();
	_RoiAngleProps->SetColor(1.0, 1.0, 1.0);
    //_RoiAngleProps->ShadowOn();
    _RoiAngleProps->ItalicOn();
    _RoiAngleProps->BoldOff();
    _RoiAngleProps->SetFontSize(12);
	_RoiAngle->GetPositionCoordinate()->SetCoordinateSystemToDisplay();
	wxRect r;
	r = _iren->GetRect();	
	_RoiAngle->GetPositionCoordinate()->SetValue(event.GetX()+7,r.height-event.GetY()+7);
	Renderer->AddActor(_RoiAngle);
}

void wxVtkViewer2d::setRoiAngle(wxMouseEvent& event, float angle){

	if (_RoiAngle==NULL)
		setRoiAngleActor(event);
	char _RoiAngleText[100];
	sprintf(_RoiAngleText,"Angle: %g °", angle );
	_RoiAngle->SetInput(_RoiAngleText);
}

void wxVtkViewer2d::setTextActor(){

	_leftTopInfo->SetVisibility(1);
	_rightTopInfo->SetVisibility(1);
	_leftBottomInfo->SetVisibility(1);
	_rightBottomInfo->SetVisibility(1);
	
	vtkTextProperty * _leftTopInfoProps = _leftTopInfo->GetTextProperty();
	_leftTopInfoProps->SetColor(1.0, 1.0, 1.0);
    //_leftTopInfoProps->ShadowOn();
    _leftTopInfoProps->ItalicOn();
    _leftTopInfoProps->BoldOff();
    _leftTopInfoProps->SetFontSize(12);
	_leftTopInfoProps->SetVerticalJustificationToTop();
	_leftTopInfo->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	_leftTopInfo->GetPositionCoordinate()->SetValue(0.005,0.97);
	
	vtkTextProperty * _rightTopInfoProps = _rightTopInfo->GetTextProperty();
	_rightTopInfoProps->SetColor(1.0, 1.0, 1.0);
    //_rightTopInfoProps->ShadowOn();
    _rightTopInfoProps->ItalicOn();
    _rightTopInfoProps->BoldOff();
    _rightTopInfoProps->SetFontSize(12);
	_rightTopInfoProps->SetJustificationToRight();
	_rightTopInfoProps->SetVerticalJustificationToTop();
	_rightTopInfo->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	_rightTopInfo->GetPositionCoordinate()->SetValue(0.985,0.97);

	vtkTextProperty * _leftBottomInfoProps = _leftBottomInfo->GetTextProperty();
	_leftBottomInfoProps->SetColor(1.0, 1.0, 1.0);
    //_leftBottomInfoProps->ShadowOn();
    _leftBottomInfoProps->ItalicOn();
    _leftBottomInfoProps->BoldOff();
    _leftBottomInfoProps->SetFontSize(12);
	_leftBottomInfoProps->SetVerticalJustificationToBottom();
	_leftBottomInfo->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	_leftBottomInfo->GetPositionCoordinate()->SetValue(0.005,0.10);

	vtkTextProperty * _rightBottomInfoProps = _rightBottomInfo->GetTextProperty();
	_rightBottomInfoProps->SetColor(1.0, 1.0, 1.0);
    //_rightBottomInfoProps->ShadowOn();
    _rightBottomInfoProps->ItalicOn();
    _rightBottomInfoProps->BoldOff();
    _rightBottomInfoProps->SetFontSize(12);
	_rightBottomInfoProps->SetJustificationToRight();
	_rightBottomInfoProps->SetVerticalJustificationToBottom();
	_rightBottomInfo->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	_rightBottomInfo->GetPositionCoordinate()->SetValue(0.985,0.10);

	Renderer->AddActor(_rightBottomInfo);
	Renderer->AddActor(_leftBottomInfo);
	Renderer->AddActor(_leftTopInfo);
	Renderer->AddActor(_rightTopInfo);

}

void wxVtkViewer2d::setText(infoFilter infoFilter, int posX, int posY) {
	int  rows = atoi(_itkVtkData->getHeaderEntry(13).c_str());
	int columns = atoi(_itkVtkData->getHeaderEntry(14).c_str());
	bool outViewer = false;
	if (_horFlip)
		_x = columns - 1 - posX;
	else
		_x = posX;
	if (_verFlip)
		_y = rows - 1 -posY;
	else
		_y = posY;

	if(!_itkVtkData->getRgb()) this->_pixelValue = infoFilter.getPixelValue(_x,_y);
	else this->_pixelValueRgb = infoFilter.getPixelValueRgb(_x,_y);
	
	_x = posX;
	_y = rows - posY -1;
	
	if (_x<0 || _x>=columns || _y<0 || _y>=rows){
		_x=0;
		_y=0;
		outViewer = true;
	}

	char _leftTopInfoText[100];
	if(!_itkVtkData->getRgb()) 
		sprintf(_leftTopInfoText,"Image Size: %d x %d \nX: %d px Y: %d px Value: %hd \nWL: %g WW: %g",rows, columns,this->_x,this->_y,this->_pixelValue, this->GetColorLevel(),this->GetColorWindow());
	else {
		unsigned char r = (_x==0 || _y==0) ? 0 : _pixelValueRgb.GetRed();
		unsigned char g = (_x==0 || _y==0) ? 0 : _pixelValueRgb.GetGreen();
		unsigned char b = (_x==0 || _y==0) ? 0 : _pixelValueRgb.GetBlue();
		sprintf(_leftTopInfoText,"Image Size: %d x %d \nX: %d px Y: %d px Value: R:%d G:%d B:%d \nWL: %g WW: %g",rows, columns,this->_x,this->_y,r,g,b, this->GetColorLevel(), this->GetColorWindow());
	}
	_leftTopInfo->SetInput(_leftTopInfoText);

	string patientName = _itkVtkData->getHeaderEntry(0);
	string patientID = _itkVtkData->getHeaderEntry(1);
	string patientAge = _itkVtkData->getHeaderEntry(2);
	string studyDescription = _itkVtkData->getHeaderEntry(4);
	string studyID = _itkVtkData->getHeaderEntry(6);
	
	char _rightTopInfoText[200];
	sprintf(_rightTopInfoText,"%s\n%s\n%s\n%s\n%s",patientName.c_str(),patientID.c_str(),patientAge.c_str(),studyDescription.c_str(),studyID.c_str());
	_rightTopInfo->SetInput(_rightTopInfoText);
	
	int sliceNumber = atoi(_itkVtkData->getHeaderEntry(8).c_str());
	int currentSlice = this->getCurrentSlice()+1;
	int zoom = static_cast<int>((initialZoom/(Renderer->GetActiveCamera()->GetParallelScale()))*100);
	double angle = Renderer->GetActiveCamera()->GetRoll();
	double thickness = 0;
	if (strcmp("",_itkVtkData->getHeaderEntry(16).c_str())!=0)
		thickness = atof(_itkVtkData->getHeaderEntry(16).c_str());
	double location = 0;
	if (strcmp("",_itkVtkData->getHeaderEntry(15).c_str())!=0) {
		//if(!_itkVtkData->getDownUp())
			//location = atof(_itkVtkData->getHeaderEntry(15).c_str()) + ((sliceNumber - 1) * _itkVtkData->getOffset()) - (this->getCurrentSlice() * _itkVtkData->getOffset());
		//else
			location = atof(_itkVtkData->getHeaderEntry(15).c_str()) - ((sliceNumber - 1) * _itkVtkData->getOffset()) + (this->getCurrentSlice() * _itkVtkData->getOffset());
	}

	
	//Image initial position
	int xPos = 0;
	int yPos = 0;
	double xDisplacement = 0;
	double yDisplacement = 0;
	double zDisplacement = 0;
	if (strcmp("",_itkVtkData->getHeaderEntry(18).c_str())!=0){
		xPos = _itkVtkData->getHeaderEntry(18).find("\\",0);
		yPos = (_itkVtkData->getHeaderEntry(18)).find("\\",xPos+1);
		xDisplacement = atof(_itkVtkData->getHeaderEntry(18).substr(0,xPos-1).c_str());
		yDisplacement = atof(_itkVtkData->getHeaderEntry(18).substr(xPos+1,yPos-1).c_str());
		zDisplacement = atof((_itkVtkData->getHeaderEntry(18)).substr(yPos+1,_itkVtkData->getHeaderEntry(18).size()).c_str());
	}
	//Image pixel spacing
	int pos = 0;
	double xPixelSpacing = 0;
	double yPixelSpacing = 0;
	if (strcmp("",_itkVtkData->getHeaderEntry(17).c_str())!=0){
		pos = _itkVtkData->getHeaderEntry(17).find("\\",0);
		xPixelSpacing = atof((_itkVtkData->getHeaderEntry(17)).substr(pos+1,_itkVtkData->getHeaderEntry(17).size()).c_str());
		yPixelSpacing = atof(_itkVtkData->getHeaderEntry(17).substr(0,pos-1).c_str());
	}
	//Image orientation
	double X_x = 1;
	double X_y = 0;
	double X_z = 0;
	double Y_x = 0;
	double Y_y = 1;
	double Y_z = 0;
	if (strcmp("",_itkVtkData->getHeaderEntry(19).c_str())!=0){
		if (_itkVtkData->getHeaderEntry(19).size()==11){
			X_x = atof(_itkVtkData->getHeaderEntry(19).substr(0,1).c_str());
			X_y = atof(_itkVtkData->getHeaderEntry(19).substr(2,3).c_str());
			X_z = atof(_itkVtkData->getHeaderEntry(19).substr(4,5).c_str());
			Y_x = atof(_itkVtkData->getHeaderEntry(19).substr(6,7).c_str());
			Y_y = atof(_itkVtkData->getHeaderEntry(19).substr(8,9).c_str());
			Y_z = atof((_itkVtkData->getHeaderEntry(19)).substr(10,11).c_str());
		}
		else if (_itkVtkData->getHeaderEntry(19).size()==23){
			X_x = atof(_itkVtkData->getHeaderEntry(19).substr(0,3).c_str());
			X_y = atof(_itkVtkData->getHeaderEntry(19).substr(4,7).c_str());
			X_z = atof(_itkVtkData->getHeaderEntry(19).substr(8,11).c_str());
			Y_x = atof(_itkVtkData->getHeaderEntry(19).substr(12,15).c_str());
			Y_y = atof(_itkVtkData->getHeaderEntry(19).substr(16,19).c_str());
			Y_z = atof((_itkVtkData->getHeaderEntry(19)).substr(20,23).c_str());
		}
	}	
	//Image position in mm
	double position[3];
	if(outViewer){
		position[0] =0;
		position[1] =0;
	}
	else {
		position[0] = (X_x * xPixelSpacing * _x) + (Y_x * yPixelSpacing * (rows - _y)) + xDisplacement;
		position[1] =(X_y * xPixelSpacing * _x) + (Y_y * yPixelSpacing * (rows - _y)) + yDisplacement;
	}
	position[2] = location;

	char _leftBottomInfoText[200];
	sprintf(_leftBottomInfoText,"%d/%d\nZoom: %d %c Angle: %g\nThickness: %g mm Location: %g mm\n X: %g mm Y: %g mm Z: %g mm", currentSlice, sliceNumber, zoom,'%', angle, thickness, location, position[0], position[1], position[2]);
	_leftBottomInfo->SetInput(_leftBottomInfoText);

	string dateAcquired = _itkVtkData->getHeaderEntry(7);;

	char _rightBottomInfoText[100];
	if (strcmp("",dateAcquired.c_str())!=0)
		sprintf(_rightBottomInfoText,"%s/%s/%s\nMade with MITO",(dateAcquired.substr(6,2)).c_str(),(dateAcquired.substr(4,2)).c_str(),(dateAcquired.substr(0,4)).c_str());
	else
		sprintf(_rightBottomInfoText,"%s\nMade with MITO","");		
	_rightBottomInfo->SetInput(_rightBottomInfoText);
	_y = rows - _y -1; 
}