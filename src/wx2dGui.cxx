/**
 * \file wx2dGui.cxx
 * \brief File per la gestione della finestra 2D
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/splitter.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/busyinfo.h>

#include "wx2dGui.h"
#include "exportFilter.h"
#include "flipFilter.h"
#include "infoFilter.h"
#include "itkVtkData.h"
#include "mipFilter.h"
//#include "minipFilter.h"
#include "roiManager.h"
#include "segmentationFilter.h"
#include "surfaceRenderingFilter.h"
#include "volumeRenderingFilter.h"
#include "segmentationManager.h"
#include "wxAnonymizeDialog.h"
#include "wxSurfaceRenderingGui.h"
#include "wxVolumeRenderingGui.h"
#include "wxWLWWDialog.h"

#include "icons/Album.xpm"
#include "icons/All.xpm"
#include "icons/Animate.xpm"
#include "icons/Move.xpm"
#include "icons/Zoom.xpm"
#include "icons/Rotate.xpm"
#include "icons/WindowLevel.xpm"
#include "icons/Length.xpm"
#include "icons/Angle.xpm"
#include "icons/Rectangle.xpm"
#include "icons/Polygon.xpm"
#include "icons/Pencil.xpm"
#include "icons/ConnectedThreshold.xpm"
#include "icons/NeighborhoodConnected.xpm"
#include "icons/ConfidenceConnected.xpm"
#include "icons/VolumeRendering.xpm"
#include "icons/Surface.xpm"
#include "icons/MIP.xpm"
//#include "icons/MinIP.xpm"

BEGIN_EVENT_TABLE(wx2dGui, wxFrame)
	EVT_MENU(m_2d_fileShowAlbum,wx2dGui::onAlbum)	
	EVT_MENU(m_2d_viewerResetImageView,wx2dGui::onResetView)	
	EVT_MENU(m_2d_viewerNextSlice,wx2dGui::onNextSlice)
	EVT_MENU(m_2d_viewerPrevSlice,wx2dGui::onPreviousSlice)
	EVT_MENU(m_2d_fileHeader,wx2dGui::onViewHeader)
	EVT_MENU(m_2d_viewerFlipImageHorizontal,wx2dGui::onFlipHorizontal)
	EVT_MENU(m_2d_viewerFlipImageVertical,wx2dGui::onFlipVertical)
	EVT_MENU(m_2d_ModifyWLWW,wx2dGui::onModifyWLWW)
	EVT_MENU(m_2d_All,wx2dGui::onAll)
	EVT_MENU(m_2d_Window_Level_Width,wx2dGui::onWindowLevel)
	EVT_MENU(m_2d_Move,wx2dGui::onMove)
	EVT_MENU(m_2d_Zoom,wx2dGui::onZoom)
	EVT_MENU(m_2d_Rotate,wx2dGui::onRotate)
	EVT_MENU(m_2d_Animate,wx2dGui::onAnimate)
	EVT_MENU(m_2d_Length,wx2dGui::onLength)
	EVT_MENU(m_2d_Angle,wx2dGui::onAngle)
	EVT_MENU(m_2d_Rectangle,wx2dGui::onRectangle)
	EVT_MENU(m_2d_Polygon,wx2dGui::onPolygon)
	EVT_MENU(m_2d_Pencil,wx2dGui::onPencil)
	EVT_MENU(m_2d_fileAnonymize,wx2dGui::onAnonymize)
	EVT_MENU(m_2d_fileExportDICOM,wx2dGui::onExportToDicom)
	EVT_MENU(m_2d_fileExportBMP,wx2dGui::onExportToBmp)
	EVT_MENU(m_2d_fileClose,wx2dGui::onClose)
	EVT_MENU(m_2d_SelectRoi,wx2dGui::onSelectRoi)	
	EVT_MENU(m_2d_DeleteRoi,wx2dGui::onDeleteRoi)
	EVT_MENU(m_2d_ExtractRoiInside,wx2dGui::onExtractRoiInside)	
	EVT_MENU(m_2d_ExtractRoiOutside,wx2dGui::onExtractRoiOutside)	
	EVT_MENU(m_2d_Volume_Rendering,wx2dGui::onVolumeRendering)
	EVT_MENU(m_2d_Surface_Rendering,wx2dGui::onSurfaceRendering)
	EVT_MENU(m_2d_Maximum_Intensity_Projection,wx2dGui::onMaximumIntensityProjection)
	//EVT_MENU(m_2d_Minimum_Intensity_Projection,wx2dGui::onMinimumIntensityProjection)
	EVT_MENU(m_2d_ConnectedThreshold,wx2dGui::onConnectedThreshold)	
	EVT_MENU(m_2d_NeighborhoodConnected,wx2dGui::onNeighborhoodConnected)	
	EVT_MENU(m_2d_ConfidenceConnected,wx2dGui::onConfidenceConnected)	
	EVT_MENU(m_2d_HelpAbout,wx2dGui::onShowAboutDialog)	

	EVT_CHECKBOX(id_2dAutoPlay, wx2dGui::onAutoPlay)

	EVT_BUTTON(id_2dPlay,wx2dGui::onPlay)
	EVT_BUTTON(id_2dStop,wx2dGui::onStop)
	EVT_BUTTON(tool_album,wx2dGui::onAlbum)
	EVT_BUTTON(tool_all,wx2dGui::onAll)
	EVT_BUTTON(tool_windowlevel,wx2dGui::onWindowLevel)
	EVT_BUTTON(tool_move,wx2dGui::onMove)
	EVT_BUTTON(tool_zoom,wx2dGui::onZoom)
	EVT_BUTTON(tool_rotate,wx2dGui::onRotate)
	EVT_BUTTON(tool_animate,wx2dGui::onAnimate)
	EVT_BUTTON(tool_length,wx2dGui::onLength)
	EVT_BUTTON(tool_angle,wx2dGui::onAngle)
	EVT_BUTTON(tool_rectangle,wx2dGui::onRectangle)
	EVT_BUTTON(tool_polygon,wx2dGui::onPolygon)
	EVT_BUTTON(tool_pencil,wx2dGui::onPencil)
	EVT_BUTTON(tool_volumerendering,wx2dGui::onVolumeRendering)
	EVT_BUTTON(tool_surface,wx2dGui::onSurfaceRendering)
	EVT_BUTTON(tool_mip,wx2dGui::onMaximumIntensityProjection)
	//EVT_BUTTON(tool_minip,wx2dGui::onMinimumIntensityProjection)
	EVT_BUTTON(tool_connectedthreshold,wx2dGui::onConnectedThreshold)	
	EVT_BUTTON(tool_neighborhoodconnected,wx2dGui::onNeighborhoodConnected)	
	EVT_BUTTON(tool_confidenceconnected,wx2dGui::onConfidenceConnected)	

	EVT_SLIDER(id_2dSlider, wx2dGui::onSliderUpdate)
	EVT_MOUSEWHEEL(wx2dGui::OnMouseWheel)
	EVT_ACTIVATE(wx2dGui::onActivate)
	EVT_CLOSE(wx2dGui::onQuit)
END_EVENT_TABLE()

wx2dGui::wx2dGui(const wxString& title, const wxPoint& pos, const wxSize& size)
       //: wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxDOUBLE_BORDER) {
	     : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE ) {

	/*wxIcon icon;
	icon.LoadFile("icon.ico",wxBITMAP_TYPE_ICO);
	ths->SetIcon(icon);*/

   	SetIcon(wxICON(MITO));

   _preview = false;
	_working = false;
	_selectedButton = tool_all;

	wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
	menuFile->Append(m_2d_fileShowAlbum, "Show Album\tCtrl-S");
	menuFile->AppendSeparator();
	_menuFileExport = new wxMenu;
    _menuFileExport->Append(m_2d_fileExportBMP, "Export to BMP\tCtrl-J");
	_menuFileExport->Append(m_2d_fileExportDICOM, "Export to DICOM\tCtrl-D");
	menuFile->Append(m_2d_fileExport, "Export", _menuFileExport);
	menuFile->AppendSeparator();
	menuFile->Append(m_2d_fileHeader, "Show DICOM Header\tCtrl-H");
	menuFile->Append(m_2d_fileAnonymize, "Anonymize\tCtrl-A");
	menuFile->AppendSeparator();
	menuFile->Append(m_2d_fileClose, "Close 2D Viewer\tCtrl-C");

	wxMenu *menuViewer = new wxMenu("", wxMENU_TEAROFF);
	menuViewer->Append(m_2d_viewerNextSlice, "Next Slice\tCtrl-N");
	menuViewer->Append(m_2d_viewerPrevSlice, "Previous Slice\tCtrl-P");
	menuViewer->AppendSeparator();
	menuViewer->Append(m_2d_viewerResetImageView, "Reset Image View\tCtrl-R"); 
	menuViewer->Enable(m_2d_viewerResetImageView, true);
	menuViewer->Append(m_2d_viewerFlipImageHorizontal, "Flip Image Horizontal\tCtrl-O");
	menuViewer->Append(m_2d_viewerFlipImageVertical, "Flip Image Vertical\tCtrl-V");
	
	menuViewer->AppendSeparator();
	menuViewer->Append(m_2d_ModifyWLWW, "Modify WL/WW\tCtrl-W");

	menuViewer->AppendSeparator();
	wxMenu *menuMouseFunctions = new wxMenu; 
	menuMouseFunctions->Append(m_2d_All,_("All functions"),_T(""),wxITEM_NORMAL);
	menuMouseFunctions->Append(m_2d_Window_Level_Width,_("Window Level/Width"),_T(""),wxITEM_NORMAL);
	menuMouseFunctions->Append(m_2d_Move,_("Move"),_T(""),wxITEM_NORMAL);
	menuMouseFunctions->Append(m_2d_Zoom,_("Zoom"),_T(""),wxITEM_NORMAL);
	menuMouseFunctions->Append(m_2d_Rotate,_("Rotate"),_T(""),wxITEM_NORMAL);
	menuMouseFunctions->Append(m_2d_Animate,_("Animate"),_T(""),wxITEM_NORMAL);
	menuViewer->Append(m_2d_Mouse_Button_Function,"Mouse Functions",menuMouseFunctions);

	wxMenu *menu3DViewer=new wxMenu;
	menu3DViewer->Append(m_2d_Volume_Rendering,"Direct Volume Rendering");
	menu3DViewer->Append(m_2d_Surface_Rendering,"Indirect Volume Rendering");
	//menu3DViewer->Append(m_2d_Maximum_Intensity_Projection,"Maximum Intensity Projection");
	//menu3DViewer->Append(m_2d_Minimum_Intensity_Projection,"Minimum Intensity Projection");

	wxMenu *menuRoi = new wxMenu;
	menuRoi->Append(m_2d_SelectRoi, "Select ROI");
	menuRoi->Append(m_2d_DeleteRoi, "Delete ROI");
	wxMenu *menuRoiFunction = new wxMenu;
	menuRoi->AppendSeparator();
	menuRoiFunction->Append(m_2d_Length,_("Length"),_T(""),wxITEM_NORMAL);
	menuRoiFunction->Append(m_2d_Angle,_("Angle"),_T(""),wxITEM_NORMAL);
	menuRoiFunction->Append(m_2d_Polygon,_("Polygon"),_T(""),wxITEM_NORMAL);
	menuRoiFunction->Append(m_2d_Rectangle,_("Rectangle"),_T(""),wxITEM_NORMAL);
	menuRoiFunction->Append(m_2d_Pencil,_("Pencil"),_T(""),wxITEM_NORMAL);
	menuRoi->Append(m_2d_Roi_Function,"ROI Tools",menuRoiFunction);
	menuRoi->AppendSeparator();
	wxMenu *menuRoiExtract = new wxMenu;
    menuRoiExtract->Append(m_2d_ExtractRoiInside, "Inside Roi");
	menuRoiExtract->Append(m_2d_ExtractRoiOutside, "Outside Roi");
	menuRoi->Append(m_2d_ExtractRoi, "Extract ROI",menuRoiExtract);

	wxMenu *menuRegion=new wxMenu;
	menuRegion->Append(m_2d_ConnectedThreshold,"Connected Threshold");
	menuRegion->Append(m_2d_NeighborhoodConnected,"Neighborhood Connected");
	menuRegion->Append(m_2d_ConfidenceConnected,"Confidence Connected ");
	menuRoi->Append(m_2d_Region,"Region growing (2D Segmentation)",menuRegion);

	wxMenu *menuHelp = new wxMenu("", wxMENU_TEAROFF);
	menuHelp->Append(m_2d_HelpAbout, "About MITO"); 

	wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
	menuBar->Append(menuViewer, "&2D Viewer");
	menuBar->Append(menu3DViewer,"&3D Viewer");
	menuBar->Append(menuRoi, "&ROI" );
	menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

	long style = wxTB_FLAT | wxTB_DOCKABLE| wxTB_TEXT | wxTB_HORIZONTAL;
	
	_toolBar = CreateToolBar(style, id_HorToolBar);

	// aggiunto da Luigi per compatibilità con Vista Aero
	_toolBar->SetBackgroundColour(*wxBLACK);
	_toolBar->SetForegroundColour(*wxWHITE);

	_toolBar->Realize();


	// Set up _toolbar
    wxBitmap toolBarBitmaps[tool_Max];

	#define INIT_TOOL_BMP(bmp) \
	toolBarBitmaps[tool_##bmp] = wxBitmap(bmp##_xpm)

    INIT_TOOL_BMP(album);
    INIT_TOOL_BMP(all);
    INIT_TOOL_BMP(windowlevel);
    INIT_TOOL_BMP(move);
    INIT_TOOL_BMP(zoom);
    INIT_TOOL_BMP(rotate);
	INIT_TOOL_BMP(animate);
    INIT_TOOL_BMP(length);
    INIT_TOOL_BMP(angle);
    INIT_TOOL_BMP(polygon);
    INIT_TOOL_BMP(rectangle);
    INIT_TOOL_BMP(pencil);
    INIT_TOOL_BMP(connectedthreshold);
    INIT_TOOL_BMP(neighborhoodconnected);
    INIT_TOOL_BMP(confidenceconnected);
	INIT_TOOL_BMP(volumerendering);
    INIT_TOOL_BMP(surface);
    INIT_TOOL_BMP(mip);
    //INIT_TOOL_BMP(minip);

    _toolBar->SetToolBitmapSize(wxSize(50, 51));



	int altezzaButton = 15;
	int currentHorizontalPosition = 0;

	//Contenitore raggruppante l'album button
	currentHorizontalPosition=currentHorizontalPosition+10;
	wxStaticBox* albumBox = new wxStaticBox(_toolBar, wxID_ANY, wxString("Album"), wxPoint(currentHorizontalPosition,altezzaButton-15),wxSize(51,53));

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* albumButton = new wxBitmapButton( _toolBar, tool_album, toolBarBitmaps[tool_album], wxPoint(currentHorizontalPosition,altezzaButton),wxSize(31,31));
	albumButton->SetToolTip(_("Close 2d viewer and open Album window"));
	_toolBar->AddControl(albumButton);
	currentHorizontalPosition=currentHorizontalPosition+albumButton->GetSize().GetWidth()-1;

	
	//Contenitore raggruppante i Mouse Functions Buttons
	currentHorizontalPosition=currentHorizontalPosition+35;	
	wxStaticBox* mouseFunctionsButtonsBox = new wxStaticBox(_toolBar, wxID_ANY, wxString("Mouse Functions"), wxPoint(currentHorizontalPosition,altezzaButton-15),wxSize(250,53));

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* allButton = new wxBitmapButton( _toolBar, tool_all, toolBarBitmaps[tool_all], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	allButton->SetToolTip(_("All functions"));
	_toolBar->AddControl(allButton);
	currentHorizontalPosition=currentHorizontalPosition+allButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* windowLevelButton = new wxBitmapButton( _toolBar, tool_windowlevel, toolBarBitmaps[tool_windowlevel], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	windowLevelButton->SetToolTip(_("Window Level"));
	_toolBar->AddControl(windowLevelButton);
	currentHorizontalPosition=currentHorizontalPosition+windowLevelButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* moveButton = new wxBitmapButton( _toolBar, tool_move, toolBarBitmaps[tool_move], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	moveButton->SetToolTip(_("Move"));
	_toolBar->AddControl(moveButton);
	currentHorizontalPosition=currentHorizontalPosition+moveButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* zoomButton = new wxBitmapButton( _toolBar, tool_zoom, toolBarBitmaps[tool_zoom], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	zoomButton->SetToolTip(_("Zoom"));
	_toolBar->AddControl(zoomButton);
	currentHorizontalPosition=currentHorizontalPosition+zoomButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* rotateButton = new wxBitmapButton( _toolBar, tool_rotate, toolBarBitmaps[tool_rotate], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	rotateButton->SetToolTip(_("Rotate"));
	_toolBar->AddControl(rotateButton);
	currentHorizontalPosition=currentHorizontalPosition+rotateButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* animateButton = new wxBitmapButton( _toolBar, tool_animate, toolBarBitmaps[tool_animate], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	animateButton->SetToolTip(_("Animate"));
	_toolBar->AddControl(animateButton);
	currentHorizontalPosition=currentHorizontalPosition+animateButton->GetSize().GetWidth()-1;


	//Contenitore raggruppante i ROI Tools Buttons
	currentHorizontalPosition=currentHorizontalPosition+35;	
	wxStaticBox* ROIToolsButtonsBox = new wxStaticBox(_toolBar, wxID_ANY, wxString("ROI Tools"), wxPoint(currentHorizontalPosition,altezzaButton-15),wxSize(210,53));

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* lengthButton = new wxBitmapButton( _toolBar, tool_length, toolBarBitmaps[tool_length], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	lengthButton->SetToolTip(_("Length"));
	_toolBar->AddControl(lengthButton);
	currentHorizontalPosition=currentHorizontalPosition+lengthButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* angleButton = new wxBitmapButton( _toolBar, tool_angle, toolBarBitmaps[tool_angle], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	angleButton->SetToolTip(_("Angle"));
	_toolBar->AddControl(angleButton);
	currentHorizontalPosition=currentHorizontalPosition+angleButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* rectangleButton = new wxBitmapButton( _toolBar, tool_rectangle, toolBarBitmaps[tool_rectangle], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	rectangleButton->SetToolTip(_("Rectangle"));
	_toolBar->AddControl(rectangleButton);
	currentHorizontalPosition=currentHorizontalPosition+rectangleButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* polygonButton = new wxBitmapButton( _toolBar, tool_polygon, toolBarBitmaps[tool_polygon], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	polygonButton->SetToolTip(_("Polygon"));
	_toolBar->AddControl(polygonButton);
	currentHorizontalPosition=currentHorizontalPosition+polygonButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
    wxBitmapButton* pencilButton = new wxBitmapButton( _toolBar, tool_pencil, toolBarBitmaps[tool_pencil], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	pencilButton->SetToolTip(_("Pencil"));
	_toolBar->AddControl(pencilButton);
	currentHorizontalPosition=currentHorizontalPosition+pencilButton->GetSize().GetWidth()-1;
	


	//Contenitore raggruppante i Segmentation Buttons
	currentHorizontalPosition=currentHorizontalPosition+35;	
	wxStaticBox* segmentationsButtonsBox = new wxStaticBox(_toolBar, wxID_ANY, wxString("Segmentation Techniques"), wxPoint(currentHorizontalPosition,altezzaButton-15),wxSize(150,53));

	currentHorizontalPosition=currentHorizontalPosition+20;
    wxBitmapButton* connectedThresholdButton = new wxBitmapButton( _toolBar, tool_connectedthreshold, toolBarBitmaps[tool_connectedthreshold], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	connectedThresholdButton->SetToolTip(_("Connected Threshold"));
	_toolBar->AddControl(connectedThresholdButton);
	currentHorizontalPosition=currentHorizontalPosition+connectedThresholdButton->GetSize().GetWidth()-1;
	
	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* neighborhoodConnectedButton = new wxBitmapButton( _toolBar, tool_neighborhoodconnected, toolBarBitmaps[tool_neighborhoodconnected], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	neighborhoodConnectedButton->SetToolTip(_("Neighborhood Connected"));
	_toolBar->AddControl(neighborhoodConnectedButton);
	currentHorizontalPosition=currentHorizontalPosition+neighborhoodConnectedButton->GetSize().GetWidth()-1;

	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* confidenceConnectedButton = new wxBitmapButton( _toolBar, tool_confidenceconnected, toolBarBitmaps[tool_confidenceconnected], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	confidenceConnectedButton->SetToolTip(_("Confidence Connected"));
	_toolBar->AddControl(confidenceConnectedButton);
	currentHorizontalPosition=currentHorizontalPosition+confidenceConnectedButton->GetSize().GetWidth()-1;



	//Contenitore raggruppante i renderingButtons
	currentHorizontalPosition=currentHorizontalPosition+45;	
	wxStaticBox* renderingButtonsBox = new wxStaticBox(_toolBar, wxID_ANY, wxString("3D Rendering Techniques"), wxPoint(currentHorizontalPosition,altezzaButton-15),wxSize(150,53));

	currentHorizontalPosition=currentHorizontalPosition+40;
	wxBitmapButton* volumeRenderingButton = new wxBitmapButton( _toolBar, tool_volumerendering, toolBarBitmaps[tool_volumerendering], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	volumeRenderingButton->SetToolTip(_("Direct Volume Rendering"));
	_toolBar->AddControl(volumeRenderingButton);
	currentHorizontalPosition=currentHorizontalPosition+volumeRenderingButton->GetSize().GetWidth()-1;
	
	currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* surfaceButton = new wxBitmapButton( _toolBar, tool_surface, toolBarBitmaps[tool_surface], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	surfaceButton->SetToolTip(_("Indirect Volume Rendering"));
	_toolBar->AddControl(surfaceButton);
	currentHorizontalPosition=currentHorizontalPosition+surfaceButton->GetSize().GetWidth()-1;

	//currentHorizontalPosition=currentHorizontalPosition+10;
	wxBitmapButton* MIPButton = new wxBitmapButton( _toolBar, tool_mip, toolBarBitmaps[tool_mip], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	MIPButton->SetToolTip(_("Maximum Intensity Projection"));
	_toolBar->AddControl(MIPButton);
	//currentHorizontalPosition=currentHorizontalPosition+MIPButton->GetSize().GetWidth()-1;
	MIPButton->Show(false);

	//currentHorizontalPosition=currentHorizontalPosition+10;
	//wxBitmapButton* MinIPButton = new wxBitmapButton( _toolBar, tool_minip, toolBarBitmaps[tool_minip], wxPoint(currentHorizontalPosition,altezzaButton), wxSize(31,31));
	//MinIPButton->SetToolTip(_("Minimum Intensity Projection"));
	//_toolBar->AddControl(MinIPButton);
	//currentHorizontalPosition=currentHorizontalPosition+MinIPButton->GetSize().GetWidth()-1;
	//MinIPButton->Show(false);
	
	(_toolBar->FindControl(tool_all))->SetBackgroundColour(*wxCYAN);

	CreateStatusBar(3);
	SetStatusText("Ready", 0);

	wxPanel* mainPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN);

    _imageBoxPanel = new wxPanel(mainPanel, id_2dWindow, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
    _imageBoxPanel->SetBackgroundColour(*wxBLACK);

	_slider = new wxSlider(mainPanel, id_2dSlider, 0, 0, 0, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_TOP);
	_autoPlay = new wxCheckBox(mainPanel,id_2dAutoPlay,_T("&Auto"),wxDefaultPosition,wxDefaultSize);    

	wxBoxSizer* mainHorWinSizer = new wxBoxSizer(wxHORIZONTAL);

	_imageBoxSizer = new wxBoxSizer(wxVERTICAL);
	_imageBoxSizer->Fit(_imageBoxPanel);
    _imageBoxSizer->SetSizeHints(_imageBoxPanel);
	_imageBoxPanel->SetAutoLayout(true);
    _imageBoxPanel->SetSizer(_imageBoxSizer);

	wxBoxSizer* sliderSizer = new wxBoxSizer(wxVERTICAL);
	sliderSizer->Add(_imageBoxPanel, 1,  wxEXPAND|wxALIGN_CENTER, 0);
	sliderSizer->Add(_slider, 0,  wxEXPAND|wxALIGN_LEFT, 0);
	sliderSizer->Add(_autoPlay, 0,  wxALIGN_RIGHT, 0);
	sliderSizer->Fit(mainPanel);
    sliderSizer->SetSizeHints(mainPanel);
	mainPanel->SetAutoLayout(true);
    mainPanel->SetSizer(sliderSizer);

	_slider->Enable(false);
	_autoPlay->Enable(false);

	wxBoxSizer* mainVerWinSizer = new wxBoxSizer(wxVERTICAL);
    mainVerWinSizer->Add(mainPanel, 1,  wxEXPAND, 0);
	mainHorWinSizer->Add(mainVerWinSizer, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(mainHorWinSizer);
	Layout();
    Centre();

	_slider->SetFocus();
}

wx2dGui::~wx2dGui() {
	_mainGui = 0;
	this->Destroy();
}

wxVtkViewer2d* wx2dGui::new2dViewer(wxMainGui* mainGui) {	
	_mainGui = mainGui;
	_viewer2d = new wxVtkViewer2d(_mainGui, _imageBoxPanel, id_2dImageBox);
	return _viewer2d;
}

void wx2dGui::show(itkVtkData *itkVtkData, bool showme){
	_imageBoxSizer->Add(_viewer2d->getWxWindow(), 1, wxEXPAND|wxALIGN_CENTER, 0);
	_viewer2d->getWxWindow()->Freeze();
	_viewer2d->showAppData(itkVtkData);
	_imageBoxSizer->Layout();
	_viewer2d->getWxWindow()->Thaw();
	if(!strcmp(itkVtkData->getName().c_str(),"Image Fusion")) {
		_menuFileExport->Enable(m_2d_fileExportDICOM,false);
		_menuFileExport->Enable(m_2d_fileExportBMP,false);
	}

	const int slicesNumber = _viewer2d->getSliceNumber(itkVtkData);
	_slider->SetRange(0,(slicesNumber-1));
	_slider->SetValue(0);
	_slider->Enable(true);
	//A.Placitelli
	_slider->SetFocus();
	_autoPlay->Enable(true);
	Maximize(true);
	Show(showme);
	_mainGui->viewTextData();
}

void wx2dGui::onActivate(wxActivateEvent& event) {
	if (_mainGui->onViewerActivate(_idViewer))
	{
		// A.Placitelli
		_slider->SetFocus();
		event.Skip();
	}
}

void wx2dGui::onFlipVertical(wxCommandEvent& event)
{
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager){
		manager->deleteAll();
		_viewer2d->removeRoiLengthAreaVolume();
		_viewer2d->removeRoiAngle();
	}
	_mainGui->onFlipVertical(event);
	if(manager){
		manager->setVtkRoi(_viewer2d->getItkVtkData()->getVtkImage());
		manager->setSliceRoi(_slider->GetValue());
		_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
	}
}

void wx2dGui::onFlipHorizontal(wxCommandEvent& event)
{
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager){
		manager->deleteAll();
		_viewer2d->removeRoiLengthAreaVolume();
		_viewer2d->removeRoiAngle();
	}
	_mainGui->onFlipHorizontal(event);
	if(manager){
		manager->setVtkRoi(_viewer2d->getItkVtkData()->getVtkImage());
		manager->setSliceRoi(_slider->GetValue());
		_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
	}
}

void wx2dGui::onSliderUpdate(wxCommandEvent& event) {
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager){
		if(manager->getNumPoints()>0){
			manager->setSliceRoi(_slider->GetValue());
			manager->selectAll();
			_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
		}
		else{
			manager->setSliceRoi(_slider->GetValue());
			_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
		}	
	}
	_viewer2d->setSlice(_slider->GetValue());	
	_mainGui->viewTextData();
}

void wx2dGui::OnMouseWheel( wxMouseEvent &event)
{
	_slider->SetFocus();
}

void wx2dGui::onAnonymize(wxCommandEvent& event) {  
	unsigned int idActiveViewer = _mainGui->getActiveViewer();
	if (idActiveViewer) {
		unsigned int idData = _mainGui->getViewer(idActiveViewer)->getActiveIdSingleData();
		if (idData) {
			itkVtkData *_itkVtkData = (itkVtkData*)_mainGui->getDataHandler()->getData(idData);

			wxAnonymizeDialog *dlg = new wxAnonymizeDialog(this, ID_DIALOG, _("Anonymize"));

			dlg->setRgb(_itkVtkData->getRgb());

			string path = _itkVtkData->getName();
			wxFileName wxFileName_((wxString)path.c_str(), wxPATH_NATIVE);
			//if(wxFileName_.DirExists())
			if(!wxFileName_.FileExists() && wxFileName_.DirExists())
				dlg->setSeries(true);
			else dlg->setSeries(false);
			dlg->setInputPath(path.c_str());
			dlg->ShowModal();
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}

void wx2dGui::onExportToDicom(wxCommandEvent& event) {  
	unsigned int idActiveViewer = _mainGui->getActiveViewer();
	if (idActiveViewer) {
		unsigned int idData = _mainGui->getViewer(idActiveViewer)->getActiveIdSingleData();
		if (idData) {
			itkVtkData *_itkVtkData = (itkVtkData*)_mainGui->getDataHandler()->getData(idData);

			string inputPath = _itkVtkData->getName();
			string outputPath = "";

			exportFilter _exportFilter(_itkVtkData->getRgb());

			wxFileName wxFileName_((wxString)inputPath.c_str(), wxPATH_NATIVE);
			//if(wxFileName_.DirExists()) {
			if(!wxFileName_.FileExists() && wxFileName_.DirExists()) {
				wxDirDialog *dlgDir = new wxDirDialog(this, "Save series", "", wxSAVE|wxDD_NEW_DIR_BUTTON, wxDefaultPosition);
				if (dlgDir->ShowModal() == wxID_OK) {
					outputPath = dlgDir->GetPath();

					wxWindowDisabler disabler;
					wxBusyInfo wait("Please wait, saving...");
					wxBusyCursor cursor;

					_exportFilter.exportSeriesToDicom(inputPath, outputPath);

					this->Raise();
				}
				dlgDir->Destroy();
			}
			else {
				wxFileDialog *dlgFile = new wxFileDialog(this, "Save image", "", "", "DICOM Files(*.dcm;*.dicom)|*.dcm;*.dicom",  wxSAVE|wxOVERWRITE_PROMPT, wxDefaultPosition);
				if (dlgFile->ShowModal() == wxID_OK) {
					outputPath = dlgFile->GetPath();

					wxWindowDisabler disabler;
					wxBusyInfo wait("Please wait, saving...");
					wxBusyCursor cursor;

					_exportFilter.exportImageToDicom(_itkVtkData, outputPath);

					this->Raise();
				}
				dlgFile->Destroy();
			}
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}


void wx2dGui::onExportToBmp(wxCommandEvent& event) {  
	unsigned int idActiveViewer = _mainGui->getActiveViewer();
	if (idActiveViewer) {
		unsigned int idData = _mainGui->getViewer(idActiveViewer)->getActiveIdSingleData();
		if (idData) {
			itkVtkData *_itkVtkData = (itkVtkData*)_mainGui->getDataHandler()->getData(idData);

			string inputPath = _itkVtkData->getName();
			string outputPath = "";

			exportFilter _exportFilter(_itkVtkData->getRgb());

			wxFileName wxFileName_((wxString)inputPath.c_str(), wxPATH_NATIVE);
			//if(wxFileName_.DirExists()) {
			if(!wxFileName_.FileExists() && wxFileName_.DirExists()) {
				wxDirDialog *dlgDir = new wxDirDialog(this, "Save series", "", wxSAVE|wxDD_NEW_DIR_BUTTON, wxDefaultPosition);
				if (dlgDir->ShowModal() == wxID_OK) {
					outputPath = dlgDir->GetPath();

					wxWindowDisabler disabler;
					wxBusyInfo wait("Please wait, saving...");
					wxBusyCursor cursor;

					_exportFilter.exportSeriesToBmp(inputPath, outputPath);

					this->Raise();
				}
				dlgDir->Destroy();
			}
			else {
				wxFileDialog *dlgFile = new wxFileDialog(this, "Save image", "", "", "BMP Files(*.bmp)|*.bmp",  wxSAVE|wxOVERWRITE_PROMPT, wxDefaultPosition);
				if (dlgFile->ShowModal() == wxID_OK) {
					outputPath = dlgFile->GetPath();

					wxWindowDisabler disabler;
					wxBusyInfo wait("Please wait, saving...");
					wxBusyCursor cursor;

					_exportFilter.exportImageToBmp(inputPath, outputPath);

					this->Raise();
				}
				dlgFile->Destroy();
			}
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}


void wx2dGui::onViewHeader(wxCommandEvent& event) {
	unsigned int idViewer = _mainGui->getActiveViewer();
	if (idViewer) {
		_mainGui->viewHeaderDicom(this);
	}	
}

void wx2dGui::onSelectRoi(wxCommandEvent& event) {
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager){
		if(manager->getNumPoints()>0){
			manager->selectAll();
			_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			_viewer2d->updateViewer();
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) defined on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();	
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) defined on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wx2dGui::onDeleteRoi(wxCommandEvent& event) {
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if (manager){
		if(manager->getNumPoints()>0){
			manager->deleteAll();
			_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			_viewer2d->removeRoiLengthAreaVolume();
			_viewer2d->removeRoiAngle();
			_viewer2d->updateViewer();
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) defined on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();	
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) defined on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wx2dGui::onExtractRoiOutside(wxCommandEvent& event) {
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if (manager){
		if(manager->getNumPoints()>0){
			
			wxWindowDisabler disabler;
			wxBusyInfo wait("Please wait, extracting...");
			wxBusyCursor cursor;

			_viewer2d->removeRoiLengthAreaVolume();
			_viewer2d->removeRoiAngle();
			manager->set_typeExtraction(1);
			manager->setFlipState(_viewer2d->getVerFlip(),_viewer2d->getHorFlip());
			manager->extraction();
			_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			_viewer2d->updateViewer();
			this->onAll((wxCommandEvent)NULL);
			this->Raise();
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) selected on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();	
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) selected on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wx2dGui::onExtractRoiInside(wxCommandEvent& event) {
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if (manager){
		if(manager->getNumPoints()>0){
			
			wxWindowDisabler disabler;
			wxBusyInfo wait("Please wait, extracting...");
			wxBusyCursor cursor;
			
			_viewer2d->removeRoiLengthAreaVolume();
			_viewer2d->removeRoiAngle();
			manager->set_typeExtraction(0);
			manager->setFlipState(_viewer2d->getVerFlip(),_viewer2d->getHorFlip());
			manager->extraction();
			_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			_viewer2d->updateViewer();
			this->onAll((wxCommandEvent)NULL);
			this->Raise();
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) selected on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();	
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No Roi(s) selected on the image(s)."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}


void wx2dGui::onModifyWLWW(wxCommandEvent& event) {
	double wl, ww;
	wxWLWWDialog* dlg = new wxWLWWDialog(this);
	if (dlg->ShowModal() == wxID_OK) {
		wl = dlg->getWL();
		ww = dlg->getWW();
		dlg->Destroy();

		wxWindowDisabler disabler;
		wxBusyInfo wait("Please wait, modifying WL/WW...");
		wxBusyCursor cursor;
		
		_viewer2d->SetColorLevel(wl);
		_viewer2d->SetColorWindow(ww);
		_viewer2d->getItkVtkData()->setWl(wl);
		_viewer2d->getItkVtkData()->setWw(ww);
	}
	else {
		dlg->Destroy();
		return;
	}
}

void wx2dGui::onAll(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_all))->GetBackgroundColour());
	(_toolBar->FindControl(tool_all))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_all;
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(all2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onWindowLevel(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_windowlevel))->GetBackgroundColour());
	(_toolBar->FindControl(tool_windowlevel))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_windowlevel;
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(windowLevel2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onMove(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_move))->GetBackgroundColour());
	(_toolBar->FindControl(tool_move))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_move;
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(move2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onZoom(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_zoom))->GetBackgroundColour());
	(_toolBar->FindControl(tool_zoom))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_zoom;
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(zoom2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onRotate(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_rotate))->GetBackgroundColour());
	(_toolBar->FindControl(tool_rotate))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_rotate;
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(rotate2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onAnimate(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_animate))->GetBackgroundColour());
	(_toolBar->FindControl(tool_animate))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_animate;
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(animate2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onLength(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_length))->GetBackgroundColour());
	(_toolBar->FindControl(tool_length))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_length;
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager)
		manager->deleteAll();
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(length2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onAngle(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_angle))->GetBackgroundColour());
	(_toolBar->FindControl(tool_angle))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_angle;
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager)
		manager->deleteAll();
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(angle2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onPolygon(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_polygon))->GetBackgroundColour());
	(_toolBar->FindControl(tool_polygon))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_polygon;
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager)
		manager->deleteAll();
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(polygon2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onRectangle(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_rectangle))->GetBackgroundColour());
	(_toolBar->FindControl(tool_rectangle))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_rectangle;
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager)
		manager->deleteAll();
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(rectangle2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onPencil(wxCommandEvent& event) {
	(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_pencil))->GetBackgroundColour());
	(_toolBar->FindControl(tool_pencil))->SetBackgroundColour(*wxCYAN);
	_selectedButton = tool_pencil;
	roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
	if(manager)
		manager->deleteAll();
	((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(pencil2d);
	_viewer2d->removeRoiLengthAreaVolume();
	_viewer2d->removeRoiAngle();
	_viewer2d->updateViewer();
}

void wx2dGui::onAutoPlay( wxCommandEvent &event ) {	
	wxWindowDisabler disabler;
	if(_slider->GetMin()!=_slider->GetMax()){
		if (_autoPlay->GetValue()){
			if(!_working){
				wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED , id_2dPlay);
				wxPostEvent(this, event );
			}
			else
				_autoPlay->SetValue(false);
	    }
		else {
			if(_working){
				wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED , id_2dStop);
				wxPostEvent(this, event );
			}
			else
				_autoPlay->SetValue(true);
		}
	}
	else
		_autoPlay->SetValue(false);
}

void wx2dGui::onPlay( wxCommandEvent &event )
{	
	if(!_working){
		_working = true;

		_slider->Enable(true);	
		for (int i=0;i<GetMenuBar()->GetMenuCount();i++)
			GetMenuBar()->EnableTop(i, false);
		_preview = false;
		int index;
		roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
		
		for( index=_slider->GetValue()+1; index<=_slider->GetMax();index++){
			if(manager){
				if(manager->getNumPoints()>0){
					manager->setSliceRoi(index);
					manager->selectAll();
					_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());
				}
				else{
					manager->setSliceRoi(index);
					_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
				}	
			}
			_viewer2d->SetSlice(index);
			
			_slider->SetValue(index);
			_mainGui->viewTextData();
			wxMilliSleep(500);
			wxYield();
			if (_preview)
				break;	
		}
		if(index>_slider->GetMax()){
			if(manager){
				if(manager->getNumPoints()>0){
					manager->setSliceRoi(0);
					manager->selectAll();
					_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
				}
				else{
					manager->setSliceRoi(0);
					_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
				}	
			}
			_viewer2d->SetSlice(0);
			_slider->SetValue(0);
			_mainGui->viewTextData();
		}
		_working = false;
		_autoPlay->SetValue(false);
		_slider->Enable(true);	
		for (int i=0;i<GetMenuBar()->GetMenuCount();i++)
			GetMenuBar()->EnableTop(i, true);
		}
}

void wx2dGui::onStop( wxCommandEvent &event )
{	
	if(_working)
		_preview=true;
}

void wx2dGui::onNextSlice(wxCommandEvent &event)
{ 
	if (_viewer2d->getSliceNumber(_viewer2d->getItkVtkData())>1){
		int zSlice = _slider->GetValue()+1;
		if (zSlice > _slider->GetMax())
				zSlice = _slider->GetMin();
		roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
		if(manager){
			if(manager->getNumPoints()>0){
				manager->setSliceRoi(zSlice);
				manager->selectAll();
				_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			}
			else{
				manager->setSliceRoi(zSlice);
				_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			}	
		}
		_viewer2d->SetSlice(zSlice);
		_slider->SetValue(zSlice);
		_mainGui->viewTextData();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No series is currently opened."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wx2dGui::onPreviousSlice(wxCommandEvent &event)
{ 
	if (_viewer2d->getSliceNumber(_viewer2d->getItkVtkData())>1){

		int zSlice = _slider->GetValue()-1;
		if (zSlice < _slider->GetMin())
				zSlice = _slider->GetMax();
		roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
		if(manager){
			if(manager->getNumPoints()>0){
				manager->setSliceRoi(zSlice);
				manager->selectAll();
				_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			}
			else{
				manager->setSliceRoi(zSlice);
				_viewer2d->getItkVtkData()->setVtkImage(manager->getVtkRoi());	
			}	

		}
		_viewer2d->SetSlice(zSlice);
		_slider->SetValue(zSlice);
		_mainGui->viewTextData();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No series is currently opened."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wx2dGui::onResetView(wxCommandEvent &event)
{
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading...");
	wxBusyCursor cursor;

	unsigned int idData = -1;
	list<unsigned int> *l = _mainGui->getDataHandler()->getIdDataByIdViewer(_idViewer);
	
	list<unsigned int>::iterator iteList = l->begin();
	while (iteList != l->end()) {
		idData = *iteList;
		iteList++;
	}
	delete l;
	_mainGui->getDataHandler()->getDataItem(idData)->idViewer = 0;
	_imageBoxSizer->Detach(_viewer2d->getWxWindow());
	_mainGui->quitViewer(_idViewer);

	_viewer2d = new wxVtkViewer2d(_mainGui, _imageBoxPanel, id_2dImageBox);
	_mainGui->new2dViewer(_viewer2d);
	_idViewer = _mainGui->getActiveViewer();

	_mainGui->getDataHandler()->getDataItem(idData)->idViewer = _idViewer;
		
	_imageBoxSizer->Add(_viewer2d->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
	
	_viewer2d->getWxWindow()->Freeze();
	itkVtkData *itkVtkData_ = (itkVtkData*)(_mainGui->getDataHandler()->getData(idData));

	string fileName = itkVtkData_->getName();
	_mainGui->getDataHandler()->deleteData(idData);
	idData = _mainGui->getDataHandler()->newData(_mainGui->getViewer(_idViewer)->newAppData(), _idViewer);
	itkVtkData_ = (itkVtkData*)(_mainGui->getDataHandler()->getData(idData));		
	itkVtkData_->open(fileName,true);
	itkVtkData_->setName(fileName);

	//itkVtkData_->resetVtkData();
/*

	string fileName = itkVtkData_->getName();
	_imageBoxSizer->Detach(((wxVtkViewer2d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()))->getWxWindow());
	unsigned int oldIdViewer = _idViewer;
	_idViewer = _mainGui->getActiveViewer();
	idData = _mainGui->getDataHandler()->newData(_mainGui->getViewer(_idViewer)->newAppData(), _idViewer);

	_mainGui->onViewerQuit(oldIdViewer);
	_mainGui->getDataHandler()->getData(idData)->setName(fileName);
	_imageBoxSizer->Add(((wxVtkViewer2d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()))->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);

	((wxVtkViewer2d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()))->getWxWindow()->Freeze();
	_mainGui->getViewer(_idViewer)->showAppData(_mainGui->getDataHandler()->getData(idData));
	_imageBoxSizer->Layout();
	((wxVtkViewer2d*)(_mainGui->getViewer(_idViewer)->getViewerDrawing()))->getWxWindow()->Thaw();
	itkVtkData_ = (itkVtkData*)(_mainGui->getDataHandler()->getData(idData));		
*/

	_viewer2d->showAppData(itkVtkData_);
	_imageBoxSizer->Layout();
	_viewer2d->getWxWindow()->Thaw();
	_slider->SetValue(0);
	_mainGui->viewTextData();
	this->onAll((wxCommandEvent)NULL);
	this->Raise();
}


void wx2dGui::onVolumeRendering(wxCommandEvent& event) {
	if(_viewer2d->getItkVtkData()->getSliceNumber() > 1) {
		unsigned int idActiveViewer;

		itkVtkData *_itkVtkData = new itkVtkData();

		if(!_viewer2d->getItkVtkData()->getRgb()) {
			try {
				Duplicator::Pointer duplicator = Duplicator::New();
				duplicator->SetInputImage(_viewer2d->getItkVtkData()->getItkImage());
				duplicator->SetMetaDataDictionary(_viewer2d->getItkVtkData()->getItkImage()->GetMetaDataDictionary());
				duplicator->Update();
				_itkVtkData->setRgb(false);
				_itkVtkData->setItkImage(duplicator->GetOutput());
				_itkVtkData->resetVtkData();
				_itkVtkData->setItkImage(NULL);
				_itkVtkData->setImageViewPlane(_viewer2d->getItkVtkData()->getImageViewPlane());
				_itkVtkData->setWl(_viewer2d->getItkVtkData()->getWl());
				_itkVtkData->setWw(_viewer2d->getItkVtkData()->getWw());
			}
			catch(itk::ExceptionObject & e) {
				delete _itkVtkData;
				wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
				return;
			}
		}
		else {
			try {
				RGBDuplicator::Pointer duplicator = RGBDuplicator::New();
				duplicator->SetInputImage(_viewer2d->getItkVtkData()->getItkRgbImage());
				duplicator->SetMetaDataDictionary(_viewer2d->getItkVtkData()->getItkRgbImage()->GetMetaDataDictionary());
				duplicator->Update();
				_itkVtkData->setRgb(true);
				_itkVtkData->setItkRgbImage(duplicator->GetOutput());
				_itkVtkData->resetVtkData();
				_itkVtkData->setItkRgbImage(NULL);
				_itkVtkData->setImageViewPlane(_viewer2d->getItkVtkData()->getImageViewPlane());
				_itkVtkData->setWl(_viewer2d->getItkVtkData()->getWl());
				_itkVtkData->setWw(_viewer2d->getItkVtkData()->getWw());
			}
			catch(itk::ExceptionObject & e) {
				delete _itkVtkData;
				wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
				return;
			}
		}

		wxVolumeRenderingGui *g3d = new wxVolumeRenderingGui(_T("MITO: "), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());
		_mainGui->new3dViewer(g3d->new3dViewer(_mainGui));
		g3d->setMinPixel(_viewer2d->getItkVtkData()->getMinPixelValue());
		g3d->setMaxPixel(_viewer2d->getItkVtkData()->getMaxPixelValue());

		idActiveViewer = _mainGui->getActiveViewer();
		unsigned int newIdData = _mainGui->getDataHandler()->newData(_itkVtkData, idActiveViewer, false);
		
		long wl = (long)_itkVtkData->getWl();
		long ww = (long)_itkVtkData->getWw();
		int clut = 0;

		volumeRenderingFilter volumeRendering(newIdData, _mainGui->getDataHandler());
		
		// controlla se il 3D texture mapping è supportato dalla scheda grafica
		bool is3DtextureSupported = volumeRendering.is3DtextureSupported();
		g3d->set3DtextureSupported(is3DtextureSupported);
		
		
		/*
		//Operazioni richieste per visualizzare inizialmente in modalità VolumeRendering
		if(!_viewer2d->getItkVtkData()->getRgb())
			_itkVtkData->setVtkVolume(volumeRendering.compute(wl,ww,clut,_viewer2d->getItkVtkData()->getMinPixelValue()));
		else
			_itkVtkData->setVtkVolume(volumeRendering.computeRgb(wl,ww,clut));		
		g3d->SetTitle(_T("MITO: 3D Volume Rendering"));
		g3d->setRenderingTechnique(VolumeRendering);
		*/
		
		
		
		//Operazioni richieste per visualizzare inizialmente in modalità TextureRendering		
		if (!_viewer2d->getItkVtkData()->getRgb()) {
			if (is3DtextureSupported) {	// se il 3D texture mapping è supportato lo usa, altrimenti usa il 2D texture mapping
				_itkVtkData->setVtkVolume(volumeRendering.computeTextures3D(wl,ww,clut,_viewer2d->getItkVtkData()->getMinPixelValue(),0.5));
				g3d->SetTitle(_T("MITO: 3D Texture Rendering"));
			}		
			else {	// se non è possibile usare texture 3D vengono usate le 2D
				_itkVtkData->setVtkVolume(volumeRendering.computeTextures2D(wl,ww,clut,_viewer2d->getItkVtkData()->getMinPixelValue()));
				g3d->SetTitle(_T("MITO: 2D Texture Rendering"));
			}
			g3d->setRenderingTechnique(VolumeTextureRendering);
		}
		else {
			_itkVtkData->setVtkVolume(volumeRendering.computeRgb(wl,ww,clut));
			g3d->SetTitle(_T("MITO: 3D Volume Fixed Point Rendering"));
			g3d->setRenderingTechnique(VolumeFixedPointRendering);
		}	
		

		// richiede la distanza dell'osservatore dallo schermo
		/*const wxChar* PODChar;

		do {
			char message[60];
			strcpy(message,"Set the distance from the screen in cm");
			wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, "3D Volume Rendering", "200");
			if (dlg->ShowModal() == wxID_OK) {
				PODChar = dlg->GetValue().c_str();
				dlg->Destroy();
			}
			else {
				dlg->Destroy();
				return;
			}
		} while(atoi(PODChar) < 10);*/

		g3d->setCLUT(clut);		
		g3d->setIdViewer(idActiveViewer);
		g3d->setIdData(newIdData);
		g3d->setWlWw(wl,ww);

		//g3d->setPOD(atoi(PODChar));

		g3d->show();
		
		g3d->setInitialDistance();
		g3d->setInitialProjectionValues();

		// aggiunta per prova WII
		//g3d->onWii(event);

		_mainGui->viewTextData();

		this->Close();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Volume Rendering works only with series."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}


void wx2dGui::onSurfaceRendering(wxCommandEvent& event) {
	if(_viewer2d->getItkVtkData()->getSliceNumber() > 1) {

		itkVtkData* _itkVtkData = new itkVtkData();
		try {
			if(_viewer2d->getItkVtkData()->getRgb()) _viewer2d->getItkVtkData()->convertRgbToGrayVtkImage(false);
			Duplicator::Pointer duplicator = Duplicator::New();
			duplicator->SetInputImage(_viewer2d->getItkVtkData()->getItkImage());
			duplicator->SetMetaDataDictionary(_viewer2d->getItkVtkData()->getItkImage()->GetMetaDataDictionary());
			duplicator->Update();
			_itkVtkData->setItkImage(duplicator->GetOutput());
			_itkVtkData->resetVtkData();
			_itkVtkData->setItkImage(NULL);
			_itkVtkData->setImageViewPlane(_viewer2d->getItkVtkData()->getImageViewPlane());
			_itkVtkData->setRgb(false);
			if(_viewer2d->getItkVtkData()->getRgb()) _viewer2d->getItkVtkData()->convertGrayToRgbVtkImage(true);
		}
		catch(itk::ExceptionObject & e) {
			delete _itkVtkData;
			wxMessageBox(_T("Study too large for your hardware! Try with a smaller data set!!"));
			return;
		}

		unsigned int idActiveViewer;
		const wxChar* contourChar;

		itkVtkData *_data = (itkVtkData*)_viewer2d->getItkVtkData();
		do {
			char min[10], max[10];

			itoa(_data->getMinPixelValue(),min,10);
			itoa(_data->getMaxPixelValue(),max,10);
			char message[60];
			strcpy(message,"Set a contour value between ");
			strcat(message,min);
			strcat(message," and "),
			strcat(message,max);
			strcat(message,".");
			wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, "3D Surface Rendering", "900");
			if (dlg->ShowModal() == wxID_OK) {
				contourChar = dlg->GetValue().c_str();
				dlg->Destroy();
			}
			else {
				dlg->Destroy();

				return;
			}
			if(atoi(contourChar) < _data->getMinPixelValue() || atoi(contourChar) > _data->getMaxPixelValue()) {
				wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Contour value wrong! Set another value."), _T("MITO"), wxOK | wxICON_INFORMATION);
				if (messDialog->ShowModal()==wxID_OK)
					messDialog->Destroy();
			}
		} while(atoi(contourChar) < _data->getMinPixelValue() || atoi(contourChar) > _data->getMaxPixelValue());


		wxSurfaceRenderingGui *g3d = new wxSurfaceRenderingGui(_T("MITO: 3D Surface Rendering"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());		
		_mainGui->new3dViewer(g3d->new3dViewer(_mainGui));

		idActiveViewer = _mainGui->getActiveViewer();
		unsigned int newIdData = _mainGui->getDataHandler()->newData(_itkVtkData, idActiveViewer, false);

		surfaceRenderingFilter f(newIdData, _mainGui->getDataHandler());
		_itkVtkData->setVtkActor(f.compute(atoi(contourChar)));

		g3d->setIdViewer(idActiveViewer);
		g3d->setIdData(newIdData);
		g3d->show();
		g3d->setInitialDistance();
		g3d->setInitialProjectionValues();
		
		g3d->onRotateAround(event);

		this->Close();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Surface Rendering works only with series."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}


void wx2dGui::onMaximumIntensityProjection(wxCommandEvent& event) {
	if(_viewer2d->getItkVtkData()->getSliceNumber() > 1) {
		unsigned int idActiveViewer;

		wxVolumeRenderingGui *g3d = new wxVolumeRenderingGui(_T("MITO: 3D MIP"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());
		_mainGui->new3dViewer(g3d->new3dViewer(_mainGui));
		g3d->setMinPixel(_viewer2d->getItkVtkData()->getMinPixelValue());
		g3d->setMaxPixel(_viewer2d->getItkVtkData()->getMaxPixelValue());
	
		itkVtkData *_itkVtkData = new itkVtkData();

		if(!_viewer2d->getItkVtkData()->getRgb()) {
			Duplicator::Pointer duplicator = Duplicator::New();
			duplicator->SetInputImage(_viewer2d->getItkVtkData()->getItkImage());
			duplicator->SetMetaDataDictionary(_viewer2d->getItkVtkData()->getItkImage()->GetMetaDataDictionary());
			duplicator->Update();
			_itkVtkData->setRgb(false);
			_itkVtkData->setItkImage(duplicator->GetOutput());
			_itkVtkData->resetVtkData();
			_itkVtkData->setItkImage(NULL);
			_itkVtkData->setImageViewPlane(_viewer2d->getItkVtkData()->getImageViewPlane());
			_itkVtkData->setWl(_viewer2d->getItkVtkData()->getWl());
			_itkVtkData->setWw(_viewer2d->getItkVtkData()->getWw());
		}
		else {
			RGBDuplicator::Pointer duplicator = RGBDuplicator::New();
			duplicator->SetInputImage(_viewer2d->getItkVtkData()->getItkRgbImage());
			duplicator->SetMetaDataDictionary(_viewer2d->getItkVtkData()->getItkRgbImage()->GetMetaDataDictionary());
			duplicator->Update();
			_itkVtkData->setRgb(true);
			_itkVtkData->setItkRgbImage(duplicator->GetOutput());
			_itkVtkData->resetVtkData();
			_itkVtkData->setItkRgbImage(NULL);
			_itkVtkData->setImageViewPlane(_viewer2d->getItkVtkData()->getImageViewPlane());
			_itkVtkData->setWl(_viewer2d->getItkVtkData()->getWl());
			_itkVtkData->setWw(_viewer2d->getItkVtkData()->getWw());
		}

		idActiveViewer = _mainGui->getActiveViewer();
		unsigned int newIdData = _mainGui->getDataHandler()->newData(_itkVtkData, idActiveViewer, false);

		long wl = (long)_itkVtkData->getWl();
		long ww = (long)_itkVtkData->getWw();
		int clut = 0;

		mipFilter mip(newIdData, _mainGui->getDataHandler());
		if(!_viewer2d->getItkVtkData()->getRgb())
			_itkVtkData->setVtkVolume(mip.compute(wl, ww, clut, g3d->getMinPixel(), true));
		else
			_itkVtkData->setVtkVolume(mip.computeRgb(wl, ww, clut, true));

		// richiede la distanza dell'osservatore dallo schermo
		/*const wxChar* PODChar;

		do {
			char message[60];
			strcpy(message,"Set the distance from the screen in cm");
			wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, "3D Volume Rendering", "200");
			if (dlg->ShowModal() == wxID_OK) {
				PODChar = dlg->GetValue().c_str();
				dlg->Destroy();
			}
			else {
				dlg->Destroy();
				return;
			}
		} while(atoi(PODChar) < 10);*/

		g3d->setCLUT(clut);
		g3d->setRenderingTechnique(MIP);
		g3d->setIdViewer(idActiveViewer);
		g3d->setIdData(newIdData);
		g3d->setWlWw(wl,ww);

		//g3d->setPOD(atoi(PODChar));

		g3d->show();
		
		g3d->setInitialDistance();
		g3d->setInitialProjectionValues();

		_mainGui->viewTextData();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("MIP works only with series."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}


 /*
 void wx2dGui::onMinimumIntensityProjection(wxCommandEvent& event) {
	if(_viewer2d->getItkVtkData()->getSliceNumber() > 1) {
		unsigned int idActiveViewer;
		
		wxVolumeRenderingGui *g3d = new wxVolumeRenderingGui(_T("MITO: 3D MinIP"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());
		_mainGui->new3dViewer(g3d->new3dViewer(_mainGui));
		g3d->setMinPixel(_viewer2d->getItkVtkData()->getMinPixelValue());
		g3d->setMaxPixel(_viewer2d->getItkVtkData()->getMaxPixelValue());
	
		itkVtkData *_itkVtkData = new itkVtkData();

		if(!_viewer2d->getItkVtkData()->getRgb()) {
			Duplicator::Pointer duplicator = Duplicator::New();
			duplicator->SetInputImage(_viewer2d->getItkVtkData()->getItkImage());
			duplicator->SetMetaDataDictionary(_viewer2d->getItkVtkData()->getItkImage()->GetMetaDataDictionary());
			duplicator->Update();
			_itkVtkData->setRgb(false);
			_itkVtkData->setItkImage(duplicator->GetOutput());
			_itkVtkData->resetVtkData();
			_itkVtkData->setItkImage(NULL);
			_itkVtkData->setImageViewPlane(_viewer2d->getItkVtkData()->getImageViewPlane());
			_itkVtkData->setWl(_viewer2d->getItkVtkData()->getWl());
			_itkVtkData->setWw(_viewer2d->getItkVtkData()->getWw());
		}
		else {
			RGBDuplicator::Pointer duplicator = RGBDuplicator::New();
			duplicator->SetInputImage(_viewer2d->getItkVtkData()->getItkRgbImage());
			duplicator->SetMetaDataDictionary(_viewer2d->getItkVtkData()->getItkRgbImage()->GetMetaDataDictionary());
			duplicator->Update();
			_itkVtkData->setRgb(true);
			_itkVtkData->setItkRgbImage(duplicator->GetOutput());
			_itkVtkData->resetVtkData();
			_itkVtkData->setItkRgbImage(NULL);
			_itkVtkData->setImageViewPlane(_viewer2d->getItkVtkData()->getImageViewPlane());
			_itkVtkData->setWl(_viewer2d->getItkVtkData()->getWl());
			_itkVtkData->setWw(_viewer2d->getItkVtkData()->getWw());
		}

		idActiveViewer = _mainGui->getActiveViewer();
		unsigned int newIdData = _mainGui->getDataHandler()->newData(_itkVtkData, idActiveViewer, false);

		long wl = (long)_itkVtkData->getWl();
		long ww = (long)_itkVtkData->getWw();
		int clut = 0;

		minipFilter minip(newIdData, _mainGui->getDataHandler());
		if(!_viewer2d->getItkVtkData()->getRgb())
			_itkVtkData->setVtkVolume(minip.compute(wl, ww, clut, g3d->getMinPixel(), true));
		else
			_itkVtkData->setVtkVolume(minip.computeRgb(wl, ww, clut, true));

		// richiede la distanza dell'osservatore dallo schermo
		/*const wxChar* PODChar;

		do {
			char message[60];
			strcpy(message,"Set the distance from the screen in cm");
			wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, "3D Volume Rendering", "200");
			if (dlg->ShowModal() == wxID_OK) {
				PODChar = dlg->GetValue().c_str();
				dlg->Destroy();
			}
			else {
				dlg->Destroy();
				return;
			}
		} while(atoi(PODChar) < 10);

		g3d->setCLUT(clut);
		g3d->setRenderingTechnique(MinIP);
		g3d->setIdViewer(idActiveViewer);
		g3d->setIdData(newIdData);
		g3d->setWlWw(wl,ww);

		//g3d->setPOD(atoi(PODChar));

		g3d->show();
		
		g3d->setInitialDistance();
		g3d->setInitialProjectionValues();

		_mainGui->viewTextData();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("MinIP works only with series."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}
*/

void wx2dGui::onConnectedThreshold(wxCommandEvent& event) {
	unsigned int idData = _mainGui->getViewer(_idViewer)->getActiveIdSingleData();
	if (idData) {
		if(!((itkVtkData*)(_mainGui->getDataHandler()->getData(idData)))->getRgb()) {
			(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_connectedthreshold))->GetBackgroundColour());
			(_toolBar->FindControl(tool_connectedthreshold))->SetBackgroundColour(*wxCYAN);
			_selectedButton = tool_connectedthreshold;
			((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(connectedThreshold2d);
			roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
			if(manager)
				manager->deleteAll();
			_viewer2d->removeRoiLengthAreaVolume();
			_viewer2d->removeRoiAngle();
			_viewer2d->updateViewer();
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Click the starting point in the image."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy(); 
			segmentationManager* segmentationMngr = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getSegmentationManager();
			if(segmentationMngr==NULL){
				segmentationMngr = new segmentationManager(_mainGui);
				((appWxVtkInteractor*)_viewer2d->getWxWindow())->setSegmentationManager(segmentationMngr);
			}
			segmentationMngr->setFlipState(_viewer2d->getVerFlip(), _viewer2d->getHorFlip());
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Connected Threshold doesn't work with RGB images."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();
		}
	}

}

void wx2dGui::onNeighborhoodConnected(wxCommandEvent& event) {
	unsigned int idData = _mainGui->getViewer(_idViewer)->getActiveIdSingleData();
	if (idData) {
		if(!((itkVtkData*)(_mainGui->getDataHandler()->getData(idData)))->getRgb()) {
			(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_neighborhoodconnected))->GetBackgroundColour());
			(_toolBar->FindControl(tool_neighborhoodconnected))->SetBackgroundColour(*wxCYAN);
			_selectedButton = tool_neighborhoodconnected;
			((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(neighborhoodConnected2d);
			roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
			if(manager)
				manager->deleteAll();
			_viewer2d->removeRoiLengthAreaVolume();
			_viewer2d->removeRoiAngle();
			_viewer2d->updateViewer();
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Click the starting point in the image."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();
			segmentationManager* segmentationMngr = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getSegmentationManager();
			if(segmentationMngr==NULL){
				segmentationMngr = new segmentationManager(_mainGui);
				((appWxVtkInteractor*)_viewer2d->getWxWindow())->setSegmentationManager(segmentationMngr);
			}
			segmentationMngr->setFlipState(_viewer2d->getVerFlip(), _viewer2d->getHorFlip());
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Neighborhood Connected doesn't work with RGB images."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();
		}
	}
}

void wx2dGui::onConfidenceConnected(wxCommandEvent& event) {
	unsigned int idData = _mainGui->getViewer(_idViewer)->getActiveIdSingleData();
	if (idData) {
		if(!((itkVtkData*)(_mainGui->getDataHandler()->getData(idData)))->getRgb()) {
			(_toolBar->FindControl(_selectedButton))->SetBackgroundColour((_toolBar->FindControl(tool_confidenceconnected))->GetBackgroundColour());
			(_toolBar->FindControl(tool_confidenceconnected))->SetBackgroundColour(*wxCYAN);
			_selectedButton = tool_confidenceconnected;
			((appWxVtkInteractor*)_viewer2d->getWxWindow())->setInteractionType(confidenceConnected2d);
			roiManager* manager = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getRoiManager();
			if(manager)
				manager->deleteAll();
			_viewer2d->removeRoiLengthAreaVolume();
			_viewer2d->removeRoiAngle();
			_viewer2d->updateViewer();
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Click the starting point in the image."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();	
			segmentationManager* segmentationMngr = ((appWxVtkInteractor*)_viewer2d->getWxWindow())->getSegmentationManager();
			if(segmentationMngr==NULL){
				segmentationMngr = new segmentationManager(_mainGui);
				((appWxVtkInteractor*)_viewer2d->getWxWindow())->setSegmentationManager(segmentationMngr);
			}
			segmentationMngr->setFlipState(_viewer2d->getVerFlip(), _viewer2d->getHorFlip());
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Confidence Connected doesn't work with RGB images."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();
		}
	}
}


void wx2dGui::onAlbum(wxCommandEvent &event){
	for (int i=1; i<=(_mainGui->getViewerNumber()); i++){
		if((_mainGui->getViewer(i))){
			wxWindow* window = ((wxVtkViewer2d*)(_mainGui->getViewer(i)->getViewerDrawing()))->getWxWindow();
			while(!(window->IsTopLevel())){
				if(window->GetParent())
					window = window->GetParent(); 			
			}
			if(window!=_mainGui && window!=this)
				window->Close(TRUE);
		}
	}
	this->Close(TRUE);
}


void wx2dGui::onClose(wxCommandEvent& event) {  
	Close(TRUE);
}


void wx2dGui::onQuit(wxCloseEvent& event) {	
	if(!_working)
		if (_mainGui->onViewerQuit(_idViewer))
			event.Skip();
}

void wx2dGui::onShowAboutDialog(wxCommandEvent& WXUNUSED(event)) {
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