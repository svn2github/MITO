/**
 * \file wxFusionDialog.cxx
 * \brief File per la creazione del dialog per la selezione dei parametri per la fusione di immaigni
 * \author ICAR-CNR Napoli
 */


#include "wxFusionDialog.h"
#include <wx/stattext.h>
#include <stdio.h>

IMPLEMENT_CLASS(wxFusionDialog, wxDialog)

BEGIN_EVENT_TABLE(wxFusionDialog, wxDialog)
	EVT_BUTTON (setIdData1_button, wxFusionDialog::onSetIdData1)
	EVT_BUTTON (setIdData2_button, wxFusionDialog::onSetIdData2)
	EVT_BUTTON (ok_buttonFusion, wxFusionDialog::onOkDown)
	EVT_BUTTON (cancel_buttonFusion, wxFusionDialog::onCancelDown)
	EVT_COMMAND_SCROLL(weight_slider, wxFusionDialog::onScroll)
	EVT_RADIOBOX(fusionType_radioBox, wxFusionDialog::onFusionType)
	EVT_COMBOBOX(choiceRule, wxFusionDialog::onFusionRule)
	EVT_CHOICE(waveletTypeChoice, wxFusionDialog::onWaveletType)
	EVT_CHOICE(levelChoice, wxFusionDialog::onLivelloDec)

END_EVENT_TABLE()

const wxString algoTypeArray [] = {"Aritmethic (faster)", "Wavelet (slower)"};


wxFusionDialog::wxFusionDialog(wxWindow* win) : wxDialog(win, -1, "Image Fusion Settings", wxDefaultPosition, wxSize(770, 480)) {
	_fusionType = new wxRadioBox(this, fusionType_radioBox, "Fusion algorithm", wxPoint(10, 10), wxDefaultSize, 2, algoTypeArray, 0, wxRA_SPECIFY_COLS);
	_listIdData = new wxListBox(this, -1, wxPoint(10, 80), wxSize(200, 200), 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB);
	
	wxArrayString rules;
	//rules.Add("Complete Detail Substitution");
	rules.Add("Mean/Modulus Max");
	rules.Add("Mean/Modulus Max + Consistency Verification");
	//rules.Add("Variance/Modulus Max + Consistency Verification");
	
	wxArrayString waveletType;
	waveletType.Add("haar");
	waveletType.Add("db4");
	waveletType.Add("db8");
	waveletType.Add("db20");

	wxArrayString levels;
	levels.Add("1");
	levels.Add("2");
	levels.Add("3");
	levels.Add("4");
	
	_levelChoice = new wxChoice(this, levelChoice, wxPoint(600, 200), wxSize(50, 50), levels, 0,wxDefaultValidator,"Decomposition Level" );
	wxStaticText * labelLevel = new wxStaticText(this, -1, "Select decomposition level", wxPoint(600, 180), wxDefaultSize); 
	_levelChoice->SetSelection(0);

	wxStaticText * label1 = new wxStaticText(this, -1, "Select CT image/series", wxPoint(220, 80), wxDefaultSize); 
	wxButton* _setIdData1 = new wxButton(this, setIdData1_button, ">", wxPoint(220, 100), wxSize(30, 20));
	_textIdData1 = new wxTextCtrl(this, -1, "", wxPoint(260, 100), wxSize(200, 20));

	wxStaticText * label2 = new wxStaticText(this, -1, "Select PET image/series", wxPoint(220, 180), wxDefaultSize);
	wxButton* _setIdData2 = new wxButton(this, setIdData2_button, ">", wxPoint(220, 200), wxSize(30, 20));
	_textIdData2 = new wxTextCtrl(this, -1, "", wxPoint(260, 200), wxSize(200, 20));
	_checkCLUT = new wxCheckBox(this, -1, "Apply CLUT", wxPoint(260, 230), wxDefaultSize);

	_choiceRule = new wxComboBox(this, choiceRule,"", wxPoint(500, 100), wxSize(250, 50), rules, wxCB_READONLY,wxDefaultValidator,"Fusion Rule" );
	_choiceRule->SetSelection(fusionFilter::Consistenza_media);
	wxStaticText * labelRules = new wxStaticText(this, -1, "Select fusion rule", wxPoint(500, 80), wxDefaultSize);
	_ruleType = fusionFilter::Consistenza_media;

	_waveletTypeChoice = new wxChoice(this, waveletTypeChoice, wxPoint(500, 200), wxSize(50, 50), waveletType, 0,wxDefaultValidator,"Wavelet Type" );
	_waveletTypeChoice->SetSelection(2);
	wxStaticText * waveletTypeText = new wxStaticText(this, -1, "Select Wavelet", wxPoint(500, 180), wxDefaultSize); 
	 _waveletType = 2;

	wxStaticText * label3 = new wxStaticText(this, -1, "Weight first image/series", wxPoint(130, 300));
	wxStaticText * label4 = new wxStaticText(this, -1, "Weight second image/series", wxPoint(505, 300));

	_pesoData1 = new wxTextCtrl(this, -1, "0.5", wxPoint(165, 320), wxSize(50, 20), wxTE_READONLY);
	_slideWeight = new wxSlider(this, weight_slider, 50, 1, 99, wxPoint(225, 320), wxSize(300, 20), wxSL_HORIZONTAL | wxSL_AUTOTICKS);
	_slideWeight->SetTickFreq(2,1);
	_pesoData2 = new wxTextCtrl(this, -1, "0.5", wxPoint(535, 320), wxSize(50, 20), wxTE_READONLY);

	wxButton * ok = new wxButton(this, ok_buttonFusion, "OK", wxPoint(215, 385), wxSize(150, 30));
	wxButton * cancel = new wxButton(this, cancel_buttonFusion, "Cancel", wxPoint(385, 385), wxSize(150, 30));

	onScroll(wxScrollEvent());
	onFusionType(wxCommandEvent());
}

void wxFusionDialog::onSetIdData1( wxCommandEvent& event ) {
	_idSel1 = _listIdData->GetSelection();
	_textIdData1->SetValue(_listIdData->GetStringSelection());
}

void wxFusionDialog::onSetIdData2( wxCommandEvent& event ) {
	_idSel2 = _listIdData->GetSelection();
	_textIdData2->SetValue(_listIdData->GetStringSelection());
}


void wxFusionDialog::onFusionRule( wxCommandEvent& event ) {
	int rul = _choiceRule->GetSelection();
	switch (rul) {
		/*case 0:
			_ruleType = fusionFilter::Sostituzione_dettaglio;
			//_levelChoice->Enable();
			break;
		case 1:
			_ruleType = fusionFilter::Modulo_max;
			//_levelChoice->SetSelection(0);
			//_levelChoice->Disable();
			
			break;
		case 2:
			_ruleType = fusionFilter::Consistenza_media;
			//_levelChoice->SetSelection(0);
			//_levelChoice->Disable();
			break;
		case 3:
			_ruleType = fusionFilter::Consistenza_var;
			//_levelChoice->SetSelection(0);
			//_levelChoice->Disable();
			break;
		*/
		
		case 0:
			_ruleType = fusionFilter::Modulo_max;
			//_levelChoice->SetSelection(0);
			//_levelChoice->Disable();
			
			break;
		case 1:
			_ruleType = fusionFilter::Consistenza_media;
			//_levelChoice->SetSelection(0);
			//_levelChoice->Disable();
			break;
	}	
}

void wxFusionDialog::onLivelloDec( wxCommandEvent& event ) {
	_level = _levelChoice->GetSelection();
	
}


void wxFusionDialog::onWaveletType( wxCommandEvent& event ) {
	 _waveletType = _waveletTypeChoice->GetSelection();
	
}
void wxFusionDialog::onOkDown( wxCommandEvent& event ) {
	if(strcmp(_textIdData1->GetValue().c_str(),"") && strcmp(_textIdData2->GetValue().c_str(),"")) {
		_peso1 = atof(_pesoData1->GetValue());
		_peso2 = atof(_pesoData2->GetValue());
		SetReturnCode(wxID_OK);
		Destroy();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Select images to fuse"), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wxFusionDialog::onCancelDown( wxCommandEvent& event ) {
	SetReturnCode(wxID_CANCEL);
	Destroy();
}

void wxFusionDialog::onScroll(wxScrollEvent& event) {
	int _peso = _slideWeight->GetValue();
	wxString & a = wxString();
	wxString & b = wxString();

	if (_peso == 0)
		a << "0.01";
	else if (_peso == 100)
		a << "0.99";
	else if (_peso < 10)
		a << "0.0" << _peso;
	else
		a << "0." << _peso;

	if (_peso == 100)
		b << "0.01";
	else if (_peso == 0)
		b << "0.99";
	else if (100-_peso < 10)
		b << "0.0" << 100-_peso;
	else
		b << "0." << 100-_peso;

	_pesoData1->SetValue(b);
	_pesoData2->SetValue(a);
}


void wxFusionDialog::onFusionType(wxCommandEvent& event) {
	int index = _fusionType->GetSelection();
	switch (index) {
		case 0:
			_algoType = fusionFilter::media_pesata;
			_choiceRule->Disable();
			_waveletTypeChoice->Disable();
			_levelChoice->Disable();

			break;
		case 1:
			_algoType = fusionFilter::wavelet;
			_choiceRule->Enable();
			_waveletTypeChoice->Enable();
			_levelChoice->Enable();

			break;
	}
}