/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkInteractorStyleWIITrackball.cxx,v $

  Author:	 Luigi Gallo (ICAR-CNR)

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInteractorStyleWIITrackball.h"
//#include "conio.h"
//#include <string>
#include "vtkCamera.h"
#include "vtkCallbackCommand.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "vtkWindows.h"
#include "WiiCommandEvents.h"

vtkCxxRevisionMacro(vtkInteractorStyleWIITrackball, "$Revision: 1.10 $");
vtkStandardNewMacro(vtkInteractorStyleWIITrackball);

//----------------------------------------------------------------------------
vtkInteractorStyleWIITrackball::vtkInteractorStyleWIITrackball() 
{
  this->MotionFactor   = 10.0;

  
  this->EventCallbackCommand->SetCallback(vtkInteractorStyleWIITrackball::ProcessEvents);
}

//----------------------------------------------------------------------------
vtkInteractorStyleWIITrackball::~vtkInteractorStyleWIITrackball() 
{

}

void vtkInteractorStyleWIITrackball::setupEventObservers(vtkRenderWindowInteractor* i) 
{
	if (!i)
		return;

	//Wiimote_Motion_Event observer
	i->AddObserver(vtkWiiMotion, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_A_Down_Event observer
	i->AddObserver(vtkWiiADown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_A_Up_Event observer
	i->AddObserver(vtkWiiAUp, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_B_Down_Event observer
	i->AddObserver(vtkWiiBDown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_B_Up_Event observer
	i->AddObserver(vtkWiiBUp, 
                   this->EventCallbackCommand, 
                   this->Priority);

	
	//Wiimote_Minus_Down_Event observer
	i->AddObserver(vtkWiiMinusDown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_Minus_Up_Event observer
	i->AddObserver(vtkWiiMinusUp, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_Home_Down_Event observer
	i->AddObserver(vtkWiiHomeDown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_Home_Up_Event observer
	i->AddObserver(vtkWiiHomeUp, 
                   this->EventCallbackCommand, 
                   this->Priority);
    
	//Wiimote_Plus_Down_Event observer
	i->AddObserver(vtkWiiPlusDown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_Plus_Up_Event observer
	i->AddObserver(vtkWiiPlusUp, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_One_Down_Event observer
	i->AddObserver(vtkWiiOneDown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_One_Up_Event observer
	i->AddObserver(vtkWiiOneUp, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_Two_Down_Event observer
	i->AddObserver(vtkWiiTwoDown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_Two_Up_Event observer
	i->AddObserver(vtkWiiTwoUp, 
                   this->EventCallbackCommand, 
                   this->Priority);	 

	//Wiimote_Cross_Down_Event observer
	i->AddObserver(vtkWiiCrossDown, 
                   this->EventCallbackCommand, 
                   this->Priority);

	//Wiimote_Cross_Up_Event observer
	i->AddObserver(vtkWiiCrossUp, 
                   this->EventCallbackCommand, 
                   this->Priority);	 
	
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::onWiiMotion() 
{   

	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	switch (this->State)
	{
		case VTKIS_ROTATE:
				this->FindPokedRenderer(x, y);
				this->Rotate();
				this->InvokeEvent(vtkCommand::InteractionEvent, NULL);									
				break;
		
		case VTKIS_SPIN:
				this->FindPokedRenderer(x, y);
				this->Spin();
				this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
				break;
	}
}


void vtkInteractorStyleWIITrackball::onWiiADown()
{
	this->FindPokedRenderer( this->Interactor->GetEventPosition()[0], 
							 this->Interactor->GetEventPosition()[1] );
  
	if (this->CurrentRenderer == NULL)
    {
		return;
	}

	this->GrabFocus(this->EventCallbackCommand);
	this->StartSpin();
	

}

void vtkInteractorStyleWIITrackball::onWiiAUp() 
{
	if (this->State == VTKIS_SPIN) 
	{
		this->EndSpin();
    }
	if ( this->Interactor )
    {
		this->ReleaseFocus();
    }		
};

void vtkInteractorStyleWIITrackball::onWiiBDown() 
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);
	if (this->CurrentRenderer == NULL)
	{
		return;
	}
	this->GrabFocus(this->EventCallbackCommand);
	this->StartRotate();	
};

void vtkInteractorStyleWIITrackball::onWiiBUp() 
{
	if (this->State == VTKIS_ROTATE) 
	{		
		int stopStateDoingARender = this->Interactor->GetControlKey();		
		if (stopStateDoingARender) 
		{
			this->EndRotate();		
		}
		else 
		{
			this->State = VTKIS_NONE;
			if (this->AnimState == VTKIS_ANIM_OFF) 
			{   
				vtkRenderWindowInteractor *rwi = this->Interactor;
				vtkRenderWindow *renwin = rwi->GetRenderWindow();
				renwin->SetDesiredUpdateRate(rwi->GetStillUpdateRate());
				if (this->UseTimers && !rwi->DestroyTimer(this->TimerId)) 
				{
					vtkErrorMacro(<< "Timer stop failed");
				}
				this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);				
			}   
		}
    }
	if ( this->Interactor )
    {
		this->ReleaseFocus();
    }	
};

void vtkInteractorStyleWIITrackball::onWiiMinusDown() {

	if (this->CurrentRenderer == NULL)
	{
		this->FindPokedRenderer(0,0);
	}

	this->GrabFocus(this->EventCallbackCommand);
	this->StartDolly();
	this->Dolly(pow((double)1.01, -MotionFactor/10));
	
};

void vtkInteractorStyleWIITrackball::onWiiMinusUp() 
{
	if (this->State ==  VTKIS_DOLLY )
	{        
		int stopStateDoingARender = this->Interactor->GetControlKey();		
		if (stopStateDoingARender) 
		{
			this->EndDolly();
		}
		else 
		{
			this->State = VTKIS_NONE;
			if (this->AnimState == VTKIS_ANIM_OFF) 
			{   
				vtkRenderWindowInteractor *rwi = this->Interactor;
				vtkRenderWindow *renwin = rwi->GetRenderWindow();
				renwin->SetDesiredUpdateRate(rwi->GetStillUpdateRate());
				if (this->UseTimers && !rwi->DestroyTimer(this->TimerId)) 
				{
					vtkErrorMacro(<< "Timer stop failed");
				}
				this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);				
			}   
		}
	}
		
	if (this->Interactor)
	{
		this->ReleaseFocus();
	}	
};

void vtkInteractorStyleWIITrackball::onWiiHomeDown() {
};

void vtkInteractorStyleWIITrackball::onWiiHomeUp() {
};

void vtkInteractorStyleWIITrackball::onWiiPlusDown() 
{
	if (this->CurrentRenderer == NULL)
	{
		this->FindPokedRenderer(0,0);
	}

	this->GrabFocus(this->EventCallbackCommand);
	this->StartDolly();
	this->Dolly(pow((double)1.01, MotionFactor/10));
	
};

void vtkInteractorStyleWIITrackball::onWiiPlusUp() 
{
	if (this->State ==  VTKIS_DOLLY )
	{
		int stopStateDoingARender = this->Interactor->GetControlKey();		
		if (stopStateDoingARender) 
		{
			this->EndDolly();
		}
		else 
		{
			this->State = VTKIS_NONE;
			if (this->AnimState == VTKIS_ANIM_OFF) 
			{   
				vtkRenderWindowInteractor *rwi = this->Interactor;
				vtkRenderWindow *renwin = rwi->GetRenderWindow();
				renwin->SetDesiredUpdateRate(rwi->GetStillUpdateRate());
				if (this->UseTimers && !rwi->DestroyTimer(this->TimerId)) 
				{
					vtkErrorMacro(<< "Timer stop failed");
				}
				this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);				
			}   
		}
	}
		
	if (this->Interactor)
	{
		this->ReleaseFocus();
	}	
};

void vtkInteractorStyleWIITrackball::onWiiOneDown() {	
};

void vtkInteractorStyleWIITrackball::onWiiOneUp() {
};

void vtkInteractorStyleWIITrackball::onWiiTwoDown() {
};

void vtkInteractorStyleWIITrackball::onWiiTwoUp() {
};

void vtkInteractorStyleWIITrackball::onWiiCrossDown() 
{

	if (this->CurrentRenderer == NULL)
	{
		this->FindPokedRenderer(0,0);
	}

	if (this->State != VTKIS_PAN)
		{
			this->GrabFocus(this->EventCallbackCommand);
			this->StartPan();
		}

	Pan();
};

void vtkInteractorStyleWIITrackball::onWiiCrossUp() 
{
	if (this->State = VTKIS_PAN) 
	{        
		int stopStateDoingARender = this->Interactor->GetControlKey();		
		if (stopStateDoingARender) 
		{
			this->EndPan();
		}
		else 
		{
			this->State = VTKIS_NONE;
			if (this->AnimState == VTKIS_ANIM_OFF) 
			{   
				vtkRenderWindowInteractor *rwi = this->Interactor;
				vtkRenderWindow *renwin = rwi->GetRenderWindow();
				renwin->SetDesiredUpdateRate(rwi->GetStillUpdateRate());
				if (this->UseTimers && !rwi->DestroyTimer(this->TimerId)) 
				{
					vtkErrorMacro(<< "Timer stop failed");
				}
				this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);				
			}   
		}

	}
	
	if (this->Interactor)
	{
		this->ReleaseFocus();
	}	
};
//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::Rotate()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  
  vtkRenderWindowInteractor *rwi = this->Interactor;

  
/*  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  
  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  // by Nello 
  // Se rilevo una variazione maggiore della metà dello schermo la ignoro
  if (abs(dx)>(size[0]/2.0)) {
	  dx=0;
	  dy=0;
  }
  else if (abs(dy)>(size[1]/2.0)) {
	  dx=0;
	  dy=0;
  }

  // changes the spin if the clockwise is ON
  bool clockwise = (rwi->GetShiftKey() == 666);
  dx = clockwise ? -dx : dx;
  dy = clockwise ? -dy : dy;  
*/
  int dx = rwi->GetEventPosition()[0];
  int dy = rwi->GetEventPosition()[1];
  int dz = rwi->GetShiftKey();

  // con questa modifica il mouse non raggiunge mai i bordi
  //rwi->SetLastEventPosition(size[0]/2,size[1]/2);
  //rwi->SetEventPosition(size[0]/2+dx,size[1]/2+dy);

  //this->GetCurrentRenderer()->GetRenderWindow()->SetCursorPosition(size[0]/2+dx,size[1]/2+dy);

  //double delta_elevation = -20.0 / size[1];
  //double delta_azimuth = -20.0 / size[0];
  
  //double rxf = (double)dx * delta_azimuth * this->MotionFactor;
  //double ryf = (double)dy * delta_elevation * this->MotionFactor;

  
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();  
  
  //double factor = this->MotionFactor * camera->GetDistance() / 300;  
  //double rxf		= (double)(-dz * factor / 2500 );
  //double ryf		= (double)( dy * factor / 5000 );
  //double angle	= (double)(-dx * factor / 5000 );  
  
  //double rxf	= (double)(-dz * MotionFactor/1000 );
  //double ryf	= (double)( dy * MotionFactor/1000 );
  //double angle	= (double)(-dx * MotionFactor/1000 );  

  double rxf	= -dz / 100.0;
  double ryf	=  dy / 100.0;
  double angle	= -dx / 100.0;  
  
  //if ( abs(rxf) > 1 )   camera->Azimuth(rxf);
  //if ( abs(ryf) > 1 )   camera->Elevation(ryf);
  //if ( abs(angle) > 1 ) camera->Roll(angle);   
  camera->Azimuth(rxf);
  camera->Elevation(ryf);
  camera->Roll(angle);   

  camera->OrthogonalizeViewUp();

  if (this->AutoAdjustCameraClippingRange)
    {
    this->CurrentRenderer->ResetCameraClippingRange();
    }

  if (rwi->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  rwi->Render();
  MotionFactor = 10 + 100*this->CurrentRenderer->GetLastRenderTimeInSeconds();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::Spin()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkRenderWindowInteractor *rwi = this->Interactor;

  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  //double angle = (double)((rwi->GetLastEventPosition()[0] - rwi->GetEventPosition()[0]) * MotionFactor*10 / size[0]);
  double angle = (double)( -( rwi->GetEventPosition()[0] * MotionFactor / size[0]) );

  // con questa modifica il mouse non raggiunge mai i bordi
  /*
  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  rwi->SetLastEventPosition(size[0]/2,size[1]/2);
  rwi->SetEventPosition(size[0]/2+dx,size[1]/2+dy);
  this->GetCurrentRenderer()->GetRenderWindow()->SetCursorPosition(size[0]/2+dx,size[1]/2+dy);
  */

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Roll(angle);
  camera->OrthogonalizeViewUp();
      
  rwi->Render();
}


//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::Pan()
{
	if (CurrentRenderer == NULL)
	{
		return;
	}
    
	vtkRenderWindowInteractor *rwi = Interactor;
    
	double viewFocus[4], focalDepth, viewPoint[3];
	double newPickPoint[4], oldPickPoint[4], motionVector[3];
    
	// Calculate the focal depth since we'll be using it a lot
    
	vtkCamera *camera = CurrentRenderer->GetActiveCamera();
	camera->GetFocalPoint(viewFocus);
	ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
	focalDepth = viewFocus[2];

	
	ComputeDisplayToWorld(0, 0, focalDepth, oldPickPoint);
	double newX = 0, newY = 0, delta = MotionFactor/3;
	 
  
	int CrossDirection = this->Interactor->GetControlKey();

	switch (CrossDirection)
	{
		case 1	:	
					newY = delta;
					break;

		case 2  :					
					newX = newY = delta;
					break;

		case 3	:
					newX = delta;
					break;	

		case 4	:	
					newX =  delta;
					newY = -delta;
					break;	

		case 5	:
					newY = -delta;
					break;

		case 6	:
					newX = newY = -delta;
					break;

		case 7	:
					newX = -delta;
					break;

		case 8	:
					newY =  delta;
					newX = -delta;
	 }
    
    
	ComputeDisplayToWorld(newX, newY, focalDepth, newPickPoint);

	// Camera motion is reversed

	motionVector[0] = oldPickPoint[0] - newPickPoint[0];
	motionVector[1] = oldPickPoint[1] - newPickPoint[1];
	motionVector[2] = oldPickPoint[2] - newPickPoint[2];
    
	camera->GetFocalPoint(viewFocus);
	camera->GetPosition(viewPoint);
	camera->SetFocalPoint(motionVector[0] + viewFocus[0],
							motionVector[1] + viewFocus[1],
							motionVector[2] + viewFocus[2]);

	camera->SetPosition(motionVector[0] + viewPoint[0],
							motionVector[1] + viewPoint[1],
							motionVector[2] + viewPoint[2]);
      
	if (rwi->GetLightFollowCamera()) 
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}
    
	rwi->Render();
	MotionFactor = 10 + 100*this->CurrentRenderer->GetLastRenderTimeInSeconds();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::Dolly()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  
  vtkRenderWindowInteractor *rwi = this->Interactor;
  double *center = this->CurrentRenderer->GetCenter();
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];  
  double dyf = MotionFactor * (double)(dy) / (double)(center[1]);
  this->Dolly(pow((double)1.1, dyf));
}
//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::Dolly(double factor)
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if (camera->GetParallelProjection())
    {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
    }
  else
    {
    camera->Dolly(factor);
    if (this->AutoAdjustCameraClippingRange)
      {
      this->CurrentRenderer->ResetCameraClippingRange();
      }
    }
	
  if (this->Interactor->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }
  
  this->Interactor->Render();
  MotionFactor = 10 + 100*this->CurrentRenderer->GetLastRenderTimeInSeconds();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "MotionFactor: " << this->MotionFactor << "\n";
}


//----------------------------------------------------------------------------
void vtkInteractorStyleWIITrackball::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                       unsigned long event,
                                       void* clientdata, 
                                       void* calldata)
{
  vtkInteractorStyleWIITrackball* self 
    = reinterpret_cast<vtkInteractorStyleWIITrackball *>( clientdata );

  switch(event)
    {
    case vtkCommand::ExposeEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::ExposeEvent)) 
        {
        self->InvokeEvent(vtkCommand::ExposeEvent,NULL);
        }
      else 
        {
        self->OnExpose();
        }
      break;

    case vtkCommand::ConfigureEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::ConfigureEvent)) 
        {
        self->InvokeEvent(vtkCommand::ConfigureEvent,NULL);
        }
      else 
        {
        self->OnConfigure();
        }
      break;

    case vtkCommand::EnterEvent:  
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::EnterEvent)) 
        {
        self->InvokeEvent(vtkCommand::EnterEvent, NULL);
        }
      else
        {
        self->OnEnter();
        }
      break;

    case vtkCommand::LeaveEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::LeaveEvent)) 
        {
        self->InvokeEvent(vtkCommand::LeaveEvent,NULL);
        }
      else
        {
        self->OnLeave();
        }
      break;

    case vtkCommand::TimerEvent: 
      {
      // The calldata should be a timer id, but because of legacy we check
      // and make sure that it is non-NULL.
      int timerId = (calldata ? *(reinterpret_cast<int*>(calldata)) : 1);
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::TimerEvent)) 
        {
        self->InvokeEvent(vtkCommand::TimerEvent,&timerId);
        }
      else 
        {
        self->OnTimer();
        }
      }
      break;

    case vtkCommand::MouseMoveEvent: 
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::MouseMoveEvent)) 
        {
        self->InvokeEvent(vtkCommand::MouseMoveEvent,NULL);
        }
      else
        {
        self->OnMouseMove();
        }
      break;

    case vtkCommand::LeftButtonPressEvent: 
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::LeftButtonPressEvent)) 
        {
        self->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
        }
      else
        {
        self->OnLeftButtonDown();
        }
      break;

    case vtkCommand::LeftButtonReleaseEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::LeftButtonReleaseEvent)) 
        {
        self->InvokeEvent(vtkCommand::LeftButtonReleaseEvent,NULL);
        }
      else 
        {
        self->OnLeftButtonUp();
        }
      break;

    case vtkCommand::MiddleButtonPressEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::MiddleButtonPressEvent)) 
        {
        self->InvokeEvent(vtkCommand::MiddleButtonPressEvent,NULL);
        }
      else 
        {
        self->OnMiddleButtonDown();
        }
      break;

    case vtkCommand::MiddleButtonReleaseEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::MiddleButtonReleaseEvent)) 
        {
        self->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent,NULL);
        }
      else 
        {
        self->OnMiddleButtonUp();
        }
      break;

    case vtkCommand::RightButtonPressEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::RightButtonPressEvent)) 
        {
        self->InvokeEvent(vtkCommand::RightButtonPressEvent,NULL);
        }
      else 
        {
        self->OnRightButtonDown();
        }
      break;

    case vtkCommand::RightButtonReleaseEvent: 
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::RightButtonReleaseEvent)) 
        {
        self->InvokeEvent(vtkCommand::RightButtonReleaseEvent,NULL);
        }
      else 
        {
        self->OnRightButtonUp();
        }
      break;

    case vtkCommand::MouseWheelForwardEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::MouseWheelForwardEvent)) 
        {
        self->InvokeEvent(vtkCommand::MouseWheelForwardEvent,NULL);
        }
      else 
        {
        self->OnMouseWheelForward();
        }
      break;

    case vtkCommand::MouseWheelBackwardEvent: 
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::MouseWheelBackwardEvent)) 
        {
        self->InvokeEvent(vtkCommand::MouseWheelBackwardEvent,NULL);
        }
      else 
        {
        self->OnMouseWheelBackward();
        }
      break;

    case vtkCommand::KeyPressEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::KeyPressEvent)) 
        {
        self->InvokeEvent(vtkCommand::KeyPressEvent,NULL);
        }
      else 
        {
        self->OnKeyDown();
        self->OnKeyPress();
        }
      break;

    case vtkCommand::KeyReleaseEvent: 
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::KeyReleaseEvent)) 
        {
        self->InvokeEvent(vtkCommand::KeyReleaseEvent,NULL);
        }
      else 
        {
        self->OnKeyUp();
        self->OnKeyRelease(); 
        }
      break;

    case vtkCommand::CharEvent:  
      if (self->HandleObservers && 
          self->HasObserver(vtkCommand::CharEvent)) 
        {
        self->InvokeEvent(vtkCommand::CharEvent,NULL);
        }
      else 
        {
        self->OnChar();
        }
      break;

    case vtkCommand::DeleteEvent:
      self->SetInteractor(0);
      break;
      
    case vtkCommand::TDxMotionEvent:
    case vtkCommand::TDxButtonPressEvent:
    case vtkCommand::TDxButtonReleaseEvent:
      self->DelegateTDxEvent(event,calldata);
      break;

// Wiimote

	//Process Wiimote_Motion_Event
	case vtkWiiMotion:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiMotion)) 
        {
        self->InvokeEvent(vtkWiiMotion,NULL);
        }
      else 
        {
        self->onWiiMotion();
        }
      break;

	//Process Wiimote_A_Down_Event
	case vtkWiiADown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiADown)) 
        {
        self->InvokeEvent(vtkWiiADown,NULL);
        }
      else 
        {
        self->onWiiADown();
        }
      break;

	//Process Wiimote_A_Up_Event
	case vtkWiiAUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiAUp)) 
        {
        self->InvokeEvent(vtkWiiAUp,NULL);
        }
      else 
        {
        self->onWiiAUp();
        }
      break;

	//Process Wiimote_B_Down_Event
	case vtkWiiBDown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiBDown)) 
        {
        self->InvokeEvent(vtkWiiBDown,NULL);
        }
      else 
        {
        self->onWiiBDown();
        }
      break;

	//Process Wiimote_B_Up_Event
	case vtkWiiBUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiBUp)) 
        {
        self->InvokeEvent(vtkWiiBUp,NULL);
        }
      else 
        {
        self->onWiiBUp();
        }
      break;

	//Process Wiimote_Minus_Down_Event
	case vtkWiiMinusDown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiMinusDown)) 
        {
        self->InvokeEvent(vtkWiiMinusDown,NULL);
        }
      else 
        {
        self->onWiiMinusDown();
        }
      break;

	//Process Wiimote_Minus_Up_Event
	case vtkWiiMinusUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiMinusUp)) 
        {
        self->InvokeEvent(vtkWiiMinusUp,NULL);
        }
      else 
        {
        self->onWiiMinusUp();
        }
      break;

	//Process Wiimote_Home_Down_Event
	case vtkWiiHomeDown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiHomeDown)) 
        {
        self->InvokeEvent(vtkWiiHomeDown,NULL);
        }
      else 
        {
        self->onWiiHomeDown();
        }
      break;

	//Process Wiimote_Home_Up_Event
	case vtkWiiHomeUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiHomeUp)) 
        {
        self->InvokeEvent(vtkWiiHomeUp,NULL);
        }
      else 
        {
        self->onWiiHomeUp();
        }
      break;

	//Process Wiimote_Plus_Down_Event
	case vtkWiiPlusDown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiPlusDown)) 
        {
        self->InvokeEvent(vtkWiiPlusDown,NULL);
        }
      else 
        {
        self->onWiiPlusDown();
        }
      break;

	//Process Wiimote_Plus_Up_Event
	case vtkWiiPlusUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiPlusUp)) 
        {
        self->InvokeEvent(vtkWiiPlusUp,NULL);
        }
      else 
        {
        self->onWiiPlusUp();
        }
      break;

	//Process Wiimote_One_Down_Event
	case vtkWiiOneDown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiOneDown)) 
        {
        self->InvokeEvent(vtkWiiOneDown,NULL);
        }
      else 
        {
        self->onWiiOneDown();
        }
      break;

	//Process Wiimote_One_Up_Event
	case vtkWiiOneUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiOneUp)) 
        {
        self->InvokeEvent(vtkWiiOneUp,NULL);
        }
      else 
        {
        self->onWiiOneUp();
        }
      break;

	//Process Wiimote_Two_Down_Event
	case vtkWiiTwoDown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiTwoDown)) 
        {
        self->InvokeEvent(vtkWiiTwoDown,NULL);
        }
      else 
        {
        self->onWiiTwoDown();
        }
      break;

	//Process Wiimote_Two_Up_Event
	case vtkWiiTwoUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiTwoUp)) 
        {
        self->InvokeEvent(vtkWiiTwoUp,NULL);
        }
      else 
        {
        self->onWiiTwoUp();
        }
      break;	

	//Process Wiimote_Cross_Down_Event
	case vtkWiiCrossDown:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiCrossDown)) 
        {
        self->InvokeEvent(vtkWiiCrossDown,NULL);
        }
      else 
        {
        self->onWiiCrossDown();
        }
      break;

	//Process Wiimote_Cross_Up_Event
	case vtkWiiCrossUp:  
      if (self->HandleObservers && 
          self->HasObserver(vtkWiiCrossUp)) 
        {
        self->InvokeEvent(vtkWiiCrossUp,NULL);
        }
      else 
        {
        self->onWiiCrossUp();
        }
      break;
    }
}
