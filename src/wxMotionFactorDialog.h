/**
 * \file wxMotionFactorDialog.h
 * \brief File per la creazione del dialog che permette di impostare i parametri dei Wiimote rilevabili
 */

#ifndef _wxMotionFactorDialog_h_
#define _wxMotionFactorDialog_h_

#include <wx/wx.h>

class wxMotionFactorDialog : public wxDialog 
{
public:

	wxMotionFactorDialog( wxWindow *parent, wxWindowID id = wxID_ANY, float _mFactor = 10.0, 
					 const wxString & title = "Modify Motion Factor", const wxPoint& position = wxDefaultPosition, 
					 const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);

	~wxMotionFactorDialog() {}

	float getMFactor();

private:

	float _mFactor;
	wxTextCtrl*	_textCtrl;	

};

#endif
