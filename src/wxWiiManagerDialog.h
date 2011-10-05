/**
 * \file wxWiiManagerDialog.h
 * \brief File per la creazione del dialog che permette di impostare i parametri dei Wiimote rilevabili
 */

#ifndef _wxWiiManagerDialog_h_
#define _wxWiiManagerDialog_h_

#include <wx/wx.h>
#include <wx/odcombo.h>
#include <wx/bmpcbox.h>

#define num_Colors			7
#define connectLabel		"Connect"
#define selectAllLabel		"Connect to all Wii"
#define pointerColorLabel	"Pointer Color"
#define choiceColorLabel	"Choice Pointer Color"

#define rilevaButtonLabel	"Redetect"
#define rilevaPressed		wxID_HIGHEST + 1
#define idRotationMode		wxID_HIGHEST + 2

#define listOfColors		{"Red","Yellow","Blue","Green","Violet","Orange","White"}


class wxWiiManagerDialog : public wxDialog 
{
public:
	
	wxWiiManagerDialog( wxWindow *parent, int Wii_num = 0, int* batteryLevel = NULL, wxWindowID id = wxID_ANY, const wxString & title = "Wiimote Manager", const wxPoint& position = wxDefaultPosition, 
						const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);

	~wxWiiManagerDialog() {}

	bool isWiiSelected(int i);
	int  getColorChosen(int i);
	wxChoice* getRotationModeChoice();



private:

	DECLARE_EVENT_TABLE()

	wxPanel*	createAWiiBox(wxWindow * parent, int idWii, int batteryLevel);
	wxWindow*		getWiiPanel(int i);
	wxChoice*		getChoiceColor(wxWindow * win);
	wxCheckBox*		getCheckBox(wxWindow * win);
	wxStaticText*	getBatteryText(wxWindow * win);
	wxCheckBox*		getAllSelectBox();	
	wxButton*		getStartButton();

	void OnRileva(wxCommandEvent & event);
	void OnCheckBox(wxCommandEvent & event);

	int Wii_num;
};

#endif