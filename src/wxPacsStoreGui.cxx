/**
 * \file wxPacsStoreGui.cxx
 * \brief File per la creazione del dialog per l'invio di immagini al PACS
 * \author ICAR-CNR Napoli
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxPacsStoreGui.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <string>
#include <iostream>

using namespace std;


#include "wxPacsStoreGui.h"
#include "wxMainGui.h"
#include <wx/listctrl.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/busyinfo.h>


/*
 * wxPacsStoreGui type definition
 */

IMPLEMENT_CLASS( wxPacsStoreGui, wxFrame )

/*
 * wxPacsStoreGui event table definition
 */

BEGIN_EVENT_TABLE( wxPacsStoreGui, wxFrame )

    EVT_CLOSE( wxPacsStoreGui::OnCloseWindow )
	/*
	EVT_MENU( _menu_preferences, wxPacsStoreGui::OnSetPreferences )
    EVT_MENU( _menu_open, wxPacsStoreGui::OnOpenPreferences )
    EVT_MENU( _menu_save, wxPacsStoreGui::OnSavePreferences )
	EVT_MENU( _menu_saveas, wxPacsStoreGui::OnSaveasPreferences )
    EVT_MENU( _menu_exit, wxPacsStoreGui::OnExit)
    EVT_MENU( _menu_automatically_save_preferences, wxPacsStoreGui::OnAutoSavePrefs)*/
    EVT_BUTTON( _send_button, wxPacsStoreGui::OnFileGo)
    EVT_BUTTON( _file_button, wxPacsStoreGui::OnFileBrowse)
    EVT_BUTTON( _dir_button, wxPacsStoreGui::OnDirBrowse)
    EVT_BUTTON( _cancel_button, wxPacsStoreGui::OnExit)

END_EVENT_TABLE()

/*
 * wxPacsStoreGui constructors
 */

 wxPacsStoreGui::wxPacsStoreGui( ) : pacsCommunicator()
{
	/*wxIcon icon;
	icon.LoadFile("icon.ico",wxBITMAP_TYPE_ICO);
	this->SetIcon(icon);*/
	SetIcon(wxICON(MITO));
}

wxPacsStoreGui::wxPacsStoreGui( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) : pacsCommunicator()
{
	Create( parent, id, caption, pos, size, style );

	/*wxIcon icon;
	icon.LoadFile("icon.ico",wxBITMAP_TYPE_ICO);
	this->SetIcon(icon);*/
	SetIcon(wxICON(MITO));
}


/*
 * wxPacsStoreGui creator
*/

bool wxPacsStoreGui::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	wxFrame::Create( parent, id, caption, pos, size, style );
	wxFrame::MakeModal(true);
    CreateControls();
    Centre();

	//prefsUI = new wxPacsPreferencesGui(this,prefs);
	setOutput(outputBrowser);

    return true;
}


void wxPacsStoreGui::CreateControls()
{    
    wxPacsStoreGui* window = this;
	window->SetSizeHints(400,330);
/*
	//Menubar
	wxMenuBar* menuBar = new wxMenuBar;
    
	//Item Options
	options = new wxMenu;
    options->Append(_menu_preferences, _("Preferences"), _T(""), wxITEM_NORMAL);
    options->AppendSeparator();
    options->Append(_menu_automatically_save_preferences, _("Auto Save Preferences on Exit"), _T(""), wxITEM_CHECK);
    options->Check(_menu_automatically_save_preferences, true);
	if (prefs->autoSave)
		options->Check(_menu_automatically_save_preferences, true);
	else
		options->Check(_menu_automatically_save_preferences, false);
    options->AppendSeparator();
    options->Append(_menu_open, _("Open Preferences"), _T(""), wxITEM_NORMAL);
    options->Append(_menu_save, _("Save Preferences"), _T(""), wxITEM_NORMAL);
    options->Append(_menu_saveas, _("Save Preferences as..."), _T(""), wxITEM_NORMAL);
    options->Append(_menu_exit, _("Exit"), _T(""), wxITEM_NORMAL);
    menuBar->Append(options, _("Options"));

	window->SetMenuBar(menuBar);*/

	wxPanel* panel = new wxPanel( window, _center_panel, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

    wxPanel* topPanel = new wxPanel( panel, _top_panel, wxPoint(10, 10), wxSize(340, 150), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	wxStaticText* itemStaticText19 = new wxStaticText( topPanel, wxID_STATIC,_("&Actions"), wxDefaultPosition, wxSize(80, 20), wxALIGN_CENTRE|wxRAISED_BORDER|wxWANTS_CHARS );
	wxPanel* topInnerPanel = new wxPanel( topPanel, _top_inner_panel, wxDefaultPosition, wxSize(240, 130), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	
    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxVERTICAL);
	wxStaticText* itemStaticText36 = new wxStaticText( topInnerPanel, wxID_STATIC, _("&File or Directory to Send"), wxDefaultPosition, wxDefaultSize, 0 );
    directoryInput = new wxTextCtrl( topInnerPanel, _send_text, _T(""), wxDefaultPosition, wxSize(200, 20), 0 );
    itemBoxSizer35->Add(itemStaticText36, 0, wxALIGN_LEFT|wxALL, 5);
    itemBoxSizer35->Add(directoryInput, 0, wxEXPAND|wxALL, 5);

    wxBoxSizer* itemBoxSizer38 = new wxBoxSizer(wxVERTICAL);
    wxButton* itemButton39 = new wxButton( topInnerPanel, _dir_button, _("&Choose a Series"), wxDefaultPosition, wxDefaultSize, 0 );
    wxButton* itemButton40 = new wxButton( topInnerPanel, _file_button, _("&Choose an Image"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer38->Add(itemButton39, 0, wxEXPAND|wxALL, 5);
    itemBoxSizer38->Add(itemButton40, 0, wxEXPAND|wxALL, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer34->Add(itemBoxSizer38, 0, wxEXPAND|wxALL, 5);
    itemBoxSizer34->Add(itemBoxSizer35, 0, wxEXPAND|wxALL, 5);


	wxBoxSizer* itemBoxSizer41 = new wxBoxSizer(wxHORIZONTAL);
    wxButton* sendButton = new wxButton( topInnerPanel, _send_button, _("&Send"), wxDefaultPosition, wxDefaultSize, 0 );
	wxButton* cancelButton = new wxButton( topInnerPanel, _cancel_button, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer41->Add(sendButton, 0, wxEXPAND|wxALL, 5);
    itemBoxSizer41->Add(cancelButton, 0, wxEXPAND|wxALL, 5);

	wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer33->Add(itemBoxSizer34, 0, wxALIGN_CENTER|wxALL, 5);
	itemBoxSizer33->Add(itemBoxSizer41, 0, wxALIGN_CENTER|wxALL, 5);

	topInnerPanel->SetAutoLayout(true);
	topInnerPanel->SetSizer(itemBoxSizer33);
    
	outputBrowser = new wxTextCtrl( topPanel, _bottom_textCtrl, _("Welcome"), wxDefaultPosition, wxSize(350, 30), wxTE_MULTILINE|wxTE_READONLY );

	wxBoxSizer* sub1 = new wxBoxSizer(wxVERTICAL);

	sub1->Add(itemStaticText19, 0, wxEXPAND|wxALL, 5);
 	sub1->Add(topInnerPanel, 0, wxEXPAND|wxALL, 5);
 	sub1->Add(outputBrowser, 1, wxEXPAND|wxALL, 5);

	topPanel->SetAutoLayout(true);
	topPanel->SetSizer(sub1);
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(topPanel, 1,  wxALL| wxEXPAND , 5);

	panel->SetAutoLayout( true );
	panel->SetSizer(topSizer);
    
}


bool wxPacsStoreGui::ShowToolTips()
{
    return true;
}


wxBitmap wxPacsStoreGui::GetBitmapResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon wxPacsStoreGui::GetIconResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullIcon;
}

void wxPacsStoreGui::OnFileGo( wxCommandEvent& event ) 
{
	// Verifichiamo se è un file o una cartella. Nel secondo caso, inviamo
	// file per file tutti i file della cartella.
	wxString sPath = directoryInput->GetValue();
	if(sPath.empty())
	{
		wxMessageDialog messageDialog(this, "Choose a file or directory.", "MITO",wxICON_INFORMATION|wxOK);
		messageDialog.ShowModal();
		return;
	}

	// E' un file
	wxString sMsg;
	if(wxFileName::FileExists(sPath))
	{
		if(store(sPath.c_str()))
		{
			sMsg.Printf("%s was sent successfully\n", sPath.c_str());
			outputBrowser->AppendText(sMsg);
		}else
		{
			sMsg.Printf("There was an error while sending %s.\n", sPath.c_str());
			outputBrowser->AppendText(sMsg);
		}
	}else if(wxFileName::DirExists(sPath))
	{
		// E' una cartella
		wxArrayString* seriesFiles = new wxArrayString();
		if(!seriesFiles)
			return;		
		
		wxDir dirName(sPath);

		dirName.GetAllFiles(sPath.c_str(), seriesFiles);

		size_t numFiles = seriesFiles->GetCount();
		if(!numFiles)
			return;

		for(size_t k = 0; k < numFiles; k++)
		{
			if(store(seriesFiles->Item(k).c_str()))
			{
				sMsg.Printf("%s was sent successfully\n", seriesFiles->Item(k).c_str());
				outputBrowser->AppendText(sMsg);
			}else
			{
				sMsg.Printf("There was an error while sending %s.\n", seriesFiles->Item(k).c_str());
				outputBrowser->AppendText(sMsg);

				if(wxMessageBox("There was an error while sending a file to the PACS server. Do you want to skip the error and continue?", "Warning", wxYES_NO | wxICON_EXCLAMATION ) == wxNO)	
				{
					delete seriesFiles;
					seriesFiles = NULL;
					return;
				}
			}
		}

		delete seriesFiles;
		seriesFiles = NULL;
	}
}


void wxPacsStoreGui::OnFileBrowse( wxCommandEvent& event ) {
		
	string fileName=".";
	wxFileDialog *dlg = new wxFileDialog(this, "Choose a file", "", "", "", wxOPEN, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK)
			fileName = dlg->GetPath().c_str();
	dlg->Destroy();
	directoryInput->SetValue(fileName.c_str());

}

void wxPacsStoreGui::OnDirBrowse( wxCommandEvent& event ) {
	string fileName=".";
	wxDirDialog *dlg = new wxDirDialog(this, "Choose a directory", ".");
	if (dlg->ShowModal() == wxID_OK)
			fileName = dlg->GetPath().c_str();
	dlg->Destroy();
	directoryInput->SetValue(fileName.c_str());
}

void wxPacsStoreGui::OnExit( wxCommandEvent& event )
{
	this->Close(true);
}

void wxPacsStoreGui::OnCloseWindow( wxCloseEvent& event )
{
	wxFrame::MakeModal(false);
	//prefsUI->quit(0);
	//prefsUI->Destroy();
	//delete prefsUI;

	this->Show(false);
	this->Destroy();
}