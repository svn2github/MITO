/**
 * \file wxSurfaceRenderingGui.cxx
 * \brief File per la creazione della finestra 3D contenente il risultato del surface rendering
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/busyinfo.h>

#include "wxSurfaceRenderingGui.h"
#include "itkVtkData.h"
#include "wxVtkViewer3d.h"

#include <vtkPolyDataMapper.h>

#include "icons/Zoom.xpm"
#include "icons/Dolly.xpm"
#include "icons/Cut3d.xpm"
#include "icons/Rot3d.xpm"
#include "icons/Move.xpm"
#include "icons/Rotate.xpm"
#include "icons/Axial.xpm"
#include "icons/Coronal.xpm"
#include "icons/Sagittal.xpm"
#include "icons/OppositeSagittal.xpm"

// Wii
#include "icons/Wii.xpm"
#include "vtkInteractorStyleWIITrackball.h"
#include <vtkInteractorStyleTrackballCamera.h>
#include "wxCDRatioDialog.h"
#include "wxMotionFactorDialog.h"

// shading
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/clrpicker.h>
#include <wx/tglbtn.h>

#include "wxShadingDialog.h"

#include <vtkInteractorStyleTrackballActor.h>
#include "WiiCommandEvents.h"
#include "vtkMitoCustomCamera.h"


BEGIN_EVENT_TABLE(wxSurfaceRenderingGui, wxFrame)
	EVT_ACTIVATE(wxSurfaceRenderingGui::onActivate)
	EVT_BUTTON(id3d_moveTool,wxSurfaceRenderingGui::onMove)
	EVT_BUTTON(id3d_dollyTool,wxSurfaceRenderingGui::onDolly)
	EVT_BUTTON(id3d_zoomTool,wxSurfaceRenderingGui::onZoom)
	EVT_BUTTON(id3d_rotateTool,wxSurfaceRenderingGui::onRotate)
	EVT_BUTTON(id3d_3drotTool,wxSurfaceRenderingGui::onRotateAround)
	EVT_BUTTON(id3d_AxialTool,wxSurfaceRenderingGui::onAxialView)	
	EVT_BUTTON(id3d_CorTool,wxSurfaceRenderingGui::onCoronalView)	
	EVT_BUTTON(id3d_SagTool,wxSurfaceRenderingGui::onSagittalView)
	EVT_BUTTON(id3d_sagoppTool,wxSurfaceRenderingGui::onOppositeSagittalView)
	EVT_BUTTON(id3d_shadingChangeButton,wxSurfaceRenderingGui::onChangeShadingParameters)
	EVT_TOGGLEBUTTON(id3d_stereo,wxSurfaceRenderingGui::onStereo)
	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPInputDevice1,wxSurfaceRenderingGui::onChangeInputDevice)
	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPInputDevice2,wxSurfaceRenderingGui::onChangeInputDevice)
	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPCursorType1,wxSurfaceRenderingGui::onCursorType)
	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPCursorType2,wxSurfaceRenderingGui::onCursorType)
	EVT_RADIOBUTTON(id3d_perspective,wxSurfaceRenderingGui::onProjectionTypePerspective)
	EVT_RADIOBUTTON(id3d_parallel,wxSurfaceRenderingGui::onProjectionTypeParallel)
	EVT_RADIOBUTTON(id3d_endoscopy,wxSurfaceRenderingGui::onProjectionTypeEndoscopy)
	EVT_MENU(m_3d_3dviewerResetToInitialView,wxSurfaceRenderingGui::onResetView)
	EVT_MENU(m_3d_StereoMode,wxSurfaceRenderingGui::onStereo)
	EVT_MENU(m_3d_MoveSR,wxSurfaceRenderingGui::onMove)
	EVT_MENU(m_3d_DollySR,wxSurfaceRenderingGui::onDolly)
	EVT_MENU(m_3d_ZoomSR,wxSurfaceRenderingGui::onZoom)
	EVT_MENU(m_3d_RotateSR,wxSurfaceRenderingGui::onRotate)
	EVT_MENU(m_3d_RotateAroundSR,wxSurfaceRenderingGui::onRotateAround)
	EVT_MENU(m_3d_AxialTool,wxSurfaceRenderingGui::onAxialView)	
	EVT_MENU(m_3d_CorTool,wxSurfaceRenderingGui::onCoronalView)	
	EVT_MENU(m_3d_SagTool,wxSurfaceRenderingGui::onSagittalView)
	EVT_MENU(m_3d_sagoppTool,wxSurfaceRenderingGui::onOppositeSagittalView)	
	EVT_MENU(id3d_fileCloseWindow,wxSurfaceRenderingGui::onClose)
	EVT_MENU(id3d_HelpAbout,wxSurfaceRenderingGui::onShowAboutDialog)	
	EVT_MENU(m_3d_3dviewerFullScreen,wxSurfaceRenderingGui::onFullScreen)
	EVT_MENU(m_3d_StereoModeAnaglyph,wxSurfaceRenderingGui::onStereoModeAnaglyph)
	EVT_MENU(m_3d_StereoModeActivePassive,wxSurfaceRenderingGui::onStereoModeActivePassive)
	EVT_MENU(m_3d_StereoModeCheckerboard,wxSurfaceRenderingGui::onStereoModeCheckerboard)
	EVT_MENU(m_3d_ModifyObserverDistance,wxSurfaceRenderingGui::onModifyObserverDistance)
	EVT_MENU(id3d_WiiTool,wxSurfaceRenderingGui::onWii)
	EVT_MENU(id3d_WiiConfigurationTool,wxSurfaceRenderingGui::onWiiConfiguration)
	EVT_MENU(id3d_WiiChangeSceneIRInteraction,wxSurfaceRenderingGui::onWiiChangeSceneIRInteraction)
	EVT_MENU(id3d_ModifyCDRatioFunction,wxSurfaceRenderingGui::onModifyCDRatioFunction)
	EVT_MENU(id3d_ModifyMotionFactor,wxSurfaceRenderingGui::onModifyMotionFactor)
	EVT_CHAR_HOOK(wxSurfaceRenderingGui::onChar)
	EVT_CLOSE(wxSurfaceRenderingGui::onQuit)
END_EVENT_TABLE()

wxSurfaceRenderingGui::wxSurfaceRenderingGui(const wxString& title, const wxPoint& pos, const wxSize& size)
					  :wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE) {

	/*wxIcon icon;
	icon.LoadFile("icon.ico",wxBITMAP_TYPE_ICO);
	this->SetIcon(icon);*/

	SetIcon(wxICON(MITO));

	_selectedButton = id3d_3drotTool;
	_POD = 600;

	// inizializza contour, decimate and smoothing
	_contourValue = 0;
	_decimateValue = 0.50;
	_smoothValue = 20.0;

	// inizializza la parallasse
	_parallax = 0;

	// cursore 3D
	_3DCursor = NULL;
	_cellLocator = NULL;
	_3DCursorIsOnModelSurface = false;

	// Assi 3D passanti per il centro dell'oggetto
	_3DAxesActor = NULL;

	// inizializza lo zoom
	_globalZoomFactor = 1;

	// shading
	_shadingAmbient = 0.10;
	_shadingDiffuse = 1.00;
	_shadingSpecular = 1.00;
	_shadingSpecularPower = 50.0;
	_shadingColourAmbient = wxColour(70,70,70);
	_shadingColourDiffuse = wxColour(80,80,80);
	_shadingColourSpecular = wxColour(200,200,200);

	CreateSurfaceToolbar();
	Createmenu();
}

void wxSurfaceRenderingGui::Createmenu() {
	CreateStatusBar(3);
	SetStatusText("Ready", 0);
	
	wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
	menuFile->Append(id3d_fileCloseWindow, "Close 3D Viewer\tCtrl-C"); 

	wxMenu *menu3dViewer = new wxMenu("", wxMENU_TEAROFF);
	menu3dViewer->Append(m_3d_3dviewerResetToInitialView, "Reset to initial view\tCtrl-R"); 
	menu3dViewer->AppendSeparator();
	menu3dViewer->Append(m_3d_CorTool, "Coronal View\tAlt-C"); 
	menu3dViewer->Append(m_3d_SagTool, "Left Sagittal View\tAlt-L");
	menu3dViewer->Append(m_3d_sagoppTool, "Right Sagittal View\tAlt-R");
	menu3dViewer->Append(m_3d_AxialTool, "Axial View\tAlt-A");
	menu3dViewer->AppendSeparator();
	menu3dViewer->Append(m_3d_3dviewerFullScreen, "Full Screen\tCtrl-F");
	menu3dViewer->AppendSeparator();

	_menuStereoMode = new wxMenu; 
	//_menuStereoMode->Append(m_3d_StereoModeOff,_("Off\tCtrl-O"),_T(""),wxITEM_NORMAL);
	_menuStereoMode->AppendCheckItem(m_3d_StereoModeAnaglyph,_("Anaglyph\tCtrl-Y"),_T(""));
	(_menuStereoMode->AppendCheckItem(m_3d_StereoModeActivePassive,_("Active/Passive\tCtrl-S"),_T("")))->Check();
	_menuStereoMode->AppendCheckItem(m_3d_StereoModeCheckerboard,_("Checkerboard\tCtrl-H"),_T(""));
	_menuStereoMode->Append(m_3d_ModifyObserverDistance,_("Modify observer distance\tCtrl-B"),_T(""),wxITEM_NORMAL);
	menu3dViewer->Append(m_3d_StereoMode,"Stereo Mode",_menuStereoMode);

	wxMenu *menuInteraction = new wxMenu("", wxMENU_TEAROFF);
	menuInteraction->Append(m_3d_RotateAroundSR,_("ROTATE 3D\tCtrl-A"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_DollySR,_("DOLLY\tCtrl-D"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_ZoomSR,_("ZOOM\tCtrl-Z"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_RotateSR,_("ROLL\tCtrl-T"),_T(""),wxITEM_NORMAL);		
	menuInteraction->Append(m_3d_MoveSR,_("PAN\tCtrl-M"),_T(""),wxITEM_NORMAL);		
	//menuInteraction->Append(m_3d_VOISR,_("VOI\tCtrl-V"),_T(""),wxITEM_NORMAL);
	//menuInteraction->Enable(m_3d_VOISR,false);
	menuInteraction->AppendSeparator();		
	menuInteraction->Append(id3d_WiiTool,_("Wiimote Quick Start\tCtrl-1"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(id3d_WiiConfigurationTool,_("Wiimote Options\tCtrl-2"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(id3d_WiiChangeSceneIRInteraction,_("Wiimote interaction technique switch\tCtrl-3"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(id3d_ModifyCDRatioFunction,_("Modify pointing C-D ratio"),_T(""),wxITEM_NORMAL);
	menuInteraction->AppendSeparator();		
	menuInteraction->Append(id3d_ModifyMotionFactor,_("Modify rotation Motion Factor"),_T(""),wxITEM_NORMAL);
	
	wxMenu *menuHelp = new wxMenu("", wxMENU_TEAROFF);
	menuHelp->Append(id3d_HelpAbout, "About MITO"); 

	wxMenuBar *menuBar = new wxMenuBar();
  	menuBar->Append(menuFile, "File");
  	menuBar->Append(menu3dViewer, "3D Viewer");
	menuBar->Append(menuInteraction, "Interaction");
	menuBar->Append(menuHelp, "Help");
    SetMenuBar(menuBar);
}

void wxSurfaceRenderingGui::CreateSurfaceToolbar() {

	int altezzatool=25;
	int altezzaradiobox=5;
	int currentHorizontalPosition=10;

	_surfaceToolbar = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL|wxTB_TEXT, id3d_surfaceToolbar3D );

	_surfaceToolbar->SetToolSeparation(30); 
	_surfaceToolbar->SetToolPacking(20);

	// aggiunto da Luigi per compatibilità con Vista Aero
	_surfaceToolbar->SetBackgroundColour(*wxBLACK);
	_surfaceToolbar->SetForegroundColour(*wxWHITE);

	wxBitmap zoomTool(this->GetBitmapResource(wxT("icons/Zoom.xpm")));
	wxBitmap dollyTool(this->GetBitmapResource(wxT("icons/Dolly.xpm")));
	wxBitmap voiTool(this->GetBitmapResource(wxT("icons/Cut3d.xpm")));
	wxBitmap rot3dTool(this->GetBitmapResource(wxT("icons/Rot3d.xpm")));
	wxBitmap rot3dcamTool(this->GetBitmapResource(wxT("icons/Rot3dcam.xpm")));
	wxBitmap moveTool(this->GetBitmapResource(wxT("icons/Move.xpm")));
	wxBitmap RotateTool(this->GetBitmapResource(wxT("icons/Rotate.xpm")));
	wxBitmap AxialTool(this->GetBitmapResource(wxT("icons/Axial.xpm")));
	wxBitmap CorTool(this->GetBitmapResource(wxT("icons/Coronal.xpm")));
	wxBitmap SagTool(this->GetBitmapResource(wxT("icons/Sagittal.xpm")));
	wxBitmap sagoppTool(this->GetBitmapResource(wxT("icons/OppositeSagittal.xpm")));
	wxBitmap WiiTool(this->GetBitmapResource(wxT("icons/Wii.xpm")));

	_surfaceToolbar->SetToolBitmapSize(wxSize(28,65));
	_surfaceToolbar->Realize();
	_surfaceToolbar->SetRows(1);

	//Raggruppamento dei radioButton associati al inputDevice
	wxPanel* inputDeviceRadioPanel = new wxPanel(_surfaceToolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	inputDeviceRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* inputDeviceStaticBox = new wxStaticBox(inputDeviceRadioPanel, wxID_ANY, "Input"); 	
	wxStaticBoxSizer* inputDeviceRadioSizer = new wxStaticBoxSizer( inputDeviceStaticBox, wxVERTICAL );	  
	wxBoxSizer* inputDeviceRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB1 = new wxRadioButton(inputDeviceRadioPanel, id3d_RADIOBUTTONGROUPInputDevice1, "");	
	inputDeviceB1->SetForegroundColour(*wxWHITE);  
	inputDeviceB1->SetValue(true);
	wxStaticText* inputDeviceB1Text = new wxStaticText(inputDeviceRadioPanel,wxID_ANY,"Mouse");
	inputDeviceB1Text->SetForegroundColour(*wxWHITE);  
	inputDeviceRow1->Add( inputDeviceB1 , 0, wxLEFT, 0); 
	inputDeviceRow1->Add( inputDeviceB1Text , 0, wxLEFT | wxRIGHT, 0); 		
	wxBoxSizer* inputDeviceRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB2 = new wxRadioButton(inputDeviceRadioPanel, id3d_RADIOBUTTONGROUPInputDevice2, "");
	wxStaticText* inputDeviceB2Text = new wxStaticText(inputDeviceRadioPanel,wxID_ANY,"Wiimote");
	inputDeviceB2Text->SetForegroundColour(*wxWHITE);  
	inputDeviceRow2->Add( inputDeviceB2 , 0, wxRIGHT, 0); 
	inputDeviceRow2->Add( inputDeviceB2Text , 0, wxRIGHT, 0);	 
	inputDeviceRadioSizer->Add( inputDeviceRow1 , 0, wxTOP , 6); 
	inputDeviceRadioSizer->Add( inputDeviceRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	inputDeviceRadioPanel->SetSizerAndFit(inputDeviceRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+inputDeviceRadioPanel->GetSize().GetWidth();
	
	//Contenitore raggruppante i Mouse Functions Buttons
	currentHorizontalPosition=currentHorizontalPosition+5;	
	wxStaticBox* mouseFunctionsButtonsBox = new wxStaticBox(_surfaceToolbar, wxID_ANY, wxString("Mouse Functions"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(183,64));

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* button3drotTool = new wxBitmapButton(_surfaceToolbar, id3d_3drotTool, rot3dTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	button3drotTool->SetToolTip(_("ROTATE 3D"));
	_surfaceToolbar->AddControl(button3drotTool);
	currentHorizontalPosition=currentHorizontalPosition+button3drotTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonDolly = new wxBitmapButton(_surfaceToolbar, id3d_dollyTool, dollyTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonDolly->SetToolTip(_("DOLLY"));
	_surfaceToolbar->AddControl(buttonDolly);
	currentHorizontalPosition=currentHorizontalPosition+buttonDolly->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonZoom = new wxBitmapButton(_surfaceToolbar, id3d_zoomTool,zoomTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonZoom->SetToolTip(_("ZOOM"));
	_surfaceToolbar->AddControl(buttonZoom);
	currentHorizontalPosition=currentHorizontalPosition+buttonZoom->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonRotate = new wxBitmapButton(_surfaceToolbar, id3d_rotateTool,RotateTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonRotate->SetToolTip(_("ROLL"));
	_surfaceToolbar->AddControl(buttonRotate);
	currentHorizontalPosition=currentHorizontalPosition+buttonRotate->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonMove = new wxBitmapButton(_surfaceToolbar, id3d_moveTool,moveTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonMove->SetToolTip(_("PAN"));
	_surfaceToolbar->AddControl(buttonMove);
	currentHorizontalPosition=currentHorizontalPosition+buttonMove->GetSize().GetWidth()-1;

	//Raggruppamento dei radioButton associati al cursorType
	currentHorizontalPosition=currentHorizontalPosition+13;
	wxPanel* cursorTypeRadioPanel = new wxPanel(_surfaceToolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	cursorTypeRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* cursorTypeStaticBox = new wxStaticBox(cursorTypeRadioPanel, wxID_ANY, "Cursor"); 	
	wxStaticBoxSizer* cursorTypeRadioSizer = new wxStaticBoxSizer( cursorTypeStaticBox, wxVERTICAL );	  
	wxBoxSizer* cursorTypeRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB1 = new wxRadioButton(cursorTypeRadioPanel, id3d_RADIOBUTTONGROUPCursorType1, "");	
	cursorTypeB1->SetForegroundColour(*wxWHITE);  
	cursorTypeB1->SetValue(true);
	wxStaticText* cursorTypeB1Text = new wxStaticText(cursorTypeRadioPanel,wxID_ANY,"2D ");
	cursorTypeB1Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow1->Add( cursorTypeB1 , 0, wxLEFT, 0); 
	cursorTypeRow1->Add( cursorTypeB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* cursorTypeRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB2 = new wxRadioButton(cursorTypeRadioPanel, id3d_RADIOBUTTONGROUPCursorType2, "");
	wxStaticText* cursorTypeB2Text = new wxStaticText(cursorTypeRadioPanel,wxID_ANY,"3D ");
	cursorTypeB2Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow2->Add( cursorTypeB2 , 0, wxRIGHT, 3); 
	cursorTypeRow2->Add( cursorTypeB2Text , 0, wxRIGHT, 5);	 
	cursorTypeRadioSizer->Add( cursorTypeRow1 , 0, wxTOP , 6); 
	cursorTypeRadioSizer->Add( cursorTypeRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	cursorTypeRadioPanel->SetSizerAndFit(cursorTypeRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+cursorTypeRadioPanel->GetSize().GetWidth();

	//Raggruppamento Shading
	currentHorizontalPosition=currentHorizontalPosition+5;	
	wxStaticBox* shadingStaticBox2 = new wxStaticBox(_surfaceToolbar, wxID_ANY, wxString("Surface properties"),wxPoint(currentHorizontalPosition,4),wxSize(205,64));
	currentHorizontalPosition=currentHorizontalPosition+10;	

	wxPanel* shadingPanel = new wxPanel(_surfaceToolbar, wxID_ANY, wxPoint(currentHorizontalPosition,19),wxSize(190,50));
	shadingPanel->SetForegroundColour( *wxWHITE);

	wxBoxSizer* shadingBoxSizer = new wxBoxSizer(wxVERTICAL);	
	wxGridBagSizer* shadingGridBagSizer;
	shadingGridBagSizer = new wxGridBagSizer( 2, 5 );
	shadingGridBagSizer->SetFlexibleDirection( wxBOTH );
	shadingGridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	shadingGridBagSizer->SetEmptyCellSize( wxSize( -1,-1 ) );

	wxString contourValueText = _T(" value: ") + wxString::Format(_T("%6i"), (int)_contourValue);
	wxStaticText* contourValueStaticText = new wxStaticText( shadingPanel, id3d_contourValueText, contourValueText, wxDefaultPosition, wxDefaultSize, 0 );
	contourValueStaticText->Wrap( -1 );
	contourValueStaticText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( contourValueStaticText, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxALIGN_LEFT|wxALL, 0 );

	wxButton* shadingChangeButton = new wxButton( shadingPanel, id3d_shadingChangeButton, wxT("change"), wxDefaultPosition, wxDefaultSize, 0 );
	shadingChangeButton->SetForegroundColour( wxColour( 0, 0, 0 ) );
	shadingGridBagSizer->Add( shadingChangeButton, wxGBPosition( 1, 0 ), wxGBSpan( 2, 2 ), wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 0 );

	wxStaticText* shadingAmbient = new wxStaticText( shadingPanel, wxID_ANY, wxT("ambient: "), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingAmbient->Wrap( -1 );
	shadingAmbient->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingAmbient, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	wxString ambientTxt = wxString::Format(_T("%1.2f"), _shadingAmbient);
	wxStaticText* shadingAmbientText = new wxStaticText( shadingPanel, id3d_shadingAmbientText, ambientTxt, wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingAmbientText->Wrap( -1 );
	shadingAmbientText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingAmbientText, wxGBPosition( 0, 3 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	
	wxStaticText* shadingDiffuse = new wxStaticText( shadingPanel, wxID_ANY, wxT("diffuse: "), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingDiffuse->Wrap( -1 );
	shadingDiffuse->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingDiffuse, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	wxString diffuseTxt = wxString::Format(_T("%1.2f"), _shadingDiffuse);
	wxStaticText* shadingDiffuseText = new wxStaticText( shadingPanel, id3d_shadingDiffuseText, diffuseTxt, wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingDiffuseText->Wrap( -1 );
	shadingDiffuseText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingDiffuseText, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	
	wxStaticText* shadingSpecular = new wxStaticText( shadingPanel, wxID_ANY, wxT("specular: "), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingSpecular->Wrap( -1 );
	shadingSpecular->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingSpecular, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	wxString specularTxt = wxString::Format(_T("%1.2f"), _shadingSpecular);
	wxString specularPowerTxt = wxString::Format(_T("%2.1f"), _shadingSpecularPower);
	wxString specularAllTxt = specularTxt + _T(", ") + specularPowerTxt;
	wxStaticText* shadingSpecularText = new wxStaticText( shadingPanel, id3d_shadingSpecularText, specularAllTxt, wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingSpecularText->Wrap( -1 );
	shadingSpecularText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingSpecularText, wxGBPosition( 2, 3 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );

	shadingBoxSizer->Add( shadingGridBagSizer, 0, 0, 0 );
	shadingPanel->SetSizerAndFit( shadingBoxSizer );
	currentHorizontalPosition=currentHorizontalPosition+shadingPanel->GetSize().GetWidth();

	//Contenitore raggruppante Stereo e Parallel/Perspective/Endoscopy
	currentHorizontalPosition=currentHorizontalPosition+13;	
	wxStaticBox* shadingStaticBoxViewMode = new wxStaticBox(_surfaceToolbar, wxID_ANY, wxString("View mode"),wxPoint(currentHorizontalPosition,4),wxSize(145,64));
	currentHorizontalPosition=currentHorizontalPosition+8;	

	wxPanel* viewModePanel = new wxPanel(_surfaceToolbar, wxID_ANY, wxPoint(currentHorizontalPosition,19),wxSize(140,50));
	viewModePanel->SetForegroundColour( *wxWHITE);

	wxBoxSizer* viewModeBoxSizer = new wxBoxSizer(wxVERTICAL);	
	wxGridBagSizer* viewModeGridBagSizer;
	viewModeGridBagSizer = new wxGridBagSizer( 2, 5 );
	viewModeGridBagSizer->SetFlexibleDirection( wxBOTH );
	viewModeGridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );

	wxToggleButton* stereoToggleBtn = new wxToggleButton( viewModePanel, id3d_stereo, wxT("Stereo"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	stereoToggleBtn->SetValue( false ); 
	viewModeGridBagSizer->Add( stereoToggleBtn, wxGBPosition( 0, 0 ), wxGBSpan( 3, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 0 );
	
	wxRadioButton* perspectiveRadioBtn = new wxRadioButton( viewModePanel, id3d_perspective, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	perspectiveRadioBtn->SetValue( true );
	viewModeGridBagSizer->Add( perspectiveRadioBtn, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxStaticText* perspectiveStaticText = new wxStaticText( viewModePanel, wxID_ANY, wxT("Perspective"), wxDefaultPosition, wxDefaultSize, 0 );
	perspectiveStaticText->Wrap( -1 );
	perspectiveStaticText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	perspectiveStaticText->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	viewModeGridBagSizer->Add( perspectiveStaticText, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxRadioButton* parallelRadioBtn = new wxRadioButton( viewModePanel, id3d_parallel, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	viewModeGridBagSizer->Add( parallelRadioBtn, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxStaticText* parallelStaticText = new wxStaticText( viewModePanel, wxID_ANY, wxT("Parallel"), wxDefaultPosition, wxDefaultSize, 0 );
	parallelStaticText->Wrap( -1 );
	parallelStaticText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	parallelStaticText->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	viewModeGridBagSizer->Add( parallelStaticText, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxRadioButton* endoscopyRadioBtn = new wxRadioButton( viewModePanel, id3d_endoscopy, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	viewModeGridBagSizer->Add( endoscopyRadioBtn, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxStaticText* endoscopyStaticText = new wxStaticText( viewModePanel, wxID_ANY, wxT("Endoscopy"), wxDefaultPosition, wxDefaultSize, 0 );
	endoscopyStaticText->Wrap( -1 );
	endoscopyStaticText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	endoscopyStaticText->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	viewModeGridBagSizer->Add( endoscopyStaticText, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALL, 0 );

	viewModeBoxSizer->Add( viewModeGridBagSizer, 0, 0, 0 );
	viewModePanel->SetSizerAndFit( viewModeBoxSizer );
	currentHorizontalPosition=currentHorizontalPosition+viewModePanel->GetSize().GetWidth();

	//Contenitore raggruppante i View Functions Buttons
	currentHorizontalPosition=currentHorizontalPosition+12;	
	wxStaticBox* viewButtonsBox = new wxStaticBox(_surfaceToolbar, wxID_ANY, wxString("Views"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(147,64));

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonAxialTool = new wxBitmapButton(_surfaceToolbar, id3d_AxialTool,AxialTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	_surfaceToolbar->AddControl(buttonAxialTool);
	buttonAxialTool->SetToolTip(_("Move to an axial view"));
	currentHorizontalPosition=currentHorizontalPosition+buttonAxialTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonCorTool = new wxBitmapButton(_surfaceToolbar, id3d_CorTool,CorTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonCorTool->SetToolTip(_("Move to a coronal view"));
	_surfaceToolbar->AddControl(buttonCorTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonCorTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonSagTool = new wxBitmapButton(_surfaceToolbar, id3d_SagTool,SagTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonSagTool->SetToolTip(_("Move to a left sagittal view"));
	_surfaceToolbar->AddControl(buttonSagTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonSagTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonSagOppTool = new wxBitmapButton(_surfaceToolbar, id3d_sagoppTool,sagoppTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonSagOppTool->SetToolTip(_("Move to a right sagittal view"));
	_surfaceToolbar->AddControl(buttonSagOppTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonSagOppTool->GetSize().GetWidth()-1;

    this->SetToolBar(_surfaceToolbar);

	(_surfaceToolbar->FindControl(id3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
}

wxSurfaceRenderingGui::~wxSurfaceRenderingGui() {

	destroy3DCursor();
	destroy3DAxes();

	_mainGui=0;
	this->Destroy();
}

wxBitmap wxSurfaceRenderingGui::GetBitmapResource( const wxString& name ){

	wxUnusedVar(name);

	if (name == _T("icons/Zoom.xpm"))
		{
			wxBitmap bitmap(zoom_xpm);
			return bitmap;
		}
	if (name == _T("icons/Dolly.xpm"))
		{
			wxBitmap bitmap(dolly_xpm);
			return bitmap;
		}
	if (name == _T("icons/Cut3d.xpm"))
		{
			wxBitmap bitmap(cut3d_xpm);
			return bitmap;
		}
	else if (name == _T("icons/Rot3d.xpm"))
		{
			wxBitmap bitmap( rot3d_xpm);
			return bitmap;
		}
	else if (name == _T("icons/Move.xpm"))
		{
			wxBitmap bitmap( move_xpm);
			return bitmap;
		}
	else if (name == _T("icons/Rotate.xpm"))
		{
			wxBitmap bitmap( rotate_xpm);
			return bitmap;
		}
	else if (name == _T("icons/Axial.xpm"))
		{
			wxBitmap bitmap( axial_xpm);
			return bitmap;
		}
	else if (name == _T("icons/Coronal.xpm"))
		{
			wxBitmap bitmap( coronal_xpm);
			return bitmap;
		}
	else if (name == _T("icons/Sagittal.xpm"))
		{
			wxBitmap bitmap( sagittal_xpm);
			return bitmap;
		}
	else if (name == _T("icons/OppositeSagittal.xpm"))
		{
			wxBitmap bitmap( oppositesagittal_xpm);
			return bitmap;
		}
	else if (name == _T("icons/Wii.xpm"))
    {
        wxBitmap bitmap( wii_xpm);
		return bitmap;
	}
	return wxNullBitmap;

}


void wxSurfaceRenderingGui::onClose(wxCommandEvent& event) {  
	Close(TRUE);
}

void wxSurfaceRenderingGui::onQuit(wxCloseEvent& event) {
	if ( _mainGui->onViewerQuit(_idViewer)){
				event.Skip();
	}
}

void wxSurfaceRenderingGui::onChar(wxKeyEvent & event)
{
	if ( event.GetKeyCode() == 27 && ((appWxVtkInteractor*)_viewer3d->getWxWindow())->getInteractionType() == wiiSR )
		this->onWii(wxCommandEvent());
	//else event.Skip();   
}

void wxSurfaceRenderingGui::onFullScreen(wxCommandEvent& event) {
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	if ( appWxVtkI->getInteractionType() != wiiSR )
		this->ShowFullScreen(!this->IsFullScreen());
}

void wxSurfaceRenderingGui::onResetView(wxCommandEvent& event) {
	
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait...");
	wxBusyCursor cursor;

	// 3d cursor disattivato
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPCursorType1,_surfaceToolbar))->SetValue(true);
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
	this->hide3DCursor();

	double* focalPoint = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetCenter();
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0], focalPoint[1], focalPoint[2]);
	_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
	_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
	_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (focalPoint[0], focalPoint[1], focalPoint[2]+_distance);
	_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
	
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_surfaceToolbar->FindControl(id3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = id3d_3drotTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3dSR);

	// view mode
	((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->SetValue(true);
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();

	// stereo mode
	((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->SetValue(false);
	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	_viewer3d->getRenderWindow()->StereoRenderOff();
	_parallax = 0;

	// inizializza lo zoom
	//_zoomValuePerspectiveSR = 100;
	//_zoomValueParallelSR = 100;
	//_comboBoxZoom->SetValue("100%");
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(1/_globalZoomFactor); // in realtà al momento lo zoom non viene mai usato
	_globalZoomFactor = 1;

	// shading
	_shadingAmbient = 0.10;
	_shadingDiffuse = 1.00;
	_shadingSpecular = 1.00;
	_shadingSpecularPower = 50.0;
	_shadingColourAmbient = wxColour(70,70,70);
	_shadingColourDiffuse = wxColour(80,80,80);
	_shadingColourSpecular = wxColour(200,200,200);
	this->updateShading( _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower);
	
	this->setInitialDistance();
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}


void wxSurfaceRenderingGui::onAxialView(wxCommandEvent& event) {

	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetPosition(_initialPosition);

	float distance = _distance;

	if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("AXIAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, 0, -1);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("CORONAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, -1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 0, 1);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("SAGITTAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, -1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(-1, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->GetActiveCamera()->Dolly(1.5);
		_viewer3d->getRenderer()->ResetCamera();
	}

	// Apply the same zoom
	double vn[3], center[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(center);
	_viewer3d->getRenderer()->GetActiveCamera()->GetViewPlaneNormal(vn);
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition(center[0]+distance*vn[0], center[1]+distance*vn[1], center[2]+distance*vn[2]);
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(1/_globalZoomFactor);
	_globalZoomFactor=1;
	_viewer3d->getRenderer()->ResetCameraClippingRange();	
	this->updateStereoView(false);

	// ripristina il 3D cursor
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->AddActor( _3DCursor );

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}

}

void wxSurfaceRenderingGui::onCoronalView(wxCommandEvent& event) {

	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetPosition(_initialPosition);

	float distance = _distance;

	if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("CORONAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, 0, 1);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("AXIAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 0, 1);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("SAGITTAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (-1, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->ResetCamera();
	}

	// Apply the same zoom
	double vn[3], center[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(center);
	_viewer3d->getRenderer()->GetActiveCamera()->GetViewPlaneNormal(vn);
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition(center[0]+distance*vn[0], center[1]+distance*vn[1], center[2]+distance*vn[2]);
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(1/_globalZoomFactor);
	_globalZoomFactor=1;
	_viewer3d->getRenderer()->ResetCameraClippingRange();	
	this->updateStereoView(false);

	// ripristina il 3D cursor
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->AddActor( _3DCursor );

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onSagittalView(wxCommandEvent& event) {
	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetPosition(_initialPosition);

	float distance = _distance;

	if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("SAGITTAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, 0, 1);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->GetActiveCamera()->Dolly(1.5);
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("AXIAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (-1, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 0, 1);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->GetActiveCamera()->Dolly(1.5);
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("CORONAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (1, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->GetActiveCamera()->Dolly(1.5);
		_viewer3d->getRenderer()->ResetCamera();
	}

	// Apply the same zoom
	double vn[3], center[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(center);
	_viewer3d->getRenderer()->GetActiveCamera()->GetViewPlaneNormal(vn);
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition(center[0]+distance*vn[0], center[1]+distance*vn[1], center[2]+distance*vn[2]);
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(1/_globalZoomFactor);
	_globalZoomFactor=1;
	_viewer3d->getRenderer()->ResetCameraClippingRange();	
	this->updateStereoView(false);

	// ripristina il 3D cursor
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->AddActor( _3DCursor );

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onOppositeSagittalView(wxCommandEvent& event) {
	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetPosition(_initialPosition);

	float distance = _distance;

	if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("SAGITTAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, 0, -1);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->GetActiveCamera()->Dolly(1.5);
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("AXIAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (1, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 0, 1);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->GetActiveCamera()->Dolly(1.5);
		_viewer3d->getRenderer()->ResetCamera();
	}
	else if (!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getImageViewPlane().compare("CORONAL")) {
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (-1, 0, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
		_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
		_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
		_viewer3d->getRenderer()->GetActiveCamera()->Dolly(1.5);
		_viewer3d->getRenderer()->ResetCamera();
	}

	// Apply the same zoom
	double vn[3], center[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(center);
	_viewer3d->getRenderer()->GetActiveCamera()->GetViewPlaneNormal(vn);
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition(center[0]+distance*vn[0], center[1]+distance*vn[1], center[2]+distance*vn[2]);
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(1/_globalZoomFactor);
	_globalZoomFactor=1;
	_viewer3d->getRenderer()->ResetCameraClippingRange();	
	this->updateStereoView(false);

	// ripristina il 3D cursor
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->AddActor( _3DCursor );

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onChangeInputDevice(wxCommandEvent& event) {
	if(event.GetId() == id3d_RADIOBUTTONGROUPInputDevice2 ) 
		wxSurfaceRenderingGui::onWii(event);
}

void wxSurfaceRenderingGui::onStereo(wxCommandEvent& event) 
{
	if ( !( (wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar) )->GetValue() ) // modalità mono
	{		
		_viewer3d->getRenderWindow()->StereoRenderOff();

		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}		
	}
	else // modalità stereo
	{
		if ( _menuStereoMode->FindItem(m_3d_StereoModeActivePassive)->IsChecked() ) 
			onStereoModeActivePassive(event);
		else if ( _menuStereoMode->FindItem(m_3d_StereoModeAnaglyph)->IsChecked() ) 
			onStereoModeAnaglyph(event);
		else
			onStereoModeCheckerboard(event);
	}
}

void wxSurfaceRenderingGui::onStereoModeAnaglyph(wxCommandEvent& event) 
{
	// disattiva la visione parallela se attiva
	if ( ((wxRadioButton*)this->FindWindowById(id3d_parallel,_surfaceToolbar))->GetValue() ) 
	{
		((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->SetValue(true);
		this->onProjectionTypePerspective(event);
	}

	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyph)->Check(true);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassive)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeCheckerboard)->Check(false);

	((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToAnaglyph();
	this->updateStereoView(true);
}

void wxSurfaceRenderingGui::onStereoModeCheckerboard(wxCommandEvent& event) 
{
	// disattiva la visione parallela se attiva
	if ( ((wxRadioButton*)this->FindWindowById(id3d_parallel,_surfaceToolbar))->GetValue() ) 
	{
		((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->SetValue(true);
		this->onProjectionTypePerspective(event);
	}

	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyph)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassive)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeCheckerboard)->Check(true);

	((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToCheckerboard();
	this->updateStereoView(true);
}

void wxSurfaceRenderingGui::onStereoModeActivePassive(wxCommandEvent& event) 
{
	// disattiva la visione parallela se attiva
	if ( ((wxRadioButton*)this->FindWindowById(id3d_parallel,_surfaceToolbar))->GetValue() ) 
	{
		((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->SetValue(true);
		this->onProjectionTypePerspective(event);
	}

	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyph)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassive)->Check(true);
	_menuStereoMode->FindItem(m_3d_StereoModeCheckerboard)->Check(false);

	((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	this->updateStereoView(true);
}

void wxSurfaceRenderingGui::onModifyObserverDistance(wxCommandEvent& event) {
	const wxChar* PODChar;
	do {
		char message[60];
		strcpy(message,"Set the distance from the screen in cm");
		char initialValue[5];
		sprintf(initialValue,"%d",_POD);
		wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, "3D Volume Rendering", initialValue);
		if (dlg->ShowModal() == wxID_OK) {
			PODChar = dlg->GetValue().c_str();
			dlg->Destroy();
		}
		else {
			dlg->Destroy();
			return;
		}
	} while(atoi(PODChar) < 10);

	_POD = atoi(PODChar);
	this->updateStereoView();
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onCursorType( wxCommandEvent &event ) {
	if(event.GetId() == id3d_RADIOBUTTONGROUPCursorType1) {
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
		hide3DCursor();
	}
	else {
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOn();
		show3DCursor();
	}
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}
void wxSurfaceRenderingGui::onProjectionTypeParallel(wxCommandEvent& event) 
{
	// abilita il 3d cursor
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPCursorType2,_surfaceToolbar))->Enable();

	// disattiva lo stereo se presente
	if ( ((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->GetValue() ) 
	{
		((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->SetValue(false);
		this->onStereo(event);
	}

	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOn();

	// rimette a posto se c'è stato endoscopy
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetPosition(_initialPosition);
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint( ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->GetCenter() );
	_viewer3d->getRenderer()->ResetCameraClippingRange();

	// passo all'interactor style trackball camera
	vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetInteractorStyle(style);
	style->Delete();

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onProjectionTypePerspective(wxCommandEvent& event) 
{
	// abilita il 3d cursor
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPCursorType2,_surfaceToolbar))->Enable();

	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();

	// rimette a posto se c'è stato endoscopy
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->SetPosition(_initialPosition);
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint( ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->GetCenter() );
	_viewer3d->getRenderer()->ResetCameraClippingRange();


	// passo all'interactor style trackball camera
	vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetInteractorStyle(style);
	style->Delete();

	this->updateStereoView(true);
}

void wxSurfaceRenderingGui::onProjectionTypeEndoscopy(wxCommandEvent& event) 
{
	// 3d cursor disattivato
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPCursorType1,_surfaceToolbar))->SetValue(true);
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPCursorType2,_surfaceToolbar))->Disable();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
	this->hide3DCursor();

	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();

	// passo all'interactor style trackball actor
	vtkInteractorStyleTrackballActor *style = vtkInteractorStyleTrackballActor::New();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetInteractorStyle(style);
	style->Delete();

	this->updateStereoView(true);
}

void wxSurfaceRenderingGui::create3DAxes()
{
	if (_3DAxesActor != NULL) return;

	_3DAxes = vtkAxes::New();
	_3DAxes->SymmetricOn();
	
	vtkPolyDataMapper* axes_mapper = vtkPolyDataMapper::New();
	axes_mapper->SetInputConnection(_3DAxes->GetOutputPort());

	_3DAxesActor = vtkActor::New();
	_3DAxesActor->SetMapper(axes_mapper);
	_3DAxesActor->GetProperty()->SetAmbient(0.5);
	_3DAxesActor->VisibilityOff();
	_viewer3d->getRenderer()->AddActor( _3DAxesActor );

	//axes_mapper->Delete();
}

void wxSurfaceRenderingGui::destroy3DAxes() {
	if (_3DAxesActor == NULL) return;

	_3DAxesActor->Delete();
	_3DAxesActor = NULL;
	_3DAxes->Delete();
}

void wxSurfaceRenderingGui::show3DAxes() {
	if (_3DAxesActor == NULL) return;
	
	//Cerco la max distanza tra i piani dei bounds
	double b[6];
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetBounds(b);			
	double borderPoints[8][3] = {	{b[0],b[2],b[4]} ,		
									{b[0],b[2],b[5]} ,
									{b[0],b[3],b[4]} , 
									{b[0],b[3],b[5]} ,
									{b[1],b[2],b[4]} ,
									{b[1],b[2],b[5]} , 
									{b[1],b[3],b[4]} , 
									{b[1],b[3],b[5]}		};	
	double maxDist = sqrt(	pow(abs(borderPoints[0][0]-borderPoints[1][0]),2)	+ 
							pow(abs(borderPoints[0][1]-borderPoints[1][1]),2)	+
							pow(abs(borderPoints[0][2]-borderPoints[1][2]),2)	);
	double temp	= sqrt(	pow(abs(borderPoints[0][0]-borderPoints[2][0]),2)	+ 
						pow(abs(borderPoints[0][1]-borderPoints[2][1]),2)	+
						pow(abs(borderPoints[0][2]-borderPoints[2][2]),2)	);	
	if (temp>maxDist) maxDist = temp;
	temp = 	sqrt(	pow(abs(borderPoints[0][0]-borderPoints[4][0]),2)	+ 
					pow(abs(borderPoints[0][1]-borderPoints[4][1]),2)	+
					pow(abs(borderPoints[0][2]-borderPoints[4][2]),2)	);
	if (temp>maxDist) maxDist = temp;
	_3DAxes->SetScaleFactor(maxDist);
	
	// LUIGI - così gli assi erano centrati al centro dell'oggetto
	//double center[3];
	//center[0] = (b[1] + b[0])/2.0;
	//center[1] = (b[3] + b[2])/2.0;
	//center[2] = (b[5] + b[4])/2.0;
	
	// LUIGI - ora sono centrati sul focalPoint	
	double focalPosition[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPosition);
	_3DAxes->SetOrigin(focalPosition);

	_3DAxesActor->VisibilityOn();
}

void wxSurfaceRenderingGui::hide3DAxes() {
	if (_3DAxesActor == NULL) return;

	_3DAxesActor->VisibilityOff();
}


void wxSurfaceRenderingGui::create3DCursor()
{
	if (_3DCursor != NULL) return;

	double bounds[6];
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetBounds(bounds);			
	double radius = ( bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4] ) / 300;

	vtkSphereSource* _3DCursorSource = vtkSphereSource::New();
	_3DCursorSource->SetRadius(radius);				
	_3DCursorSource->SetPhiResolution(20);
	_3DCursorSource->SetThetaResolution(20);

	vtkDataSetMapper* _3DCursorMapper = vtkDataSetMapper::New();
	_3DCursorMapper->SetInput(_3DCursorSource->GetOutput());	
	_3DCursorMapper->ImmediateModeRenderingOn();

	_3DCursor = vtkActor::New();
	_3DCursor->SetMapper( _3DCursorMapper );
	_3DCursor->GetProperty()->SetColor(1,1,0);
	_viewer3d->getRenderer()->AddActor( _3DCursor );
	_3DCursor->VisibilityOff();

    _3DCursorSource->Delete();
	_3DCursorMapper->Delete();

	double focalPosition[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPosition);
	_3DCursor->SetPosition(focalPosition);

	// Build a cell locator
	_cellLocator = vtkCellLocator::New();
	_cellLocator->SetDataSet( ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetMapper()->GetInput() );
	_cellLocator->BuildLocator();
}

void wxSurfaceRenderingGui::destroy3DCursor()
{
	if (_3DCursor == NULL) return;

	_cellLocator->Delete();
	_3DCursor->Delete();
	_3DCursor = NULL;
	_cellLocator = NULL;
}

bool wxSurfaceRenderingGui::intersectingModelSurface(long x, long y, double * point)
{
	if (_cellLocator == NULL) return false;

	double closest, farthest;	
	this->getBoundsDepthInfo(closest, farthest);

	vtkRenderer* ren = _viewer3d->getRenderer();
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());			

	// Intersect with line
	double p1[4]; double p2[4];
	double t, pcoords[3]; int subId;
	appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, closest, p1);
	appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, farthest, p2);

	double distance = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
	_3DCursorIsOnModelSurface = _cellLocator->IntersectWithLine(p1, p2, 1/distance, t, point, pcoords, subId);
	return _3DCursorIsOnModelSurface;
}


void wxSurfaceRenderingGui::get3DCursorDisplayPosition(int & x, int & y, bool flipY)
{
	if (_3DCursor == NULL) return;

	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());			
	vtkRenderer* ren = _viewer3d->getRenderer();

	double worldPosition[3],displayPosition[3];
	_3DCursor->GetPosition( worldPosition );
	
	
	appWxVtkI->GetInteractorStyle()->ComputeWorldToDisplay( ren, worldPosition[0], worldPosition[1], 
																	worldPosition[2], displayPosition);				

	x = displayPosition[0];
	y = displayPosition[1];
	
	if (flipY) 
	{
		int* size = ren->GetRenderWindow()->GetSize();
		y = size[1] - y;
	}

	return;
}

bool wxSurfaceRenderingGui::set3DCursorPosition(long x, long y, bool flipY)
{
	if (_3DCursor == NULL) return false;

	vtkRenderer* ren = _viewer3d->getRenderer();
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());			
	
	if (flipY) 
	{
		int* size = ren->GetRenderWindow()->GetSize();
		y = size[1] - y;
	}

	// Intersect with line
	double point[3];	
	if ( intersectingModelSurface(x,y,point) )
	{
		_3DCursor->SetPosition(point[0],point[1],point[2]);
		_3DCursor->GetProperty()->SetColor(1,1,0);
		return true;
	}
	else
	{
		double focalDepth, focalPosition[3],newPosition[4];
		ren->GetActiveCamera()->GetFocalPoint(focalPosition);
		appWxVtkI->GetInteractorStyle()->ComputeWorldToDisplay( ren, focalPosition[0], focalPosition[1], 
																	focalPosition[2], focalPosition);				
		focalDepth = focalPosition[2];
		appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, focalDepth, newPosition);

		_3DCursor->SetPosition( newPosition[0], newPosition[1], newPosition[2]);
		_3DCursor->GetProperty()->SetColor(1,1,1);
		return false;
	}
}

bool wxSurfaceRenderingGui::is3DCursorBeyondFocalPlane()
{
	double* versor = _viewer3d->getRenderer()->GetActiveCamera()->GetDirectionOfProjection();
	double focalPosition[3], cursorPosition[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPosition);							
    _3DCursor->GetPosition(cursorPosition);	
			
	double cursorProjection, focalProjection;
	cursorProjection = versor[0] * cursorPosition[0] + versor[1] * cursorPosition[1] + versor[2] * cursorPosition[2];
	focalProjection = versor[0] * focalPosition[0] + versor[1] * focalPosition[1] + versor[2] * focalPosition[2];
	
	return (cursorProjection>focalProjection);
}
void wxSurfaceRenderingGui::show3DCursor()
{
	if (_3DCursor == NULL) return;

	_3DCursor->VisibilityOn();
}
void wxSurfaceRenderingGui::hide3DCursor()
{
	if (_3DCursor == NULL) return;

	_3DCursor->VisibilityOff();
}
void wxSurfaceRenderingGui::getBoundsDepthInfo(double & closest, double & farthest)
{
	double bounds[6];
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetBounds(bounds);			
	
	double temp[3];
	closest = 1.0;
	farthest = -1.0;

	double borderPoints[8][3] = {	{bounds[0],bounds[2],bounds[4]} ,		
									{bounds[0],bounds[2],bounds[5]} ,
									{bounds[0],bounds[3],bounds[4]} , 
									{bounds[0],bounds[3],bounds[5]} ,
									{bounds[1],bounds[2],bounds[4]} ,
									{bounds[1],bounds[2],bounds[5]} , 
									{bounds[1],bounds[3],bounds[4]} , 
									{bounds[1],bounds[3],bounds[5]}		};

    for (int i = 0; i<8; i++ )
	{
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->GetInteractorStyle()->ComputeWorldToDisplay( _viewer3d->getRenderer(), borderPoints[i][0], borderPoints[i][1], borderPoints[i][2], temp);				

		if (temp[2]>1) { // camera nel bounding box
			temp[2]=0.999999;
			farthest=0.999999;
			closest=-0.999999;
		}

		if ( temp[2] > farthest ) farthest = temp[2];
		if ( temp[2] < closest ) closest = temp[2];
	}

}


void wxSurfaceRenderingGui::onWiiConfiguration( wxCommandEvent &event )
{		
	appWxVtkInteractor* appWxVtkI = ( (appWxVtkInteractor*)_viewer3d->getWxWindow() );			
		
	wxWiiManager* WiiManager = appWxVtkI->getWiiManager( _viewer3d );			

	if ( appWxVtkI->getInteractionType() == wiiSR )		//Operazioni da fare in uscita dalla modalità Wii
	{		

		//controllo se posso uscire o meno
		if ( (appWxVtkI->idWiiEnabled != AnyWiiEnabled) || (WiiManager->WiiPointing_num) ) return;
		else if (appWxVtkI->getInteractionType() == voi3dVRwii) appWxVtkI->InvokeEvent(vtkWiiHomeDown,NULL);
		//if (!appWxVtkI->closeWiiAbility()) return;

		//_viewer3d->showLeftTopInfo();

		appWxVtkI->setInteractionType(rotateAround3dSR);				
		appWxVtkI->closeWiiManager();

		_selectedButton = id3d_rotateTool;	

		double currentFactor = ((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);		
		style->Delete();
		((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->SetMotionFactor ( currentFactor );
		
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x/2,y/2);
		//_viewer3d->getRenderWindow()->ShowCursor();
		this->show3DCursor();

		
		// full screen nascondendo l'oggetto
		double focalPoint[3];
		_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPoint);
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0]+10000,focalPoint[1]+10000,focalPoint[2]+10000);		
		_viewer3d->getRenderWindow()->Render();
		wglMakeCurrent(NULL, NULL);			
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}
		this->ShowFullScreen(false);	
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint);    
		_viewer3d->getRenderWindow()->Render();
		wglMakeCurrent(NULL, NULL);				
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}

		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice1,_surfaceToolbar))->SetValue(true);
	}
	else if ( WiiManager->SetupGui(_viewer3d->getWxWindow()) )
	{
		// passa alla visione perspective
		if ( !( ((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->GetValue() ) ) 
		{
			((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->SetValue(true);
			this->onProjectionTypePerspective(event);
		}

		_selectedButton = id3d_WiiTool;
		
		/*
		vtkInteractorStyleWII *style = vtkInteractorStyleWII::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
		*/

		double currentFactor = ((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->setupEventObservers(appWxVtkI);
		style->Delete();		
		((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->SetMotionFactor ( currentFactor );
           		
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x,y);
		//_viewer3d->getRenderWindow()->HideCursor();
		this->hide3DCursor();

		// full screen nascondendo l'oggetto			
		double focalPoint[3];
		_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPoint);
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0]+10000,focalPoint[1]+10000,focalPoint[2]+10000);	
		_viewer3d->getRenderWindow()->Render();
		wglMakeCurrent(NULL, NULL);						
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}
		this->ShowFullScreen(true);	
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint);			
		_viewer3d->getRenderWindow()->Render();
		wglMakeCurrent(NULL, NULL);						
		//_viewer3d->hideLeftTopInfo();
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}

		double bounds[6];
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetBounds(bounds);			
		double radius = (bounds[1]-bounds[0]+bounds[3]-bounds[2]+bounds[5]-bounds[4])*0.02/3;
		WiiManager->setPointerRadius(radius);
		WiiManager->Start();

		appWxVtkI->setInteractionType(wiiSR);			
		appWxVtkI->initInteractionSignals();	
		appWxVtkI->WiiManagerStarted = true;

		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice2,_surfaceToolbar))->SetValue(true);
	}
	else  {		
		appWxVtkI->closeWiiManager();
		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice1,_surfaceToolbar))->SetValue(true);
	}	
}

void wxSurfaceRenderingGui::onWii( wxCommandEvent &event )
{		
	appWxVtkInteractor* appWxVtkI = ( (appWxVtkInteractor*)_viewer3d->getWxWindow() );			
		
	wxWiiManager* WiiManager = appWxVtkI->getWiiManager( _viewer3d );			

	if ( appWxVtkI->getInteractionType() == wiiSR )		//Operazioni da fare in uscita dalla modalità Wii
	{		

		//controllo se posso uscire o meno
		//if ( (appWxVtkI->idWiiEnabled != AnyWiiEnabled) || (WiiManager->WiiPointing_num) ) return;
		//else if (appWxVtkI->getInteractionType() == voi3dVRwii) appWxVtkI->InvokeEvent(vtkWiiHomeDown,NULL);	
		//if (!appWxVtkI->closeWiiAbility()) return;

		//_viewer3d->showLeftTopInfo();

		appWxVtkI->setInteractionType(rotateAround3dSR);				
		appWxVtkI->closeWiiManager();

		_selectedButton = id3d_rotateTool;	

		double currentFactor = ((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);		
		style->Delete();
		((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->SetMotionFactor ( currentFactor );
		
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x/2,y/2);
		//_viewer3d->getRenderWindow()->ShowCursor();
		this->show3DCursor();

		
		// full screen nascondendo l'oggetto
		double focalPoint[3];
		_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPoint);
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0]+10000,focalPoint[1]+10000,focalPoint[2]+10000);		
		//_viewer3d->getRenderWindow()->Render();
		//wglMakeCurrent(NULL, NULL);			
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}
		this->ShowFullScreen(false);	
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint);    
		_viewer3d->getRenderWindow()->Render();
		//wglMakeCurrent(NULL, NULL);				
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}

		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice1,_surfaceToolbar))->SetValue(true);
	}
	else if ( WiiManager->QuickSetupGui(_viewer3d->getWxWindow()) )
	{
		// passa alla visione perspective
		if ( !( ((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->GetValue() ) ) 
		{
			((wxRadioButton*)this->FindWindowById(id3d_perspective,_surfaceToolbar))->SetValue(true);
			this->onProjectionTypePerspective(event);
		}

		_selectedButton = id3d_WiiTool;
		
		/*
		vtkInteractorStyleWII *style = vtkInteractorStyleWII::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
		*/

		double currentFactor = ((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->setupEventObservers(appWxVtkI);
		style->Delete();		
		((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->SetMotionFactor ( currentFactor );
	            	
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x,y);
		//_viewer3d->getRenderWindow()->HideCursor();
		this->hide3DCursor();

		// full screen nascondendo l'oggetto			
		double focalPoint[3];
		_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPoint);
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0]+10000,focalPoint[1]+10000,focalPoint[2]+10000);	
		//_viewer3d->getRenderWindow()->Render();
		//wglMakeCurrent(NULL, NULL);						
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}
		this->ShowFullScreen(true);						
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint);			
		_viewer3d->getRenderWindow()->Render();
		//wglMakeCurrent(NULL, NULL);						
		//_viewer3d->hideLeftTopInfo();
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}

		double bounds[6];
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetBounds(bounds);			
		double radius = (bounds[1]-bounds[0]+bounds[3]-bounds[2]+bounds[5]-bounds[4])*0.02/3;
		WiiManager->setPointerRadius(radius);
		WiiManager->Start();

		appWxVtkI->setInteractionType(wiiSR);			
		appWxVtkI->initInteractionSignals();	
		appWxVtkI->WiiManagerStarted = true;		

		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice2,_surfaceToolbar))->SetValue(true);
	}
	else  {		
		appWxVtkI->closeWiiManager();
		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice1,_surfaceToolbar))->SetValue(true);
	}
}

void wxSurfaceRenderingGui::onModifyCDRatioFunction(wxCommandEvent& WXUNUSED(event)) {
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	wxCDRatioDialog* d = new wxCDRatioDialog(this, -1, appWxVtkI->_minDISP / 1279.0, appWxVtkI->_maxDISP / 1279.0);
	if (d->ShowModal() == wxID_OK) {	  
		appWxVtkI->_minDISP = d->getCMin() * 1279;
		appWxVtkI->_maxDISP = d->getCMax() * 1279;
	}
}

void wxSurfaceRenderingGui::onModifyMotionFactor(wxCommandEvent& WXUNUSED(event)) {
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	if (appWxVtkI->getInteractionType() == wiiSR) {
		double currentFactor = ((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		wxMotionFactorDialog* d = new wxMotionFactorDialog(this, -1, currentFactor);
		if (d->ShowModal() == wxID_OK) 
			((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->SetMotionFactor ( d->getMFactor() );
	}
	else {
		double currentFactor = ((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		wxMotionFactorDialog* d = new wxMotionFactorDialog(this, -1, currentFactor);
		if (d->ShowModal() == wxID_OK) 
			((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->SetMotionFactor ( d->getMFactor() );
	}	
}


void wxSurfaceRenderingGui::onWiiChangeSceneIRInteraction( wxCommandEvent &event )
{
	appWxVtkInteractor* appWVI = (appWxVtkInteractor*)_viewer3d->getWxWindow();
	appWVI->_isSceneRotatingIR_1 = !appWVI->_isSceneRotatingIR_1;
}
void wxSurfaceRenderingGui::onDolly(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_surfaceToolbar->FindControl(id3d_dollyTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = id3d_dollyTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(dolly3dSR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onZoom(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_surfaceToolbar->FindControl(id3d_zoomTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = id3d_zoomTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(zoom3dSR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::setInitialDistance() {
	_distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
}

void wxSurfaceRenderingGui::setInitialProjectionValues() {
	 ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->GetOrientation( _initialOrientation );
	 ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->GetScale( _initialScale );
	 ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->GetPosition( _initialPosition );
}

void wxSurfaceRenderingGui::updateStereoView(bool doARender) 
{
	double eyeAngle = 0;	// convergence angle
	double parallax = 0;	// offset between focus distance (FD) and convergence distance (CD) -> parallax = FD - CD
							// interocular distance IOD = 63.5 mm
							// POD = viewing distance (VD)
							// eye angle (convergence angle) is computed as: 2 * tg-1 ( IOD / (2*CD) )
							// where CD = POD * distance / _distance 
							// _distance is the initial distance between the focal point (set at the center of the object) and the camera
	double distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
							// distance between the camera and the focal point
	double comfortZone = distance/_distance;
							// si cerca di contenere la depth bracket nella comfort zone

	if ( ((wxRadioButton*)this->FindWindowById(id3d_endoscopy,_surfaceToolbar))->GetValue() ) // endoscopy mode
	{
		double cameraPosition[3];
		_viewer3d->getRenderer()->GetActiveCamera()->GetPosition(cameraPosition);

		double closest, farthest;	
		this->getBoundsDepthInfo(closest, farthest);

		vtkRenderer* ren = _viewer3d->getRenderer();
		appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());			

		double p1[4], p2[4];
		appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, ren->GetSize()[0]/2, ren->GetSize()[1]/2, closest, p1);
		appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, ren->GetSize()[0]/2, ren->GetSize()[1]/2, farthest, p2);

		double firstVolumePoint[3], lastVolumePoint[3];
		for (int i=0; i<3; i++)
		{
			firstVolumePoint[i] = p1[i];
			lastVolumePoint[i] = p2[i];
		}

		if ( ((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->GetValue() ) // modalità stereo
		{
			_viewer3d->getRenderWindow()->StereoRenderOn();
			// angolo di convergenza sullo schermo
			eyeAngle = vtkMath::DegreesFromRadians( 2 * atan( (6.35/2) / _POD ) );
			// convergence distance = focal distance - parallax -> settiamo la parallax in modo da avere la convergenza (e quindi la parallasse 0) sullo schermo
			parallax = sqrt( vtkMath::Distance2BetweenPoints( cameraPosition, lastVolumePoint ) ); // parallasse solo positiva, tutto dietro lo schermo
		}
		// focal point sull'estremo del volume
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint ( lastVolumePoint );
	}

	else // perspective mode
	{
		if ( ((wxToggleButton*)this->FindWindowById(id3d_stereo,_surfaceToolbar))->GetValue() ) // modalità stereo
		{
			_viewer3d->getRenderWindow()->StereoRenderOn();
			if ( comfortZone>0.5 ) {
				eyeAngle = vtkMath::DegreesFromRadians( 2 * atan( (6.35/2) / (_POD * distance/_distance) ) );
				parallax = distance - _distance;
			}
			else {
				eyeAngle = vtkMath::DegreesFromRadians( 2 * atan( (6.35/2) / (_POD * 0.5) ) );
				parallax = -0.5 * _distance;
			}
		}
	}
	// camera perspective update
	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(eyeAngle);
	((vtkMitoCustomCamera*)(_viewer3d->getRenderer()->GetActiveCamera()))->SetParallax(parallax/_globalZoomFactor);

	if (doARender) 
	{
		try 
		{
			_viewer3d->updateViewer();
		}
		catch(...) 
		{
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}
	}
}

void wxSurfaceRenderingGui::updateZoom(double factor, bool doARender) {
	// camera - volume distance

	if ( _globalZoomFactor*factor < 1 ) return;

	_globalZoomFactor = _globalZoomFactor * factor;	
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(factor);

	if (doARender) {
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}
	}
}

/*
void wxSurfaceRenderingGui::onZoom(wxCommandEvent& event) {
	string value = _comboBoxZoom->GetValue().c_str();
	double zoom, tempValue;
	if(!value.compare("25%")) zoom = 25;
	else if(!value.compare("50%")) zoom = 50;
	else if(!value.compare("75%")) zoom = 75;
	else if(!value.compare("100%")) zoom = 100;
	else if(!value.compare("125%")) zoom = 125;
	else if(!value.compare("150%")) zoom = 150;
	else if(!value.compare("200%")) zoom = 200;
	else if(!value.compare("500%")) zoom = 500;
	if (_itemRadioBoxProjectionType->GetSelection()==1) {
		tempValue = _zoomValuePerspectiveSR;
		_zoomValuePerspectiveSR = zoom;
	}
	else {
		tempValue = _zoomValueParallelSR;
		_zoomValueParallelSR = zoom;
	}
	zoom = zoom/tempValue;
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(zoom);
	_viewer3d->updateViewer();
}
*/
void wxSurfaceRenderingGui::onRotate(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_surfaceToolbar->FindControl(id3d_rotateTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = id3d_rotateTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotate3d);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onMove(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_surfaceToolbar->FindControl(id3d_moveTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = id3d_moveTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(move3d);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onRotateAround(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_surfaceToolbar->FindControl(id3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = id3d_3drotTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3dSR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onActivate(wxActivateEvent& event) {
	if (_mainGui->onViewerActivate(_idViewer))
		event.Skip();
}


wxVtkViewer3d* wxSurfaceRenderingGui::new3dViewer(wxMainGui* mainGui) {	
	_mainGui = mainGui;
	_viewer3d = new wxVtkViewer3d(_mainGui, this, -1);
	_viewer3d->setAlgorithm(surfaceRendering);
	// inizializza al 100% il valore dello zoom
	//_zoomValuePerspectiveSR = 100;
	//_zoomValueParallelSR = 100;
	// inizializza la distanza iniziale tra camera e volume
	_distance = 0;
	// inizializza la distanza tra osservatore e schermo
	//_POD = 0;
	// inizializza i valori di zoom per parallel e perspective mode
	//_viewAngle = 0;
	//_parallelScale = 0;

	return _viewer3d;
}

void wxSurfaceRenderingGui::show(float contourValue, double decimateValue, double smoothValue) {
	wxWindowDisabler disabler;
	wxBusyInfo wait("Preparing 3D data...");
	wxBusyCursor cursor;

	_contourValue = contourValue;
	_decimateValue = decimateValue;
	_smoothValue = smoothValue;

	wxString contourValueTxt = _T(" value: ") + wxString::Format(_T("%6i"), (int)_contourValue);
	((wxStaticText*)this->FindWindowById(id3d_contourValueText,_surfaceToolbar))->SetLabel(contourValueTxt);

	double colourAmbient[3];
	colourAmbient[0] = ((double)_shadingColourAmbient.Red())/100;
	colourAmbient[1] = ((double)_shadingColourAmbient.Green())/100;
	colourAmbient[2] = ((double)_shadingColourAmbient.Blue())/100;
	double colourDiffuse[3];
	colourDiffuse[0] = ((double)_shadingColourDiffuse.Red())/100;
	colourDiffuse[1] = ((double)_shadingColourDiffuse.Green())/100;
	colourDiffuse[2] = ((double)_shadingColourDiffuse.Blue())/100;
	double colourSpecular[3];
	colourSpecular[0] = ((double)_shadingColourSpecular.Red())/100;
	colourSpecular[1] = ((double)_shadingColourSpecular.Green())/100;
	colourSpecular[2] = ((double)_shadingColourSpecular.Blue())/100;

	surfaceRenderingFilter surfaceRendering( _idData, _mainGui->getDataHandler());
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkActor( surfaceRendering.compute(contourValue, decimateValue, smoothValue, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower, colourAmbient, colourDiffuse, colourSpecular) );
	
	_viewer3d->showAppData(_mainGui->getDataHandler()->getData(_idData));
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
	Show(TRUE);

	create3DCursor();
	create3DAxes();

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3dTechnique(SurfaceRendering);			

	Raise();
}

void wxSurfaceRenderingGui::onShowAboutDialog(wxCommandEvent& WXUNUSED(event)) {
	const wxString name = "MITO - Medical imaging toolkit";
	const wxString version = "2.0";
	const wxString copyright = "(C) 2006-2011 Institute for High Performance Computing and Networking of the National Research Council of Italy (ICAR-CNR), Naples Branch";
	const wxString conjunction = "Institute of Biostructure and Bioimaging (IBB-CNR)";
	const wxString hyperlink = "<http://ihealthlab.icar.cnr.it/>";
	const wxString hyperlink2 = "<http://amico.icar.cnr.it/>";

	wxString msg;
    msg << name;
    msg << _(" Version ") << version << _T('\n');
    msg << copyright << _T('\n');
	msg << _("in conjunction with ") << conjunction << _T('\n');
    msg << hyperlink << _T('\n');
	msg << hyperlink2;

    wxMessageBox(msg, _T("About ") + name);
}
void wxSurfaceRenderingGui::set3DCursorRGBColor(double R, double G, double B)
{
	if (_3DCursor == NULL) return;
	_3DCursor->GetProperty()->SetColor(R,G,B);
}
void wxSurfaceRenderingGui::updateShading(double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower) {
	
	//((wxCheckBox*)this->FindWindowById(id3d_shadingCheckBox,_surfaceToolbar))->SetValue(true);
	//_isShadeActive = true;

	double colourAmbient[3];
	colourAmbient[0] = ((double)_shadingColourAmbient.Red())/100;
	colourAmbient[1] = ((double)_shadingColourAmbient.Green())/100;
	colourAmbient[2] = ((double)_shadingColourAmbient.Blue())/100;
	double colourDiffuse[3];
	colourDiffuse[0] = ((double)_shadingColourDiffuse.Red())/100;
	colourDiffuse[1] = ((double)_shadingColourDiffuse.Green())/100;
	colourDiffuse[2] = ((double)_shadingColourDiffuse.Blue())/100;
	double colourSpecular[3];
	colourSpecular[0] = ((double)_shadingColourSpecular.Red())/100;
	colourSpecular[1] = ((double)_shadingColourSpecular.Green())/100;
	colourSpecular[2] = ((double)_shadingColourSpecular.Blue())/100;

	_shadingAmbient = shadingAmbient;
	_shadingDiffuse = shadingDiffuse;
	_shadingSpecular = shadingSpecular;
	_shadingSpecularPower = shadingSpecularPower;
	
	vtkProperty *actorProperty = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkActor()->GetProperty();
	actorProperty->SetAmbientColor(colourAmbient[0], colourAmbient[1], colourAmbient[2]);
	actorProperty->SetDiffuseColor(colourDiffuse[0], colourDiffuse[1], colourDiffuse[2]);
	actorProperty->SetSpecularColor(colourSpecular[0], colourSpecular[1], colourSpecular[2]);
	actorProperty->SetAmbient(_shadingAmbient);
	actorProperty->SetDiffuse(_shadingDiffuse);
	actorProperty->SetSpecular(_shadingSpecular);
	actorProperty->SetSpecularPower(_shadingSpecularPower);
	
	wxString ambientTxt = wxString::Format(_T("%1.2f"), _shadingAmbient);
	wxString diffuseTxt = wxString::Format(_T("%1.2f"), _shadingDiffuse);
	wxString specularTxt = wxString::Format(_T("%1.2f"), _shadingSpecular);
	wxString specularPowerTxt = wxString::Format(_T("%2.1f"), _shadingSpecularPower);
	wxString specularAllTxt = specularTxt + _T(", ") + specularPowerTxt;

	((wxStaticText*)this->FindWindowById(id3d_shadingAmbientText,_surfaceToolbar))->SetLabel(ambientTxt);
	((wxStaticText*)this->FindWindowById(id3d_shadingDiffuseText,_surfaceToolbar))->SetLabel(diffuseTxt);
	((wxStaticText*)this->FindWindowById(id3d_shadingSpecularText,_surfaceToolbar))->SetLabel(specularAllTxt);

	_viewer3d->updateViewer();
}

void wxSurfaceRenderingGui::onChangeShadingParameters( wxCommandEvent &event ) {
	
	// calcola dove posizionare la finestra
	int x = 14;
	int y = 14;
	int* xClient = &x;
	int* yClient = &y;
	this->ClientToScreen(xClient,yClient);
	
	wxShadingDialog* d = new wxShadingDialog(1, this, -1);
	d->SetSize(*xClient,*yClient,-1,-1);

	if (d->ShowModal() == wxID_OK) {	  
	}
}

void wxSurfaceRenderingGui::computeSurfaceRendering(float contourValue, double decimateValue, double smoothValue)
{
	_contourValue = contourValue;
	_decimateValue = decimateValue;
	_smoothValue = smoothValue;

	wxString contourValueTxt = _T(" value: ") + wxString::Format(_T("%6i"), (int)_contourValue);
	((wxStaticText*)this->FindWindowById(id3d_contourValueText,_surfaceToolbar))->SetLabel(contourValueTxt);

	double colourAmbient[3];
	colourAmbient[0] = ((double)_shadingColourAmbient.Red())/100;
	colourAmbient[1] = ((double)_shadingColourAmbient.Green())/100;
	colourAmbient[2] = ((double)_shadingColourAmbient.Blue())/100;
	double colourDiffuse[3];
	colourDiffuse[0] = ((double)_shadingColourDiffuse.Red())/100;
	colourDiffuse[1] = ((double)_shadingColourDiffuse.Green())/100;
	colourDiffuse[2] = ((double)_shadingColourDiffuse.Blue())/100;
	double colourSpecular[3];
	colourSpecular[0] = ((double)_shadingColourSpecular.Red())/100;
	colourSpecular[1] = ((double)_shadingColourSpecular.Green())/100;
	colourSpecular[2] = ((double)_shadingColourSpecular.Blue())/100;

	if ( ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor() )
		_viewer3d->getRenderer()->RemoveActor(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor());

	surfaceRenderingFilter surfaceRendering( _idData, _mainGui->getDataHandler());

	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkActor( surfaceRendering.compute(contourValue, decimateValue, smoothValue, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower, colourAmbient, colourDiffuse, colourSpecular) );

	if ( _3DCursor != NULL ) 
	{
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );
		this->destroy3DCursor();
		this->create3DCursor();
		if ( ((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPCursorType2,_surfaceToolbar))->GetValue()) 
		{
			((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOn();
			show3DCursor();
		}
	}
	_viewer3d->getRenderer()->AddActor(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor());
	_viewer3d->updateViewer();

	this->SetTitle(_T("MITO: Surface Rendering"));
}
