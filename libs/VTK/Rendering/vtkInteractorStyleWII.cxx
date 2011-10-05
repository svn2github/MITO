/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkInteractorStyleWII.cxx,v $

  Author:	 Luigi Gallo (ICAR-CNR), Aniello Minutolo (ICAR-CNR)

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInteractorStyleWII.h"
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

vtkCxxRevisionMacro(vtkInteractorStyleWII, "$Revision: 1.10 $");
vtkStandardNewMacro(vtkInteractorStyleWII);

//----------------------------------------------------------------------------
vtkInteractorStyleWII::vtkInteractorStyleWII() 
{
  this->MotionFactor   = 10.0;
}

//----------------------------------------------------------------------------
vtkInteractorStyleWII::~vtkInteractorStyleWII() 
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiMotion() 
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

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiADown()
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

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiAUp() 
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

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiBDown() 
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

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiBUp() 
{
	if (this->State == VTKIS_ROTATE) 
	{
		this->EndRotate();		
    }
	if ( this->Interactor )
    {
		this->ReleaseFocus();
    }	
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiMinusDown() {

	if (this->CurrentRenderer == NULL)
	{
		this->FindPokedRenderer(0,0);
	}

	this->GrabFocus(this->EventCallbackCommand);
	this->StartDolly();
	this->Dolly(pow((double)1.01, -MotionFactor/10));
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiMinusUp() 
{
	if (this->State ==  VTKIS_DOLLY )
	{
        this->EndDolly();
	}
		
	if (this->Interactor)
	{
		this->ReleaseFocus();
	}	
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiHomeDown() {
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiHomeUp() {
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiPlusDown() 
{
	if (this->CurrentRenderer == NULL)
	{
		this->FindPokedRenderer(0,0);
	}

	this->GrabFocus(this->EventCallbackCommand);
	this->StartDolly();
	this->Dolly(pow((double)1.01, MotionFactor/10));
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiPlusUp() 
{
	if (this->State ==  VTKIS_DOLLY )
	{
        this->EndDolly();
	}
		
	if (this->Interactor)
	{
		this->ReleaseFocus();
	}	
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiOneDown() {	
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiOneUp() {
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiTwoDown() {
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiTwoUp() {
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiCrossDown() 
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

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::onWiiCrossUp() 
{
	if (this->State = VTKIS_PAN) 
	{
		this->EndPan();
	}
	
	if (this->Interactor)
	{
		this->ReleaseFocus();
	}	
};

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::OnKeyDown()
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::OnKeyUp()
{
}

//----------------------------------------------------------------------------
void dxdy_To_xyz( int dx, int dy, int & x, int & y, int & z)
{	
	x = dx >> 16;
	if (x >= 0x8000) {
		x = x & 0x7fff;
		x = -x;
	}
	
	y = dx;
	if (y >= 0x8000) {
		y = y & 0x7fff;
		y = -y;
	}

	z=0;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::OnMouseMove() 
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::OnLeftButtonDown() 
{ 
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::OnLeftButtonUp()
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::OnRightButtonDown() 
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::OnRightButtonUp()
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::Rotate()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  
  vtkRenderWindowInteractor *rwi = this->Interactor;

  int dx = rwi->GetEventPosition()[0];
  int dy = rwi->GetEventPosition()[1];
  int dz = rwi->GetShiftKey();

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();  
    
  double rxf	= (double)(-dz * MotionFactor/1000 );
  double ryf	= (double)( dy * MotionFactor/1000 );
  double angle	= (double)(-dx * MotionFactor/1000 );  
  
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
void vtkInteractorStyleWII::Spin()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkRenderWindowInteractor *rwi = this->Interactor;

  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  double angle = (double)( -( rwi->GetEventPosition()[0] * MotionFactor / size[0]) );

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Roll(angle);
  camera->OrthogonalizeViewUp();
      
  rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWII::Pan()
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
	double newX = 0, newY = 0, teta = MotionFactor/3;
	 
	int CrossDirection = this->Interactor->GetControlKey();

	switch (CrossDirection)
	{
		case 1	:	
					newY = teta;
					break;

		case 2  :					
					newX = newY = teta;
					break;

		case 3	:
					newX = teta;
					break;	

		case 4	:	
					newX = teta;
					newY = -teta;
					break;	

		case 5	:
					newY = -teta;
					break;

		case 6	:
					newX = newY = -teta;
					break;

		case 7	:
					newX = -teta;
					break;

		case 8	:
					newY = teta;
					newX = -teta;
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
void vtkInteractorStyleWII::Dolly()
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
void vtkInteractorStyleWII::Dolly(double factor)
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
void vtkInteractorStyleWII::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

