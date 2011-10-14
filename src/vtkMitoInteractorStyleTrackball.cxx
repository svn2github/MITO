/*=========================================================================


=========================================================================*/
#include "vtkMitoInteractorStyleTrackball.h"
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

vtkCxxRevisionMacro(vtkMitoInteractorStyleTrackball, "$Revision: 1.10 $");
vtkStandardNewMacro(vtkMitoInteractorStyleTrackball);

//----------------------------------------------------------------------------
vtkMitoInteractorStyleTrackball::vtkMitoInteractorStyleTrackball() : vtkInteractorStyleTrackballCamera()
{
  
}

//----------------------------------------------------------------------------
vtkMitoInteractorStyleTrackball::~vtkMitoInteractorStyleTrackball() 
{

}

void vtkMitoInteractorStyleTrackball::OnMouseMove() 
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

    case VTKIS_PAN:
      this->FindPokedRenderer(x, y);
      this->Pan();
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;

    case VTKIS_DOLLY:
      this->FindPokedRenderer(x, y);
      this->Dolly();
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
void vtkMitoInteractorStyleTrackball::Rotate()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  
  vtkRenderWindowInteractor *rwi = this->Interactor;

  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
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
  double delta_elevation = -20.0 / size[1];
  double delta_azimuth = -20.0 / size[0];
  
  double rxf = dx * delta_azimuth * this->MotionFactor;
  double ryf = dy * delta_elevation * this->MotionFactor;
  
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Azimuth(rxf);
  camera->Elevation(ryf);
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
}


//----------------------------------------------------------------------------
void vtkMitoInteractorStyleTrackball::Spin()
{
  if ( this->CurrentRenderer == NULL )
    {
    return;
    }

  vtkRenderWindowInteractor *rwi = this->Interactor;
  
  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();
  double angle = (double)( -( rwi->GetEventPosition()[0] * MotionFactor / size[0]) );

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Roll( angle);
  camera->OrthogonalizeViewUp();
      
  rwi->Render();
}

//----------------------------------------------------------------------------
void vtkMitoInteractorStyleTrackball::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "MotionFactor: " << this->MotionFactor << "\n";
}