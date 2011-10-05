#include "wxWiiManager.h"
#include <wx/busyinfo.h>
#include "wxVtkViewer3d.h"

wxWiiManager* wxWiiManager::Istance=NULL;

wxWiiManager* wxWiiManager::getIstance(wxVtkViewer3d* viewer3d) 
{
       if (Istance==NULL) Istance = new wxWiiManager(viewer3d);
       return Istance;
}

wxWiiManager::wxWiiManager(wxVtkViewer3d* viewer3d)	
{	
	WiiManagerStarted = false;
	this->viewer3d		= viewer3d;
	this->WiiEvtHandler	= viewer3d->getWxWindow()->GetEventHandler();
	this->renderer		= viewer3d->getRenderer();
	this->camera		= renderer->GetActiveCamera();

	_isSceneRotatingIR_1 = false;
	_pointerRadius		= 1;
	WiiPointing_num = 0;
	first = (Wiimote*) NULL;
	sem = CreateEvent(NULL, FALSE, TRUE, NULL);
	Wii_num = 0;

	//initInteractionSignal();	
	
}	

bool wxWiiManager::QuickSetupGui(wxWindow* win)
{
	bool retval = false;
	int retvalDialog;

	do
	{

		wxBusyInfo wait("Rilevamento Wiimote in corso...");

		RilevaWii();
		
		retvalDialog = wxID_OK;

		if (Wii_num)
		{
			// associa i colori di default
			for (int i=1; i<=Wii_num; i++) 
			{
				findWii(i)->pointerColor = i;
			}

			
			// inizializza i Wiimote collegati
			//InitSelectedWii();

			// Wiimote inizializzati in modalità puntatore
			WiiPointing_num = Wii_num;

			retval = true;
		}
		else
		{
			int* batteryLevel = new int [Wii_num];
			wxWiiManagerDialog* gui = new wxWiiManagerDialog( win, Wii_num, batteryLevel);
			retvalDialog = gui->ShowModal();
			retval = false;
			gui->Destroy();
		}
	}
	while (retvalDialog == rilevaPressed);
 
	return retval;
}

bool wxWiiManager::SetupGui(wxWindow* win)
{
	bool retval = false;
    
	wxBusyInfo wait("Rilevamento Wiimote in corso...");

	RilevaWii();
	
	int* batteryLevel = new int [Wii_num];		// vettore temporaneo da passare al costruttore del wxWiiManagerDialog
	for (int i=1; i<=Wii_num; i++)	batteryLevel[i-1] = findWii(i)->batteryLevel;
	
	wxWiiManagerDialog* gui = new wxWiiManagerDialog( win, Wii_num, batteryLevel);
	
	delete batteryLevel;

	int retvalDialog;
	do
	{
		retvalDialog = gui->ShowModal();
        
		if ( retvalDialog == wxID_OK )
		{
			wxChoice* rotMode = gui->getRotationModeChoice();
			_isSceneRotatingIR_1 = rotMode->GetColumns();

			for (int i=1; i<=Wii_num; i++) 
			{
				if ( gui->isWiiSelected(i) ) 
				{
					findWii(i)->pointerColor = gui->getColorChosen(i);
				}
                else RemoveWii(i);
			}

			//InitSelectedWii();

			// Wiimote inizializzati in modalità puntatore
			WiiPointing_num = Wii_num;

			retval = true;
		}
		else 
			if ( retvalDialog == rilevaPressed )
			{
				gui->Destroy();
				wxBusyInfo wait("Rilevamento Wiimote in corso...");
				RemoveWii(-1);			// Rimuove tutti gli elementi dalla lista dei Wii rilevati.
				
				RilevaWii();
				
				batteryLevel = new int [Wii_num];	
				for (int i=1; i<=Wii_num; i++)	batteryLevel[i-1] = findWii(i)->batteryLevel;
				
				gui = new wxWiiManagerDialog( win, Wii_num, batteryLevel);	
				
				delete batteryLevel;
			}
			else
				if (retvalDialog == wxID_CANCEL )
				{
					RemoveWii(-1);		// Rimuove tutti gli elementi dalla lista dei Wii rilevati.
				}
	
	}
	while ( retvalDialog == rilevaPressed );
 
	return retval;
}

int wxWiiManager::RilevaWii()
{
	int FoundIndex = 0;
	int StartIndex = 0;
	while ( FoundIndex != -1 )	//FoundIndex varrà -1 se ConnectToDevice non trova alcun Wii a partire da StartIndex
	{
		wxWiiDriver Wii(WiiEvtHandler,sem);
		
		int timeout=50;
		do  { timeout--; }
		while ( (!Wii.ConnectToDevice(FoundIndex,StartIndex)) && timeout>0 );

		if (timeout>0)
		{
            Wii_num++;
			AddWii(Wii_num, FoundIndex, Wii.getWiiBatteryLevel());            

			FoundIndex = -1; //Cosi al primo Wiimote che trova smette di cercare
		}
				
		StartIndex = FoundIndex + 1;
	}

	return Wii_num;
}

bool wxWiiManager::AddWii(int idWii, int HIDTableIndex, int batteryLevel)
{
	bool retval = false;
	if (HIDTableIndex > -1 )
	{
		Wiimote* t = new Wiimote;
		t->idWii = idWii;		
		t->HIDTableIndex = HIDTableIndex;
		t->WiiStarted = false;
		t->batteryLevel = batteryLevel;		

		t->next = first;
		first = t;

		retval = true;
	}
	return retval;
}

bool wxWiiManager::RemoveWii(int idWii)
{
    bool retval = false;
	
	Wiimote* t = first;
	Wiimote* prec = NULL;
	while (t != (Wiimote*)NULL)	
	{
		if ( (t->idWii == idWii) || (idWii == -1 /* AnyWii */) )
		{
			if (t == first) { first = t->next; }
			else prec->next = t->next;
			delete t;
			Wii_num--;
			retval = true;
		}
		else prec = t;

		if (prec==NULL) { t=first; }		
		else t = prec->next;
	}
	
	return retval;
}
void wxWiiManager::InitSelectedWii()
{
	//double Position[3];
	//renderer->GetActiveCamera()->GetFocalPoint(Position);

	Wiimote* t = first;
	while (t != (Wiimote*)NULL)	
	{
		t->driver = NULL;
		t->WiiStarted = false;

		t->Mode = Pointing;
		t->renderDuty = false;
		t->isOnSurface = false;
		
		
		// per il PRISM filter		
		t->interval			 = 0.25;
		t->minDISP			 = 1279 * 0.005;
		t->maxDISP			 = 1279 * 0.75;
		t->minRotDISP		 = 2 * 0.01;
		t->maxRotDISP		 = 2 * 0.5;
		t->invCDratio		 = 1;
		t->invCDRollRatio	 = 1;
		t->lastVelocityTime  = 0;
		t->xPositionVelocity = 0;
		t->yPositionVelocity = 0;
		t->zPositionVelocity = 0;		

		//t->latencyEvent	= maxLatency;
		//t->timeoutEvent	= 0;

		for (int i=0;i<5;i++) t->lastWiiX[i] = t->lastWiiY[i] = t->lastIR_X[i] = t->lastIR_Y[i] =0;		
		t->WiiXo = t->WiiYo = 0;
		t->IR_Xo = t->IR_Yo = 0;
		//t->lastIR_X = 511;
		//t->lastIR_Y = 383;

		
		// create an actor and give it cone geometry
		// and map to graphics library

		/*
		t->pointerGeometry = vtkConeSource::New();
		t->pointerGeometry->SetDirection(0,1,-1);
		t->pointerGeometry->SetResolution(20);		
		t->pointerGeometry->SetHeight(4);
		t->pointerGeometry->SetRadius(1);
		*/		
		
		t->pointerGeometry = vtkSphereSource::New();   
		t->pointerGeometry->SetRadius(_pointerRadius);				
		t->pointerGeometry->SetPhiResolution(20);
		t->pointerGeometry->SetThetaResolution(20);

		t->pointerMapper = vtkDataSetMapper::New();
		t->pointerMapper->SetInput(t->pointerGeometry->GetOutput());	
		t->pointerMapper->ImmediateModeRenderingOn();

		// actor coordinates geometry, properties, transformation
		t->pointer = vtkActor::New();
		t->pointer->SetMapper(t->pointerMapper);
		//t->pointer->VisibilityOff();
		
		// setta la posizione iniziale del puntatore
		double pointerPosition[3];
		double versor[3];
		this->camera->GetDirectionOfProjection(versor);						
		double* clip = this->camera->GetClippingRange();
		double focus = this->camera->GetDistance();						
		double* camPosition = this->camera->GetPosition();						
		pointerPosition[0] = camPosition[0] + ((clip[0]+focus)/2) * versor[0]; 
		pointerPosition[1] = camPosition[1] + ((clip[0]+focus)/2) * versor[1];
		pointerPosition[2] = camPosition[2] + ((clip[0]+focus)/2) * versor[2];
		
		camera->GetFocalPoint(pointerPosition);		
		t->pointer->SetPosition(pointerPosition);
		
		switch (t->pointerColor)
		{
			case 0 :	//Red
				t->pointer->GetProperty()->SetColor(1,0,0);
				break;

			case 1 :	//Yellow
				t->pointer->GetProperty()->SetColor(1,1,0);
				break;

			case 2 :	//Blu
				t->pointer->GetProperty()->SetColor(0,0,1);
				break;

			case 3 :	//Green
				t->pointer->GetProperty()->SetColor(0,1,0);
				break;
			
			case 4 :	//Violet
				t->pointer->GetProperty()->SetColor(1,0,1);
				break;

			case 5 :	//Orange
				t->pointer->GetProperty()->SetColor(2,1,0);
				break;

			case 6 :	//White
				t->pointer->GetProperty()->SetColor(1,1,1);
				break;

			default :
				t->pointer->GetProperty()->SetColor(1,0,0);

		}

		
		renderer->AddActor(t->pointer);		

		t = t->next;
	}

}		
wxWiiManager::Wiimote* wxWiiManager::findWii(int idWii)
{
	Wiimote* retval = NULL;
	
	Wiimote* t = first;
	while (t != (Wiimote*)NULL)	
	{
		if (t->idWii == idWii)
		{
			retval = t;
			break;
		}
		t = t->next;
	}
	
	return retval;
}

bool wxWiiManager::getLeds(int idWii, bool & Led1, bool & Led2, bool & Led3, bool & Led4)
{
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;
	if (w->WiiStarted) { 
		Led1 = w->driver->Led1; 
		Led2 = w->driver->Led2; 
		Led3 = w->driver->Led3; 
		Led4 = w->driver->Led4; 
		return true;
	} 
	else return false; 
}

bool wxWiiManager::getLed1(int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;
	
	if (w->WiiStarted) { return w->driver->Led1; } else return false; 
}

bool wxWiiManager::getLed2(int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;
		
	if (w->WiiStarted) { return w->driver->Led2; } else return false; 
}

bool wxWiiManager::getLed3(int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;
		
	if (w->WiiStarted) { return w->driver->Led3; } else return false; 
}

bool wxWiiManager::getLed4(int idWii) 	
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;
		
	if (w->WiiStarted) { return w->driver->Led4; } else return false; 
}

bool wxWiiManager::setLeds(int idWii, bool Led1, bool Led2, bool Led3, bool Led4) 
{
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;
			
	if (w->WiiStarted) 
	{
		w->driver->Led1 = Led1; 
		w->driver->Led2 = Led2; 
		w->driver->Led3 = Led3; 
		w->driver->Led4 = Led4; 
		
		w->driver->changeLedsStatus = true;
	
		return true;
	} 
	else return false; 
}

bool wxWiiManager::setLed1(bool value, int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;		
		
	if (w->WiiStarted) {
		w->driver->Led1 = value; 
		w->driver->changeLedsStatus = true;
	}
	return true;
}

bool wxWiiManager::setLed2(bool value, int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;		
		
	if (w->WiiStarted) {
		w->driver->Led2 = value; 
		w->driver->changeLedsStatus = true;
	}
	return true;
}

bool wxWiiManager::setLed3(bool value, int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;		
		
	if (w->WiiStarted) {
		w->driver->Led3 = value; 
		w->driver->changeLedsStatus = true;
	}
	return true;
}

bool wxWiiManager::setLed4(bool value, int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return false;		
		
	if (w->WiiStarted) {
		w->driver->Led4 = value; 
		w->driver->changeLedsStatus = true;
	}
	return true;
}

void wxWiiManager::setVibration(int cont, int idWii) 
{ 
	Wiimote* w = findWii(idWii);
	if ( w == (Wiimote*)NULL ) return ;		
		
	//if (w->WiiStarted) w->driver->VibrationCont = cont;  
}

void wxWiiManager::DisableTheOthersWii(int idWii)
{
	if (!WiiManagerStarted) return;
	Wiimote* t = first;
	while ( t != (Wiimote*)NULL )
	{
		if (t->idWii != idWii)	t->driver->SendEvents = false;
		t = t->next;
	}
}
void wxWiiManager::EnableTheOthersWii(int idWii)
{
	if (!WiiManagerStarted) return;
	Wiimote* t = first;
	while ( t != (Wiimote*)NULL )
	{
		if (t->idWii != idWii)	t->driver->SendEvents = true;
		t = t->next;
	}
}

bool wxWiiManager::AnotherWiiDoingRender(int idWii)
{
	bool retval = false;
	if (!WiiManagerStarted) return false;
	Wiimote* t = first;
	while ( t != (Wiimote*)NULL )
	{
		if ( (t->idWii != idWii) && (t->renderDuty) ) return true;
		t = t->next;
	}
	return retval;
}

void wxWiiManager::ReleaseRenderDuty()
{
	if (!WiiManagerStarted) return;
	Wiimote* t = first;
	while ( t != (Wiimote*)NULL )
	{
		if (t->renderDuty)
		{
			t->renderDuty = false;
			//break;
		}
		t = t->next;
	}
}



bool wxWiiManager::Start() 
{
     bool retval = false;
	 	 
     if (WiiManagerStarted) return retval;     

	 InitSelectedWii();

	 Wiimote* t = first;	
	 while ( t != (Wiimote*)NULL ) 
	 {
		 t->driver = new wxWiiDriver(WiiEvtHandler, sem, t->idWii, t->HIDTableIndex);     
		 if ( t->driver->Create() != wxTHREAD_NO_ERROR )
		 {
			 wxLogError(wxT("Can't create thread!"));
			 retval = false;
			 break;
		 }
		 else 
		 {
			 t->driver->Run();
			 retval = true;
			 t->WiiStarted=true;
		 }
		 t = t->next;
		
	 }
	 

	 WiiManagerStarted = true;
     return retval;
}


bool wxWiiManager::Close()
{
	bool retval=false;
	if (!WiiManagerStarted) return retval;     	 
    
	// Chiedo ad ogni thread di uscire dal loop di interazione col relativo Wiimote
	Wiimote* t = first;	
	while ( t != (Wiimote*)NULL ) 
	{
		t->driver->closingThreadLoop=true;	 	 
		t = t->next;
	}

	
	// Resto in attesa finchè ogni thread non è uscito dal proprio loop di interazione
	bool allWiiClosed;
	do
	{
		allWiiClosed = true;

		t = first;	
		while ( t != (Wiimote*)NULL ) 
		{			
			allWiiClosed = allWiiClosed && (! t->driver->stillLooping );
			t = t->next;			
		}		
	}
	while ( allWiiClosed == false );

	// Solo adesso posso procedere alla liberazione delle risorse
	t = first;	
	while ( t != (Wiimote*)NULL ) 
	{
		t->driver->closingThread=true;	 	 
		first = t->next;

		renderer->RemoveActor(t->pointer);
		t->pointerGeometry->Delete();
		t->pointerMapper->Delete();
		t->pointer->Delete();

		delete t;
		t = first;
	}
	
	retval=true;	
	WiiManagerStarted=false;
	return retval;
}

