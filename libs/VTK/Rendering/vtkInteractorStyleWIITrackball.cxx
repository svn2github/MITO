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

vtkCxxRevisionMacro(vtkInteractorStyleWIITrackball, "$Revision: 1.10 $");
vtkStandardNewMacro(vtkInteractorStyleWIITrackball);

//----------------------------------------------------------------------------
vtkInteractorStyleWIITrackball::vtkInteractorStyleWIITrackball() 
{
  this->MotionFactor   = 10.0;

}

//----------------------------------------------------------------------------
vtkInteractorStyleWIITrackball::~vtkInteractorStyleWIITrackball() 
{
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

  /*
  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  
  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();
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

  double rxf	= -dz / 100;
  double ryf	=  dy / 100;
  double angle	= -dx / 100;  
  
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

