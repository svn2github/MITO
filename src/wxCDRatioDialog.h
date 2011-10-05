/**
 * \file wxCDRatioDialog.h
 * \brief File per la creazione del dialog che permette di impostare i parametri dei Wiimote rilevabili
 */

#ifndef _wxCDRatioDialog_h_
#define _wxCDRatioDialog_h_

#include <wx/wx.h>
#include <wx/odcombo.h>
#include <wx/bmpcbox.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>




class wxCDRatioDialog : public wxDialog 
{
public:

	wxCDRatioDialog( wxWindow *parent, wxWindowID id = wxID_ANY, float Cmin = 0.005, float Cmax = 0.750, 
					 const wxString & title = "Modify pointing C-D ratio", const wxPoint& position = wxDefaultPosition, 
					 const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);

	~wxCDRatioDialog() {}


	float getCMin();
	float getCMax();

private:

	int _Cmin;
	int _Cmax;

	wxTextCtrl*	_minTextCtrl;
	wxTextCtrl*	_maxTextCtrl;
	
	int TextCtrlValueToInt(wxTextCtrl* textCtrl);

	void updateTextCtrl(wxTextCtrl *textCtrl, int n);

	void onCMinSpinUp(wxSpinEvent & event);
	void onCMinSpinDown(wxSpinEvent & event);

	void onCMaxSpinUp(wxSpinEvent & event);
	void onCMaxSpinDown(wxSpinEvent & event);


	DECLARE_EVENT_TABLE()

};

#endif
