/**
 * \file wxVolumeRenderingGui.cxx
 * \brief File per la creazione della finestra 3D contenente il risultato del volume rendering
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/busyinfo.h>

// aggiunte
/*
#include <wx/image.h>
#include <wx/splitter.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
*/

/*
 * include the header
 */
#include "colorTransferTable.h"
#include "wxVolumeRenderingGui.h"
#include "wxVtkViewer3d.h"
#include "wxWLWWDialog.h"
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkFixedPointVolumeRayCastMapper.h>

#include <vtkVolumeTextureMapper3D.h>

/*
 * include all the reconstruction .xpm used in the toolbar
 */
#include "icons/Cut3d.xpm"
#include "icons/Length.xpm"
#include "icons/Move.xpm"
#include "icons/Point.xpm"
#include "icons/Rot3d.xpm"
#include "icons/Rotate.xpm"
#include "icons/WindowLevel.xpm"
#include "icons/Dolly.xpm"
#include "icons/Zoom.xpm"
#include "icons/Skin.xpm"
#include "icons/Muscle.xpm"
#include "icons/Bone.xpm"

// Wii
#include "icons/Wii.xpm"
#include <vtkInteractorStyleWII.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleWIITrackball.h>
#include "wxCDRatioDialog.h"
#include "wxMotionFactorDialog.h"

// 3DCursor
#include <vtkLineSource.h>
//#include <vtkProbeFilter.h>
#include <vtkPProbeFilter.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkMath.h>
#include <vtkPlane.h>

// prova
#include <vtkPolyDataMapper.h>

#include <vtkCoordinate.h>
#include <vtkPlane.h>
#include <math.h>

#include <vtkVolumeRayCastCompositeFunction.h>

/*
 * here is the event table 
 */
BEGIN_EVENT_TABLE(wxVolumeRenderingGui, wxFrame)
	EVT_ACTIVATE(wxVolumeRenderingGui::onActivate)

	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPRendering1,wxVolumeRenderingGui::onChangeRendering)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPRendering2,wxVolumeRenderingGui::onChangeRendering)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPRendering3,wxVolumeRenderingGui::onChangeRendering)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPRendering4,wxVolumeRenderingGui::onChangeRendering)

	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPInputDevice1,wxVolumeRenderingGui::onChangeInputDevice)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPInputDevice2,wxVolumeRenderingGui::onChangeInputDevice)

	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPStereoMode1,wxVolumeRenderingGui::onStereo)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPStereoMode2,wxVolumeRenderingGui::onStereo)

	EVT_RADIOBOX(IDREC3d_RADIOBOXProjectionType,wxVolumeRenderingGui::onProjectionType)
	
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPCursorType1,wxVolumeRenderingGui::onCursorType)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPCursorType2,wxVolumeRenderingGui::onCursorType)	

	EVT_MENU(IDREC3d_3dviewerResetToInitialView,wxVolumeRenderingGui::onResetView)
	EVT_MENU(m_3d_WindowLevel,wxVolumeRenderingGui::onWindowLevel)
	EVT_MENU(m_3d_Move,wxVolumeRenderingGui::onMove)
	EVT_MENU(m_3d_Dolly,wxVolumeRenderingGui::onDolly)
	EVT_MENU(m_3d_Zoom,wxVolumeRenderingGui::onZoom)
	EVT_MENU(m_3d_Rotate,wxVolumeRenderingGui::onRotate)
	EVT_MENU(m_3d_RotateAround,wxVolumeRenderingGui::onRotateAround)
	EVT_MENU(m_3d_VolumeRendering,wxVolumeRenderingGui::onVolumeRendering)
	EVT_MENU(m_3d_TextureRendering,wxVolumeRenderingGui::onTextureRendering)
	EVT_MENU(m_3d_FixedPointRendering,wxVolumeRenderingGui::onFixedPointRendering)
	EVT_MENU(m_3d_MIP,wxVolumeRenderingGui::onMIP)
	//EVT_MENU(m_3d_MinIP,wxVolumeRenderingGui::onMinIP)

	EVT_MENU(IDREC3d_3dviewerAxialView,wxVolumeRenderingGui::onAxialView)	
	EVT_MENU(IDREC3d_3dviewerCoronalView,wxVolumeRenderingGui::onCoronalView)	
	EVT_MENU(IDREC3d_3dviewerLeftSagittalView,wxVolumeRenderingGui::onSagittalView)
	EVT_MENU(IDREC3d_3dviewerRightSagittalView,wxVolumeRenderingGui::onOppositeSagittalView)
	EVT_MENU(m_3d_voi,wxVolumeRenderingGui::onVOI)
	EVT_MENU(IDREC3d_HelpAbout,wxVolumeRenderingGui::onShowAboutDialog)	
	EVT_MENU(IDREC3d_fileCloseWindow,wxVolumeRenderingGui::onClose)
	EVT_MENU(IDREC3d_3dFullScreen,wxVolumeRenderingGui::onFullScreen)
	EVT_MENU(m_3d_StereoModeAnaglyphVR,wxVolumeRenderingGui::onStereoModeAnaglyph)
	EVT_MENU(m_3d_StereoModeActivePassiveVR,wxVolumeRenderingGui::onStereoModeActivePassive)
	EVT_MENU(m_3d_ModifyObserverDistanceVR,wxVolumeRenderingGui::onModifyObserverDistance)
	EVT_MENU(m_3d_ParallelProjectionTypeVR,wxVolumeRenderingGui::onProjectionTypeParallel)
	EVT_MENU(m_3d_PerspectiveProjectionTypeVR,wxVolumeRenderingGui::onProjectionTypePerspective)
	EVT_MENU(m_3d_ModifyWLWWVR,wxVolumeRenderingGui::onModifyWLWW)
	EVT_BUTTON(IDREC3d_WLWTool,wxVolumeRenderingGui::onWindowLevel)
	EVT_BUTTON(IDREC3d_moveTool,wxVolumeRenderingGui::onMove)
	EVT_BUTTON(IDREC3d_dollyTool,wxVolumeRenderingGui::onDolly)
	EVT_BUTTON(IDREC3d_zoomTool,wxVolumeRenderingGui::onZoom)
	EVT_BUTTON(IDREC3d_RotateTool,wxVolumeRenderingGui::onRotate)
	EVT_BUTTON(IDREC3d_3drotTool,wxVolumeRenderingGui::onRotateAround)
	EVT_BUTTON(IDREC3d_VOITool,wxVolumeRenderingGui::onVOI)
	EVT_BUTTON(IDREC3d_SkinTool,wxVolumeRenderingGui::onSkinVision)
	EVT_BUTTON(IDREC3d_MuscleTool,wxVolumeRenderingGui::onMuscleVision)
	EVT_BUTTON(IDREC3d_BoneTool,wxVolumeRenderingGui::onBoneVision)

	// Wii
	EVT_BUTTON(IDREC3d_WiiTool,wxVolumeRenderingGui::onWii)
	EVT_MENU(IDREC3d_WiiTool,wxVolumeRenderingGui::onWii)
	EVT_MENU(IDREC3d_WiiConfigurationTool,wxVolumeRenderingGui::onWiiConfiguration)
	EVT_MENU(IDREC3d_WiiChangeSceneIRInteraction,wxVolumeRenderingGui::onWiiChangeSceneIRInteraction)
	EVT_MENU(IDREC3d_ModifyCDRatioFunction,wxVolumeRenderingGui::onModifyCDRatioFunction)
	EVT_MENU(IDREC3d_ModifyMotionFactor,wxVolumeRenderingGui::onModifyMotionFactor)
	
	//EVT_COMBOBOX(IDREC3d_COMBOBOXZoom,wxVolumeRenderingGui::onZoom)
	EVT_COMBOBOX(IDREC3d_COMBOBOXClut,wxVolumeRenderingGui::onCLUT)

	EVT_COMBOBOX(IDREC3d_COMBOBOXSampleDistance,wxVolumeRenderingGui::onSampleDistance)

	EVT_CHAR_HOOK(wxVolumeRenderingGui::onChar)

	//EVT_SLIDER(IDREC3d_SLIDER1, wxVolumeRenderingGui::onEyeAngle)
	EVT_CLOSE(wxVolumeRenderingGui::onQuit)

	EVT_ENTER_WINDOW(wxVolumeRenderingGui::onEnterToolbar)
END_EVENT_TABLE()

wxVolumeRenderingGui::wxVolumeRenderingGui(const wxString& title, const wxPoint& pos, const wxSize& size)
//       : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxDOUBLE_BORDER | wxMAXIMIZE){
         : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE){

	/*wxIcon icon;
	icon.LoadFile("icon.ico",wxBITMAP_TYPE_ICO);
	this->SetIcon(icon);*/

	SetIcon(wxICON(MITO));

	_idData = 0;
	_minPixel = 0;
	_maxPixel = 0;
	_minOpacityValue = 0.0;
	_maxOpacityValue = 1.0;
	_selectedButton = IDREC3d_3drotTool;

	// crea il cropping box
	_boxWidget = vtkBoxWidget::New();
	_clippingPlanes = vtkPlanes::New();
	_croppingInitialized = 0;

	// inizializza la distanza iniziale tra camera e volume
	_distance = 0;

	// inizializza la distanza tra osservatore e schermo
	_POD = 400;

	// inizializza i valori di zoom per parallel e perspective mode
	//_viewAngle = 0;
	//_parallelScale = 0;

	// inizializza la parallasse
	_parallax = 0;

	// inizializza lo zoom
	_globalZoomFactor = 1;
	
	// cursore 3D
	_3DCursor = NULL;
	_3DCursorIsOnModelSurface = false;

	// Assi 3D passanti per il centro dell'oggetto
	_3DAxesActor = NULL;

	// _3DtextureSupported è true solo se sono supportate le 3D textures
	_3DtextureSupported = false;

	// vale diventa true se si seleziona una delle visioni di tessuto predefinite
	_tissueVisionSelected = false;

	// sample distance di default
	_sampleDistance = 0.5;

	Createmenu();
	CreateReconstructionToolbar();
}

 
void wxVolumeRenderingGui::Createmenu()
{
	CreateStatusBar(3);
	SetStatusText("Ready", 0);

    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
	menuFile->Append(IDREC3d_fileCloseWindow, "Close 3D Viewer\tCtrl-C");

	wxMenu *menu3dViewer = new wxMenu("", wxMENU_TEAROFF);
	menu3dViewer->Append(IDREC3d_3dviewerResetToInitialView, "Reset to initial view\tCtrl-R");
	menu3dViewer->AppendSeparator();
	menu3dViewer->Append(IDREC3d_3dviewerCoronalView, "Coronal View\tAlt-C");
	menu3dViewer->Append(IDREC3d_3dviewerLeftSagittalView, "Left Sagittal View\tAlt-L");
	menu3dViewer->Append(IDREC3d_3dviewerRightSagittalView, "Right Sagittal View\tAlt-R");
	menu3dViewer->Append(IDREC3d_3dviewerAxialView, "Axial View\tAlt-A");
	menu3dViewer->AppendSeparator();
	menu3dViewer->Append(IDREC3d_3dFullScreen, "Full Screen\tCtrl-F");
	menu3dViewer->AppendSeparator();
	menu3dViewer->Append(m_3d_ModifyWLWWVR, "Modify WL/WW\tCtrl-W");
	menu3dViewer->AppendSeparator();

	wxMenu *menuRenderingMode = new wxMenu; 
	menuRenderingMode->Append(m_3d_VolumeRendering,_("Floating Point Ray Casting"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->Append(m_3d_FixedPointRendering,_("Fixed Point Ray Casting"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->Append(m_3d_TextureRendering,_("Texture Mapping"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->Append(m_3d_MIP,_("Maximum Intensity Projection"),_T(""),wxITEM_NORMAL);
	//menuRenderingMode->Append(m_3d_MinIP,_("MinIP\tCtrl-P"),_T(""),wxITEM_NORMAL);
	menu3dViewer->Append(m_3d_RenderingMode,"Rendering Mode",menuRenderingMode);
	menu3dViewer->AppendSeparator();

	/*
	_menuProjectionType = new wxMenu; 
	_menuProjectionType->Append(m_3d_ParallelProjectionTypeVR,_("Parallel"),_T(""),wxITEM_NORMAL);
	_menuProjectionType->Append(m_3d_PerspectiveProjectionTypeVR,_("Perspective"),_T(""),wxITEM_NORMAL);
	menu3dViewer->Append(m_3d_ProjectionTypeVR,"Projection Type",_menuProjectionType);
	*/

	_menuStereoMode = new wxMenu; 
	//_menuStereoMode->Append(m_3d_StereoModeOffVR,_("Off\tCtrl-O"),_T(""),wxITEM_NORMAL);
	_menuStereoMode->AppendCheckItem(m_3d_StereoModeAnaglyphVR,_("Anaglyph\tCtrl-Y"),_T(""));
	(_menuStereoMode->AppendCheckItem(m_3d_StereoModeActivePassiveVR,_("Active/Passive\tCtrl-S"),_T("")))->Check();
	_menuStereoMode->Append(m_3d_ModifyObserverDistanceVR,_("Modify observer distance\tCtrl-B"),_T(""),wxITEM_NORMAL);
	menu3dViewer->Append(m_3d_StereoModeVR,"Stereo Mode",_menuStereoMode);

	wxMenu *menuInteraction = new wxMenu("", wxMENU_TEAROFF);
	menuInteraction->Append(m_3d_RotateAround,_("ROTATE 3D\tCtrl-A"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_Dolly,_("DOLLY\tCtrl-D"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_Zoom,_("ZOOM\tCtrl-Z"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_Rotate,_("ROLL\tCtrl-T"),_T(""),wxITEM_NORMAL);	
	menuInteraction->Append(m_3d_Move,_("PAN\tCtrl-M"),_T(""),wxITEM_NORMAL);	
	menuInteraction->Append(m_3d_voi,_("CROP\tCtrl-V"),_T(""),wxITEM_NORMAL);
	menuInteraction->AppendSeparator();	
	menuInteraction->Append(m_3d_WindowLevel,_("Window Level\tCtrl-W"),_T(""),wxITEM_NORMAL);	
	menuInteraction->AppendSeparator();		
	menuInteraction->Append(IDREC3d_WiiTool,_("Wiimote Quick Start\tCtrl-1"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(IDREC3d_WiiConfigurationTool,_("Wiimote Options\tCtrl-2"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(IDREC3d_WiiChangeSceneIRInteraction,_("Wiimote interaction technique switch\tCtrl-3"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(IDREC3d_ModifyCDRatioFunction,_("Modify pointing C-D ratio"),_T(""),wxITEM_NORMAL);
	menuInteraction->AppendSeparator();		
	menuInteraction->Append(IDREC3d_ModifyMotionFactor,_("Modify rotation Motion Factor"),_T(""),wxITEM_NORMAL);
	

	//menuMouseFunction->Enable(m_3d_voi,false);
	//menu3dViewer->Append(m_3d_Interaction,"Interaction",menuInteraction);
	//menu3dViewer->AppendSeparator();

	wxMenu *menuHelp = new wxMenu("", wxMENU_TEAROFF);
	menuHelp->Append(IDREC3d_HelpAbout, "About MITO"); 

	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "File");
	menuBar->Append(menu3dViewer, "3D Viewer");
	menuBar->Append(menuInteraction, "Interaction");
	menuBar->Append(menuHelp, "Help");
    SetMenuBar(menuBar);
	
	_mainPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN);

    _mainPanel->SetBackgroundColour(*wxBLACK);
	wxBoxSizer* mainHorWinSizer = new wxBoxSizer(wxHORIZONTAL);

	_imageBoxSizer = new wxBoxSizer(wxVERTICAL);
	_imageBoxSizer->Fit(_mainPanel);
    _imageBoxSizer->SetSizeHints(_mainPanel);
	_mainPanel->SetAutoLayout(true);
    _mainPanel->SetSizer(_imageBoxSizer);

	wxBoxSizer* mainVerWinSizer = new wxBoxSizer(wxVERTICAL);
    mainVerWinSizer->Add(_mainPanel, 1,  wxEXPAND|wxALIGN_CENTER, 0);
	mainHorWinSizer->Add(mainVerWinSizer, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(mainHorWinSizer);
	Layout();
    Centre();
}


void wxVolumeRenderingGui::CreateReconstructionToolbar() {
	int altezzatool=25;
	int altezzaradiobox=5;
	int currentHorizontalPosition=5;

	long style = wxTB_FLAT | wxTB_DOCKABLE| wxTB_TEXT | wxTB_HORIZONTAL;
	//_toolbar = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL|wxTB_TEXT, -1 );


	//ho impostato lo stile e ho dato un identificatore toolbar
	//ora ricavo le bitmap
	
	_toolbar = CreateToolBar( style, -1 );
	_toolbar->SetToolPacking(20);
	_toolbar->SetToolSeparation(30); 
	_toolbar->SetToolBitmapSize(wxSize(40,65));
	_toolbar->Realize();	

	wxBitmap cut3dTool(this->GetBitmapResource(wxT("icons/Cut3d.xpm")));
	wxBitmap dollyTool(this->GetBitmapResource(wxT("icons/Dolly.xpm")));	
	wxBitmap zoomTool(this->GetBitmapResource(wxT("icons/Zoom.xpm")));	
	wxBitmap rot3dTool(this->GetBitmapResource(wxT("icons/Rot3d.xpm")));
	wxBitmap rot3dcamTool(this->GetBitmapResource(wxT("icons/Rot3dcam.xpm")));
	wxBitmap moveTool(this->GetBitmapResource(wxT("icons/Move.xpm")));
	wxBitmap CroppingTool(this->GetBitmapResource(wxT("icons/Cropping.xpm")));
	wxBitmap OrientTool(this->GetBitmapResource(wxT("icons/Orient.xpm")));
	wxBitmap RotateTool(this->GetBitmapResource(wxT("icons/Rotate.xpm")));
	wxBitmap PointTool(this->GetBitmapResource(wxT("icons/Point.xpm")));
	wxBitmap WLWTool(this->GetBitmapResource(wxT("icons/WindowLevel.xpm")));
	wxBitmap LengthTool(this->GetBitmapResource(wxT("icons/Length.xpm")));
	wxBitmap BestTool(this->GetBitmapResource(wxT("icons/Best.xpm")));
	wxBitmap WiiTool(this->GetBitmapResource(wxT("icons/Wii.xpm")));
	wxBitmap SkinTool(this->GetBitmapResource(wxT("icons/Skin.xpm")));
	wxBitmap MuscleTool(this->GetBitmapResource(wxT("icons/Muscle.xpm")));
	wxBitmap BoneTool(this->GetBitmapResource(wxT("icons/Bone.xpm")));

	_toolbar->SetRows(1);

	// aggiunto da Luigi per compatibilità con Vista Aero
	_toolbar->SetBackgroundColour(*wxBLACK);
	_toolbar->SetForegroundColour(*wxWHITE);

	//Raggruppamento dei radioButton associati al inputDevice
	wxPanel* inputDeviceRadioPanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	inputDeviceRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* inputDeviceStaticBox = new wxStaticBox(inputDeviceRadioPanel, wxID_ANY, "Input Device"); 	
	wxStaticBoxSizer* inputDeviceRadioSizer = new wxStaticBoxSizer( inputDeviceStaticBox, wxVERTICAL );	  
	wxBoxSizer* inputDeviceRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB1 = new wxRadioButton(inputDeviceRadioPanel, IDREC3d_RADIOBUTTONGROUPInputDevice1, "");	
	inputDeviceB1->SetForegroundColour(*wxWHITE);  
	inputDeviceB1->SetValue(true);
	wxStaticText* inputDeviceB1Text = new wxStaticText(inputDeviceRadioPanel,wxID_ANY,"Mouse");
	inputDeviceB1Text->SetForegroundColour(*wxWHITE);  
	inputDeviceRow1->Add( inputDeviceB1 , 0, wxLEFT, 0); 
	inputDeviceRow1->Add( inputDeviceB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* inputDeviceRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB2 = new wxRadioButton(inputDeviceRadioPanel, IDREC3d_RADIOBUTTONGROUPInputDevice2, "");
	wxStaticText* inputDeviceB2Text = new wxStaticText(inputDeviceRadioPanel,wxID_ANY,"Wiimote");
	inputDeviceB2Text->SetForegroundColour(*wxWHITE);  
	inputDeviceRow2->Add( inputDeviceB2 , 0, wxRIGHT, 3); 
	inputDeviceRow2->Add( inputDeviceB2Text , 0, wxRIGHT, 15);	 
	inputDeviceRadioSizer->Add( inputDeviceRow1 , 0, wxTOP , 6); 
	inputDeviceRadioSizer->Add( inputDeviceRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	inputDeviceRadioPanel->SetSizerAndFit(inputDeviceRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+inputDeviceRadioPanel->GetSize().GetWidth();


	//Contenitore raggruppante i Mouse Functions Buttons
	currentHorizontalPosition=currentHorizontalPosition+5;	
	wxStaticBox* mouseFunctionsButtonsBox = new wxStaticBox(_toolbar, wxID_ANY, wxString("Mouse Functions"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(250,64));
	
	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* button3drotTool = new wxBitmapButton(_toolbar, IDREC3d_3drotTool,rot3dTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	button3drotTool->SetToolTip(_("ROTATE 3D"));
	_toolbar->AddControl(button3drotTool);
	currentHorizontalPosition=currentHorizontalPosition+button3drotTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonDolly = new wxBitmapButton(_toolbar, IDREC3d_dollyTool,dollyTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonDolly->SetToolTip(_("DOLLY"));
	_toolbar->AddControl(buttonDolly);
	currentHorizontalPosition=currentHorizontalPosition+buttonDolly->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonZoom = new wxBitmapButton(_toolbar, IDREC3d_zoomTool,zoomTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonZoom->SetToolTip(_("ZOOM"));
	_toolbar->AddControl(buttonZoom);
	currentHorizontalPosition=currentHorizontalPosition+buttonZoom->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonRotate = new wxBitmapButton(_toolbar, IDREC3d_RotateTool,RotateTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonRotate->SetToolTip(_("ROLL"));
	_toolbar->AddControl(buttonRotate);
	currentHorizontalPosition=currentHorizontalPosition+buttonRotate->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonMove = new wxBitmapButton(_toolbar, IDREC3d_moveTool,moveTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonMove->SetToolTip(_("PAN"));
	_toolbar->AddControl(buttonMove);
	currentHorizontalPosition=currentHorizontalPosition+buttonMove->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonVOITool = new wxBitmapButton(_toolbar, IDREC3d_VOITool, cut3dTool, wxPoint(currentHorizontalPosition,altezzatool),wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonVOITool->SetToolTip(_("CROP"));
	//buttonVOITool->Enable(false);
	_toolbar->AddControl(buttonVOITool);
	currentHorizontalPosition=currentHorizontalPosition+buttonVOITool->GetSize().GetWidth()-1;

	/*
	wxString zoomValues[8] = {"25%", "50%", "75%", "100%", "125%", "150%", "200%", "500%"};
	_comboBoxZoom = new wxComboBox(_toolbar, IDREC3d_COMBOBOXZoom, _("100%"), wxPoint(290,30), wxDefaultSize, 8, zoomValues, wxCB_READONLY);
	_toolbar->AddControl(_comboBoxZoom);
	wxStaticText* TestoZoom = new wxStaticText(_toolbar, wxID_STATIC, _("Zoom"), wxPoint(305,65), wxDefaultSize, 0 );
	*/

	//Raggruppamento dei radioButton associati al cursorType
	currentHorizontalPosition=currentHorizontalPosition+15;
	wxPanel* cursorTypeRadioPanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	cursorTypeRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* cursorTypeStaticBox = new wxStaticBox(cursorTypeRadioPanel, wxID_ANY, "Cursor Type"); 	
	wxStaticBoxSizer* cursorTypeRadioSizer = new wxStaticBoxSizer( cursorTypeStaticBox, wxVERTICAL );	  
	wxBoxSizer* cursorTypeRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB1 = new wxRadioButton(cursorTypeRadioPanel, IDREC3d_RADIOBUTTONGROUPCursorType1, "");	
	cursorTypeB1->SetForegroundColour(*wxWHITE);  
	cursorTypeB1->SetValue(true);
	wxStaticText* cursorTypeB1Text = new wxStaticText(cursorTypeRadioPanel,wxID_ANY,"2D Cursor");
	cursorTypeB1Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow1->Add( cursorTypeB1 , 0, wxLEFT, 0); 
	cursorTypeRow1->Add( cursorTypeB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* cursorTypeRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB2 = new wxRadioButton(cursorTypeRadioPanel, IDREC3d_RADIOBUTTONGROUPCursorType2, "");
	wxStaticText* cursorTypeB2Text = new wxStaticText(cursorTypeRadioPanel,wxID_ANY,"3D Cursor");
	cursorTypeB2Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow2->Add( cursorTypeB2 , 0, wxRIGHT, 3); 
	cursorTypeRow2->Add( cursorTypeB2Text , 0, wxRIGHT, 5);	 
	cursorTypeRadioSizer->Add( cursorTypeRow1 , 0, wxTOP , 6); 
	cursorTypeRadioSizer->Add( cursorTypeRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	cursorTypeRadioPanel->SetSizerAndFit(cursorTypeRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+cursorTypeRadioPanel->GetSize().GetWidth();

	//Raggruppamento dei radioButton associati al rendering
	currentHorizontalPosition=currentHorizontalPosition+5;
	wxPanel* renderingRadioPanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	renderingRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* renderingStaticBox = new wxStaticBox(renderingRadioPanel, wxID_ANY, "Rendering Mode"); 	
	wxStaticBoxSizer* renderingRadioSizer = new wxStaticBoxSizer( renderingStaticBox, wxVERTICAL );	  
	wxBoxSizer* renderingRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* renderingB1 = new wxRadioButton(renderingRadioPanel, IDREC3d_RADIOBUTTONGROUPRendering1, "");
	renderingB1->SetForegroundColour(*wxWHITE);  
	wxStaticText* renderingB1Text = new wxStaticText(renderingRadioPanel,wxID_ANY,"Floating Pt Ray Casting");
	renderingB1Text->SetForegroundColour(*wxWHITE);  
	renderingRow1->Add( renderingB1 , 0, wxLEFT, 0); 
	renderingRow1->Add( renderingB1Text , 0, wxLEFT | wxRIGHT, 3); 	
	wxRadioButton* renderingB3 = new wxRadioButton(renderingRadioPanel, IDREC3d_RADIOBUTTONGROUPRendering3, "");
	renderingB3->SetForegroundColour(*wxWHITE);  
	wxStaticText* renderingB3Text = new wxStaticText(renderingRadioPanel, wxID_ANY, "3D Texture");
	renderingB3Text->SetForegroundColour(*wxWHITE);  
	renderingRow1->Add( renderingB3 , 0, wxLEFT, 5); 
	renderingRow1->Add( renderingB3Text , 0, wxLEFT | wxRIGHT, 3); 	
	wxBoxSizer* renderingRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* renderingB2 = new wxRadioButton(renderingRadioPanel, IDREC3d_RADIOBUTTONGROUPRendering2, "");
	wxStaticText* renderingB2Text = new wxStaticText(renderingRadioPanel,wxID_ANY,"Fixed Pt Ray Casting");
	renderingB2Text->SetForegroundColour(*wxWHITE);  
	renderingRow2->Add( renderingB2 , 0, wxLEFT, 0); 
	renderingRow2->Add( renderingB2Text , 0, wxLEFT | wxRIGHT, 3);	 
	wxRadioButton* renderingB4 = new wxRadioButton(renderingRadioPanel, IDREC3d_RADIOBUTTONGROUPRendering4, "");
	wxStaticText* renderingB4Text = new wxStaticText(renderingRadioPanel,wxID_ANY,"MIP");
	renderingB4Text->SetForegroundColour(*wxWHITE);  
	renderingRow2->Add( renderingB4 , 0, wxLEFT, 17); 
	renderingRow2->Add( renderingB4Text , 0, wxLEFT | wxRIGHT, 3);	 
	renderingRadioSizer->Add( renderingRow1 , 0, wxTOP , 6); 
	renderingRadioSizer->Add( renderingRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	renderingRadioPanel->SetSizerAndFit(renderingRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+renderingRadioPanel->GetSize().GetWidth();

	//Raggruppamento dei radioButton associati al stereoMode
	currentHorizontalPosition=currentHorizontalPosition+5;
	wxPanel* stereoModeRadioPanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	stereoModeRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* stereoModeStaticBox = new wxStaticBox(stereoModeRadioPanel, wxID_ANY, "Stereo"); 	
	wxStaticBoxSizer* stereoModeRadioSizer = new wxStaticBoxSizer( stereoModeStaticBox, wxVERTICAL );	  
	wxBoxSizer* stereoModeRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* stereoModeB1 = new wxRadioButton(stereoModeRadioPanel, IDREC3d_RADIOBUTTONGROUPStereoMode1, "");	
	stereoModeB1->SetForegroundColour(*wxWHITE);  
	stereoModeB1->SetValue(true);
	wxStaticText* stereoModeB1Text = new wxStaticText(stereoModeRadioPanel,wxID_ANY,"Off");
	stereoModeB1Text->SetForegroundColour(*wxWHITE);  
	stereoModeRow1->Add( stereoModeB1 , 0, wxLEFT, 0); 
	stereoModeRow1->Add( stereoModeB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* stereoModeRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* stereoModeB2 = new wxRadioButton(stereoModeRadioPanel, IDREC3d_RADIOBUTTONGROUPStereoMode2, "");
	wxStaticText* stereoModeB2Text = new wxStaticText(stereoModeRadioPanel,wxID_ANY,"On");
	stereoModeB2Text->SetForegroundColour(*wxWHITE);  
	stereoModeRow2->Add( stereoModeB2 , 0, wxRIGHT, 3); 
	stereoModeRow2->Add( stereoModeB2Text , 0, wxRIGHT, 15);	 
	stereoModeRadioSizer->Add( stereoModeRow1 , 0, wxTOP , 6); 
	stereoModeRadioSizer->Add( stereoModeRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	stereoModeRadioPanel->SetSizerAndFit(stereoModeRadioSizer);		
	currentHorizontalPosition=currentHorizontalPosition+stereoModeRadioPanel->GetSize().GetWidth();
	

//	wxString radioBoxProjectionTypeStrings[] = {
//      _("Parallel"),
//      _("Perspective")
//  };
//	currentHorizontalPosition=currentHorizontalPosition+10;
//	_itemRadioBoxProjectionType = new wxRadioBox( _toolbar, IDREC3d_RADIOBOXProjectionType, _("Projection Type"), wxPoint(currentHorizontalPosition,altezzaradiobox), wxSize(-1,-1), 2, radioBoxProjectionTypeStrings, 1, wxRA_SPECIFY_COLS );
//	_itemRadioBoxProjectionType->SetSelection(1);
//	_itemRadioBoxProjectionType->Show(false);
//  _toolbar->AddControl(_itemRadioBoxProjectionType);
//	currentHorizontalPosition=currentHorizontalPosition+_itemRadioBoxProjectionType->GetSize().GetWidth()-1;


	//Contenitore raggruppante i Tissue Selection Buttons
	currentHorizontalPosition=currentHorizontalPosition+5;	
	wxStaticBox* tissueSelectionButtonsBox = new wxStaticBox(_toolbar, wxID_ANY, wxString("Tissue Selection"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(305,64));

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonSkin = new wxBitmapButton(_toolbar, IDREC3d_SkinTool,SkinTool, wxPoint(currentHorizontalPosition,altezzatool),wxSize(31,31) ,wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonSkin->SetToolTip(_("View Skin"));
	buttonSkin->Enable(false);
	_toolbar->AddControl(buttonSkin);
	currentHorizontalPosition=currentHorizontalPosition+buttonSkin->GetSize().GetWidth()-1;
	
	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonMuscle = new wxBitmapButton(_toolbar, IDREC3d_MuscleTool,MuscleTool, wxPoint(currentHorizontalPosition,altezzatool),wxSize(31,31) ,wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonMuscle->SetToolTip(_("View Muscle"));
	buttonMuscle->Enable(false);
	_toolbar->AddControl(buttonMuscle);
	currentHorizontalPosition=currentHorizontalPosition+buttonMuscle->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* buttonBone = new wxBitmapButton(_toolbar, IDREC3d_BoneTool,BoneTool, wxPoint(currentHorizontalPosition,altezzatool),wxSize(31,31) ,wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonBone->SetToolTip(_("View Bone"));
	buttonBone->Enable(false);
	_toolbar->AddControl(buttonBone);
	currentHorizontalPosition=currentHorizontalPosition+buttonBone->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+20;
	wxBitmapButton* buttonWLW = new wxBitmapButton(_toolbar, IDREC3d_WLWTool,WLWTool, wxPoint(currentHorizontalPosition,altezzatool),wxSize(31,31) ,wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonWLW->SetToolTip(_("Modify Transfer Function"));
	_toolbar->AddControl(buttonWLW);
	currentHorizontalPosition=currentHorizontalPosition+buttonWLW->GetSize().GetWidth()-1;

	wxString choices[12] = {"NO CLUT", "BlackBody", "Cardiac", "Flow", "GEColor", "GrainRainbow", "HotIron", "NIH", "Spectrum", "VR Bones", "VR Muscles-Bones", "VR Red Vessels"};
	currentHorizontalPosition=currentHorizontalPosition+10;
	_comboBoxClut = new wxComboBox(_toolbar, IDREC3d_COMBOBOXClut, _("NO CLUT"), wxPoint(currentHorizontalPosition,altezzatool+1), wxDefaultSize, 12, choices, wxCB_READONLY);
	_toolbar->AddControl(_comboBoxClut);
	currentHorizontalPosition=currentHorizontalPosition+_comboBoxClut->GetSize().GetWidth()-1;

	// variazione della sample distance per il 3d texture mapping
	wxString sdchoices[10] = {"0.01", "0.05", "0.10", "0.25", "0.50", "0.75", "1.00", "1.25", "1.5", "2"};
	//currentHorizontalPosition=currentHorizontalPosition+10;
	//_comboBoxSampleDistance = new wxComboBox(_toolbar, IDREC3d_COMBOBOXSampleDistance, _("0.50"), wxPoint(currentHorizontalPosition,altezzatool+1), wxDefaultSize, 10, sdchoices, wxCB_READONLY);
	//_comboBoxSampleDistance->Show(false);
	//_toolbar->AddControl(_comboBoxSampleDistance);
	//currentHorizontalPosition=currentHorizontalPosition+_comboBoxSampleDistance->GetSize().GetWidth()-1;	
	
	
	this->SetToolBar(_toolbar);		

	(_toolbar->FindControl(IDREC3d_3drotTool))->SetBackgroundColour(*wxCYAN);
}

wxVolumeRenderingGui::~wxVolumeRenderingGui(){
	_mainGui = 0;
	this->Destroy();
}

wxBitmap wxVolumeRenderingGui::GetBitmapResource( const wxString& name )
{

    wxUnusedVar(name);
    if (name == _T("icons/Cut3d.xpm"))
    {
        wxBitmap bitmap( cut3d_xpm);
        return bitmap;
    }
	else if (name == _T("icons/Length.xpm"))
    {
        wxBitmap bitmap( length_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Move.xpm"))
    {
        wxBitmap bitmap( move_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Point.xpm"))
    {
        wxBitmap bitmap( point_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Rot3d.xpm"))
    {
        wxBitmap bitmap( rot3d_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Rotate.xpm"))
    {
        wxBitmap bitmap( rotate_xpm);
		return bitmap;
	}
	else if (name == _T("icons/WindowLevel.xpm"))
    {
        wxBitmap bitmap( windowlevel_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Dolly.xpm"))
    {
        wxBitmap bitmap( dolly_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Zoom.xpm"))
	{
		wxBitmap bitmap( zoom_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Wii.xpm"))
    {
        wxBitmap bitmap( wii_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Skin.xpm"))
    {
        wxBitmap bitmap( skin_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Muscle.xpm"))
    {
        wxBitmap bitmap( muscle_xpm);
		return bitmap;
	}
	else if (name == _T("icons/Bone.xpm"))
    {
        wxBitmap bitmap( bone_xpm);
		return bitmap;
	}
    return wxNullBitmap;
////@end frame_3drec bitmap retrieval
}

void wxVolumeRenderingGui::onSkinVision(wxCommandEvent& event) {
/*	
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading ...");
	wxBusyCursor cursor;

	vtkFixedPointVolumeRayCastMapper *mapper;
	if ( _renderingTechnique == VolumeFixedPointRendering ) {
		mapper = (vtkFixedPointVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper();
	}
	else mapper = vtkFixedPointVolumeRayCastMapper::New();

	int shiftValue = 0;
	if( _minPixel < 0 ) 
		shiftValue =(-1)*(_minPixel);

	vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
	shiftScale->SetInput(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkImage());
	shiftScale->SetScale(1);
	shiftScale->SetShift(shiftValue);
	shiftScale->ClampOverflowOn();
	shiftScale->SetOutputScalarTypeToUnsignedShort();

	mapper->SetInput(shiftScale->GetOutput());
//	mapper->SetInput(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkImage());

	// Create our transfer function
	vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
	vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();

	// Create the property and attach the transfer functions
	vtkVolumeProperty *prop = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetProperty();
	prop->SetColor( colorFun );
	prop->SetScalarOpacity( opacityFun );
	
	colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
	colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
	colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
	colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

	opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-1000, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-500, 1.0, 0.33, 0.45 );
	opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);

	mapper->SetBlendModeToComposite();
	
	prop->ShadeOn();
	prop->SetAmbient(0.1);
	prop->SetDiffuse(0.9);
	prop->SetSpecular(0.2);			
	prop->SetSpecularPower(10.0);
	prop->SetScalarOpacityUnitDistance(0.8919);  

	_comboBoxClut->SetValue("NO CLUT");
	_comboBoxClut->Enable(false);	
	_tissueVisionSelected=true;

	onRotateAround(event);
*/	
}

void wxVolumeRenderingGui::onModifyCDRatioFunction(wxCommandEvent& WXUNUSED(event)) {
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	wxCDRatioDialog* d = new wxCDRatioDialog(this, -1, appWxVtkI->_minDISP / 1279.0, appWxVtkI->_maxDISP / 1279.0);
	if (d->ShowModal() == wxID_OK) {	  
		appWxVtkI->_minDISP = d->getCMin() * 1279;
		appWxVtkI->_maxDISP = d->getCMax() * 1279;
	}
}

void wxVolumeRenderingGui::onModifyMotionFactor(wxCommandEvent& WXUNUSED(event)) {
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	if (appWxVtkI->getInteractionType() == wii || appWxVtkI->getInteractionType() == voi3dVRwii) {
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

void wxVolumeRenderingGui::onMuscleVision(wxCommandEvent& event) {
/*
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading ...");
	wxBusyCursor cursor;

	
	// VERSIONE CPU FixedPointRayCast
	//vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();
	//mapper->SetInput(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkImage());
	

	// VERSIONE CPU RayCast
	int shiftValue = 0;
	int minPixel = 0;
	shiftValue =(-1)*(minPixel);
	vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
	shiftScale->SetInput(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkImage());
	shiftScale->SetScale(1);
	shiftScale->SetShift(shiftValue);
	shiftScale->ClampOverflowOn();
	shiftScale->SetOutputScalarTypeToUnsignedShort();
	
	vtkVolumeRayCastMapper *mapper = vtkVolumeRayCastMapper::New();
	vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
	mapper->SetVolumeRayCastFunction(compositeFunction);
	mapper->SetInput(shiftScale->GetOutput());

	// Create our transfer function
	vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
	vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();

	// Create the property and attach the transfer functions
	vtkVolumeProperty *prop = vtkVolumeProperty::New();
	//prop->SetIndependentComponents(true);
	prop->SetColor( colorFun );
	prop->SetScalarOpacity( opacityFun );
	//prop->SetInterpolationTypeToLinear();

	// connect up the volume to the property and the mapper
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->SetProperty(prop);
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->SetMapper(mapper);

	colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
	colorFun->AddRGBPoint( -155, .55, .25, .15, 0.5, .92 );
	colorFun->AddRGBPoint( 217, .88, .60, .29, 0.33, 0.45 );
	colorFun->AddRGBPoint( 420, 1, .94, .95, 0.5, 0.0 );
	colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

	opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-155, 0, 0.5, 0.92 );
	opacityFun->AddPoint(217, .68, 0.33, 0.45 );
	opacityFun->AddPoint(420,.83, 0.5, 0.0);
	opacityFun->AddPoint(3071, .80, 0.5, 0.0);
	
	mapper->SetBlendModeToComposite();

	prop->ShadeOn();
	prop->SetAmbient(0.1);
	prop->SetDiffuse(0.9);
	prop->SetSpecular(0.2);
	prop->SetSpecularPower(10.0);
	prop->SetScalarOpacityUnitDistance(0.8919);  

	_comboBoxClut->SetValue("NO CLUT");
	_comboBoxClut->Enable(false);	
	_tissueVisionSelected=true;

	onRotateAround(event);
	*/
}

void wxVolumeRenderingGui::onBoneVision(wxCommandEvent& event) {
	/*
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading ...");
	wxBusyCursor cursor;

	
	// VERSIONE CPU FixedPointRayCast
	//vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();
	//mapper->SetInput(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkImage());
	

	// VERSIONE CPU RayCast
	int shiftValue = 0;
	int minPixel = 0;
	shiftValue =(-1)*(minPixel);
	vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
	shiftScale->SetInput(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkImage());
	shiftScale->SetScale(1);
	shiftScale->SetShift(shiftValue);
	shiftScale->ClampOverflowOn();
	shiftScale->SetOutputScalarTypeToUnsignedShort();
	vtkVolumeRayCastMapper *mapper = vtkVolumeRayCastMapper::New();
	vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
	mapper->SetVolumeRayCastFunction(compositeFunction);
	mapper->SetInput(shiftScale->GetOutput());

	// Create our transfer function
	vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
	vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();

	// Create the property and attach the transfer functions
	vtkVolumeProperty *prop = vtkVolumeProperty::New();
	prop->SetIndependentComponents(true);
	prop->SetColor( colorFun );
	prop->SetScalarOpacity( opacityFun );
	prop->SetInterpolationTypeToLinear();

	// connect up the volume to the property and the mapper
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->SetProperty(prop);
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->SetMapper(mapper);

	colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
	colorFun->AddRGBPoint( -16, 0.73, 0.25, 0.30, 0.49, .61 );
	colorFun->AddRGBPoint( 641, .90, .82, .56, .5, 0.0 );
	colorFun->AddRGBPoint( 3071, 1, 1, 1, .5, 0.0 );

	opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-16, 0, .49, .61 );
	opacityFun->AddPoint(641, .72, .5, 0.0 );
	opacityFun->AddPoint(3071, .71, 0.5, 0.0);

	mapper->SetBlendModeToComposite();

	prop->ShadeOn();
	prop->SetAmbient(0.1);
	prop->SetDiffuse(0.9);
	prop->SetSpecular(0.2);
	prop->SetSpecularPower(10.0);
	prop->SetScalarOpacityUnitDistance(0.8919);  

	_comboBoxClut->SetValue("NO CLUT");
	_comboBoxClut->Enable(false);	
	_tissueVisionSelected=true;

	onRotateAround(event);
	*/
}

void wxVolumeRenderingGui::onVOI( wxCommandEvent &event )
{
	
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());
	
	if (appWxVtkI->getInteractionType() == wii)
	{
		// CON BOX WIDGET
		if (!_croppingInitialized) {
			initializeCroppingBox();
		}
		if (!_boxWidget->GetEnabled()) {
			_boxWidget->EnabledOn();
			_viewer3d->getRenderer()->ResetCameraClippingRange();
		}
		// FINE BOX WIDGET
	
		/*
		//showMouseCursor();
		int x,y;
		GetSize(&x,&y);
		WarpPointer(x/2,y/2);		
		*/

		appWxVtkI->setInteractionType(voi3dVRwii);
	}
	else if (appWxVtkI->getInteractionType() == voi3dVRwii)
	{
		if (_boxWidget->GetEnabled()) {
			_boxWidget->EnabledOff();
		}
	
		/*
		//hideMouseCursor();
		int x,y;
		GetSize(&x,&y);
		WarpPointer(x,y);			
		*/

		appWxVtkI->setInteractionType(wii);
		this->updateClippingPlanes(true);
	}
	else if (_selectedButton == IDREC3d_VOITool) {
		if (_boxWidget->GetEnabled()) {
			_boxWidget->EnabledOff();
		}
		(_toolbar->FindControl(IDREC3d_VOITool))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_RotateTool))->GetBackgroundColour());
		appWxVtkI->setInteractionType(rotateAround3d);
		_selectedButton = IDREC3d_RotateTool;		
		this->updateClippingPlanes(true);
	}
	else {
		(_toolbar->FindControl(_selectedButton))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_VOITool))->GetBackgroundColour());
		(_toolbar->FindControl(IDREC3d_VOITool))->SetBackgroundColour(*wxCYAN);
		_selectedButton = IDREC3d_VOITool;
		
		// CON BOX WIDGET
		if (!_croppingInitialized) {
			initializeCroppingBox();
		}
		if (!_boxWidget->GetEnabled()) {
			_boxWidget->EnabledOn();
			_viewer3d->getRenderer()->ResetCameraClippingRange();
		}
		// FINE BOX WIDGET

		appWxVtkI->setInteractionType(voi3dVR);
		//_viewer3d->updateViewer();
	}
	this->updateClippingPlanes(false);

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::create3DAxes()
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

    //axesTubes->Delete();
	//axes_mapper->Delete();
}

void wxVolumeRenderingGui::destroy3DAxes() {
	if (_3DAxesActor == NULL) return;

	_3DAxesActor->Delete();
	_3DAxesActor = NULL;
	_3DAxes->Delete();
}

void wxVolumeRenderingGui::show3DAxes() {
	if (_3DAxesActor == NULL) return;
	
	//Cerco la max distanza tra i piani dei bounds
	double b[6];
	if (_croppingInitialized==1)
		_clippingPlanes->GetPoints()->GetBounds(b);
	else
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetBounds(b);			
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

void wxVolumeRenderingGui::hide3DAxes() {
	if (_3DAxesActor == NULL) return;

	_3DAxesActor->VisibilityOff();
}

void wxVolumeRenderingGui::create3DCursor()
{
	if (_3DCursor != NULL) return;

	double bounds[6];
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetBounds(bounds);			
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

}

void wxVolumeRenderingGui::destroy3DCursor()
{
	if (_3DCursor == NULL) return;

	_3DCursor->Delete();
	_3DCursor = NULL;
}

bool wxVolumeRenderingGui::isThisVoxelVisible(double scalar)
{
	int shiftValue = 0;
	if(_minPixel < 0) 
		shiftValue =(-1)*(_minPixel);
	double start = (shiftValue + _wl - _ww/2) + (0.1*_ww);
	
	return (scalar>start);
}

bool wxVolumeRenderingGui::intersectingModelSurface(long x, long y, double point[3])
{
	
	_3DCursorIsOnModelSurface = false;

	double closest, farthest;	
	getBoundsDepthInfo(closest, farthest);

	vtkRenderer* ren = _viewer3d->getRenderer();
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());			

	// Intersect with line
	double p1[4]; double p2[4];
	appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, closest, p1);
	appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, farthest, p2);

	vtkLineSource* line = vtkLineSource::New();
	line->SetPoint1(p1);
	line->SetPoint2(p2);
	double distance = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
	line->SetResolution(distance);

	vtkPProbeFilter* probe = vtkPProbeFilter::New();
	//vtkProbeFilter* probe = vtkProbeFilter::New();
	probe->SetInput(line->GetOutput());
	probe->SetSource( ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->
									getVtkVolume()->GetMapper()->GetDataObjectInput() );
	probe->Update();

	vtkPointData* pointData = probe->GetOutput()->GetPointData();

	vtkDataArray* dataArray = pointData->GetScalars();
	int n = dataArray->GetNumberOfTuples();

	for (int i=0; i<n; i++) {
		double* val = dataArray->GetTuple(i);
		if (isThisVoxelVisible(val[0]))
		{
			probe->GetOutput()->GetPoint(i,point);
			// controlla che il punto non sia esterno al cropping box
			if (_croppingInitialized==1 && isPointOutsideCroppingBox(point))
				_3DCursorIsOnModelSurface = false;
			else {
				_3DCursorIsOnModelSurface = true;
				break;
			}
		}
	}
	
	line->Delete();
	probe->Delete();

	return _3DCursorIsOnModelSurface;
}

bool wxVolumeRenderingGui::isPointOutsideCroppingBox(double point[3]) 
{
	double bounds[6], center[3];
	_clippingPlanes->GetPoints()->GetBounds(bounds);
	center[0] = (bounds[1] + bounds[0])/2.0;
	center[1] = (bounds[3] + bounds[2])/2.0;
	center[2] = (bounds[5] + bounds[4])/2.0;

	double t;
	double x[3];
	int result = 0;
	for (int i=0; i < 6; i++) {
		vtkPlane* plane = _clippingPlanes->GetPlane(i); 
		result = result + plane->IntersectWithLine(point, center, plane->GetNormal(), plane->GetOrigin(), t, x);
	}
	
	return result;
}


void wxVolumeRenderingGui::get3DCursorDisplayPosition(int & x, int & y, bool flipY)
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

bool wxVolumeRenderingGui::set3DCursorPosition(long x, long y, bool flipY)
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
		/*
		double focalDepth, focalPosition[3],newPosition[4];
		ren->GetActiveCamera()->GetFocalPoint(focalPosition);
		appWxVtkI->GetInteractorStyle()->ComputeWorldToDisplay( ren, focalPosition[0], focalPosition[1], 
																	focalPosition[2], focalPosition);				
		focalDepth = focalPosition[2];
		appWxVtkI->GetInteractorStyle()->ComputeDisplayToWorld( ren, x, y, focalDepth, newPosition);
		_3DCursor->SetPosition( newPosition[0], newPosition[1], newPosition[2]);
		*/

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

bool wxVolumeRenderingGui::is3DCursorBeyondFocalPlane()
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
void wxVolumeRenderingGui::show3DCursor()
{
	if (_3DCursor == NULL) return;

	_3DCursor->VisibilityOn();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorCanMove(true);
}
void wxVolumeRenderingGui::hide3DCursor()
{
	if (_3DCursor == NULL) return;

	_3DCursor->VisibilityOff();
}
void wxVolumeRenderingGui::getBoundsDepthInfo(double & closest, double & farthest)
{
	double bounds[6];
	if (_croppingInitialized==1)
		_clippingPlanes->GetPoints()->GetBounds(bounds);
	else
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetBounds(bounds);			
	
	double temp[3];

	// inizializzo closest e farthest pari al primo vertice
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->
		GetInteractorStyle()->ComputeWorldToDisplay( _viewer3d->getRenderer(), bounds[0], bounds[2], bounds[4], temp);				
	closest = farthest = temp[2];

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
	
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->
		GetInteractorStyle()->ComputeWorldToDisplay( _viewer3d->getRenderer(), borderPoints[i][0], 
																borderPoints[i][1], borderPoints[i][2], temp);				

		if ( temp[2] > farthest ) farthest = temp[2];
		if ( temp[2] < closest ) closest = temp[2];
	}
}

void wxVolumeRenderingGui::onWii( wxCommandEvent &event )
{	
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	
	wxWiiManager* WiiManager = appWxVtkI->getWiiManager( _viewer3d );			

	if ( (appWxVtkI->getInteractionType() == wii) || (appWxVtkI->getInteractionType() == voi3dVRwii) )	//Operazioni da fare in uscita dalla modalità Wii
	{	
		//controllo se posso uscire o meno
		//if ( (appWxVtkI->idWiiEnabled != AnyWiiEnabled) || (WiiManager->WiiPointing_num) ) return;
		//else if (appWxVtkI->getInteractionType() == voi3dVRwii) appWxVtkI->InvokeEvent(vtkWiiHomeDown,NULL);			        
		//if (!appWxVtkI->closeWiiAbility()) return;

		// chiudo la VOI se aperta
		if (appWxVtkI->getInteractionType() == voi3dVRwii) appWxVtkI->InvokeEvent(vtkWiiHomeDown,NULL);

		_viewer3d->showLeftTopInfo();

		if (appWxVtkI->getInteractionType() == voi3dVRwii)
		{
			appWxVtkI->setInteractionType(voi3dVR);
			_selectedButton = IDREC3d_VOITool;
		}
		else
		{
			appWxVtkI->setInteractionType(rotateAround3d);
			_selectedButton = IDREC3d_3drotTool;
		}			
		
		appWxVtkI->closeWiiManager();					


		vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		

		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x/2,y/2);

		if (appWxVtkI->get3DcursorMode()) {
			this->show3DCursor();
			appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		}
		else {
			appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() / 10 );
		}
		// regola la definizione
		appWxVtkI->dummyMethod();

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
//		wglMakeCurrent(NULL, NULL);						
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			Close(TRUE);
			return;
		}

		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice1,_toolbar))->SetValue(true);
	}
	else if ( WiiManager->QuickSetupGui(_viewer3d->getWxWindow()) ) // entra
	{
		_viewer3d->hideLeftTopInfo();

		_selectedButton = IDREC3d_WiiTool;
		
		// passa a bassa risoluzione
		appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		// regola la definizione
		appWxVtkI->dummyMethod();

		/*
		// nasconde il cropping box
		if (_boxWidget->GetEnabled())
			_boxWidget->EnabledOff();
		*/
		
		/*
		vtkInteractorStyleWII *style = vtkInteractorStyleWII::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
        */			
		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
					
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x,y);
		//_viewer3d->getRenderWindow()->HideCursor();
		this->hide3DCursor();
		
		//_viewer3d->getRenderWindow()->BordersOff();
				
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
		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			Close(TRUE);
			return;
		}

		
		double bounds[6];
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetBounds(bounds);			
		double radius = (bounds[1]-bounds[0]+bounds[3]-bounds[2]+bounds[5]-bounds[4])*0.02/3;
		WiiManager->setPointerRadius(radius);
		WiiManager->Start();	
		
		if (appWxVtkI->getInteractionType() == voi3dVR)
		{
			appWxVtkI->setInteractionType(voi3dVRwii);
		}
		else appWxVtkI->setInteractionType(wii);

		appWxVtkI->initInteractionSignals();	
		appWxVtkI->WiiManagerStarted = true;

		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice2,_toolbar))->SetValue(true);
	}
	else {		
		appWxVtkI->closeWiiManager();
		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice1,_toolbar))->SetValue(true);
	}
}

void wxVolumeRenderingGui::onWiiConfiguration( wxCommandEvent &event )
{	
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	
	wxWiiManager* WiiManager = appWxVtkI->getWiiManager( _viewer3d );			

	if ( (appWxVtkI->getInteractionType() == wii) || (appWxVtkI->getInteractionType() == voi3dVRwii) ) // esce
	{	

		//controllo se posso uscire o meno
		if ( (appWxVtkI->idWiiEnabled != AnyWiiEnabled) && (WiiManager->WiiPointing_num) ) return;
		else if (appWxVtkI->getInteractionType() == voi3dVRwii) appWxVtkI->InvokeEvent(vtkWiiHomeDown,NULL);
		//if (!appWxVtkI->closeWiiAbility()) return;

		_viewer3d->showLeftTopInfo();

		if (appWxVtkI->getInteractionType() == voi3dVRwii)
		{
			appWxVtkI->setInteractionType(voi3dVR);
			_selectedButton = IDREC3d_VOITool;
		}
		else
		{
			appWxVtkI->setInteractionType(rotateAround3d);
			_selectedButton = IDREC3d_3drotTool;
		}			
		
		appWxVtkI->closeWiiManager();					

		vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		

		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x/2,y/2);
		
		if (appWxVtkI->get3DcursorMode()) {
			this->show3DCursor();
			appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		}
		else {
			appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() / 10 );
		}
		// regola la definizione
		appWxVtkI->dummyMethod();

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

		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice1,_toolbar))->SetValue(true);
	}
	else if ( WiiManager->SetupGui(_viewer3d->getWxWindow()) ) // entra
	{
		_viewer3d->hideLeftTopInfo();

		_selectedButton = IDREC3d_WiiTool;

		// passa a bassa risoluzione
		appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		// regola la definizione
		appWxVtkI->dummyMethod();

		/*
		// nasconde il cropping box
		if (_boxWidget->GetEnabled())
			_boxWidget->EnabledOff();
		*/
		
		/*
		vtkInteractorStyleWII *style = vtkInteractorStyleWII::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
        */			
		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		
					
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x,y);
		//_viewer3d->getRenderWindow()->HideCursor();
		this->hide3DCursor();
		
		//_viewer3d->getRenderWindow()->BordersOff();
		
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

		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			Close(TRUE);
			return;
		}

	
		double bounds[6];
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetBounds(bounds);			
		double radius = (bounds[1]-bounds[0]+bounds[3]-bounds[2]+bounds[5]-bounds[4])*0.02/3;
		WiiManager->setPointerRadius(radius);
		WiiManager->Start();	
		
		if (appWxVtkI->getInteractionType() == voi3dVR)
		{
			appWxVtkI->setInteractionType(voi3dVRwii);
		}
		else appWxVtkI->setInteractionType(wii);

		appWxVtkI->initInteractionSignals();	
		appWxVtkI->WiiManagerStarted = true;		

		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice2,_toolbar))->SetValue(true);
	}
	else {		
		appWxVtkI->closeWiiManager();
		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice1,_toolbar))->SetValue(true);
	}
}

void wxVolumeRenderingGui::onWiiChangeSceneIRInteraction( wxCommandEvent &event )
{
	appWxVtkInteractor* appWVI = (appWxVtkInteractor*)_viewer3d->getWxWindow();
	appWVI->_isSceneRotatingIR_1 = !appWVI->_isSceneRotatingIR_1;
}

void wxVolumeRenderingGui::onResetView(wxCommandEvent& event) {
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading ...");
	wxBusyCursor cursor;

	_tissueVisionSelected=false;

	// rimuove il volume dalla finestra
	_viewer3d->getRenderer()->RemoveVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());

	// inizializza a NO CLUT il tipo di clut
	_comboBoxClut->SetValue("NO CLUT");
	_clut = 0;

	_wl = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getWl();
	_ww = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getWw();
	if(_viewer3d) {
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->setCLUT(_clut);
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->setWlWw(_wl,_ww);
	}
	if(_renderingTechnique == VolumeRendering) {
		volumeRenderingFilter volumeRendering(_idData, _mainGui->getDataHandler());
		if(!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb())
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(volumeRendering.compute(_wl,_ww,_clut,_minPixel));
		else
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(volumeRendering.computeRgb(_wl,_ww,_clut));
	}
	else if(_renderingTechnique == VolumeFixedPointRendering) {
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeFixedPoint(_wl,_ww, _clut,_minPixel));		
	}
	else if(_renderingTechnique == VolumeTextureRendering) {
		if (_3DtextureSupported) {
			volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeTextures3D(_wl,_ww, _clut,_minPixel,_sampleDistance));
		}
		else {
			volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeTextures2D(_wl,_ww, _clut,_minPixel));
		}
	}
	else if(_renderingTechnique == MIP) {
		mipFilter mip(_idData, _mainGui->getDataHandler());
		if(!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb())
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(mip.compute(_wl, _ww, _clut, _minPixel, true));
		else
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(mip.computeRgb(_wl, _ww, _clut, true));
	}
	/*else if(_renderingTechnique == MinIP) {
		minipFilter minip(_idData, _mainGui->getDataHandler());
		if(!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb())
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(minip.compute(_wl, _ww, _clut, _minPixel, true));
		else
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(minip.computeRgb(_wl, _ww, _clut, true));
	}*/

	// inizializza al 100% il fattore di zoom sulla toolbar
	//_zoomValuePerspective = 100;
	//_zoomValueParallel = 100;
	//_comboBoxZoom->SetValue("100%");

	// inizializza a OFF la vista stereoscopica sulla toolbar
	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(0);
	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();

	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode1,_toolbar))->SetValue(true);

	// inizializza distanza dalla camera e parallasse
	_parallax = 0;
	//_viewer3d->getRenderWindow()->StereoRenderOff();

	// inizializza a Perspective
//	_itemRadioBoxProjectionType->Enable(true);
//	_itemRadioBoxProjectionType->SetSelection(1);
//	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();

	// inizializza a 2.0 il valore dell'eye angle
	//_itemSlider->SetValue(2.0);

	// aggiunge il volume calcolato alla scena
	_viewer3d->getRenderer()->AddVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());

	// inizializza la posizione della camera e il focal point
	double* focalPoint = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetCenter();
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0], focalPoint[1], focalPoint[2]);
	_viewer3d->getRenderer()->GetActiveCamera()->ComputeViewPlaneNormal();
	_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
	_viewer3d->getRenderer()->GetActiveCamera()->OrthogonalizeViewUp();
	_viewer3d->getRenderer()->ResetCamera();
	double distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
	_viewer3d->getRenderer()->GetActiveCamera()->SetPosition (focalPoint[0], focalPoint[1], focalPoint[2]+distance);
	_viewer3d->getRenderer()->GetActiveCamera()->SetViewUp(0, 1, 0);
	
	// inizializza il Cropping Box
	initializeCroppingBox();
	updateClippingPlanes();

	this->Raise();
	this->setInitialDistance();

	_selectedButton = IDREC3d_3drotTool;
}


void wxVolumeRenderingGui::onAxialView(wxCommandEvent& event) {
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

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}


void wxVolumeRenderingGui::onCoronalView(wxCommandEvent& event) {
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
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onSagittalView(wxCommandEvent& event) {
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
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onOppositeSagittalView(wxCommandEvent& event) {
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
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onChangeInputDevice(wxCommandEvent& event) {
	if(event.GetId() == IDREC3d_RADIOBUTTONGROUPInputDevice2 ) 
		wxVolumeRenderingGui::onWii(event);
}

void wxVolumeRenderingGui::setRenderingTechnique(techniqueRayCasting renderingTechnique) {
	_renderingTechnique = renderingTechnique;
	//_comboBoxClut->Enable(false);		

	switch (renderingTechnique) {

		case VolumeRendering:
			((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPRendering1,_toolbar))->SetValue(true);
			break;

		case VolumeFixedPointRendering:
			((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPRendering2,_toolbar))->SetValue(true);
			break;

		case VolumeTextureRendering:
			((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPRendering3,_toolbar))->SetValue(true);
			break;

		case MIP:
			((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPRendering4,_toolbar))->SetValue(true);
			break;
	}
}

void wxVolumeRenderingGui::onChangeRendering(wxCommandEvent& event) {
	
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading ...");
	wxBusyCursor cursor;

	/*
	if(event.GetId() != IDREC3d_RADIOBUTTONGROUPRendering3) {
		if (((wxRadioButton*)FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->GetValue() == true) {
			((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
			((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetStillUpdateRate( (((appWxVtkInteractor*)_viewer3d->getWxWindow())->GetDesiredUpdateRate() / 10) );	
			hide3DCursor();			
			((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType1,_toolbar))->SetValue(true);
		}
		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Enable(false);
	}
	else ((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Enable(true);
	*/

	_viewer3d->getWxWindow()->Freeze();

	// rimuove il volume dalla finestra
	_viewer3d->getRenderer()->RemoveVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());

	if(event.GetId() == IDREC3d_RADIOBUTTONGROUPRendering1) {
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		if(!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb()) {
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.compute(_wl,_ww, _clut,_minPixel));
			this->SetTitle(_T("MITO: 3D Volume Rendering"));
			setRenderingTechnique(VolumeRendering);		
		}
		else {
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeRgb(_wl,_ww, _clut));
			this->SetTitle(_T("MITO: 3D Volume Fixed Point Rendering"));
			setRenderingTechnique(VolumeFixedPointRendering);
		}
		//_comboBoxClut->Enable(true);
	}
	else if(event.GetId() == IDREC3d_RADIOBUTTONGROUPRendering2) {
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeFixedPoint(_wl,_ww, _clut,_minPixel));		
		this->SetTitle(_T("MITO: 3D Volume Fixed Point Rendering"));		
		setRenderingTechnique(VolumeFixedPointRendering);		
		
	}
	else if(event.GetId() == IDREC3d_RADIOBUTTONGROUPRendering3) {
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		if (((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb()) {
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeRgb(_wl,_ww, _clut));
			this->SetTitle(_T("MITO: 3D Volume Fixed Point Rendering"));
			setRenderingTechnique(VolumeFixedPointRendering);
		}
		// se il 3D texture mapping è supportato lo usa, altrimenti usa il 2D texture mapping
		else if (_3DtextureSupported) {
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeTextures3D(_wl,_ww, _clut,_minPixel,_sampleDistance));
			this->SetTitle(_T("MITO: 3D Texture Rendering"));
			setRenderingTechnique(VolumeTextureRendering);
		}
		// se non è possibile usare texture 3D vengono usate le 2D
		else {
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_volumeRendering.computeTextures2D(_wl,_ww, _clut,_minPixel));
			this->SetTitle(_T("MITO: 2D Texture Rendering"));
			setRenderingTechnique(VolumeTextureRendering);
		}
	}
	else if(event.GetId() == IDREC3d_RADIOBUTTONGROUPRendering4) {
		mipFilter _mip(_idData, _mainGui->getDataHandler());
		if(!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb())
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_mip.compute(_wl, _ww, _clut, this->getMinPixel(), true));
		else
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(_mip.computeRgb(_wl, _ww, _clut, true));
		this->SetTitle(_T("MITO: 3D MIP"));
		setRenderingTechnique(MIP);
		//_comboBoxClut->Enable(false);
	}

	_viewer3d->getRenderer()->AddVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

	// se il cropping box e' stato attivato lo esegue anche sulla MIP / sul Volume
	if (_croppingInitialized) {
		_boxWidget->SetInteractor(_viewer3d->getRenderWindow()->GetInteractor());
		updateVolume();
	}	
	
	_viewer3d->getWxWindow()->Thaw();
	this->Raise();	
}

void wxVolumeRenderingGui::onVolumeRendering(wxCommandEvent& event) {
	if(_renderingTechnique != VolumeRendering) {
		wxCommandEvent e;
		e.SetId(IDREC3d_RADIOBUTTONGROUPRendering1);
		onChangeRendering(e);
	}
}

void wxVolumeRenderingGui::onTextureRendering(wxCommandEvent& event) {
	if(_renderingTechnique != VolumeTextureRendering) {
		wxCommandEvent e;
		e.SetId(IDREC3d_RADIOBUTTONGROUPRendering3);
		onChangeRendering(e);
	}
}



void wxVolumeRenderingGui::onFixedPointRendering(wxCommandEvent& event) {
	if(_renderingTechnique != VolumeFixedPointRendering) {
		wxCommandEvent e;
		e.SetId(IDREC3d_RADIOBUTTONGROUPRendering2);
		onChangeRendering(e);
	}
}

void wxVolumeRenderingGui::onMIP(wxCommandEvent& event) {
	if(_renderingTechnique != MIP) {
		wxCommandEvent e;
		e.SetId(IDREC3d_RADIOBUTTONGROUPRendering4);
		onChangeRendering(e);
	}
}

/*
void wxVolumeRenderingGui::onMinIP(wxCommandEvent& event) {
	if(_renderingTechnique == MinIP) {
		wxCommandEvent e;
		e.SetInt(MinIP);
		onChangeRendering(e);
	}
}*/



void wxVolumeRenderingGui::onChar(wxKeyEvent & event)
{
	if ( event.GetKeyCode() == 27 && ( ((appWxVtkInteractor*)_viewer3d->getWxWindow())->getInteractionType() == wii || ( (appWxVtkInteractor*)_viewer3d->getWxWindow())->getInteractionType() == voi3dVRwii ) )	
		this->onWii(wxCommandEvent());
	//else event.Skip();   
}

void wxVolumeRenderingGui::onFullScreen(wxCommandEvent& event) {
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	if ( appWxVtkI->getInteractionType() != wii && appWxVtkI->getInteractionType() != voi3dVRwii )
		this->ShowFullScreen(!this->IsFullScreen());
}


void wxVolumeRenderingGui::onCLUT(wxCommandEvent& event) {
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading ...");
	wxBusyCursor cursor;
	_viewer3d->getWxWindow()->Freeze();

	_clut = event.GetInt();

	/*double alpha[256];
	for(int i = 0; i < 256; i++)
	{
		alpha[i] = i / 255.;
	}*/

	if(!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb()) {
		int shiftValue = 0;
		if(_minPixel < 0) 
			shiftValue =(-1)*(_minPixel);

		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();

		float start = shiftValue + _wl - _ww/2, end = shiftValue + _wl + _ww/2;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		double table[256][3];
		if(_clut == 0) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = i / 255.;
				table[i][1] = i / 255.;
				table[i][2] = i / 255.;
			}
		}
		else if(_clut == 1) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = BlackBody_Red[i] / 255.;
				table[i][1] = BlackBody_Green[i] / 255.;
				table[i][2] = BlackBody_Blue[i] / 255.;
			}
		}
		else if(_clut == 2) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Cardiac_Red[i] / 255.;
				table[i][1] = Cardiac_Green[i] / 255.;
				table[i][2] = Cardiac_Blue[i] / 255.;
			}
		}
		else if(_clut == 3) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Flow_Red[i] / 255.;
				table[i][1] = Flow_Green[i] / 255.;
				table[i][2] = Flow_Blue[i] / 255.;
			}
		}
		else if(_clut == 4) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GEColor_Red[i] / 255.;
				table[i][1] = GEColor_Green[i] / 255.;
				table[i][2] = GEColor_Blue[i] / 255.;
			}
		}
		else if(_clut == 5) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GrainRainbow_Red[i] / 255.;
				table[i][1] = GrainRainbow_Green[i] / 255.;
				table[i][2] = GrainRainbow_Blue[i] / 255.;
			}
		}
		else if(_clut == 6) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = HotIron_Red[i] / 255.;
				table[i][1] = HotIron_Green[i] / 255.;
				table[i][2] = HotIron_Blue[i] / 255.;
			}
		}
		else if(_clut == 7) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = NIH_Red[i] / 255.;
				table[i][1] = NIH_Green[i] / 255.;
				table[i][2] = NIH_Blue[i] / 255.;
			}
		}
		else if(_clut == 8) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Spectrum_Red[i] / 255.;
				table[i][1] = Spectrum_Green[i] / 255.;
				table[i][2] = Spectrum_Blue[i] / 255.;
			}
		}
		else if(_clut == 9) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRBones_Red[i] / 255.;
				table[i][1] = VRBones_Green[i] / 255.;
				table[i][2] = VRBones_Blue[i] / 255.;
			}
		}
		else if(_clut == 10) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRMusclesBones_Red[i] / 255.;
				table[i][1] = VRMusclesBones_Green[i] / 255.;
				table[i][2] = VRMusclesBones_Blue[i] / 255.;
			}
		}
		else if(_clut == 11) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRRedVessels_Red[i] / 255.;
				table[i][1] = VRRedVessels_Green[i] / 255.;
				table[i][2] = VRRedVessels_Blue[i] / 255.;
			}
		}

		
		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
		colorTransferFunction->BuildFunctionFromTable(shiftValue + _wl-_ww/2, shiftValue + _wl+_ww/2, 255, (double*) &table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
		volumeProperty->SetColor(colorTransferFunction);

		volumeProperty->SetInterpolationTypeToLinear();
		
		// LUIGI - non deve essere eseguita per 3D texture mapping e fixedpoint
		if ( _renderingTechnique != VolumeTextureRendering &&
			 _renderingTechnique != VolumeFixedPointRendering )
			((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetMinimumImageSampleDistance(1.5);

		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->SetProperty(volumeProperty);
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->Update();

		colorTransferFunction->Delete();
		opacityTransferFunction->Delete();
		volumeProperty->Delete();
	}
	else {
		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();

		float start = _wl - _ww/2, end = _wl + _ww/2;
		opacityTransferFunction->AddPoint(start, 0);
		opacityTransferFunction->AddPoint(end, 1);

		double tableRed[256][3];
		double tableGreen[256][3];
		double tableBlue[256][3];
		double table[256][3];

		if(_clut == 0) {
			for(int i = 0; i < 256; i++)
			{
				tableRed[i][0] = i / 255.;
				tableRed[i][1] = 0.;
				tableRed[i][2] = 0.;
			}
			for(int i = 0; i < 256; i++)
			{
				tableGreen[i][0] = 0.;
				tableGreen[i][1] = i / 255.;
				tableGreen[i][2] = 0.;
			}
			for(int i = 0; i < 256; i++)
			{
				tableBlue[i][0] = 0.;
				tableBlue[i][1] = 0.;
				tableBlue[i][2] = i / 255.;
			}
		}
		else if(_clut == 1) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = BlackBody_Red[i] / 255.;
				table[i][1] = BlackBody_Green[i] / 255.;
				table[i][2] = BlackBody_Blue[i] / 255.;
			}
		}
		else if(_clut == 2) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Cardiac_Red[i] / 255.;
				table[i][1] = Cardiac_Green[i] / 255.;
				table[i][2] = Cardiac_Blue[i] / 255.;
			}
		}
		else if(_clut == 3) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Flow_Red[i] / 255.;
				table[i][1] = Flow_Green[i] / 255.;
				table[i][2] = Flow_Blue[i] / 255.;
			}
		}
		else if(_clut == 4) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GEColor_Red[i] / 255.;
				table[i][1] = GEColor_Green[i] / 255.;
				table[i][2] = GEColor_Blue[i] / 255.;
			}
		}
		else if(_clut == 5) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = GrainRainbow_Red[i] / 255.;
				table[i][1] = GrainRainbow_Green[i] / 255.;
				table[i][2] = GrainRainbow_Blue[i] / 255.;
			}
		}
		else if(_clut == 6) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = HotIron_Red[i] / 255.;
				table[i][1] = HotIron_Green[i] / 255.;
				table[i][2] = HotIron_Blue[i] / 255.;
			}
		}
		else if(_clut == 7) {
			for(int i = 0; i < 256; i++)
			{
				tableRed[i][0] = NIH_Red[i] / 255.;
				tableRed[i][1] = NIH_Green[i] / 255.;
				tableRed[i][2] = NIH_Blue[i] / 255.;
			}
		}
		else if(_clut == 8) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = Spectrum_Red[i] / 255.;
				table[i][1] = Spectrum_Green[i] / 255.;
				table[i][2] = Spectrum_Blue[i] / 255.;
			}
		}
		else if(_clut == 9) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRBones_Red[i] / 255.;
				table[i][1] = VRBones_Green[i] / 255.;
				table[i][2] = VRBones_Blue[i] / 255.;
			}
		}
		else if(_clut == 10) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRMusclesBones_Red[i] / 255.;
				table[i][1] = VRMusclesBones_Green[i] / 255.;
				table[i][2] = VRMusclesBones_Blue[i] / 255.;
			}
		}
		else if(_clut == 11) {
			for(int i = 0; i < 256; i++)
			{
				table[i][0] = VRRedVessels_Red[i] / 255.;
				table[i][1] = VRRedVessels_Green[i] / 255.;
				table[i][2] = VRRedVessels_Blue[i] / 255.;
			}
		}

		
		vtkColorTransferFunction *red = vtkColorTransferFunction::New();
		red->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&tableRed);
		
		vtkColorTransferFunction *green = vtkColorTransferFunction::New();
		green->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&tableGreen);
		
		vtkColorTransferFunction *blue = vtkColorTransferFunction::New();
		blue->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&tableBlue);

		vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
		colorTransferFunction->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&table);

		vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
		volumeProperty->IndependentComponentsOn();

		if(_clut == 0) {
			volumeProperty->SetColor(0, red);
			volumeProperty->SetColor(1, green);
			volumeProperty->SetColor(2, blue);
		}
		else {
			volumeProperty->SetColor(0, colorTransferFunction);
			volumeProperty->SetColor(1, colorTransferFunction);
			volumeProperty->SetColor(2, colorTransferFunction);
		}

		volumeProperty->SetScalarOpacity(0, opacityTransferFunction);
		volumeProperty->SetScalarOpacity(1, opacityTransferFunction);
		volumeProperty->SetScalarOpacity(2, opacityTransferFunction);
		
		volumeProperty->SetComponentWeight(3, 0);
		volumeProperty->SetInterpolationTypeToLinear();

		float LOD = 3.5;
		((vtkFixedPointVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetMinimumImageSampleDistance(LOD);
		((vtkFixedPointVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetMaximumImageSampleDistance(3*LOD);

		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->SetProperty(volumeProperty);
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->Update();

		opacityTransferFunction->Delete();
		colorTransferFunction->Delete();
		red->Delete();
		green->Delete();
		blue->Delete();
		volumeProperty->Delete();
	}

	if(_viewer3d) {
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->setWlWw(_wl,_ww);
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->setCLUT(_clut);
	}
	_viewer3d->getWxWindow()->Thaw();
	this->Raise();
}

void wxVolumeRenderingGui::onSampleDistance(wxCommandEvent& event) {
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading ...");
	wxBusyCursor cursor;
	_viewer3d->getWxWindow()->Freeze();

	int sD = event.GetInt();

	switch (sD) 
	{
		case (0):
			_sampleDistance = 0.01;
			break;
		case (1):
			_sampleDistance = 0.05;
			break;	
		case (2):
			_sampleDistance = 0.10;
			break;
		case (3):
			_sampleDistance = 0.25;
			break;
		case (4):
			_sampleDistance = 0.5;
			break;
		case (5):
			_sampleDistance = 0.75;
			break;
		case (6):
			_sampleDistance = 1.00;
			break;
		case (7):
			_sampleDistance = 1.25;
			break;
		case (8):
			_sampleDistance = 1.5;
			break;
		case (9):
			_sampleDistance = 2;
			break;
		default:
			break;
	}

	
	if ( _renderingTechnique == VolumeTextureRendering )
	{
		// applica la nuova sample distance per 3D textures
		((vtkVolumeTextureMapper3D*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetSampleDistance(_sampleDistance);
	}
	else if (_renderingTechnique == VolumeFixedPointRendering )
	{
		// applica la nuova sample distance per il Fixed Point Rendering
		((vtkFixedPointVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetSampleDistance(_sampleDistance);
	}
	else
		// applica la nuova sample distance per volumetric ray casting
		((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetSampleDistance(_sampleDistance);

	_viewer3d->getWxWindow()->Thaw();
	this->Raise();
}

void wxVolumeRenderingGui::onWindowLevel(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	// controlla se c'è bisogno di resettare il volume in caso sia stata selezionata una visione di tessuto predefinita
	if (_tissueVisionSelected) {
		onResetView(event);
		_comboBoxClut->Enable(true);	
	}

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_WLWTool))->GetBackgroundColour());
	(_toolbar->FindControl(IDREC3d_WLWTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = IDREC3d_WLWTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(windowLevel3d);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}
}

void wxVolumeRenderingGui::onMove(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_moveTool))->GetBackgroundColour());
	(_toolbar->FindControl(IDREC3d_moveTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = IDREC3d_moveTool;

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

void wxVolumeRenderingGui::onDolly(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_dollyTool))->GetBackgroundColour());
	(_toolbar->FindControl(IDREC3d_dollyTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = IDREC3d_dollyTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(dolly3dVR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onZoom(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_zoomTool))->GetBackgroundColour());
	(_toolbar->FindControl(IDREC3d_zoomTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = IDREC3d_zoomTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(zoom3dVR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onRotate(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_RotateTool))->GetBackgroundColour());
	(_toolbar->FindControl(IDREC3d_RotateTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = IDREC3d_RotateTool;

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

void wxVolumeRenderingGui::onRotateAround(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour((_toolbar->FindControl(IDREC3d_3drotTool))->GetBackgroundColour());
	(_toolbar->FindControl(IDREC3d_3drotTool))->SetBackgroundColour(*wxCYAN);
	_selectedButton = IDREC3d_3drotTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3d);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onStereo( wxCommandEvent &event ) {
	if (event.GetId()==IDREC3d_RADIOBUTTONGROUPStereoMode1) {
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
		if ( _menuStereoMode->FindItem(m_3d_StereoModeActivePassiveVR)->IsChecked() ) 
			onStereoModeActivePassive(event);
		else 
			onStereoModeAnaglyph(event);
	}
}

void wxVolumeRenderingGui::onStereoModeAnaglyph(wxCommandEvent& event) {
	// disabilita lo zoom
	//_comboBoxZoom->Enable(false);
	//_zoomValuePerspective = _zoomValueParallel = 100;
	//_comboBoxZoom->SetValue("100%");
	//_viewer3d->getRenderer()->GetActiveCamera()->SetViewAngle(30);
	//_viewer3d->getRenderer()->GetActiveCamera()->SetParallelScale(1);

	// setta il tipo di proiezione a perspective
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyphVR)->Check(true);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassiveVR)->Check(false);

	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode2,_toolbar))->SetValue(true);

	//_viewer3d->getRenderWindow()->StereoRenderOff();
	//_viewer3d->updateViewer();
	_viewer3d->getRenderWindow()->SetStereoTypeToAnaglyph();
	this->updateStereoView();
	//this->updateStereoView(0);
	//_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle((_itemSlider->GetValue()));
	//_viewer3d->getRenderWindow()->StereoRenderOn();
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onStereoModeActivePassive(wxCommandEvent& event) {
	// disabilita lo zoom
	//_comboBoxZoom->Enable(false);
	//_zoomValuePerspective = _zoomValueParallel = 100;
	//_comboBoxZoom->SetValue("100%");
	//_viewer3d->getRenderer()->GetActiveCamera()->SetViewAngle(30);
	//_viewer3d->getRenderer()->GetActiveCamera()->SetParallelScale(1);

	// setta il tipo di proiezione a perspective
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyphVR)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassiveVR)->Check(true);

	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode2,_toolbar))->SetValue(true);
	
	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	this->updateStereoView();
	//this->updateStereoView(0);
	//_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle((_itemSlider->GetValue()));
	//_viewer3d->getRenderWindow()->StereoRenderOn();
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
void wxVolumeRenderingGui::onEyeAngle( wxCommandEvent &event ) {
	_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle((_itemSlider->GetValue()));
	_viewer3d->updateViewer();
}
*/

void wxVolumeRenderingGui::onProjectionType( wxCommandEvent &event ) {
	if(event.GetInt() == 0) {
		_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	}
	else {
		_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	}
	//wxVolumeRenderingGui::onZoom(event);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onCursorType( wxCommandEvent &event ) {		
	if(event.GetId() == IDREC3d_RADIOBUTTONGROUPCursorType1) {
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetStillUpdateRate( (((appWxVtkInteractor*)_viewer3d->getWxWindow())->GetDesiredUpdateRate() / 10) );	
		hide3DCursor();
	}
	else {
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOn();
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetStillUpdateRate( ((appWxVtkInteractor*)_viewer3d->getWxWindow())->GetDesiredUpdateRate() );
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

void wxVolumeRenderingGui::onProjectionTypeParallel(wxCommandEvent& event) {
//	_itemRadioBoxProjectionType->SetSelection(0);
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	//wxVolumeRenderingGui::onZoom(event);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onProjectionTypePerspective(wxCommandEvent& event) {
//	_itemRadioBoxProjectionType->SetSelection(1);
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();
	//wxVolumeRenderingGui::onZoom(event);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onModifyWLWW(wxCommandEvent& event) {
	double wl, ww;
	do {
		wxWLWWDialog* dlg = new wxWLWWDialog(this);
		if (dlg->ShowModal() == wxID_OK) {
			ww = dlg->getWW();
			if(ww >= 1) {
				wl = dlg->getWL();
				dlg->Destroy();

				wxWindowDisabler disabler;
				wxBusyInfo wait("Please wait, modifying WL/WW...");
				wxBusyCursor cursor;
				
				_wl = wl;
				_ww = ww;
				((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setWl(wl);
				((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setWw(ww);
				((appWxVtkInteractor*)_viewer3d->getWxWindow())->setWlWw(wl,ww);

				/*vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
				opacityTransferFunction->AddPoint(wl - ww/2, 0);
				opacityTransferFunction->AddPoint(wl + ww/2, 1);

				vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
				colorTransferFunction->BuildFunctionFromTable(wl - ww/2, wl ww/2, 255, (double*) &table);

				//Creo un set di proprietà per il volume con varie opzioni
				vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
				volumeProperty->SetScalarOpacity(opacityTransferFunction);
				volumeProperty->SetColor(colorTransferFunction);
				volumeProperty->SetInterpolationTypeToLinear();
				if(_renderingTechnique) {			
					volumeProperty->ShadeOn();
				}

				((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->SetProperty(volumeProperty);
				((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->Update();*/
				_viewer3d->setText();
				wxCommandEvent event;
				event.SetInt(_clut);
				onCLUT(event);
				//_viewer3d->updateViewer();	
			}
			else
				wxMessageBox(_T("Set WW larger or equal than 1"));
		}
		else {
			dlg->Destroy();
			return;
		}
	} while(ww < 1);
}

void wxVolumeRenderingGui::initializeCroppingBox() {
	_boxWidget->SetInteractor(_viewer3d->getRenderWindow()->GetInteractor());
	_boxWidget->InsideOutOn();
	_boxWidget->SetPlaceFactor(1.0);
	_boxWidget->TranslationEnabledOn();
	_boxWidget->ScalingEnabledOn();
	_boxWidget->RotationEnabledOn();
	double bounds[6];
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetBounds(bounds);
	_boxWidget->PlaceWidget(bounds);
	_boxWidget->EnabledOff();
	_croppingInitialized=1;
}

void wxVolumeRenderingGui::updateClippingPlanes(bool updateFocalPoint) {
	_boxWidget->GetPlanes(_clippingPlanes);
	if (updateFocalPoint) {
		double bounds[6], center[3];
		_clippingPlanes->GetPoints()->GetBounds(bounds);
		center[0] = (bounds[1] + bounds[0])/2.0;
		center[1] = (bounds[3] + bounds[2])/2.0;
		center[2] = (bounds[5] + bounds[4])/2.0;
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(center);
	}
}

void wxVolumeRenderingGui::updateVolume(bool doARender) {
	((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->RemoveAllClippingPlanes();
	((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetClippingPlanes(_clippingPlanes);
	_viewer3d->getRenderer()->ResetCameraClippingRange();
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

void wxVolumeRenderingGui::setInitialDistance() {
	_distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
}

void wxVolumeRenderingGui::setInitialProjectionValues() {
	//_viewAngle = _viewer3d->getRenderer()->GetActiveCamera()->GetViewAngle();
	//_parallelScale = _viewer3d->getRenderer()->GetActiveCamera()->GetParallelScale();
}

void wxVolumeRenderingGui::setPOD(int POD) {
	_POD = POD;
}

int wxVolumeRenderingGui::getPOD() {
	return _POD;
}

void wxVolumeRenderingGui::onModifyObserverDistance(wxCommandEvent& event) {
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
void wxVolumeRenderingGui::updateStereoView() {
	if(((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode1,_toolbar)->GetValue() == false) {
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

void wxVolumeRenderingGui::updateStereoView(bool doARender) {
	if(((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode1,_toolbar))->GetValue() == false) {
		// camera - volume distance
		double distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
		// eye angle estimate
		double eyeAngle = 2 * atan(_distance*6.35/(2*_POD*distance)) * 180 / M_PI;
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

void wxVolumeRenderingGui::updateStereoView(int dy) {
	// controlla che ci sia una modalità stereo attiva e che la camera non superi il focal point
	if  ( ((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode1,_toolbar))->GetValue() == false  && 
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

void wxVolumeRenderingGui::updateZoom(double factor, bool doARender) 
{
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

void wxVolumeRenderingGui::set3DtextureSupported(bool textureSupported) {
	_3DtextureSupported = textureSupported;
	if (!_3DtextureSupported) {	
		wxString radioBoxRenderingStrings[] = {
			_("Floating Point Ray Casting"),
			_("Fixed Point Ray Casting"),
			_("2D Texture Mapping"),
			_("Maximum Intensity Projection"),
			//_("MinIP")
		};	
	}
}

void wxVolumeRenderingGui::onActivate(wxActivateEvent& event) {
	if (_mainGui->onViewerActivate(_idViewer))
		event.Skip();
}

void wxVolumeRenderingGui::onClose(wxCommandEvent& event) {
	Close(TRUE);
}

void wxVolumeRenderingGui::onQuit(wxCloseEvent& event) {
	_boxWidget->Delete();
	_clippingPlanes->Delete();	
	if ( _mainGui->onViewerQuit(_idViewer))
		event.Skip();
}

/*
void wxVolumeRenderingGui::onZoom(wxCommandEvent& event) {
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
		tempValue = _zoomValuePerspective;
		_zoomValuePerspective = zoom;
	}
	else {
		tempValue = _zoomValueParallel;
		_zoomValueParallel = zoom;
	}
	zoom = zoom/tempValue;
	_viewer3d->getRenderer()->GetActiveCamera()->Zoom(zoom);
	_viewer3d->updateViewer();
}
*/

wxVtkViewer3d* wxVolumeRenderingGui::new3dViewer(wxMainGui* mainGui) {
	_mainGui = mainGui;
	_viewer3d = new wxVtkViewer3d(_mainGui, _mainPanel, -1);
	_viewer3d->setAlgorithm(rayCast);
	//_zoomValuePerspective = 100;
	//_zoomValueParallel = 100;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3dTechnique(VolumeRendering);	

	return _viewer3d;
}

void wxVolumeRenderingGui::show(){
	wxWindowDisabler disabler;
	wxBusyInfo wait("Preparing 3D data...");
	wxBusyCursor cursor;
	
	_imageBoxSizer->Add(_viewer3d->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
	_viewer3d->getWxWindow()->Freeze();
	_viewer3d->showAppData(_mainGui->getDataHandler()->getData(_idData));
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
		Close(TRUE);
		return;
	}

	_imageBoxSizer->Layout();
	_viewer3d->getWxWindow()->Thaw();
	Show(TRUE);
		
	create3DCursor();
	create3DAxes();

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3dTechnique(VolumeRendering);	
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3d);

	Raise();
}

void wxVolumeRenderingGui::onShowAboutDialog(wxCommandEvent& WXUNUSED(event)) {
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
void wxVolumeRenderingGui::showMouseCursor() {
	_viewer3d->getRenderWindow()->ShowCursor();
}
void wxVolumeRenderingGui::hideMouseCursor() {
	_viewer3d->getRenderWindow()->HideCursor();
}
void wxVolumeRenderingGui::onEnterToolbar(wxMouseEvent& event) {
	_viewer3d->getRenderWindow()->ShowCursor();
}

void wxVolumeRenderingGui::set3DCursorRGBColor(double R, double G, double B)
{
	if (_3DCursor == NULL) return;
	_3DCursor->GetProperty()->SetColor(R,G,B);
}