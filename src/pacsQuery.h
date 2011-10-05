/**
 * \file pacsQuery.h
 * \brief File per la realizzazione di query al PACS
 * \author ICAR-CNR Napoli
 */

#ifndef _pacsQuery_h_
#define _pacsQuery_h_

#include <dcmtk/dcmnet/assoc.h>

#include "pacsPreferences.h"
#include "pacsDataTypes.h"

// Forward declarations
class DcmDataset;
struct T_DIMSE_C_FindRQ;
struct T_DIMSE_C_MoveRQ;
struct T_DIMSE_C_FindRSP;
struct T_DIMSE_C_MoveRSP;

//
// Once this class is instantiated, one must build a query using 
// the buildQuery command.  Following this, the object can be further
// manipulated (queryObject) to add finer control of query constraints.
// Then the query is sent using sendQuery, which deposits the data in 
// the users PacsList
//


/** \class pacsQuery
 * \brief Classe che fornisce supporto per le query DICOM
 */
class pacsQuery {
public:
	/** Costruttore con parametro preferenze */
	pacsQuery (const pacsPreferences*);

	/** Distruttore */
	~pacsQuery();

	/**
	 * \fn void buildQuery (const PacsData* _data)
	 * \brief Copia un dato in queryObject
	 * \param _data Dato da copiare
	 */
	void buildQuery(const PacsData*, bool bUseRequiredAttribsOnly = false);

	/**
	 * \fn void buildQuery (const PacsData* _data)
	 * \brief Copia un dato in queryObject e cerca oggetti in un livello inferiore
	 * \param _data Dato da copiare
	 */
	void buildFollowupQuery (const PacsData*);

	/**
	 * \fn QueryLevel queryLevel() const
	 * \brief Restituisce il livello di query corrente
	 * \return Il livello di query
	 */
	QueryLevel queryLevel() const;

	/**
	 * \fn int sendQuery(PacsList* _lst)
	 * \brief Effettua la query e riempie PacsList
	 * \param _lst Lista da riempire
	 * \return 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
	int sendQuery(PacsList*);

	/**
	 * \fn int sendMoveRequest(const char* _server = NULL)
	 * \brief Invia una richiesta per il download delle immagini
	 * \param _server Server a cui inviare la richiesta
	 * \return 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
	int sendMoveRequest(const char* = NULL);

	/**
	 * \fn int makeQuery(const char *_name, const char *_patientBirthdate, const char *_studyDate, PacsList *_lst)
	 * \brief Invia una richiesta per un dato paziente
	 * \param _name Nome del paziente
	 * \param _patientBirthdate Data di nascita del paziente
	 * \param _studyDate Data dello studio
	 * \param _lst Lista contenente i risultati
	 * \return 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
    int makeQuery (const char*, const char*, const char*, PacsList*);

	/**
	 * \fn int makeQuery(const char* _dir, PacsList* _lst)
	 * \brief Verifica se un percorso è un file o una directory e legge i file trovati
	 * \param _dir Percorso
	 * \param _lst Lista contenente i file
	 * \return 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
	int makeQuery (const char*, PacsList*);

	/**
	 * \fn int makeFollowupQuery(const PacsData* _data, PacsList* _lst)
	 * \brief Preleva un oggetto PacsData esistente e ritorna la lista di oggetti nel gruppo successivo
	 * \param _data Oggetto PacsData
	 * \param _lst Lista contenente gli oggetti
	 * \return 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
	int makeFollowupQuery(const PacsData*, PacsList*);

	/** 
	 * \var DcmDataset *m_pQueryObject
	 * \brief Contiene informazioni per le query
	 */
	DcmDataset *m_pQueryObject;

	/** 
	 * \var char localhost[256]
	 * \brief Bug WIN32: nome localhost cache
	 */
    char localhost[256];

protected:
	/**
	 * \fn int requestAssociation(char* _SOPClass)
	 * \brief Effettua una richiesta di associazione con un server
	 * \param _SOPClass Dovrebbe riferirsi ad un valido SOP Class
	 * \return 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
    int requestAssociation(char*);

	/**
	 * \fn static void FindCallback(void* callbackData, T_DIMSE_C_FindRQ* req, int responseCount, T_DIMSE_C_FindRSP* resp, DcmDataset *responseIdentifiers)
	 * \brief Funzione callback richiamata da DIMSE_findUser per ogni risultato della query
	 * \param callbackData Contiene i dati passati alla funzione callback
	 * \param req Contiene la richiesta che ha dato vita a questa ricerca
	 * \param responseCount Specifica quanti C-FIND-RSP sono stati ricevuti incluso quello attuale
	 * \param resp the C-FIND-RSP message which was received shortly before the call to this function.
	 * \param responseIdentifiers Contains the record which was received. This record matches the search mask of the C-FIND-RQ which was sent.
	 */
	static void FindCallback(void* , T_DIMSE_C_FindRQ*, int , T_DIMSE_C_FindRSP*, DcmDataset *);

	/**
	 * \fn static void MoveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, int responseCount, T_DIMSE_C_MoveRSP *response)
	 * \brief Funzione callback richiamata da DIMSE_findUser per ogni risultato della query
	 * \param callbackData Contiene i dati passati alla funzione callback
	 * \param req Contiene la richiesta che ha dato vita a questo MOVE
	 * \param responseCount Specifica quanti C-MOVE-RSP sono stati ricevuti incluso quello attuale
	 * \param resp the C-MOVE-RSP message which was received shortly before the call to this function.
	 */
	static void MoveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, int responseCount, T_DIMSE_C_MoveRSP *response);

	/**
	 * \fn void setParams()
	 * \brief Configuira i parametri
	 */
	void setParams();

	/** 
	 * \var T_ASC_Network* network
	 * \brief Usato per inizializzare/rilasciare la rete
	 */
	T_ASC_Network	*m_pNetwork;

	/** 
	 * \var T_ASC_Association* m_pAssociation
	 * \brief Usato per gestire l'associazione
	 */
	T_ASC_Association* m_pAssociation;

	/** 
	 * \var T_ASC_Parameters *m_pAssocParams
	 * \brief Usato per settare i parametri di rete
	 */
	T_ASC_Parameters *m_pAssocParams;

	/** 
	 * \var const pacsPreferences* prefs
	 * \brief Parametri per la comuncazione con ill PACS
	 */
	const pacsPreferences* prefs;

	/** 
	 * \var QueryLevel qLev
	 * \brief Definisce il livello corrente della query
	 */
	QueryLevel qLev;
};

#endif
