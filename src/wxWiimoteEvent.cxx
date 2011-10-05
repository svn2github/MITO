#include "wxWiimoteEvent.h"

DEFINE_EVENT_TYPE(wxEVT_WII_MOTION_IR);
DEFINE_EVENT_TYPE(wxEVT_WII_A_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_A_UP);
DEFINE_EVENT_TYPE(wxEVT_WII_B_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_B_UP);
DEFINE_EVENT_TYPE(wxEVT_WII_MINUS_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_MINUS_UP);
DEFINE_EVENT_TYPE(wxEVT_WII_HOME_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_HOME_UP);
DEFINE_EVENT_TYPE(wxEVT_WII_PLUS_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_PLUS_UP);
DEFINE_EVENT_TYPE(wxEVT_WII_ONE_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_ONE_UP);
DEFINE_EVENT_TYPE(wxEVT_WII_TWO_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_TWO_UP);
DEFINE_EVENT_TYPE(wxEVT_WII_CROSS_DOWN);
DEFINE_EVENT_TYPE(wxEVT_WII_CROSS_UP);

IMPLEMENT_DYNAMIC_CLASS(wxWiimoteEvent, wxEvent)


wxWiimoteEvent::wxWiimoteEvent() : wxEvent() 
{	
	A = B = Plus = Minus = Home = One = Two = false;
	LeftCross = RightCross = UpCross = DownCross = false;		

	WiiX = WiiY = WiiZ = 0.f;

	IRLight1X = IRLight1Y = IRLight2X = IRLight2Y = -1;		
	IRLight1Found = IRLight2Found = false;

	Led1 = Led2 = Led3 = Led4 = false;

	idWii = 0;
}

wxWiimoteEvent::wxWiimoteEvent(wxEventType type) {
	wxWiimoteEvent();
	this->SetEventType(type);
}

wxWiimoteEvent::wxWiimoteEvent(const wxWiimoteEvent & event) {
	wxWiimoteEvent();

	//--- new ---
	this->A				= event.A;
	this->B				= event.B;
	this->Minus			= event.Minus;
	this->Home			= event.Home;
	this->Plus			= event.Plus;    
	this->One			= event.One;
	this->Two			= event.Two;    
	this->LeftCross		= event.LeftCross;
	this->RightCross	= event.RightCross;
	this->UpCross		= event.UpCross;
	this->DownCross		= event.DownCross;
	this->WiiX			= event.WiiX;
	this->WiiY			= event.WiiY;
	this->WiiZ			= event.WiiZ;
	this->IRLight1Found = event.IRLight1Found;
	this->IRLight1X		= event.IRLight1X;
	this->IRLight1Y		= event.IRLight1Y;
	this->IRLight2Found = event.IRLight2Found;
	this->IRLight2X		= event.IRLight2X;
	this->IRLight2Y		= event.IRLight2Y;
	this->Led1			= event.Led1; 
	this->Led2			= event.Led2; 
	this->Led3			= event.Led3; 
	this->Led4			= event.Led4;
	this->idWii			= event.idWii;
	this->SetEventObject(event.GetEventObject());
	this->SetEventType(event.GetEventType());
	this->SetTimestamp(event.GetTimestamp());
	//--- --- ----

	//this->SetEventType( event.GetEventType() );  //not really needed in this sample, but it's boring to have it empty
}