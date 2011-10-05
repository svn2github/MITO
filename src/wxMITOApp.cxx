/**
 * \file wxMITOApp.cxx
 * \brief File per la creazione dell'applicazione MITO
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "wxMITOApp.h"
#include "wxMainGui.h"
#include <wx/gdicmn.h>

IMPLEMENT_APP(wxMITOApp)

wxMITOApp::wxMITOApp() {
}

wxMITOApp::~wxMITOApp() {
}

bool wxMITOApp::OnInit() {	
	wxMainGui* mainGui = new wxMainGui(_T("MITO"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());//wxGetDisplaySize());
	mainGui->Maximize(true);
	mainGui->Show(TRUE);
	SetTopWindow(mainGui);
    
	return true;
}

int wxMITOApp::OnExit()
{    
    return wxApp::OnExit();
}