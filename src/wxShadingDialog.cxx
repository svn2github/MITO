#include "wxShadingDialog.h"
#include <wx/busyinfo.h>

enum {
	idSliderAmbient = wxID_HIGHEST +1,
	idSliderDiffuse,
	idSliderSpecular,
	idSliderSpecularPower,
	idSliderDecimate,
	idSliderSmooth,
	idApply,
};

enum {
	Direct,
	Indirect,
};

BEGIN_EVENT_TABLE (wxShadingDialog, wxDialog)
	EVT_SLIDER(idSliderAmbient, wxShadingDialog::onAmbientSlider)
	EVT_SLIDER(idSliderDiffuse, wxShadingDialog::onDiffuseSlider)
	EVT_SLIDER(idSliderSpecular, wxShadingDialog::onSpecularSlider)
	EVT_SLIDER(idSliderSpecularPower, wxShadingDialog::onSpecularPowerSlider)
	EVT_SLIDER(idSliderDecimate, wxShadingDialog::onDecimateSlider)
	EVT_SLIDER(idSliderSmooth, wxShadingDialog::onSmoothSlider)
	EVT_BUTTON(idApply, wxShadingDialog::onApply)
END_EVENT_TABLE()

wxShadingDialog::wxShadingDialog( int renderingTechnique, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
				:wxDialog( parent, id, title, pos, size, style )
{
	_parent = parent;
	_contourValue = 0;
	_decimateValue = 0;
	_smoothValue = 0;
	_renderingTechnique = renderingTechnique;

	double ambient, diffuse, specular, specularPower;
	wxColour shadingColourAmbient = wxColour(255,255,255);
	wxColour shadingColourDiffuse = wxColour(255,255,255);
	wxColour shadingColourSpecular = wxColour(255,255,255);

	int minValue=0, maxValue=0;

	if ( _renderingTechnique == Direct ) // Volume Rendering
		((wxVolumeRenderingGui*)_parent)->getShadingValues(ambient, diffuse, specular, specularPower);
	else // Surface Rendering
	{
		((wxSurfaceRenderingGui*)_parent)->getDecimateSmoothingValues(_decimateValue, _smoothValue);
		((wxSurfaceRenderingGui*)_parent)->getShadingValues(ambient, diffuse, specular, specularPower);
		shadingColourAmbient = ((wxSurfaceRenderingGui*)_parent)->getShadingColourAmbient();
		shadingColourDiffuse = ((wxSurfaceRenderingGui*)_parent)->getShadingColourDiffuse();
		shadingColourSpecular = ((wxSurfaceRenderingGui*)_parent)->getShadingColourSpecular();
		minValue =((wxSurfaceRenderingGui*)_parent)->getMinPixel()+1;
		maxValue =((wxSurfaceRenderingGui*)_parent)->getMaxPixel()-1;
		_contourValue = (int)((wxSurfaceRenderingGui*)_parent)->getContourValue();
	}

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* shadingSizer;
	shadingSizer = new wxBoxSizer( wxVERTICAL );
	
	wxGridBagSizer* gridBagSizer;
	gridBagSizer = new wxGridBagSizer( 0, 0 );
	gridBagSizer->SetFlexibleDirection( wxBOTH );
	gridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	ambientSlider = new wxSlider( this, idSliderAmbient, (int)(ambient*100), 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE|wxSL_VERTICAL );
	gridBagSizer->Add( ambientSlider, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP, 0 );
	
	diffuseSlider = new wxSlider( this, idSliderDiffuse, (int)(diffuse*100), 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE|wxSL_VERTICAL );
	gridBagSizer->Add( diffuseSlider, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
	
	if ( _renderingTechnique == Direct ) // nel VR varia tra 0 e 4
		specularSlider = new wxSlider( this, idSliderSpecular, (int)(specular*100), 0, 400, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE|wxSL_VERTICAL );
	else // nel SR varia tra 0 e 1
		specularSlider = new wxSlider( this, idSliderSpecular, (int)(specular*100), 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE|wxSL_VERTICAL );
	gridBagSizer->Add( specularSlider, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
	
	specularPowerSlider = new wxSlider( this, idSliderSpecularPower, (int)(specularPower*100), 0, 5000, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE|wxSL_VERTICAL );
	gridBagSizer->Add( specularPowerSlider, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
	
	ambientText = new wxStaticText( this, wxID_ANY, wxT("ambient"), wxPoint( -1,-1 ), wxSize( 50,27 ), wxALIGN_CENTRE );
	ambientText->Wrap( -1 );
	gridBagSizer->Add( ambientText, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_BOTTOM, 2 );
	
	diffuseText = new wxStaticText( this, wxID_ANY, wxT("diffuse"), wxDefaultPosition, wxSize( 50,27 ), wxALIGN_CENTRE );
	diffuseText->Wrap( -1 );
	gridBagSizer->Add( diffuseText, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALIGN_BOTTOM|wxALL, 2 );
	
	specularText = new wxStaticText( this, wxID_ANY, wxT("specular"), wxDefaultPosition, wxSize( 50,27 ), wxALIGN_CENTRE );
	specularText->Wrap( -1 );
	gridBagSizer->Add( specularText, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_BOTTOM|wxALL, 2 );
	
	specularPowerText = new wxStaticText( this, wxID_ANY, wxT("specular power"), wxDefaultPosition, wxSize( 50,27 ), wxALIGN_CENTRE );
	specularPowerText->Wrap( -1 );
	gridBagSizer->Add( specularPowerText, wxGBPosition( 0, 3 ), wxGBSpan( 1, 1 ), wxALIGN_BOTTOM|wxALL, 2 );
	
	wxString ambientTxt = wxString::Format(_T("%1.2f"), ambient);
	ambientValue = new wxStaticText( this, wxID_ANY, ambientTxt, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	gridBagSizer->Add( ambientValue, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );

	wxString diffuseTxt = wxString::Format(_T("%1.2f"), diffuse);
	diffuseValue = new wxStaticText( this, wxID_ANY, diffuseTxt, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	gridBagSizer->Add( diffuseValue, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
	
	wxString specularTxt = wxString::Format(_T("%1.2f"), specular);
	specularValue = new wxStaticText( this, wxID_ANY, specularTxt, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	gridBagSizer->Add( specularValue, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
	
	wxString specularPowerTxt = wxString::Format(_T("%2.1f"), specularPower);
	specularPowerValue = new wxStaticText( this, wxID_ANY, specularPowerTxt, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	gridBagSizer->Add( specularPowerValue, wxGBPosition( 2, 3 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );

	if ( _renderingTechnique == Indirect ) // Surface Rendering
	{
		colourPickerAmbient = new wxColourPickerCtrl( this, wxID_ANY, shadingColourAmbient, wxDefaultPosition, wxDefaultSize, 0 );
		gridBagSizer->Add( colourPickerAmbient, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 10 );
		
		colourPickerDiffuse = new wxColourPickerCtrl( this, wxID_ANY, shadingColourDiffuse, wxDefaultPosition, wxDefaultSize, 0 );
		gridBagSizer->Add( colourPickerDiffuse, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 10 );
		
		colourPickerSpecular = new wxColourPickerCtrl( this, wxID_ANY, shadingColourSpecular, wxDefaultPosition, wxDefaultSize, 0 );
		gridBagSizer->Add( colourPickerSpecular, wxGBPosition( 3, 2 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 10 );

		contourStaticText = new wxStaticText( this, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
		contourStaticText->Wrap( -1 );
		gridBagSizer->Add( contourStaticText, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
		
		contourValueSpinCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, minValue, maxValue, _contourValue );
		gridBagSizer->Add( contourValueSpinCtrl, wxGBPosition( 4, 1 ), wxGBSpan( 1, 3 ), wxALL, 5 );

		decimateStaticText = new wxStaticText( this, wxID_ANY, wxT("Decimate"), wxDefaultPosition, wxDefaultSize, 0 );
		decimateStaticText->Wrap( -1 );
		gridBagSizer->Add( decimateStaticText, wxGBPosition( 5, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
		
		decimateSlider = new wxSlider( this, idSliderDecimate, (int)(_decimateValue*10), 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
		gridBagSizer->Add( decimateSlider, wxGBPosition( 5, 1 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
		
		wxString decimateTxt = wxString::Format(_T("%1.1f"), _decimateValue);
		decimateValue = new wxStaticText( this, wxID_ANY, decimateTxt, wxDefaultPosition, wxSize( -1,-1 ), 0 );
		decimateValue->Wrap( -1 );
		gridBagSizer->Add( decimateValue, wxGBPosition( 5, 3 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 0 );
		
		smoothStaticText = new wxStaticText( this, wxID_ANY, wxT("Smooth"), wxDefaultPosition, wxDefaultSize, 0 );
		smoothStaticText->Wrap( -1 );
		gridBagSizer->Add( smoothStaticText, wxGBPosition( 6, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
		
		smoothSlider = new wxSlider( this, idSliderSmooth, (int)_smoothValue, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
		gridBagSizer->Add( smoothSlider, wxGBPosition( 6, 1 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
		
		wxString smoothTxt = wxString::Format(_T("%3.0f"), _smoothValue);
		smoothValue = new wxStaticText( this, wxID_ANY, smoothTxt, wxDefaultPosition, wxSize( -1,-1 ), 0 );
		smoothValue->Wrap( -1 );
		gridBagSizer->Add( smoothValue, wxGBPosition( 6, 3 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );

		Apply = new wxButton( this, idApply, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
		gridBagSizer->Add( Apply, wxGBPosition( 7, 0 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER|wxALL, 10 );
		
		Close = new wxButton( this, wxID_CANCEL, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
		gridBagSizer->Add( Close, wxGBPosition( 7, 2 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER|wxALL, 10 );
	}
	else // Volume Rendering
	{
		Apply = new wxButton( this, idApply, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
		gridBagSizer->Add( Apply, wxGBPosition( 3, 0 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER|wxALL, 5 );
		
		Close = new wxButton( this, wxID_CANCEL, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
		gridBagSizer->Add( Close, wxGBPosition( 3, 2 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER|wxALL, 5 );
	}

	shadingSizer->Add( gridBagSizer, 1, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( shadingSizer );
	this->Layout();
	shadingSizer->Fit( this );
	
	this->Centre( wxBOTH );
}

wxShadingDialog::~wxShadingDialog()
{
}

void wxShadingDialog::onAmbientSlider(wxCommandEvent &event) {
    float value = (float)(ambientSlider->GetValue())/100;
	wxString msg = wxString::Format(_T("%1.2f"), value);
	ambientValue->SetLabel( msg );
}

void wxShadingDialog::onDiffuseSlider(wxCommandEvent &event) {
    float value = (float)(diffuseSlider->GetValue())/100;
	wxString msg = wxString::Format(_T("%1.2f"), value);
	diffuseValue->SetLabel( msg );
}

void wxShadingDialog::onSpecularSlider(wxCommandEvent &event) {
    float value = (float)(specularSlider->GetValue())/100;
	wxString msg = wxString::Format(_T("%1.2f"), value);
	specularValue->SetLabel( msg );
}

void wxShadingDialog::onSpecularPowerSlider(wxCommandEvent &event) {
	float value = (float)(specularPowerSlider->GetValue())/100;
	wxString msg = wxString::Format(_T("%2.1f"), value);
	specularPowerValue->SetLabel( msg );
}

void wxShadingDialog::onDecimateSlider(wxCommandEvent &event) {
    float value = (float)(decimateSlider->GetValue())/10;
	wxString msg = wxString::Format(_T("%1.1f"), value);
	decimateValue->SetLabel( msg );
}

void wxShadingDialog::onSmoothSlider(wxCommandEvent &event) {
    float value = (float)(smoothSlider->GetValue());
	wxString msg = wxString::Format(_T("%3.0f"), value);
	smoothValue->SetLabel( msg );
}

void wxShadingDialog::onApply(wxCommandEvent &event) {
	double ambient = (double)(ambientSlider->GetValue()) / 100;
	double diffuse = (double)(diffuseSlider->GetValue()) / 100;
	double specular = (double)(specularSlider->GetValue()) / 100;
	double specularPower = (double)(specularPowerSlider->GetValue()) / 100;

	if ( _renderingTechnique == Indirect ) // Surface Rendering
	{
		((wxSurfaceRenderingGui*)_parent)->setShadingColourAmbient( colourPickerAmbient->GetColour() );
		((wxSurfaceRenderingGui*)_parent)->setShadingColourDiffuse( colourPickerDiffuse->GetColour() );
		((wxSurfaceRenderingGui*)_parent)->setShadingColourSpecular( colourPickerSpecular->GetColour() );

		int		newContourValue  = contourValueSpinCtrl->GetValue();
		double	newDecimateValue = (double)(decimateSlider->GetValue()) / 10;
		double	newSmoothValue	 = (double)(smoothSlider->GetValue());

		if ( (_contourValue == newContourValue) && (_decimateValue == newDecimateValue) && (_smoothValue == newSmoothValue) ) // non c'è da ricalcolare il surface
		{
			((wxSurfaceRenderingGui*)_parent)->updateShading(ambient, diffuse, specular, specularPower);
		}
		else // ricalcolo
		{
			wxWindowDisabler disabler;
			wxBusyInfo wait("Please wait...");
			wxBusyCursor cursor;
			_contourValue = newContourValue;
			_decimateValue = newDecimateValue;
			_smoothValue = newSmoothValue;
			((wxSurfaceRenderingGui*)_parent)->computeSurfaceRendering( _contourValue, _decimateValue, _smoothValue );
			this->Raise();
		}
	}
	else // Volume Rendering
		((wxVolumeRenderingGui*)_parent)->updateShading(ambient, diffuse, specular, specularPower);
}