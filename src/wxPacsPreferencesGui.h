/**
 * \file wxPacsPreferencesGui.h
 * \brief File per la creazione del dialog per la scelta dei parametri di connessione al PACS
 * \author ICAR-CNR Napoli
 */

#ifndef _wxPacsPreferencesGui_h_
#define _wxPacsPreferencesGui_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wxPacsPreferencesGui.h"
#endif

/*
 * Includes
 */

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/notebook.h>
#include "pacsPreferences.h"


#define SYMBOL_PREFERENCES_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PREFERENCES_TITLE _("Preferences")
#define SYMBOL_PREFERENCES_IDNAME ID_DIALOG_PREFERENCES
#define SYMBOL_PREFERENCES_SIZE wxSize(400, 300)
#define SYMBOL_PREFERENCES_POSITION wxDefaultPosition
#define ID_DIALOG_PREFERENCES 10026


#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif


/**
 * \class wxPacsPreferencesGui
 * \brief Dialog per la scelta dei parametri di connessione al PACS. Deriva da wxDialog
 */
class wxPacsPreferencesGui: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( Preferences )
    DECLARE_EVENT_TABLE()

private:
	/** 
	 * \var pacsPreferences* prefs
	 * \brief Puntatore ai parametri di connessione
	 */	
	pacsPreferences* prefs;

	/** 
	 * \var pacsPreferences* current
	 * \brief Puntatore ai parametri di connessione correnti
	 */
	pacsPreferences* current;

	/** 
	 * \var wxPacsPreferencesGui* preferencesDialog
	 * \brief Puntatore al dialog per la specifica dei parametri di connessione al PACS
	 */
	wxPacsPreferencesGui* preferencesDialog;

	/** 
	 * \var wxTextCtrl* serverInput
	 * \brief Casella di testo in cui è possibile specificare il server di ingresso
	 */
	wxTextCtrl* serverInput;

	/** 
	 * \var wxTextCtrl* portInput
	 * \brief Casella di testo in cui è possibile specificare la porta del server di ingresso
	 */
	wxTextCtrl* portInput;

	/** 
	 * \var wxTextCtrl* calledAPInput
	 * \brief Casella di testo in cui è possibile specificare il nome dell'application entity del server
	 */
	wxTextCtrl* calledAPInput;

	/** 
	 * \var wxTextCtrl* callingAPInput
	 * \brief Casella di testo in cui è possibile specificare il nome dell'application entity del client
	 */
	wxTextCtrl* callingAPInput;

	/** 
	 * \var wxTextCtrl* storageAPInput
	 * \brief Casella di testo in cui è possibile specificare il nome dell'application entity del server per il downlaod delle immagini
	 */
	wxTextCtrl* storageAPInput;

	/** 
	 * \var wxTextCtrl* receiveStorageInput
	 * \brief Casella di testo in cui è possibile specificare il nome del server per il downlaod delle immagini
	 */
	wxTextCtrl* receiveStorageInput;

	/** 
	 * \var wxTextCtrl* receiveDirectoryInput
	 * \brief Casella di testo in cui è possibile specificare la directory in cui scaricare le immagini
	 */
	wxTextCtrl* receiveDirectoryInput;

	/** 
	 * \var wxTextCtrl* maxPDU
	 * \brief Casella di testo in cui è possibile specificare il PDU massimo
	 */
	wxTextCtrl* maxPDU;

	/** 
	 * \var wxTextCtrl* maxPDU
	 * \brief Checkbox che indica se avviare e arrestare il server per il download delle immagini automaticamente
	 */
	wxCheckBox* receiveStorageExecButton;


public:
    /** Costruttore */
    wxPacsPreferencesGui();

	/** Costruttore con parametri la finestra padre, un puntatore a pacsPreferences, l'identificativo, il titolo,  la posizione, la dimensione e lo stile del dialog */
	wxPacsPreferencesGui(wxWindow* parent, pacsPreferences *_prefs, wxWindowID id = SYMBOL_PREFERENCES_IDNAME, const wxString& caption = SYMBOL_PREFERENCES_TITLE, const wxPoint& pos = SYMBOL_PREFERENCES_POSITION, const wxSize& size = SYMBOL_PREFERENCES_SIZE, long style = SYMBOL_PREFERENCES_STYLE);

	/** Distruttore */
	~wxPacsPreferencesGui();

    /**
	* \fn bool Create(wxWindow* parent, pacsPreferences *_prefs, wxWindowID id = SYMBOL_PREFERENCES_IDNAME, const wxString& caption = SYMBOL_PREFERENCES_TITLE, const wxPoint& pos = SYMBOL_PREFERENCES_POSITION, const wxSize& size = SYMBOL_PREFERENCES_SIZE, long style = SYMBOL_PREFERENCES_STYLE)
	* \brief Crea il dialog
	* \param parent Finestra padre
	* \param prefs Puntatore ai parametri di connessione al PACS
	* \param id Identificativo del dialog
	* \param caption Titolo del dialog
	* \param pos Posizione del dialog
	* \param size Dimensione del dialog
	* \param style Stile del dialog
	* \return true se l'operazione va buon fine
	*/
	bool Create(wxWindow* parent, pacsPreferences *_prefs, wxWindowID id = SYMBOL_PREFERENCES_IDNAME, const wxString& caption = SYMBOL_PREFERENCES_TITLE, const wxPoint& pos = SYMBOL_PREFERENCES_POSITION, const wxSize& size = SYMBOL_PREFERENCES_SIZE, long style = SYMBOL_PREFERENCES_STYLE);

	/**
	* \fn void CreateControls()
	* \brief Crea i controlli
	*/
	void CreateControls();

	/**
	* \fn void show()
	* \brief Visualizza i parametri
	*/
	void show();

	/**
	* \fn void browse(wxTextCtrl* input)
	* \brief Esplora le directory sull'hard disk
	*/
	void browse(wxTextCtrl* input);

	/**
	* \fn void initSettings()
	* \brief Inizializza i parametri
	*/
	void initSettings();

	/**
	* \fn void saveSettings()
	* \brief Salva i parametri inseriti
	*/
	void saveSettings();
	
	/**
	* \fn void quit(int _save)
	* \brief Chiude il dialog
	* \param _save Indica se salvare i parametri (=1) o meno (=0)
	*/
	void quit(int _save); 

	/**
	* \fn void OnCloseWindow(wxCloseEvent& event)
	* \brief Distrugge il dialog
	* \param event Evento
	*/
	void OnCloseWindow(wxCloseEvent& event);

	/**
	* \fn void OnCancel(wxCommandEvent& event)
	* \brief Chiude il dialog senza memorizzare i parametri inseriti
	* \param event Evento
	*/
	void OnCancel(wxCommandEvent& event);

	/**
	* \fn void OnAccept(wxCommandEvent& event)
	* \brief Chiude il dialog e memorizza i parametri inseriti
	* \param event Evento
	*/
	void OnAccept(wxCommandEvent& event);

	/**
	* \fn void OnHelp1(wxCommandEvent& event)
	* \brief Primo messaggio informativo
	* \param event Evento
	*/
	void OnHelp1(wxCommandEvent& event);

	/**
	* \fn void OnHelp2(wxCommandEvent& event)
	* \brief Secondo messaggio informativo
	* \param event Evento
	*/
	void OnHelp2(wxCommandEvent& event);

	/**
	* \fn void OnHelp3(wxCommandEvent& event)
	* \brief Terzo messaggio informativo
	* \param event Evento
	*/
	void OnHelp3(wxCommandEvent& event);

	/**
	* \fn void OnBrowse(wxCommandEvent& event)
	* \brief Avvia la ricerca delle directory sull'hard disk
	* \param event Evento
	*/
	void OnBrowse(wxCommandEvent& event);


	/**
	* \fn wxBitmap GetBitmapResource(const wxString& name)
	* \brief Restituisce le risorse bitmap
	* \param name Nome bitmap
	* \return Risorsa bitmap
	*/
	wxBitmap GetBitmapResource(const wxString& name);

	/**
	* \fn wxIcon GetIconResource(const wxString& name)
	* \brief Restituisce le icone
	* \param name Nome icona
	* \return Icona
	*/
	wxIcon GetIconResource(const wxString& name);

	/**
	* \fn static bool ShowToolTips()
	* \brief Attiva/disattiva la visualizzazione dei tooltip
	* \return true se la visualizzazione è attivata
	*/
	static bool ShowToolTips();

};

enum {
	notebook2 = 1,
	notebook2_panel1,
	notebook2_panel2,
	panel1_textCtrl1,
	panel1_textCtrl2,
	panel1_textCtrl3,
	panel1_textCtrl4,
	panel1_textCtrl5,
	panel2_textCtrl1,
	panel2_textCtrl2,
	panel2_textCtrl3,
	ok_button,
	cancel_button,
	help_button1,
	help_button2,
	help_button3,
	browse_button,
	start_button

};
#endif _wxPacsPreferencesGui_h_
