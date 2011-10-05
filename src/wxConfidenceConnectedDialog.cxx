/**
 * \file wxConfidenceConnectedDialog.cxx
 * \brief File per la creazione del dialog per la segmentazione secondo la tecnica confidence connected
 * \author ICAR-CNR Napoli
 */

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "wxConfidenceConnectedDialog.h"


wxConfidenceConnectedDialog::wxConfidenceConnectedDialog(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style) {
    Create(parent, id, caption, pos, size, style);
}

bool wxConfidenceConnectedDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );
	CreateControls();
    Centre();

    return true;
}

void wxConfidenceConnectedDialog::CreateControls() {    
	

	wxStaticText* mainText = new wxStaticText( this, wxID_STATIC, _("&Parameters"), wxPoint(15,10), wxSize(160,20), 0  );
	wxPanel* upperPanel = new wxPanel( this, -1, wxPoint(15,30), wxSize(320, 100), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	wxStaticText* upperPanelText = new wxStaticText( upperPanel, wxID_STATIC, _("&Starting Point:"), wxPoint(10,10), wxSize(70,23), 0  );

	wxStaticText* seedXText = new wxStaticText( upperPanel, wxID_STATIC, _("&Coordinate X:"), wxPoint(10,40), wxSize(80,23), 0  );
	_seedX = new wxTextCtrl( upperPanel, id_ConfidenceConnected_seedX, _T(""), wxPoint(100,40), wxSize(55,20), 0 );
	_seedX->SetEditable(false);

	wxStaticText* seedYText = new wxStaticText( upperPanel, wxID_STATIC, _("&Coordinate Y:"), wxPoint(165,40), wxSize(80,23), 0  );
	_seedY = new wxTextCtrl( upperPanel, id_ConfidenceConnected_seedY, _T(""),  wxPoint(255,40), wxSize(55,20), 0 );
	_seedY->SetEditable(false);

	wxStaticText* seedValueText = new wxStaticText( upperPanel, wxID_STATIC, _("&Pixel Value:"), wxPoint(10,70), wxSize(80,23), 0  );
	_seedValue = new wxTextCtrl( upperPanel, id_ConfidenceConnected_seedValue, _T(""), wxPoint(100,70), wxSize(55,20), 0 );
	_seedValue->SetEditable(false);

	wxPanel* middlePanel = new wxPanel( this, -1, wxPoint(15,143), wxSize(320, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	wxStaticText* middlePanelText = new wxStaticText( middlePanel, wxID_STATIC, _("&Insert the multiplier value and the number of iterations:"), wxPoint(10,10), wxSize(300,23), 0  );

	wxStaticText* multiplierText = new wxStaticText( middlePanel, wxID_STATIC, _("&Multiplier value:"), wxPoint(10,40),wxSize(85,23), 0  );
	_multiplier = new wxTextCtrl( middlePanel, id_ConfidenceConnected_multiplier, _T("2"), wxPoint(100,40), wxSize(55,20), 0 );
	
	wxStaticText* iterationsText = new wxStaticText( middlePanel, wxID_STATIC, _("&Iterations:"), wxPoint(165,40), wxSize(85,23), 0  );
	_iterations= new wxTextCtrl( middlePanel, id_ConfidenceConnected_iterations, _T("5"), wxPoint(255,40), wxSize(55,20), 0 );

	wxPanel* lowerPanel = new wxPanel( this, -1, wxPoint(15,233), wxSize(320, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	wxStaticText* lowerPanelText = new wxStaticText( lowerPanel, wxID_STATIC, _("&Insert the radius dimension (one value for both the axes):"), wxPoint(10,10), wxSize(300,23), 0  );

	wxStaticText* radiusText = new wxStaticText( lowerPanel, wxID_STATIC, _("&Dimension:"), wxPoint(10,40),wxSize(85,23), 0  );
	_radius= new wxTextCtrl( lowerPanel, id_ConfidenceConnected_radius, _T("1"),  wxPoint(100,40), wxSize(55,20), 0 );

	wxStdDialogButtonSizer* itemStdDialogButtonSizer = new wxStdDialogButtonSizer;

	SetSizer(itemStdDialogButtonSizer);
	
	itemStdDialogButtonSizer->Realize();

	wxButton* buttonOk = new wxButton( this, wxID_OK, _("&Compute"), wxPoint(90,340), wxDefaultSize, 0 );
    itemStdDialogButtonSizer->AddButton(buttonOk);

    wxButton* buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxPoint(185,340), wxDefaultSize, 0 );
    itemStdDialogButtonSizer->AddButton(buttonCancel);}

void wxConfidenceConnectedDialog::InitializeDialog(double seedX, double seedY, short seedValue){
	char seedXText[10];
	sprintf(seedXText,"%g",seedX);
	_seedX->SetValue(seedXText);

	char seedYText[10];
	sprintf(seedYText,"%g",seedY);
	_seedY->SetValue(seedYText);

	char seedValueText[10];
	sprintf(seedValueText,"%hd",seedValue);
	_seedValue->SetValue(seedValueText);
}


float wxConfidenceConnectedDialog::GetRadius(){
	return atof(_radius->GetValue().c_str());
}

float wxConfidenceConnectedDialog::GetMultiplier(){
	return atof(_multiplier->GetValue().c_str());
}

int wxConfidenceConnectedDialog::GetIterations(){
	return atoi(_iterations->GetValue().c_str());
}
