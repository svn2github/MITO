#include "wxMotionFactorDialog.h"
#include <math.h>

wxMotionFactorDialog::wxMotionFactorDialog( wxWindow *parent, wxWindowID id, float mFactor, const wxString &title, 
								  const wxPoint& position, const wxSize& size, long style) 
								  : wxDialog( parent, id, title, position, size, style) 
{    

	_mFactor = mFactor;

	wxStaticText* text = new wxStaticText(this,-1,"Motion Factor = ");

	int  pDecimale = ( ceil(mFactor)-mFactor == 0 ) ? 0 : (int)((1+mFactor-ceil(mFactor))*100+0.5);
	int   pIntera = (pDecimale==0) ? mFactor : ceil(mFactor)-1;
	_mFactor = pIntera + pDecimale/100.0;

	_textCtrl = new wxTextCtrl(this,-1,wxString::Format(wxT("%d"),(int)pIntera) + "." + wxString::Format(wxT("%d"),(int)pDecimale),wxDefaultPosition,wxSize(52,17));
	_textCtrl->SetMaxLength(7);
	wxBoxSizer *mFactorSizer = new wxBoxSizer( wxHORIZONTAL );
	mFactorSizer->Add(text, 0, wxTOP, 20);
	mFactorSizer->Add(_textCtrl, 0, wxTOP, 18);
    	
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);	
	wxButton* okButton = new wxButton(this, wxID_OK, "Ok");							
	buttonSizer->Add(okButton, 0, wxALL, 5);	
	wxButton* escButton = new wxButton(this, wxID_CANCEL, "Cancel");
	buttonSizer->Add(escButton, 0, wxALL, 5);

	//Contenitore principale, raggruppante i contenitori precedenti.
	wxBoxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
	MainSizer->Add(mFactorSizer, 0, wxALL | wxALIGN_CENTER, 5);
	MainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 20);

	this->SetSizerAndFit(MainSizer);		
	this->Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}


float wxMotionFactorDialog::getMFactor() {
	double value;	
	if(_textCtrl->GetValue().ToDouble(&value)) 
		return value;
	else
		return _mFactor;
}

