/**
 * \file wxConnectedThresholdDialog.cxx
 * \brief File per la creazione del dialog per la segmentazione secondo la tecnica connected threshold
 * \author ICAR-CNR Napoli
 */

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "wxConnectedThresholdDialog.h"


wxConnectedThresholdDialog::wxConnectedThresholdDialog(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style) {
    Create(parent, id, caption, pos, size, style);
}

bool wxConnectedThresholdDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );
	CreateControls();
    Centre();

    return true;
}

void wxConnectedThresholdDialog::CreateControls() {    

	wxStaticText* mainText = new wxStaticText( this, wxID_STATIC, _("&Parameters"), wxPoint(15,10), wxSize(160,20), 0  );
	wxPanel* upperPanel = new wxPanel( this, -1, wxPoint(15,30), wxSize(320, 100), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	wxStaticText* upperPanelText = new wxStaticText( upperPanel, wxID_STATIC, _("&Starting Point:"), wxPoint(10,10), wxSize(70,23), 0  );

	wxStaticText* seedXText = new wxStaticText( upperPanel, wxID_STATIC, _("&Coordinate X:"), wxPoint(10,40), wxSize(80,23), 0  );
	_seedX = new wxTextCtrl( upperPanel, id_ConnectedThreshold_seedX, _T(""), wxPoint(100,40), wxSize(55,20), 0 );
	_seedX->SetEditable(false);

	wxStaticText* seedYText = new wxStaticText( upperPanel, wxID_STATIC, _("&Coordinate Y:"), wxPoint(165,40), wxSize(80,23), 0  );
	_seedY = new wxTextCtrl( upperPanel, id_ConnectedThreshold_seedY, _T(""),  wxPoint(255,40), wxSize(55,20), 0 );
	_seedY->SetEditable(false);

	wxStaticText* seedValueText = new wxStaticText( upperPanel, wxID_STATIC, _("&Pixel Value:"), wxPoint(10,70), wxSize(80,23), 0  );
	_seedValue = new wxTextCtrl( upperPanel, id_ConnectedThreshold_seedValue, _T(""), wxPoint(100,70), wxSize(55,20), 0 );
	_seedValue->SetEditable(false);

	wxPanel* lowerPanel = new wxPanel( this, -1, wxPoint(15,143), wxSize(320, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	wxStaticText* lowerPanelText = new wxStaticText( lowerPanel, wxID_STATIC, _("&Insert the Lower and Upper Thresholds:"), wxPoint(10,10), wxSize(260,23), 0  );

	wxStaticText* lowerThresholdText = new wxStaticText( lowerPanel, wxID_STATIC, _("&Lower Threshold:"), wxPoint(10,40),wxSize(85,23), 0  );
	_lowerThreshold = new wxTextCtrl( lowerPanel, id_ConnectedThreshold_lowerThreshold, _T("0"), wxPoint(100,40), wxSize(55,20), 0 );
	
	wxStaticText* upperThresholdText = new wxStaticText( lowerPanel, wxID_STATIC, _("&Upper Threshold:"), wxPoint(165,40), wxSize(85,23), 0  );
	_upperThreshold= new wxTextCtrl( lowerPanel, id_ConnectedThreshold_upperThreshold, _T("0"), wxPoint(255,40), wxSize(55,20), 0 );

	wxStdDialogButtonSizer* itemStdDialogButtonSizer = new wxStdDialogButtonSizer;

	SetSizer(itemStdDialogButtonSizer);
	
	itemStdDialogButtonSizer->Realize();

	wxButton* buttonOk = new wxButton( this, wxID_OK, _("&Compute"), wxPoint(90,240), wxDefaultSize, 0 );
    itemStdDialogButtonSizer->AddButton(buttonOk);

    wxButton* buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxPoint(185,240), wxDefaultSize, 0 );
    itemStdDialogButtonSizer->AddButton(buttonCancel);
}

void wxConnectedThresholdDialog::InitializeDialog(double seedX, double seedY, short seedValue){
	
	short lowerThreshold;
	short upperThreshold;
	if ((seedValue - 50)<-32768)
		lowerThreshold = -32768;
	else
		lowerThreshold = seedValue-50;
	if ((seedValue + 50)>32767)
		upperThreshold = 32767;
	else
		upperThreshold = seedValue + 50;
	
	char seedXText[10];
	sprintf(seedXText,"%g",seedX);
	_seedX->SetValue(seedXText);

	char seedYText[10];
	sprintf(seedYText,"%g",seedY);
	_seedY->SetValue(seedYText);

	char seedValueText[10];
	sprintf(seedValueText,"%hd",seedValue);
	_seedValue->SetValue(seedValueText);

	char lowerThresholdText[10];
	sprintf(lowerThresholdText,"%hd",lowerThreshold);
	_lowerThreshold->SetValue(lowerThresholdText);

	char upperThresholdText[10];
	sprintf(upperThresholdText,"%hd",upperThreshold);
	_upperThreshold->SetValue(upperThresholdText);

}

short wxConnectedThresholdDialog::GetLowerThreshold(){
	return (short)atoi(_lowerThreshold->GetValue().c_str());
}

short wxConnectedThresholdDialog::GetUpperThreshold(){
	return (short)atoi(_upperThreshold->GetValue().c_str());
}
