/*

*/

#include "vtkBoxWidgetWII.h"
#include "WiiCommandEvents.h"


vtkStandardNewMacro(vtkBoxWidgetWii);


vtkBoxWidgetWii::vtkBoxWidgetWii() : vtkBoxWidget()
{
  this->EventCallbackCommand->SetCallback(vtkBoxWidgetWii::ProcessEvents);
}

void vtkBoxWidgetWii::SetEnabled(int enabling) 
{
	if (enabling) 
	{
		if ( this->Enabled ) //already enabled, just return
		{
		  return;
		}
	    
		if ( ! this->CurrentRenderer )
		{
			this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]));
			
			if (this->CurrentRenderer == NULL)
			{
				return;
			}
		}
	}

	vtkBoxWidget::SetEnabled(enabling);

	if (enabling) {
		// listen to the following events
		vtkRenderWindowInteractor *i = this->Interactor;

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

		// Wiimote

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
}

void vtkBoxWidgetWii::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                 unsigned long event,
                                 void* clientdata, 
                                 void* vtkNotUsed(calldata))
{
  vtkBoxWidgetWii* self = reinterpret_cast<vtkBoxWidgetWii *>( clientdata );

  //okay, let's do the right thing
  switch(event)
    {
    case vtkCommand::LeftButtonPressEvent:
      self->OnLeftButtonDown();
      break;
    case vtkCommand::LeftButtonReleaseEvent:
      self->OnLeftButtonUp();
      break;
    case vtkCommand::MiddleButtonPressEvent:
      self->OnMiddleButtonDown();
      break;
    case vtkCommand::MiddleButtonReleaseEvent:
      self->OnMiddleButtonUp();
      break;
    case vtkCommand::RightButtonPressEvent:
      self->OnRightButtonDown();
      break;
    case vtkCommand::RightButtonReleaseEvent:
      self->OnRightButtonUp();
      break;
    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;

    //Wiimote

	//Process Wiimote_Motion_Event
	case vtkWiiMotion:  
		self->onWiiMotion();    
		break;

	//Process Wiimote_A_Down_Event
	case vtkWiiADown:  
		self->onWiiADown();      
		break;

	//Process Wiimote_A_Up_Event
	case vtkWiiAUp:  
		self->onWiiAUp();
		break;

	//Process Wiimote_B_Down_Event
	case vtkWiiBDown:  
		self->onWiiBDown();
		break;

	//Process Wiimote_B_Up_Event
	case vtkWiiBUp:        
		self->onWiiBUp();
		break;

	//Process Wiimote_Minus_Down_Event
	case vtkWiiMinusDown:  
		self->onWiiMinusDown();
		break;

	//Process Wiimote_Minus_Up_Event
	case vtkWiiMinusUp:  
		self->onWiiMinusUp();
		break;

	//Process Wiimote_Home_Down_Event
	case vtkWiiHomeDown:  
		self->onWiiHomeDown();
		break;

	//Process Wiimote_Home_Up_Event
	case vtkWiiHomeUp:  
		self->onWiiHomeUp();
		break;

	//Process Wiimote_Plus_Down_Event
	case vtkWiiPlusDown:  
		self->onWiiPlusDown();
		break;

	//Process Wiimote_Plus_Up_Event
	case vtkWiiPlusUp:  
		self->onWiiPlusUp();
		break;

	//Process Wiimote_One_Down_Event
	case vtkWiiOneDown:        
		self->onWiiOneDown();
		break;

	//Process Wiimote_One_Up_Event
	case vtkWiiOneUp:  
		self->onWiiOneUp();
		break;

	//Process Wiimote_Two_Down_Event
	case vtkWiiTwoDown:        
		self->onWiiTwoDown();
		break;

	//Process Wiimote_Two_Up_Event
	case vtkWiiTwoUp:  
		self->onWiiTwoUp();
		break;	

	//Process Wiimote_Cross_Down_Event
	case vtkWiiCrossDown:  
		self->onWiiCrossDown();
		break;

	//Process Wiimote_Cross_Up_Event
	case vtkWiiCrossUp:  
		self->onWiiCrossUp();     
    }
}


//---------------------------------------------------------- Wii Methods -------------------------------------------------------------------

void vtkBoxWidgetWii::onWiiMotion() 
{   		
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiADown()
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiAUp() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiBDown() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiBUp() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiMinusDown() 
{
	this->State = vtkBoxWidget::Moving;		
	this->HighlightFace(this->HighlightHandle(NULL));
	idFaceSelected = (idFaceSelected+5)%6;
	this->HighlightHandle(Handle[idFaceSelected]);
	this->HighlightFace(idFaceSelected);	
	this->HandlePicker->GetPickPosition(this->LastPickPosition);
	this->ValidPick = 1;

	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	

	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	this->Interactor->Render();
}

void vtkBoxWidgetWii::onWiiMinusUp() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiHomeDown() 
{  
	if ( this->State == vtkBoxWidget::Outside || this->State == vtkBoxWidget::Start )
	{
		return;
	}
	
	this->State = vtkBoxWidget::Start;
	this->HighlightFace(this->HighlightHandle(NULL));
	//this->SizeHandles();
    
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	

	this->EndInteraction();
	this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
	if ( Interactor->GetControlKey() ) Interactor->Render();
}

void vtkBoxWidgetWii::onWiiHomeUp() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiPlusDown() 
{	  
	this->State = vtkBoxWidget::Moving;		
	
	this->HighlightFace(this->HighlightHandle(NULL));

	idFaceSelected = (idFaceSelected+1)%6;

	this->HighlightHandle(Handle[idFaceSelected]);
	this->HighlightFace(idFaceSelected);	
	this->HandlePicker->GetPickPosition(this->LastPickPosition);
	this->ValidPick = 1;

	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	

	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	this->Interactor->Render();
}

void vtkBoxWidgetWii::onWiiPlusUp() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiOneDown() 
{	
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiOneUp() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiTwoDown() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiTwoUp() 
{
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiCrossDown() 
{		
	// Process the motion
	if ( this->State == vtkBoxWidget::Moving )
    {		
		double newPickPoint[3], pickPoint[3] = {0,0,0};	
		
		this->CurrentHandle = (vtkActor *)this->Handle[idFaceSelected];
		this->ValidPick = 1;
		
		// Se translationFactor è positivo la faccia selezionata si muoverà lungo la normale uscente da essa, 
		// se negativo si muoverà nel verso opposto, se nullo la faccià resterà ferma.
		double translationFactor = this->Interactor->GetShiftKey() / 1000;

		int CrossDirection = this->Interactor->GetControlKey();

		double Center[3], Center_DisplayPosition[3], newCenter_DisplayPosition[3];		
		CurrentRenderer->GetActiveCamera()->GetFocalPoint(Center);
		ComputeWorldToDisplay(Center[0], Center[1], Center[2], Center_DisplayPosition);
		
		if ( idFaceSelected != 6 )
		{	
			ComputeWorldToDisplay(	Center[0] + N[idFaceSelected][0], Center[1] + N[idFaceSelected][1], 
									Center[2] + N[idFaceSelected][2], newCenter_DisplayPosition);

		
			if ( CrossDirection == 1 )			// Up Direction
			{
				if ( (newCenter_DisplayPosition[1]-Center_DisplayPosition[1]) < 0 )		
				{ 
					translationFactor = -translationFactor; 
				}
				else if ( (newCenter_DisplayPosition[1]-Center_DisplayPosition[1]) == 0  )
				{
					if ( (newCenter_DisplayPosition[2]-Center_DisplayPosition[2]) <  0 )	{ translationFactor = -translationFactor; }
					else if ( (newCenter_DisplayPosition[2]-Center_DisplayPosition[2]) == 0 )		{ translationFactor = 0; }
				}
			}
			else if ( CrossDirection == 3 )		// Right Direction						
			{
				if ( (newCenter_DisplayPosition[0]-Center_DisplayPosition[0]) < 0 )		{ translationFactor = -translationFactor; }
				else if ( (newCenter_DisplayPosition[0]-Center_DisplayPosition[0]) == 0 )		{ translationFactor = 0; }
			}
			else if ( CrossDirection == 5 )		// Down Direction
			{
				if ( (newCenter_DisplayPosition[1]-Center_DisplayPosition[1]) > 0 )	
				{
					translationFactor = -translationFactor;
				}
				else if ( (newCenter_DisplayPosition[1]-Center_DisplayPosition[1]) == 0 )
				{
					if ( (newCenter_DisplayPosition[2]-Center_DisplayPosition[2]) >  0	)	{ translationFactor = -translationFactor; }
					else if ( (newCenter_DisplayPosition[2]-Center_DisplayPosition[2]) ==  0	)	{ translationFactor = 0; }
				}
			}
			else if ( CrossDirection == 7 )		// Left Direction
			{
				if ( (newCenter_DisplayPosition[0]-Center_DisplayPosition[0]) > 0 )		{ translationFactor = -translationFactor; }
				else if ( (newCenter_DisplayPosition[0]-Center_DisplayPosition[0]) == 0 )		{ translationFactor = 0; }
			}
			
			for (int i=0; i<3; i++) 
				newPickPoint[i] = pickPoint[i] + translationFactor * this->N[idFaceSelected][i];

			if ( this->TranslationEnabled && this->ScalingEnabled ) 
        {
			if ( idFaceSelected == 0 )
			{
				this->MoveMinusXFace(pickPoint, newPickPoint);
			}
			else if ( idFaceSelected == 1 )
			{
				this->MovePlusXFace(pickPoint, newPickPoint);
			}
			else if ( idFaceSelected == 2 )
			{
				this->MoveMinusYFace(pickPoint, newPickPoint);
			}
			else if ( idFaceSelected == 3 )
			{
				this->MovePlusYFace(pickPoint, newPickPoint);
			}
			else if ( idFaceSelected == 4 )
			{
				this->MoveMinusZFace(pickPoint, newPickPoint);
			}
			else if ( idFaceSelected == 5 )
			{
				this->MovePlusZFace(pickPoint, newPickPoint);
			}
        }	


		}
		else 
		{    
			switch (CrossDirection)
			{
				case 1	:	// Up Direction
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0];
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1]+translationFactor;
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
							break;

				case 2  :	// RightUp Direction						
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0]+translationFactor;
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1]+translationFactor;
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
							break;

				case 3	:	// Right Direction						
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0]+translationFactor;
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1];
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
							break;	

				case 4	:	// RightDown Direction						
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0]+translationFactor;
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1]-translationFactor;
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
							break;	

				case 5	:	// Down Direction						
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0];
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1]-translationFactor;
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
							break;

				case 6	:	// LeftDown Direction
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0]-translationFactor;
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1]-translationFactor;
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
							break;

				case 7	:	// Left Direction						
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0]-translationFactor;
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1];
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
							break;

				case 8	:	// LeftUp Direction
							newCenter_DisplayPosition[0]=Center_DisplayPosition[0]-translationFactor;
							newCenter_DisplayPosition[1]=Center_DisplayPosition[1]+translationFactor;
							newCenter_DisplayPosition[2]=Center_DisplayPosition[2];
			}

			ComputeDisplayToWorld(newCenter_DisplayPosition[0],newCenter_DisplayPosition[1],newCenter_DisplayPosition[2],newPickPoint);

			Translate(pickPoint, newPickPoint);

		}



		
		// Interact, if desired
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);		
	}   
	
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	
}

void vtkBoxWidgetWii::onWiiCrossUp() 
{ 
	// Settando questo flag l'evento vtk non sarà propagato ulteriormente.
	this->EventCallbackCommand->SetAbortFlag(1);	

	this->Interactor->Render();
}

/*
void vtkBoxWidget::WiiRotate(int X, int Y, double *p1, double *p2, double *vpn)
{
}
*/
//------------------------------------------------------------------------------------------------------------------------------------------

