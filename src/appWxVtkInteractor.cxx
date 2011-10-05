/**
 * \file appWxVtkInteractor.cxx
 * \brief File per l'interazione dell'utente con la GUI
 * \author ICAR-CNR Napoli
 */

#include "appWxVtkInteractor.h"
#include "colorTransferTable.h"
#include "wx2dGui.h"
#include "wxVolumeRenderingGui.h"
#include "wxSurfaceRenderingGui.h"
#include "wxVtkViewer2d.h"
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <wx/busyinfo.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkRendererCollection.h>
#include "WiiCommandEvents.h"

BEGIN_EVENT_TABLE(appWxVtkInteractor, wxVTKRenderWindowInteractor)
    //EVT_TIMER(-1, appWxVtkInteractor::OnFocusTimer)
	//EVT_LEAVE_WINDOW(appWxVtkInteractor::OnLeave)
END_EVENT_TABLE()

#include "vtkCommand.h"
#include <vtkMath.h>
#include <vtkCellArray.h>

// settare a true / false per abilitare / disabilitare la scrittura su log
#define LOG_ACTIVATION false

appWxVtkInteractor::appWxVtkInteractor(wxWindow* parent, wxWindowID id, viewerHandler* vHandler)  : wxVTKRenderWindowInteractor (parent, id) {
	_parent = parent;
	_viewerHandler = vHandler;
	_interactionType = 0;
	_3dTechnique = Undecided;
	_actualPoint.x = 0;
	_actualPoint.y = 0;
	_roiManager = NULL; 
	_segmentationManager = NULL;
	_wlWwModified = true;
	vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
	this->SetInteractorStyle(style);
	style->Delete();

	// Modifica A.Placitelli
	//m_focusTimer = new wxTimer(this, -1);
	//m_focusTimer->Connect(wxEVT_TIMER, wxTimerEventHandler( appWxVtkInteractor::OnFocusTimer ), NULL, this);

	// frame rate desiderato
	renderingRate = 25;

	// frame rate impostato a renderingRate
	this->SetDesiredUpdateRate(renderingRate);
	this->SetStillUpdateRate(0.0001);

	// frame rate tale da lasciare il volume sempre al massimo della definizione
	//this->SetDesiredUpdateRate(this->GetStillUpdateRate());

	_minDISP				= 1279 * 0.005;
	_maxDISP				= 1279 * 0.75;
	_isSceneRotatingIR_1	= false;
	_3DCursorCanMove		= true;
	_3DCursorEnabled		= false;
	_clockwise				= false;
	//lastRenderTime		= 0;
	_lastRenderTime			= 0;
	idWiiEnabled			= AnyWiiEnabled;
	WiiManager				= NULL;	
	WiiManagerStarted		= false;		
	InteractionSignalsAdded = false;
	enabled_angle_Rotation = enabled_ryf_Rotation = enabled_rxf_Rotation = false;

	_lastMousePosition[0]=0;
	_lastMousePosition[1]=0;

	// calcolo la frequenza
	QueryPerformanceFrequency((LARGE_INTEGER *)&_clockFrequency);
}


appWxVtkInteractor::~appWxVtkInteractor() {
	_viewerHandler = 0;
	delete _roiManager;
	delete _segmentationManager;

	//delete m_focusTimer;	// Mod. A. Placitelli

	closeWiiManager();	
}


void appWxVtkInteractor::OnMouseWheel(wxMouseEvent &event) {	
	unsigned int idViewer = _viewerHandler->getActiveViewer();
	if(idViewer != 0) {
		wxVtkViewer2d* viewer2d = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()));
		wxWindow* window = wxGetTopLevelParent(viewer2d->getWxWindow());
		if(strcmp(typeid(*window).name(),"class wxMainGui")==0) 
			((wxMainGui*)window)->getSlider()->SetFocus();		
		else if(strcmp(typeid(*window).name(),"class wx2dGui")==0) 
			((wx2dGui*)window)->getSlider()->SetFocus();		
	}
}

void appWxVtkInteractor::OnButtonDown(wxMouseEvent &event) {
	_actualPoint = event.GetPosition(); 

	if(_interactionType == wii || _interactionType == wiiSR) {
		wxVTKRenderWindowInteractor::OnButtonDown(event);
		_parent->GetEventHandler()->ProcessEvent(event);
	}
	else if(_interactionType == all2d) {
		if(event.RightDown())
			_interactionType = zoom2d;
		wxVTKRenderWindowInteractor::OnButtonDown(event);
		_parent->GetEventHandler()->ProcessEvent(event);
	}
	else if(_interactionType == rotateAround3d || _interactionType == rotateAround3dSR) 
	{
		if (event.LeftDown()) {
			if (_3dTechnique==VolumeRendering) 
				((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->show3DAxes();
			else if (_3dTechnique==SurfaceRendering) 
				((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->show3DAxes();
		}

		if (_3DCursorEnabled) {
			if (event.LeftDown()||event.MiddleDown())
			{			
				if (_3dTechnique==SurfaceRendering)
				{
					wxSurfaceRenderingGui* wxSRGui = (wxSurfaceRenderingGui*)wxGetTopLevelParent(this);
				
					if ( wxSRGui->is3DCursorOnModelSurface() ) _3DCursorCanMove = false;
					else return;
				
					wxSRGui->set3DCursorRGBColor(0,0,1);
					wxSRGui->get_viewer3d()->updateViewer();

					_clockwise = wxSRGui->is3DCursorBeyondFocalPlane();
				}
				else if (_3dTechnique==VolumeRendering)
				{
					wxVolumeRenderingGui* wxVRGui = (wxVolumeRenderingGui*)wxGetTopLevelParent(this);
				
					if ( wxVRGui->is3DCursorOnModelSurface() ) _3DCursorCanMove = false;
					else return;
				
					wxVRGui->set3DCursorRGBColor(0,0,1);
					wxVRGui->get_viewer3d()->updateViewer();

					_clockwise = wxVRGui->is3DCursorBeyondFocalPlane();
				}
			}
		}

		// by Nello if(event.RightDown()) _interactionType = zoom3dVR;
		// by Nello _parent->GetEventHandler()->ProcessEvent(event);
		wxVTKRenderWindowInteractor::OnButtonDown(event);		
	}
	else if(_interactionType == zoom3dSR || _interactionType == zoom3dVR) {
		this->GetRenderWindow()->SetDesiredUpdateRate(this->GetDesiredUpdateRate());
		// resetta il valore per il ricalcolo del fattore di zoom
		_lastMousePosition[1]=0;
		//SetShiftKey(666);
		//InvokeEvent(vtkCommand::LeftButtonPressEvent, NULL);
		wxVTKRenderWindowInteractor::OnButtonDown(event);
		_parent->GetEventHandler()->ProcessEvent(event);
	}
	else if(_interactionType == dolly3dSR || _interactionType == dolly3dVR) {
		/* new version
		this->GetRenderWindow()->SetDesiredUpdateRate(this->GetDesiredUpdateRate());
		// resetta il valore per il ricalcolo del fattore di zoom
		_lastMousePosition[1]=0;
		wxVTKRenderWindowInteractor::OnButtonDown(event);
		*/
		if(event.LeftDown()) {
			event.m_controlDown = false;
			event.m_shiftDown = false;
			event.SetEventType(wxEVT_RIGHT_DOWN);			
			wxVTKRenderWindowInteractor::OnButtonDown(event);
			//_parent->GetEventHandler()->ProcessEvent(event);			
		}
	}
	else if(_interactionType == windowLevel2d) {
		if(event.LeftDown()) {
			event.m_controlDown = false;
			event.m_shiftDown = false;
			wxVTKRenderWindowInteractor::OnButtonDown(event);
			_parent->GetEventHandler()->ProcessEvent(event);
		}
	}
	else if(_interactionType == move2d || _interactionType == move3d) {	
			if (event.LeftDown())
			{			
				if (_3DCursorEnabled) {
					if (_3dTechnique==SurfaceRendering)
					{
						wxSurfaceRenderingGui* wxSRGui = (wxSurfaceRenderingGui*)wxGetTopLevelParent(this);
					
						if ( wxSRGui->is3DCursorOnModelSurface() ) _3DCursorCanMove = false;
						else return;
					
						wxSRGui->set3DCursorRGBColor(0,0,1);
						wxSRGui->get_viewer3d()->updateViewer();
					}
					else if (_3dTechnique==VolumeRendering)
					{
						wxVolumeRenderingGui* wxVRGui = (wxVolumeRenderingGui*)wxGetTopLevelParent(this);
					
						if ( wxVRGui->is3DCursorOnModelSurface() ) _3DCursorCanMove = false;
						else return;
					
						wxVRGui->set3DCursorRGBColor(0,0,1);
						wxVRGui->get_viewer3d()->updateViewer();
					}					
				}
				
				event.m_controlDown = false;			
				event.m_shiftDown = false;
				event.SetEventType(wxEVT_MIDDLE_DOWN);
				wxVTKRenderWindowInteractor::OnButtonDown(event);
				_parent->GetEventHandler()->ProcessEvent(event);
			}
	}
	else if(_interactionType == zoom2d) {
		if(event.LeftDown()) {
			event.m_controlDown = false;
			event.m_shiftDown = false;
			event.SetEventType(wxEVT_RIGHT_DOWN);			
			wxVTKRenderWindowInteractor::OnButtonDown(event);
			_parent->GetEventHandler()->ProcessEvent(event);			
		}
	}
	else if(_interactionType == rotate2d || _interactionType == rotate3d) {		
		if (event.LeftDown())
		{	if (_3DCursorEnabled) {
				if (_3dTechnique==SurfaceRendering)
				{
					wxSurfaceRenderingGui* wxSRGui = (wxSurfaceRenderingGui*)wxGetTopLevelParent(this);
				
					if ( wxSRGui->is3DCursorOnModelSurface() ) _3DCursorCanMove = false;
					else return;
				
					wxSRGui->set3DCursorRGBColor(0,0,1);
					wxSRGui->get_viewer3d()->updateViewer();
				}
				else if (_3dTechnique==VolumeRendering)
				{
					wxVolumeRenderingGui* wxVRGui = (wxVolumeRenderingGui*)wxGetTopLevelParent(this);
				
					if ( wxVRGui->is3DCursorOnModelSurface() ) _3DCursorCanMove = false;
					else return;
				
					wxVRGui->set3DCursorRGBColor(0,0,1);
					wxVRGui->get_viewer3d()->updateViewer();
				}
			}

			event.m_controlDown = true;
			event.m_shiftDown = false;
			wxVTKRenderWindowInteractor::OnButtonDown(event);
			_parent->GetEventHandler()->ProcessEvent(event);			
		}
	}
	else if(_interactionType == length2d || _interactionType == angle2d ||_interactionType == rectangle2d || _interactionType == polygon2d || _interactionType == pencil2d) {
		unsigned int idViewer = _viewerHandler->getActiveViewer();
		wxVtkViewer2d* viewer2d = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()));
		int x = viewer2d->getCurrentX();
		int y = viewer2d->getCurrentY();

		if(_roiManager==NULL)
			_roiManager = new roiManager(viewer2d->getItkVtkData());

		_roiManager->setSliceRoi(viewer2d->GetSlice());
		_roiManager->OnButtonDown(_interactionType, event, x, y);

		viewer2d->getItkVtkData()->setVtkImage(_roiManager->getVtkRoi());	

		if(event.LeftDown()) {
			switch(_interactionType){
				case length2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),0,0);
				break;
				case angle2d:
					viewer2d->setRoiAngle(event,_roiManager->angle());
				break;
				case rectangle2d:
					viewer2d->setRoiLengthAreaVolume(event, _roiManager->length(), _roiManager->area(), _roiManager->volume());
				break;
				case polygon2d:
					viewer2d->setRoiLengthAreaVolume(event, _roiManager->length(), _roiManager->area(), _roiManager->volume());
				break;
				case pencil2d:
					viewer2d->setRoiLengthAreaVolume(event, _roiManager->length(), _roiManager->area(), _roiManager->volume());
				break;
			}
		}
		if(event.RightDown()&&_roiManager->length()!=0){
			switch(_interactionType){
				case polygon2d:
					viewer2d->setRoiLengthAreaVolume(event, _roiManager->length(), _roiManager->area(), _roiManager->volume());
				break;
			}
		}
		viewer2d->updateViewer();
	}
	else if (_interactionType == connectedThreshold2d || _interactionType == neighborhoodConnected2d ||_interactionType == confidenceConnected2d) {
		
		_segmentationManager->initializeSegmentationManager();
		bool exit = false;
		switch (_interactionType) {
			case connectedThreshold2d: {
				exit = _segmentationManager->computeConnectedThresholdAlgorithm();
				wxBusyInfo wait("Please wait, segmenting...");
			}
			break;

			case neighborhoodConnected2d: {
				exit = _segmentationManager->computeNeighborhoodConnectedAlgorithm();
				wxBusyInfo wait("Please wait, segmenting...");
			}
			break;

			case confidenceConnected2d: {
				exit = _segmentationManager->computeConfidenceConnectedAlgorithm();
				wxBusyInfo wait("Please wait, segmenting...");
			}
			break;
		}
		if (exit){
			if(_roiManager){
				_roiManager->deleteAll();
				delete _roiManager;
				_roiManager = NULL;
			}
		}
	}
	else if(_interactionType == voi3dVR || _interactionType == voi3dVRwii) {		
			
		if(event.LeftDown()) {

			wxWindow* window = wxGetTopLevelParent(this);

			//Mostra nuovamente il cursore al termine di un'eventuale  rotazione
			if (_interactionType == voi3dVRwii) ((wxVolumeRenderingGui*)window)->hideMouseCursor();

			wxVTKRenderWindowInteractor::OnButtonDown(event);
			_parent->GetEventHandler()->ProcessEvent(event);
		}
	}
	unsigned int idViewer = _viewerHandler->getActiveViewer();
	if(idViewer != 0) {
		wxVtkViewer2d* viewer2d = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()));
		wxWindow* window = wxGetTopLevelParent(viewer2d->getWxWindow());
		if(strcmp(typeid(*window).name(),"class wxMainGui")==0) 
			((wxMainGui*)window)->getSlider()->SetFocus();		
	}
}

void appWxVtkInteractor::OnButtonUp(wxMouseEvent &event) {
	if(_interactionType == wii || _interactionType == wiiSR) {
		wxVTKRenderWindowInteractor::OnButtonUp(event);
		_parent->GetEventHandler()->ProcessEvent(event);
	}
	else if(_interactionType == rotateAround3d || _interactionType == rotateAround3dSR) {

		if (event.LeftUp()) {
			if (_3dTechnique==VolumeRendering) {
				wxVolumeRenderingGui* wxVRGui = ((wxVolumeRenderingGui*)wxGetTopLevelParent(this));
				wxVRGui->hide3DAxes();
				if (_3DCursorEnabled) {
					int x=0,y=0;
					wxVRGui->get3DCursorDisplayPosition(x,y,true);
					wxVRGui->WarpPointer(x,y);								
				}
			}
			else if (_3dTechnique==SurfaceRendering) {
				wxSurfaceRenderingGui* wxSRGui = ((wxSurfaceRenderingGui*)wxGetTopLevelParent(this));
				wxSRGui->hide3DAxes();
				if (_3DCursorEnabled) {
					int x=0,y=0;
					wxSRGui->get3DCursorDisplayPosition(x,y,true);
					wxSRGui->WarpPointer(x,y);								
				}
			}
		}

		if (_3DCursorEnabled)
			_3DCursorCanMove = true;
		wxVTKRenderWindowInteractor::OnButtonUp(event);	
	}
	else if(_interactionType == all2d) {
		wxVTKRenderWindowInteractor::OnButtonUp(event);
	}
	else if(_interactionType == windowLevel2d) {
		if(event.LeftUp()) {
			wxVTKRenderWindowInteractor::OnButtonUp(event);
		}
	}
	else if(_interactionType == move2d || _interactionType == move3d) {
		if(event.LeftUp()) {
			if (_3DCursorEnabled)
				_3DCursorCanMove = true;

			event.SetEventType(wxEVT_MIDDLE_UP);
			wxVTKRenderWindowInteractor::OnButtonUp(event);
		}
	}
	else if(_interactionType == zoom3dSR || _interactionType == zoom3dVR) {
		//this->GetRenderWindow()->SetDesiredUpdateRate(this->GetStillUpdateRate());
		//this->GetRenderWindow()->Render();
		//wxVTKRenderWindowInteractor::OnButtonUp(event);
		// resetta il valore per il ricalcolo del fattore di zoom
		_lastMousePosition[1]=0;
		wxVTKRenderWindowInteractor::OnButtonUp(event);
	}
	else if(_interactionType == dolly3dSR || _interactionType == dolly3dVR) {
		/*new version
		this->GetRenderWindow()->SetDesiredUpdateRate(this->GetStillUpdateRate());
		this->GetRenderWindow()->Render();
		//wxVTKRenderWindowInteractor::OnButtonUp(event);
		// resetta il valore per il ricalcolo del fattore di zoom
		_lastMousePosition[1]=0;
		wxVTKRenderWindowInteractor::OnButtonUp(event);
		*/
		if(event.LeftUp()) {
			event.SetEventType(wxEVT_RIGHT_UP);
			wxVTKRenderWindowInteractor::OnButtonUp(event);
		}
	}
	else if(_interactionType == zoom2d) {
		if(event.LeftUp()) {
			event.SetEventType(wxEVT_RIGHT_UP);
			wxVTKRenderWindowInteractor::OnButtonUp(event);
			// serve per riazzerare il calcolo della parallasse alla prossima pressione
			_lastMousePosition[1]=0;
		}
		else if(event.RightUp())
			_interactionType = all2d;
		event.SetEventType(wxEVT_RIGHT_UP);
		wxVTKRenderWindowInteractor::OnButtonUp(event);
	}
	else if(_interactionType == rotate2d || _interactionType == rotate3d) {
		if(event.LeftUp()) {
			if (_3DCursorEnabled)
				_3DCursorCanMove = true;
			event.m_controlDown = false;
			wxVTKRenderWindowInteractor::OnButtonUp(event);
		}
	}
	else if(_interactionType == length2d || _interactionType == angle2d ||_interactionType == rectangle2d || _interactionType == polygon2d || _interactionType == pencil2d) {
		unsigned int idViewer = _viewerHandler->getActiveViewer();
		wxVtkViewer2d* viewer2d = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()));
		int x = viewer2d->getCurrentX();
		int y = viewer2d->getCurrentY();

		if(_roiManager==NULL)
			_roiManager = new roiManager(viewer2d->getItkVtkData());

		_roiManager->setSliceRoi(viewer2d->GetSlice());
		_roiManager->OnButtonUp(_interactionType,event,x,y);
		
		viewer2d->getItkVtkData()->setVtkImage(_roiManager->getVtkRoi());	
		
		if(event.LeftUp()) {
			switch(_interactionType){
				case length2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),0,0);
				break;
				case angle2d:
					viewer2d->setRoiAngle(event,_roiManager->angle());
				break;
				case rectangle2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),_roiManager->area(),_roiManager->volume());
				break;
				case polygon2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),_roiManager->area(),_roiManager->volume());
				break;
				case pencil2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),_roiManager->area(),_roiManager->volume());
				break;
			}		
		}
		viewer2d->updateViewer();
	}
	else if(_interactionType == voi3dVR || _interactionType == voi3dVRwii) {
		if(event.LeftUp()) {
			wxVTKRenderWindowInteractor::OnButtonUp(event);

			wxWindow* window = wxGetTopLevelParent(this);
			
			//Mostra nuovamente il cursore al termine di un'eventuale rotazione
			if (_interactionType == voi3dVRwii) ((wxVolumeRenderingGui*)window)->showMouseCursor();

			wxVolumeRenderingGui* g3d = (wxVolumeRenderingGui*)window;
			
			// aggiorna i clipping planes
			g3d->updateClippingPlanes();
			g3d->updateVolume();
			
		}
	}
}

void appWxVtkInteractor::OnMotion(wxMouseEvent &event) 
{
	// posizionamento cursore 3D
	if ( _3DCursorEnabled && !WiiManagerStarted)
		if (_3DCursorCanMove )
		{
			if (_3dTechnique == SurfaceRendering)
			{
				wxSurfaceRenderingGui* wxSRGui = ((wxSurfaceRenderingGui*)wxGetTopLevelParent(this));
				wxSRGui->set3DCursorPosition(event.m_x, event.m_y, true);
				wxSRGui->get_viewer3d()->updateViewer();
			}
			else if (_3dTechnique == VolumeRendering)
			{
				wxVolumeRenderingGui* wxVRGui = ((wxVolumeRenderingGui*)wxGetTopLevelParent(this));
				wxVRGui->set3DCursorPosition(event.m_x, event.m_y, true);
				wxVRGui->get_viewer3d()->updateViewer();
			}
		}
		else if (event.LeftIsDown() && ( _interactionType == rotateAround3dSR || _interactionType == rotateAround3d ) )
			{				
				if (_3dTechnique == VolumeRendering)
				{
					wxVolumeRenderingGui* wxVRGui = (wxVolumeRenderingGui*)(wxGetTopLevelParent(this));					
															
					int x,y;					
					wxVRGui->GetSize(&x,&y);			
					
					int xOffSet = (0.05*x);
					int yOffSet = (0.2*y);

					if (event.GetX() > x-xOffSet)	{						
						wxVRGui->WarpPointer(xOffSet,event.GetY());								
						return;
					}
					if (event.GetX() < xOffSet)	{						
						wxVRGui->WarpPointer(x-xOffSet,event.GetY());								
						return;
					}
					if (event.GetY() > y-yOffSet)	{
						wxVRGui->WarpPointer(event.GetX(),yOffSet);								
						return;
					}					
					if (event.GetY() < yOffSet)	{
						wxVRGui->WarpPointer(event.GetX(),y-yOffSet);								
						return;
					}					

				}
				else if (_3dTechnique == SurfaceRendering)
				{
					wxSurfaceRenderingGui* wxSRGui = (wxSurfaceRenderingGui*)(wxGetTopLevelParent(this));
					
					int x,y;					
					wxSRGui->GetSize(&x,&y);			
					
					int xOffSet = (0.05*x);
					int yOffSet = (0.2*y);

					if (event.GetX() > x-xOffSet)	{						
						wxSRGui->WarpPointer(xOffSet,event.GetY());								
						return;
					}
					if (event.GetX() < xOffSet)	{						
						wxSRGui->WarpPointer(x-xOffSet,event.GetY());								
						return;
					}
					if (event.GetY() > y-yOffSet)	{
						wxSRGui->WarpPointer(event.GetX(),yOffSet);								
						return;
					}					
					if (event.GetY() < yOffSet)	{
						wxSRGui->WarpPointer(event.GetX(),y-yOffSet);								
						return;
					}					
				}
			}				


	// gestione rotazione 3D surface
	if(_interactionType == rotateAround3dSR)
	{			
		if (event.RightIsDown() && !event.LeftIsDown())
		{
			wxVTKRenderWindowInteractor::OnMotion(event);
			_parent->GetEventHandler()->ProcessEvent(event);
			((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->updateStereoView();
		}
		else if (_3DCursorEnabled)
		{
			int clockwise = _clockwise ? 666 : 0; 			
			SetEventInformationFlipY(event.GetX(),event.GetY(),event.ControlDown(),clockwise,'\0',0,NULL);
			InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
		}
		else {
			wxVTKRenderWindowInteractor::OnMotion(event);
			_parent->GetEventHandler()->ProcessEvent(event);
		}
	}
	else if(_interactionType == rotateAround3d)
	{
		/*
		if (_lastMousePosition[1] != 0 && event.RightIsDown() && !event.LeftIsDown())
		{
			double factor = 1 - (double)(event.GetY() - _lastMousePosition[1])/1000;
			wxWindow* window = wxGetTopLevelParent(this);
			((wxVolumeRenderingGui*)window)->updateZoom(factor, false);
			((wxVolumeRenderingGui*)window)->updateStereoView(true);
		}
		else
		{
			wxVTKRenderWindowInteractor::OnMotion(event);
			_parent->GetEventHandler()->ProcessEvent(event);
		}
		_lastMousePosition[1]=event.GetY();
		*/
		if (event.RightIsDown() && !event.LeftIsDown())
		{
			wxVTKRenderWindowInteractor::OnMotion(event);
			_parent->GetEventHandler()->ProcessEvent(event);
			((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->updateStereoView();
		}
		else if (_3DCursorEnabled)
		{
			int clockwise = _clockwise ? 666 : 0;		
			SetEventInformationFlipY(event.GetX(),event.GetY(),event.ControlDown(),clockwise,'\0',0,NULL);
			InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
		}
		else {
			wxVTKRenderWindowInteractor::OnMotion(event);
			_parent->GetEventHandler()->ProcessEvent(event);
		}
	}
	else if(_interactionType == zoom3dSR)
	{
		if (_lastMousePosition[1] != 0 && event.LeftIsDown())
		{
			double factor = 1 - (double)(event.GetY() - _lastMousePosition[1])/1000;
			((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->updateZoom(factor, false);
			((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->updateStereoView(true);
		}
		_lastMousePosition[1]=event.GetY();
	}
	else if(_interactionType == zoom3dVR)
	{
		if (_lastMousePosition[1] != 0 && event.LeftIsDown())
		{
			double factor = 1 - (double)(event.GetY() - _lastMousePosition[1])/1000;
			((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->updateZoom(factor, false);
			((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->updateStereoView(true);
		}
		_lastMousePosition[1]=event.GetY();
	}
	else if(_interactionType == dolly3dSR)
	{
		/* new version
		if (_lastMousePosition[1] != 0 && event.LeftIsDown())
		{
			int dy = event.GetY() - _lastMousePosition[1];
			wxWindow* window = wxGetTopLevelParent(this);
			((wxSurfaceRenderingGui*)window)->updateStereoView(dy);
		}
		_lastMousePosition[1]=event.GetY();
		*/
		
		/*
		double gzf = ((wxSurfaceRenderingGui*)window)->getGlobalZoomFactor();
		if (gzf != 1) {
			((wxSurfaceRenderingGui*)window)->updateZoom(1/gzf, false);
			((wxSurfaceRenderingGui*)window)->setGlobalZoomFactor(1);
		}
		*/
		wxVTKRenderWindowInteractor::OnMotion(event);
		_parent->GetEventHandler()->ProcessEvent(event);
		((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->updateStereoView();
	}
	else if(_interactionType == dolly3dVR)
	{
		/*
		if (_lastMousePosition[1] != 0 && event.LeftIsDown())
		{
			int dy = event.GetY() - _lastMousePosition[1];
			wxWindow* window = wxGetTopLevelParent(this);
			((wxVolumeRenderingGui*)window)->updateStereoView(dy);
		}
		_lastMousePosition[1]=event.GetY();
		*/
		
		/*
		double gzf = ((wxVolumeRenderingGui*)window)->getGlobalZoomFactor();
		if (gzf != 1) {
			((wxVolumeRenderingGui*)window)->updateZoom(1/gzf, false);
			((wxVolumeRenderingGui*)window)->setGlobalZoomFactor(1);
		}
		*/
		wxVTKRenderWindowInteractor::OnMotion(event);
		_parent->GetEventHandler()->ProcessEvent(event);
		((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->updateStereoView();
	}
	else if(_interactionType == voi3dVR) {
		((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->updateClippingPlanes();
		((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->updateVolume(false);
		wxVTKRenderWindowInteractor::OnMotion(event);
		_parent->GetEventHandler()->ProcessEvent(event);
	}
	/*
	else if(_interactionType == rotateAround3d) {
		if(event.RightIsDown()) {
			wxWindow* window = wxGetTopLevelParent(this);
			((wxVolumeRenderingGui*)window)->updateStereoView();
		}
		//wxVTKRenderWindowInteractor::OnMotion(event);
		//_parent->GetEventHandler()->ProcessEvent(event);
	}
	*/
	else if(_interactionType == animate2d && event.Dragging()) 
	{				
		unsigned int idViewer = _viewerHandler->getActiveViewer();
		wxVtkViewer2d* viewer2d = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()));

		wxWindow* window = viewer2d->getWxWindow();
		while(!(window->IsTopLevel())){
			if(window->GetParent())
				window = window->GetParent(); 			
		}
		if(_actualPoint.x == 0) _actualPoint = event.GetPosition();
		if(	event.GetPosition().x - _actualPoint.x > 0 ) 
		{ 
			if(_roiManager){
				if(_roiManager->getNumPoints()>0){
					if(viewer2d->GetSlice()==(((wx2dGui*)window)->getSlider())->GetMax())
						_roiManager->setSliceRoi((((wx2dGui*)window)->getSlider())->GetMax());
					else
						_roiManager->setSliceRoi((viewer2d->GetSlice())+1);
					_roiManager->selectAll();
					viewer2d->getItkVtkData()->setVtkImage(_roiManager->getVtkRoi());	
				}
				else {
					if(viewer2d->GetSlice()==(((wx2dGui*)window)->getSlider())->GetMax())
						_roiManager->setSliceRoi((((wx2dGui*)window)->getSlider())->GetMax());
					else
						_roiManager->setSliceRoi((viewer2d->GetSlice())+1);
					viewer2d->getItkVtkData()->setVtkImage(_roiManager->getVtkRoi());	
				}
			}

			viewer2d->SetSlice((viewer2d->GetSlice())+1);
			(((wx2dGui*)window)->getSlider())->SetValue(viewer2d->GetSlice());	
			
			
		}
		else if(event.GetPosition().x - _actualPoint.x < 0) {
			if(_roiManager){
				if(_roiManager->getNumPoints()>0){
					if(viewer2d->GetSlice()==(((wx2dGui*)window)->getSlider())->GetMin())
						_roiManager->setSliceRoi(0);
					else
						_roiManager->setSliceRoi((viewer2d->GetSlice())-1);
					_roiManager->selectAll();
					viewer2d->getItkVtkData()->setVtkImage(_roiManager->getVtkRoi());	
				}
				else{
					if(viewer2d->GetSlice()==(((wx2dGui*)window)->getSlider())->GetMin())
						_roiManager->setSliceRoi(0);
					else
						_roiManager->setSliceRoi((viewer2d->GetSlice())-1);
					viewer2d->getItkVtkData()->setVtkImage(_roiManager->getVtkRoi());	
					}	
			}
			viewer2d->SetSlice((viewer2d->GetSlice())-1);
		
			
			(((wx2dGui*)window)->getSlider())->SetValue(viewer2d->GetSlice());
			
		}
		_actualPoint = event.GetPosition();	
	}
	else if( event.Dragging() && (	_interactionType == length2d 
								||	_interactionType == angle2d 
								||	_interactionType == rectangle2d 
								||  _interactionType == polygon2d 
								||	_interactionType == pencil2d ) ) 
	{
		unsigned int idViewer = _viewerHandler->getActiveViewer();
		wxVtkViewer2d* viewer2d = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()));
		int x = viewer2d->getCurrentX();
		int y = viewer2d->getCurrentY();

		if(_roiManager==NULL)
			_roiManager = new roiManager(viewer2d->getItkVtkData());

		_roiManager->setSliceRoi(viewer2d->GetSlice());
		_roiManager->OnMotion(_interactionType,event,x,y);

		viewer2d->getItkVtkData()->setVtkImage(_roiManager->getVtkRoi());	

		if(event.LeftIsDown()) {
			switch(_interactionType){
				case length2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),0,0);
				break;
				case angle2d:
					viewer2d->setRoiAngle(event,_roiManager->angle());
				break;
				case rectangle2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),_roiManager->area(),_roiManager->volume());
				break;
				case polygon2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),_roiManager->area(),_roiManager->volume());
				break;
				case pencil2d:
					viewer2d->setRoiLengthAreaVolume(event,_roiManager->length(),_roiManager->area(),_roiManager->volume());
				break;
			}
		}
		viewer2d->updateViewer();
	}
	else if( event.Dragging() && (_interactionType == windowLevel3d)) 
	{
		wxWindow* window = wxGetTopLevelParent(this);
		wxVolumeRenderingGui* g3d = (wxVolumeRenderingGui*)window;
		unsigned int idViewer = _viewerHandler->getActiveViewer();
		unsigned int _idData = _viewerHandler->getViewer(idViewer)->getActiveIdSingleData();

		_wlWwModified = true;

		// INIZIO CALCOLO WL e WW
		long startWW = _ww, startWL = _wl;
		float WWAdapter;
		
		WWAdapter = startWW / 200.0;
		if (WWAdapter < 0.1) WWAdapter = 0.1;
		
		_wl = (long)(startWL + (long)(_actualPoint.y - event.GetPosition().y)*WWAdapter);
		_ww = (long)(startWW + (long)(event.GetPosition().x - _actualPoint.x)*WWAdapter);

		int minWL = ( (itkVtkData*)_viewerHandler->getDataHandler()->getData(_idData) )->getMinPixelValue();
		int maxWL = ( (itkVtkData*)_viewerHandler->getDataHandler()->getData(_idData) )->getMaxPixelValue();

		if		(_wl < minWL)	_wl = minWL;
		else if	(_wl > maxWL)	_wl = maxWL;

		long maxWW = 2*abs(maxWL - _wl);
		if ( 2*abs(_wl - minWL) > maxWW )
			maxWW = 2*abs(_wl - minWL);

		if		(_ww < 2)		_ww = 2;
		else if	(_ww > maxWW)	_ww = maxWW;
		// FINE CALCOLO WL e WW

		g3d->setWlWw(_wl,_ww);
		g3d->computeOpacityColor();

		_actualPoint = event.GetPosition();
		this->Render();
	}
	else if ( WiiManagerStarted )	// nascondo il mouse all'occorrenza
	{
		if ( (_interactionType == wii) || (_interactionType == voi3dVRwii) )
		{
			wxVolumeRenderingGui* wxVRGui = (wxVolumeRenderingGui*)(wxGetTopLevelParent(this));
			int x,y;
			wxVRGui->GetSize(&x,&y);			
			if ( (event.GetX() != x) || (event.GetY() != y) )		wxVRGui->WarpPointer(x,y);		
		}
		else if ( _interactionType == wiiSR )
		{
			wxSurfaceRenderingGui* wxSRGui = (wxSurfaceRenderingGui*)(wxGetTopLevelParent(this));
			int x,y;
			wxSRGui->GetSize(&x,&y);			
			if ( (event.GetX() != x) || (event.GetY() != y) )		wxSRGui->WarpPointer(x,y);		
		}
	}
	/*
    else if ( _interactionType == wii )		// nascondo il mouse all'occorrenza
	{
		wxVolumeRenderingGui* wxVRGui = (wxVolumeRenderingGui*)(wxGetTopLevelParent(this));
		int x,y;
		wxVRGui->GetSize(&x,&y);			
		if ( (event.GetX() != x) || (event.GetY() != y) )		wxVRGui->WarpPointer(x,y);		
		WiiManagerStarted = true;		
	}
	else if ( _interactionType == wiiSR )
	{
		wxSurfaceRenderingGui* wxSRGui = (wxSurfaceRenderingGui*)(wxGetTopLevelParent(this));
		int x,y;
		wxSRGui->GetSize(&x,&y);			
		if ( (event.GetX() != x) || (event.GetY() != y) )		wxSRGui->WarpPointer(x,y);		
		WiiManagerStarted = true;		
	}
	else if ( _interactionType == voi3dVRwii )
	{
		WiiManagerStarted = true;		
	}
	*/
	else if (	event.LeftIsDown() && (
				_interactionType == all2d 
			|| 	_interactionType == windowLevel2d ) )
	{	
		unsigned int idViewer = _viewerHandler->getActiveViewer();
		unsigned int _idData = _viewerHandler->getViewer(idViewer)->getActiveIdSingleData();		
		
		this->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->DrawOff();

		wxVTKRenderWindowInteractor::OnMotion(event);
		_parent->GetEventHandler()->ProcessEvent(event);		

		// Controllo estremi WL e WW
		int minWL = ( (itkVtkData*)_viewerHandler->getDataHandler()->getData(_idData) )->getMinPixelValue();
		int maxWL = ( (itkVtkData*)_viewerHandler->getDataHandler()->getData(_idData) )->getMaxPixelValue();

		long wl = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()))->GetColorLevel();
		long ww = ((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()))->GetColorWindow();

		if		(wl < minWL)	wl = minWL;
		else if	(wl > maxWL)	wl = maxWL;

		long maxWW = 2*abs(maxWL - wl);
		if ( 2*abs(wl - minWL) > maxWW )
			maxWW = 2*abs(wl - minWL);

		if		(ww < 2)		ww = 2;
		else if	(ww > maxWW)	ww = maxWW;

		((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()))->SetColorLevel(wl);
		((wxVtkViewer2d*)(_viewerHandler->getViewer(idViewer)->getViewerDrawing()))->SetColorWindow(ww);

		this->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->DrawOn();
		// FINE CONTROLLO

		this->Render();
	}
	else
	{
		wxVTKRenderWindowInteractor::OnMotion(event);
		_parent->GetEventHandler()->ProcessEvent(event);	
	}
}

void appWxVtkInteractor::OnLeaveWindow(wxMouseEvent &event) 
{
	if (_3DCursorEnabled) {
		if (_3dTechnique == SurfaceRendering ) ((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->get_viewer3d()->getRenderWindow()->ShowCursor();
		else if (_3dTechnique == VolumeRendering ) ((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->get_viewer3d()->getRenderWindow()->ShowCursor();
	}
}
void appWxVtkInteractor::OnEnterWindow(wxMouseEvent &event) 
{
	if (_3DCursorEnabled) {
		if (_3dTechnique == SurfaceRendering ) ((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->get_viewer3d()->getRenderWindow()->HideCursor();
		else if (_3dTechnique == VolumeRendering ) ((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->get_viewer3d()->getRenderWindow()->HideCursor();
	}
}
void appWxVtkInteractor::OnKeyDown(wxKeyEvent &event) 
{		
	if (WiiManagerStarted) 
	{
		/*
		if ( ( event.GetKeyCode() == 0x41 /*A) || ( event.GetKeyCode() == 0x5A /*Z) ) 
		{
			wxWindow* window = wxGetTopLevelParent(this);
			if (_interactionType == wii || _interactionType == voi3dVRwii)
				((wxVolumeRenderingGui*)window)->updateStereoView();
			else
				((wxSurfaceRenderingGui*)window)->updateStereoView();
		}

		else if ( event.GetKeyCode() == 0x58 /*X) 
		{
			wxWindow* window = wxGetTopLevelParent(this);
			wxCommandEvent myevent = wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED,0);
			if (_interactionType == wii || _interactionType == voi3dVRwii)
				((wxVolumeRenderingGui*)window)->onWii(myevent);
			else
				((wxSurfaceRenderingGui*)window)->onWii(myevent);
		}

		else if ( event.GetKeyCode() == 0x47 /*G) 
		{
			wxWindow* window = wxGetTopLevelParent(this);
			wxCommandEvent myevent = wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, 0);
			myevent.SetInt(98);
			if (_interactionType == wii)
			{
				((wxVolumeRenderingGui*)window)->onVOI(myevent);
				((wxVolumeRenderingGui*)window)->showMouseCursor();
			}
			else
			{
				((wxSurfaceRenderingGui*)window)->onVOI(myevent);
			}
		}

		else if ( event.GetKeyCode() == 0x42 /*B) 
		{
			wxWindow* window = wxGetTopLevelParent(this);
			wxCommandEvent myevent = wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, 0);
			myevent.SetInt(99);
			if (_interactionType == voi3dVRwii)
			{
				((wxVolumeRenderingGui*)window)->onVOI(myevent);
				((wxVolumeRenderingGui*)window)->hideMouseCursor();
			}
			else
			{
				((wxSurfaceRenderingGui*)window)->onVOI(myevent);
			}
		}

		*/
	}	
	else
	{
		wxVTKRenderWindowInteractor::OnKeyDown(event);
		_parent->GetEventHandler()->ProcessEvent(event); 	
	}
}

/*void appWxVtkInteractor::OnFocusTimer(wxTimerEvent& event)
{
	// Verifichiamo che il mouse sia nella finestra..
	((wxMainGui*)_viewerHandler)->getSlider()->SetFocus();
}*/

/*void appWxVtkInteractor::OnLeave(wxMouseEvent &event)
{
	if(m_focusTimer)
		m_focusTimer->Stop();
}*/

void appWxVtkInteractor::dummyMethod() 
{
	InvokeEvent(vtkCommand::LeftButtonPressEvent, NULL);
	InvokeEvent(vtkCommand::LeftButtonReleaseEvent, NULL);
}
void appWxVtkInteractor::dummyMethodRight() 
{
	InvokeEvent(vtkCommand::RightButtonPressEvent, NULL);
	InvokeEvent(vtkCommand::RightButtonReleaseEvent, NULL);
}
//----------------------------------------------- Wiimote ---------------------------------------------------------	
wxWiiManager* appWxVtkInteractor::getWiiManager(wxVtkViewer3d* viewer3d)
{
	WiiManager = wxWiiManager::getIstance( viewer3d );				
	return WiiManager;
}

void appWxVtkInteractor::closeWiiManager() 
{ 
	WiiManagerStarted = false;

	if (WiiManager != NULL) 
	{
		if (InteractionSignalsAdded) this->removeInteractionSignals();	
		delete WiiManager; 
		WiiManager = NULL;
	}
}

void appWxVtkInteractor::initInteractionSignals()
{
	int* size = WiiManager->viewer3d->getRenderWindow()->GetSize();

	float x = 0.04 * size[0];
	float y = 0.95 * size[1];	
	float radius = 0.02 * size[1];	

	vtkDiskSource* interactionSignalGeometry = vtkDiskSource::New();
	interactionSignalGeometry->SetInnerRadius(0);
	interactionSignalGeometry->SetOuterRadius(radius);
	interactionSignalGeometry->SetRadialResolution(10);
	interactionSignalGeometry->SetCircumferentialResolution(20);

	vtkPolyDataMapper2D* interactionSignalMapper = vtkPolyDataMapper2D::New();
	interactionSignalMapper->SetInput(interactionSignalGeometry->GetOutput());

	interactionSignal = vtkActor2D::New();
	interactionSignal->SetMapper(interactionSignalMapper);	
	interactionSignal->SetDisplayPosition( x, y );

	interactionSignalText = vtkTextActor::New();	
	interactionSignalText->SetDisplayPosition( x - 0.5*radius, y - 0.6*radius );
	interactionSignalText->GetTextProperty()->SetFontSize((1.5*radius));

	WiiManager->renderer->AddActor( interactionSignal );	
	WiiManager->renderer->AddActor( interactionSignalText );	
	interactionSignalGeometry->Delete();
	interactionSignalMapper->Delete();
	interactionSignal->VisibilityOff();
	interactionSignalText->VisibilityOff();

	rxfRotationAxis = vtkAxisActor2D::New();	
	rxfRotationAxis->SetPoint1(0.015,0.97);	
	rxfRotationAxis->SetPoint2(0.065,0.97);		
	rxfRotationAxis->LabelVisibilityOff();
	WiiManager->renderer->AddActor( rxfRotationAxis );
	rxfRotationAxis->VisibilityOff();
	
	ryfRotationAxis = vtkAxisActor2D::New();	
	ryfRotationAxis->SetPoint1(0.015,0.91);	
	ryfRotationAxis->SetPoint2(0.015,0.97);		
	ryfRotationAxis->LabelVisibilityOff();
	WiiManager->renderer->AddActor( ryfRotationAxis );
	ryfRotationAxis->VisibilityOff();

	/*
	//old version
	angleRotationAxisGeometry = vtkDiskSource::New();
	angleRotationAxisGeometry->SetInnerRadius(20);
	angleRotationAxisGeometry->SetOuterRadius(21);
	angleRotationAxisGeometry->SetRadialResolution(20);
	angleRotationAxisGeometry->SetCircumferentialResolution(20);
	angleRotationAxisMapper = vtkPolyDataMapper2D::New();
	angleRotationAxisMapper->SetInput(angleRotationAxisGeometry->GetOutput());	
	angleRotationAxis = vtkActor2D::New();
	angleRotationAxis->SetMapper(angleRotationAxisMapper);
	angleRotationAxis->SetDisplayPosition(x,y);
	WiiManager->renderer->AddActor( angleRotationAxis );	
	angleRotationAxis->VisibilityOff();
	*/

	// initialize polydata object
	vtkPolyData* polydata = vtkPolyData::New();
	polydata->SetLines(vtkCellArray::New());
	polydata->SetPolys(vtkCellArray::New());

	vtkPoints* points = vtkPoints::New();

    float a = 0.01, b = 0.09;			// L'ascissa  dei punti dell'arco varierà da a*Size[0] a b*Size[0]
	float c = 0.92, d = 0.87;		// L'ordinata dei punti dell'arco varierà da c*Size[1] a d*Size[1]
		
	double degreeFactor = 90;		// 2*degreeFactor corrisponde all' apertura del compasso
	double piGreco=3.141592;

	//ascissa corrispondente alla massima angolazione
	double Xmax = cos( (90-degreeFactor) * piGreco / 180);	

	//centro
	points->InsertNextPoint( (b+a)/2,  c, 0);
	
	//compasso sx
	//points->InsertNextPoint( (b-a-Xmax)*(b-a) + (b+a)/2, 0.942, 0);

	// genero i punti della semicirconferenza
	const int nPunti = 21;
	float xArc[nPunti];
	float yArc[nPunti];
	for (int i=0; i<nPunti; i++)
	{
		xArc[i] = -Xmax + i*( 2*Xmax/(nPunti-1) ) ;
		yArc[i] = sqrt(1-xArc[i]*xArc[i]);

		points->InsertNextPoint( xArc[i]*(b-a)/2 + (b+a)/2, yArc[i]*(c-d) + c, 0);
	}
	
	//compasso dx
	//points->InsertNextPoint( (Xmax-b+a)*(b-a) + (b+a)/2, 0.942, 0);

	//centro
	points->InsertNextPoint( (b+a)/2,  c, 0);
	
	polydata->SetPoints(points);
	points->Delete();

	// create line
	vtkIdList* ids = vtkIdList::New();
	for (int i=0; i<nPunti+2; i++) ids->InsertNextId(i);	
	polydata->InsertNextCell(VTK_LINE, ids);
	ids->Delete();
	
	vtkCoordinate* coordinate = vtkCoordinate::New();
	coordinate->SetCoordinateSystemToNormalizedViewport();
	
	vtkPolyDataMapper2D* angleRotationAxisMapper = vtkPolyDataMapper2D::New();
	angleRotationAxisMapper->SetTransformCoordinate(coordinate);
	angleRotationAxisMapper->SetInput(polydata);
	coordinate->Delete();
	polydata->Delete();
	
	// create actors
	angleRotationAxis = vtkActor2D::New();
	angleRotationAxis->SetMapper(angleRotationAxisMapper);
	angleRotationAxisMapper->Delete();
	WiiManager->renderer->AddActor( angleRotationAxis );	
	angleRotationAxis->VisibilityOff();

	vtkDiskSource* degreeRotationIndicatorGeometry = vtkDiskSource::New();
	degreeRotationIndicatorGeometry->SetInnerRadius(0);
	degreeRotationIndicatorGeometry->SetOuterRadius(8);
	degreeRotationIndicatorGeometry->SetRadialResolution(20);
	degreeRotationIndicatorGeometry->SetCircumferentialResolution(20);
	vtkPolyDataMapper2D* degreeRotationIndicatorMapper = vtkPolyDataMapper2D::New();
	degreeRotationIndicatorMapper->SetInput(degreeRotationIndicatorGeometry->GetOutput());
	degreeRotationIndicator = vtkActor2D::New();
	degreeRotationIndicator->SetMapper(degreeRotationIndicatorMapper);
	degreeRotationIndicator->SetDisplayPosition(x,y);
	WiiManager->renderer->AddActor( degreeRotationIndicator );
	degreeRotationIndicatorGeometry->Delete();
	degreeRotationIndicatorMapper->Delete();
	degreeRotationIndicator->VisibilityOff();

	InteractionSignalsAdded = true;

	#ifdef LOG_ACTIVATION
		// inizializza il tempo per il LOG
		QueryPerformanceCounter((LARGE_INTEGER *)&_lastUpdatedTime);
		_initialTime = _lastUpdatedTime;
	#endif
}

void appWxVtkInteractor::showInteractionSignal(int signalColor, char* text)
{	
	interactionSignalText->SetInput(text);
	
	switch (signalColor)
	{
		case 0 :	//Red			
			interactionSignal->GetProperty()->SetColor(1,0,0);
			interactionSignalText->GetTextProperty()->SetColor(1,1,1);
			break;

		case 1 :	//Yellow
			interactionSignal->GetProperty()->SetColor(1,1,0);			
			interactionSignalText->GetTextProperty()->SetColor(1,0,0);
			break;

		case 2 :	//Blu
			interactionSignal->GetProperty()->SetColor(0,0,1);			
			interactionSignalText->GetTextProperty()->SetColor(1,1,1);
			break;

		case 3 :	//Green
			interactionSignal->GetProperty()->SetColor(0,1,0);			
			interactionSignalText->GetTextProperty()->SetColor(1,0,0);
			break;
		
		case 4 :	//Violet
			interactionSignal->GetProperty()->SetColor(1,0,1);			
			interactionSignalText->GetTextProperty()->SetColor(1,1,1);
			break;

		case 5 :	//Orange
			interactionSignal->GetProperty()->SetColor(2,165,0);
			interactionSignalText->GetTextProperty()->SetColor(0,0,1);
			break;

		case 6 :	//White
			interactionSignal->GetProperty()->SetColor(1,1,1);			
			interactionSignalText->GetTextProperty()->SetColor(1,0,0);
			break;
	}	

	interactionSignal->VisibilityOn();
	interactionSignalText->VisibilityOn();
}

void appWxVtkInteractor::hideInteractionSignal()
{
	interactionSignal->VisibilityOff();
	interactionSignalText->VisibilityOff();
}
void appWxVtkInteractor::removeInteractionSignals()
{	
	WiiManager->renderer->RemoveActor( interactionSignal );
	WiiManager->renderer->RemoveActor( interactionSignalText );
	//interactionSignalGeometry->Delete();
	//interactionSignalMapper->Delete();
	interactionSignal->Delete();
	interactionSignalText->Delete();
	
	WiiManager->renderer->RemoveActor( rxfRotationAxis );
	WiiManager->renderer->RemoveActor( ryfRotationAxis );
	WiiManager->renderer->RemoveActor( angleRotationAxis );
	WiiManager->renderer->RemoveActor( degreeRotationIndicator );

	rxfRotationAxis->Delete();
	ryfRotationAxis->Delete();
	//angleRotationAxisGeometry->Delete();
	//angleRotationAxisMapper->Delete();
	angleRotationAxis->Delete();
	//degreeRotationIndicatorGeometry->Delete();
	//degreeRotationIndicatorMapper->Delete();
	degreeRotationIndicator->Delete();

	InteractionSignalsAdded = false;
}


void appWxVtkInteractor::showRotationIndicator(int rotationType, int signalColor, float p0, float p)
{
	int* size = WiiManager->viewer3d->getRenderWindow()->GetSize();

	float xmin, xmax, ymin, ymax;
	float radius = 0.018 * size[1];	

	switch (rotationType)
	{
		case 1:		// rxfRotation
			if (!rxfRotationAxis->GetVisibility())
			{
				rxfRotationAxis->VisibilityOn();
				degreeRotationIndicator->VisibilityOn();
				switch (signalColor)
				{
					case 0 :	//Red			
						degreeRotationIndicator->GetProperty()->SetColor(1,0,0);						
						break;

					case 1 :	//Yellow
						degreeRotationIndicator->GetProperty()->SetColor(1,1,0);									
						break;

					case 2 :	//Blu
						degreeRotationIndicator->GetProperty()->SetColor(0,0,1);									
						break;

					case 3 :	//Green
						degreeRotationIndicator->GetProperty()->SetColor(0,1,0);									
						break;
					
					case 4 :	//Violet
						degreeRotationIndicator->GetProperty()->SetColor(1,0,1);									
						break;

					case 5 :	//Orange
						degreeRotationIndicator->GetProperty()->SetColor(2,165,0);						
						break;

					case 6 :	//White
						degreeRotationIndicator->GetProperty()->SetColor(1,1,1);									
						break;
				}	
			}

			xmin = 0.015 * size[0];
			xmax = 0.065 * size[0];
			ymax = 0.97 * size[1];
			degreeRotationIndicator->SetDisplayPosition( xmin + (xmax-xmin)*(0.5*(p0+p)+0.5), ymax );
			break;				

		case 2:		// ryfRotation
			if (!ryfRotationAxis->GetVisibility())
			{
				ryfRotationAxis->VisibilityOn();
				degreeRotationIndicator->VisibilityOn();
				switch (signalColor)
				{
					case 0 :	//Red			
						degreeRotationIndicator->GetProperty()->SetColor(1,0,0);						
						break;

					case 1 :	//Yellow
						degreeRotationIndicator->GetProperty()->SetColor(1,1,0);									
						break;

					case 2 :	//Blu
						degreeRotationIndicator->GetProperty()->SetColor(0,0,1);									
						break;

					case 3 :	//Green
						degreeRotationIndicator->GetProperty()->SetColor(0,1,0);									
						break;
					
					case 4 :	//Violet
						degreeRotationIndicator->GetProperty()->SetColor(1,0,1);									
						break;

					case 5 :	//Orange
						degreeRotationIndicator->GetProperty()->SetColor(2,165,0);						
						break;

					case 6 :	//White
						degreeRotationIndicator->GetProperty()->SetColor(1,1,1);									
						break;
				}	
			}

			xmin = 0.015 * size[0];
			ymin = 0.91 * size[1];
			ymax = 0.97 * size[1];			
			degreeRotationIndicator->SetDisplayPosition( xmin, ymin + (ymax-ymin)*(-0.5*(p0+p)+0.5) );
			break;

		case 3:		// angleRotation
			if (!angleRotationAxis->GetVisibility())
			{
				angleRotationAxis->VisibilityOn();
				degreeRotationIndicator->VisibilityOn();
				switch (signalColor)
				{
					case 0 :	//Red			
						degreeRotationIndicator->GetProperty()->SetColor(1,0,0);						
						break;

					case 1 :	//Yellow
						degreeRotationIndicator->GetProperty()->SetColor(1,1,0);									
						break;

					case 2 :	//Blu
						degreeRotationIndicator->GetProperty()->SetColor(0,0,1);									
						break;

					case 3 :	//Green
						degreeRotationIndicator->GetProperty()->SetColor(0,1,0);									
						break;
					
					case 4 :	//Violet
						degreeRotationIndicator->GetProperty()->SetColor(1,0,1);									
						break;

					case 5 :	//Orange
						degreeRotationIndicator->GetProperty()->SetColor(2,165,0);						
						break;

					case 6 :	//White
						degreeRotationIndicator->GetProperty()->SetColor(1,1,1);									
						break;
				}	
			}

			/*
			xmin = 0.04 * size[0] - radius;
			xmax = 0.04 * size[0] + radius;
			ymin = 0.95 * size[1] - radius;			
			ymax = 0.95 * size[1];			
			
			degreeRotationIndicator->
				SetDisplayPosition( xmin + (xmax-xmin)*(-0.5*(p0+p)+0.5), 
									ymax - (ymax-ymin)*( ((1-(p0+p)*(p0+p))<0) ? 0 : (sqrt(1-(p0+p)*(p0+p))) ) );
			*/

			float a = 0, b = 0.08;		// L'ascissa  dei punti dell'arco varierà da a*Size[0] a b*Size[0]
			float c = 0.92, d = 0.87;		// L'ordinata dei punti dell'arco varierà da c*Size[1] a d*Size[1]
			double degreeFactor = 25;
			double piGreco=3.141592;
			//ascissa corrispondente alla massima angolazione
			double Xmax = cos( (90-degreeFactor) * piGreco / 180 );	
			float x = (p0+p)>1 ? Xmax : ( (p0+p)<-1 ? -Xmax : (p0+p)*Xmax );
			float y = sqrt(1-x*x);
			degreeRotationIndicator->SetDisplayPosition( (x*(b-a)/2 + (b+a)/2)*size[0],(y*(c-d)+c)*size[1]);
			break;
	}	

}
void appWxVtkInteractor::showRotationIndicator(int rotationType, float p, int signalColor)
{
	int* size = WiiManager->viewer3d->getRenderWindow()->GetSize();

	float xmin, xmax, ymin, ymax;
	float radius = 0.018 * size[1];	

	if (p>1) {p=1;}	else if (p<-1) {p=-1;}

	switch (rotationType)
	{
		case 1:		// rxfRotation
			if (!rxfRotationAxis->GetVisibility())
			{
				rxfRotationAxis->VisibilityOn();
				degreeRotationIndicator->VisibilityOn();
				switch (signalColor)
				{
					case 0 :	//Red			
						degreeRotationIndicator->GetProperty()->SetColor(1,0,0);						
						break;

					case 1 :	//Yellow
						degreeRotationIndicator->GetProperty()->SetColor(1,1,0);									
						break;

					case 2 :	//Blu
						degreeRotationIndicator->GetProperty()->SetColor(0,0,1);									
						break;

					case 3 :	//Green
						degreeRotationIndicator->GetProperty()->SetColor(0,1,0);									
						break;
					
					case 4 :	//Violet
						degreeRotationIndicator->GetProperty()->SetColor(1,0,1);									
						break;

					case 5 :	//Orange
						degreeRotationIndicator->GetProperty()->SetColor(2,165,0);						
						break;

					case 6 :	//White
						degreeRotationIndicator->GetProperty()->SetColor(1,1,1);									
						break;
				}	
			}

			xmin = 0.015 * size[0];
			xmax = 0.065 * size[0];
			ymax = 0.97 * size[1];
			degreeRotationIndicator->SetDisplayPosition( p*(xmax-xmin)/2 + xmax - (xmax-xmin)/2, ymax );
			break;				

		case 2:		// ryfRotation
			if (!ryfRotationAxis->GetVisibility())
			{
				ryfRotationAxis->VisibilityOn();
				degreeRotationIndicator->VisibilityOn();
				switch (signalColor)
				{
					case 0 :	//Red			
						degreeRotationIndicator->GetProperty()->SetColor(1,0,0);						
						break;

					case 1 :	//Yellow
						degreeRotationIndicator->GetProperty()->SetColor(1,1,0);									
						break;

					case 2 :	//Blu
						degreeRotationIndicator->GetProperty()->SetColor(0,0,1);									
						break;

					case 3 :	//Green
						degreeRotationIndicator->GetProperty()->SetColor(0,1,0);									
						break;
					
					case 4 :	//Violet
						degreeRotationIndicator->GetProperty()->SetColor(1,0,1);									
						break;

					case 5 :	//Orange
						degreeRotationIndicator->GetProperty()->SetColor(2,165,0);						
						break;

					case 6 :	//White
						degreeRotationIndicator->GetProperty()->SetColor(1,1,1);									
						break;
				}	
			}

			xmin = 0.015 * size[0];
			ymin = 0.91 * size[1];
			ymax = 0.97 * size[1];			
			degreeRotationIndicator->SetDisplayPosition( xmin, p*(ymax-ymin)/2 + ymax - (ymax-ymin)/2 );
			break;

		case 3:		// angleRotation
			if (!angleRotationAxis->GetVisibility())
			{
				angleRotationAxis->VisibilityOn();
				degreeRotationIndicator->VisibilityOn();
				switch (signalColor)
				{
					case 0 :	//Red			
						degreeRotationIndicator->GetProperty()->SetColor(1,0,0);						
						break;

					case 1 :	//Yellow
						degreeRotationIndicator->GetProperty()->SetColor(1,1,0);									
						break;

					case 2 :	//Blu
						degreeRotationIndicator->GetProperty()->SetColor(0,0,1);									
						break;

					case 3 :	//Green
						degreeRotationIndicator->GetProperty()->SetColor(0,1,0);									
						break;
					
					case 4 :	//Violet
						degreeRotationIndicator->GetProperty()->SetColor(1,0,1);									
						break;

					case 5 :	//Orange
						degreeRotationIndicator->GetProperty()->SetColor(2,165,0);						
						break;

					case 6 :	//White
						degreeRotationIndicator->GetProperty()->SetColor(1,1,1);									
						break;
				}	
			}

			float a = 0.01, b = 0.09;		// L'ascissa  dei punti dell'arco varierà da a*Size[0] a b*Size[0]
			float c = 0.92, d = 0.87;		// L'ordinata dei punti dell'arco varierà da c*Size[1] a d*Size[1]
			double degreeFactor = 90;
			double piGreco=3.141592;
			//ascissa corrispondente alla massima angolazione
			double Xmax = cos( (90-degreeFactor) * piGreco / 180 );	
//			float x = (p0+p)>1 ? Xmax : ( (p0+p)<-1 ? -Xmax : (p0+p)*Xmax );
			float x = p*Xmax ;
			float y = sqrt(1-x*x);
			degreeRotationIndicator->SetDisplayPosition( (x*(b-a)/2 + (b+a)/2)*size[0],(y*(c-d)+c)*size[1]);
			break;
	}	

}
void appWxVtkInteractor::hideRotationIndicator()
{
	rxfRotationAxis->VisibilityOff();
	ryfRotationAxis->VisibilityOff();
	angleRotationAxis->VisibilityOff();
	degreeRotationIndicator->VisibilityOff();
}

/*
void appWxVtkInteractor::updateLastRenderTime(double delay)
{
	GetSystemTime(&this->st);
	lastRenderTime = st.wSecond*1000 + st.wMilliseconds + delay;		
}
*/

inline void appWxVtkInteractor::updateLastRenderTime()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&_lastRenderTime);
}

bool appWxVtkInteractor::isRenderingNeeded(int frameRate)
{
	__int64 time;
	QueryPerformanceCounter((LARGE_INTEGER *)&time);
	time = (time - _lastRenderTime) * 1000 / _clockFrequency; // difference in milliseconds
	
	if ( time >= (1000/frameRate) ) return true;
	return false;
}

/*
bool appWxVtkInteractor::isRenderingNeeded(int frameRate)
{
	GetSystemTime(&this->st);
	int sTime = st.wSecond*1000+st.wMilliseconds;

	if ( abs(sTime-lastRenderTime) >= (1000/frameRate) ) 
	{
		return true;
	}
	return false;
}
*/


bool appWxVtkInteractor::closeWiiAbility()
{
	//controllo se posso uscire o meno
	/*
	if ( (idWiiEnabled != AnyWiiEnabled) || WiiManager->WiiPointing_num ) return false;
	else if (_interactionType == voi3dVRwii)	
	{
		InvokeEvent(vtkWiiHomeDown,NULL);
		//this->GetRenderWindow()->SetDesiredUpdateRate(this->GetStillUpdateRate());
	}	
	return true;
	*/
	if (_interactionType == voi3dVRwii)	
	{
		InvokeEvent(vtkWiiHomeDown,NULL);
	}	
	return true;
}

void appWxVtkInteractor::prismStyle1DFilter(float newPosition, float & oldPosition, float & invCDRatio, wxWiiManager::Wiimote* thisWii)
{
	// filtro 1D stile prism
	__int64 currentTime;
	if (QueryPerformanceCounter((LARGE_INTEGER *)&currentTime)!= 0) {
		if ( ((currentTime - thisWii->lastVelocityTime) * 1000 / this->_clockFrequency) >= thisWii->interval*1000 ) {
			// calcola lo spostamento rispetto alla rilevazione precedente
			float delta = newPosition - oldPosition;
			
			// aggiorna il valore precedente
			oldPosition = newPosition;

			// calcola il C-D ratio in funzione dello spostamento
			if (abs(delta) < thisWii->minRotDISP)
				invCDRatio = 0;
			else if (abs(delta) < thisWii->maxRotDISP)
				invCDRatio = (abs(delta) - thisWii->minRotDISP) / (thisWii->maxRotDISP - thisWii->minRotDISP);
			else
				invCDRatio = 1;
		
			// aggiorna il valore temporale dell'ultimo spostamento
			thisWii->lastVelocityTime = currentTime;
		}	
	}
}
bool appWxVtkInteractor::prismStyle2DFilter(float newX, float & oldX, float newY, float & oldY, float & invCDRatio, wxWiiManager::Wiimote* thisWii)
{
	// filtro stile PRISM per rotazione 2DoF o puntamento
	__int64 currentTime;
	if (QueryPerformanceCounter((LARGE_INTEGER *)&currentTime)!= 0) {
		if ( ((currentTime - thisWii->lastVelocityTime) * 1000 / this->_clockFrequency) >= thisWii->interval*1000 ) {
			// calcola lo spostamento rispetto alla rilevazione precedente
			float dx = newX - oldX;
			float dy = newY - oldY;
			
			// aggiorna i valori precedenti
			oldX = newX;
			oldY = newY;

			// calcola la diagonale dello spostamento
			float dd;
			if (dx && dy)
				dd = sqrt(pow(dx,2) + pow(dy,2));
			else
				dd = 0;

			// calcola il C-D ratio in funzione dello spostamento
			if (dd < _minDISP)
				invCDRatio = 0;
			else if (dd < _maxDISP)
				invCDRatio = (dd - _minDISP) / (_maxDISP - _minDISP);
			else
				invCDRatio=1;
		
			// aggiorna il valore temporale dell'ultimo spostamento
			thisWii->lastVelocityTime = currentTime;
			return true;
		}
	}
	return false;
}

void appWxVtkInteractor::onWiiMotionIR( wxWiimoteEvent & event) 
{	
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	
	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{	
		{
		#ifdef LOG_ACTIVATION
			// log new
			if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
				char buffer[64];
				string r0, r1, r2, r3;
				r0 = _itoa(event.idWii, buffer, 10);
				r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10); // microseconds
				r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
				r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
				string strDatanew = "idWii: " + r0 + 
									"\tsTime: " + r1 + " mcs" +
									"\tDelay(last instr): " + r2 + " mcs" + 
									"\tDelay(event): " + r3 + 
									" ms\tEntrato nel motion";
				list.push_back(strDatanew);
				_lastUpdatedTime = _updatedTime;
			}
			// fine log new
		#endif
		}
		
		wxWindow* window = wxGetTopLevelParent(this);		

		//LUIGI
		if ( thisWii->Mode == SceneRotatingIR && isRenderingNeeded(renderingRate) )
		{
			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + 
										" ms\tEntrato in SceneRotatingIR";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new
			#endif
			}
			
			// filtro stile PRISM per la rotazione a 2 DoF
			/* Nello
			GetSystemTime(&this->st);
			int sTime = st.wSecond*1000+st.wMilliseconds;			
			if ( abs(sTime-lastVelocityTime) >= interval*1000 ) {
				// calcola lo spostamento rispetto alla rilevazione precedente
				float dx = event.IRLight1X - xPositionVelocity;
				float dy = event.IRLight1Y - yPositionVelocity;
				
				// aggiorna i valori precedenti
				xPositionVelocity = event.IRLight1X;
				yPositionVelocity = event.IRLight1Y;

				// calcola la diagonale dello spostamento
				float dd;
				if (dx && dy)
					dd = sqrt(pow(dx,2) + pow(dy,2));
				else
					dd = 0;

				// calcola il C-D ratio in funzione dello spostamento
				if (dd < minDISP)
					invCDratio = 0;
				else if (dd < maxDISP)
					invCDratio = (dd - minDISP) / (maxDISP - minDISP);
				else
					invCDratio=1;
			
				// aggiorna il valore temporale dell'ultimo spostamento
				lastVelocityTime = sTime;
			}
			*/
			//prismStyle2DFilter( thisWii->lastVelocityTime, thisWii->interval, thisWii->invCDratio, 
			//					event.IRLight1X, thisWii->xPositionVelocity, event.IRLight1Y, thisWii->yPositionVelocity );
			prismStyle2DFilter( event.IRLight1X, thisWii->xPositionVelocity, event.IRLight1Y, thisWii->yPositionVelocity,
								thisWii->invCDratio, thisWii);
			
			// calcola la nuova posizione (xIR, yIR)
			float x = thisWii->lastIR_X[0] + thisWii->invCDratio * (event.IRLight1X - thisWii->lastIR_X[0]);
			float y = thisWii->lastIR_Y[0] + thisWii->invCDratio * (event.IRLight1Y - thisWii->lastIR_Y[0]);
			// fine filtro PRISM-like			

			// riconduce i valori x e y forniti dalla IR camera nel range [size[0],size[1]]
			int* size;
			size = WiiManager->viewer3d->getRenderWindow()->GetSize();
			x = ( size[0] * (1023 - event.IRLight1X ) ) / 1023;
			y = ( size[1] * (767 - event.IRLight1Y ) ) / 767;						

			float dx = x - thisWii->lastIR_X[0];
			float dy = y - thisWii->lastIR_Y[0];

			// aggiorna il valore precedente 
			thisWii->lastIR_X[0] = x;
			thisWii->lastIR_Y[0] = y;
		
			double delta_elevation = -20.0 / size[1]; // pitch
			double delta_azimuth = 20.0 / size[0]; // yaw
			double MotionFactor = 10;

			// calcola l'entita' della rotazione della scena
			double yaw = (double)dx * delta_azimuth * MotionFactor;
			double pitch = (double)dy * delta_elevation * MotionFactor;

			// l'angolo cambia a seconda del clockwise calcolato alla pressione del bottone
			yaw = _clockwise ? -yaw : yaw;
			pitch = _clockwise ? -pitch : pitch;	

			// calcolo della coordinata z del puntatore
			if (!event.B) 
			{
				// Intersect with line
				double point[3];	
				if ( _3dTechnique == SurfaceRendering ) {
					thisWii->isOnSurface = ((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->intersectingModelSurface(x,y,point);
				}
				else if ( _3dTechnique == VolumeRendering ) {
					thisWii->isOnSurface = ((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->intersectingModelSurface(x,y,point);
				}

				if ( thisWii->isOnSurface ) {
					thisWii->pointer->SetPosition(point[0], point[1], point[2]);
					thisWii->pointer->GetProperty()->SetColor(1,1,0);
				}
				// puntatore fuori dalla superficie - colore rosso
				else {	                
					double focalDepth, focalPosition[3], newPosition[4];
					WiiManager->camera->GetFocalPoint(focalPosition);
					GetInteractorStyle()->ComputeWorldToDisplay( WiiManager->renderer, focalPosition[0], focalPosition[1], 
																focalPosition[2], focalPosition);				
					focalDepth = focalPosition[2];
					GetInteractorStyle()->ComputeDisplayToWorld( WiiManager->renderer, x, y, focalDepth, newPosition);

					thisWii->pointer->SetPosition( newPosition[0], newPosition[1], newPosition[2]);
					thisWii->pointer->GetProperty()->SetColor(1,1,1);
				}
			}
					
			// invia a VTK l'evento rotazione indicando l'entita' della stessa
			SetShiftKey( yaw * 100 /*rxf*/ );
			SetEventPosition( 0 /*angle*/ , pitch * 100 /*ryf*/ );
			
			{ 
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + 
										" ms\tEseguo render da SceneRotatingIR";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new

			#endif
			}
			
			InvokeEvent(vtkWiiMotion, NULL);
			this->updateLastRenderTime();

			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + " ms" +
										+"\tRender eseguito da SceneRotatingIR\n";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new
			#endif
			}
			
		}		
		else if ( thisWii->Mode == SceneRotating )
		{	
			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + 
										" ms\tENTRATO IN SCENE ROTATING??????????????";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new

			#endif
			}
			
			// Calcolo l'ascissa della posizione puntata dagli infrarossi.
			float IR_X = 0;
			//if (event.IRLight1X != -1)
			{
				int x;
				int* size;
				size = WiiManager->viewer3d->getRenderWindow()->GetSize();
				x = ( size[0] * (1023 - event.IRLight1X ) ) / 1023;				
                				
				float rangex = size[0]/2;				
				IR_X = (x-rangex)/rangex;				
			}
			IR_X = IR_X - thisWii->IR_Xo;

			float WiiX = event.WiiX, WiiY = event.WiiY;	
				
			
			// Il wiimote misura anche l'accellerazione delle rotazioni, dando valori esterni all'intervallo
			// [-1,1] in funzione dell'accellerazione impressa. Per evitare valori esterni a tale intervallo
			// imponiamo i seguenti limiti.
			if (WiiX>1) {WiiX=1;}	else if (WiiX<-1) {WiiX=-1;}
			if (WiiY>1) {WiiY=1;}	else if (WiiY<-1) {WiiY=-1;}
			
			double minDeltaWiiX = 0.08,	minDeltaWiiY = 0.08, minDeltaWiiIR = 0.08;
			double maxDeltaWiiX = 0.6,	maxDeltaWiiY = 0.6,  maxDeltaWiiIR = 0.6;					
			double WiiX_window = 0.16, WiiY_window = 0.16, WiiIR_window = 0.16;
			double degreeFactor = 30;			

			// Valuto l'angolazione in funzione della posizione iniziale del wii.
			WiiX = WiiX - thisWii->WiiXo;
			WiiY = WiiY - thisWii->WiiYo;

			if ( !(enabled_angle_Rotation || enabled_ryf_Rotation || enabled_rxf_Rotation) )
			{
				if (	( abs(WiiX)					> WiiX_window )		&&
						( abs(thisWii->lastWiiX[0]) > WiiX_window )		&&
						( abs(thisWii->lastWiiX[1]) > WiiX_window )		&&
						( abs(thisWii->lastWiiX[2]) > WiiX_window )		&&
						( abs(thisWii->lastWiiX[3]) > WiiX_window )			)
				{
					enabled_angle_Rotation = true;					
					thisWii->lastWiiX[0]=0;
				}
				else if (	( abs(WiiY)					> WiiY_window )		&&
							( abs(thisWii->lastWiiY[0]) > WiiY_window )		&&
							( abs(thisWii->lastWiiY[1]) > WiiY_window )		&&
							( abs(thisWii->lastWiiY[2]) > WiiY_window )		&&
							( abs(thisWii->lastWiiY[3]) > WiiY_window )			)
				{
					enabled_ryf_Rotation = true;
					thisWii->lastWiiY[0]=0;
				}
				else if (	( abs(IR_X)					> WiiIR_window )		&&
							( abs(thisWii->lastIR_X[0]) > WiiIR_window )		&&
							( abs(thisWii->lastIR_X[1]) > WiiIR_window )		&&
							( abs(thisWii->lastIR_X[2]) > WiiIR_window )		&&
							( abs(thisWii->lastIR_X[3]) > WiiIR_window )			)
				{
					enabled_rxf_Rotation = true;					
				}
			}
			else if (!isRenderingNeeded(renderingRate)) return;			

			if (enabled_angle_Rotation)
			{
				WiiY = IR_X = 0;

				double deltaWiiX = WiiX - thisWii->lastWiiX[0];
				if ( abs(deltaWiiX) < minDeltaWiiX )	{ WiiX = thisWii->lastWiiX[0]; }
				else if ( deltaWiiX >=  maxDeltaWiiX )	{ WiiX = thisWii->lastWiiX[0] + maxDeltaWiiX; }
				else if ( deltaWiiX <= -maxDeltaWiiX )	{ WiiX = thisWii->lastWiiX[0] - maxDeltaWiiX; }
				
				for (int i=2; i>0; i--)	{ thisWii->lastWiiX[i] = thisWii->lastWiiX[i-1]; }				
				thisWii->lastWiiX[0] = WiiX;				

				WiiX = ( thisWii->lastWiiX[0] + thisWii->lastWiiX[1] + thisWii->lastWiiX[2] ) / 3;				
				
				showRotationIndicator( 3, thisWii->pointerColor, thisWii->WiiXo, WiiX );
			}
			else if (enabled_ryf_Rotation)
			{
				WiiX = IR_X = 0;
				double deltaWiiY = WiiY - thisWii->lastWiiY[0];
				if ( abs(deltaWiiY) < minDeltaWiiY )	{ WiiY = thisWii->lastWiiY[0]; }
				else if ( deltaWiiY >=  maxDeltaWiiY )	{ WiiY = thisWii->lastWiiY[0] + maxDeltaWiiY; }
				else if ( deltaWiiY <= -maxDeltaWiiY )	{ WiiY = thisWii->lastWiiY[0] - maxDeltaWiiY; }
				
				for (int i=2; i>0; i--)	{ thisWii->lastWiiY[i] = thisWii->lastWiiY[i-1]; }				
				thisWii->lastWiiY[0] = WiiY;				

				WiiY = ( thisWii->lastWiiY[0] + thisWii->lastWiiY[1] + thisWii->lastWiiY[2] ) / 3;				

				showRotationIndicator( 2, thisWii->pointerColor, thisWii->WiiYo, WiiY );
			}
			else if (enabled_rxf_Rotation)
			{
				WiiX = WiiY = 0;
				
				double deltaWiiIR = IR_X - thisWii->lastIR_X[0];
				if ( abs(deltaWiiIR) < minDeltaWiiIR )	{ IR_X = thisWii->lastIR_X[0]; }
				else if ( deltaWiiIR >=  maxDeltaWiiIR )	{ IR_X = thisWii->lastIR_X[0] + maxDeltaWiiIR; }
				else if ( deltaWiiIR <= -maxDeltaWiiIR )	{ IR_X = thisWii->lastIR_X[0] - maxDeltaWiiIR; }				

				for (int i=2; i>0; i--)	{ thisWii->lastIR_X[i] = thisWii->lastIR_X[i-1]; }				
				
				thisWii->lastIR_X[0] = IR_X;				

				IR_X = ( thisWii->lastIR_X[0] + thisWii->lastIR_X[1] + thisWii->lastIR_X[2] ) / 3;				
								
				showRotationIndicator( 1, thisWii->pointerColor, thisWii->IR_Xo, IR_X );
			}
			else 
			{
				for (int i=4; i>0; i--)	{ thisWii->lastWiiX[i] = thisWii->lastWiiX[i-1]; }				
				thisWii->lastWiiX[0] = WiiX;				
	
				for (int i=4; i>0; i--)	{ thisWii->lastWiiY[i] = thisWii->lastWiiY[i-1]; }				
				thisWii->lastWiiY[0] = WiiY;				

				for (int i=4; i>0; i--)	{ thisWii->lastIR_X[i] = thisWii->lastIR_X[i-1]; }				
				thisWii->lastIR_X[0] = IR_X;	

				WiiX = WiiY = IR_X = 0;
			}
			
			WiiX = (WiiX*degreeFactor);
			WiiY = (WiiY*degreeFactor);
			
			WiiManager->camera->SetPosition(camPosition);
			WiiManager->camera->SetViewUp(camViewUp);
												
			SetShiftKey( IR_X*41 * 100 /*rxf*/ );
			SetEventPosition( WiiX * 100 /*angle*/ , WiiY * 100 /*ryf*/ );

			InvokeEvent(vtkWiiMotion, NULL);
		}

		else if ( thisWii->Mode == SceneRotatingIR_1 ) 
		{	
			// Calcolo la IR_X, IR_Y e WiiX normalizzati in [-1,1]
			float IR_X = 0, IR_Y = 0;
			float WiiX = event.WiiX;
			
			int x,y;
			int* size;

			size = WiiManager->viewer3d->getRenderWindow()->GetSize();
			x = ( size[0] * (1023 - event.IRLight1X ) ) / 1023;				
			y = ( size[1] * ( 767 - event.IRLight1Y ) ) /  767;				
		                	
			float rangex = size[0]/2;
			IR_X = (x-rangex)/rangex;

			float rangey = size[1]/2;
			IR_Y = (y-rangey)/rangey;

			if (WiiX>1) {WiiX=1;}	else if (WiiX<-1) {WiiX=-1;}
			
			// N.B. IR_X e IR_Y e WiiX qui sono tutti DELTA e non valori assoluti
			IR_X = IR_X - thisWii->IR_Xo;
			IR_Y = IR_Y - thisWii->IR_Yo;
			WiiX = WiiX - thisWii->WiiXo;
		
			double WiiX_window  = 0.16,  WiiIR_window = 0.2; 

			// identificazione tipo di rotazione (yaw, pitch, roll)
			if ( !(enabled_angle_Rotation || enabled_ryf_Rotation || enabled_rxf_Rotation) )
			{
				// identifica un ROLL
				if (	( abs(WiiX)					> WiiX_window )		&&
						( abs(thisWii->lastWiiX[0]) > WiiX_window )		&&
						( abs(thisWii->lastWiiX[1]) > WiiX_window )		&&
						( abs(thisWii->lastWiiX[2]) > WiiX_window )		&&
						( abs(thisWii->lastWiiX[3]) > WiiX_window )			)
				{
					enabled_angle_Rotation = true;
					thisWii->WiiXo = event.WiiX;
					thisWii->zPositionVelocity = thisWii->lastWiiX[0] = 0;
					return;
				}
				// identifica un PITCH
				else if (	( abs(IR_Y)					> WiiIR_window )		&&
							( abs(thisWii->lastIR_Y[0])  > WiiIR_window )		&&
							( abs(thisWii->lastIR_Y[1])  > WiiIR_window )		&&
							( abs(thisWii->lastIR_Y[2])  > WiiIR_window )		&&
							( abs(thisWii->lastIR_Y[3]) > WiiIR_window )			)
				{							
					enabled_ryf_Rotation = true;
					thisWii->IR_Yo = (y-rangey)/rangey;
					thisWii->yPositionVelocity = thisWii->lastIR_Y[0] = 0;
					return;
				}
				// identifica una YAW
				else if (	( abs(IR_X)					> WiiIR_window )		&&
							( abs(thisWii->lastIR_X[0]) > WiiIR_window )		&&
							( abs(thisWii->lastIR_X[1]) > WiiIR_window )		&&
							( abs(thisWii->lastIR_X[2]) > WiiIR_window )		&&
							( abs(thisWii->lastIR_X[3]) > WiiIR_window )			)
				{
					enabled_rxf_Rotation = true;
					thisWii->IR_Xo = (x-rangex)/rangex;
					thisWii->xPositionVelocity = thisWii->lastIR_X[0] = 0;
					return;
				}
			}
			else if (!isRenderingNeeded(renderingRate)) return;			

			// valore massimo = 170
			double degreeFactor = 170;
		
			// identificato un ROLL
			if (enabled_angle_Rotation) {
				IR_Y = IR_X = 0;

				
				// filtro stile prism per il roll
				/* Nello
				GetSystemTime(&this->st);
				int sTime = st.wSecond*1000+st.wMilliseconds;				
				if ( abs(sTime-lastVelocityTime) >= interval*1000 ) {
					// calcola lo spostamento rispetto alla rilevazione precedente
					float dz = WiiX - zPositionVelocity;
					
					// aggiorna il valore precedente
					zPositionVelocity = WiiX;

					// calcola il C-D ratio in funzione dello spostamento
					if (abs(dz) < minRotDISP)
						invCDRollRatio = 0;
					else if (abs(dz) < maxDISP)
						invCDRollRatio = (abs(dz) - minRotDISP) / (maxRotDISP - minRotDISP);
					else
						invCDRollRatio=1;
				
					// aggiorna il valore temporale dell'ultimo spostamento
					lastVelocityTime = sTime;
				}
				*/				
				prismStyle1DFilter(WiiX, thisWii->zPositionVelocity, thisWii->invCDRollRatio, thisWii);

				float dz = WiiX - thisWii->lastWiiX[0];
				WiiX = dz * thisWii->invCDRollRatio + thisWii->lastWiiX[0];
				
				for (int i=2; i>0; i--)	{ thisWii->lastWiiX[i] = thisWii->lastWiiX[i-1]; }				
				thisWii->lastWiiX[0] = WiiX;
				// fine filtro

				showRotationIndicator(3, WiiX+thisWii->WiiXo, thisWii->pointerColor);
			}
			// identificato un PITCH
			else if (enabled_ryf_Rotation)
			{
				WiiX = IR_X = 0;

				// filtro stile prism per il pitch
				/* Nello
				GetSystemTime(&this->st);
				int sTime = st.wSecond*1000+st.wMilliseconds;				
				if ( abs(sTime-lastVelocityTime) >= interval*1000 ) {
					// calcola lo spostamento rispetto alla rilevazione precedente
					float dy = IR_Y - yPositionVelocity;
					
					// aggiorna il valore precedente
					yPositionVelocity = IR_Y;

					// calcola il C-D ratio in funzione dello spostamento
					if (abs(dy) < minRotDISP)
						invCDRollRatio = 0;
					else if (abs(dy) < maxDISP)
						invCDRollRatio = (abs(dy) - minRotDISP) / (maxRotDISP - minRotDISP);
					else
						invCDRollRatio=1;
				
					// aggiorna il valore temporale dell'ultimo spostamento
					lastVelocityTime = sTime;
				}
				*/
				//prismStyle1DFilter(thisWii->lastVelocityTime, thisWii->interval, thisWii->invCDRollRatio, thisWii->yPositionVelocity, IR_Y);
				prismStyle1DFilter(IR_Y, thisWii->yPositionVelocity, thisWii->invCDRollRatio, thisWii);

				float dy = IR_Y - thisWii->lastIR_Y[0];
				IR_Y = dy * thisWii->invCDRollRatio + thisWii->lastIR_Y[0];
				
				for (int i=2; i>0; i--)	{ thisWii->lastIR_Y[i] = thisWii->lastIR_Y[i-1]; }				
				thisWii->lastIR_Y[0] = IR_Y;
				// fine filtro

				showRotationIndicator(2, IR_Y+thisWii->IR_Yo, thisWii->pointerColor);
			}
			// identificata una YAW
			else if (enabled_rxf_Rotation)
			{
				WiiX = IR_Y = 0;
				
				// filtro stile prism per la yaw
				/* Nello
				GetSystemTime(&this->st);
				int sTime = st.wSecond*1000+st.wMilliseconds;				
				if ( abs(sTime-lastVelocityTime) >= interval*1000 ) {
					// calcola lo spostamento rispetto alla rilevazione precedente
					float dx = IR_X - xPositionVelocity;
					
					// aggiorna il valore precedente
					xPositionVelocity = IR_X;

					// calcola il C-D ratio in funzione dello spostamento
					if (abs(dx) < minRotDISP)
						invCDRollRatio = 0;
					else if (abs(dx) < maxDISP)
						invCDRollRatio = (abs(dx) - minRotDISP) / (maxRotDISP - minRotDISP);
					else
						invCDRollRatio=1;
				
					// aggiorna il valore temporale dell'ultimo spostamento
					lastVelocityTime = sTime;
				}
				*/
				//prismStyle1DFilter(thisWii->lastVelocityTime, thisWii->interval, thisWii->invCDRollRatio, thisWii->xPositionVelocity, IR_X);
				prismStyle1DFilter(IR_X, thisWii->xPositionVelocity, thisWii->invCDRollRatio, thisWii);

				float dx = IR_X - thisWii->lastIR_X[0];
				IR_X = dx * thisWii->invCDRollRatio + thisWii->lastIR_X[0];
				
				for (int i=2; i>0; i--)	{ thisWii->lastIR_X[i] = thisWii->lastIR_X[i-1]; }				
				thisWii->lastIR_X[0] = IR_X;
				// fine filtro
				
				showRotationIndicator(1, IR_X+thisWii->IR_Xo, thisWii->pointerColor);
			}
			// nulla è stato identificato, aggiorna i valori per le identificazioni future
			else 
			{
				for (int i=4; i>0; i--)	{ thisWii->lastWiiX[i] = thisWii->lastWiiX[i-1]; }				
				thisWii->lastWiiX[0] = WiiX;				

				for (int i=4; i>0; i--)	{ thisWii->lastIR_Y[i] = thisWii->lastIR_Y[i-1]; }				
				thisWii->lastIR_Y[0] = IR_Y;				

				for (int i=4; i>0; i--)	{ thisWii->lastIR_X[i] = thisWii->lastIR_X[i-1]; }				
				thisWii->lastIR_X[0] = IR_X;	

				WiiX = IR_Y = IR_X = 0;
			}

			
			if ( (IR_X || IR_Y || WiiX) ) {
				
				if (IR_Y > 1)
					IR_Y = 1;
				else if (IR_Y < -1)
					IR_Y = -1;

				// esegue la rotazione
				WiiX = (WiiX*degreeFactor);
				IR_Y = -(IR_Y*degreeFactor);
				IR_X = (IR_X*degreeFactor);

				IR_X = _clockwise ? -IR_X : IR_X;
				IR_Y = _clockwise ? -IR_Y : IR_Y;

				WiiManager->camera->SetPosition(camPosition);
				WiiManager->camera->SetViewUp(camViewUp);

				if (enabled_ryf_Rotation && (abs(IR_Y) > 89)) {
					SetShiftKey( 0 );
					SetEventPosition( 0 , ( (IR_Y > 0) ? 89 : -89 ) * 100 );
					WiiManager->renderer->DrawOff();
					InvokeEvent(vtkWiiMotion, NULL);
					IR_Y = (IR_Y > 0) ? (IR_Y - 89) : (IR_Y + 89);
				}

				SetShiftKey( IR_X * 100 /*rxf*/ );
				SetEventPosition( WiiX * 100 /*angle*/ , IR_Y * 100 /*ryf*/ );
					
				WiiManager->renderer->DrawOn();
				
				InvokeEvent(vtkWiiMotion, NULL);
				this->updateLastRenderTime();
			}
		} // fine sceneRotating 1 DoF
		else if ( thisWii->Mode == CameraDollyIn && isRenderingNeeded(renderingRate)) 
		{
			//if (thisWii->timeoutEvent == 0)
			//{
				/*
				if (_3dTechnique == SurfaceRendering) {
					double gzf = ((wxSurfaceRenderingGui*)window)->getGlobalZoomFactor();
					if (gzf != 1) {
						((wxSurfaceRenderingGui*)window)->updateZoom(1/gzf, false);
						((wxSurfaceRenderingGui*)window)->setGlobalZoomFactor(1);
					}
				}
				else {
					double gzf = ((wxVolumeRenderingGui*)window)->getGlobalZoomFactor();
					if (gzf != 1) {
						((wxVolumeRenderingGui*)window)->updateZoom(1/gzf, false);
						((wxVolumeRenderingGui*)window)->setGlobalZoomFactor(1);
					}
				}
				*/

				//WiiManager->renderer->DrawOff();
				InvokeEvent(vtkWiiPlusDown, NULL);
				this->updateLastRenderTime();
				//WiiManager->renderer->DrawOn();

				if (_interactionType == wii || _interactionType == voi3dVRwii)
				{
					((wxVolumeRenderingGui*)window)->updateStereoView(false);
				}	
				else
					((wxSurfaceRenderingGui*)window)->updateStereoView(false);
		}		
		else if ( thisWii->Mode == CameraDollyOut && isRenderingNeeded(renderingRate)) 
		{
			//if (thisWii->timeoutEvent == 0)
			//{				
				/*
				if (_3dTechnique == SurfaceRendering) {
					double gzf = ((wxSurfaceRenderingGui*)window)->getGlobalZoomFactor();
					if (gzf != 1) {
						((wxSurfaceRenderingGui*)window)->updateZoom(1/gzf, false);
						((wxSurfaceRenderingGui*)window)->setGlobalZoomFactor(1);
					}
				}
				else {
					double gzf = ((wxVolumeRenderingGui*)window)->getGlobalZoomFactor();
					if (gzf != 1) {
						((wxVolumeRenderingGui*)window)->updateZoom(1/gzf, false);
						((wxVolumeRenderingGui*)window)->setGlobalZoomFactor(1);
					}
				}
				*/

				//WiiManager->renderer->DrawOff();
				InvokeEvent(vtkWiiMinusDown, NULL);
				this->updateLastRenderTime();
				//WiiManager->renderer->DrawOn();

				if ( _interactionType == wii || _interactionType == voi3dVRwii )
				{
					((wxVolumeRenderingGui*)window)->updateStereoView(false);
				}
				else
					((wxSurfaceRenderingGui*)window)->updateStereoView(false);
		}
		else if ( thisWii->Mode == SceneZoomingIn )
		{
			double factor = 1.0+1.0/200;

			if (_interactionType == wii || _interactionType == voi3dVRwii)
			{
				((wxVolumeRenderingGui*)window)->updateZoom(factor,false);
				((wxVolumeRenderingGui*)window)->updateStereoView(false);
			}	
			else
			{
				((wxSurfaceRenderingGui*)window)->updateZoom(factor,false);
				((wxSurfaceRenderingGui*)window)->updateStereoView(false);
			}
		}
		else if ( thisWii->Mode == SceneZoomingOut )
		{
			double factor = 1.0 - 1.0/200;

			if (_interactionType == wii || _interactionType == voi3dVRwii)
			{
				((wxVolumeRenderingGui*)window)->updateZoom(factor,false);
				((wxVolumeRenderingGui*)window)->updateStereoView(false);
			}	
			else
			{
				((wxSurfaceRenderingGui*)window)->updateZoom(factor,false);
				((wxSurfaceRenderingGui*)window)->updateStereoView(false);
			}
		}	
		else if ( thisWii->Mode == SceneRolling )
		{	
			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + " ms" +
										"\tEntrato in SceneRolling";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new

			#endif
			}
			
			
			float WiiX = event.WiiX;
			if (WiiX>1) {WiiX=1;}	else if (WiiX<-1) {WiiX=-1;}


			// filtro stile prism per il roll
			/* Nello			
			GetSystemTime(&this->st);
			int sTime = st.wSecond*1000+st.wMilliseconds;
			if ( abs(sTime-lastVelocityTime) >= interval*1000 ) {

				// calcola lo spostamento rispetto alla rilevazione precedente
				float dz = WiiX - zPositionVelocity;
				
				// aggiorna il valore precedenti
				zPositionVelocity = WiiX;

				// calcola il C-D ratio in funzione dello spostamento
				if (abs(dz) < minRotDISP)
					invCDRollRatio = 0;
				else if (abs(dz) < maxDISP)
					invCDRollRatio = (abs(dz) - minRotDISP) / (maxRotDISP - minRotDISP);
				else
					invCDRollRatio=1;
			
				// aggiorna il valore temporale dell'ultimo spostamento
				lastVelocityTime = sTime;
			}
			*/
			//prismStyle1DFilter(thisWii->lastVelocityTime, thisWii->interval, thisWii->invCDRollRatio, thisWii->zPositionVelocity, WiiX);
			prismStyle1DFilter(WiiX, thisWii->zPositionVelocity, thisWii->invCDRollRatio, thisWii);

			float dz = WiiX - thisWii->lastWiiX[0];
			WiiX = dz * thisWii->invCDRollRatio + thisWii->lastWiiX[0];
			//if (WiiX>1) {WiiX=1;}	else if (WiiX<-1) {WiiX=-1;}
			thisWii->lastWiiX[0] = WiiX;

			showRotationIndicator(3, WiiX+thisWii->WiiXo, thisWii->pointerColor);

			if (this->isRenderingNeeded(renderingRate)) {
				double degreeFactor = 90;

				WiiManager->camera->SetPosition(camPosition);
				WiiManager->camera->SetViewUp(camViewUp);

				SetShiftKey( 0 /*rxf*/ );
				SetEventPosition( (WiiX - thisWii->WiiXo) * 100 * degreeFactor /*angle*/ , 0 /*ryf*/ );

				{
				#ifdef LOG_ACTIVATION
					// log new
					if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
						char buffer[64];
						string r0, r1, r2, r3;
						r0 = _itoa(event.idWii, buffer, 10);
						r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
						r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
						r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
						string strDatanew = "idWii: " + r0 + 
											"\tsTime: " + r1 + " mcs" +
											"\tDelay(last instr): " + r2 + " mcs" + 
											"\tDelay(event): " + r3 + " ms" +
											"\tEseguo Render da SceneRolling";
						list.push_back(strDatanew);
						_lastUpdatedTime = _updatedTime;
					}
					// fine log new

				#endif
				}
				
				InvokeEvent(vtkWiiMotion, NULL);
				this->updateLastRenderTime();

				{
				#ifdef LOG_ACTIVATION
					// log new
					if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
						char buffer[64];
						string r0, r1, r2, r3;
						r0 = _itoa(event.idWii, buffer, 10);
						r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
						r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
						r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
						string strDatanew = "idWii: " + r0 + 
											"\tsTime: " + r1 + " mcs" +
											"\tDelay(last instr): " + r2 + " mcs" + 
											"\tDelay(event): " + r3 + " ms" +
											"\tRender eseguito da SceneRolling\n";
						list.push_back(strDatanew);
						_lastUpdatedTime = _updatedTime;
					}
					// fine log new
				#endif
				}
			}
		}	


		// aggiorna la posizione del puntatore
		if ( thisWii->Mode != SceneRotatingIR && thisWii->Mode != SceneRotating && thisWii->Mode != SceneRotatingIR_1 && thisWii->Mode != SceneRolling)
		{
			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + " ms" +
										"\tEntrato nella routine di Pointing";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new
			#endif
			}
			
			int* size;
			size = WiiManager->viewer3d->getRenderWindow()->GetSize();
	        
			bool isCDratioComputed = prismStyle2DFilter( event.IRLight1X, thisWii->xPositionVelocity, event.IRLight1Y, thisWii->yPositionVelocity,
								   thisWii->invCDratio, thisWii);

			if ( !isCDratioComputed && thisWii->invCDratio==0 && (event.IRLight1X - thisWii->lastIR_X[0]) > 1024*0.2 || (event.IRLight1Y - thisWii->lastIR_Y[0]) > 768*0.2 )
				thisWii->invCDratio = 0.5;
			
			{
			#ifdef LOG_ACTIVATION
				if (isCDratioComputed) {
					// log new
					if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
						char buffer[64];
						string r0, r1, r2, r3, r4;
						r0 = _itoa(event.idWii, buffer, 10);
						r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
						r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
						r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
						r4 = _ltoa(thisWii->invCDratio*1000, buffer, 10);
						string strDatanew = "idWii: " + r0 + 
											"\tsTime: " + r1 + " mcs" +
											"\tDelay(last instr): " + r2 + " mcs" + 
											"\tDelay(event): " + r3 + " ms" +
											"\tinvCDratio (x1000): " + r4;
						list.push_back(strDatanew);
						_lastUpdatedTime = _updatedTime;
					}
					// fine log new
				}
			#endif
			}

			// calcola la nuova posizione (xIR, yIR)
			float x = thisWii->lastIR_X[0] + thisWii->invCDratio * (event.IRLight1X - thisWii->lastIR_X[0]);
			float y = thisWii->lastIR_Y[0] + thisWii->invCDratio * (event.IRLight1Y - thisWii->lastIR_Y[0]);

			// aggiorna il valore precedente 
			thisWii->lastIR_X[0] = x;
			thisWii->lastIR_Y[0] = y;
		
			// adattamento alla risoluzione dello schermo da (xIR, yIR) a (x, y)
			x = ( size[0] * (1023 - x ) ) / 1023;
			y = ( size[1] * (767 - y ) ) / 767;

			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10); // microseconds
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + " ms" +
										"\tCalcolate x e y del puntatore";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new
			#endif
			}

			if (!isRenderingNeeded(renderingRate)) return;
			// inizio calcolo z del puntatore

			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + " ms" +
										"\tCalcolo della Z del puntatore";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new
			#endif
			}
			
			double focalDepth, pointerPosition[3];
			thisWii->pointer->GetPosition(pointerPosition);									          
			GetInteractorStyle()->ComputeWorldToDisplay( WiiManager->renderer, pointerPosition[0], 
														pointerPosition[1], pointerPosition[2], pointerPosition);
			focalDepth = pointerPosition[2];

			// Intersect with line
			double point[3];	
			if (_3dTechnique == SurfaceRendering) {
				thisWii->isOnSurface = ((wxSurfaceRenderingGui*)wxGetTopLevelParent(this))->intersectingModelSurface(x,y,point);
			}
			else if (_3dTechnique == VolumeRendering) {
				thisWii->isOnSurface = ((wxVolumeRenderingGui*)wxGetTopLevelParent(this))->intersectingModelSurface(x,y,point);
			}
			
			if ( thisWii->isOnSurface )
			{
				thisWii->pointer->SetPosition(point[0], point[1], point[2]);
				thisWii->pointer->GetProperty()->SetColor(1,1,0);
			}
			else
			{	                
				double focalDepth, focalPosition[3], newPosition[4];
				WiiManager->camera->GetFocalPoint(focalPosition);
				GetInteractorStyle()->ComputeWorldToDisplay( WiiManager->renderer, focalPosition[0], focalPosition[1], 
															focalPosition[2], focalPosition);				
				focalDepth = focalPosition[2];
				GetInteractorStyle()->ComputeDisplayToWorld( WiiManager->renderer, x, y, focalDepth, newPosition);

				thisWii->pointer->SetPosition( newPosition[0], newPosition[1], newPosition[2]);
				thisWii->pointer->GetProperty()->SetColor(1,1,1);
			}
	
			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + " ms" +
										"\tFine calcolo della Z del puntatore";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new
			#endif
			}
			
			// fine calcolo z del puntatore
		}	
		
		if ( idWiiEnabled == AnyWiiEnabled && isRenderingNeeded(renderingRate) )
		{			
			updateLastRenderTime();

			if (WiiManager->WiiPointing_num)
			{
				{
				#ifdef LOG_ACTIVATION
					// log new
					if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
						char buffer[64];
						string r0, r1, r2, r3;
						r0 = _itoa(event.idWii, buffer, 10);
						r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
						r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
						r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
						string strDatanew = "idWii: " + r0 + 
											"\tsTime: " + r1 + " mcs" +
											"\tDelay(last instr): " + r2 + " mcs" + 
											"\tDelay(event): " + r3 + " ms" +
											"\tEseguo il rendering da WiiManager->WiiPointing_num";
						list.push_back(strDatanew);
						_lastUpdatedTime = _updatedTime;
					}
					// fine log new
				#endif
				}
				
				WiiManager->viewer3d->updateViewer();				
			}
			// Se nessuno sta puntando ne interagendo, faccio il rendering se e soltanto se il modello non si trova già ad alta definizione
			else //if ( (_interactionType != voi3dVRwii) /*&& (this->GetRenderWindow()->GetDesiredUpdateRate() == this->GetDesiredUpdateRate())*/ )			
			{					
				{
				#ifdef LOG_ACTIVATION
					// log new
					if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
						char buffer[64];
						string r0, r1, r2, r3;
						r0 = _itoa(event.idWii, buffer, 10);
						r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
						r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
						r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
						string strDatanew = "idWii: " + r0 + 
											"\tsTime: " + r1 + " mcs" +
											"\tDelay(last instr): " + r2 + " mcs" + 
											"\tDelay(event): " + r3 + " ms" +
											"\tEseguo il rendering dall'else";
						list.push_back(strDatanew);
						_lastUpdatedTime = _updatedTime;
					}
					// fine log new
				#endif
				}
				
				WiiManager->viewer3d->updateViewer();
			}

			{
			#ifdef LOG_ACTIVATION
				// log new
				if (QueryPerformanceCounter((LARGE_INTEGER *)&_updatedTime)!= 0) {
					char buffer[64];
					string r0, r1, r2, r3;
					r0 = _itoa(event.idWii, buffer, 10);
					r1 = _i64toa(( (_updatedTime - _initialTime) * 1000000 / _clockFrequency), buffer, 10);
					r2 = _i64toa(( (_updatedTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
					r3 = _i64toa( (_updatedTime*1000/_clockFrequency - event.GetTimestamp()), buffer, 10); // milliseconds
					string strDatanew = "idWii: " + r0 + 
										"\tsTime: " + r1 + " mcs" +
										"\tDelay(last instr): " + r2 + " mcs" + 
										"\tDelay(event): " + r3 + " ms" +
										"\tRender eseguito\n";
					list.push_back(strDatanew);
					_lastUpdatedTime = _updatedTime;
				}
				// fine log new
			#endif
			}
		}		
	}
}


void appWxVtkInteractor::onWiiADown( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);		

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{			
		/*
		// avvia il riconoscitore di comandi vocali
		if ( (thisWii->Mode == Waiting) && (idWiiEnabled == AnyWiiEnabled) && ( _interactionType != wiiSR ) )
		{				
			if ( wxVocalDecoder::vocalDecoderEnabled && vocalDecoder->startRecognition() )
			{
				showInteractionSignal(thisWii->pointerColor,"V");

				if (!WiiManager->WiiPointing_num)
				{
					WiiManager->renderer->SetAllocatedRenderTime(0.1);
					WiiManager->viewer3d->updateViewer();							
					WiiManager->renderer->SetAllocatedRenderTime(1000);                    
				}

				// Operazioni da fare quando un Wii prende il controllo delle interazioni						
				WiiManager->setLed4(true,event.idWii);				
				//WiiManager->DisableTheOthersWii(event.idWii); 
				idWiiEnabled = event.idWii;
				thisWii->Mode = SceneVocalAcquiring;	
			}
		}
		*/

		if (idWiiEnabled == AnyWiiEnabled)
		{
			if ( thisWii->Mode == Pointing && !_isSceneRotatingIR_1)
			{
				if (!thisWii->isOnSurface) return;
				else thisWii->pointer->GetProperty()->SetColor(0,0,1);

				// Operazioni da fare quando un Wii prende il controllo delle interazioni						
				WiiManager->setLed4(true,event.idWii);				
				idWiiEnabled = event.idWii;

				thisWii->Mode = SceneRolling;				

				float WiiX = event.WiiX;
				// Il wiimote misura anche l'accellerazione delle rotazioni, dando valori esterni all'intervallo
				// [-1,1] in funzione dell'accellerazione impressa. Per evitare valori esterni a tale intervallo
				// imponiamo i seguenti limiti.
				if (WiiX>1) {WiiX=1;}	else if (WiiX<-1) {WiiX=-1;}		        
				// In questo modo il nuovo valore iniziale sarà pari all' inclinazione attuale del wii.
				thisWii->WiiXo = WiiX;	
				thisWii->lastWiiX[0] = WiiX;

				WiiManager->camera->GetPosition(camPosition);
				WiiManager->camera->GetViewUp(camViewUp);

				
				InvokeEvent(vtkWiiBDown, NULL);	

				/*
				double pointerPosition[3];
				thisWii->pointer->GetPosition(pointerPosition);

				this->SetNumberOfFlyFrames(50);
				this->FlyTo(WiiManager->renderer,pointerPosition);

				wxWindow* window = wxGetTopLevelParent(this);
				if(_interactionType == wiiSR) 
					((wxSurfaceRenderingGui*)window)->updateStereoView();
				else 
					((wxVolumeRenderingGui*)window)->updateStereoView();
					
				//WiiManager->camera->SetFocalPoint(pointerPosition);
				*/
			}
/*
			else if ( (thisWii->Mode == Waiting) && (_interactionType != wiiSR) )
			{					
				if ( _interactionType == voi3dVRwii )
				{
					if (!WiiManager->WiiPointing_num)	InvokeEvent(vtkWiiHomeDown,NULL);								
					updateLastRenderTime();
				}

				wxWindow* window = wxGetTopLevelParent(this);	
				((wxVolumeRenderingGui*)window)->onVOI( wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, 0) );
			}
*/
		}

	}
	
}


void appWxVtkInteractor::onWiiAUp( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	
	
	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{		
		if (idWiiEnabled == event.idWii)
		{
			if (thisWii->Mode==SceneRolling)
			{
				// Operazioni da fare quando un Wii rilascia il controllo delle interazioni			
				WiiManager->setLed4(false,event.idWii);				
				idWiiEnabled = AnyWiiEnabled;							
				thisWii->Mode = Pointing;
				hideRotationIndicator();

				InvokeEvent(vtkWiiBUp, NULL);	
			}
		}	
	}	
}

void appWxVtkInteractor::onWiiBDown( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{
		if ( (thisWii->Mode == Waiting) && (idWiiEnabled == AnyWiiEnabled) ) 
		{			
			WiiManager->setLed4(true,event.idWii);
			idWiiEnabled = event.idWii;
					
			float WiiX = event.WiiX, WiiY = event.WiiY;
			// Il wiimote misura anche l'accellerazione delle rotazioni, dando valori esterni all'intervallo
			// [-1,1] in funzione dell'accellerazione impressa. Per evitare valori esterni a tale intervallo
			// imponiamo i seguenti limiti.
			if (WiiX>1) {WiiX=1;}	else if (WiiX<-1) {WiiX=-1;}
			if (WiiY>1) {WiiY=1;}	else if (WiiY<-1) {WiiY=-1;}	
	        
			// In questo modo il nuovo valore iniziale sarà pari all' inclinazione attuale del wii.
			thisWii->WiiXo = WiiX;	
			thisWii->WiiYo = WiiY;

			// Cambio il valore iniziale dell'ascissa della posizione puntata dagli infrarossi.
			int x;
			int* size;
			size = WiiManager->viewer3d->getRenderWindow()->GetSize();
			x = ( size[0] * (1023 - event.IRLight1X ) ) / 1023;				            
			float rangex = size[0]/2;				
			thisWii->IR_Xo = (x-rangex)/rangex;				
	        
			SetEventPosition( WiiX * 1000, WiiY * 1000);
			InvokeEvent(vtkWiiBDown, NULL);				
			WiiManager->camera->GetPosition(camPosition);
			WiiManager->camera->GetViewUp(camViewUp);

			thisWii->lastWiiX[0] = thisWii->lastWiiX[1] = event.WiiX;
			thisWii->lastWiiY[0] = thisWii->lastWiiY[1] = event.WiiY;

			thisWii->Mode = SceneRotating;
		}
		else if (thisWii->Mode == Pointing && !_isSceneRotatingIR_1)
		{
			// LUIGI - nuova metafora di rotazione

			// controllo diritti di accesso
			if ( idWiiEnabled == AnyWiiEnabled )
			{			

				if (!thisWii->isOnSurface) return;
				else thisWii->pointer->GetProperty()->SetColor(0,0,1);

				WiiManager->setLed4(true,event.idWii);
				idWiiEnabled = event.idWii;
				
				InvokeEvent(vtkWiiBDown, NULL);		
				//WiiManager->camera->GetPosition(camPosition);
				//WiiManager->camera->GetViewUp(camViewUp);

				float x, y;
				int* size;
				size = this->GetRenderWindow()->GetSize();
	                
				x = ( size[0] * (1023 - event.IRLight1X ) ) / 1023;
				y = ( size[1] * (767 - event.IRLight1Y ) ) / 767;						
				thisWii->lastIR_X[0] = thisWii->lastIR_X[1] = x;
				thisWii->lastIR_Y[0] = thisWii->lastIR_Y[1] = y;

				// aggiorno i dati per il filtro PRISM
				thisWii->xPositionVelocity = event.IRLight1X;
				thisWii->yPositionVelocity = event.IRLight1Y;

				thisWii->Mode = SceneRotatingIR;

				// impostazione del clockwise
				double* versor = WiiManager->camera->GetDirectionOfProjection();
				double focalPosition[3], pointerPosition[3];;
				WiiManager->camera->GetFocalPoint(focalPosition);				
				thisWii->pointer->GetPosition(pointerPosition);	
				double pointerProjection, focalProjection;
				pointerProjection = versor[0] * pointerPosition[0] + versor[1] * pointerPosition[1] + versor[2] * pointerPosition[2];
				focalProjection = versor[0] * focalPosition[0] + versor[1] * focalPosition[1] + versor[2] * focalPosition[2];

				if (pointerProjection>focalProjection) 
					_clockwise=true;
				else 
					_clockwise=false;
			}
		}
		else if (thisWii->Mode == Pointing && _isSceneRotatingIR_1)
		{

			if (idWiiEnabled == AnyWiiEnabled)
			{				
				if (!thisWii->isOnSurface) return;
				else thisWii->pointer->GetProperty()->SetColor(0,0,1);

				WiiManager->setLed4(true,event.idWii);
				idWiiEnabled = event.idWii;
				
				InvokeEvent(vtkWiiBDown, NULL);		

				// Calcolo la posizione puntata dagli infrarossi.
				float IR_X = 0, IR_Y = 0;
				
				int x,y;
				int* size;
				size = WiiManager->viewer3d->getRenderWindow()->GetSize();
				x = ( size[0] * (1023 - event.IRLight1X ) ) / 1023;				
				y = ( size[1] * ( 767 - event.IRLight1Y ) ) /  767;				
	                			
				float rangex = size[0]/2;				
				IR_X = (x-rangex)/rangex;				

				float rangey = size[1]/2;				
				IR_Y = (y-rangey)/rangey;				

				thisWii->IR_Xo = IR_X;
				thisWii->IR_Yo = IR_Y;
				thisWii->lastIR_X[0] = thisWii->lastIR_X[1] = IR_X;
				thisWii->lastIR_Y[0] = thisWii->lastIR_Y[1] = IR_Y;

				float WiiX = event.WiiX;
				// Il wiimote misura anche l'accellerazione delle rotazioni, dando valori esterni all'intervallo
				// [-1,1] in funzione dell'accellerazione impressa. Per evitare valori esterni a tale intervallo
				// imponiamo i seguenti limiti.
				if (WiiX>1) {WiiX=1;}	else if (WiiX<-1) {WiiX=-1;}		        
				// In questo modo il nuovo valore iniziale sarà pari all' inclinazione attuale del wii.
				thisWii->WiiXo = WiiX;	
				thisWii->lastWiiX[0] = thisWii->lastWiiX[1] = event.WiiX;

				WiiManager->camera->GetPosition(camPosition);
				WiiManager->camera->GetViewUp(camViewUp);

				thisWii->Mode = SceneRotatingIR_1;

				// impostazione del clockwise
				double* versor = WiiManager->camera->GetDirectionOfProjection();
				double focalPosition[3], pointerPosition[3];;
				WiiManager->camera->GetFocalPoint(focalPosition);				
				thisWii->pointer->GetPosition(pointerPosition);	
				double pointerProjection, focalProjection;
				pointerProjection = versor[0] * pointerPosition[0] + versor[1] * pointerPosition[1] + versor[2] * pointerPosition[2];
				focalProjection = versor[0] * focalPosition[0] + versor[1] * focalPosition[1] + versor[2] * focalPosition[2];

				if (pointerProjection>focalProjection) 
					_clockwise=true;
				else 
					_clockwise=false;

				WiiManager->viewer3d->updateViewer();
			}
		}
	}

		
}

void appWxVtkInteractor::onWiiBUp( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{	

		if (thisWii->Mode == SceneRotating) 
		{				
			thisWii->WiiXo = 0;	
			thisWii->WiiYo = 0;
			thisWii->IR_Xo = 0;
			//thisWii->driver->lastIRLight1X = thisWii->driver->lastIRLight2X = 511;
			//thisWii->driver->lastIRLight1Y = thisWii->driver->lastIRLight2Y = 383;

			for (int i=0; i<5; i++)	{ thisWii->lastWiiX[i] = 0; }						
			for (int i=0; i<5; i++)	{ thisWii->lastWiiY[i] = 0; }				
			for (int i=0; i<5; i++)	{ thisWii->lastIR_X[i] = 0; }			
			enabled_angle_Rotation = enabled_ryf_Rotation = enabled_rxf_Rotation = false;
			hideRotationIndicator();
			
			//Al termine di un'iterazione aggiorno i valori di camPosition e camViewUp
			if ( idWiiEnabled == event.idWii ) {
					WiiManager->camera->GetPosition(camPosition);
					WiiManager->camera->GetViewUp(camViewUp);
			}			

			// Se c'è almeno un wiimote in modalità Pointing, evito di fare il rendering ad alto livello.
			if (WiiManager->WiiPointing_num)	
			{ 
				SetControlKey(0);	// StopInteractionWithoutRendering
			}
			else 
			{
				SetControlKey(1);	// StopInteractionDoingARender				
			}
			InvokeEvent(vtkWiiBUp, NULL);					
			updateLastRenderTime();
					
			// Operazioni da fare quando un Wii rilascia il controllo delle interazioni	
			WiiManager->setLed4(false,event.idWii);				
			idWiiEnabled = AnyWiiEnabled;		
			//thisWii->Mode = Waiting;
			thisWii->Mode = Pointing;
		}
		else if (thisWii->Mode == SceneRotatingIR)
		{
			// Se c'è almeno un wiimote in modalità Pointing, evito di fare il rendering ad alto livello.
			if (WiiManager->WiiPointing_num)	
			{ 
				SetControlKey(0);	// StopInteractionWithoutRendering
			}
			else 
			{
				SetControlKey(1);	// StopInteractionDoingARender				
			}
			InvokeEvent(vtkWiiBUp, NULL);			
			updateLastRenderTime();

			WiiManager->setLed4(false,event.idWii);				
			idWiiEnabled = AnyWiiEnabled;	
			thisWii->Mode = Pointing;
		}

		else if (thisWii->Mode == SceneRotatingIR_1) 
		{				
			thisWii->WiiXo = 0;	
			thisWii->WiiYo = 0;
			thisWii->IR_Xo = 0;
			//thisWii->driver->lastIRLight1X = thisWii->driver->lastIRLight2X = 511;
			//thisWii->driver->lastIRLight1Y = thisWii->driver->lastIRLight2Y = 383;

			for (int i=0; i<5; i++)	{ thisWii->lastWiiX[i] = 0; }						
			for (int i=0; i<5; i++)	{ thisWii->lastWiiY[i] = 0; }				
			for (int i=0; i<5; i++)	{ thisWii->lastIR_X[i] = 0; }			
			enabled_angle_Rotation = enabled_ryf_Rotation = enabled_rxf_Rotation = false;
			hideRotationIndicator();
			
			//Al termine di un'iterazione aggiorno i valori di camPosition e camViewUp
			if ( idWiiEnabled == event.idWii ) {
					WiiManager->camera->GetPosition(camPosition);
					WiiManager->camera->GetViewUp(camViewUp);
			}			

			// Se c'è almeno un wiimote in modalità Pointing, evito di fare il rendering ad alto livello.
			if (WiiManager->WiiPointing_num)	
			{ 
				SetControlKey(0);	// StopInteractionWithoutRendering
			}
			else 
			{
				SetControlKey(1);	// StopInteractionDoingARender				
			}
			InvokeEvent(vtkWiiBUp, NULL);					
			updateLastRenderTime();
					
			// Operazioni da fare quando un Wii rilascia il controllo delle interazioni	
			WiiManager->setLed4(false,event.idWii);				
			idWiiEnabled = AnyWiiEnabled;		
			thisWii->Mode = Pointing;
		}
		
		// aggiorna le informazioni per il calcolo della posizione del puntatore
		thisWii->lastIR_X[0] = event.IRLight1X;
		thisWii->lastIR_Y[0] = event.IRLight1Y;
	}		
}

void appWxVtkInteractor::onWiiMinusDown( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{	
		if ( /*(thisWii->Mode == Waiting) &&*/ (idWiiEnabled == AnyWiiEnabled) )
		{			
			if ( _interactionType == voi3dVRwii )
			{
				WiiManager->setLed4(true,event.idWii);			
				//this->GetRenderWindow()->SetDesiredUpdateRate(this->GetDesiredUpdateRate());		            
				idWiiEnabled = event.idWii;
				thisWii->Mode = BoxWidgetSelecting;
				InvokeEvent(vtkWiiMinusDown, NULL);
			}
			else
			{
				WiiManager->setLed4(true,event.idWii);			
				idWiiEnabled = event.idWii;
				thisWii->Mode = CameraDollyOut;
			}
		}
	}

	
}

void appWxVtkInteractor::onWiiMinusUp( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{			
		if (thisWii->Mode == CameraDollyOut)
		{			
			//Al termine di un'iterazione aggiorno i valori di camPosition e camViewUp
			WiiManager->camera->GetPosition(camPosition);
			WiiManager->camera->GetViewUp(camViewUp);
			
			// Se c'è almeno un wiimote in modalità Pointing, evito di fare il rendering ad alto livello.
			if (WiiManager->WiiPointing_num)	
			{ 
				SetControlKey(0);	// StopInteractionWithoutRendering
			}
			else 
			{
				SetControlKey(1);	// StopInteractionDoingARender				
			}
			InvokeEvent(vtkWiiMinusUp, NULL);					
			updateLastRenderTime();

			// Operazioni da fare quando un Wii rilascia il controllo delle interazioni
			WiiManager->setLed4(false,event.idWii);
			idWiiEnabled = AnyWiiEnabled;
			thisWii->Mode = thisWii->driver->Led2 ? Pointing : Waiting;
		}
		else if ( thisWii->Mode == BoxWidgetSelecting )
		{
			WiiManager->setLed4(false,event.idWii);			
			idWiiEnabled = AnyWiiEnabled;
			thisWii->Mode = Pointing;
			InvokeEvent(vtkWiiMinusUp, NULL);	
		}
	}

		
}

void appWxVtkInteractor::onWiiHomeDown( wxWiimoteEvent & event) 
{	
	{
	#ifdef LOG_ACTIVATION
		// scrittura log su file
		// per il log - tipo file di log
		ofstream wiilog;
		wiilog.open("appwxWiilog.txt", ios::app);
		for(int i=0;i < list.size(); i++)
		{
			std::string strd = list.at(i);
			wiilog << strd.c_str()<<endl;            			
		}
		// fine scrittura log su file
		wiilog.close();
	#endif
	}
	
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) {				
		if ( _interactionType != wiiSR ) {					
			wxWindow* window = wxGetTopLevelParent(this);	
			((wxVolumeRenderingGui*)window)->onVOI( wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, 0) );
		}
	}
}

void appWxVtkInteractor::onWiiHomeUp( wxWiimoteEvent & event) 
{
}

void appWxVtkInteractor::onWiiPlusDown( wxWiimoteEvent & event) 
{	
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{	
		if ( /*(thisWii->Mode == Waiting) &&*/ (idWiiEnabled == AnyWiiEnabled) )
		{			
			if ( _interactionType == voi3dVRwii )
			{
				WiiManager->setLed4(true,event.idWii);				            
				idWiiEnabled = event.idWii;
				thisWii->Mode = BoxWidgetSelecting;
				InvokeEvent(vtkWiiPlusDown, NULL);
			}
			else
			{
				WiiManager->setLed4(true,event.idWii);					            
				idWiiEnabled = event.idWii;
				thisWii->Mode = CameraDollyIn;
			}
		}
	}
}

void appWxVtkInteractor::onWiiPlusUp( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{	
		if (thisWii->Mode == CameraDollyIn)
		{				
			//Al termine di un'iterazione aggiorno i valori di camPosition e camViewUp            
			WiiManager->camera->GetPosition(camPosition);
			WiiManager->camera->GetViewUp(camViewUp);
	
			// Se c'è almeno un wiimote in modalità Pointing, evito di fare il rendering ad alto livello.
			if (WiiManager->WiiPointing_num)	
			{ 
				SetControlKey(0);	// StopInteractionWithoutRendering
			}
			else 
			{
				SetControlKey(1);	// StopInteractionDoingARender			
			}
			InvokeEvent(vtkWiiPlusUp, NULL);					
			updateLastRenderTime();
            
			// Operazioni da fare quando un Wii rilascia il controllo delle interazioni			
			thisWii->Mode = thisWii->driver->Led2 ? Pointing : Waiting;

			WiiManager->setLed4(false,event.idWii);			
			idWiiEnabled = AnyWiiEnabled;
		}		
		else if ( thisWii->Mode == BoxWidgetSelecting )
		{
			WiiManager->setLed4(false,event.idWii);			
			idWiiEnabled = AnyWiiEnabled;
			thisWii->Mode = Pointing;
			InvokeEvent(vtkWiiPlusUp, NULL);	
		}
	}

}

void appWxVtkInteractor::onWiiOneDown( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{   
		if ( idWiiEnabled == AnyWiiEnabled )
		//if ( (thisWii->Mode == Waiting) && (idWiiEnabled == AnyWiiEnabled) )
		{   	
			WiiManager->setLed4(true,event.idWii);	
			//idWiiEnabled = event.idWii;				
			thisWii->Mode = SceneZoomingOut;
			this->GetRenderWindow()->SetDesiredUpdateRate(this->GetDesiredUpdateRate());
		}
	}	

		
}

void appWxVtkInteractor::onWiiOneUp( wxWiimoteEvent & event) 
{	
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{
		if (thisWii->Mode == SceneZoomingOut)
		{			
			WiiManager->setLed4(false,event.idWii);		
			thisWii->Mode = thisWii->driver->Led2 ? Pointing : Waiting;
			this->GetRenderWindow()->SetDesiredUpdateRate(this->GetDesiredUpdateRate());
		}
	}	

	
}

void appWxVtkInteractor::onWiiTwoDown( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{		
		if ( idWiiEnabled == AnyWiiEnabled )
		//if ( (thisWii->Mode == Waiting) && (idWiiEnabled == AnyWiiEnabled) )
		{   	
			WiiManager->setLed4(true,event.idWii);	
			//idWiiEnabled = event.idWii;				
			thisWii->Mode = SceneZoomingIn;
			this->GetRenderWindow()->SetDesiredUpdateRate(this->GetDesiredUpdateRate());
		}
	}

		
}

void appWxVtkInteractor::onWiiTwoUp( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura quando 
	// non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{
		if (thisWii->Mode == SceneZoomingIn)
		{
			WiiManager->setLed4(false,event.idWii);
			//idWiiEnabled = AnyWiiEnabled;				
			thisWii->Mode = thisWii->driver->Led2 ? Pointing : Waiting;
			this->GetRenderWindow()->SetDesiredUpdateRate(this->GetDesiredUpdateRate());
		}
	}

		
}

void appWxVtkInteractor::onWiiCrossDown( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{	
		if ( thisWii->Mode==Waiting || thisWii->Mode==Pointing)
		{			
			if ( _interactionType == voi3dVRwii)   
			{
				if (idWiiEnabled==AnyWiiEnabled)
				{
					WiiManager->setLed4(true,event.idWii);			
					        
					idWiiEnabled = event.idWii;
					thisWii->Mode = BoxWidgetTranslating;
				}
			}
			else //if (idWiiEnabled==AnyWiiEnabled)
			{
            				
				WiiManager->setLed4(true,event.idWii);								

				idWiiEnabled = event.idWii;						
				thisWii->Mode = SceneTranslating;
			}
		}
		else if ( thisWii->Mode == BoxWidgetTranslating ) 	
		{
			if (event.UpCross)
			{
				if (event.LeftCross) { SetControlKey(8); }		// LeftUp Direction
				else 
					if (event.RightCross) { SetControlKey(2); }	// RightUp Direction
					else SetControlKey(1);	// Up Direction
			}
			else
				if (event.DownCross)
				{
					if (event.LeftCross) { SetControlKey(6); }		// LeftDown Direction
					else 
						if (event.RightCross) { SetControlKey(4); }	// RightDown Direction
						else SetControlKey(5);	// Down Direction
				}
				else 
					if (event.RightCross) { SetControlKey(3); }		// Right Direction
					else { SetControlKey(7); }		// Left Direction

			double translationFactor = 1;
			SetShiftKey( translationFactor*1000 );	// translation factor
					
			InvokeEvent(vtkWiiCrossDown, NULL);	
			// esegue il cropping
			wxWindow* window = wxGetTopLevelParent(this);	
			((wxVolumeRenderingGui*)window)->updateClippingPlanes();
			((wxVolumeRenderingGui*)window)->updateVolume();            				
		}
		else if ( thisWii->Mode == SceneTranslating )
		{
			if (event.UpCross)
			{
				if (event.LeftCross) { SetControlKey(8); }		// LeftUp Direction
				else 
					if (event.RightCross) { SetControlKey(2); }	// RightUp Direction
					else SetControlKey(1);	// Up Direction
			}
			else
				if (event.DownCross)
				{
					if (event.LeftCross) { SetControlKey(6); }		// LeftDown Direction
					else 
						if (event.RightCross) { SetControlKey(4); }	// RightDown Direction
						else SetControlKey(5);	// Down Direction
				}
				else 
					if (event.RightCross) { SetControlKey(3); }		// Right Direction
					else { SetControlKey(7); }		// Left Direction

			InvokeEvent(vtkWiiCrossDown, NULL);	
		}	
	}

}

void appWxVtkInteractor::onWiiCrossUp( wxWiimoteEvent & event) 
{
	wxWiiManager::Wiimote* thisWii = WiiManager->findWii(event.idWii);	

	// Questo test evita che vengano processati eventi residui generati in fase di chiusura 
	// quando non ancora tutti i thread hanno dato seguita alla richiesta di chiusura.
	if (WiiManagerStarted) 
	{	               
		if ( thisWii->Mode == SceneTranslating )
		{
			//thisWii->timeoutEvent=0; thisWii->latencyEvent=maxLatency;
						
			//Al termine di un'iterazione aggiorno i valori di camPosition e camViewUp
			WiiManager->camera->GetPosition(camPosition);
			WiiManager->camera->GetViewUp(camViewUp);
			
			// Se c'è almeno un wiimote in modalità Pointing, evito di fare il rendering ad alto livello.
			if (WiiManager->WiiPointing_num)	
			{ 
				SetControlKey(0);	// StopInteractionWithoutRendering
			}
			else 
			{
				SetControlKey(1);	// StopInteractionDoingARender				
			}
			InvokeEvent(vtkWiiCrossUp, NULL);
			
			updateLastRenderTime();
            
			// Operazioni da fare quando un Wii rilascia il controllo delle interazioni				
			WiiManager->setLed4(false,event.idWii);		
			idWiiEnabled = AnyWiiEnabled;
			//thisWii->Mode=Waiting;
			thisWii->Mode=Pointing;
		}			
		else if ( thisWii->Mode == BoxWidgetTranslating )
		{
			//thisWii->timeoutEvent=0; thisWii->latencyEvent=maxLatency;

			WiiManager->setLed4(false,event.idWii);			
			            
			idWiiEnabled = AnyWiiEnabled;
			thisWii->Mode = Pointing;

			InvokeEvent(vtkWiiCrossUp, NULL);
		}
	}
		
}



//------------------------------------------------------------------------------------------------------------------	
void appWxVtkInteractor::OnSize(wxSizeEvent &event) {
	wxVTKRenderWindowInteractor::OnSize(event);
}

