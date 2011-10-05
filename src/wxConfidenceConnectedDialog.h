/**
 * \file wxConfidenceConnectedDialog.h
 * \brief File per la creazione del dialog per la segmentazione secondo la tecnica confidence connected
 * \author ICAR-CNR Napoli
 */

#ifndef _wxConfidenceConnectedDialog_h_
#define _wxConfidenceConnectedDialog_h_

#define SYMBOL_CONFIDENCECONNECTEDDDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDIALOG_MODAL
#define SYMBOL_CONFIDENCECONNECTEDDDIALOG_TITLE _("Confidence Connected")
#define SYMBOL_CONFIDENCECONNECTEDDDIALOG_IDNAME ID_DIALOG
#define SYMBOL_CONFIDENCECONNECTEDDDIALOG_SIZE wxSize(355, 410)
#define SYMBOL_CONFIDENCECONNECTEDDDIALOG_POSITION wxDefaultPosition
#define ID_DIALOG 1000


/**
 * \class wxConfidenceConnectedDialog
 * \brief Dialog per la segmentazione secondo la tecnica confidence connected. Deriva da wxDialog
 *
 * Questa tecnica appartiene alla famiglia Region Growing. Il criterio per includere i pixel nella regione è
 * basato sul calcolo della media e della deviazione standard dell'intensità di tutti i pixel inclusi nella 
 * regione. L'utente immette un fattore che è usato per moltiplicare la deviazione standard e definire un range
 * intorno alla media. Pixel vicini, con valori di intensità che ricadono nel range, sono inclusi nella regione.
 * Quando nessun pixel confinante può essere incluso nella regione, l'algoritmo termina la sua prima iterazione.
 * A questo punto l'algoritmo ricalcola la media e la deviazione standard dell'intensità dei pixel inclusi nella 
 * regione e ripete il procedimento fino a quando non si possono aggiungere più pixel o quando è stato raggiunto
 * il numero massimo di iterazioni. Detti f il fattore moltiplicativo, m e s la media e la deviazione standard 
 * dell'intensità dei pixel della regione individuata a partire dal seme e dal raggio specificati, il range che 
 * il criterio considera è il seguente: [m-fs,m+fs]
 */
class wxConfidenceConnectedDialog: public wxDialog {    

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
	 * \var wxTextCtrl* _radius
	 * \brief Raggio
	 */
	wxTextCtrl* _radius;

	/** 
	 * \var wxTextCtrl* _multiplier
	 * \brief Valore del moltiplicatore
	 */
	wxTextCtrl* _multiplier;	

	/** 
	 * \var wxTextCtrl* _iterations
	 * \brief Numero di iterazioni
	 */
    wxTextCtrl* _iterations;

public:
	/** Costruttore con parametri la finestra padre, l'identificativo, il titolo,  la posizione, la dimensione e lo stile del dialog */
	wxConfidenceConnectedDialog(wxWindow* parent, wxWindowID id = SYMBOL_CONFIDENCECONNECTEDDDIALOG_IDNAME, const wxString& caption = SYMBOL_CONFIDENCECONNECTEDDDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONFIDENCECONNECTEDDDIALOG_POSITION, const wxSize& size = SYMBOL_CONFIDENCECONNECTEDDDIALOG_SIZE, long style = SYMBOL_CONFIDENCECONNECTEDDDIALOG_STYLE);

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
	bool Create(wxWindow* parent, wxWindowID id = SYMBOL_CONFIDENCECONNECTEDDDIALOG_IDNAME, const wxString& caption = SYMBOL_CONFIDENCECONNECTEDDDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONFIDENCECONNECTEDDDIALOG_POSITION, const wxSize& size = SYMBOL_CONFIDENCECONNECTEDDDIALOG_SIZE, long style = SYMBOL_CONFIDENCECONNECTEDDDIALOG_STYLE);

    /**
	* \fn void InitializeDialog(double seedX, double seedY, short seedValue)
	* \brief Inizializza il dialog
	* \param seedX Ascissa del seme
	* \param seedY Ordinata del seme
	* \param seedValue Intensità del seme
	*/
	void InitializeDialog(double seedX, double seedY, short seedValue);
	
	/**
	* \fn void CreateControls()
	* \brief Crea i controlli
	*/
	void CreateControls();
	
	/**
	* \fn float GetRadius()
	* \brief Restituisce il raggio
	* \return Il raggio
	*/
	float GetRadius();

	/**
	* \fn float GetMultiplier()
	* \brief Restituisce il moltiplicatore
	* \return Il moltiplicatore
	*/
	float GetMultiplier();

	/**
	* \fn int GetIterations()
	* \brief Restituisce il numero di iterazioni
	* \return Il numero di iterazioni
	*/
	int GetIterations();

};

enum {
	id_ConfidenceConnected_seedX = 0,
	id_ConfidenceConnected_seedY,
	id_ConfidenceConnected_seedValue,
	id_ConfidenceConnected_radius,
	id_ConfidenceConnected_multiplier,
	id_ConfidenceConnected_iterations,
};

#endif _wxConfidenceConnectedDialog_h_