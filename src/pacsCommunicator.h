/**
 * \file pacsCommunicator.h
 * \brief File per la comunicazione con i PACS
 * \author ICAR-CNR Napoli
 */

#ifndef _pacsCommunicator_h_
#define _pacsCommunicator_h_

#include "pacsDataTypes.h"
#include "pacsPreferences.h"
#include "pacsQuery.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#include <wx/wx.h>
#include <wx/notebook.h>
//#include <ctn_os.h>

//#include "wxPacsPreferencesGui.h"
#include "wxPacsListBox.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// location of settings.cfg
#ifdef WIN32
#define PacsCFG "settings.cfg"
#else
#define PacsCFG "$HOME/.settingsrc"
#endif

// Eventi
#define PVT_EVT_BASE						666
#define EVT_MITO_QUERY_TERMINATA			(PVT_EVT_BASE)
#define EVT_MITO_QUERY_ERROR				(PVT_EVT_BASE) + 1
#define EVT_MITO_RECEIVED_IMAGE				(PVT_EVT_BASE) + 2
#define EVT_MITO_RECEIVING_NEW_SERIES		(PVT_EVT_BASE) + 3
#define EVT_MITO_RECEIVED_SERIES			(PVT_EVT_BASE) + 4
#define EVT_MITO_RECEIVING_NEW_STUDY		(PVT_EVT_BASE) + 5
#define EVT_MITO_RECEIVED_STUDY				(PVT_EVT_BASE) + 6
#define EVT_MITO_RECEIVE_NOMORE				(PVT_EVT_BASE) + 7
#define EVT_MITO_CLOSING_QUERYGUI			(PVT_EVT_BASE) + 8
#define EVT_MITO_RECEIVE_START				(PVT_EVT_BASE) + 9

// Forward declarations
class pacsStoreSCP;
class wxEvtHandler;

/** \class pacsCommunicator
 * \brief Classe responsabile della comunicazione con i PACS
 */
class pacsCommunicator {

public:
	/** Costruttore */
	pacsCommunicator();

	/** Distruttore */
	virtual ~pacsCommunicator();

	/**
	 * \fn bool queryFollowUp (PacsData *pStart, const QueryLevel _qLev, PacsList ** lists)
	 * \brief Realizza le query in base ai dataset
	 * \param pStart Struttura relativa all'elemento selezionato nel TreeList
	 * \param _qLev Livello query
	 * \param PacsList * Lista nella quale salvare i risultati della query
	 * \return Ritorna true in caso di successo, false in caso di problemi (probabilmente dovuti alla mancata risposta del PACS)
	 */
	bool queryFollowUp (PacsData *pStart, const QueryLevel _qLev, PacsList ** lists);

	/**
	 * \fn bool query (const char *_name, const char *_patientBirthdate, const char *_studyDate)
	 * \brief Realizza le query al server per date informazioni e memorizza i risultati in lists[0]
	 * \param _name Nome del paziente
	 * \param _patientBirthdate Data di nascita del paziente
	 * \param _studyDate Data dello studio
	 * \return Ritorna true in caso di successo, false in caso di problemi (probabilmente dovuti alla mancata risposta del PACS)
	 */
	bool query (const char*, const char*, const char*, PacsList **);

	/**
	 * \fn bool query (StudyData *queryData, PacsList **lists)
	 * \brief Realizza le query al server per date informazioni e memorizza i risultati in lists[0]
	 * \param queryData Contiene le informazioni tramite le quali effettuare la query
	 * \param lists Lista in cui salvare i risultati della query
	 * \param bDeleteStudyData Dealloca queryData alla fine della query (valore predefinito false)
	 * \return Ritorna true in caso di successo, false in caso di problemi (probabilmente dovuti alla mancata risposta del PACS)
	 */
	bool query (StudyData *, PacsList **, bool bDeleteStudyData = false);

	/**
	 * \fn void query (const char*)
	 * \brief Realizza le query alle directory
	 * \param _dir Nome della directory
	 * \return Ritorna true in caso di successo, false in caso di problemi (probabilmente dovuti alla mancata risposta del PACS)
	 */
	bool query (const char*, PacsList **);

	/**
	 * \fn int send (const char* _server, , PacsData *pToMove)
	 * \brief Scarica una immagine dal server
	 * \param _server Server da cui scaricare le immagini
	 * \param pToMove Riferimento dell'oggetto da scaricare (preso dalla lista)
	 */
	int send (const char*, PacsData *);

	/**
	 * \fn void clearAll()
	 * \brief Elimina tutti gli elementi dalla lista
	 */
    void clearAll();

	/**
	 * \fn void setOutput(wxTextCtrl* _output)
	 * \brief Assegna il browser di output
	 * \param _output Browser di output
	 */
	void setOutput(wxTextCtrl*);

	/**
	 * \fn int store(const char *fileName)
	 * \brief Invia immagini al PACS
	 * \param fileName Nome del file o della directory da inviare
	 * \return Vale 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
	int store(const char *fileName);

	/**
	 * \fn pacsQuery* getPacsQuery()
	 * \brief Restituisce il puntatore all'oggetto di tipo pacsQuery
	 * \return Puntatore all'oggetto di tipo pacsQuery
	 */
	inline pacsQuery* getPacsQuery() {
		return qMod;
	}

	/**
	 * \fn pacsStoreSCP *spawnStoreSCP(wxEvtHandler *pParent)
	 * \brief Avvia il server per il download delle immagini
	 * \return Ritorna una instanza della classe server SCP
	 */
	pacsStoreSCP *spawnStoreSCP(wxEvtHandler *);

	/**
	 * \fn pacsPreferences *getPrefs()
	 * \brief Restituisce il puntatore all'oggetto di tipo pacsPreferences
	 * \return Puntatore all'oggetto di tipo pacsPreferences
	 */
	inline pacsPreferences *getPrefs() {
		return prefs;
	}

	void setPrefs(const pacsPreferences *pPrefs) {
		memcpy(prefs, pPrefs, sizeof(pacsPreferences));
	}

	/** 
	 * \var PacsList* lists[2]
	 * \brief Lista per i dati
	 */
	//PacsList* lists[2];


protected:
	/** 
	 * \var pacsPreferences *prefs
	 * \brief Parametri per il PACS
	 */
	pacsPreferences *prefs;

	/** 
	 * \var pacsQuery *qMod
	 * \brief Puntatore al gestore delle query per i PACS
	 */
	pacsQuery *qMod;


	/** 
	 * \var char prefsName[255]
	 * \brief Nome del file contenente i parametri del PACS
	 */
	char prefsName[255];

	/** 
	 * \var wxTextCtrl *output
	 * \brief Browser di output
	 */
	wxTextCtrl *output;

                                                
	/**
	 * \fn void writeOutput(const char* _msg)
	 * \brief Scrive un messaggio nel browser di output
	 * \param _msg Messaggio da visualizzare
	 */
	void writeOutput(const char*);
};

#endif //_pacsCommunicator_h_