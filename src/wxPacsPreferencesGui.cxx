/**
 * \file wxPacsPreferencesGui.cxx
 * \brief File per la creazione del dialog per la scelta dei parametri di connessione al PACS
 * \author ICAR-CNR Napoli
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "preferences.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#include "wxPacsPreferencesGui.h"
#include "pacsStoreSCP.h"
#include "wxMainGui.h"
#include <string>

using namespace std;


/*
 * wxPacsPreferencesGui type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxPacsPreferencesGui, wxDialog )

/*
 * wxPacsPreferencesGui event table definition
 */

BEGIN_EVENT_TABLE( wxPacsPreferencesGui, wxDialog )

//wxPacsPreferencesGui event table entries
    EVT_CLOSE( wxPacsPreferencesGui::OnCloseWindow )

    EVT_BUTTON( cancel_button, wxPacsPreferencesGui::OnCancel )
    EVT_BUTTON( ok_button, wxPacsPreferencesGui::OnAccept )
    EVT_BUTTON( help_button1, wxPacsPreferencesGui::OnHelp1 )
    EVT_BUTTON( help_button2, wxPacsPreferencesGui::OnHelp2 )
    EVT_BUTTON( help_button3, wxPacsPreferencesGui::OnHelp3 )
    EVT_BUTTON( browse_button, wxPacsPreferencesGui::OnBrowse )

//wxPacsPreferencesGui event table entries

END_EVENT_TABLE()

/*
 * wxPacsPreferencesGui constructors
 */

wxPacsPreferencesGui::wxPacsPreferencesGui()
{
}

wxPacsPreferencesGui::wxPacsPreferencesGui(wxWindow* parent, pacsPreferences *_prefs, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Create(parent, _prefs, id, caption, pos, size, style);
}

/*
 * wxPacsPreferencesGui distructor
 */
wxPacsPreferencesGui::~wxPacsPreferencesGui()
{
}

/*!
 * wxPacsPreferencesGui creator
 */

bool wxPacsPreferencesGui::Create( wxWindow* parent, pacsPreferences *_prefs, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );
	
	prefs = _prefs;
	current = NULL;

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
    return true;
}

/*
 * Control creation for wxPacsPreferencesGui
 */

void wxPacsPreferencesGui::CreateControls()
{    
    preferencesDialog = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    preferencesDialog->SetSizer(itemBoxSizer2);

    wxNotebook* itemNotebook3 = new wxNotebook( preferencesDialog, notebook2, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

    wxPanel* itemPanel4 = new wxPanel( itemNotebook3, notebook2_panel1, wxDefaultPosition, wxSize(250, 280), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel4->SetSizer(itemBoxSizer5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer5->Add(itemBoxSizer6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel4, wxID_STATIC, _("&Server Node"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    serverInput = new wxTextCtrl( itemPanel4, panel1_textCtrl1, _T(""), wxDefaultPosition, wxSize(220, -1), 0 );
    itemBoxSizer6->Add(serverInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxStaticText* itemStaticText9 = new wxStaticText( itemPanel4, wxID_STATIC, _("&Server Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText9, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    portInput = new wxTextCtrl( itemPanel4, panel1_textCtrl2, _T(""), wxDefaultPosition, wxSize(220, -1), 0 );
    itemBoxSizer6->Add(portInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel4, wxID_STATIC, _("&Called AP Title"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    calledAPInput = new wxTextCtrl( itemPanel4, panel1_textCtrl3, _T(""), wxDefaultPosition, wxSize(220, -1), 0 );
    itemBoxSizer6->Add(calledAPInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel4, wxID_STATIC, _("&Calling AP Title"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    callingAPInput = new wxTextCtrl( itemPanel4, panel1_textCtrl4, _T(""), wxDefaultPosition, wxSize(220, -1), 0 );
    itemBoxSizer6->Add(callingAPInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel4, wxID_STATIC, _("&Storage AP Title"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(itemStaticText15, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    storageAPInput = new wxTextCtrl( itemPanel4, panel1_textCtrl5, _T(""), wxDefaultPosition, wxSize(220, -1), 0 );
    itemBoxSizer6->Add(storageAPInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    itemNotebook3->AddPage(itemPanel4, _("&Network"));

    wxPanel* itemPanel17 = new wxPanel( itemNotebook3, notebook2_panel2, wxDefaultPosition, wxSize(250, 280), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel17->SetSizer(itemBoxSizer18);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer18->Add(itemBoxSizer19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText20 = new wxStaticText( itemPanel17, wxID_STATIC, _("&Storage Server Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(itemStaticText20, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    receiveStorageInput = new wxTextCtrl( itemPanel17, panel2_textCtrl1, _T(""), wxDefaultPosition, wxSize(220, -1), 0 );
    itemBoxSizer19->Add(receiveStorageInput, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer22, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    //receiveStorageExecButton = new wxCheckBox( itemPanel17, start_button, _("&Start/Stop automatically"), wxDefaultPosition, wxDefaultSize, 0 );
    //receiveStorageExecButton->SetValue(false);
    //itemBoxSizer22->Add(receiveStorageExecButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton24 = new wxButton( itemPanel17, help_button1, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemButton24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText25 = new wxStaticText( itemPanel17, wxID_STATIC, _("&Receive/Local Files Directory"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(itemStaticText25, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    receiveDirectoryInput = new wxTextCtrl( itemPanel17, panel2_textCtrl2, _T(""), wxDefaultPosition, wxSize(220, -1), wxTE_READONLY  );
    itemBoxSizer19->Add(receiveDirectoryInput, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer27, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxButton* itemButton28 = new wxButton( itemPanel17, help_button2, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer27->Add(itemButton28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton29 = new wxButton( itemPanel17, browse_button, _("&Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer27->Add(itemButton29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText30 = new wxStaticText( itemPanel17, wxID_STATIC, _("&Max PDU"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(itemStaticText30, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    maxPDU = new wxTextCtrl( itemPanel17, panel2_textCtrl3, _T(""), wxDefaultPosition, wxSize(220, -1), 0 );
    itemBoxSizer19->Add(maxPDU, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* itemButton32 = new wxButton( itemPanel17, help_button3, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(itemButton32, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    itemNotebook3->AddPage(itemPanel17, _("&Transfer/Receive"));

    itemBoxSizer2->Add(itemNotebook3, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer33, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton34 = new wxButton( preferencesDialog, ok_button, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer33->Add(itemButton34, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton35 = new wxButton( preferencesDialog, cancel_button, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer33->Add(itemButton35, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

}

/*
 * Should we show tooltips?
 */

bool wxPacsPreferencesGui::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap wxPacsPreferencesGui::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
}

/*
 * Get icon resources
 */

wxIcon wxPacsPreferencesGui::GetIconResource( const wxString& name )
{
    // Icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
}


void wxPacsPreferencesGui::initSettings() {
  // initialize current settings
	if (current) {

		// items in the Network group
		serverInput->SetValue(current->server);
		calledAPInput->SetValue(current->calledAP);
		callingAPInput->SetValue(current->callingAP);
		storageAPInput->SetValue(current->storageAP);
		portInput->SetValue(current->serverPort);

		// items in the Receive group
		receiveStorageInput->SetValue(current->port);
		receiveDirectoryInput->SetValue(current->localDirectory);
		maxPDU->SetValue(current->maxPDU);
	}
	else {
		serverInput->SetValue("");
		calledAPInput->SetValue("");
		callingAPInput->SetValue("");
		storageAPInput->SetValue("");
		portInput->SetValue("");

		// items in the Receive group
		receiveStorageInput->SetValue("");
		receiveDirectoryInput->SetValue("");
		maxPDU->SetValue("");
	}
}

void wxPacsPreferencesGui::saveSettings() {
  // save fields to current

	// items in the Network group
	strcpy(current->server, serverInput->GetValue().c_str());
	strcpy(current->calledAP, calledAPInput->GetValue().c_str());
	strcpy(current->callingAP, callingAPInput->GetValue().c_str());
	strcpy(current->storageAP, storageAPInput->GetValue().c_str());
	strcpy(current->serverPort, portInput->GetValue().c_str());

	// items in the Receive group
	strcpy(current->port, receiveStorageInput->GetValue().c_str());
	//current->storageExec = receiveStorageExecButton->GetValue();
	strcpy(current->localDirectory, receiveDirectoryInput->GetValue().c_str());
	strcpy(current->maxPDU, maxPDU->GetValue().c_str());

	strcpy(current->storageServer,"");
	strcpy(current->storeClient,"send_image -a ");
	strcat(current->storeClient,current->callingAP);
	strcat(current->storeClient," -c ");
	strcat(current->storeClient,current->calledAP);
	strcat(current->storeClient," -m ");
	strcat(current->storeClient,current->maxPDU);
	strcat(current->storeClient," ");
	strcat(current->storeClient,current->server);
	strcat(current->storeClient," ");
	strcat(current->storeClient,current->serverPort);
	//strcat(current->storeClient," > ");
	//strcat(current->storeClient,current->logFile);
}


void wxPacsPreferencesGui::show() {
	// create our working copy
	current = new pacsPreferences(prefs);

	// set up the data for editing
	initSettings();

	// show the window
	preferencesDialog->Show(true);
}

void wxPacsPreferencesGui::quit(int _save) {

	// save indicates if current should replace prefs
	if (current != NULL) 
	{
		if (_save == 1) 
		{
			memcpy(prefs, current, sizeof(pacsPreferences));
			delete current;
			current = NULL;
		
			// Alcune impostazioni richiedono il riavvio del server SCP
			pacsStoreSCP *pServer = ((wxMainGui*)GetParent())->getPacsStoreServer();
			if(pServer)
			{
				// close the gui
				//preferencesDialog->Show(false);
				pServer->Stop();
				pServer->Start();
			}
		}
		else {
			delete current;
			current = NULL;
		}
	}
	// close the gui
	preferencesDialog->Show(false);
}

void wxPacsPreferencesGui::browse(wxTextCtrl* input) {

	string fileName=".";
	wxDirDialog *dlg = new wxDirDialog(this, "Select a directory", ".");
	if (dlg->ShowModal() == wxID_OK)
			fileName = dlg->GetPath().c_str();
	dlg->Destroy();
	input->SetValue(fileName.c_str());		
}


void wxPacsPreferencesGui::OnCancel( wxCommandEvent& event )
{
   quit(0);
}

void wxPacsPreferencesGui::OnAccept( wxCommandEvent& event )
{
    saveSettings();
	quit(1);
}

void wxPacsPreferencesGui::OnHelp1( wxCommandEvent& event )
{
	 wxMessageBox(_("Please enter the storage server port."));
}

void wxPacsPreferencesGui::OnHelp2( wxCommandEvent& event )
{
	 wxMessageBox(_("Enter the directory to be used by default for browsing local DICOM objects and storing images after transfer."));
}

void wxPacsPreferencesGui::OnHelp3( wxCommandEvent& event )
{
	 wxMessageBox(_("Please enter the Maximum PDU for find, move and store operations."));
}

void wxPacsPreferencesGui::OnBrowse( wxCommandEvent& event )
{
	browse(receiveDirectoryInput);
}


void wxPacsPreferencesGui::OnCloseWindow( wxCloseEvent& event )
{
    quit(0);
}


