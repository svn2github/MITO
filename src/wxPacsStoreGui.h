/**
 * \file wxPacsStoreGui.h
 * \brief File per la creazione del dialog per l'invio di immagini al PACS
 * \author ICAR-CNR Napoli
 */

#ifndef _wxStoreGui_h_
#define _wxStoreGui_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxStoreGui.h"
#endif

/*
 * Includes
 */

#include <wx/frame.h>
#include <wx/statusbr.h>
#include "pacsDataTypes.h"
#include "pacsCommunicator.h"
#include "wxPacsListBox.h"



/*
 * Control identifiers
 */

#define ID_FRAME 10000
#define SYMBOL_STOREWINDOW_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT 
#define SYMBOL_STOREWINDOW_TITLE _("Send to Pacs")
#define SYMBOL_STOREWINDOW_IDNAME ID_FRAME
#define SYMBOL_STOREWINDOW_SIZE wxSize(400, 330)
#define SYMBOL_STOREWINDOW_POSITION wxDefaultPosition

/*
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/**
 * \class wxPacsStoreGui
 * \brief Finestra che consente di inviare le immaigni al PACS. Deriva da wxFrame e pacsCommunicator
 */
class wxPacsStoreGui: public wxFrame, public pacsCommunicator
{    
    DECLARE_CLASS( wxPacsStoreGui )
    DECLARE_EVENT_TABLE()
private:

	/** 
	 * \var wxMenu* options
	 * \brief Menù per le opzioni
	 */
	wxMenu* options;

	/** 
	 * \var wxTextCtrl* outputBrowser
	 * \brief Browser contenente informazioni di log
	 */
	wxTextCtrl* outputBrowser;

	/** 
	 * \var wxTextCtrl* directoryInput
	 * \brief Casella di testo in cui è possibile inserire il nome della directory contenente le immagini da inviare
	 */
	wxTextCtrl* directoryInput;

public:
    /** Costruttore */
    wxPacsStoreGui();

	/** Costruttore con parametri la finestra padre, l'identificativo, il titolo,  la posizione, la dimensione e lo stile della finestra */
	wxPacsStoreGui(wxWindow* parent, wxWindowID id = SYMBOL_STOREWINDOW_IDNAME, const wxString& caption = SYMBOL_STOREWINDOW_TITLE, const wxPoint& pos = SYMBOL_STOREWINDOW_POSITION, const wxSize& size = SYMBOL_STOREWINDOW_SIZE, long style = SYMBOL_STOREWINDOW_STYLE);

    /**
	* \fn bool Create(wxWindow* parent, wxWindowID id = SYMBOL_PACSWINDOW_IDNAME, const wxString& caption = SYMBOL_PACSWINDOW_TITLE, const wxPoint& pos = SYMBOL_PACSWINDOW_POSITION, const wxSize& size = SYMBOL_PACSWINDOW_SIZE, long style = SYMBOL_PACSWINDOW_STYLE)
	* \brief Crea il dialog
	* \param parent Finestra padre
	* \param id Identificativo del dialog
	* \param caption Titolo del dialog
	* \param pos Posizione del dialog
	* \param size Dimensione del dialog
	* \param style Stile del dialog
	* \return true se l'operazione va buon fine
	*/
	bool Create(wxWindow* parent, wxWindowID id = SYMBOL_STOREWINDOW_IDNAME, const wxString& caption = SYMBOL_STOREWINDOW_TITLE, const wxPoint& pos = SYMBOL_STOREWINDOW_POSITION, const wxSize& size = SYMBOL_STOREWINDOW_SIZE, long style = SYMBOL_STOREWINDOW_STYLE);

	/**
	* \fn void CreateControls()
	* \brief Crea i controlli
	*/
	void CreateControls();

	/**
	* \fn void OnExit(wxCommandEvent& event)
	* \brief Chiude la finestra
	* \param event Evento
	*/
    void OnExit( wxCommandEvent& event );

	/**
	* \fn void OnCloseWindow(wxCloseEvent& event)
	* \brief Chiude la finestra
	* \param event Evento
	*/
	void OnCloseWindow(wxCloseEvent& event);

	/**
	* \fn void OnFileGo(wxCommandEvent& event)
	* \brief Invia un file o una directory specificata al PACS
	* \param event Evento
	*/
	void OnFileGo(wxCommandEvent& event);

	/**
	* \fn void OnFileBrowse(wxCommandEvent& event)
	* \brief Esplora i file sull'hard disk
	* \param event Evento
	*/
	void OnFileBrowse(wxCommandEvent& event);

	/**
	* \fn void OnDirBrowse(wxCommandEvent& event)
	* \brief Esplora le directory sull'hard disk
	* \param event Evento
	*/
	void OnDirBrowse(wxCommandEvent& event);

	/**
	* \fn wxBitmap GetBitmapResource(const wxString& name)
	* \brief Restituisce le risorse bitmap
	* \param name Nome bitmap
	* \return Risorsa bitmap
	*/
	wxBitmap GetBitmapResource( const wxString& name );

	/**
	* \fn wxIcon GetIconResource(const wxString& name)
	* \brief Restituisce le icone
	* \param name Nome icona
	* \return Icona
	*/
    wxIcon GetIconResource( const wxString& name );

	/**
	* \fn static bool ShowToolTips()
	* \brief Attiva/disattiva la visualizzazione dei tooltip
	* \return true se la visualizzazione è attivata
	*/
	static bool ShowToolTips();

};

enum {
    _menu_preferences = 100,
	_menu_automatically_save_preferences,
	_menu_open,
	_menu_save,
	_menu_saveas,
	_menu_exit,
	_menu_about,
	_center_panel,
	_top_panel,
	_top_inner_panel,
	_right_panel,
	_send_text,
	_send_button,
	_cancel_button,
	_dir_button,
	_file_button,
	_bottom_textCtrl
};


#endif _wxStoreGui_h_