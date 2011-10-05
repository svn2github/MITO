/**
 * \file wxWLWWDialog.cxx
 * \brief File per la creazione del dialog per la modifica di window level e window width
 * \author ICAR-CNR Napoli
 */


#include "wxWLWWDialog.h"

IMPLEMENT_CLASS(wxWLWWDialog, wxDialog)

BEGIN_EVENT_TABLE(wxWLWWDialog, wxDialog)
	EVT_BUTTON (okButton, wxWLWWDialog::onOkDown)
	EVT_BUTTON (cancelButton, wxWLWWDialog::onCancelDown)
END_EVENT_TABLE()


wxWLWWDialog::wxWLWWDialog(wxWindow* window) : wxDialog(window, -1, "Modify WL/WW", wxDefaultPosition, wxSize(250, 220)) {
	wxStaticText *wlLabel = new wxStaticText(this, -1, "Set window level", wxPoint(40, 20), wxDefaultSize); 
	wxStaticText *wwLabel = new wxStaticText(this, -1, "Set window width", wxPoint(40, 70), wxDefaultSize); 

	_wxWL = new wxTextCtrl(this, -1, "", wxPoint(140, 20), wxSize(50, 20));
	_wxWW = new wxTextCtrl(this, -1, "", wxPoint(140, 70), wxSize(50, 20));

	wxButton *ok = new wxButton(this, okButton, "OK", wxPoint(30, 130), wxDefaultSize);
	wxButton *cancel = new wxButton(this, cancelButton, "Cancel", wxPoint(120, 130), wxDefaultSize);
}

void wxWLWWDialog::onOkDown(wxCommandEvent& event) {
	if(strcmp(_wxWL->GetValue().c_str(), "") && strcmp(_wxWW->GetValue().c_str(), "")) 
	{
		_wl = atof(_wxWL->GetValue());
		if (_wl < -9999) 
			_wl = -9999;
		else if (_wl > 9999) 
			_wl = 9999;

		_ww = atof(_wxWW->GetValue());
		if (_ww < 1) 
			_ww = 1;
		else if (_ww > 9999) 
			_ww = 9999;	

		SetReturnCode(wxID_OK);
		Destroy();
	}
	else {
		wxMessageDialog* message = new wxMessageDialog(this, _T("Set window level and window width"), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (message->ShowModal() == wxID_OK)
			message->Destroy();	
	}
}

void wxWLWWDialog::onCancelDown(wxCommandEvent& event) {
	SetReturnCode(wxID_CANCEL);
	Destroy();
}