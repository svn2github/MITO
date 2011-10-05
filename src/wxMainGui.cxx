/**
 * \file wxMainGui.cxx
 * \brief File per la gestione della finestra principale
 * \author ICAR-CNR Napoli
 * \todo Investigare BUG query (espansione) che provoca crash (wxPaintDc crasha in modo CASUALE!)
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/toolbar.h>
#include <wx/image.h>
#include <wx/splitter.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/busyinfo.h>

#include "wx/progdlg.h"

#include "icons/ImportFile.xpm"
#include "icons/ImportFolder.xpm"
#include "icons/ImportStudy.xpm"
//#include "icons/ImportDicomdir.xpm"
#include "icons/QueryRetrieve.xpm"
#include "icons/Export.xpm"
#include "icons/Anonymizer.xpm"
#include "icons/ImageFusion.xpm"
#include "icons/Send.xpm"
#include "icons/Viewer.xpm"
#include "icons/Trash.xpm"
#include "icons/VolumeRendering.xpm"
#include "icons/Surface.xpm"

#include "wxVolumeRenderingGui.h"
#include "wxMainGui.h"
#include "wx2dGui.h"
#include "exportFilter.h"
#include "wxAnonymizeDialog.h"
#include "wxPacsQueryGui.h"
#include "wxPacsStoreGui.h"
#include "fusionFilter.h"
#include "wxFusionDialog.h"
#include "wxVtkViewer3d.h"

//prova
#include "itkVtkData.h"

// by Nello per la gestione del progressDialog
#include <wx/progdlg.h>

#include "wxPacsPreferencesGui.h"
#include "pacsStoreSCP.h"
#include "mitoDefinitions.h"
#include "mitoLocalDatabase.h"

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcddirif.h>
//#include <dcmtk/dcmjpeg/ddpiimpl.h>

// Definita in pacsDataType.cxx
void FormatDicomDate(char *pOut, const char *pDateIn);
// Definita in pacsStoreSCP.cpp
wxArrayString *ExtractRelevantInformations(DcmDataset *pDataset, const char *pFilePath);

BEGIN_EVENT_TABLE(wxMainGui, wxFrame)
	EVT_MENU(Tool_importfile,wxMainGui::onOpenImage)
	//EVT_MENU(Tool_importfolder,wxMainGui::onOpenSeries)
	EVT_MENU(Tool_importstudy,wxMainGui::onOpenStudy)
	//EVT_MENU(Tool_importdicomdir,wxMainGui::onImportDicomdir)
	EVT_MENU(Tool_queryretrieve, wxMainGui::onQuery)
	EVT_MENU(Tool_send, wxMainGui::onSend)
	EVT_MENU(Tool_trash,wxMainGui::onDeleteExam)
	EVT_MENU(Tool_viewer,wxMainGui::onNew2dViewer)
	EVT_MENU(Tool_volumerendering,wxMainGui::onVolumeRendering)
	EVT_MENU(Tool_surface,wxMainGui::onSurfaceRendering)
	EVT_MENU(Tool_anonymizer,wxMainGui::onAnonymize)
	EVT_MENU(Tool_imageFusion,wxMainGui::onImageFusion)
	EVT_MENU(Tool_export,wxMainGui::onExportToDicom)
	EVT_MENU(m_fileImportFile, wxMainGui::onOpenImage)
	EVT_MENU(m_fileImportSeries, wxMainGui::onOpenSeries)
	EVT_MENU(m_fileImportStudy, wxMainGui::onOpenStudy)
	EVT_MENU(m_fileHeader, wxMainGui::onViewHeader)
	EVT_MENU(m_fileDeleteSelectedExam, wxMainGui::onDeleteExam)
    EVT_MENU(m_viewerNew, wxMainGui::onNew2dViewer)
	EVT_MENU(m_viewerNextSlice, wxMainGui::onNextSlice)
	EVT_MENU(m_viewerPrevSlice, wxMainGui::onPrevSlice)
	EVT_MENU(m_viewerResetImageView, wxMainGui::onResetView)
	EVT_MENU(m_viewerFlipImageHorizontal, wxMainGui::onFlipHorizontal)
	EVT_MENU(m_viewerFlipImageVertical, wxMainGui::onFlipVertical)
	EVT_MENU(m_imageFusion,wxMainGui::onImageFusion)
	EVT_MENU(m_pacsSendToPacs, wxMainGui::onSend)
	EVT_MENU(m_pacsQueryRetrieve, wxMainGui::onQuery)
    EVT_MENU(m_pacsAutosavePreferences, wxMainGui::onPacsAutosavePreferences)
	EVT_MENU(m_pacsOpenPreferences, wxMainGui::onPacsOpenPreferences)
	EVT_MENU(m_pacsSavePreferences, wxMainGui::onPacsSavePreferences)
	EVT_MENU(m_pacsSavePreferencesAs, wxMainGui::onPacsSavePreferencesAs)
	EVT_MENU(m_pacsPreferences, wxMainGui::onPacsPreferences)
	EVT_MENU(m_fileAnonymize,wxMainGui::onAnonymize)
	EVT_MENU(m_fileExportDICOM,wxMainGui::onExportToDicom)
	EVT_MENU(m_fileExportBMP,wxMainGui::onExportToBmp)
	EVT_MENU(m_fileExportDICOMDIR, wxMainGui::OnExportDicomdir)
    EVT_MENU(m_HelpAbout, wxMainGui::onShowAboutDialog)
    EVT_MENU(m_fileClose, wxMainGui::onClose)

	EVT_ACTIVATE(wxMainGui::onActivate)
	EVT_SLIDER(id_Slider, wxMainGui::onSliderUpdate)
	//EVT_LIST_ITEM_ACTIVATED(id_Albums, wxMainGui::onAlbumActivated)
    EVT_TREE_ITEM_ACTIVATED(id_Albums, wxMainGui::onAlbumActivated)
	EVT_THUMBNAILS_DCLICK(id_Thumbnails, wxMainGui::onSelectThumb)
	EVT_CHECKBOX(id_AutoPlay, wxMainGui::onAutoPlay)

	EVT_BUTTON(id_Play,wxMainGui::onPlay)
	EVT_BUTTON(id_Stop,wxMainGui::onStop)
	EVT_CLOSE(wxMainGui::onQuit)

	// Messaggio inviato dal thread del server (store) per gli aggiornamenti sul suo stato
	EVT_COMMAND(wxID_ANY, wxEVT_STORE_NOTIFICATION, wxMainGui::OnStoreServerNotification)
	EVT_COMMAND(wxID_ANY, wxEVT_QUERYGUI_NOTIFICATION, wxMainGui::OnQueryGUINotification)

	// Gestiamo il RMB per mostrare un menu contestuale
	EVT_TREE_ITEM_MENU(id_Albums, wxMainGui::OnAlbumItemRightClick)
	EVT_MENU(m_contextSendToPACS, wxMainGui::OnContextSendToPACS)
	EVT_MENU(m_contextDeleteLocal, wxMainGui::OnContextDeleteLocal)
	EVT_MOUSEWHEEL(wxMainGui::OnMouseWheel)
END_EVENT_TABLE()


wxMainGui::wxMainGui(const wxString& title, const wxPoint& pos, const wxSize& size)
//       : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxDOUBLE_BORDER),
         : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE),
	     viewerHandler(), pacsCommunicator() {

	/*wxIcon icon;
	icon.LoadFile("icon.ico",wxBITMAP_TYPE_ICO);
	this->SetIcon(icon);*/
	SetIcon(wxICON(MITO));

	_idActiveViewer = 0;
	_pathDicomPacs = "";
	_preview = false;
	_working = false;
	_pacsModal = false;
	_pacsDaemon = false;
	m_bOpenedFirstSeries = false;
	_isOpSegmentation = segmentationFilter::notDefined;
	_pacsPreferencesGUI = NULL;
	_pacsWindow = NULL;
	m_pStoreSrv	= NULL;		// Puntatore al server per la ricezione delle immagini
	m_pLocalCache = NULL;

	CreateStatusBar(3);
	SetStatusText("Ready", 0);

	wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
	wxMenu *menuFileImport = new wxMenu;
    menuFileImport->Append(m_fileImportFile, "Import File\tCtrl-I");
	//menuFileImport->Append(m_fileImportSeries, "Import Series\tCtrl-E");
	menuFileImport->Append(m_fileImportStudy, "Import Study\tCtrl-S");
	//menuFileImport->Append(m_fileImportDicomdir, "Import Study\tCtrl-S");
	menuFile->Append(m_fileImport, "Import", menuFileImport);

	wxMenu *menuFileExport = new wxMenu;
    menuFileExport->Append(m_fileExportBMP, "Export to BMP\tCtrl-J");
	menuFileExport->Append(m_fileExportDICOM, "Export to DICOM\tCtrl-D");
	menuFileExport->Append(m_fileExportDICOMDIR, "Export to DICOMDIR\tCtrl-M");
	menuFile->Append(m_fileExport, "Export", menuFileExport);
	menuFile->AppendSeparator();
	menuFile->Append(m_fileHeader, "Show DICOM Header\tCtrl-H");
	menuFile->Append(m_fileAnonymize, "Anonymize\tCtrl-A");
	menuFile->Append(m_fileDeleteSelectedExam, "Delete Selected Exam\tCtrl-J");
	menuFile->AppendSeparator();
	menuFile->Append(m_fileClose, "Quit MITO\tCtrl-C");

	wxMenu *menuViewer = new wxMenu("", wxMENU_TEAROFF);
	menuViewer->Append(m_viewerNew, "New 2D Viewer\tCtrl-2");
	menuViewer->Append(Tool_volumerendering, "New 3D Direct VR viewer");
	menuViewer->Append(Tool_surface, "New 3D Indirect VR viewer");	
	menuViewer->AppendSeparator();

	menuViewer->Append(m_viewerNextSlice, "Next Slice\tCtrl-N");
	menuViewer->Append(m_viewerPrevSlice, "Previous Slice\tCtrl-P");
	menuViewer->AppendSeparator();
	menuViewer->Append(m_viewerResetImageView, "Reset Image View\tCtrl-R"); 
	menuViewer->Enable(m_viewerResetImageView, true);
	menuViewer->Append(m_viewerFlipImageHorizontal, "Flip Image Horizontal\tCtrl-O");
	menuViewer->Append(m_viewerFlipImageVertical, "Flip Image Vertical\tCtrl-V");

	wxMenu *menuImage = new wxMenu("", wxMENU_TEAROFF);
	menuImage->Append(m_imageFusion, "Image Fusion\tCtrl-F"); 

	wxMenu *menuPacs = new wxMenu("", wxMENU_TEAROFF);
	menuPacs->Append(m_pacsSendToPacs, "Send to Pacs\tCtrl-S");
	menuPacs->Append(m_pacsQueryRetrieve, "Query/Retrieve\tCtrl-Q");
	menuPacs->AppendSeparator();
	menuPacs->Append(m_pacsAutosavePreferences, "Autosave Preferences", wxT(""), wxITEM_CHECK);
	if(prefs->autoSave)
		menuPacs->Check(m_pacsAutosavePreferences, true);
	else
		menuPacs->Check(m_pacsAutosavePreferences, false);
	menuPacs->AppendSeparator();
	menuPacs->Append(m_pacsOpenPreferences, "Open Preferences");
	menuPacs->Append(m_pacsSavePreferences, "Save Preferences");
	menuPacs->Append(m_pacsSavePreferencesAs, "Save Preferences As...");
	menuPacs->AppendSeparator();
	menuPacs->Append(m_pacsPreferences, "Preferences\tCtrl-E");

	wxMenu *menuHelp = new wxMenu("", wxMENU_TEAROFF);
	menuHelp->Append(m_HelpAbout, "About MITO");

	wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
	menuBar->Append(menuPacs, "&Pacs");
	menuBar->Append(menuViewer, "&Viewer");
	menuBar->Append(menuImage, "&Image");
	menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

	long style = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT | wxTB_HORIZONTAL;

    wxToolBar *toolBar = CreateToolBar(style, id_ToolBar);

    // Set up toolbar
    wxBitmap toolBarBitmaps[Tool_max];

	#define INIT_TOOL_BMP(bmp) \
	toolBarBitmaps[Tool_##bmp] = wxBitmap(bmp##_xpm)

    INIT_TOOL_BMP(importfile);
    //INIT_TOOL_BMP(importfolder);
    INIT_TOOL_BMP(importstudy);
    //INIT_TOOL_BMP(importdicomdir);
    INIT_TOOL_BMP(queryretrieve);
    INIT_TOOL_BMP(export);
    INIT_TOOL_BMP(anonymizer);
    INIT_TOOL_BMP(imageFusion);
    INIT_TOOL_BMP(send);
    INIT_TOOL_BMP(viewer);
	INIT_TOOL_BMP(volumerendering);
	INIT_TOOL_BMP(surface);
    INIT_TOOL_BMP(trash);

	int w = toolBarBitmaps[Tool_importfile].GetWidth(),
        h = toolBarBitmaps[Tool_importfile].GetHeight();

    toolBar->SetToolBitmapSize(wxSize(w, h));
   
    toolBar->AddTool(Tool_importfile, _T("Import File"), toolBarBitmaps[Tool_importfile], _T("Import a DICOM file"));
    //toolBar->AddTool(Tool_importfolder, _T("Import Series"), toolBarBitmaps[Tool_importfolder], _T("Import a DICOM folder"));
    toolBar->AddTool(Tool_importstudy, _T("Import Study"), toolBarBitmaps[Tool_importstudy], _T("Import a DICOM study"));
    //toolBar->AddTool(Tool_importdicomdir, _T("Import DICOMDIR"), toolBarBitmaps[Tool_importdicomdir], _T("Import a DICOM directory"));
	toolBar->AddTool(Tool_export, _T("Export"), toolBarBitmaps[Tool_export], _T("Export selected study/series to a DICOM folder/image"));
	toolBar->AddTool(Tool_anonymizer, _T("Anonymize"), toolBarBitmaps[Tool_anonymizer], _T("Anonymize selected study/series to a DICOM folder/image"));
	toolBar->AddTool(Tool_queryretrieve, _T("Query"), toolBarBitmaps[Tool_queryretrieve], _T("Query and retrieve a DICOM study from your PACS Archive"));    
    toolBar->AddTool(Tool_send, _T("Send"), toolBarBitmaps[Tool_send], _T("Send selected study/series to your PACS Archive"));
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_imageFusion, _T("Image Fusion"), toolBarBitmaps[Tool_imageFusion], _T("Fuse selected study/series"));
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_viewer, _T("2D Viewer"), toolBarBitmaps[Tool_viewer], _T("Open a 2D viewer"));
	toolBar->AddTool(Tool_volumerendering, _T("3D Direct VR"), toolBarBitmaps[Tool_volumerendering], _T("3D Direct Volume Rendering"));
	toolBar->AddTool(Tool_surface, _T("3D Indirect VR"), toolBarBitmaps[Tool_surface], _T("3D Indirect Volume Rendering"));
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_trash, _T("Delete"), toolBarBitmaps[Tool_trash], _T("Delete selected exams from the album"));

	toolBar->Realize();
    toolBar->SetRows(1);

	wxSplitterWindow* thumb_Window = new wxSplitterWindow(this, id_Thumb_Window, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
    wxPanel* thumbPanel = new wxPanel(thumb_Window, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN);
    wxPanel *_sliderPanel = new wxPanel(thumb_Window, -1, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);	
    wxSplitterWindow* albums_Window = new wxSplitterWindow(_sliderPanel , id_Albums_Window, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
    
	_thumbnails = new wxThumbnails(thumbPanel, id_Thumbnails);
    
	wxPanel* albumsPanel = new wxPanel(albums_Window, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN);
	wxStaticText* albumsLabel = new wxStaticText(this, -1, _("Album: recent opened exams"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT | wxSTATIC_BORDER | wxST_NO_AUTORESIZE);
    wxFont fontAlbums = toolBar->GetFont();
	fontAlbums.SetPointSize(10);
	fontAlbums.SetWeight(wxFONTWEIGHT_BOLD);
	albumsLabel->SetFont(fontAlbums);
    albumsLabel->SetForegroundColour(toolBar->GetForegroundColour());
	albumsLabel->SetBackgroundColour(toolBar->GetBackgroundColour());
	    
	_album = new wxTreeListCtrl(albumsPanel, id_Albums, wxPoint( 0, 0 ), wxSize(350, 200), wxTR_HIDE_ROOT|wxTR_ROW_LINES| wxTR_HAS_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT );
	
	_album->AddColumn (_T("Name"), 220, wxALIGN_LEFT);
	_album->AddColumn (_T("Birthdate"), 170, wxALIGN_LEFT);
	_album->AddColumn (_T("Modality"), 85, wxALIGN_LEFT);
	_album->AddColumn (_T("Description"), 200, wxALIGN_LEFT);
	_album->AddColumn (_T("ID"), 150, wxALIGN_LEFT);
	_album->AddColumn (_T("Age"), 85, wxALIGN_LEFT);
	_album->AddColumn (_T("Accession Number"), 150, wxALIGN_LEFT);
	_album->AddColumn (_T("Date"), 170, wxALIGN_LEFT);
	_album->AddColumn (_T("# im"), 85, wxALIGN_LEFT);
	_album->AddColumn (_T("Referring Physician"), 170, wxALIGN_LEFT);
	_album->AddColumn (_T("Performing"), 170, wxALIGN_LEFT);
	_album->AddColumn (_T("Institution"), 170, wxALIGN_LEFT);
	_album->AddColumn (_T("Path"), 220, wxALIGN_LEFT);

	// A.Placitelli: Aggiungiamo l'elemento ROOT (risolve crash resize automatico colonne ad album vuoto)
	_album->AddRoot (_T("Root"));

  	_imageBoxPanel = new wxPanel(albums_Window, id_ImageBox_Window, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
    _imageBoxPanel->SetBackgroundColour(*wxBLACK);

	_slider = new wxSlider(_sliderPanel, id_Slider, 0, 0, 0, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_TOP);
	_autoPlay = new wxCheckBox(_sliderPanel,id_AutoPlay,_T("&Auto"),wxDefaultPosition,wxDefaultSize);    

	wxBoxSizer* mainHorWinSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* thumbSizer = new wxBoxSizer(wxVERTICAL);
    thumbSizer->Add(_thumbnails, 1, wxEXPAND, 0);
    thumbSizer->Fit(thumbPanel);
    thumbSizer->SetSizeHints(thumbPanel);
	thumbPanel->SetAutoLayout(true);
    thumbPanel->SetSizer(thumbSizer);

	_imageBoxSizer = new wxBoxSizer(wxVERTICAL);
	_imageBoxSizer->Fit(_imageBoxPanel);
    _imageBoxSizer->SetSizeHints(_imageBoxPanel);
	_imageBoxPanel->SetAutoLayout(true);
    _imageBoxPanel->SetSizer(_imageBoxSizer);

	wxBoxSizer* sliderSizer = new wxBoxSizer(wxVERTICAL);
	sliderSizer->Add(albums_Window, 1,  wxEXPAND|wxALIGN_CENTER, 0);
	sliderSizer->Add(_slider, 0,  wxEXPAND|wxALIGN_LEFT, 0);
	sliderSizer->Add(_autoPlay, 0,  wxALIGN_RIGHT, 0);
	sliderSizer->Fit(_sliderPanel);
    sliderSizer->SetSizeHints(_sliderPanel);
	_sliderPanel->SetAutoLayout(true);
    _sliderPanel->SetSizer(sliderSizer);

	wxBoxSizer* albumsSizer = new wxBoxSizer(wxVERTICAL);
    albumsSizer->Add(_album, 1, wxEXPAND, 0);    
    albumsSizer->Fit(albumsPanel);
    albumsSizer->SetSizeHints(albumsPanel);
	albumsPanel->SetAutoLayout(true);
    albumsPanel->SetSizer(albumsSizer);
    
	thumbPanel->SetSizeHints(_thumbnails->GetMinWidth(), _thumbnails->GetMinHeight());
	albums_Window->SetMinimumPaneSize(150);
	thumb_Window->SetMinimumPaneSize(250);
	
	_thumbnails->ShowFileNames(true);
	_thumbnails->SetThumbSize(70,70,20);
	_slider->Enable(false);
	_autoPlay->Enable(false);
    albums_Window->SplitHorizontally(albumsPanel, _imageBoxPanel, 250);
	thumb_Window->SplitVertically(_sliderPanel, thumbPanel, ((this->GetRect()).GetWidth()-330));

	wxBoxSizer* mainVerWinSizer = new wxBoxSizer(wxVERTICAL);
    mainVerWinSizer->Add(albumsLabel, 0, wxEXPAND, 0);
    mainVerWinSizer->Add(thumb_Window, 1, wxEXPAND, 0);
	mainHorWinSizer->Add(mainVerWinSizer, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(mainHorWinSizer);
	Layout();
    Centre();

	// Avviamo lo store server
	m_pStoreSrv = spawnStoreSCP(this);
	if(!m_pStoreSrv)
	{
		wxMessageBox(wxT("Could not start the store server! Check the port you are trying to listen to"), wxT("Error"));
	}

	m_pLocalCache = new mitoLocalDatabase();

	loadAlbum();
}

wxMainGui::~wxMainGui() {

	savePrefs(NULL);

	if(_pacsPreferencesGUI)
	{
		_pacsPreferencesGUI->quit(0);
		_pacsPreferencesGUI->Destroy();
		delete _pacsPreferencesGUI;		

		_pacsPreferencesGUI = NULL;
	}

	if(m_pStoreSrv)
	{
		delete m_pStoreSrv;
		m_pStoreSrv = NULL;
	}

	if(m_pLocalCache)
	{
		//m_pLocalCache->SaveToFile
		delete m_pLocalCache;
		m_pLocalCache = NULL;
	}

	this->Destroy();
}


void wxMainGui::onNew2dViewer(wxCommandEvent& event) {	
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			wx2dGui *w = new wx2dGui(_T("MITO: 2D Viewer"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());
			new2dViewer(w->new2dViewer(this));
			itkVtkData *itkVtkData_ = new itkVtkData();
			if(!((itkVtkData*)(getDataHandler()->getData(idData)))->getRgb()) {
				Duplicator::Pointer duplicator = Duplicator::New();
				duplicator->SetInputImage(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkImage());
				duplicator->SetMetaDataDictionary(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkImage()->GetMetaDataDictionary());
				duplicator->Update();
				itkVtkData_->setItkImage(duplicator->GetOutput());
				itkVtkData_->setRgb(false);
			}
			else {
				RGBDuplicator::Pointer duplicator = RGBDuplicator::New();
				duplicator->SetInputImage(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkRgbImage());
				duplicator->SetMetaDataDictionary(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkRgbImage()->GetMetaDataDictionary());
				duplicator->Update();
				itkVtkData_->setItkRgbImage(duplicator->GetOutput());
				itkVtkData_->setRgb(true);
			}
			itkVtkData_->resetVtkData();
			itkVtkData_->copyHeaderInfo(((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomEntries(),((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomKey(),((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomValue());
			itkVtkData_->setSliceNumber(((itkVtkData*)(getDataHandler()->getData(idData)))->getSliceNumber());
			itkVtkData_->setName(((itkVtkData*)(getDataHandler()->getData(idData)))->getName());
			itkVtkData_->setDownUp(((itkVtkData*)(getDataHandler()->getData(idData)))->getDownUp());
			itkVtkData_->setOffset(((itkVtkData*)(getDataHandler()->getData(idData)))->getOffset());
			itkVtkData_->setImageViewPlane(((itkVtkData*)(getDataHandler()->getData(idData)))->getImageViewPlane());
			itkVtkData_->setWl(((itkVtkData*)(getDataHandler()->getData(idData)))->getWl());
			itkVtkData_->setWw(((itkVtkData*)(getDataHandler()->getData(idData)))->getWw());
			idViewer = getActiveViewer();
			getDataHandler()->newData(itkVtkData_,idViewer,false);
			w->setIdViewer(idViewer);
			w->show(itkVtkData_);

			// A.Placitelli
			w->SetFocus();
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}




void wxMainGui::onVolumeRendering(wxCommandEvent& event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			wx2dGui *w = new wx2dGui(_T("MITO: 2D Viewer"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());
			new2dViewer(w->new2dViewer(this));
			itkVtkData *itkVtkData_ = new itkVtkData();
			if(!((itkVtkData*)(getDataHandler()->getData(idData)))->getRgb()) { // non RGB
				Duplicator::Pointer duplicator = Duplicator::New();
				duplicator->SetInputImage(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkImage());
				duplicator->SetMetaDataDictionary(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkImage()->GetMetaDataDictionary());
				duplicator->Update();
				itkVtkData_->setItkImage(duplicator->GetOutput());
				itkVtkData_->setRgb(false);
			}
			else { // RGB
				RGBDuplicator::Pointer duplicator = RGBDuplicator::New();
				duplicator->SetInputImage(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkRgbImage());
				duplicator->SetMetaDataDictionary(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkRgbImage()->GetMetaDataDictionary());
				duplicator->Update();
				itkVtkData_->setItkRgbImage(duplicator->GetOutput());
				itkVtkData_->setRgb(true);
			}
			itkVtkData_->resetVtkData();
			itkVtkData_->copyHeaderInfo(((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomEntries(),((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomKey(),((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomValue());
			itkVtkData_->setSliceNumber(((itkVtkData*)(getDataHandler()->getData(idData)))->getSliceNumber());
			itkVtkData_->setName(((itkVtkData*)(getDataHandler()->getData(idData)))->getName());
			itkVtkData_->setDownUp(((itkVtkData*)(getDataHandler()->getData(idData)))->getDownUp());
			itkVtkData_->setOffset(((itkVtkData*)(getDataHandler()->getData(idData)))->getOffset());
			itkVtkData_->setImageViewPlane(((itkVtkData*)(getDataHandler()->getData(idData)))->getImageViewPlane());
			itkVtkData_->setWl(((itkVtkData*)(getDataHandler()->getData(idData)))->getWl());
			itkVtkData_->setWw(((itkVtkData*)(getDataHandler()->getData(idData)))->getWw());
			idViewer = getActiveViewer();
			getDataHandler()->newData(itkVtkData_,idViewer,false);
			w->setIdViewer(idViewer);
			w->show(itkVtkData_,false);			
						
			w->onVolumeRendering(event);

			//w->show(itkVtkData_);			
			//w->SetFocus();
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wxMainGui::onSurfaceRendering(wxCommandEvent& event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			wx2dGui *w = new wx2dGui(_T("MITO: 2D Viewer"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());
			new2dViewer(w->new2dViewer(this));
			itkVtkData *itkVtkData_ = new itkVtkData();
			if(!((itkVtkData*)(getDataHandler()->getData(idData)))->getRgb()) {
				Duplicator::Pointer duplicator = Duplicator::New();
				duplicator->SetInputImage(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkImage());
				duplicator->SetMetaDataDictionary(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkImage()->GetMetaDataDictionary());
				duplicator->Update();
				itkVtkData_->setItkImage(duplicator->GetOutput());
				itkVtkData_->setRgb(false);
			}
			else {
				RGBDuplicator::Pointer duplicator = RGBDuplicator::New();
				duplicator->SetInputImage(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkRgbImage());
				duplicator->SetMetaDataDictionary(((itkVtkData*)(getDataHandler()->getData(idData)))->getItkRgbImage()->GetMetaDataDictionary());
				duplicator->Update();
				itkVtkData_->setItkRgbImage(duplicator->GetOutput());
				itkVtkData_->setRgb(true);
			}
			itkVtkData_->resetVtkData();
			itkVtkData_->copyHeaderInfo(((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomEntries(),((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomKey(),((itkVtkData*)(getDataHandler()->getData(idData)))->getDicomValue());
			itkVtkData_->setSliceNumber(((itkVtkData*)(getDataHandler()->getData(idData)))->getSliceNumber());
			itkVtkData_->setName(((itkVtkData*)(getDataHandler()->getData(idData)))->getName());
			itkVtkData_->setDownUp(((itkVtkData*)(getDataHandler()->getData(idData)))->getDownUp());
			itkVtkData_->setOffset(((itkVtkData*)(getDataHandler()->getData(idData)))->getOffset());
			itkVtkData_->setImageViewPlane(((itkVtkData*)(getDataHandler()->getData(idData)))->getImageViewPlane());
			itkVtkData_->setWl(((itkVtkData*)(getDataHandler()->getData(idData)))->getWl());
			itkVtkData_->setWw(((itkVtkData*)(getDataHandler()->getData(idData)))->getWw());
			idViewer = getActiveViewer();
			getDataHandler()->newData(itkVtkData_,idViewer,false);
			w->setIdViewer(idViewer);
			w->show(itkVtkData_,false);			
						
			w->onSurfaceRendering(event);

			//w->show(itkVtkData_);			
			//w->SetFocus();
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wxMainGui::onActivate(wxActivateEvent& event) {
	if (onViewerActivate(_idPreview))
		event.Skip();
}


void wxMainGui::onInitializeViewer() 
{
	string fileName = "";
	if(_pathDicomPacs == "") 
	{
		wxFileDialog *dlg = new wxFileDialog(this, "Open image",
									"", "", "DICOM Files(*.dcm;*.dicom)|*.dcm;*.dicom|All Files(*.*)|*.*",
									wxOPEN, wxDefaultPosition);
		if (dlg->ShowModal() == wxID_OK)
			fileName = dlg->GetPath().c_str();
		dlg->Destroy();
	}
	else 
		fileName = _pathDicomPacs;

	if (fileName != "") {
		wxWindowDisabler disabler;
		wxBusyInfo wait("Please wait, loading...");
		wxBusyCursor cursor;

		// TODO: Cercare usando StudyUID
		int index = -1;//searchRecord(fileName);
		if(index==-1){
			new2dViewer(new wxVtkViewer2d(this, _imageBoxPanel, id_ImageBox));
			unsigned int idViewer = getActiveViewer();
			_idPreview = idViewer;
			unsigned int idData = getDataHandler()->newData(getViewer(idViewer)->newAppData(), idViewer);
			if (getDataHandler()->getData(idData)->open(fileName, true)) {
				getDataHandler()->getData(idData)->setName(fileName);
				_imageBoxSizer->Add(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
				((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Freeze();
				getViewer(idViewer)->showAppData(getDataHandler()->getData(idData));
				_imageBoxSizer->Layout();
				((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Thaw();
				itkVtkData *itkVtkData_ = (itkVtkData*)(getDataHandler()->getData(idData));		
				const int slicesNumber = get2dViewer(idViewer)->getSliceNumber(getDataHandler()->getData(idData));
				wxImage* images = itkVtkData_->getThumbnails();
				_thumbnails->Clear();
				_thumbnails->ShowImages(slicesNumber, images);
				_slider->SetRange(0,(slicesNumber-1));
				_slider->SetValue(0);
				_slider->Enable(true);
				_autoPlay->Enable(true);

				// TODO: Verificare se non intacca alcuna funzione..

/*				albumData *aData = new albumData();
				wxArrayString* array = new wxArrayString();
				array->Add(itkVtkData_->getHeaderEntry(20).c_str());
				array->Add(itkVtkData_->getHeaderEntry(21).c_str());
				array->Add(itkVtkData_->getHeaderEntry(5).c_str());
				char sliceNumber[5];
				itoa(itkVtkData_->getSliceNumber(),sliceNumber,10);
				array->Add(sliceNumber);
				array->Add(itkVtkData_->getName().c_str());
				aData->data.push_back(array);
				aData->idStudy = fileName;
				aData->open = false;
				_albumHandler.push_back(aData);*/

				viewTextData();

					// TODO : Aggiungi all'album l'immagine singola
					DcmFileFormat dcm;
					OFCondition cond = dcm.loadFile(fileName.c_str());
					if(cond.good()) 
					{
						DcmDataset *pDataset = dcm.getDataset();
						wxArrayString *pInfo = ExtractRelevantInformations(pDataset, fileName.c_str());
						if(!pInfo)
							return;

						if(!m_pLocalCache->AddFileToDatabase(pInfo))
						{
							delete pInfo;
							return;
						}

						// Aggiungi all'album
						wxString sStudyUID, sSeriesUID;
						
						sStudyUID = pInfo->Item(MITO_STUDY_UID);		// Salviamo lo study UID corrente
						sSeriesUID = pInfo->Item(MITO_SERIES_UID);		// Salviamo il series UID corrente

						wxTreeItemId rootItem = _album->GetRootItem();
						if(!rootItem.IsOk())
						{
							rootItem = _album->AddRoot (_T("Root"));
						}

						wxTreeItemId foundStudy;

						int iResult = searchRecord((const char*)sStudyUID, (const char*)sSeriesUID, foundStudy);
						switch(iResult)
						{
						case 0:	// Non trovato
							{
								wxTreeItemId studyItem = addStudyToAlbum(pInfo, rootItem);
								wxTreeItemId seriesItem = addSeriesToAlbum(pInfo, studyItem, 1);
								
								// Associamo il pInfo al nodo dell'album
								_album->SetItemDataPtr(seriesItem, pInfo);

							}break;
						case 1:	// Trovato lo studio
							{
								wxTreeItemId seriesItem = addSeriesToAlbum(pInfo, foundStudy, 1);

								// Associamo il pInfo al nodo dell'album
								_album->SetItemDataPtr(seriesItem, pInfo);
							}break;
						case 2:	// Trovato serie e studio
							{
								wxMessageBox(wxT("The file you are trying to open is already in the album."), wxT("Information"));
								return;
							}break;
						}

					}
			}
			else{
				getDataHandler()->deleteData(idData);
				_imageBoxSizer->Detach(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow());
				quitViewer(idViewer);
				wxMessageDialog messageDialog(this, "Error opening the selected dicom file or directory.", "MITO",wxICON_INFORMATION|wxOK);
				messageDialog.ShowModal();
			}
		}
		else {
			wxListEvent event;
			event.m_itemIndex = index;
			//onAlbumActivated(event);

			// TODO
			//_album->SetItemState(index,wxLIST_STATE_SELECTED ,wxLIST_STATE_SELECTED);
		}
		this->Raise();
	}
}

void wxMainGui::onOpenImage(wxCommandEvent& event) 
{
	unsigned int idViewer = getActiveViewer();

	if (idViewer)
	{
		string fileName = "";
		if(_pathDicomPacs == "") 
		{
			wxFileDialog *dlg = new wxFileDialog(this, "Open image",
				"", "", "DICOM Files(*.dcm;*.dicom)|*.dcm;*.dicom|All Files(*.*)|*.*", wxOPEN, wxDefaultPosition);
			if (dlg->ShowModal() == wxID_OK)
					fileName = dlg->GetPath().c_str();
				dlg->Destroy();
		}
		else 
			fileName = _pathDicomPacs;

		if (fileName != "") 
		{
			wxWindowDisabler disabler;
			wxBusyInfo wait("Please wait, loading...");
			wxBusyCursor cursor;

			// TODO: usare la funzione per ricerca study UID
			int index = -1;//searchRecord(fileName);
			if(index==-1) {
				_imageBoxSizer->Detach(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow());
				unsigned int oldIdViewer = idViewer;
				new2dViewer(new wxVtkViewer2d(this, _imageBoxPanel, id_ImageBox));
				idViewer = getActiveViewer();
				_idPreview = idViewer;
				unsigned int idData = getDataHandler()->newData(getViewer(idViewer)->newAppData(), idViewer);

				if (getDataHandler()->getData(idData)->open(fileName,true)) {
					onViewerQuit(oldIdViewer);
					getDataHandler()->getData(idData)->setName(fileName);
					_imageBoxSizer->Add(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
				
					((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Freeze();
					getViewer(idViewer)->showAppData(getDataHandler()->getData(idData));
					_imageBoxSizer->Layout();
					((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Thaw();
					itkVtkData *itkVtkData_ = (itkVtkData*)(getDataHandler()->getData(idData));		
					const int slicesNumber = get2dViewer(idViewer)->getSliceNumber(getDataHandler()->getData(idData));
					wxImage* images = itkVtkData_->getThumbnails();
					_thumbnails->Clear();
					_thumbnails->ShowImages(slicesNumber, images);
					_slider->SetRange(0,(slicesNumber-1));
					_slider->SetValue(0);
					_slider->Enable(true);
					_autoPlay->Enable(true);

					/* Verificare se non intacca alcuna funzione
					albumData *aData = new albumData();
					wxArrayString* array = new wxArrayString();
					array->Add(itkVtkData_->getHeaderEntry(20).c_str());
					array->Add(itkVtkData_->getHeaderEntry(21).c_str());
					array->Add(itkVtkData_->getHeaderEntry(5).c_str());
					char sliceNumber[5];
					itoa(itkVtkData_->getSliceNumber(),sliceNumber,10);
					array->Add(sliceNumber);
					array->Add(itkVtkData_->getName().c_str());
					aData->data.push_back(array);
					aData->idStudy = fileName;
					aData->open = false;
					_albumHandler.push_back(aData);
					*/

					viewTextData();
					
					// TODO : Aggiungi all'album l'immagine singola
					DcmFileFormat dcm;
					OFCondition cond = dcm.loadFile(fileName.c_str());
					if(cond.good()) 
					{
						DcmDataset *pDataset = dcm.getDataset();
						wxArrayString *pInfo = ExtractRelevantInformations(pDataset, fileName.c_str());
						if(!pInfo)
							return;

						if(!m_pLocalCache->AddFileToDatabase(pInfo))
						{
							delete pInfo;
							return;
						}

						// Aggiungi all'album
						wxString sStudyUID, sSeriesUID;
						
						sStudyUID = pInfo->Item(MITO_STUDY_UID);		// Salviamo lo study UID corrente
						sSeriesUID = pInfo->Item(MITO_SERIES_UID);		// Salviamo il series UID corrente

						wxTreeItemId rootItem = _album->GetRootItem();
						if(!rootItem.IsOk())
						{
							rootItem = _album->AddRoot (_T("Root"));
						}

						wxTreeItemId foundStudy;

						int iResult = searchRecord((const char*)sStudyUID, (const char*)sSeriesUID, foundStudy);
						switch(iResult)
						{
						case 0:	// Non trovato
							{
								wxTreeItemId studyItem = addStudyToAlbum(pInfo, rootItem);
								wxTreeItemId seriesItem = addSeriesToAlbum(pInfo, studyItem, 1);
								
								// Associamo il pInfo al nodo dell'album
								_album->SetItemDataPtr(seriesItem, pInfo);

							}break;
						case 1:	// Trovato lo studio
							{
								wxTreeItemId seriesItem = addSeriesToAlbum(pInfo, foundStudy, 1);

								// Associamo il pInfo al nodo dell'album
								_album->SetItemDataPtr(seriesItem, pInfo);
							}break;
						case 2:	// Trovato serie e studio
							{
								wxMessageBox(wxT("The file you are trying to open is already in the album."), wxT("Information"));
								return;
							}break;
						}

					}
				}
				else{
					getDataHandler()->deleteData(idData);
					_imageBoxSizer->Detach(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow());
					quitViewer(idViewer);
					_imageBoxSizer->Add(((wxVtkViewer2d*)(getViewer(oldIdViewer)->getViewerDrawing()))->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
					this->onViewerActivate(oldIdViewer);
					_idPreview = oldIdViewer;	
					wxMessageDialog messageDialog(this, "Error opening the selected dicom file or directory.", "MITO",wxICON_INFORMATION|wxOK);
					messageDialog.ShowModal();
				}
			}
			else {
				wxListEvent event;
				event.m_itemIndex = index;
//				onAlbumActivated(event);
				
				// TODO
				// _album->SetItemState(index,wxLIST_STATE_SELECTED ,wxLIST_STATE_SELECTED);
			}
			this->Raise();
		}
	}
	else 
		onInitializeViewer();
}

void wxMainGui::onFlipHorizontal(wxCommandEvent& event) {
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading...");
	wxBusyCursor cursor;

	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			flipFilter f(idData, getDataHandler());
			f.computeVTK('x');
			((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->setHorFlip();
			updateViewerByIdData(idData);
			wxWindow* window = ((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow();
			while(!(window->IsTopLevel())){
				if(window->GetParent())
					window = window->GetParent(); 			
			}
			window->Raise();
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wxMainGui::onFlipVertical(wxCommandEvent& event) {
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading...");
	wxBusyCursor cursor;

	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			flipFilter f(idData, getDataHandler());
			f.computeVTK('y');
			((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->setVerFlip();
			updateViewerByIdData(idData);
			wxWindow* window = ((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow();
			while(!(window->IsTopLevel())){
				if(window->GetParent())
					window = window->GetParent(); 			
			}
			window->Raise();
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wxMainGui::onOpenSeries(wxCommandEvent& event) 
{
	// Modifica A. Placitelli: a causa delle modifiche nel codice,
	// open study è diventato molto più generico ed è quindi in grado
	// di gestire anche l'apertura di una serie.
	onOpenStudy(event);
}

void wxMainGui::onOpenStudy(wxCommandEvent& event) 
{
	// Mostriamo il dialog per la scelta del percorso
	string fileName = "";
	if(_pathDicomPacs == "") 
	{
		wxDirDialog *dlg = new wxDirDialog(this, "Browse for a folder containing DICOM files", "");
		if (dlg->ShowModal() == wxID_OK)
			fileName = dlg->GetPath().c_str();
		
		dlg->Destroy();
	}
	else 
		fileName = _pathDicomPacs;

	// Enumeriamo le sotto directory
	if(fileName == "")
		return;

	wxWindowDisabler disabler;
	// by Nello 
	// wxBusyInfo wait("Please wait, loading...");
	wxBusyCursor cursor;


	wxDir studyPath(fileName.c_str());
	if(!wxDir::Exists(fileName.c_str())) 
		return;

	
	wxArrayString* studyFiles = new wxArrayString();
	if(!studyFiles)
		return;

	studyPath.GetAllFiles(fileName.c_str(), studyFiles);

//	wxULongLong temp = studyPath.GetTotalSize(fileName.c_str());

	size_t numFiles = studyFiles->GetCount();
	if(!numFiles)
		return;

	// Assicuriamoci che l'album sia pronto per ricevere i nuovi elementi
	wxTreeItemId rootItem = _album->GetRootItem();
	if(!rootItem.IsOk())
	{
		rootItem = _album->AddRoot (_T("Root"));
	}

	DcmFileFormat dcm;
	OFCondition cond;

	wxString sLastSeriesUID;
	wxString sLastStudyUID;
	int		 iStudyImgCount = 0,
			 iSeriesImgCount = 0;
	wxTreeItemId lastStudyItem,
				 lastSeriesItem,
				 firstSeriesItem;

	// Carichiamo i tag dei files ed estraiamo le informazioni utili
	for(size_t k = 0; k < numFiles; k++)
	{
		cond = dcm.loadFile((const char*)(studyFiles->Item(k)));
		if (cond.bad()) 
		{
			// Arghhh qualcosa è andato storto.. saltiamolo :)
			continue;
		}
	
		DcmDataset *pDataset = dcm.getDataset();
		if(!pDataset)
			continue;

		wxArrayString *pInfo = ExtractRelevantInformations(pDataset, studyFiles->Item(k));
		if(!pInfo)
			continue;

		if(!m_pLocalCache->AddFileToDatabase(pInfo))
		{
			delete pInfo;
			continue;
		}

		// Aggiungi all'album
		wxString sStudyUID, sSeriesUID;
		
		sStudyUID = pInfo->Item(MITO_STUDY_UID);		// Salviamo lo study UID corrente
		sSeriesUID = pInfo->Item(MITO_SERIES_UID);	// Salviamo il series UID corrente

		// Aggiungiamo l'elemento studio
		if(sLastStudyUID.IsEmpty() || (sLastStudyUID != sStudyUID))
		{
			// Resettiamo il numero di immagini nello studio e salviamole nell'album
			if(!sLastStudyUID.IsEmpty() && lastStudyItem.IsOk())
			{
				_album->SetItemText(lastStudyItem, 8, wxString::Format("%i", iStudyImgCount));
				iStudyImgCount = 0;
			}

			sLastStudyUID = sStudyUID;

			lastStudyItem = addStudyToAlbum(pInfo, rootItem);

			// Associamo il pInfo al nodo dell'album
			_album->SetItemDataPtr(lastStudyItem, pInfo);
		}

		// Aggiungiamo le serie
		if(sSeriesUID != sLastSeriesUID)
		{
			if(!sLastSeriesUID.IsEmpty() && lastSeriesItem.IsOk())
			{
				_album->SetItemText(lastSeriesItem, 8, wxString::Format("%i", iSeriesImgCount));
				iSeriesImgCount = 0;
			}

			// Il numero di immagini verrà impostato successivamente dopo averle contate!
			lastSeriesItem = addSeriesToAlbum(pInfo, lastStudyItem, 0);

			sLastSeriesUID = sSeriesUID;

			if(!firstSeriesItem.IsOk())
			{
				firstSeriesItem = lastSeriesItem;
			}

			// Associamo il pInfo al nodo dell'album
			_album->SetItemDataPtr(lastSeriesItem, pInfo);
		}

		iStudyImgCount++;
		iSeriesImgCount++;
	}

	if(lastStudyItem.IsOk())
	{
		_album->SetItemText(lastStudyItem, 8, wxString::Format("%i", iStudyImgCount));	
	}

	// Nel caso in cui ci sia una sola serie nello studio, non dimentichiamo di
	// salvare il numero di immagini della serie..
	if(iSeriesImgCount && !sLastSeriesUID.IsEmpty() && lastSeriesItem.IsOk())
	{
		_album->SetItemText(lastSeriesItem, 8, wxString::Format("%i", iSeriesImgCount));
	}

	// Ricoloriamo..
	_album->DoAlternateRowColors();

	// Carichiamo la prima serie importata
	wxTreeEvent evt;
	evt.SetItem(firstSeriesItem);
	onAlbumActivated(evt);
	_slider->SetFocus();
}

void wxMainGui::onImportDicomdir(wxCommandEvent &event)
{/*
	string fileName = "";

	wxFileDialog *dlgFile = new wxFileDialog(this, "Load DICOMDIR index", "", "", "DICOMDIR Index Files(*.dir)|*.dir|All files(*.*)|*.*");
	if (dlgFile->ShowModal() == wxID_OK) 
	{
		fileName = dlgFile->GetPath();
		
		dlgFile->Destroy();
	}else
		return;

	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading...");
	wxBusyCursor cursor;


	//if(!wxFileName::FileExists(fileName.c_str())) 
	//	return;

	if(!m_pLocalCache->ImportDicomDir(fileName.c_str(), _album))
		wxMessageBox(wxT("An error occoured while importing DICOMDIR."));*/

	onOpenStudy(event);
}

void wxMainGui::onNextSlice(wxCommandEvent& event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (get2dViewer(idViewer)->getSliceNumber(getDataHandler()->getData(idData))>1){
			get2dViewer(idViewer)->nextSlice();
			int zSlice = _slider->GetValue()+1;
			if (zSlice > _slider->GetMax())
				zSlice = _slider->GetMin();
			_slider->SetValue(zSlice);
			viewTextData();
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No series is currently opened."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();	
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wxMainGui::onPrevSlice(wxCommandEvent& event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (get2dViewer(idViewer)->getSliceNumber(getDataHandler()->getData(idData))>1){
			get2dViewer(idViewer)->prevSlice();
			int zSlice = _slider->GetValue()-1;
			if (zSlice < _slider->GetMin())
				zSlice = _slider->GetMax();
			_slider->SetValue(zSlice);
			viewTextData();
		}
		else {
			wxMessageDialog* messDialog = new wxMessageDialog(this, _T("No series is currently opened."), _T("MITO"), wxOK | wxICON_INFORMATION);
			if (messDialog->ShowModal()==wxID_OK)
				messDialog->Destroy();	
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

unsigned int wxMainGui::getActiveViewer() {
	if (getViewer(_idActiveViewer))
		return _idActiveViewer;
	else return 0;
}

bool wxMainGui::onButtonDown(unsigned int idViewer, wxMouseEvent &event) {
	return true;
}


bool wxMainGui::onMotion(unsigned int idViewer, wxMouseEvent &event) {
	if (getViewer(idViewer)) {
		getViewer(idViewer)->setMouseX(event.GetX());
		getViewer(idViewer)->setMouseY(event.GetY());
		viewTextData();
	}
	return true;
}


void wxMainGui::viewHeaderDicom(wxWindow* parent){
	// TODO: Aprire l'header dell'immagine corrente e non dell'ultima!
	unsigned int idData = getViewer(getActiveViewer())->getActiveIdSingleData();
	if (idData) 
	{
		wxArrayString a;
	
		for (int i=0;i<getDataHandler()->getData(idData)->getHeaderLength();i++) 
		{
			wxString s;
			wxString k = getDataHandler()->getData(idData)->getHeaderKey(i).c_str();
			wxString v = getDataHandler()->getData(idData)->getHeaderValue(i).c_str();
			s = s.Format("%s - %s", k.c_str(), v.c_str());
			a.Add(s);
		}

		wxSingleChoiceDialog *dlg;
		if(((itkVtkData*)getDataHandler()->getData(idData))->getSliceNumber() > 1)
			dlg = new wxSingleChoiceDialog(parent, "Keys and Values of the last image", "DICOM Header", a);
		else
			dlg = new wxSingleChoiceDialog(parent, "Keys and Values", "DICOM Header", a);
		
		dlg->ShowModal();
		dlg->Destroy();
	}
}

void wxMainGui::onViewHeader(wxCommandEvent& event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		viewHeaderDicom(this);
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}

void wxMainGui::onSelectThumb(wxCommandEvent& event)
{
	if (_thumbnails->GetSelected() != -1){
		unsigned int idViewer = getActiveViewer();
		if (idViewer) {
			_slider->SetValue(_thumbnails->GetSelected());	
			get2dViewer(idViewer)->setSlice(_thumbnails->GetSelected());
			viewTextData();
		}
	}
}		

void wxMainGui::onSliderUpdate( wxCommandEvent &event )
{
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		get2dViewer(idViewer)->setSlice(_slider->GetValue());
		_thumbnails->SetSelected(_slider->GetValue());
		viewTextData();
	}
}

void wxMainGui::OnMouseWheel( wxMouseEvent &event)
{
	_slider->SetFocus();
}

void wxMainGui::onAutoPlay( wxCommandEvent &event )
{	
	wxWindowDisabler disabler;
	if(_slider->GetMin()!=_slider->GetMax()){
		if (_autoPlay->GetValue()){
			if(!_working){
				wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED , id_Play);
				wxPostEvent(this, event );
			}
			else
				_autoPlay->SetValue(false);
		}
		else {
			if(_working){
				wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED , id_Stop );
				wxPostEvent(this, event );
			}
			else
				_autoPlay->SetValue(true);
		}
	}
	else
		_autoPlay->SetValue(false);
}

void wxMainGui::onPlay( wxCommandEvent &event )
{	
	if(!_working){
		_working = true;
		unsigned int idViewer = getActiveViewer();
		_slider->Enable(false);	
		_thumbnails->Enable(false);
		((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Enable(false);
		_album->Enable(false);
		for (int i=0;i<GetMenuBar()->GetMenuCount();i++)
			GetMenuBar()->EnableTop(i, false);
		GetToolBar()->Enable(false);
		_preview = false;
		int index;
		for( index=_slider->GetValue()+1; index<=_slider->GetMax();index++){
			get2dViewer(idViewer)->nextSlice();
			_slider->SetValue(index);
			viewTextData();
			wxMilliSleep(500);
			wxYield();
			if (_preview)
				break;	
		}
		if(index>_slider->GetMax()){
			get2dViewer(idViewer)->nextSlice();
			_slider->SetValue(0);
			viewTextData();
		}
		_working = false;
		_autoPlay->SetValue(false);
		_slider->Enable(true);	
		_thumbnails->Enable(true);
		((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Enable(true);
		_album->Enable(true);
		for (int i=0;i<GetMenuBar()->GetMenuCount();i++)
			GetMenuBar()->EnableTop(i, true);
		GetToolBar()->Enable(true);
	}
}

void wxMainGui::onStop( wxCommandEvent &event )
{	
	if(_working)
		_preview=true;
}

void wxMainGui::onDeleteExam(wxCommandEvent &event)
{
	if (_album->GetCount()!=0) 
	{
		// Siccome il codice per l'eliminazione degli studi è già gestito 
		// dal menu a tendina, utilizziamo direttamente quell'handler :)
		OnContextDeleteLocal(event);
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("There is no studies in the album."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();	
	}
}


void wxMainGui::openDicomStudyPacs(string path, wxArrayInt seriesFailedPacs) {
	wxDir wxDirectory(path.c_str());
	if(wxDirectory.Exists(path.c_str())) {
		//_pathDicomPacs.assign(path);
		_seriesFailedPacs = seriesFailedPacs;
		_pathDicomPacs = path;
		onOpenStudy((wxCommandEvent)NULL);
		_pathDicomPacs = "";
	}
}

void wxMainGui::deleteAlbumEntry(wxTreeItemId &itemId, bool bDeleteFromHdd)
{
	wxArrayString *pInfo = (wxArrayString *)(_album->GetItemDataPtr(itemId));
	if(!pInfo)
	{
		// Se le informazioni associate al tree item non sono valide, eliminiamolo
		// dall'album
		_album->Delete(itemId);
		return;
	}

	wxTreeItemId itemParent = _album->GetItemParent(itemId);
	wxASSERT(itemParent.IsOk());

	bool bIsStudy = (itemParent == _album->GetRootItem());
	if(bIsStudy)
	{
		if(!m_pLocalCache->RemoveFromDatabase(pInfo, bIsStudy, bDeleteFromHdd))
		{
			wxMessageBox(wxT("There was an error while removing local cache."), wxT("MITO Error"));
		}

		_album->Delete(itemId);
		_album->DoAlternateRowColors();
	}else
	{
		deleteAlbumEntry(itemParent, bDeleteFromHdd);
	}
}

void wxMainGui::closeActiveSeries()
{
	unsigned int idViewer = getActiveViewer();
	
	_imageBoxSizer->Detach(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow());

	onViewerQuit(idViewer);

	_thumbnails->Clear();
}

wxTreeItemId wxMainGui::addStudyToAlbum(wxArrayString *pInfo, wxTreeItemId parentNode)
{
	char szDateTemp[16];
	wxString szValue;

	wxTreeItemId newStudy = _album->AppendItem(parentNode, (const char *)pInfo->Item(MITO_PATIENT_NAME));	// Nome
	FormatDicomDate(szDateTemp, pInfo->Item(2));
	_album->SetItemText (newStudy, 1, (const char *)szDateTemp);	// Birthdate

	szValue = pInfo->Item(MITO_STUDY_MODALITY);
	_album->SetItemText (newStudy, 2, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Modality

	szValue = pInfo->Item(MITO_STUDY_DESCRIPTION);
	_album->SetItemText (newStudy, 3, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Desc

	szValue = pInfo->Item(MITO_PATIENT_ID);
	_album->SetItemText (newStudy, 4, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Patient ID

	szValue = pInfo->Item(MITO_PATIENT_AGE);
	_album->SetItemText (newStudy, 5, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Patient Age

	szValue = pInfo->Item(MITO_ACCESSION);
	_album->SetItemText (newStudy, 6, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Accession

	FormatDicomDate(szDateTemp, pInfo->Item(MITO_STUDY_DATE));
	_album->SetItemText (newStudy, 7, (const char *)szDateTemp);	// Date

	szValue = pInfo->Item(MITO_STUDY_IMAGES);
	_album->SetItemText (newStudy, 8, (szValue != wxT("")) ? szValue : wxT("N/A"));	// # imgs

	szValue = pInfo->Item(MITO_REFERRING_PHYSICIAN);
	_album->SetItemText (newStudy, 9, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Referring phy

	szValue = pInfo->Item(MITO_PERFORMING_PHYSICIAN);
	_album->SetItemText (newStudy, 10, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Performing phy

	szValue = pInfo->Item(MITO_INSITUTION);
	_album->SetItemText (newStudy, 11, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Institution


	// Path
	if(pInfo->Item(MITO_SAVEPATH) != wxT(""))
	{/*	// OLD
		wxFileName dirname(pInfo->Item(MITO_SAVEPATH), wxPATH_NATIVE );
		dirname.RemoveLastDir();
		dirname.Normalize();

		wxString fullPath(dirname.GetFullPath());
		_album->SetItemText(newStudy, 12, fullPath);	
		*/

		wxString filePath(pInfo->Item(MITO_SAVEPATH));
		wxFileName dirName = wxFileName::DirName(filePath);
		dirName.RemoveLastDir();
		dirName.Normalize();

		_album->SetItemText(newStudy, 12, dirName.GetPath());	
	}else
		_album->SetItemText(newStudy, 12, wxT("N/A"));	

	return newStudy;
}

wxTreeItemId wxMainGui::addSeriesToAlbum(wxArrayString *pInfo, wxTreeItemId parentNode, int iSeriesImagesNum)
{
	wxString szValue;
			
	// In release non dovrebbe mai fallire, dato che il messaggio
	// di nuova serie segue SEMPRE ad almeno uno di nuovo studio.
	wxASSERT(parentNode.IsOk());

	szValue = pInfo->Item(MITO_BODYPART);
	wxTreeItemId newSeries = _album->AppendItem(parentNode, (szValue != wxT("")) ? szValue : wxT("Series"));	// Body part
	_album->SetItemText (newSeries, 1, (const char *)_T(""));			// Empty (Birthdate)

	szValue = pInfo->Item(MITO_STUDY_MODALITY);
	_album->SetItemText (newSeries, 2, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Modality

	szValue = pInfo->Item(MITO_SERIES_DESCRIPTION);
	_album->SetItemText (newSeries, 3, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Desc

	szValue = pInfo->Item(MITO_SERIES_UID);
	_album->SetItemText (newSeries, 4, (szValue != wxT("")) ? szValue : wxT("N/A"));	// Series UID

	_album->SetItemText (newSeries, 5, wxT(""));			// Empty (Age)
	_album->SetItemText (newSeries, 6, wxT(""));			// Empty (Ascession)


	char szDateTemp[16];
	FormatDicomDate(szDateTemp, pInfo->Item(MITO_SERIES_DATE));
	_album->SetItemText (newSeries, 7, (const char *)szDateTemp);		// Series date

	pInfo->Item(12) = wxString::Format(wxT("%i"), iSeriesImagesNum);
	_album->SetItemText (newSeries, 8, (const char *)pInfo->Item(MITO_SERIES_IMAGES));	// # images

	szValue = pInfo->Item(MITO_REFERRING_PHYSICIAN);
	_album->SetItemText (newSeries, 9, (szValue != wxT("")) ? szValue : wxT("N/A"));			// Referring 

	szValue = pInfo->Item(MITO_PERFORMING_PHYSICIAN);
	_album->SetItemText (newSeries, 10, (szValue != wxT("")) ? szValue : wxT("N/A"));			// Performing

	szValue = pInfo->Item(MITO_INSITUTION);
	_album->SetItemText (newSeries, 11, (szValue != wxT("")) ? szValue : wxT("N/A"));			// Instit

	// Path
	if(pInfo->Item(MITO_SAVEPATH) != wxT(""))
	{/* // OLD
		wxFileName dirname(pInfo->Item(MITO_SAVEPATH), wxPATH_NATIVE );
		dirname.RemoveLastDir();
		dirname.Normalize();

		wxString fullPath(dirname.GetFullPath());
		_album->SetItemText(newSeries, 12, fullPath);	*/

		wxString filePath(pInfo->Item(MITO_SAVEPATH));
		wxFileName dirName = wxFileName::DirName(filePath);
		dirName.RemoveLastDir();
		dirName.Normalize();

		_album->SetItemText(newSeries, 12, dirName.GetPath());
	}else
		_album->SetItemText(newSeries, 12, wxT("N/A"));	

	return newSeries;
}

void wxMainGui::onQuery(wxCommandEvent &event) {
/*	_pacsWindow = new wxPacsQueryGui(_pacsModal, _pacsDaemon, this, id_Query );
	if(_pacsWindow)
		_pacsWindow->Show(true);*/

	if(_pacsWindow)
	{
		wxMessageBox(wxT("You can't open more than one query interface."));
		return;
	}

    _pacsWindow = new wxPacsQueryGUI( this );
	if(_pacsWindow)
		_pacsWindow->Show(true);
    
	//SetTopWindow(frame);
}

void wxMainGui::onPacsAutosavePreferences(wxCommandEvent &event)
{
	prefs->autoSave = GetMenuBar()->IsChecked(m_pacsAutosavePreferences);
}

void wxMainGui::onPacsOpenPreferences(wxCommandEvent &event)
{
//	if (prefs->autoSave)
//		prefs->write(prefsName);

	string fileName = "";
	wxFileDialog *dlg = new wxFileDialog(this, "Open preferences file",
                                      "", "", "Configuration Files(*.cfg)|*.cfg|All files(*.*)|*.*",
                                       wxOPEN, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK)
					fileName = dlg->GetPath().c_str();
	dlg->Destroy();

	if (fileName.compare("")) {
		strcpy(prefsName, fileName.c_str());
		writeOutput("\nReading preferences...");
		prefs->read(prefsName);
	}
}

void wxMainGui::onPacsSavePreferences(wxCommandEvent &event)
{
	savePrefs(NULL);
}

void wxMainGui::onPacsSavePreferencesAs(wxCommandEvent &event)
{
	string fileName = "";
	wxFileDialog *dlg = new wxFileDialog(this, "Save preferences file",
                                      "", "", "Configuration Files(*.cfg)|*.cfg| All files(*.*)|*.*",
                                       wxSAVE, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK)
					fileName = dlg->GetPath().c_str();
	dlg->Destroy();
	savePrefs((char*)fileName.c_str());
}

void wxMainGui::onPacsPreferences(wxCommandEvent &event)
{
	if(!_pacsPreferencesGUI)
		_pacsPreferencesGUI = new wxPacsPreferencesGui(this, prefs);

	_pacsPreferencesGUI->show();
}

void wxMainGui::onSend(wxCommandEvent &event){
	_storeWindow = new wxPacsStoreGui( this, id_Send );
	_storeWindow->setPrefs(prefs);
    _storeWindow->Show(true);
}

void wxMainGui::onAlbumActivated(wxTreeEvent& evt)
{
	wxTreeItemId selectedItem = evt.GetItem();
	if(!selectedItem.IsOk())
		return;

	bool bIsStudy = (_album->GetItemParent(selectedItem) == _album->GetRootItem());
	if(bIsStudy)
	{
		_album->Toggle(selectedItem);	
		return;	// Si dovrebbe comportare così?
	}

	wxArrayString *pInfo = (wxArrayString *)(_album->GetItemDataPtr(selectedItem));
	if(!pInfo)
		return;

	wxWindowDisabler disabler;
	// by Nello per la gestione del progressDialog
	//wxBusyInfo wait("Please wait, loading data ...");
	wxBusyCursor cursor;

	wxArrayString *pPaths = m_pLocalCache->GetFilePaths(pInfo, false);
	if(!pPaths)
		return;

	// Evitiamo che MITO crashi all'apertura di "serie" non previste (PR e SR)
//	if(pPaths->GetCount() == 1)
	{
		wxString sModality = pInfo->Item(MITO_STUDY_MODALITY);
		if((sModality.Find(wxT("SR")) != wxNOT_FOUND) ||
		   (sModality.Find(wxT("PR")) != wxNOT_FOUND))
		{
			wxMessageBox(wxT("The file you are trying to open isn't supported by MITO yet. Operation aborted."), wxT("Warning"), wxICON_WARNING);
			return;
		}
	}

	unsigned int idViewer = getActiveViewer() ;
	unsigned int oldIdViewer = 0;
	if (idViewer) {
		_imageBoxSizer->Detach(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow());
		oldIdViewer = idViewer;
	}

	new2dViewer(new wxVtkViewer2d(this, _imageBoxPanel, id_ImageBox));
	idViewer = getActiveViewer();
	_idPreview = idViewer;
	unsigned int idData = getDataHandler()->newData(getViewer(idViewer)->newAppData(), idViewer);


	// Barra di avanzamento del caricamento serie
	wxProgressDialog * wait = new wxProgressDialog("", "please wait, loading data...", 100, this, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);
	((itkVtkData*)(getDataHandler()->getData(idData)))->setBusyWindow(wait);	
	((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Freeze();				
	int width = 0.2 * this->GetSize().GetWidth();
	wait->SetSize( (width>250 ? width : 250) , 180 );
	wait->Centre();
	//wait->Update(0, "Please wait ...  0%");			
	wait->Update(0, "please wait, loading data...");
	wait->Raise();

	
	if (getDataHandler()->getData(idData)->open(pPaths,true)) 
	{
		if (oldIdViewer)
			onViewerQuit(oldIdViewer);

		// A seconda se si tratta di un file singolo o di una serie,
		// specifichiamo in setName rispettivamente il percorso del file
		// o il path della serie
		if(pPaths->GetCount() == 1)
		{
			getDataHandler()->getData(idData)->setName((const char *)pInfo->Item(MITO_SAVEPATH));
		}else
		{
			wxString filePath((const char *)pInfo->Item(MITO_SAVEPATH));
			wxFileName dirName = wxFileName::DirName(filePath);
			dirName.Normalize();	

			dirName.RemoveLastDir();	// Elimina il nome del file?

			wxString szTemp = dirName.GetFullPath();
			getDataHandler()->getData(idData)->setName((const char *)szTemp);
		}

//		getDataHandler()->getData(idData)->setName((const char *)pPaths->Item(0));
		_imageBoxSizer->Add(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
	
		//By Nello ((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Freeze();

		getViewer(idViewer)->showAppData(getDataHandler()->getData(idData));
		_imageBoxSizer->Layout();
		((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Thaw();
		wxString id;
		id.Printf("%u",idData); 
		itkVtkData *itkVtkData_ = (itkVtkData*)(getDataHandler()->getData(idData));		
		const int slicesNumber = get2dViewer(idViewer)->getSliceNumber(getDataHandler()->getData(idData));
		wxImage* images = itkVtkData_->getThumbnails();
		_thumbnails->Clear();
		_thumbnails->ShowImages(slicesNumber, images);
		_slider->SetRange(0,(slicesNumber-1));
		_slider->SetValue(0);
		_slider->Enable(true);	
		_autoPlay->Enable(true);
		viewTextData();


		// by Nello per la gestione del progressDialog					
		wait->Update(100,"please wait, creating thumbnails...100%");			
		wait->Raise();
	}
	else {
		// by Nello per la gestione del progressDialog
		((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Thaw();
		wait->Hide();

		getDataHandler()->deleteData(idData);
		_imageBoxSizer->Detach(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow());
		quitViewer(idViewer);

		// A.Placitelli: Evitiamo il crash se fallisce l'apertura di una serie
		// e non esiste un old viewer
		if(getViewer(oldIdViewer))
		{
			_imageBoxSizer->Add(((wxVtkViewer2d*)(getViewer(oldIdViewer)->getViewerDrawing()))->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
			this->onViewerActivate(oldIdViewer);
			_idPreview = oldIdViewer;	
		}

		wxMessageDialog messageDialog(this, "Error opening the selected dicom file or directory.", "MITO",wxICON_INFORMATION|wxOK);
		messageDialog.ShowModal();
	}

	// La serie è stata aperta, facciamo si che si noti dall'album!
	if(m_idLastSelectedTreeItem.IsOk())
		_album->SetItemBold(m_idLastSelectedTreeItem, FALSE);

	_album->SetItemBold(selectedItem, TRUE);
	m_idLastSelectedTreeItem = selectedItem;

	_slider->SetFocus();

	delete pPaths;
}


void wxMainGui::onAnonymize(wxCommandEvent &event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			itkVtkData *_itkVtkData = (itkVtkData*)getDataHandler()->getData(idData);

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


void wxMainGui::onExportToDicom(wxCommandEvent &event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			itkVtkData *_itkVtkData = (itkVtkData*)getDataHandler()->getData(idData);

			string inputPath = _itkVtkData->getName();
			string outputPath = "";

			exportFilter _exportFilter(_itkVtkData->getRgb());

			wxFileName wxFileName_((wxString)inputPath.c_str(), wxPATH_NATIVE);
			//if(wxFileName_.DirExists()) {
			if (!wxFileName_.FileExists() && wxFileName_.DirExists()) {
				wxDirDialog *dlgDir = new wxDirDialog(this, "Save series", "", wxSAVE|wxDD_NEW_DIR_BUTTON, wxDefaultPosition);
				if (dlgDir->ShowModal() == wxID_OK) {
					outputPath = dlgDir->GetPath();

					wxWindowDisabler disabler;
					wxBusyInfo wait("Please wait, saving...");
					wxBusyCursor cursor;

					_exportFilter.exportSeriesToDicom(inputPath, outputPath);
					//_exportFilter.exportSeriesToDicom(inputPath, outputPath, _itkVtkData);

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

					//_exportFilter.exportImageToDicom(inputPath, outputPath);
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


void wxMainGui::onExportToBmp(wxCommandEvent &event) {
	unsigned int idViewer = getActiveViewer();
	if (idViewer) {
		unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
		if (idData) {
			itkVtkData *_itkVtkData = (itkVtkData*)getDataHandler()->getData(idData);

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

void wxMainGui::OnExportDicomdir(wxCommandEvent &event)
{
	unsigned int idViewer = getActiveViewer();
	if(!idViewer) 
		return;

	unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
	if(!idData)
		return;

	itkVtkData *_itkVtkData = (itkVtkData*)getDataHandler()->getData(idData);
	if(!_itkVtkData)
		return;

	string inputPath = _itkVtkData->getName();

	wxDirDialog *dlgDir = new wxDirDialog(this, "Choose destination folder", "", wxSAVE|wxDD_NEW_DIR_BUTTON, wxDefaultPosition);
	if (dlgDir->ShowModal() != wxID_OK) 
	{
		dlgDir->Destroy();	
		return;
	}

	string outputPath = dlgDir->GetPath();
	dlgDir->Destroy();	// Ora che abbiamo il path non abbiamo più bisogno del dialog..

	wxFileName inPathWx(wxT(inputPath.c_str()), wxPATH_NATIVE);

	wxArrayString* studyFiles = new wxArrayString();
	if(!studyFiles)
		return;

	wxDir studyPath(inPathWx.GetFullPath());
	if(!studyPath.Exists(inPathWx.GetFullPath()))
	{
		wxMessageBox(wxT("Please, choose a series with more than one image."));
		return;
	}
	
	studyPath.GetAllFiles(inPathWx.GetFullPath(), studyFiles);

	size_t numFiles = studyFiles->GetCount();
	if(!numFiles)
		return;

	// Iniziamo a copiare i files nella cartella di destinazione ed aggiungiamoli alla dicomdir..
	DicomDirInterface ddir;

	// Aggiunge supporto per qualcosa.. ma non sò precisamente per cosa!
	//DicomDirImageImplementation imagePlugin;
    //ddir.addImageSupport(&imagePlugin);

	// DicomDir file path
	wxString sDDirFile = wxString::Format("%s\\%s", outputPath.c_str(), DEFAULT_DICOMDIR_NAME);

	// Il primo parametro di createNewDicomDir indica il tipo di DDIR da creare. I tipi sono:
	// AP_GeneralPurpose, AP_GeneralPurposeDVD, AP_GeneralPurposeMIME, AP_USBandFlash, AP_MPEG2MPatML
	// AP_BasicCardiac, AP_XrayAngiographic, AP_XrayAngiographicDVD, AP_DentalRadiograph, AP_CTandMR
	// AP_UltrasoundIDSF, AP_UltrasoundSCSF, AP_UltrasoundCCSF, AP_UltrasoundIDMF, AP_UltrasoundSCMF, 
	// AP_UltrasoundCCMF, AP_TwelveLeadECG, AP_HemodynamicWaveform
	OFCondition cond = ddir.createNewDicomDir(DicomDirInterface::AP_GeneralPurposeMIME, sDDirFile, DEFAULT_FILESETID);

	for(size_t k = 0; k < numFiles; k++)
	{
		// Ora qui generiamo nomi di files utilizzando ik "k" e con padding sx di zeri
		wxString sFileName = wxString::Format("%08i", k);
		wxString sDestFileName = outputPath + wxT("\\") + sFileName;

		// Copiamo i files nella nuova cartella..
		wxString originalFile = studyFiles->Item(k);
		bool bRet = wxCopyFile(originalFile, sDestFileName, true);

		// Generiamo la dicomdir aggiungendo ad ogni file una voce nella DDIR
		cond = ddir.addDicomFile(sFileName, outputPath.c_str());

		continue;
	}

	cond = ddir.writeDicomDir();

	//this->Raise();
}

void wxMainGui::onResetView(wxCommandEvent &event)
{
	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait, loading...");
	wxBusyCursor cursor;
	
	unsigned int idViewer = getActiveViewer() ;
	unsigned int idData;
	if (idViewer){
		list<unsigned int> *l = getDataHandler()->getIdDataByIdViewer(idViewer);
	
		list<unsigned int>::iterator iteList = l->begin();
		while (iteList != l->end()) {
			idData = *iteList;
			iteList++;
		}
		delete l;
		getDataHandler()->getDataItem(idData)->idViewer = 0;
		_imageBoxSizer->Detach(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow());
		this->quitViewer(idViewer);

		new2dViewer(new wxVtkViewer2d(this, _imageBoxPanel, id_ImageBox));
		idViewer = getActiveViewer();
		_idPreview = idViewer;
		getDataHandler()->getDataItem(idData)->idViewer = idViewer;
			
		_imageBoxSizer->Add(((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow(), 1,  wxEXPAND|wxALIGN_CENTER, 0);
		
		((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Freeze();
		itkVtkData *itkVtkData_ = (itkVtkData*)(getDataHandler()->getData(idData));	
		itkVtkData_->resetVtkData();
		getViewer(idViewer)->showAppData(getDataHandler()->getData(idData));
		_imageBoxSizer->Layout();
		((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->getWxWindow()->Thaw();
		_slider->SetValue(0);
		viewTextData();
		const int slicesNumber = get2dViewer(idViewer)->getSliceNumber(getDataHandler()->getData(idData));
		wxImage* images = itkVtkData_->getThumbnails();
		_thumbnails->Clear();
		_thumbnails->ShowImages(slicesNumber, images);
		this->Raise();
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("Open a DICOM file or folder."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
	}
}

void wxMainGui::onClose(wxCommandEvent& event) {  
	saveAlbum();
	Close(TRUE);
}

void wxMainGui::onQuit(wxCloseEvent& event) {  
	saveAlbum();
	event.Skip();
}

void wxMainGui::OnStoreServerNotification (wxCommandEvent& event)
{
	static wxTreeItemId lastStudy, lastSeries;
	static int iSeriesImagesNum = 0;
	static int iStudyImagesNum = 0;

	// Le due variabili che seguono le useremo per
	// mostrare nel browser di questo thread le serie SOLO
	// quando queste saranno completamente scaricate, di modo
	// da evitare eventuali crash per mancanza di files..
	static wxArrayString *pLastSeriesInfo = NULL,
						 *pInfoToBind = NULL;

	switch(event.GetInt())
	{
	case EVT_MITO_RECEIVED_IMAGE:
		{
			//by Nello
			//Aggiorno eventualmente la _pacsWindow
			if(_pacsWindow)	
				((wxPacsQueryGUI*)_pacsWindow)->onMitoReceivedImage();			

			// Abbiamo finito il download di una nuova immagine
			wxArrayString *pInfo = (wxArrayString*)event.GetClientData();
			if(!pInfo)
				break;

			m_pLocalCache->AddFileToDatabase(pInfo);

			// Incrementiamo il numero di immagini...
			iSeriesImagesNum++;
			iStudyImagesNum++;

			wxASSERT(lastStudy.IsOk());	// && lastSeries.IsOk());

			// Associamo un wxStringArray allo studyItem in modo
			// da avere sempre disponibili UID studio, paziente e serie
			// per le operazioni di SEND e Delete local cache
			if(!_album->GetItemDataPtr(lastStudy))
			{
				_album->SetItemDataPtr(lastStudy, pInfo);
			}
/*
			if(!_album->GetItemDataPtr(lastSeries))
			{
				_album->SetItemDataPtr(lastSeries, pInfo);
			}*/

			// Associamo il pInfo all'elemento nella browser SOLO
			// dopo che questo è stato inserito
			pInfoToBind = pInfo;

			// Non deallochiamo: ci serve per la cache locale ancora..
			//delete pInfo;
		}
		break;

	case EVT_MITO_RECEIVING_NEW_STUDY:
		{
			wxArrayString *pInfo = (wxArrayString*)event.GetClientData();
			if(!pInfo)
				break;

			// Aggiungiamo uno studio all'album
			wxTreeItemId rootItem = _album->GetRootItem();
			if(!rootItem.IsOk())
			{
				rootItem = _album->AddRoot (_T("Root"));
			}

			// Azzeriamo i conteggi
			iStudyImagesNum = 0;
			iSeriesImagesNum = 0;

			wxTreeItemId oldStudy;
			string sStudyUID = pInfo->Item(MITO_STUDY_UID);
			int iResult = searchRecord(sStudyUID, "", oldStudy);
			if((iResult == 1) && oldStudy.IsOk())
			{
				// Lo studio è già presente nella cache locale!
				lastStudy = oldStudy;
			}else
			{
				wxTreeItemId newStudy = addStudyToAlbum(pInfo, rootItem);

				delete pInfo;
				
				lastStudy = newStudy;
			}

		}
		break;

	case EVT_MITO_RECEIVED_STUDY:
		{
			// Abbiamo finito il download di un intero studio
			wxArrayString *pInfo = (wxArrayString*)event.GetClientData();
			if(!pInfo)
				break;
			
			// Aggiorniamo il conteggio delle immagini nello studio..			
			if(lastStudy.IsOk())
			{
				_album->SetItemText (lastStudy, 8, wxString::Format(wxT("%i"), iStudyImagesNum));	// # images
			}

			delete pInfo;
		}
		break;

	case EVT_MITO_RECEIVING_NEW_SERIES:
		{
			wxArrayString *pInfo = (wxArrayString*)event.GetClientData();
			if(!pInfo)
				break;

			wxASSERT(_album->GetRootItem().IsOk());

			pLastSeriesInfo = pInfo;
		}
		break;

	case EVT_MITO_RECEIVED_SERIES:
		{
			//by Nello
			//Aggiorno eventualmente la _pacsWindow
			if(_pacsWindow)	
				((wxPacsQueryGUI*)_pacsWindow)->onMitoReceivedSeries();	


			// Abbiamo finito il download di una nuova serie
			wxArrayString *pUseless = (wxArrayString*)event.GetClientData();
			if(pUseless)
			{
				delete pUseless;
				pUseless = NULL;
			}

			if(!pLastSeriesInfo)
				break;

			wxArrayString *pInfo = pLastSeriesInfo;

			wxTreeItemId newSeries = addSeriesToAlbum(pInfo, lastStudy, iSeriesImagesNum);

			// Non dimentichiamoci di liberare la memoria allocata :)
			delete pLastSeriesInfo;
			pLastSeriesInfo = NULL;

			lastSeries = newSeries;

			// Azzeriamo i conteggi
			iSeriesImagesNum = 0;

			if(!_album->GetItemDataPtr(lastSeries))
			{
				_album->SetItemDataPtr(lastSeries, pInfoToBind);
			}

			// Apriamo la prima serie scaricata se necessario
			if(!m_bOpenedFirstSeries)
			{
				wxTreeEvent evt;
				evt.SetItem(lastSeries);
				onAlbumActivated(evt);
				m_bOpenedFirstSeries = true;
			}
		}
		break;

	case EVT_MITO_RECEIVE_NOMORE:
		{
			// Nel caso non sia giunto un messaggio del tipo "END_SERIES", finalizziamo qui l'inserimento
			// dell'ultima serie
			if(!pLastSeriesInfo)
				break;

			wxArrayString *pInfo = pLastSeriesInfo;
			
			wxTreeItemId newSeries = addSeriesToAlbum(pInfo, lastStudy, iSeriesImagesNum);

			// Aggiorniamo il conteggio delle immagini nello studio..			
			if(lastStudy.IsOk())
			{
				_album->SetItemText (lastStudy, 8, wxString::Format(wxT("%i"), iStudyImagesNum));	// # images
			}

			// Non dimentichiamoci di liberare la memoria allocata :)
			delete pLastSeriesInfo;
			pLastSeriesInfo = NULL;

			lastSeries = newSeries;

			iSeriesImagesNum = 0;

			if(!_album->GetItemDataPtr(lastSeries))
			{
				_album->SetItemDataPtr(lastSeries, pInfoToBind);
			}

			// Ricoloriamo
			_album->DoAlternateRowColors();

			// Apriamo la prima serie scaricata se necessario
			if(!m_bOpenedFirstSeries)
			{
				wxTreeEvent evt;
				evt.SetItem(lastSeries);
				onAlbumActivated(evt);
				m_bOpenedFirstSeries = false;	// in questo caso è false, dato che la trasmissione è stata chiusa
			}
		}
		break;

	default:
		break;
	}
}

void wxMainGui::OnQueryGUINotification (wxCommandEvent& event)
{
	if(event.GetInt() == EVT_MITO_RECEIVE_START)
	{
		m_bOpenedFirstSeries = false;
	}else
	{	
		// La gui della query ci ha notificato che sta per chiudersi
		_pacsWindow = NULL;
	}
}

void wxMainGui::OnAlbumItemRightClick(wxTreeEvent &event)
{
	wxMenu Menu;
    Menu.Append(m_contextSendToPACS, _("Send to PACS"));
    Menu.Append(m_contextDeleteLocal, _("Delete local copy"));

	wxPoint p = event.GetPoint();
	p = _album->ClientToScreen(p);

    PopupMenu(&Menu, p);	
}

void wxMainGui::OnContextSendToPACS(wxCommandEvent& event)
{
	const wxTreeItemId selectedItem = _album->GetSelection();
	if(!selectedItem.IsOk())
		return;

	wxArrayString *pInfo = (wxArrayString *)(_album->GetItemDataPtr(selectedItem));
	if(!pInfo)
		return;

	if(wxMessageBox("Are you sure you want to send the selected series to the PACS server?", "Confirm", wxYES_NO | wxICON_QUESTION ) == wxNO)	
	{
		return;
	}

	const wxTreeItemId itemParent = _album->GetItemParent(selectedItem);
	wxASSERT(itemParent.IsOk());

	wxWindowDisabler disabler;
	wxBusyInfo wait("Please wait while your chosen PACS server receives the data.");
	wxBusyCursor cursor;

	bool bIsStudy = (itemParent == _album->GetRootItem());

	wxArrayString *pPaths = m_pLocalCache->GetFilePaths(pInfo, bIsStudy);
	if(!pPaths)
		return;

	size_t numToSend = pPaths->GetCount();
	for(size_t k = 0; k < numToSend; k++)
	{
		if(!store(pPaths->Item(k)))
		{
			if(wxMessageBox("There was an error while sending a file to the PACS server. Do you want to skip the error and continue?", "Warning", wxYES_NO | wxICON_EXCLAMATION ) == wxNO)	
			{
				delete pPaths;
				return;
			}
		}
	}

	delete pPaths;
}

void wxMainGui::OnContextDeleteLocal(wxCommandEvent& event)
{
	wxTreeItemId selectedItem = _album->GetSelection();
	if(!selectedItem.IsOk())
		return;

	bool  bDeleteFromHDD = false;
	int answer = wxMessageBox("Do you also want to delete the files from your hard drive? (NO will just delete them from the Album)", "Confirm", wxYES_NO | wxCANCEL);
	if(answer == wxYES)
		bDeleteFromHDD = true;
	else if(answer == wxNO)
		bDeleteFromHDD = false;
	else 
		return;

	// Se la serie che stiamo per cancellare è la serie che è aperta, o lo studio
	// che la contiene, allora chiudiamo la serie prima di cancellarla.
	wxTreeItemId tmpChild;
	wxTreeItemIdValue lCookie = 0L;
	tmpChild = _album->GetFirstChild(selectedItem, lCookie);
	while(tmpChild.IsOk())
	{
		if(tmpChild == m_idLastSelectedTreeItem)
			closeActiveSeries();

		tmpChild = _album->GetNextSibling(tmpChild);
	}

	if(selectedItem == m_idLastSelectedTreeItem)
		closeActiveSeries();

	deleteAlbumEntry(selectedItem, bDeleteFromHDD);
}

void wxMainGui::savePrefs(char *_file) {
  // Save preferences as

	char output[F_LEN];

	if (_file)
		strcpy(prefsName, _file);

	sprintf(output, "\nSaving preferences as %s...", prefsName);
	writeOutput(output);
	prefs->write(prefsName);
}

void wxMainGui::saveAlbum() 
{	
	const char *filename = "album.xml";
	
	if(!m_pLocalCache->SaveIndexToFile(filename))
	{
		wxMessageBox(wxT("Unable to save local database cache to album.xml"), wxT("MITO Error"));
	}
}

void wxMainGui::loadAlbum() 
{
	const char *filename = "album.xml";

	if(!m_pLocalCache->LoadIndexFromFile(filename, _album))
	{
		//wxMessageBox(wxT("Unable to load local database cache from album.xml"), wxT("MITO Error"));
		return;
	}

	_album->DoAlternateRowColors();
}

void wxMainGui::viewTextData(){
	unsigned int idViewer =getActiveViewer();
	if ((getViewer(idViewer)) && strcmp(typeid(*getViewer(idViewer)).name(),"class viewer2d")==0)
	{
		if(idViewer){
			unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
			if (idData) {
				infoFilter iF(idData, getDataHandler(), get2dViewer(idViewer)->getCurrentSlice());
				int x = iF.getCurrentX(getViewer(idViewer)->getXPos());
				int y = iF.getCurrentY(getViewer(idViewer)->getYPos());

				((wxVtkViewer2d*)(getViewer(idViewer)->getViewerDrawing()))->setText(iF,x,y);
				getViewer(idViewer)->updateViewer();	
			}
		}
	}
	else if ((getViewer(idViewer)) && strcmp(typeid(*getViewer(idViewer)).name(),"class viewer3d")==0) {
		if(idViewer){
			unsigned int idData = getViewer(idViewer)->getActiveIdSingleData();
			wxVtkViewer3d* viewer3d = ((wxVtkViewer3d*)(getViewer(idViewer)->getViewerDrawing()));
			if (idData && viewer3d->getAlgorithm() == rayCast && ((appWxVtkInteractor*)(viewer3d->getWxWindow()))->getWlWwModified()) {
				((appWxVtkInteractor*)(viewer3d->getWxWindow()))->setWlWwModified(false);
				viewer3d->setText();
				getViewer(idViewer)->updateViewer();	
			}
		}
	}
}

int wxMainGui::searchRecord(string sStudyUID, string sSeriesUID, wxTreeItemId &result)
{
	if(!_album->GetRootItem())
		return 0;

	wxTreeItemIdValue cookie;
	wxTreeItemIdValue subCookie;
	wxTreeItemId subItem = _album->GetFirstChild(_album->GetRootItem(), cookie);

	while(subItem)
	{
		// Alterniamo il colore dei records..
		wxArrayString *pStudyInfo = (wxArrayString *)_album->GetItemDataPtr(subItem);
		if(pStudyInfo && (pStudyInfo->Item(4) == sStudyUID))
		{
			// Verifichiamo, se stiamo cercando una serie specifica, che questa
			// sia in questo studio.
			if(!sSeriesUID.size())
			{
				result = subItem;
				return 1;	// Non stiamo cercando una serie in particolare.. ritorniamo l'elemento studio
			}else
			{
				// Abbiamo specificato anche un seriesUID, quindi verifichiamo che in
				// questo studio vi sia quella serie.
				wxTreeItemId subSeries = _album->GetFirstChild(subItem, subCookie);
				while(subSeries)
				{
					wxArrayString *pSeriesInfo = (wxArrayString*)_album->GetItemDataPtr(subSeries);
					if(pSeriesInfo && (pSeriesInfo->Item(9) == sSeriesUID))
					{
						result = subSeries;	// Ritorniamo sempre l'elemento top-level?
						return 2;
					}

					subSeries = _album->GetNextChild(subItem, subCookie);
				}
	
			}
		}
			
		subItem = _album->GetNextChild (_album->GetRootItem(), cookie);
	}	

	return 0;
}

bool wxMainGui::onViewerActivate(unsigned int idViewer) {
	_idActiveViewer = idViewer;
	return true;
}

bool wxMainGui::onViewerQuit(unsigned int idViewer) {
	unsigned int idData;
	list<unsigned int> *l = getDataHandler()->getIdDataByIdViewer(idViewer);
	
	list<unsigned int>::iterator iteList = l->begin();
	while (iteList != l->end()) {
		idData = *iteList;
		iteList++;
	}
	delete l;
	if (quitViewer(idViewer)) {
		getDataHandler()->deleteData(idData);
		return true;
	}
	else
		return false;
}

void wxMainGui::onShowAboutDialog(wxCommandEvent& WXUNUSED(event)) {
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




void wxMainGui::onImageFusion(wxCommandEvent& event) {
	fusionFilter::fusionType _algoType;
	fusionFilter::ruleType _ruleType;
	wxFusionDialog dlg(this);
	wxArrayString paths,names;

	list<wxArrayString*> cacheList = m_pLocalCache->GetList();
	list<wxArrayString*>::iterator iteList = cacheList.begin();

	wxString sLastSeriesUID, 
			 sSeriesLabel,
			 sTemp;

	int count = 0;
	while (iteList != cacheList.end()) 
	{
		wxArrayString *pData = *iteList;
		wxASSERT(pData);

		if(pData)
		{
			sTemp = pData->Item(MITO_SERIES_UID);
			if(sTemp != sLastSeriesUID)
			{
				// Generiamo l'etichetta da mostrare nel box relativamente alla serie corrente
				sSeriesLabel = pData->Item(MITO_SERIES_DESCRIPTION) + wxT("(") + pData->Item(MITO_PATIENT_NAME) + wxT(")");
				names.Add(sSeriesLabel);

				// Estraiamo il path della serie dal nome di un file della serie
				wxFileName dirName = wxFileName::DirName(pData->Item(MITO_SAVEPATH));
				dirName.RemoveLastDir();
				dirName.Normalize();
				
				paths.Add(dirName.GetPath());

				sLastSeriesUID = sTemp;

				count++;		
			}
		}

		++iteList;
	}

	dlg.setListData(names);
	
	unsigned int idData0, selIndex = 0;
	fusionFilter::LivelloDec levdec=0;
	fusionFilter::SizeWindow sizwin=0;
	fusionFilter::WaveletType waveletType=0;
	
	float peso1, peso2;
	if (paths.Count()) {
		if (dlg.ShowModal() == wxID_OK) {
			wxWindowDisabler disabler;
			wxBusyInfo wait("Please wait, computing image fusion...");
			wxBusyCursor cursor;

			wx2dGui *w = new wx2dGui(_T("MITO: 2D Viewer"), wxPoint(0, 0), wxGetClientDisplayRect().GetSize());
			
			int idData1,idData2;
			idData1 = dlg.getIdData1();
			idData2 = dlg.getIdData2();
			std::string path1,path2;
			path1 = paths.Item(idData1);
			path2 = paths.Item(idData2);
			
			itkVtkData im1;
			itkVtkData im2;
			im1.open(path1,true);
			im2.open(path2,true); 

			peso1 = dlg.getPeso1();
			peso2 = dlg.getPeso2();
		
			_algoType = dlg.getFusionType();
			_ruleType = dlg.getFusionRule();
			
			if(_ruleType!=fusionFilter::Sostituzione_dettaglio) //nel caso generale (no sost dett.) il livello di decomposizione è fissato ad 1
				levdec=0;
			else levdec = dlg.getLevel();

			sizwin = 2; //nb si è scelto per default una finestra 5x5 (cioè una finestra quadrata di raggio due)
			waveletType = dlg.getWaveletType();
			
			//Controllo necessario nel caso di mancata selezione, poichè viene restituito un valore -inf
			if (waveletType<0) 
				waveletType=0;

			if (levdec<0) 
				levdec=0;
			
			if (_ruleType<0) 
				_ruleType=fusionFilter::Sostituzione_dettaglio;

			bool applyClut = dlg.getApplyClut();

			new2dViewer(w->new2dViewer(this));
			unsigned int idViewer = getActiveViewer();
			itkVtkData * ris = new itkVtkData();
			idData0 = getDataHandler()->newData(ris, idViewer);

			fusionFilter ff(path1, path2, idData0, getDataHandler());
			ff.computeFusion(peso1, peso2 ,0, _algoType, applyClut, levdec, _ruleType, sizwin, waveletType);
			ris = (itkVtkData*)getDataHandler()->getData(idData0);
			ris->setName("Image Fusion");
			//ris->setWl(140.0);
			//ris->setWw(210.0);
			w->setIdViewer(idViewer);
			w->show(ris);
		}
	}
	dlg.Destroy();
}