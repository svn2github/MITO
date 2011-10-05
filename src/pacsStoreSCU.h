/**
 * \file pacsStoreSCU.h
 * \brief File header della classe per l'invio dei file dicom al PACS
 * \author ICAR-CNR Napoli
 */

#ifndef _pacsStoreSCU_h_
#define _pacsStoreSCU_h_

#include <dcmtk/dcmnet/assoc.h>

#include "pacsPreferences.h"
#include "pacsDataTypes.h"

// Forward declarations
class DcmDataset;
struct T_DIMSE_C_FindRQ;
struct T_DIMSE_C_FindRSP;

/** 
 * \class pacsStoreSCU
 * \brief Classe che fornisce supporto per l'invio di file al PACS
 */
class pacsStoreSCU {
public:
	/** Costruttore con parametro preferenze */
	pacsStoreSCU (const pacsPreferences*);

	/** Distruttore */
	~pacsStoreSCU();


	/**
	 * \fn int storeRequest (const char *szFileName)
	 * \brief Invia un file dicom al server PACS
	 * \param szFileName Percorso del file da inviare
	 */
	int storeRequest(const char *szFileName);

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

	static void storeCallback(void * /*callbackData*/, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ * /*req*/);

	/**
	 * \fn Store(const char *szFileName)
	 * \brief Richiamata da storeRequest, si occupa dell'invio reale del file
	 * \param szFileName Percorso del file da inviare
	 * \return OFCondition con lo stato della richiesta
	 */
	OFCondition	Store(const char *szFileName);

	/**
	 * \fn void setParams()
	 * \brief Configuira i parametri
	 */
	void setParams();

	/** 
	 * \var OFBool	m_bStoreFailed
	 * \brief Vero se ci sono stati errori durante il trasferimento
	 */
	OFBool	m_bStoreFailed;

	/** 
	 * \var int	m_iLastStatusCode
	 * \brief Contiene il codice di stato relativo all'ultima trasmissione
	 */
	int m_iLastStatusCode;

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
};

#endif	// _pacsStoreSCU_h_
