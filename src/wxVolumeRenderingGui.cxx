/**
 * \file wxVolumeRenderingGui.cxx
 * \brief File per la creazione della finestra 3D contenente il risultato del volume rendering
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "colorTransferTable.h"
#include "wxVolumeRenderingGui.h"
#include "wxVtkViewer3d.h"
#include "wxWLWWDialog.h"
#include "wxMotionFactorDialog.h"
#include "wxShadingDialog.h"
#include "wxCDRatioDialog.h"

#include <math.h>

#include <vtkVolumeRayCastMapper.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkInteractorStyleWIITrackball.h"
#include "vtkMitoInteractorStyleTrackball.h"
#include "WiiCommandEvents.h"
#include <vtkLineSource.h>
#include <vtkPProbeFilter.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkMath.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkCoordinate.h>
#include <vtkPlane.h>

#include <wx/busyinfo.h>
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
#include <wx/tglbtn.h>

#include "icons/Cut3d.xpm"
#include "icons/Length.xpm"
#include "icons/Move.xpm"
#include "icons/Point.xpm"
#include "icons/Rot3d.xpm"
#include "icons/Rotate.xpm"
#include "icons/WindowLevel.xpm"
#include "icons/Dolly.xpm"
#include "icons/Zoom.xpm"
#include "icons/Axial.xpm"
#include "icons/Coronal.xpm"
#include "icons/Sagittal.xpm"
#include "icons/OppositeSagittal.xpm"
#include "icons/Wii.xpm"

#include "vtkMitoCustomCamera.h"
#include "vtkBoxWidgetWii.h"

/*
 * here is the event table 
 */
BEGIN_EVENT_TABLE(wxVolumeRenderingGui, wxFrame)
	EVT_ACTIVATE(wxVolumeRenderingGui::onActivate)
	EVT_BUTTON(IDREC3d_WLWTool,wxVolumeRenderingGui::onWindowLevel)
	EVT_BUTTON(IDREC3d_moveTool,wxVolumeRenderingGui::onMove)
	EVT_BUTTON(IDREC3d_dollyTool,wxVolumeRenderingGui::onDolly)
	EVT_BUTTON(IDREC3d_zoomTool,wxVolumeRenderingGui::onZoom)
	EVT_BUTTON(IDREC3d_RotateTool,wxVolumeRenderingGui::onRotate)
	EVT_BUTTON(IDREC3d_3drotTool,wxVolumeRenderingGui::onRotateAround)
	EVT_BUTTON(IDREC3d_VOITool,wxVolumeRenderingGui::onVOI)
	EVT_BUTTON(IDREC3d_3dviewerAxialView,wxVolumeRenderingGui::onAxialView)	
	EVT_BUTTON(IDREC3d_3dviewerCoronalView,wxVolumeRenderingGui::onCoronalView)	
	EVT_BUTTON(IDREC3d_3dviewerLeftSagittalView,wxVolumeRenderingGui::onSagittalView)
	EVT_BUTTON(IDREC3d_3dviewerRightSagittalView,wxVolumeRenderingGui::onOppositeSagittalView)
	EVT_BUTTON(IDREC3d_shadingChangeButton,wxVolumeRenderingGui::onChangeShadingParameters)
	EVT_BUTTON(IDREC3d_WiiTool,wxVolumeRenderingGui::onWii)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPInputDevice1,wxVolumeRenderingGui::onChangeInputDevice)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPInputDevice2,wxVolumeRenderingGui::onChangeInputDevice)
	EVT_RADIOBUTTON(IDREC3d_perspective,wxVolumeRenderingGui::onProjectionTypePerspective)
	EVT_RADIOBUTTON(IDREC3d_parallel,wxVolumeRenderingGui::onProjectionTypeParallel)
	EVT_RADIOBUTTON(IDREC3d_endoscopy,wxVolumeRenderingGui::onProjectionTypeEndoscopy)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPCursorType1,wxVolumeRenderingGui::onCursorType)
	EVT_RADIOBUTTON(IDREC3d_RADIOBUTTONGROUPCursorType2,wxVolumeRenderingGui::onCursorType)	
	EVT_TOGGLEBUTTON(IDREC3d_stereo,wxVolumeRenderingGui::onStereo)
	EVT_COMBOBOX(IDREC3d_COMBOBOXClut,wxVolumeRenderingGui::onCLUT)
	EVT_COMBOBOX(IDREC3d_COMBOBOXRenderingMode,wxVolumeRenderingGui::onChangeRendering)
	EVT_SLIDER(IDREC3d_LODslider, wxVolumeRenderingGui::onLODslider)
	EVT_CHECKBOX(IDREC3d_shadingCheckBox,wxVolumeRenderingGui::onActivateShading)
	EVT_MENU(IDREC3d_3dviewerResetToInitialView,wxVolumeRenderingGui::onResetView)
	EVT_MENU(IDREC3d_3dviewerAxialView,wxVolumeRenderingGui::onAxialView)	
	EVT_MENU(IDREC3d_3dviewerCoronalView,wxVolumeRenderingGui::onCoronalView)	
	EVT_MENU(IDREC3d_3dviewerLeftSagittalView,wxVolumeRenderingGui::onSagittalView)
	EVT_MENU(IDREC3d_3dviewerRightSagittalView,wxVolumeRenderingGui::onOppositeSagittalView)
	EVT_MENU(IDREC3d_HelpAbout,wxVolumeRenderingGui::onShowAboutDialog)	
	EVT_MENU(IDREC3d_fileCloseWindow,wxVolumeRenderingGui::onClose)
	EVT_MENU(IDREC3d_3dFullScreen,wxVolumeRenderingGui::onFullScreen)
	EVT_MENU(IDREC3d_WiiTool,wxVolumeRenderingGui::onWii)
	EVT_MENU(IDREC3d_WiiConfigurationTool,wxVolumeRenderingGui::onWiiConfiguration)
	EVT_MENU(IDREC3d_WiiChangeSceneIRInteraction,wxVolumeRenderingGui::onWiiChangeSceneIRInteraction)
	EVT_MENU(IDREC3d_ModifyCDRatioFunction,wxVolumeRenderingGui::onModifyCDRatioFunction)
	EVT_MENU(IDREC3d_ModifyMotionFactor,wxVolumeRenderingGui::onModifyMotionFactor)
	EVT_MENU(m_3d_WindowLevel,wxVolumeRenderingGui::onWindowLevel)
	EVT_MENU(m_3d_Move,wxVolumeRenderingGui::onMove)
	EVT_MENU(m_3d_Dolly,wxVolumeRenderingGui::onDolly)
	EVT_MENU(m_3d_Zoom,wxVolumeRenderingGui::onZoom)
	EVT_MENU(m_3d_Rotate,wxVolumeRenderingGui::onRotate)
	EVT_MENU(m_3d_RotateAround,wxVolumeRenderingGui::onRotateAround)
	EVT_MENU(m_3d_VolumeRendering,wxVolumeRenderingGui::onVolumeRendering)
	EVT_MENU(m_3d_TextureRendering,wxVolumeRenderingGui::onTextureRendering)
	EVT_MENU(m_3d_GPURendering,wxVolumeRenderingGui::onGPURendering)
	EVT_MENU(m_3d_FixedPointRendering,wxVolumeRenderingGui::onFixedPointRendering)
	EVT_MENU(m_3d_MIP,wxVolumeRenderingGui::onMIP)
	EVT_MENU(m_3d_voi,wxVolumeRenderingGui::onVOI)
	EVT_MENU(m_3d_StereoModeAnaglyphVR,wxVolumeRenderingGui::onStereoModeAnaglyph)
	EVT_MENU(m_3d_StereoModeActivePassiveVR,wxVolumeRenderingGui::onStereoModeActivePassive)
	EVT_MENU(m_3d_StereoModeCheckerboardVR,wxVolumeRenderingGui::onStereoModeCheckerboard)
	EVT_MENU(m_3d_ModifyObserverDistanceVR,wxVolumeRenderingGui::onModifyObserverDistance)
	EVT_MENU(m_3d_ModifyWLWWVR,wxVolumeRenderingGui::onModifyWLWW)
	EVT_MENU(m_3d_ModifyAvailableVRAM,wxVolumeRenderingGui::onModifyAvailableVRAM)
	EVT_CHAR_HOOK(wxVolumeRenderingGui::onChar)
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
	_boxWidget = vtkBoxWidgetWii::New();
	_clippingPlanes = vtkPlanes::New();
	_croppingInitialized = 0;

	// inizializza la distanza iniziale tra camera e volume
	_distance = 0;

	// inizializza la distanza tra osservatore e schermo
	_POD = 600;

	// inizializza i valori di zoom per parallel e perspective mode
	//_viewAngle = 0;
	//_parallelScale = 0;

	// inizializza la parallasse
	_parallax = 0;

	// inizializza lo zoom
	_globalZoomFactor = 1;
	
	// shading
	_isShadeActive = false;
	/*
	_shadingAmbient = 0.10;
	_shadingDiffuse = 1.00;
	_shadingSpecular = 0.10;
	_shadingSpecularPower = 0.40;
	*/
	_shadingAmbient = 0.20;
	_shadingDiffuse = 0.90;
	_shadingSpecular = 0.30;
	_shadingSpecularPower = 15.0;

	// memoria video disponibile
	_availableVRAM = -1;

	// cursore 3D
	_3DCursor = NULL;
	_3DCursorIsOnModelSurface = false;

	// Assi 3D passanti per il centro dell'oggetto
	_3DAxesActor = NULL;

	// VRAM disponibile
	_availableVRAM = -1;

	// _3DtextureSupported è true solo se sono supportate le 3D textures
	//_3DtextureSupported = false;

	// vale diventa true se si seleziona una delle visioni di tessuto predefinite
	//_tissueVisionSelected = false;

	// inizializza il passo di campionamento
	//_sampleDistance = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->computeSamplingDistance(3);

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
	menuRenderingMode->Append(m_3d_VolumeRendering,_("Floating point ray-casting"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->Append(m_3d_FixedPointRendering,_("Fixed point ray-casting"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->Append(m_3d_TextureRendering,_("3D Texture mapping"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->Append(m_3d_GPURendering,_("GPU-based ray-casting"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->Append(m_3d_MIP,_("Maximum Intensity Projection"),_T(""),wxITEM_NORMAL);
	menuRenderingMode->AppendSeparator();
	menuRenderingMode->Append(m_3d_ModifyAvailableVRAM,_("Modify available VRAM"),_T(""),wxITEM_NORMAL);
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
	_menuStereoMode->AppendCheckItem(m_3d_StereoModeCheckerboardVR,_("Checkerboard\tCtrl-H"),_T(""));
	_menuStereoMode->AppendSeparator();
	_menuStereoMode->Append(m_3d_ModifyObserverDistanceVR,_("Modify observer distance\tCtrl-B"),_T(""),wxITEM_NORMAL);
	menu3dViewer->Append(m_3d_StereoModeVR,"Stereo Mode",_menuStereoMode);

	wxMenu *menuInteraction = new wxMenu("", wxMENU_TEAROFF);
	menuInteraction->Append(m_3d_RotateAround,_("Rotate\tCtrl-A"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_Dolly,_("Dolly\tCtrl-D"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_Zoom,_("Zoom\tCtrl-Z"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(m_3d_Rotate,_("Roll\tCtrl-T"),_T(""),wxITEM_NORMAL);	
	menuInteraction->Append(m_3d_Move,_("Pan\tCtrl-M"),_T(""),wxITEM_NORMAL);	
	menuInteraction->Append(m_3d_voi,_("Crop\tCtrl-V"),_T(""),wxITEM_NORMAL);
	//menuInteraction->AppendSeparator();	
	//menuInteraction->Append(m_3d_WindowLevel,_("Window Level\tCtrl-W"),_T(""),wxITEM_NORMAL);	
	menuInteraction->AppendSeparator();		
	menuInteraction->Append(IDREC3d_WiiTool,_("Wiimote Quick Start\tCtrl-1"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(IDREC3d_WiiConfigurationTool,_("Wiimote Options\tCtrl-2"),_T(""),wxITEM_NORMAL);
	menuInteraction->Append(IDREC3d_WiiChangeSceneIRInteraction,_("Switch betweeb Wiimote ITs\tCtrl-3"),_T(""),wxITEM_NORMAL);
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
	wxBitmap AxialTool(this->GetBitmapResource(wxT("icons/Axial.xpm")));
	wxBitmap CorTool(this->GetBitmapResource(wxT("icons/Coronal.xpm")));
	wxBitmap SagTool(this->GetBitmapResource(wxT("icons/Sagittal.xpm")));
	wxBitmap sagoppTool(this->GetBitmapResource(wxT("icons/OppositeSagittal.xpm")));
	//wxBitmap SkinTool(this->GetBitmapResource(wxT("icons/Skin.xpm")));
	//wxBitmap MuscleTool(this->GetBitmapResource(wxT("icons/Muscle.xpm")));
	//wxBitmap BoneTool(this->GetBitmapResource(wxT("icons/Bone.xpm")));

	_toolbar->SetRows(1);

	// aggiunto da Luigi per compatibilità con Vista Aero
	_toolbar->SetBackgroundColour(*wxBLACK);
	_toolbar->SetForegroundColour(*wxWHITE);

	//Raggruppamento dei radioButton associati al inputDevice
	wxPanel* inputDeviceRadioPanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	inputDeviceRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* inputDeviceStaticBox = new wxStaticBox(inputDeviceRadioPanel, wxID_ANY, "Input");
	wxStaticBoxSizer* inputDeviceRadioSizer = new wxStaticBoxSizer( inputDeviceStaticBox, wxVERTICAL );	  
	wxBoxSizer* inputDeviceRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB1 = new wxRadioButton(inputDeviceRadioPanel, IDREC3d_RADIOBUTTONGROUPInputDevice1, "");	
	inputDeviceB1->SetForegroundColour(*wxWHITE);  
	inputDeviceB1->SetValue(true);
	wxStaticText* inputDeviceB1Text = new wxStaticText(inputDeviceRadioPanel,wxID_ANY,"Mouse");
	inputDeviceB1Text->SetForegroundColour(*wxWHITE);  
	inputDeviceRow1->Add( inputDeviceB1 , 0, wxLEFT, 0); 
	inputDeviceRow1->Add( inputDeviceB1Text , 0, wxLEFT | wxRIGHT, 0); 		
	wxBoxSizer* inputDeviceRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* inputDeviceB2 = new wxRadioButton(inputDeviceRadioPanel, IDREC3d_RADIOBUTTONGROUPInputDevice2, "");
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
	wxStaticBox* mouseFunctionsButtonsBox = new wxStaticBox(_toolbar, wxID_ANY, wxString("Mouse Functions"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(215,64));
	
	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* button3drotTool = new wxBitmapButton(_toolbar, IDREC3d_3drotTool,rot3dTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	button3drotTool->SetToolTip(_("ROTATE 3D"));
	_toolbar->AddControl(button3drotTool);
	currentHorizontalPosition=currentHorizontalPosition+button3drotTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonDolly = new wxBitmapButton(_toolbar, IDREC3d_dollyTool,dollyTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonDolly->SetToolTip(_("DOLLY"));
	_toolbar->AddControl(buttonDolly);
	currentHorizontalPosition=currentHorizontalPosition+buttonDolly->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonZoom = new wxBitmapButton(_toolbar, IDREC3d_zoomTool,zoomTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31),wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonZoom->SetToolTip(_("ZOOM"));
	_toolbar->AddControl(buttonZoom);
	currentHorizontalPosition=currentHorizontalPosition+buttonZoom->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonRotate = new wxBitmapButton(_toolbar, IDREC3d_RotateTool,RotateTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonRotate->SetToolTip(_("ROLL"));
	_toolbar->AddControl(buttonRotate);
	currentHorizontalPosition=currentHorizontalPosition+buttonRotate->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonMove = new wxBitmapButton(_toolbar, IDREC3d_moveTool,moveTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonMove->SetToolTip(_("PAN"));
	_toolbar->AddControl(buttonMove);
	currentHorizontalPosition=currentHorizontalPosition+buttonMove->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonVOITool = new wxBitmapButton(_toolbar, IDREC3d_VOITool, cut3dTool, wxPoint(currentHorizontalPosition,altezzatool),wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonVOITool->SetToolTip(_("CROP"));
	//buttonVOITool->Enable(false);
	_toolbar->AddControl(buttonVOITool);
	currentHorizontalPosition=currentHorizontalPosition+buttonVOITool->GetSize().GetWidth()-1;

	//Raggruppamento dei radioButton associati al cursorType
	currentHorizontalPosition=currentHorizontalPosition+10;
	wxPanel* cursorTypeRadioPanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,4));
	cursorTypeRadioPanel->SetForegroundColour( *wxWHITE);
	wxStaticBox* cursorTypeStaticBox = new wxStaticBox(cursorTypeRadioPanel, wxID_ANY, "Cursor"); 	
	wxStaticBoxSizer* cursorTypeRadioSizer = new wxStaticBoxSizer( cursorTypeStaticBox, wxVERTICAL );	  
	wxBoxSizer* cursorTypeRow1 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB1 = new wxRadioButton(cursorTypeRadioPanel, IDREC3d_RADIOBUTTONGROUPCursorType1, "");	
	cursorTypeB1->SetForegroundColour(*wxWHITE);  
	cursorTypeB1->SetValue(true);
	wxStaticText* cursorTypeB1Text = new wxStaticText(cursorTypeRadioPanel,wxID_ANY,"2D ");
	cursorTypeB1Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow1->Add( cursorTypeB1 , 0, wxLEFT, 0); 
	cursorTypeRow1->Add( cursorTypeB1Text , 0, wxLEFT | wxRIGHT, 3); 		
	wxBoxSizer* cursorTypeRow2 = new wxBoxSizer(wxHORIZONTAL);	
	wxRadioButton* cursorTypeB2 = new wxRadioButton(cursorTypeRadioPanel, IDREC3d_RADIOBUTTONGROUPCursorType2, "");
	wxStaticText* cursorTypeB2Text = new wxStaticText(cursorTypeRadioPanel,IDREC3d_3DcursorText,"3D ");
	cursorTypeB2Text->SetForegroundColour(*wxWHITE);  
	cursorTypeRow2->Add( cursorTypeB2 , 0, wxRIGHT, 3); 
	cursorTypeRow2->Add( cursorTypeB2Text , 0, wxRIGHT, 5);	 
	cursorTypeRadioSizer->Add( cursorTypeRow1 , 0, wxTOP , 6); 
	cursorTypeRadioSizer->Add( cursorTypeRow2 , 1, wxTOP | wxBOTTOM, 5); 	  
	cursorTypeRadioPanel->SetSizerAndFit(cursorTypeRadioSizer);		  
	currentHorizontalPosition=currentHorizontalPosition+cursorTypeRadioPanel->GetSize().GetWidth();

	// rendering combobox e slider
	currentHorizontalPosition = currentHorizontalPosition + 5;
	wxStaticBox* renderingModeStaticBox = new wxStaticBox(_toolbar, wxID_ANY, "Rendering Mode", wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(170,64)); 	
	currentHorizontalPosition=currentHorizontalPosition+10;

	_itemSlider = new wxSlider(_toolbar, IDREC3d_LODslider, 3, 0, 7, wxPoint(currentHorizontalPosition+30,altezzatool-5), wxSize(98,20), wxSL_INVERSE|wxSL_HORIZONTAL);
	_toolbar->AddControl(_itemSlider);
	wxStaticText* sx = new wxStaticText(_toolbar, wxID_STATIC, _("Worst"), wxPoint(currentHorizontalPosition+1,altezzatool-5), wxDefaultSize, 0 );
	wxStaticText* dx = new wxStaticText(_toolbar, wxID_STATIC, _("Best"), wxPoint(currentHorizontalPosition+128,altezzatool-5), wxDefaultSize, 0 );

	wxString renderingChoices[1] = {"Fixed point ray-casting"};
	_comboBoxRendering = new wxComboBox(_toolbar, IDREC3d_COMBOBOXRenderingMode, _("Fixed point ray-casting"), wxPoint(currentHorizontalPosition,altezzatool+15), wxSize(150,30), 1, renderingChoices, wxCB_READONLY);
	_toolbar->AddControl(_comboBoxRendering);

	currentHorizontalPosition=currentHorizontalPosition+_comboBoxRendering->GetSize().GetWidth();

	//Contenitore raggruppante Stereo e Parallel/Perspective/Endoscopy
	currentHorizontalPosition=currentHorizontalPosition+15;	
	wxStaticBox* shadingStaticBoxViewMode = new wxStaticBox(_toolbar, wxID_ANY, wxString("View mode"),wxPoint(currentHorizontalPosition,4),wxSize(145,64));
	currentHorizontalPosition=currentHorizontalPosition+8;	

	wxPanel* viewModePanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,19),wxSize(140,50));
	viewModePanel->SetForegroundColour( *wxWHITE);

	wxBoxSizer* viewModeBoxSizer = new wxBoxSizer(wxVERTICAL);	
	wxGridBagSizer* viewModeGridBagSizer;
	viewModeGridBagSizer = new wxGridBagSizer( 2, 5 );
	viewModeGridBagSizer->SetFlexibleDirection( wxBOTH );
	viewModeGridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );

	wxToggleButton* stereoToggleBtn = new wxToggleButton( viewModePanel, IDREC3d_stereo, wxT("Stereo"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	stereoToggleBtn->SetValue( false ); 
	viewModeGridBagSizer->Add( stereoToggleBtn, wxGBPosition( 0, 0 ), wxGBSpan( 3, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 0 );
	
	wxRadioButton* perspectiveRadioBtn = new wxRadioButton( viewModePanel, IDREC3d_perspective, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	perspectiveRadioBtn->SetValue( true );
	viewModeGridBagSizer->Add( perspectiveRadioBtn, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxStaticText* perspectiveStaticText = new wxStaticText( viewModePanel, wxID_ANY, wxT("Perspective"), wxDefaultPosition, wxDefaultSize, 0 );
	perspectiveStaticText->Wrap( -1 );
	perspectiveStaticText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	perspectiveStaticText->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	viewModeGridBagSizer->Add( perspectiveStaticText, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxRadioButton* parallelRadioBtn = new wxRadioButton( viewModePanel, IDREC3d_parallel, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	viewModeGridBagSizer->Add( parallelRadioBtn, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxStaticText* parallelStaticText = new wxStaticText( viewModePanel, wxID_ANY, wxT("Parallel"), wxDefaultPosition, wxDefaultSize, 0 );
	parallelStaticText->Wrap( -1 );
	parallelStaticText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	parallelStaticText->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	viewModeGridBagSizer->Add( parallelStaticText, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxRadioButton* endoscopyRadioBtn = new wxRadioButton( viewModePanel, IDREC3d_endoscopy, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	viewModeGridBagSizer->Add( endoscopyRadioBtn, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	
	wxStaticText* endoscopyStaticText = new wxStaticText( viewModePanel, wxID_ANY, wxT("Endoscopy"), wxDefaultPosition, wxDefaultSize, 0 );
	endoscopyStaticText->Wrap( -1 );
	endoscopyStaticText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	endoscopyStaticText->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	viewModeGridBagSizer->Add( endoscopyStaticText, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALL, 0 );

	viewModeBoxSizer->Add( viewModeGridBagSizer, 0, 0, 0 );
	viewModePanel->SetSizerAndFit( viewModeBoxSizer );
	currentHorizontalPosition=currentHorizontalPosition+viewModePanel->GetSize().GetWidth();

	//Contenitore raggruppante i Tissue Selection Buttons
	currentHorizontalPosition=currentHorizontalPosition+12;	
	wxStaticBox* tissueSelectionButtonsBox = new wxStaticBox(_toolbar, wxID_ANY, wxString("Tissue Selection"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(239,64));

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxString choices[12] = {"NO CLUT", "BlackBody", "Cardiac", "Flow", "GEColor", "GrainRainbow", "HotIron", "NIH", "Spectrum", "VR Bones", "VR Muscles-Bones", "VR Red Vessels"};
	_comboBoxClut = new wxComboBox(_toolbar, IDREC3d_COMBOBOXClut, _("NO CLUT"), wxPoint(currentHorizontalPosition,altezzatool+1), wxDefaultSize, 12, choices, wxCB_READONLY);
	_toolbar->AddControl(_comboBoxClut);
	currentHorizontalPosition=currentHorizontalPosition+_comboBoxClut->GetSize().GetWidth();

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonWLW = new wxBitmapButton(_toolbar, IDREC3d_WLWTool,WLWTool, wxPoint(currentHorizontalPosition,altezzatool),wxSize(31,31) ,wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonWLW->SetToolTip(_("Modify Transfer Function"));
	_toolbar->AddControl(buttonWLW);
	currentHorizontalPosition=currentHorizontalPosition+buttonWLW->GetSize().GetWidth();

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxStaticText* textWL = new wxStaticText(_toolbar, wxID_STATIC, _("WL:"), wxPoint(currentHorizontalPosition,altezzatool+1), wxSize(25,15), wxALIGN_LEFT );
	wxStaticText* textWW = new wxStaticText(_toolbar, wxID_STATIC, _("WW:"), wxPoint(currentHorizontalPosition,altezzatool+17), wxSize(25,15), wxALIGN_LEFT );
	_textWLlabel = new wxStaticText(_toolbar, wxID_STATIC, _("0"), wxPoint(currentHorizontalPosition+28,altezzatool+1), wxSize(34,15), wxST_NO_AUTORESIZE|wxALIGN_RIGHT );
	_textWWlabel = new wxStaticText(_toolbar, wxID_STATIC, _("0"), wxPoint(currentHorizontalPosition+28,altezzatool+17), wxSize(34,15), wxST_NO_AUTORESIZE|wxALIGN_RIGHT );
	currentHorizontalPosition=currentHorizontalPosition+10+textWL->GetSize().GetWidth()+_textWLlabel->GetSize().GetWidth();

	// SHADING
	currentHorizontalPosition=currentHorizontalPosition+8;	
	wxStaticBox* shadingStaticBox2 = new wxStaticBox(_toolbar, wxID_ANY, wxString("Shading"),wxPoint(currentHorizontalPosition,4),wxSize(205,64));
	currentHorizontalPosition=currentHorizontalPosition+10;	
	wxPanel* shadingPanel = new wxPanel(_toolbar, wxID_ANY, wxPoint(currentHorizontalPosition,19),wxSize(190,55));
	shadingPanel->SetForegroundColour( *wxWHITE);
	wxBoxSizer* shadingBoxSizer = new wxBoxSizer(wxVERTICAL);	
	wxGridBagSizer* shadingGridBagSizer;
	shadingGridBagSizer = new wxGridBagSizer( 2, 0 );
	shadingGridBagSizer->SetFlexibleDirection( wxBOTH );
	shadingGridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	shadingGridBagSizer->SetEmptyCellSize( wxSize( 10,10 ) );
	wxCheckBox*	shadingCheckBox = new wxCheckBox( shadingPanel, IDREC3d_shadingCheckBox, wxT("Activate"), wxDefaultPosition, wxDefaultSize, 0 );
	shadingCheckBox->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingCheckBox, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 0 );
	wxButton* shadingChangeButton = new wxButton( shadingPanel, IDREC3d_shadingChangeButton, wxT("change"), wxDefaultPosition, wxDefaultSize, 0 );
	shadingChangeButton->SetForegroundColour( wxColour( 0, 0, 0 ) );
	shadingGridBagSizer->Add( shadingChangeButton, wxGBPosition( 1, 0 ), wxGBSpan( 2, 1 ), wxALIGN_BOTTOM, 0 );
	wxStaticText* shadingAmbient = new wxStaticText( shadingPanel, wxID_ANY, wxT("ambient: "), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingAmbient->Wrap( -1 );
	shadingAmbient->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingAmbient, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	wxString ambientTxt = wxString::Format(_T("%1.2f"), _shadingAmbient);
	wxStaticText* shadingAmbientText = new wxStaticText( shadingPanel, IDREC3d_shadingAmbientText, ambientTxt, wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingAmbientText->Wrap( -1 );
	shadingAmbientText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingAmbientText, wxGBPosition( 0, 3 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	wxStaticText* shadingDiffuse = new wxStaticText( shadingPanel, wxID_ANY, wxT("diffuse: "), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingDiffuse->Wrap( -1 );
	shadingDiffuse->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingDiffuse, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	wxString diffuseTxt = wxString::Format(_T("%1.2f"), _shadingDiffuse);
	wxStaticText* shadingDiffuseText = new wxStaticText( shadingPanel, IDREC3d_shadingDiffuseText, diffuseTxt, wxPoint( -1,-1 ), wxDefaultSize, 0 );
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
	wxStaticText* shadingSpecularText = new wxStaticText( shadingPanel, IDREC3d_shadingSpecularText, specularAllTxt, wxPoint( -1,-1 ), wxDefaultSize, 0 );
	shadingSpecularText->Wrap( -1 );
	shadingSpecularText->SetForegroundColour( wxColour( 255, 255, 255 ) );
	shadingGridBagSizer->Add( shadingSpecularText, wxGBPosition( 2, 3 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT, 0 );
	shadingBoxSizer->Add( shadingGridBagSizer, 0, 0, 0 );
	shadingPanel->SetSizerAndFit( shadingBoxSizer );
	currentHorizontalPosition=currentHorizontalPosition+shadingPanel->GetSize().GetWidth();

	// VIEWS
	currentHorizontalPosition=currentHorizontalPosition+16;	
	wxStaticBox* viewButtonsBox = new wxStaticBox(_toolbar, wxID_ANY, wxString("Views"), wxPoint(currentHorizontalPosition,altezzaradiobox-1),wxSize(145,64));

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonAxialTool = new wxBitmapButton(_toolbar, IDREC3d_3dviewerAxialView,AxialTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	_toolbar->AddControl(buttonAxialTool);
	buttonAxialTool->SetToolTip(_("Move to an axial view"));
	currentHorizontalPosition=currentHorizontalPosition+buttonAxialTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonCorTool = new wxBitmapButton(_toolbar, IDREC3d_3dviewerCoronalView,CorTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonCorTool->SetToolTip(_("Move to a coronal view"));
	_toolbar->AddControl(buttonCorTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonCorTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonSagTool = new wxBitmapButton(_toolbar, IDREC3d_3dviewerLeftSagittalView,SagTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonSagTool->SetToolTip(_("Move to a left sagittal view"));
	_toolbar->AddControl(buttonSagTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonSagTool->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+5;
	wxBitmapButton* buttonSagOppTool = new wxBitmapButton(_toolbar, IDREC3d_3dviewerRightSagittalView,sagoppTool, wxPoint(currentHorizontalPosition,altezzatool), wxSize(31,31), wxBU_AUTODRAW|wxBU_EXACTFIT );
	buttonSagOppTool->SetToolTip(_("Move to a right sagittal view"));
	_toolbar->AddControl(buttonSagOppTool);
	currentHorizontalPosition=currentHorizontalPosition+buttonSagOppTool->GetSize().GetWidth()-1;

	this->SetToolBar(_toolbar);		

	(_toolbar->FindControl(IDREC3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
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
/*	else if (name == _T("icons/Skin.xpm"))
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
	}*/
    return wxNullBitmap;
////@end frame_3drec bitmap retrieval
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
		double currentFactor = ((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		wxMotionFactorDialog* d = new wxMotionFactorDialog(this, -1, currentFactor);
		if (d->ShowModal() == wxID_OK) 
			((vtkInteractorStyleWIITrackball*)appWxVtkI->GetInteractorStyle())->SetMotionFactor ( d->getMFactor() );
	}
	else {
		double currentFactor = ((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->GetMotionFactor();
		wxMotionFactorDialog* d = new wxMotionFactorDialog(this, -1, currentFactor);
		if (d->ShowModal() == wxID_OK) 
			((vtkInteractorStyleTrackballCamera*)appWxVtkI->GetInteractorStyle())->SetMotionFactor( d->getMFactor() );
	}	
}

void wxVolumeRenderingGui::onModifyAvailableVRAM(wxCommandEvent& WXUNUSED(event)) {
		char detectedVRAM[10];
		itoa(_availableVRAM,detectedVRAM,10);
		const wxChar* videoMemory = detectedVRAM;
		do {
			char message[60];
			strcpy(message,"Enter the amount of dedicated graphics RAM in MBs");
			wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, "VRAM", detectedVRAM, wxOK);
			if (dlg->ShowModal() == wxID_OK) {
				videoMemory = dlg->GetValue().c_str();
			}
			dlg->Destroy();
			if( !(atoi(videoMemory)>0) ) {
				wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Wrong VRAM size! Enter the amount of dedicated graphics RAM in MBs"), _T("MITO"), wxOK | wxICON_INFORMATION);
				if (messDialog->ShowModal()==wxID_OK)
					messDialog->Destroy();
			}
		} while( !(atoi(videoMemory)>0) );
		_availableVRAM = atoi(videoMemory); //in MB
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
		(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
		(_toolbar->FindControl(IDREC3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
		appWxVtkI->setInteractionType(rotateAround3d);
		_selectedButton = IDREC3d_3drotTool;		
		this->updateClippingPlanes(true);
	}
	else {
		(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
		(_toolbar->FindControl(IDREC3d_VOITool))->SetBackgroundColour( wxColour(255,255,0) );
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
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
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
	// nella modalità endoscopy il 3D axes viene disabilitato
	if ( ((wxRadioButton*)this->FindWindowById(IDREC3d_endoscopy,_toolbar))->GetValue() ) return;

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
	this->getBoundsDepthInfo(closest, farthest);

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

void wxVolumeRenderingGui::onWii( wxCommandEvent &event )
{	
	appWxVtkInteractor* appWxVtkI = ((appWxVtkInteractor*)_viewer3d->getWxWindow());	
	wxWiiManager* WiiManager = appWxVtkI->getWiiManager( _viewer3d );			

	if ( (appWxVtkI->getInteractionType() == wii) || (appWxVtkI->getInteractionType() == voi3dVRwii) )	//Operazioni da fare in uscita dalla modalità Wii
	{	
		this->ShowFullScreen(false);			

		//controllo se posso uscire o meno
		//if ( (appWxVtkI->idWiiEnabled != AnyWiiEnabled) || (WiiManager->WiiPointing_num) ) return;
		//else if (appWxVtkI->getInteractionType() == voi3dVRwii) appWxVtkI->InvokeEvent(vtkWiiHomeDown,NULL);			        
		//if (!appWxVtkI->closeWiiAbility()) return;

		// chiudo la VOI se aperta
		if (appWxVtkI->getInteractionType() == voi3dVRwii) appWxVtkI->InvokeEvent(vtkWiiHomeDown,NULL);

		_viewer3d->showLeftTopInfo();
		
		vtkMitoInteractorStyleTrackball *style = vtkMitoInteractorStyleTrackball::New();
		//vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();		

		appWxVtkI->closeWiiManager();					

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
		
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x/2,y/2);

		if (appWxVtkI->get3DcursorMode()) {
			this->show3DCursor();
			appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		}
		else {
			appWxVtkI->SetStillUpdateRate( 0.0001 );
		}
		// regola la definizione
		appWxVtkI->dummyMethod();

		// full screen nascondendo l'oggetto			
		double focalPoint[3];
		_viewer3d->getRenderer()->GetActiveCamera()->GetFocalPoint(focalPoint);
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint[0]+10000,focalPoint[1]+10000,focalPoint[2]+10000);					
		//_viewer3d->getRenderWindow()->Render();
		//wglMakeCurrent(NULL, NULL);				
		/*try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}*/
		_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint(focalPoint);					
		//_viewer3d->getRenderWindow()->Render();
//		wglMakeCurrent(NULL, NULL);						
		/*try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}*/

		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice1,_toolbar))->SetValue(true);
	}
	else if ( WiiManager->QuickSetupGui(_viewer3d->getWxWindow()) ) // entra
	{
		// passa alla visione perspective
		if ( !( ((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->GetValue() ) ) 
		{
			((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->SetValue(true);
			this->onProjectionTypePerspective(event);
		}

		_viewer3d->hideLeftTopInfo();
		_selectedButton = IDREC3d_WiiTool;
		
		// passa a bassa risoluzione
		appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		// regola la definizione
		appWxVtkI->dummyMethod();

		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->setupEventObservers(appWxVtkI);
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}

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

		this->ShowFullScreen(true);		
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

		vtkMitoInteractorStyleTrackball *style = vtkMitoInteractorStyleTrackball::New();
		//vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
		appWxVtkI->SetInteractorStyle(style);
		style->Delete();	

		WiiManager->Close();
		appWxVtkI->closeWiiManager();	

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
						
	

		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x/2,y/2);
		
		if (appWxVtkI->get3DcursorMode()) {
			this->show3DCursor();
			appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		}
		else {
			appWxVtkI->SetStillUpdateRate( 0.0001 );
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

		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPInputDevice1,_toolbar))->SetValue(true);
	}
	else if ( WiiManager->SetupGui(_viewer3d->getWxWindow()) ) // entra
	{
		// passa alla visione perspective
		if ( !( ((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->GetValue() ) )
		{
			((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->SetValue(true);
			this->onProjectionTypePerspective(event);
		}

		_viewer3d->hideLeftTopInfo();
		_selectedButton = IDREC3d_WiiTool;

		// passa a bassa risoluzione
		appWxVtkI->SetStillUpdateRate( appWxVtkI->GetDesiredUpdateRate() );
		// regola la definizione
		appWxVtkI->dummyMethod();

		vtkInteractorStyleWIITrackball *style = vtkInteractorStyleWIITrackball::New();
		appWxVtkI->SetInteractorStyle(style);
		style->setupEventObservers(appWxVtkI);
		style->Delete();		
					
		int x,y;
		this->GetSize(&x,&y);
		this->WarpPointer(x,y);
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

		try {
			_viewer3d->updateViewer();
		}
		catch(...) {
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
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
	wxBusyInfo wait("Please wait...");
	wxBusyCursor cursor;

	// disabilita il 3D cursor ma abilita la sua selezione
	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType1,_toolbar))->SetValue(true);
	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Enable(true);
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetStillUpdateRate( 0.0001 );	
	this->hide3DCursor();

	// passo all'interactor style trackball camera
	vtkMitoInteractorStyleTrackball *style = vtkMitoInteractorStyleTrackball::New();
	//vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetInteractorStyle(style);
	style->Delete();

	//_tissueVisionSelected=false;

	// rimuove il volume dalla finestra
	_viewer3d->getRenderer()->RemoveVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());

	// inizializza il passo di campionamento
	_sampleDistance = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->computeSamplingDistance(3); // 3 -> min spacing
	_itemSlider->SetValue(3);

	// inizializza a NO CLUT il tipo di clut
	_comboBoxClut->SetValue("NO CLUT");
	_clut = 0;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setCLUT(_clut);

	this->setWlWw(	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getWl() , 
					((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getWw() );
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setWlWw(_wl,_ww);

	volumeRenderingFilter volumeRendering(_idData, _mainGui->getDataHandler());

	bool is3DtextureSupported = volumeRendering.is3DtextureSupported( _viewer3d->getRenderer() );
	bool isGPUraycastingSupported = volumeRendering.isGPUraycastingSupported( _viewer3d->getRenderWindow() );
	bool isRGB = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb();

	if( isRGB ) // RGB
	{
		_comboBoxClut->Enable(false);
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.computeRgb(_wl, _ww, _clut, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
		this->SetTitle(_T("MITO: RGB fixed point ray-casting"));
		this->setRenderingTechnique(VolumeFixedPointRendering);	
		_comboBoxRendering->SetValue("Fixed point ray-casting");
	}
	else // non RGB
	{
		_comboBoxClut->Enable(true);
		if (isGPUraycastingSupported)
		{
			// aggiunta per liberare la memoria grafica per il passaggio 3D texture mapping -> GPU-based ray-casting
			if (getRenderingTechnique() == VolumeTextureRendering) {
				((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.computeFixedPoint(_wl, _ww, _clut, _minPixel, _sampleDistance*10000, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
				_viewer3d->getRenderer()->AddVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());
				_viewer3d->updateViewer();
				_viewer3d->getRenderer()->RemoveVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());
			}
			// fine passaggio 3D texture mapping -> GPU-based ray-casting
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.computeGPU(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower, _availableVRAM) );
			this->SetTitle(_T("MITO: GPU-based ray-casting"));
			this->setRenderingTechnique(GPURendering);
			_comboBoxRendering->SetValue("GPU-based ray-casting");
		}
		else if (is3DtextureSupported)
		{
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.computeTextures3D(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
			this->SetTitle(_T("MITO: 3D texture mapping"));
			this->setRenderingTechnique(VolumeTextureRendering);
			_comboBoxRendering->SetValue("3D texture mapping");
		}
		else 
		{
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.compute(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
			this->SetTitle(_T("MITO: Fixed point ray-casting"));
			this->setRenderingTechnique(VolumeFixedPointRendering);
			_comboBoxRendering->SetValue("Fixed point ray-casting");
		}			
	}

	// inizializza a OFF la vista stereoscopica sulla toolbar
	//_viewer3d->getRenderer()->GetActiveCamera()->SetEyeAngle(0);
	//_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	//((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPStereoMode1,_toolbar))->SetValue(true);

	// view mode
	((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->SetValue(true);
	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();

	// stereo mode
	((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->SetValue(false);
	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	_viewer3d->getRenderWindow()->StereoRenderOff();
	_parallax = 0;

	// disabilita lo shading
	vtkVolumeProperty *volumeProperty = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetProperty();
	_isShadeActive = false;
	_shadingAmbient = 0.20;
	_shadingDiffuse = 0.90;
	_shadingSpecular = 0.30;
	_shadingSpecularPower = 15.0;
	wxString ambientTxt = wxString::Format(_T("%1.2f"), _shadingAmbient);
	wxString diffuseTxt = wxString::Format(_T("%1.2f"), _shadingDiffuse);
	wxString specularTxt = wxString::Format(_T("%1.2f"), _shadingSpecular);
	wxString specularPowerTxt = wxString::Format(_T("%2.1f"), _shadingSpecularPower);
	wxString specularAllTxt = specularTxt + _T(", ") + specularPowerTxt;
	((wxStaticText*)this->FindWindowById(IDREC3d_shadingAmbientText,_toolbar))->SetLabel(ambientTxt);
	((wxStaticText*)this->FindWindowById(IDREC3d_shadingDiffuseText,_toolbar))->SetLabel(diffuseTxt);
	((wxStaticText*)this->FindWindowById(IDREC3d_shadingSpecularText,_toolbar))->SetLabel(specularAllTxt);
	volumeProperty->ShadeOff();
	volumeProperty->SetAmbient(_shadingAmbient);
	volumeProperty->SetDiffuse(_shadingDiffuse);
	volumeProperty->SetSpecular(_shadingSpecular);
	volumeProperty->SetSpecularPower(_shadingSpecularPower);
	((wxCheckBox*)this->FindWindowById(IDREC3d_shadingCheckBox,_toolbar))->SetValue(false);

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
	this->initializeCroppingBox();
	this->updateClippingPlanes();

	// setta la distanza iniziale della camera
	this->setInitialDistance();

	// setta rotate around 3D come stile di interazione
	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_toolbar->FindControl(IDREC3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = IDREC3d_3drotTool;
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3d);

	this->Raise();
	_viewer3d->updateViewer();
}


void wxVolumeRenderingGui::onAxialView(wxCommandEvent& event) {
	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetPosition(_initialPosition);

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


void wxVolumeRenderingGui::onCoronalView(wxCommandEvent& event) {
	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetPosition(_initialPosition);

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

void wxVolumeRenderingGui::onSagittalView(wxCommandEvent& event) {
	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetPosition(_initialPosition);

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

void wxVolumeRenderingGui::onOppositeSagittalView(wxCommandEvent& event) {
	// rimuove e poi aggiunge il 3D cursor per evitare slittamenti della camera
	if ( _3DCursor != NULL ) 
		_viewer3d->getRenderer()->RemoveActor( _3DCursor );

	// necessaria nel caso si sia ruotato il volume e non la camera
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetPosition(_initialPosition);

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

void wxVolumeRenderingGui::onChangeInputDevice(wxCommandEvent& event) {
	if(event.GetId() == IDREC3d_RADIOBUTTONGROUPInputDevice2 ) 
		wxVolumeRenderingGui::onWii(event);
}

void wxVolumeRenderingGui::setRenderingTechnique(techniqueRayCasting renderingTechnique) {
	_renderingTechnique = renderingTechnique;
	//_comboBoxClut->Enable(false);		

	/*
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
	*/
}

void wxVolumeRenderingGui::onChangeRendering(wxCommandEvent& event) {
	
	wxString newRenderingMode = _comboBoxRendering->GetValue();
	int renderingMode = -1;

	if (newRenderingMode == "MIP")
		renderingMode = MIP;
	else if (newRenderingMode == "Floating point ray-casting")
		renderingMode = VolumeRendering;
	else if (newRenderingMode == "Fixed point ray-casting")
		renderingMode = VolumeFixedPointRendering;
	else if (newRenderingMode == "3D texture mapping")
		renderingMode = VolumeTextureRendering;
	else if (newRenderingMode == "GPU-based ray-casting")
		renderingMode = GPURendering;


	_viewer3d->getRenderer()->RemoveVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());
	((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(0);

	/*if (getRenderingTechnique() == GPURendering) {
		vtkRenderWindow *renWin = _viewer3d->getRenderWindow();
		((vtkGPUVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper() )->ReleaseGraphicsResources(renWin);
	}
	else*/
	if (getRenderingTechnique() == VolumeTextureRendering && renderingMode == GPURendering) { 	// aggiunta per liberare la memoria grafica per il passaggio 3D texture mapping -> GPU-based ray-casting e viceversa
		volumeRenderingFilter tempVolumeRendering(_idData, _mainGui->getDataHandler());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( tempVolumeRendering.computeFixedPoint(_wl, _ww, _clut, _minPixel, _sampleDistance*10000, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
		_viewer3d->getRenderer()->AddVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());
		_viewer3d->updateViewer();
		_viewer3d->getRenderer()->RemoveVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume(0);
	} // fine passaggio 3D texture mapping -> GPU-based ray-casting

	// se c'è il 3D texture mapping fa si che il passo sia almeno pari al min spacing
	if (renderingMode == VolumeTextureRendering && _itemSlider->GetValue()>3)
		_itemSlider->SetValue(3);
	_sampleDistance = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->computeSamplingDistance(_itemSlider->GetValue());

	if(renderingMode == VolumeRendering) { // Floating point ray-casting
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( _volumeRendering.compute(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
		this->SetTitle(_T("MITO: Floating point ray-casting"));
		setRenderingTechnique(VolumeRendering);
	}
	
	else if(renderingMode == VolumeFixedPointRendering) { // Fixed point ray-casting
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		if ( !((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb() ) // non RGB
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( _volumeRendering.computeFixedPoint(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );		
		else // RGB
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( _volumeRendering.computeRgb(_wl, _ww, _clut, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );		
		this->SetTitle(_T("MITO: Fixed point ray-casting"));		
		setRenderingTechnique(VolumeFixedPointRendering);		
	}

	else if(renderingMode == VolumeTextureRendering) { // 3D texture mapping
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( _volumeRendering.computeTextures3D(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
		this->SetTitle(_T("MITO: 3D texture mapping"));
		setRenderingTechnique(VolumeTextureRendering);
	}

	else if(renderingMode == GPURendering) { // GPU-based ray-casting
		volumeRenderingFilter _volumeRendering(_idData, _mainGui->getDataHandler());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( _volumeRendering.computeGPU(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower, _availableVRAM) );		
		this->SetTitle(_T("MITO: GPU-based ray-casting"));		
		setRenderingTechnique(GPURendering);
	}

	else if(renderingMode == MIP) { // Maximum intensity projection
		mipFilter _mip(_idData, _mainGui->getDataHandler());
		if ( !((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb() ) // non RGB
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( _mip.compute(_wl, _ww, _clut, this->getMinPixel(), true, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
		else // RGB
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( _mip.computeRgb(_wl, _ww, _clut, true, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
		this->SetTitle(_T("MITO: Maximum intensity projection"));
		setRenderingTechnique(MIP);
	}

	_viewer3d->getRenderer()->AddVolume(((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume());
	_viewer3d->getRenderer()->ResetCameraClippingRange();

	// fa eseguire il render alla massima risoluzione (image sample distance a 1) - necessario ove fosse abilitato il 3D cursor che setta lo StillUpdateRate al DesiredUpdateRate
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetStillUpdateRate( 0.0001 );
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->dummyMethod();

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}

	// se il renderTime consente interattività e se non siamo in endoscopy mode abilita il 3D cursor
	if ( !((wxRadioButton*)this->FindWindowById(IDREC3d_endoscopy,_toolbar))->GetValue() )
		this->check3DcursorCapability();

	// se il cropping box e' stato attivato lo esegue anche sulla MIP / sul Volume
	if (_croppingInitialized) {
		_boxWidget->SetInteractor(_viewer3d->getRenderWindow()->GetInteractor());
		updateVolume();
	}	
}

void wxVolumeRenderingGui::onVolumeRendering(wxCommandEvent& event) {
	if(_renderingTechnique != VolumeRendering) {
		wxCommandEvent e;
		_comboBoxRendering->SetValue("Floating point ray-casting");
		e.SetId(IDREC3d_COMBOBOXRenderingMode);
		e.SetInt(VolumeRendering);
		onChangeRendering(e);		
	}
}

void wxVolumeRenderingGui::onFixedPointRendering(wxCommandEvent& event) {
	if(_renderingTechnique != VolumeFixedPointRendering) {
		_comboBoxRendering->SetValue("Fixed point ray-casting");
		wxCommandEvent e;
		e.SetId(IDREC3d_COMBOBOXRenderingMode);
		e.SetInt(VolumeFixedPointRendering);
		onChangeRendering(e);
	}
}

void wxVolumeRenderingGui::onTextureRendering(wxCommandEvent& event) {
	if(_renderingTechnique != VolumeTextureRendering) {
		_comboBoxRendering->SetValue("3D texture mapping");
		wxCommandEvent e;
		e.SetId(IDREC3d_COMBOBOXRenderingMode);
		e.SetInt(VolumeTextureRendering);
		onChangeRendering(e);
	}
}

void wxVolumeRenderingGui::onGPURendering(wxCommandEvent& event) {
	if(_renderingTechnique != GPURendering) {
		_comboBoxRendering->SetValue("GPU-based ray-casting");
		wxCommandEvent e;
		e.SetId(IDREC3d_COMBOBOXRenderingMode);
		e.SetInt(GPURendering);
		onChangeRendering(e);
	}
}
void wxVolumeRenderingGui::onMIP(wxCommandEvent& event) {
	if(_renderingTechnique != MIP) {
		_comboBoxRendering->SetValue("MIP");
		wxCommandEvent e;
		e.SetId(IDREC3d_COMBOBOXRenderingMode);
		e.SetInt(MIP);
		onChangeRendering(e);
	}
}

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
	_clut = event.GetInt();
	
	vtkVolumeProperty *volumeProperty = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetProperty();

	vtkPiecewiseFunction		*opacityTransferFunction = vtkPiecewiseFunction::New();
	vtkColorTransferFunction	*colorTransferFunction = vtkColorTransferFunction::New();

	int shiftValue = 0;
	if(_minPixel < 0) 
		shiftValue =(-1)*(_minPixel);
	float start = shiftValue + _wl - _ww/2, end = shiftValue + _wl + _ww/2;

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

	opacityTransferFunction->AddPoint(start, 0);
	opacityTransferFunction->AddPoint(end, 1);
	
	if(!((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb()) // non RGB
	{
		colorTransferFunction->BuildFunctionFromTable(start, end, 255, (double*) &table);
		volumeProperty->SetColor(colorTransferFunction);
		volumeProperty->SetScalarOpacity(opacityTransferFunction);
	}
	else // RGB
	{

		volumeProperty->IndependentComponentsOn();
		volumeProperty->SetComponentWeight(3, 0);

		volumeProperty->SetScalarOpacity(0, opacityTransferFunction);
		volumeProperty->SetScalarOpacity(1, opacityTransferFunction);
		volumeProperty->SetScalarOpacity(2, opacityTransferFunction);

		double tableRed[256][3];
		double tableGreen[256][3];
		double tableBlue[256][3];

		vtkColorTransferFunction *red = vtkColorTransferFunction::New();
		vtkColorTransferFunction *green = vtkColorTransferFunction::New();
		vtkColorTransferFunction *blue = vtkColorTransferFunction::New();

		if(_clut == 0) 
		{
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

			red->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&tableRed);
			green->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&tableGreen);
			blue->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&tableBlue);

			volumeProperty->SetColor(0, red);
			volumeProperty->SetColor(1, green);
			volumeProperty->SetColor(2, blue);

			red->Delete();
			green->Delete();
			blue->Delete();
		}
		else
		{
			colorTransferFunction->BuildFunctionFromTable(_wl-_ww/2, _wl+_ww/2, 255, (double*)&table);

			volumeProperty->SetColor(0, colorTransferFunction);
			volumeProperty->SetColor(1, colorTransferFunction);
			volumeProperty->SetColor(2, colorTransferFunction);
		}

	}

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setWlWw(_wl,_ww);
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setCLUT(_clut);

	_viewer3d->updateViewer();
	opacityTransferFunction->Delete();
	colorTransferFunction->Delete();
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
	/*if (_tissueVisionSelected) {
		onResetView(event);
		_comboBoxClut->Enable(true);	
	}*/

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_toolbar->FindControl(IDREC3d_WLWTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = IDREC3d_WLWTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(windowLevel3d);
	
	try 
	{
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
}

void wxVolumeRenderingGui::onMove(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_toolbar->FindControl(IDREC3d_moveTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = IDREC3d_moveTool;

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

void wxVolumeRenderingGui::onDolly(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_toolbar->FindControl(IDREC3d_dollyTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = IDREC3d_dollyTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(dolly3dVR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onZoom(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_toolbar->FindControl(IDREC3d_zoomTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = IDREC3d_zoomTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(zoom3dVR);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onRotate(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_toolbar->FindControl(IDREC3d_RotateTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = IDREC3d_RotateTool;

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

void wxVolumeRenderingGui::onRotateAround(wxCommandEvent& event) {
	// esce dalla VOI se attiva
	if (_selectedButton == IDREC3d_VOITool) onVOI(event);

	(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
	(_toolbar->FindControl(IDREC3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
	_selectedButton = IDREC3d_3drotTool;

	((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3d);
	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::onStereo( wxCommandEvent &event ) 
{
	if ( !( (wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar) )->GetValue() ) // modalità mono
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
		if ( _menuStereoMode->FindItem(m_3d_StereoModeActivePassiveVR)->IsChecked() ) 
			onStereoModeActivePassive(event);
		else if ( _menuStereoMode->FindItem(m_3d_StereoModeAnaglyphVR)->IsChecked() ) 
			onStereoModeAnaglyph(event);
		else 
			onStereoModeCheckerboard(event);
	}
}

void wxVolumeRenderingGui::onStereoModeAnaglyph(wxCommandEvent& event) {
	// disattiva la visione parallela se attiva
	if ( ((wxRadioButton*)this->FindWindowById(IDREC3d_parallel,_toolbar))->GetValue() ) 
	{
		((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->SetValue(true);
		this->onProjectionTypePerspective(event);
	}

	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyphVR)->Check(true);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassiveVR)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeCheckerboardVR)->Check(false);

	((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToAnaglyph();
	this->updateStereoView(true);
}

void wxVolumeRenderingGui::onStereoModeActivePassive(wxCommandEvent& event)
{
	// disattiva la visione parallela se attiva
	if ( ((wxRadioButton*)this->FindWindowById(IDREC3d_parallel,_toolbar))->GetValue() ) 
	{
		((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->SetValue(true);
		this->onProjectionTypePerspective(event);
	}

	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyphVR)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeActivePassiveVR)->Check(true);
	_menuStereoMode->FindItem(m_3d_StereoModeCheckerboardVR)->Check(false);

	((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToCrystalEyes();
	this->updateStereoView(true);
}

void wxVolumeRenderingGui::onStereoModeCheckerboard(wxCommandEvent& event) {
	// disattiva la visione parallela se attiva
	if ( ((wxRadioButton*)this->FindWindowById(IDREC3d_parallel,_toolbar))->GetValue() ) 
	{
		((wxRadioButton*)this->FindWindowById(IDREC3d_perspective,_toolbar))->SetValue(true);
		this->onProjectionTypePerspective(event);
	}

	_menuStereoMode->FindItem(m_3d_StereoModeActivePassiveVR)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeAnaglyphVR)->Check(false);
	_menuStereoMode->FindItem(m_3d_StereoModeCheckerboardVR)->Check(true);

	((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->SetValue(true);

	_viewer3d->getRenderWindow()->SetStereoTypeToCheckerboard();
	this->updateStereoView(true);
}

void wxVolumeRenderingGui::onActivateShading( wxCommandEvent &event ) {
	vtkVolumeProperty *volumeProperty = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetProperty();
	if ( event.GetSelection() == 1) // attiva shading
	{
		_isShadeActive = true;
		volumeProperty->ShadeOn();
		volumeProperty->SetAmbient(_shadingAmbient);
		volumeProperty->SetDiffuse(_shadingDiffuse);
		volumeProperty->SetSpecular(_shadingSpecular);
		volumeProperty->SetSpecularPower(_shadingSpecularPower);
	}
	else // disattiva shading
	{
		_isShadeActive = false;
		volumeProperty->ShadeOff();
	}
	_viewer3d->updateViewer();
}

void wxVolumeRenderingGui::onChangeShadingParameters( wxCommandEvent &event ) {
	// calcola dove posizionare la finestra
	int x = 14;
	int y = 14;
	int* xClient = &x;
	int* yClient = &y;
	this->ClientToScreen(xClient,yClient);
	
	wxShadingDialog* d = new wxShadingDialog(0, this, -1);
	d->SetSize(*xClient,*yClient,-1,-1);
	if (d->ShowModal() == wxID_OK) {	  
	}
}

void wxVolumeRenderingGui::onLODslider( wxCommandEvent &event ) {
	// si assicura che per il 3D texture la sampling distance sia almeno pari al min spacing
	if (_renderingTechnique == VolumeTextureRendering && _itemSlider->GetValue()>3)
		_itemSlider->SetValue(3);

	float newSampleDistance = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->computeSamplingDistance(_itemSlider->GetValue());
	if ( _sampleDistance != newSampleDistance) 
	{
		_sampleDistance = newSampleDistance;
		switch (_renderingTechnique) {

			case VolumeRendering:
				( (vtkVolumeRayCastMapper*)( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetMapper() )->SetSampleDistance(_sampleDistance);
				break;

			case VolumeFixedPointRendering:
				( (vtkFixedPointVolumeRayCastMapper*)( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetMapper() )->SetSampleDistance(_sampleDistance);
				break;

			case VolumeTextureRendering:
				( (vtkVolumeTextureMapper3D*)( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetMapper() )->SetSampleDistance(_sampleDistance);
				break;

			case GPURendering:
				( (vtkGPUVolumeRayCastMapper*)( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetMapper() )->SetSampleDistance(_sampleDistance);
				break;

			case MIP:
				( (vtkVolumeRayCastMapper*)( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetMapper() )->SetSampleDistance(_sampleDistance);
				break;
		} 
		_viewer3d->updateViewer();
		
		// se il renderTime consente interattività e se non siamo in endoscopy mode abilita il 3D cursor
		if ( !((wxRadioButton*)this->FindWindowById(IDREC3d_endoscopy,_toolbar))->GetValue() )
			this->check3DcursorCapability();
	}
}

void wxVolumeRenderingGui::onCursorType( wxCommandEvent &event ) {		
	if(event.GetId() == IDREC3d_RADIOBUTTONGROUPCursorType1) {
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetStillUpdateRate( 0.0001 );	
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
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}

}


void wxVolumeRenderingGui::onProjectionTypePerspective(wxCommandEvent& event) 
{
	// abilita il 3d cursor
	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Enable();

	// abilita il cropping
	((wxRadioButton*)this->FindWindowById(IDREC3d_VOITool,_toolbar))->Enable();
	this->updateClippingPlanes(false);
	this->updateVolume(false);

	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();

	// passo all'interactor style trackball camera
	vtkMitoInteractorStyleTrackball *style = vtkMitoInteractorStyleTrackball::New();
	//vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetInteractorStyle(style);
	style->Delete();

	// rimette a posto se c'è stato endoscopy
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetPosition(_initialPosition);
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint( ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetCenter() );

	this->updateStereoView(true);
	_viewer3d->getRenderer()->ResetCameraClippingRange();
}

void wxVolumeRenderingGui::onProjectionTypeParallel(wxCommandEvent& event) 
{
	// abilita il 3d cursor
	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Enable();

	// abilita il cropping
	((wxRadioButton*)this->FindWindowById(IDREC3d_VOITool,_toolbar))->Enable();
	if (!_croppingInitialized) {
		this->initializeCroppingBox();
	}
	this->updateClippingPlanes(false);
	this->updateVolume(false);

	// disattiva lo stereo se presente
	if ( ((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->GetValue() ) 
	{
		((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->SetValue(false);
		this->onStereo(event);
	}

	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOn();

	// passo all'interactor style trackball camera
	vtkMitoInteractorStyleTrackball *style = vtkMitoInteractorStyleTrackball::New();
	//vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetInteractorStyle(style);
	style->Delete();

	// rimette a posto se c'è stato endoscopy
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetOrientation(_initialOrientation);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetScale(_initialScale);
	( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->SetPosition(_initialPosition);
	_viewer3d->getRenderer()->GetActiveCamera()->SetFocalPoint( ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetCenter() );

	try {
		_viewer3d->updateViewer();
	}
	catch(...) {
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}
	_viewer3d->getRenderer()->ResetCameraClippingRange();
}

void wxVolumeRenderingGui::onProjectionTypeEndoscopy(wxCommandEvent& event) 
{
	// 3d cursor disattivato
	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType1,_toolbar))->SetValue(true);
	((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Disable();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->set3DcursorOff();
	this->hide3DCursor();

	// disattiva il cropping
	if (_selectedButton == IDREC3d_VOITool) {
		if (_boxWidget->GetEnabled()) {
			_boxWidget->EnabledOff();
		}
		(_toolbar->FindControl(_selectedButton))->SetBackgroundColour(wxNullColour);
		(_toolbar->FindControl(IDREC3d_3drotTool))->SetBackgroundColour( wxColour(255,255,0) );
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->setInteractionType(rotateAround3d);
		_selectedButton = IDREC3d_3drotTool;		
		this->updateClippingPlanes(true);
	}
	else if (!_croppingInitialized) {
		this->initializeCroppingBox();
		this->updateClippingPlanes(true);
	}
	((wxRadioButton*)this->FindWindowById(IDREC3d_VOITool,_toolbar))->Disable();
	((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->RemoveAllClippingPlanes();

	_viewer3d->getRenderer()->GetActiveCamera()->ParallelProjectionOff();

	// passo all'interactor style trackball actor
	vtkInteractorStyleTrackballActor *style = vtkInteractorStyleTrackballActor::New();
	((appWxVtkInteractor*)_viewer3d->getWxWindow())->SetInteractorStyle(style);
	style->Delete();

	this->updateStereoView(true);
	_viewer3d->getRenderer()->ResetCameraClippingRange();
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
				const wxString textWL = wxString::Format( wxT("%i"), (int)_wl);
				const wxString textWW = wxString::Format( wxT("%i"), (int)_ww);
				_textWLlabel->SetLabel(textWL);
				_textWWlabel->SetLabel(textWW);
				((appWxVtkInteractor*)_viewer3d->getWxWindow())->setWlWw(wl,ww);

				_viewer3d->setText();
				wxCommandEvent event;
				event.SetInt(_clut);
				onCLUT(event);
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
	if ( !((wxRadioButton*)this->FindWindowById(IDREC3d_endoscopy,_toolbar))->GetValue() ) { // cropping box non attivo su endoscopy
		((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->RemoveAllClippingPlanes();
		((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->SetClippingPlanes(_clippingPlanes);
		_viewer3d->getRenderer()->ResetCameraClippingRange();
	}
	else { // endoscopy mode
		((vtkVolumeRayCastMapper*)((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->GetMapper())->RemoveAllClippingPlanes();
		_viewer3d->getRenderer()->ResetCameraClippingRange();
	}

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

void wxVolumeRenderingGui::setInitialDistance() {
	_distance = _viewer3d->getRenderer()->GetActiveCamera()->GetDistance();
}

void wxVolumeRenderingGui::setInitialProjectionValues() {
	 ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetOrientation( _initialOrientation );
	 ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetScale( _initialScale );
	 ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetPosition( _initialPosition );
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
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
		Close(TRUE);
		return;
	}

}

void wxVolumeRenderingGui::updateStereoView(bool doARender) 
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

	if ( ((wxRadioButton*)this->FindWindowById(IDREC3d_endoscopy,_toolbar))->GetValue() ) // endoscopy mode
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

		if ( ((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->GetValue()) // modalità stereo
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
		if ( ((wxToggleButton*)this->FindWindowById(IDREC3d_stereo,_toolbar))->GetValue() ) // modalità stereo e volume non troppo indietro
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
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
			Close(TRUE);
			return;
		}
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
		wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set"));
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

	// se il renderTime consente interattività e se non siamo in endoscopy mode abilita il 3D cursor
	if ( !((wxRadioButton*)this->FindWindowById(IDREC3d_endoscopy,_toolbar))->GetValue() )
		this->check3DcursorCapability();

	Raise();
}

void wxVolumeRenderingGui::onShowAboutDialog(wxCommandEvent& WXUNUSED(event)) {
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

void wxVolumeRenderingGui::checkSuitableMappers()
{
	volumeRenderingFilter volumeRendering(_idData, _mainGui->getDataHandler());
	bool isRGB = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getRgb();

	unsigned int idActiveViewer = _mainGui->getActiveViewer();
	bool is3DtextureSupported = volumeRendering.is3DtextureSupported( ((wxVtkViewer3d*)( _mainGui->getViewer(idActiveViewer)->getViewerDrawing()) )->getRenderer() );
	bool isGPUraycastingSupported = volumeRendering.isGPUraycastingSupported( ((wxVtkViewer3d*)( _mainGui->getViewer(idActiveViewer)->getViewerDrawing()) )->getRenderWindow() );
	if ( isGPUraycastingSupported ) { // setta la dimensione della dedicated VRAM
		_availableVRAM = volumeRendering.computeAvailableGPUMemory();
		if ( !(_availableVRAM>0) )
			_availableVRAM = 128;
		else if ( !(_availableVRAM<2048) )
			_availableVRAM = 2047;
	}

	if( isRGB ) // RGB
	{
		_comboBoxClut->Enable(false);
		_comboBoxRendering->Insert(wxT("MIP"), _comboBoxRendering->GetCount());
		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.computeRgb(_wl, _ww, _clut, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
		this->SetTitle(_T("MITO: RGB fixed point ray-casting"));
		this->setRenderingTechnique(VolumeFixedPointRendering);	
		_comboBoxRendering->SetValue("Fixed point ray-casting");
	}
	else // non RGB
	{
		_comboBoxClut->Enable(true);
		_comboBoxRendering->Insert(wxT("Floating point ray-casting"), _comboBoxRendering->GetCount());
		_comboBoxRendering->Insert(wxT("MIP"), _comboBoxRendering->GetCount());
		if (is3DtextureSupported)
			_comboBoxRendering->Insert(wxT("3D texture mapping"), _comboBoxRendering->GetCount());

		if (isGPUraycastingSupported)
		{
			_comboBoxRendering->Insert(wxT("GPU-based ray-casting"), _comboBoxRendering->GetCount());
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.computeGPU(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower, _availableVRAM) );
			this->SetTitle(_T("MITO: GPU-based ray-casting"));
			this->setRenderingTechnique(GPURendering);
			_comboBoxRendering->SetValue("GPU-based ray-casting");
		}
		else if (is3DtextureSupported)
		{
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.computeTextures3D(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
			this->SetTitle(_T("MITO: 3D texture mapping"));
			this->setRenderingTechnique(VolumeTextureRendering);
			_comboBoxRendering->SetValue("3D texture mapping");
		}
		else 
		{
			((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->setVtkVolume( volumeRendering.compute(_wl, _ww, _clut, _minPixel, _sampleDistance, _isShadeActive, _shadingAmbient, _shadingDiffuse, _shadingSpecular, _shadingSpecularPower) );
			this->SetTitle(_T("MITO: Fixed point ray-casting"));
			this->setRenderingTechnique(VolumeFixedPointRendering);
			_comboBoxRendering->SetValue("Fixed point ray-casting");
		}			
	}
} 

void wxVolumeRenderingGui::check3DcursorCapability()
{
	double renderTime = _viewer3d->getRenderer()->GetLastRenderTimeInSeconds();
	if (renderTime > 0.2) // se non supporta almeno 5 colpi render al secondo disabilita il 3D cursor
	{
		wxCommandEvent event = NULL;
		// disabilita il 3D cursor se attivo
		if ( ((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->GetValue() ) {
			((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType1,_toolbar))->SetValue(true);
			event.SetId(IDREC3d_RADIOBUTTONGROUPCursorType1);
			this->onCursorType(event);
		}
		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Enable(false);
	}
	else // altrimenti abilita il 3D cursor
	{
		((wxRadioButton*)this->FindWindowById(IDREC3d_RADIOBUTTONGROUPCursorType2,_toolbar))->Enable(true);
	}
}

void wxVolumeRenderingGui::updateShading(double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower) {
	
	((wxCheckBox*)this->FindWindowById(IDREC3d_shadingCheckBox,_toolbar))->SetValue(true);
	_isShadeActive = true;

	_shadingAmbient = shadingAmbient;
	_shadingDiffuse = shadingDiffuse;
	_shadingSpecular = shadingSpecular;
	_shadingSpecularPower = shadingSpecularPower;
	
	vtkVolumeProperty *volumeProperty = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetProperty();
	volumeProperty->ShadeOn();
	volumeProperty->SetAmbient(_shadingAmbient);
	volumeProperty->SetDiffuse(_shadingDiffuse);
	volumeProperty->SetSpecular(_shadingSpecular);
	volumeProperty->SetSpecularPower(_shadingSpecularPower);
	
	wxString ambientTxt = wxString::Format(_T("%1.2f"), _shadingAmbient);
	wxString diffuseTxt = wxString::Format(_T("%1.2f"), _shadingDiffuse);
	wxString specularTxt = wxString::Format(_T("%1.2f"), _shadingSpecular);
	wxString specularPowerTxt = wxString::Format(_T("%2.1f"), _shadingSpecularPower);
	wxString specularAllTxt = specularTxt + _T(", ") + specularPowerTxt;

	((wxStaticText*)this->FindWindowById(IDREC3d_shadingAmbientText,_toolbar))->SetLabel(ambientTxt);
	((wxStaticText*)this->FindWindowById(IDREC3d_shadingDiffuseText,_toolbar))->SetLabel(diffuseTxt);
	((wxStaticText*)this->FindWindowById(IDREC3d_shadingSpecularText,_toolbar))->SetLabel(specularAllTxt);

	_viewer3d->updateViewer();
}
void wxVolumeRenderingGui::computeOpacityColor()
{
		vtkVolumeProperty *volumeProperty = ( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getVtkVolume()->GetProperty();
		vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();

		if ( !( (itkVtkData*)_mainGui->getDataHandler()->getData(_idData) )->getRgb() ) // NON RGB
		{
			vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();

			int minPixel = ((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getMinPixelValue();
			int shiftValue = 0;
			if( minPixel < 0 ) 
				shiftValue =(-1)*(minPixel);
			float start = shiftValue + _wl - _ww/2, end = shiftValue + _wl + _ww/2;

			opacityTransferFunction->RemoveAllPoints();
			opacityTransferFunction->AddPoint(start, 0);
			opacityTransferFunction->AddPoint(end, 1);

			volumeProperty->SetScalarOpacity(opacityTransferFunction);

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

			colorTransferFunction->BuildFunctionFromTable(shiftValue + _wl - _ww/2, shiftValue + _wl + _ww/2, 255, (double*)&table);
			volumeProperty->SetColor(colorTransferFunction);
			colorTransferFunction->Delete();
		}

		else // RGB
		{
			float start = _wl - _ww/2, end = _wl + _ww/2;
			
			opacityTransferFunction->RemoveAllPoints();
			opacityTransferFunction->AddPoint(start, 0);
			opacityTransferFunction->AddPoint(end, 1);

			volumeProperty->SetScalarOpacity(0,opacityTransferFunction);
			volumeProperty->SetScalarOpacity(1,opacityTransferFunction);
			volumeProperty->SetScalarOpacity(2,opacityTransferFunction);
		}

		((itkVtkData*)_mainGui->getDataHandler()->getData(_idData))->getVtkVolume()->Update();
		opacityTransferFunction->Delete();
}