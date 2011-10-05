/**
 * \file wxEventCatcher.cxx
 * \brief File per la cattura degli eventi
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "wxEventCatcher.h" 

// settare a true / false per abilitare / disabilitare la scrittura su log
#define LOG_ACTIVATION false

BEGIN_EVENT_TABLE(wxEventCatcher, wxEvtHandler)
	EVT_ACTIVATE(wxEventCatcher::onActivate)
	EVT_CLOSE (wxEventCatcher::onQuit)
	EVT_MOUSEWHEEL(wxEventCatcher::onMouseWheel)
	EVT_LEFT_DOWN (wxEventCatcher::onButtonDown)
	EVT_MOTION (wxEventCatcher::onMotion)	
	EVT_LEAVE_WINDOW(wxEventCatcher::onLeaveWindow)
	EVT_ENTER_WINDOW(wxEventCatcher::onEnterWindow)

	//Wiimote Events
	EVT_WII_MOTION_IR(wxEventCatcher::onWiiMotionIR)
	EVT_WII_A_DOWN(wxEventCatcher::onWiiADown)
	EVT_WII_A_UP(wxEventCatcher::onWiiAUp)
	EVT_WII_B_DOWN(wxEventCatcher::onWiiBDown)
	EVT_WII_B_UP(wxEventCatcher::onWiiBUp)
	EVT_WII_MINUS_DOWN(wxEventCatcher::onWiiMinusDown)
	EVT_WII_MINUS_UP(wxEventCatcher::onWiiMinusUp)
	EVT_WII_HOME_DOWN(wxEventCatcher::onWiiHomeDown)
	EVT_WII_HOME_UP(wxEventCatcher::onWiiHomeUp)
	EVT_WII_PLUS_DOWN(wxEventCatcher::onWiiPlusDown)
	EVT_WII_PLUS_UP(wxEventCatcher::onWiiPlusUp)
	EVT_WII_ONE_DOWN(wxEventCatcher::onWiiOneDown)
	EVT_WII_ONE_UP(wxEventCatcher::onWiiOneUp)
	EVT_WII_TWO_DOWN(wxEventCatcher::onWiiTwoDown)
	EVT_WII_TWO_UP(wxEventCatcher::onWiiTwoUp)
	EVT_WII_CROSS_DOWN(wxEventCatcher::onWiiCrossDown)
	EVT_WII_CROSS_UP(wxEventCatcher::onWiiCrossUp)

END_EVENT_TABLE()

wxEventCatcher::wxEventCatcher(unsigned int idViewer, wxWindow* mainGui) : wxEvtHandler() {
	_idViewer = idViewer;
	_mainGui = (wxMainGui*)mainGui;

	for (int i=0; i<4; i++)
		lastWiimoteTimestamp[i] = 0;

	QueryPerformanceFrequency((LARGE_INTEGER *)&_clockFrequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&_lastUpdatedTime);
}

wxEventCatcher::~wxEventCatcher() {
}

void wxEventCatcher::onActivate(wxActivateEvent& event) {	
	if (_mainGui->onViewerActivate(_idViewer))
		event.Skip();
}

void wxEventCatcher::onQuit(wxCloseEvent& event) {	
}

void wxEventCatcher::onMouseWheel(wxMouseEvent &event) {
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	((appWxVtkInteractor*)viewer3d->getWxWindow())->OnMouseWheel(event);
}

void wxEventCatcher::onButtonDown(wxMouseEvent &event) {
	
	/* prova by Nello
	wxMouseEvent e;
	e.SetEventObject(this);
	e.SetEventType(wxEVT_MOUSEWHEEL);
	this->AddPendingEvent(e);
	*/

	if (_mainGui->onButtonDown(_idViewer, event))
		event.Skip();
}

void wxEventCatcher::onMotion(wxMouseEvent &event) {
	if (_mainGui->onMotion(_idViewer, event))
		event.Skip();	
}

void wxEventCatcher::onLeaveWindow(wxMouseEvent &event) {	
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	((appWxVtkInteractor*)viewer3d->getWxWindow())->OnLeaveWindow(event);
}

void wxEventCatcher::onEnterWindow(wxMouseEvent &event) {	
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	((appWxVtkInteractor*)viewer3d->getWxWindow())->OnEnterWindow(event);
}

void wxEventCatcher::onWiiMotionIR( wxWiimoteEvent & event) 
{
	{
	#ifdef LOG_ACTIVATION
		// log new
		__int64 currentTime;
		if (QueryPerformanceCounter((LARGE_INTEGER *)&currentTime)!= 0) {
			char buffer[64];
			string r0, r2, r3;
			r0 = _itoa(event.idWii, buffer, 10);
			r2 = _i64toa(( (currentTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
			r3 = _i64toa( ((currentTime*1000/_clockFrequency) - event.GetTimestamp()), buffer, 10); // milliseconds
			string strDatanew = "idWii: " + r0 + 
								"\tDelay(last instr): " + r2 + " mcs" + 
								"\tDelay(event): " + r3 + 
								" ms\tEntrato nel motionIR";
			list.push_back(strDatanew);
			_lastUpdatedTime = currentTime;
		}
		// fine log new
	#endif
	}

	__int64 currentTime;
	// non inoltra se vecchio o se a freq maggiore di 10 ms
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	if ( ( ( (currentTime * 1000 / _clockFrequency) - (__int64)event.GetTimestamp()) < 10) && ( ((__int64)event.GetTimestamp() - lastWiimoteTimestamp[event.idWii]) > 10 ) )
	{
		lastWiimoteTimestamp[event.idWii] = event.GetTimestamp();

		wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
		appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

		if (appWxVI->WiiManagerStarted) appWxVI->onWiiMotionIR(event);

		{
		#ifdef LOG_ACTIVATION
			// log new
			__int64 currentTime;
			if (QueryPerformanceCounter((LARGE_INTEGER *)&currentTime)!= 0) {
				char buffer[64];
				string r0, r2, r3;
				r0 = _itoa(event.idWii, buffer, 10);
				r2 = _i64toa(( (currentTime - _lastUpdatedTime) * 1000000 / _clockFrequency ), buffer, 10); // microseconds
				r3 = _i64toa( currentTime*1000/_clockFrequency - event.GetTimestamp(), buffer, 10); // milliseconds
				string strDatanew = "idWii: " + r0 + 
									"\tDelay(last instr): " + r2 + " mcs" + 
									"\tDelay(event): " + r3 + 
									" ms\tEvento inoltrato";
				list.push_back(strDatanew);
				_lastUpdatedTime = currentTime;
			}
			// fine log new			
		#endif
		}
	}
}

void wxEventCatcher::onWiiADown( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiADown(event);
}

void wxEventCatcher::onWiiAUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	
	
	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiAUp(event);
}

void wxEventCatcher::onWiiBDown( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiBDown(event);
}

void wxEventCatcher::onWiiBUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiBUp(event);
}

void wxEventCatcher::onWiiMinusDown( wxWiimoteEvent & event) 
{/*
	// non inoltro l'evento se vecchio
	SYSTEMTIME st;
	GetSystemTime(&st);
	int sTime = st.wSecond*1000+st.wMilliseconds;
	if ( abs((event.GetTimestamp()-sTime)) > 10 ) return;
*/
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiMinusDown(event);
}

void wxEventCatcher::onWiiMinusUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	
	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiMinusUp(event);
}

void wxEventCatcher::onWiiHomeDown( wxWiimoteEvent & event) 
{
	{
	#ifdef LOG_ACTIVATION
		// scrittura log su file
		// per il log - tipo file di log
		ofstream wiilog;
		wiilog.open("eventCatcherWiilog.txt", ios::app);
		for(int i=0;i < list.size(); i++)
		{
			std::string strd = list.at(i);
			wiilog << strd.c_str()<<endl;            			
		}
		// fine scrittura log su file
		wiilog.close();
	#endif
	}

	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiHomeDown(event);
}

void wxEventCatcher::onWiiHomeUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiHomeUp(event);
}

void wxEventCatcher::onWiiPlusDown( wxWiimoteEvent & event) 
{/*
	// non inoltro l'evento se vecchio
	SYSTEMTIME st;
	GetSystemTime(&st);
	int sTime = st.wSecond*1000+st.wMilliseconds;
	if ( abs((event.GetTimestamp()-sTime)) > 10 ) return;
*/
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiPlusDown(event);
}

void wxEventCatcher::onWiiPlusUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	
	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiPlusUp(event);
}

void wxEventCatcher::onWiiOneDown( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiOneDown(event);
}

void wxEventCatcher::onWiiOneUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	
	
	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiOneUp(event);
}

void wxEventCatcher::onWiiTwoDown( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiTwoDown(event);
}

void wxEventCatcher::onWiiTwoUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiTwoUp(event);
}

void wxEventCatcher::onWiiCrossDown( wxWiimoteEvent & event) 
{
	// non inoltra se vecchio o se a freq maggiore di 10 ms
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	if ( ( ( (currentTime * 1000 / _clockFrequency) - (__int64)event.GetTimestamp()) < 10) && ( ((__int64)event.GetTimestamp() - lastWiimoteTimestamp[event.idWii]) > 10 ) )
	{
		lastWiimoteTimestamp[event.idWii] = event.GetTimestamp();

		wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
		appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

		if (appWxVI->WiiManagerStarted)		appWxVI->onWiiCrossDown(event);
	}
}

void wxEventCatcher::onWiiCrossUp( wxWiimoteEvent & event) 
{
	wxVtkViewer3d* viewer3d = ( (wxVtkViewer3d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()) );
	appWxVtkInteractor* appWxVI = ((appWxVtkInteractor*)viewer3d->getWxWindow());	

	if (appWxVI->WiiManagerStarted)	appWxVI->onWiiCrossUp(event);
}