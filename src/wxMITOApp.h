/**
 * \file wxMITOApp.h
 * \brief File per la creazione dell'applicazione MITO
 * \author ICAR-CNR Napoli
 */

#ifndef _wxMITOApp_h_
#define _wxMITOApp_h_

/**
 * \class wxMITOApp
 * \brief Classe responsabile della creazione dell'applicazione MITO Deriva da wxApp */
class wxMITOApp : public wxApp {

public:
	/** Costruttore */
	wxMITOApp();

	/** Distruttore */
	~wxMITOApp();

    /**
	* \fn virtual bool OnInit()
	* \brief Crea la finestra principale
	* \return true se l'operazione va buon fine
	*/
	virtual bool OnInit();

    /**
	* \fn virtual int OnExit()
	* \brief Chiude la finestra principale
	* \return Codice di errore
	*/
	virtual int OnExit();
};

DECLARE_APP(wxMITOApp)

#endif _wxMITOApp_h_