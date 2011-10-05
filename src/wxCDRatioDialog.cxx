#include "wxCDRatioDialog.h"
#include <math.h>

enum {
	idCMinSpin = wxID_HIGHEST +1,
	idCMaxSpin,
};

BEGIN_EVENT_TABLE (wxCDRatioDialog, wxDialog)
	EVT_SPIN_UP(idCMinSpin, wxCDRatioDialog::onCMinSpinUp)
	EVT_SPIN_DOWN(idCMinSpin, wxCDRatioDialog::onCMinSpinDown)
	EVT_SPIN_UP(idCMaxSpin, wxCDRatioDialog::onCMaxSpinUp)
	EVT_SPIN_DOWN(idCMaxSpin, wxCDRatioDialog::onCMaxSpinDown)
END_EVENT_TABLE()

wxCDRatioDialog::wxCDRatioDialog( wxWindow *parent, wxWindowID id, float Cmin, float Cmax, const wxString &title, 
								  const wxPoint& position, const wxSize& size, long style) 
								  : wxDialog( parent, id, title, position, size, style) 
{    

	_Cmin = (int)(Cmin*1000+0.5);		// 0.5 è aggiunto per arrotondare all'intero più vicino
	_Cmax = (int)(Cmax*1000+0.5);		// 0.5 è aggiunto per arrotondare all'intero più vicino

	wxStaticText* minText1 = new wxStaticText(this,-1,"minDISP = ");
	_minTextCtrl = new wxTextCtrl(this,-1,"0.005",wxDefaultPosition,wxSize(42,17));
	_minTextCtrl->SetMaxLength(5);
	updateTextCtrl(_minTextCtrl,_Cmin);
	wxSpinButton* CminSpin = new wxSpinButton(this,idCMinSpin,wxDefaultPosition,wxSize(17,18));
	wxStaticText* minText2 = new wxStaticText(this,-1,"* ddMAX");
	wxBoxSizer *minSizer = new wxBoxSizer( wxHORIZONTAL );
	minSizer->Add(minText1, 0, wxLEFT | wxTOP, 10);
	minSizer->Add(_minTextCtrl, 0, wxTOP, 8);
	minSizer->Add(CminSpin, 0, wxTOP | wxRIGHT, 8);
	minSizer->Add(minText2, 0, wxTOP, 10);


	wxStaticText* maxText1 = new wxStaticText(this,-1,"maxDISP = ");
	_maxTextCtrl = new wxTextCtrl(this,-1,"0.750",wxDefaultPosition,wxSize(42,17));
	_maxTextCtrl->SetMaxLength(5);
	updateTextCtrl(_maxTextCtrl,_Cmax);
	wxSpinButton* CmaxSpin = new wxSpinButton(this,idCMaxSpin,wxDefaultPosition,wxSize(17,18));
	wxStaticText* maxText2 = new wxStaticText(this,-1,"* ddMAX");
	wxBoxSizer *maxSizer = new wxBoxSizer( wxHORIZONTAL );
	maxSizer->Add(maxText1, 0, wxLEFT | wxTOP, 7);
	maxSizer->Add(_maxTextCtrl, 0, wxTOP, 6);
	maxSizer->Add(CmaxSpin, 0, wxTOP | wxRIGHT,6);
	maxSizer->Add(maxText2, 0, wxTOP, 7);

	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add(minSizer, 0, wxALL, 5);
	topSizer->Add(maxSizer, 0, wxALL, 5);
    
	
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);	
	wxButton* okButton = new wxButton(this, wxID_OK, "Ok");							
	buttonSizer->Add(okButton, 0, wxALL, 5);	
	wxButton* escButton = new wxButton(this, wxID_CANCEL, "Cancel");
	buttonSizer->Add(escButton, 0, wxALL, 5);

	//Contenitore principale, raggruppante i contenitori precedenti.
	wxBoxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
	MainSizer->Add(topSizer, 0, wxALL | wxALIGN_CENTER, 5);
	MainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 20);

	this->SetSizerAndFit(MainSizer);		
	this->Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

int wxCDRatioDialog::TextCtrlValueToInt(wxTextCtrl* textCtrl) {
	double value;	
	if(textCtrl->GetValue().ToDouble(&value)) {
		int temp = ceil(value*1000);
		if (temp>1000) temp = 1000;
		else if (temp<0) temp = 0;
		return temp;
	}
	return 0;
}


void wxCDRatioDialog::updateTextCtrl(wxTextCtrl* textCtrl, int n) {
	if (n<10)
		textCtrl->SetValue(wxString("0.00")+wxString::Format(wxT("%d"), (int)n));
	else if (n<100)		
		textCtrl->SetValue(wxString("0.0")+wxString::Format(wxT("%d"), (int)n));
	else if (n<1000)
		textCtrl->SetValue(wxString("0.")+wxString::Format(wxT("%d"), (int)n));
	else 
		textCtrl->SetValue(wxString("1.000"));
}

void wxCDRatioDialog::onCMinSpinUp(wxSpinEvent & event) {	
	_Cmin = TextCtrlValueToInt(_minTextCtrl);
	_Cmin = (_Cmin<1000) ? (_Cmin+1) : _Cmin;			
	updateTextCtrl(_minTextCtrl, _Cmin);	
}

void wxCDRatioDialog::onCMinSpinDown(wxSpinEvent & event) {	
	_Cmin = TextCtrlValueToInt(_minTextCtrl);
	_Cmin = (_Cmin>0) ? (_Cmin-1) : 0;	
	updateTextCtrl(_minTextCtrl, _Cmin);
}

void wxCDRatioDialog::onCMaxSpinUp(wxSpinEvent & event) {
	_Cmax = TextCtrlValueToInt(_maxTextCtrl);
	_Cmax = (_Cmax<1000) ? (_Cmax+1) : _Cmax;	
	updateTextCtrl(_maxTextCtrl, _Cmax);
}

void wxCDRatioDialog::onCMaxSpinDown(wxSpinEvent & event) {
	_Cmax = TextCtrlValueToInt(_maxTextCtrl);
	_Cmax = (_Cmax>0) ? (_Cmax-1) : _Cmax;	
	updateTextCtrl(_maxTextCtrl, _Cmax);
}

float wxCDRatioDialog::getCMin() {
	_Cmin = TextCtrlValueToInt(_minTextCtrl);
	return (((float)_Cmin)/1000.0);
}

float wxCDRatioDialog::getCMax() {
	_Cmax = TextCtrlValueToInt(_maxTextCtrl);
	return (((float)_Cmax)/1000.0);
}