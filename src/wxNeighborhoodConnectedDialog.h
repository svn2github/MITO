/**
 * \file wxNeighborhoodConnectedDialog.h
 * \brief File per la creazione del dialog per la segmentazione secondo la tecnica neighborhood connected
 * \author ICAR-CNR Napoli
 */

#ifndef _wxNeighborhoodConnectedDialog_h_
#define _wxNeighborhoodConnectedDialog_h_

#define SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDIALOG_MODAL
#define SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_TITLE _("Neighborhood Connected")
#define SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_IDNAME ID_DIALOG
#define SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_SIZE wxSize(355, 410)
#define SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_POSITION wxDefaultPosition
#define ID_DIALOG 1000


/**
 * \class wxNeighborhoodConnectedDialog
 * \brief Dialog per la segmentazione secondo la tecnica neighborhood connected. Deriva da wxDialog
 *
 * Questa tecnica appartiene alla famiglia Region Growing. Il criterio per includere i pixel nella regione è
 * identico a quello utilizzato per il connected threshold, con la differenza che il neighborood connected
 * includerà un pixel, la cui intensità ricade nell'intervalllo definito dalla soglia inferiore e dalla soglia
 * superiore, solo se tutto il vicinato ha un valore di intensità che cade in tale intervallo
 */
class wxNeighborhoodConnectedDialog: public wxDialog {    

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

	/** 
	 * \var wxTextCtrl* _radiusX
	 * \brief Ascissa del raggio
	 */
	wxTextCtrl* _radiusX;

	/** 
	 * \var wxTextCtrl* _radiusY
	 * \brief Ordinata del raggio
	 */
	wxTextCtrl* _radiusY;
	
public:
	/** Costruttore con parametri la finestra padre, l'identificativo, il titolo,  la posizione, la dimensione e lo stile del dialog */
	wxNeighborhoodConnectedDialog(wxWindow* parent, wxWindowID id = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_IDNAME, const wxString& caption = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_TITLE, const wxPoint& pos = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_POSITION, const wxSize& size = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_SIZE, long style = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_STYLE);

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
	bool Create(wxWindow* parent, wxWindowID id = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_IDNAME, const wxString& caption = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_TITLE, const wxPoint& pos = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_POSITION, const wxSize& size = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_SIZE, long style = SYMBOL_NEIGHBORHOODCONNECTEDDIALOG_STYLE );

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

	/**
	* \fn float GetRadiusX()
	* \brief Restituisce l'ascissa del raggio
	* \return Ascissa del raggio
	*/
	float GetRadiusX();

	/**
	* \fn float GetRadiusY()
	* \brief Restituisce l'ordinata del raggio
	* \return Ordinata del raggio
	*/
	float GetRadiusY();
};

enum {
	id_NeighborhoodConnected_seedX = 0,
	id_NeighborhoodConnected_seedY,
	id_NeighborhoodConnected_seedValue,
	id_NeighborhoodConnected_lowerThreshold,
	id_NeighborhoodConnected_upperThreshold,
	id_NeighborhoodConnected_radiusX,
	id_NeighborhoodConnected_radiusY,
};

#endif _wxNeighborhoodConnectedDialog_h_