/**
 * \file wxShadingDialog.h
 * \brief File per la creazione del dialog che permette di impostare i parametri dei Wiimote rilevabili
 */

#ifndef _wxShadingDialog_h_
#define _wxShadingDialog_h_

#ifndef _wxVolumeRenderingGui_h_
#include "wxVolumeRenderingGui.h"
#endif

#ifndef _wxSurfaceRenderingGui_h_
#include "wxSurfaceRenderingGui.h"
#endif

#include <wx/slider.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
//#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/clrpicker.h>
#include <wx/colour.h>
#include <wx/spinctrl.h>

class wxShadingDialog : public wxDialog 
{

	public:
		
		wxShadingDialog( int renderingTechnique, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Modify properties", 
						const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 222,260 ), 
						long style = wxCAPTION|wxNO_BORDER|wxSTATIC_BORDER );

		~wxShadingDialog();

		void onAmbientSlider(wxCommandEvent &event);
		void onDiffuseSlider(wxCommandEvent &event);
		void onSpecularSlider(wxCommandEvent &event);
		void onSpecularPowerSlider(wxCommandEvent &event);
		void onDecimateSlider(wxCommandEvent &event);
		void onSmoothSlider(wxCommandEvent &event);
		void onApply(wxCommandEvent &event);
		
		wxWindow* _parent;

	private:
		DECLARE_EVENT_TABLE()

		int _renderingTechnique;
		int _contourValue;
		double _decimateValue;
		double _smoothValue;

	protected:
		wxSlider*			ambientSlider;
		wxSlider*			diffuseSlider;
		wxSlider*			specularSlider;
		wxSlider*			specularPowerSlider;
		wxStaticText*		ambientText;
		wxStaticText*		ambientValue;
		wxStaticText*		diffuseValue;
		wxStaticText*		specularValue;
		wxStaticText*		specularPowerValue;
		wxStaticText*		diffuseText;
		wxStaticText*		specularText;
		wxStaticText*		specularPowerText;
		wxColourPickerCtrl* colourPickerAmbient;
		wxColourPickerCtrl* colourPickerDiffuse;
		wxColourPickerCtrl*	colourPickerSpecular;
		wxStaticText*		contourStaticText;
		wxSpinCtrl*			contourValueSpinCtrl;
		wxStaticText*		decimateStaticText;
		wxSlider*			decimateSlider;
		wxStaticText*		decimateValue;
		wxSlider*			smoothSlider;
		wxStaticText*		smoothStaticText;
		wxStaticText*		smoothValue;
		wxButton*			Apply;
		wxButton*			Close;
};

#endif //__noname__
