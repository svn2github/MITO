#ifndef WXWIIMOTEEVENT_H
#define WXWIIMOTEEVENT_H

#include "wx/wx.h"

DECLARE_EVENT_TYPE(wxEVT_WII_MOTION_IR, wxID_ANY) 
#define EVT_WII_MOTION_IR(fn)	DECLARE_EVENT_TABLE_ENTRY(wxEVT_WII_MOTION_IR, wxID_ANY, wxID_ANY, \
 														(wxObjectEventFunction)(wxEventFunction)&fn, \
 														(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_A_DOWN, wxID_ANY)
#define EVT_WII_A_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_A_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_A_UP, wxID_ANY)
#define EVT_WII_A_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_A_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_B_DOWN, wxID_ANY)
#define EVT_WII_B_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_B_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_B_UP, wxID_ANY)
#define EVT_WII_B_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_B_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_MINUS_DOWN, wxID_ANY)
#define EVT_WII_MINUS_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_MINUS_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_MINUS_UP, wxID_ANY)
#define EVT_WII_MINUS_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_MINUS_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_HOME_DOWN, wxID_ANY)
#define EVT_WII_HOME_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_HOME_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_HOME_UP, wxID_ANY)
#define EVT_WII_HOME_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_HOME_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),
DECLARE_EVENT_TYPE(wxEVT_WII_PLUS_DOWN, wxID_ANY)
#define EVT_WII_PLUS_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_PLUS_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_PLUS_UP, wxID_ANY)
#define EVT_WII_PLUS_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_PLUS_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_ONE_DOWN, wxID_ANY)
#define EVT_WII_ONE_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_ONE_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_ONE_UP, wxID_ANY)
#define EVT_WII_ONE_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_ONE_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),
DECLARE_EVENT_TYPE(wxEVT_WII_TWO_DOWN, wxID_ANY)
#define EVT_WII_TWO_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_TWO_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_TWO_UP, wxID_ANY)
#define EVT_WII_TWO_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_TWO_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),


DECLARE_EVENT_TYPE(wxEVT_WII_CROSS_DOWN, wxID_ANY)
#define EVT_WII_CROSS_DOWN(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_CROSS_DOWN, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),

DECLARE_EVENT_TYPE(wxEVT_WII_CROSS_UP, wxID_ANY)
#define EVT_WII_CROSS_UP(fn)	 DECLARE_EVENT_TABLE_ENTRY( wxEVT_WII_CROSS_UP, wxID_ANY, wxID_ANY, \
 															(wxObjectEventFunction)(wxEventFunction)&fn, \
 															(wxObject *) NULL ),



class wxWiimoteEvent : public wxEvent
{    
public:
	wxWiimoteEvent();
    wxWiimoteEvent(const wxWiimoteEvent &event);
	wxWiimoteEvent(wxEventType type);
    
	// virtual wxWiimoteEvent *Clone() const {return new wxWiimoteEvent(*this);}
	wxWiimoteEvent *Clone() const {return new wxWiimoteEvent(*this);}

	// These will be true if they are currently down, independent of the current event type..
	bool A, B, Plus, Minus, Home, One, Two;
	bool LeftCross, RightCross, UpCross, DownCross;

	// Wiimote Motion
	float WiiX, WiiY, WiiZ;			

	// IR Lights Position, captured from Wiimote IR Camera 
	int IRLight1X, IRLight1Y, IRLight2X, IRLight2Y;		
	bool IRLight1Found, IRLight2Found;

	bool Led1, Led2, Led3, Led4;

	int idWii;
	
	DECLARE_DYNAMIC_CLASS(wxWiimoteEvent)
 };

#endif
