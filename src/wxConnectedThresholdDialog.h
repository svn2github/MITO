/**
 * \file wxConnectedThresholdDialog.h
 * \brief File per la creazione del dialog per la segmentazione secondo la tecnica connected threshold
 * \author ICAR-CNR Napoli
 */

#ifndef _wxConnectedThresholdDialog_h_
#define _wxConnectedThresholdDialog_h_

#define SYMBOL_CONNECTEDTHRESHOLDDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL
#define SYMBOL_CONNECTEDTHRESHOLDDIALOG_TITLE _("Connected Threshold")
#define SYMBOL_CONNECTEDTHRESHOLDDIALOG_IDNAME ID_DIALOG
#define SYMBOL_CONNECTEDTHRESHOLDDIALOG_SIZE wxSize(355, 310)
#define SYMBOL_CONNECTEDTHRESHOLDDIALOG_POSITION wxDefaultPosition
#define ID_DIALOG 1000


/**
 * \class wxConnectedThresholdDialog
 * \brief Dialog per la segmentazione secondo la tecnica connected threshold. Deriva da wxDialog
 *
 * Questa tecnica appartiene alla famiglia Region Growing. Il criterio per includere i pixel nella regione si
 * basa sulla presenza di una soglia minima ed una massima di intensità dei pixel; partendo dal punto iniziale
 * scelto dall'utente, il seme, l'algoritmo verifica singolarmente i pixel confinanti e, se la loro intensità
 * rientra nell'intervallo scelto, i pixel fanno parte della regione iniziale; l'algoritmo continua finché non
 * si possono aggiungere più pixel
 */
class wxConnectedThresholdDialog: public wxDialog {    

private:
	/** 
	 * \var wxTextCtrl* _seedX
	 * \brief Ascissa del seme
	 */
	wxTextCtrl* _seedX;

	/** 
	 * \var wxTextCtrl* _seedY
	 * \brief Ordinata del seme
	 */
	wxTextCtrl* _seedY;

	/** 
	 * \var wxTextCtrl* _seedValue
	 * \brief Intensità del seme
	 */
	wxTextCtrl* _seedValue;

	/** 
	 * \var wxTextCtrl* _lowerThreshold
	 * \brief Soglia minima
	 */
	wxTextCtrl* _lowerThreshold;

	/** 
	 * \var wxTextCtrl* _upperThreshold
	 * \brief Soglia massima
	 */
	wxTextCtrl* _upperThreshold;
	
public:
	/** Costruttore con parametri la finestra padre, l'identificativo, il titolo,  la posizione, la dimensione e lo stile del dialog */
	wxConnectedThresholdDialog(wxWindow* parent, wxWindowID id = SYMBOL_CONNECTEDTHRESHOLDDIALOG_IDNAME, const wxString& caption = SYMBOL_CONNECTEDTHRESHOLDDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONNECTEDTHRESHOLDDIALOG_POSITION, const wxSize& size = SYMBOL_CONNECTEDTHRESHOLDDIALOG_SIZE, long style = SYMBOL_CONNECTEDTHRESHOLDDIALOG_STYLE);

    /**
	* \fn bool Create(wxWindow* parent, wxWindowID id = SYMBOL_CONFIDENCECONNECTEDDDIALOG_IDNAME, const wxString& caption = SYMBOL_CONFIDENCECONNECTEDDDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONFIDENCECONNECTEDDDIALOG_POSITION, const wxSize& size = SYMBOL_CONFIDENCECONNECTEDDDIALOG_SIZE, long style = SYMBOL_CONFIDENCECONNECTEDDDIALOG_STYLE)
	* \brief Crea il dialog
	* \param parent Finestra padre
	* \param id Identificativo del dialog
	* \param caption Titolo del dialog
	* \param pos Posizione del dialog
	* \param size Dimensione del dialog
	* \param style Stile del dialog
	* \return true se l'operazione va buon fine
	*/
	bool Create(wxWindow* parent, wxWindowID id = SYMBOL_CONNECTEDTHRESHOLDDIALOG_IDNAME, const wxString& caption = SYMBOL_CONNECTEDTHRESHOLDDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONNECTEDTHRESHOLDDIALOG_POSITION, const wxSize& size = SYMBOL_CONNECTEDTHRESHOLDDIALOG_SIZE, long style = SYMBOL_CONNECTEDTHRESHOLDDIALOG_STYLE );

	/**
	* \fn void CreateControls()
	* \brief Crea i controlli
	*/
	void CreateControls();
	
    /**
	* \fn void InitializeDialog(double seedX, double seedY, short seedValue)
	* \brief Inizializza il dialog
	* \param seedX Ascissa del seme
	* \param seedY Ordinata del seme
	* \param seedValue Intensità del seme
	*/
	void InitializeDialog(double seedX, double seedY, short seedValue);
	
	/**
	* \fn short GetLowerThreshold()
	* \brief Restituisce la soglia minima
	* \return La soglia minima
	*/
	short GetLowerThreshold();

	/**
	* \fn short GetUpperThreshold()
	* \brief Restituisce la soglia massima
	* \return La soglia massima
	*/
	short GetUpperThreshold();
};

enum {
	id_ConnectedThreshold_seedX = 0,
	id_ConnectedThreshold_seedY,
	id_ConnectedThreshold_seedValue,
	id_ConnectedThreshold_lowerThreshold,
	id_ConnectedThreshold_upperThreshold, 
};

#endif _wxConnectedThresholdDialog_h_