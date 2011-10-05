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
#include <vtkInteractorStyleWII.h>
#include <vtkInteractorStyleWIITrackball.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "wxCDRatioDialog.h"
#include "wxMotionFactorDialog.h"


BEGIN_EVENT_TABLE(wxSurfaceRenderingGui, wxFrame)
	EVT_ACTIVATE(wxSurfaceRenderingGui::onActivate)
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
	EVT_MENU(m_3d_ParallelProjectionType,wxSurfaceRenderingGui::onProjectionTypeParallel)
	EVT_MENU(m_3d_PerspectiveProjectionType,wxSurfaceRenderingGui::onProjectionTypePerspective)
	EVT_MENU(m_3d_ModifyObserverDistance,wxSurfaceRenderingGui::onModifyObserverDistance)
	EVT_BUTTON(id3d_moveTool,wxSurfaceRenderingGui::onMove)
	EVT_BUTTON(id3d_dollyTool,wxSurfaceRenderingGui::onDolly)
	EVT_BUTTON(id3d_zoomTool,wxSurfaceRenderingGui::onZoom)
	EVT_BUTTON(id3d_rotateTool,wxSurfaceRenderingGui::onRotate)
	EVT_BUTTON(id3d_3drotTool,wxSurfaceRenderingGui::onRotateAround)
	EVT_BUTTON(id3d_AxialTool,wxSurfaceRenderingGui::onAxialView)	
	EVT_BUTTON(id3d_CorTool,wxSurfaceRenderingGui::onCoronalView)	
	EVT_BUTTON(id3d_SagTool,wxSurfaceRenderingGui::onSagittalView)
	EVT_BUTTON(id3d_sagoppTool,wxSurfaceRenderingGui::onOppositeSagittalView)

	// Wiimote
	EVT_MENU(id3d_WiiTool,wxSurfaceRenderingGui::onWii)
	EVT_MENU(id3d_WiiConfigurationTool,wxSurfaceRenderingGui::onWiiConfiguration)
	EVT_MENU(id3d_WiiChangeSceneIRInteraction,wxSurfaceRenderingGui::onWiiChangeSceneIRInteraction)
	EVT_MENU(id3d_ModifyCDRatioFunction,wxSurfaceRenderingGui::onModifyCDRatioFunction)
	EVT_MENU(id3d_ModifyMotionFactor,wxSurfaceRenderingGui::onModifyMotionFactor)

	//EVT_COMBOBOX(id3d_COMBOBOXZoom,wxSurfaceRenderingGui::onZoom)
	//EVT_SLIDER(id3d_SLIDER1, wxSurfaceRenderingGui::onEyeAngle)

	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPStereoMode1,wxSurfaceRenderingGui::onStereo)
	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPStereoMode2,wxSurfaceRenderingGui::onStereo)

	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPInputDevice1,wxSurfaceRenderingGui::onChangeInputDevice)
	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPInputDevice2,wxSurfaceRenderingGui::onChangeInputDevice)

	EVT_RADIOBOX(id3d_RADIOBOXProjectionType,wxSurfaceRenderingGui::onProjectionType)

	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPCursorType1,wxSurfaceRenderingGui::onCursorType)
	EVT_RADIOBUTTON(id3d_RADIOBUTTONGROUPCursorType2,wxSurfaceRenderingGui::onCursorType)

	EVT_CHAR_HOOK(wxSurfaceRenderingGui::onChar)

	EVT_CLOSE(wxSurfaceRenderingGui::onQuit)
END_EVENT_TABLE()


//wxSurfaceRenderingGui::wxSurfaceRenderingGui(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxDOUBLE_BORDER | wxMAXIMIZE) {
wxSurfaceRenderingGui::wxSurfaceRenderingGui(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE) {

	/*wxIcon icon;
	icon.LoadFile("icon.ico",wxBITMAP_TYPE_ICO);
	this->SetIcon(icon);*/

	SetIcon(wxICON(MITO));

	_selectedButton = id3d_3drotTool;
	_POD = 400;

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
	_menuStereoMode->Append(m_3d_ModifyObserverDistance,_("Modify observer distance\tCtrl-B"),_T(""),wxITEM_NORMAL);
	menu3dViewer->Append(m_3d_StereoMode,"Stereo Mode",_menuStereoMode);
	

	/*
	_menuProjectionType = new wxMenu; 
	_menuProjectionType->Append(m_3d_ParallelProjectionType,_("Parallel"),_T(""),wxITEM_NORMAL);
	_menuProjectionType->Append(m_3d_PerspectiveProjectionType,_("Perspective"),_T(""),wxITEM_NORMAL);
	menu3dViewer->Append(m_3d_ProjectionType,"Projection Type",_menuProjectionType);
	*/


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

	// Wii
	wxBitmap WiiTool(this->GetBitmapResource(wxT("icons/Wii.xpm")));

	_surfaceToolbar->SetToolBitmapSize(wxSize(28,65));
	_surfaceToolbar->Realize();
	_surfaceToolbar->SetRows(1);

	//Raggruppamento dei radioButton associati al inputDevice
	wxPanel* inputDeviceRadioPanel = new wxPanel(_surfaceToolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	inputDeviceRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* inputDeviceStaticBox = new wxStaticBox(inputDeviceRadioPanel, wxID_ANY, "Input Device"); 	
	wxStaticBoxSizer* inputDeviceRadioSizer = new wxStaticBoxSizer( inputDeviceStaticBox, wxVERTICAL );	  
	wxBoxSizer* inputDeviceRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB1 = new wxRadioButton(inputDeviceRadioPanel, id3d_RADIOBUTTONGROUPInputDevice1, "");	
	inputDeviceB1->SetForegroundColour(*wxWHITE);  
	inputDeviceB1->SetValue(true);
	wxStaticText* inputDeviceB1Text = new wxStaticText(inputDeviceRadioPanel,wxID_ANY,"Mouse");
	inputDeviceB1Text->SetForegroundColour(*wxWHITE);  
	inputDeviceRow1->Add( inputDeviceB1 , 0, wxLEFT, 0); 
	inputDeviceRow1->Add( inputDeviceB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* inputDeviceRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB2 = new wxRadioButton(inputDeviceRadioPanel, id3d_RADIOBUTTONGROUPInputDevice2, "");
	wxStaticText* inputDeviceB2Text = new wxStaticText(inputDeviceRadioPanel,wxID_ANY,"Wiimote");
	inputDeviceB2Text->SetForegroundColour(*wxWHITE);  
	inputDeviceRow2->Add( inputDeviceB2 , 0, wxRIGHT, 3); 
	inputDeviceRow2->Add( inputDeviceB2Text , 0, wxRIGHT, 20);	 
	inputDeviceRadioSizer->Add( inputDeviceRow1 , 0, wxTOP , 6); 
	inputDeviceRadioSizer->Add( inputDeviceRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	inputDeviceRadioPanel->SetSizerAndFit(inputDeviceRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+inputDeviceRadioPanel->GetSize().GetWidth();
	
	//Contenitore raggruppante i Mouse Functions Buttons
	currentHorizontalPosition=currentHorizontalPosition+5;	
	wxStaticBox* mouseFunctionsButtonsBox = new wxStaticBox(_surfaceToolbar, wxID_ANY, wxString("Mouse Functions"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(210,64));

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* button3drotTool = new wxBitmapButton(_surfaceToolbar, id3d_3drotTool, rot3dTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	button3drotTool->SetToolTip(_("ROTATE 3D"));
	_surfaceToolbar->AddControl(button3drotTool);
	currentHorizontalPosition=currentHorizontalPosition+button3drotTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonDolly = new wxBitmapButton(_surfaceToolbar, id3d_dollyTool, dollyTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonDolly->SetToolTip(_("DOLLY"));
	_surfaceToolbar->AddControl(buttonDolly);
	currentHorizontalPosition=currentHorizontalPosition+buttonDolly->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonZoom = new wxBitmapButton(_surfaceToolbar, id3d_zoomTool,zoomTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonZoom->SetToolTip(_("ZOOM"));
	_surfaceToolbar->AddControl(buttonZoom);
	currentHorizontalPosition=currentHorizontalPosition+buttonZoom->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonRotate = new wxBitmapButton(_surfaceToolbar, id3d_rotateTool,RotateTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonRotate->SetToolTip(_("ROLL"));
	_surfaceToolbar->AddControl(buttonRotate);
	currentHorizontalPosition=currentHorizontalPosition+buttonRotate->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonMove = new wxBitmapButton(_surfaceToolbar, id3d_moveTool,moveTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonMove->SetToolTip(_("PAN"));
	_surfaceToolbar->AddControl(buttonMove);
	currentHorizontalPosition=currentHorizontalPosition+buttonMove->GetSize().GetWidth()-1;


	//Raggruppamento dei radioButton associati al cursorType
	currentHorizontalPosition=currentHorizontalPosition+15;
	wxPanel* cursorTypeRadioPanel = new wxPanel(_surfaceToolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	cursorTypeRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* cursorTypeStaticBox = new wxStaticBox(cursorTypeRadioPanel, wxID_ANY, "Cursor Type"); 	
	wxStaticBoxSizer* cursorTypeRadioSizer = new wxStaticBoxSizer( cursorTypeStaticBox, wxVERTICAL );	  
	wxBoxSizer* cursorTypeRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB1 = new wxRadioButton(cursorTypeRadioPanel, id3d_RADIOBUTTONGROUPCursorType1, "");	
	cursorTypeB1->SetForegroundColour(*wxWHITE);  
	cursorTypeB1->SetValue(true);
	wxStaticText* cursorTypeB1Text = new wxStaticText(cursorTypeRadioPanel,wxID_ANY,"2D Cursor");
	cursorTypeB1Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow1->Add( cursorTypeB1 , 0, wxLEFT, 0); 
	cursorTypeRow1->Add( cursorTypeB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* cursorTypeRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB2 = new wxRadioButton(cursorTypeRadioPanel, id3d_RADIOBUTTONGROUPCursorType2, "");
	wxStaticText* cursorTypeB2Text = new wxStaticText(cursorTypeRadioPanel,wxID_ANY,"3D Cursor");
	cursorTypeB2Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow2->Add( cursorTypeB2 , 0, wxRIGHT, 3); 
	cursorTypeRow2->Add( cursorTypeB2Text , 0, wxRIGHT, 10);	 
	cursorTypeRadioSizer->Add( cursorTypeRow1 , 0, wxTOP , 6); 
	cursorTypeRadioSizer->Add( cursorTypeRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	cursorTypeRadioPanel->SetSizerAndFit(cursorTypeRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+cursorTypeRadioPanel->GetSize().GetWidth();


	//Raggruppamento dei radioButton associati al stereoMode
	currentHorizontalPosition=currentHorizontalPosition+5;
	wxPanel* stereoModeRadioPanel = new wxPanel(_surfaceToolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	stereoModeRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* stereoModeStaticBox = new wxStaticBox(stereoModeRadioPanel, wxID_ANY, "Stereo"); 	
	wxStaticBoxSizer* stereoModeRadioSizer = new wxStaticBoxSizer( stereoModeStaticBox, wxVERTICAL );	  
	wxBoxSizer* stereoModeRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* stereoModeB1 = new wxRadioButton(stereoModeRadioPanel, id3d_RADIOBUTTONGROUPStereoMode1, "");	
	stereoModeB1->SetForegroundColour(*wxWHITE);  
	stereoModeB1->SetValue(true);
	wxStaticText* stereoModeB1Text = new wxStaticText(stereoModeRadioPanel,wxID_ANY,"Off");
	stereoModeB1Text->SetForegroundColour(*wxWHITE);  
	stereoModeRow1->Add( stereoModeB1 , 0, wxLEFT, 0); 
	stereoModeRow1->Add( stereoModeB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* stereoModeRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* stereoModeB2 = new wxRadioButton(stereoModeRadioPanel, id3d_RADIOBUTTONGROUPStereoMode2, "");
	wxStaticText* stereoModeB2Text = new wxStaticText(stereoModeRadioPanel,wxID_ANY,"On");
	stereoModeB2Text->SetForegroundColour(*wxWHITE);  
	stereoModeRow2->Add( stereoModeB2 , 0, wxRIGHT, 3); 
	stereoModeRow2->Add( stereoModeB2Text , 0, wxRIGHT, 15);	 
	stereoModeRadioSizer->Add( stereoModeRow1 , 0, wxTOP , 6); 
	stereoModeRadioSizer->Add( stereoModeRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	stereoModeRadioPanel->SetSizerAndFit(stereoModeRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+stereoModeRadioPanel->GetSize().GetWidth();




	/*
	_itemSlider = new wxSlider(_surfaceToolbar, id3d_SLIDER1, 2, -5, +5, wxPoint(390,30), wxSize(150,-1), wxSL_HORIZONTAL);
	_surfaceToolbar->AddControl(_itemSlider);
	wxStaticText* TestoEyeAngle = new wxStaticText( _surfaceToolbar, wxID_STATIC, _("Eye Angle"), wxPoint(445,65), wxDefaultSize, 0 );
	int altezzafinecoarse=10;
	wxStaticText* TestoSinistraSlider = new wxStaticText(_surfaceToolbar, wxID_STATIC, _("-5"), wxPoint(390,altezzafinecoarse), wxDefaultSize, 0 );
	wxStaticText* TestoDestraSlider = new wxStaticText(_surfaceToolbar, wxID_STATIC, _("+5"), wxPoint(524,altezzafinecoarse), wxDefaultSize, 0 );
	*/

	//currentHorizontalPosition=currentHorizontalPosition+10;
//	wxString radioBoxProjectionTypeStrings[] = {
//        _("Parallel"),
//        _("Perspective")
//    };
//	_itemRadioBoxProjectionType = new wxRadioBox( _surfaceToolbar, id3d_RADIOBOXProjectionType, _("Projection Type"), wxPoint(currentHorizontalPosition,altezzaradiobox), wxSize(-1,-1), 2, radioBoxProjectionTypeStrings, 1, wxRA_SPECIFY_COLS );
//	_itemRadioBoxProjectionType->SetSelection(1);
//    _surfaceToolbar->AddControl(_itemRadioBoxProjectionType);
//	currentHorizontalPosition=currentHorizontalPosition+_itemRadioBoxProjectionType->GetSize().GetWidth()-1;
//	_itemRadioBoxProjectionType->Show(false);


	//Contenitore raggruppante i View Functions Buttons
	currentHorizontalPosition=currentHorizontalPosition+5;	
	wxStaticBox* viewButtonsBox = new wxStaticBox(_surfaceToolbar, wxID_ANY, wxString("View Functions"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(170,64));

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonAxialTool = new wxBitmapButton(_surfaceToolbar, id3d_AxialTool,AxialTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	_surfaceToolbar->AddControl(buttonAxialTool);
	buttonAxialTool->SetToolTip(_("Move to an axial view"));
	currentHorizontalPosition=currentHorizontalPosition+buttonAxialTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonCorTool = new wxBitmapButton(_surfaceToolbar, id3d_CorTool,CorTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonCorTool->SetToolTip(_("Move to a coronal view"));
	_surfaceToolbar->AddControl(buttonCorTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonCorTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonSagTool = new wxBitmapButton(_surfaceToolbar, id3d_SagTool,SagTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonSagTool->SetToolTip(_("Move to a left sagittal view"));
	_surfaceToolbar->AddControl(buttonSagTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonSagTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonSagOppTool = new wxBitmapButton(_surfaceToolbar, id3d_sagoppTool,sagoppTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonSagOppTool->SetToolTip(_("Move to a right sagittal view"));
	_surfaceToolbar->AddControl(buttonSagOppTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonSagOppTool->GetSize().GetWidth()-1;

	/*
	wxStaticText* TestoAxial = new wxStaticText(_surfaceToolbar, -1, _("Axial"), wxPoint(594,65), wxDefaultSize, 0 );
	wxStaticText* TestoCoronal = new wxStaticText(_surfaceToolbar, -1, _("Coronal"), wxPoint(637,65), wxDefaultSize, 0 );
	wxStaticText* TestoSagittal = new wxStaticText(_surfaceToolbar, -1, _("Sagittal"), wxPoint(688,65), wxDefaultSize, 0 );
	wxStaticText* TestoOppSagittal = new wxStaticText(_surfaceToolbar, -1, _("Op. Sagittal"), wxPoint(728,65), wxDefaultSize, 0 );
	*/

	//_surfaceToolbar->SetToolBitmapSize(wxSize(32,50));
    this->SetToolBar(_surfaceToolbar);

	(_surfaceToolbar->FindControl(id3d_3drotTool))->SetBackgroundColour(*wxCYAN);
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

	/*
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint (0, 0, 0);
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (0, 0, 1);
	_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
	_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
	_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
	_viewer3d->getRenderer()->ResetCamera();
	*/

	// inizializza la posizione della camera e il focal point
	double* focalPoint = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()->GetCenter();
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0], focalPoint[1], focalPoint[2]);
	_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
	_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
	_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (focalPoint[0], focalPoint[1], focalPoint[2]+_distance);
	_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);


	
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour((_surfaceToolbar->FindControl(id3d_3drotTool))->GetBackgroundColour());
	(_surfaceToolbar->FindControl(id3d_3drotTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = id3d_3drotTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3dSR);

	// inizializza al 100% il fattore di zoom sulla toolbar
	//_zoomValuePerspectiveSR = 100;
	//_zoomValueParallelSR = 100;
	//_comboBoxZoom->SetValue("100%");

	// inizializza a OFF la vista stereoscopica sulla toolbar
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPStereoMode1,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(0);
	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	_parallax = 0;
	 //_viewer3d->getRenderWindow()->StereoRenderOff();

	// inizializza a Perpective la vista stereoscopica sulla toolbar
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	
	// inizializza lo zoom
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(1/_globalZoomFactor);
	_globalZoomFactor = 1;

	// inizializza a 2.0 il valore dell'eye angle
	//_itemSlider->SetValue(2.0);
	//_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle((_itemSlider->GetValue()));

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
	this->setInitialDistance();
}


void wxSurfaceRenderingGui::onAxialView(wxCommandEvent& event) {
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
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onCoronalView(wxCommandEvent& event) {
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
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onSagittalView(wxCommandEvent& event) {
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
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onOppositeSagittalView(wxCommandEvent& event) {
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
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onChangeInputDevice(wxCommandEvent& event) {
	if(event.GetId() == id3d_RADIOBUTTONGROUPInputDevice2 ) 
		wxSurfaceRenderingGui::onWii(event);
}

void wxSurfaceRenderingGui::onStereo(wxCommandEvent& event) {
	if (event.GetId() == id3d_RADIOBUTTONGROUPStereoMode1) {		
		// setta a 0 l'eye angle
		_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(0);
		_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
		
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			Close(TRUE);
			return;
		}		
	}
	else {
		if ( _menuStereoMode->FindItem(m_3d_StereoModeActivePassive)->IsChecked() ) 
			onStereoModeActivePassive(event);
		else 
			onStereoModeAnaglyph(event);
	}
}

void wxSurfaceRenderingGui::onStereoModeAnaglyph(wxCommandEvent& event) {
		// disabilita lo zoom
	//_comboBoxZoom->Enable(false);
	//_zoomValuePerspectiveSR = _zoomValueParallelSR = 100;
	//_comboBoxZoom->SetValue("100%");
	//_viewer3d->getRenderer()->GetActiveCamera()->SetViewAngle(30);
	//_viewer3d->getRenderer()->GetActiveCamera()->SetParallelScale(1);

	// setta il tipo di proiezione a perspective
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyph)->Check(true);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassive)->Check(false);

	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPStereoMode2,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToAnaglyph();
	this->updateStereoView();
	//this->updateStereoView(0);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
	/*
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPStereo2,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->StereoRenderOff();
	_viewer3d->updateViewer();
	_viewer3d->getRenderWindow()->SetStereoTypeToAnaglyph();
	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle((_itemSlider->GetValue()));
	_viewer3d->getRenderWindow()->StereoRenderOn();
	_viewer3d->updateViewer();
	*/
}

void wxSurfaceRenderingGui::onStereoModeActivePassive(wxCommandEvent& event) {
	// disabilita lo zoom
	//_comboBoxZoom->Enable(false);
	//_zoomValuePerspectiveSR = _zoomValueParallelSR = 100;
	//_comboBoxZoom->SetValue("100%");
	//_viewer3d->getRenderer()->GetActiveCamera()->SetViewAngle(30);
	//_viewer3d->getRenderer()->GetActiveCamera()->SetParallelScale(1);

	// setta il tipo di proiezione a perspective
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyph)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassive)->Check(true);

	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPStereoMode2,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	this->updateStereoView();
	//this->updateStereoView(0);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
	/*
	((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPStereo2,_surfaceToolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle((_itemSlider->GetValue()));
	_viewer3d->getRenderWindow()->StereoRenderOn();
	_viewer3d->updateViewer();
	*/
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
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

/*
void wxSurfaceRenderingGui::onEyeAngle( wxCommandEvent &event ) {
	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle((_itemSlider->GetValue()));
	_viewer3d->updateViewer();
}
*/


void wxSurfaceRenderingGui::onProjectionType( wxCommandEvent &event ) {
	if(event.GetInt() == 0) {
		_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	}
	else {
		_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	}
	//wxSurfaceRenderingGui::onZoom(event);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}
void wxSurfaceRenderingGui::onProjectionTypeParallel(wxCommandEvent& event) {
//	_itemRadioBoxProjectionType->SetSelection(0);
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	//wxSurfaceRenderingGui::onZoom(event);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onProjectionTypePerspective(wxCommandEvent& event) {
//	_itemRadioBoxProjectionType->SetSelection(1);
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	//wxSurfaceRenderingGui::onZoom(event);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
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
	double radius = (bounds[1]-bounds[0]+bounds[3]-bounds[2]+bounds[5]-bounds[4])*0.02/3;

	vtkSphereSource* _3DCursorSource = vtkSphereSource::New();
	_3DCursorSource->SetRadius(radius);				
	_3DCursorSource->SetPhiResolution(20);
	_3DCursorSource->SetThetaResolution(20);

	vtkDataSetMapper* _3DCursorMapper = vtkDataSetMapper::New();
	_3DCursorMapper->SetInput(_3DCursorSource->GetOutput());	
	_3DCursorMapper->ImmediateModeRenderingOn();

	_3DCursor = vtkActor::New();
	_3DCursor->SetMapper( _3DCursorMapper );
	_3DCursor->GetProperty()->SetColor(1,0,0);
	_viewer3d->getRenderer()->AddActor( _3DCursor );
	_3DCursor->VisibilityOff();

    _3DCursorSource->Delete();
	_3DCursorMapper->Delete();

	double focalPosition[3];
	_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPosition);
	_3DCursor->SetPosition(focalPosition);

	// Build a locator 
	_cellLocator = vtkCellLocator::New();
	_cellLocator->SetDataSet( ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkActor()
																					->GetMapper()->GetInput() );			
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
	getBoundsDepthInfo(closest, farthest);

	vtkRenderer* ren = _viewer3d->getRenderer();
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());			

	// Intersect with line
	double p1[4]; double p2[4];
	double t, pcoords[3]; int subId;
	appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, closest, p1);
	appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, farthest, p2);

	double distance = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	//_3DCursorIsOnModelSurface = _cellLocator->IntersectWithLine(p1, p2, 0.001, t, point, pcoords, subId);
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
		_3DCursor->GetProperty()->SetColor(0,1,0);
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
		_3DCursor->GetProperty()->SetColor(1,0,0);
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
	
	double borderPoints[8][3] = {	{bounds[0],bounds[2],bounds[4]} ,		
									{bounds[0],bounds[2],bounds[5]} ,
									{bounds[0],bounds[3],bounds[4]} , 
									{bounds[0],bounds[3],bounds[5]} ,
									{bounds[1],bounds[2],bounds[4]} ,
									{bounds[1],bounds[2],bounds[5]} , 
									{bounds[1],bounds[3],bounds[4]} , 
									{bounds[1],bounds[3],bounds[5]}		};

	closest = 9999;	
	farthest = -9999;

    for (int i = 0; i<8; i++ )
	{
		double temp[3];
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->
		GetInteractorStyle()->ComputeWorldToDisplay( _viewer3d->getRenderer(), borderPoints[i][0], 
																borderPoints[i][1], borderPoints[i][2], temp);				

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

		double currentFactor = ((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->MotionFactor;
		vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);		
		style->Delete();
		((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->MotionFactor = currentFactor;
		
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			Close(TRUE);
			return;
		}

		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice1,_surfaceToolbar))->SetValue(true);
	}
	else if ( WiiManager->SetupGui(_viewer3d->getWxWindow()) )
	{		
		_selectedButton = id3d_WiiTool;
		
		/*
		vtkInteractorStyleWII *style = vtkInteractorStyleWII::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
		*/

		double currentFactor = ((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->MotionFactor;
		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
		((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->MotionFactor = currentFactor;
           		
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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

		double currentFactor = ((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->MotionFactor;
		vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);		
		style->Delete();
		((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->MotionFactor = currentFactor;
		
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			Close(TRUE);
			return;
		}

		((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPInputDevice1,_surfaceToolbar))->SetValue(true);
	}
	else if ( WiiManager->QuickSetupGui(_viewer3d->getWxWindow()) )
	{
		_selectedButton = id3d_WiiTool;
		
		/*
		vtkInteractorStyleWII *style = vtkInteractorStyleWII::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
		*/

		double currentFactor = ((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->MotionFactor;
		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
		((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->MotionFactor = currentFactor;
	            	
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
		double currentFactor = ((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->MotionFactor;
		wxMotionFactorDialog* d = new wxMotionFactorDialog(this, -1, currentFactor);
		if (d->ShowModal() == wxID_OK) 
			((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->MotionFactor = d->getMFactor();
	}
	else {
		double currentFactor = ((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->MotionFactor;
		wxMotionFactorDialog* d = new wxMotionFactorDialog(this, -1, currentFactor);
		if (d->ShowModal() == wxID_OK) 
			((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->MotionFactor = d->getMFactor();
	}	
}


void wxSurfaceRenderingGui::onWiiChangeSceneIRInteraction( wxCommandEvent &event )
{
	appWxVtkInteractor* appWVI = (appWxVtkInteractor*)_viewer3d->getWxWindow();
	appWVI->_isSceneRotatingIR_1 = !appWVI->_isSceneRotatingIR_1;
}
void wxSurfaceRenderingGui::onDolly(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour((_surfaceToolbar->FindControl(id3d_dollyTool))->GetBackgroundColour());
	(_surfaceToolbar->FindControl(id3d_dollyTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = id3d_dollyTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(dolly3dSR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onZoom(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour((_surfaceToolbar->FindControl(id3d_zoomTool))->GetBackgroundColour());
	(_surfaceToolbar->FindControl(id3d_zoomTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = id3d_zoomTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(zoom3dSR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::setInitialDistance() {
	_distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
}

void wxSurfaceRenderingGui::setInitialProjectionValues() {
	//_viewAngle = _viewer3d->getRenderer()->GetActiveCamera()->GetViewAngle();
	//_parallelScale = _viewer3d->getRenderer()->GetActiveCamera()->GetParallelScale();
}

/*
void wxSurfaceRenderingGui::updateStereoView() {
	// interocular distance
	double ID = 7.1;
	// image plane - observer distance
	double POD = 50;
	// camera - volume distance
	double distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
	// cm/pixel conversion factor
	double scale = _distance/POD;
	// eye angle estimate
	double eyeAngle = 2 * atan(ID*scale/(2*distance)) * 180 / 3.141;
	// parallax estimate
	double parallax = distance - _distance;
	// camera perspective update
	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(eyeAngle);
	_viewer3d->getRenderer()->GetActiveCamera()->SetParallax(parallax);
}
*/

void wxSurfaceRenderingGui::setPOD(int POD) {
	_POD = POD;
}

int wxSurfaceRenderingGui::getPOD() {
	return _POD;
}

/*
void wxSurfaceRenderingGui::updateStereoView() {
	if(((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode1,_surfaceToolbar)->GetValue() == false ) {
		// camera - volume distance
		double distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
		// eye angle estimate
		double eyeAngle = 2 * atan(_distance*7.1/(2*_POD*distance)) * 180 / 3.141;
		// _zoomParallax estimate
		double parallax = distance - _distance;
		//if ( eyeAngle<1.422 )
		{
			// camera perspective update
			_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(eyeAngle);
			_viewer3d->getRenderer()->GetActiveCamera()->SetParallax(parallax/_globalZoomFactor);
		}		
	}
}
*/

void wxSurfaceRenderingGui::updateStereoView(bool doARender) {
	if (((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPStereoMode1,_surfaceToolbar))->GetValue() == false) {
		// camera - volume distance
		double distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
		// eye angle estimate
		double eyeAngle = 2 * atan(_distance*6.35/(2*_POD*distance)) * 180 / 3.141;
		// _zoomParallax estimate
		double parallax = distance - _distance;
		// camera perspective update
		_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(eyeAngle);
		_viewer3d->getRenderer()->GetActiveCamera()->SetParallax(parallax/_globalZoomFactor);
	}
	if (doARender) {
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			Close(TRUE);
			return;
		}
	}
}

void wxSurfaceRenderingGui::updateStereoView(int dy) {
	// controlla che ci sia una modalità stereo attiva e che la camera non superi il focal point
	if  ( ((wxRadioButton*)this->FindWindowById(id3d_RADIOBUTTONGROUPStereoMode1,_surfaceToolbar))->GetValue() == false &&
		  (_parallax + _distance + dy > 10) ) 
	{
		// accumula la parallasse
		_parallax = _parallax + dy;
		// distance è la distanza a cui sarebbe la camera se vi fosse stato il dolly
		double distance = _parallax + _distance;
		// eye angle estimate
		double eyeAngle = 2 * atan(_distance*5.0/(2*_POD*distance)) * 180 / 3.141;
		// aggiornamento degli stereogrammi
		_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(eyeAngle);
		_viewer3d->getRenderer()->GetActiveCamera()->SetParallax(_parallax/_globalZoomFactor);

		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour((_surfaceToolbar->FindControl(id3d_rotateTool))->GetBackgroundColour());
	(_surfaceToolbar->FindControl(id3d_rotateTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = id3d_rotateTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotate3d);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onMove(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour((_surfaceToolbar->FindControl(id3d_moveTool))->GetBackgroundColour());
	(_surfaceToolbar->FindControl(id3d_moveTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = id3d_moveTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(move3d);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxSurfaceRenderingGui::onRotateAround(wxCommandEvent& event) {
	(_surfaceToolbar->FindControl(_selectedButton))->SetBackgroundColour((_surfaceToolbar->FindControl(id3d_3drotTool))->GetBackgroundColour());
	(_surfaceToolbar->FindControl(id3d_3drotTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = id3d_3drotTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3dSR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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

void wxSurfaceRenderingGui::show() {
	wxWindowDisabler disabler;
	wxBusyInfo wait("Preparing 3D data...");
	wxBusyCursor cursor;
	
	//((itkVtkData*)_mainGui->getDataHandler()->getData(_idData)))->
	
	_viewer3d->showAppData(_mainGui->getDataHandler()->getData(_idData));
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
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
	const wxString name = "MITO";
	const wxString version = "1.0 Beta";
	const wxString copyright = "(C) 2006-2009 Institute for High Performance Computing and Networking (ICAR-CNR), Naples Branch";
	const wxString conjunction = "Institute of Biostructure and Bioimaging (IBB-CNR)";
	const wxString hyperlink = "<http://amico.icar.cnr.it/>";

	wxString msg;
    msg << name;
    msg << _(" Version ") << version << _T('\n');
    msg << copyright << _T('\n');
	msg << _("in conjunction with ") << conjunction << _T('\n');
    msg << hyperlink;

    wxMessageBox(msg, _T("About ") + name);
}
void wxSurfaceRenderingGui::set3DCursorRGBColor(double R, double G, double B)
{
	if (_3DCursor == NULL) return;
	_3DCursor->GetProperty()->SetColor(R,G,B);
}