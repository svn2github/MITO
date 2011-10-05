/**
 * \file pacsStoreSCP.h
 * \brief File per la comunicazione con i PACS
 * \author ICAR-CNR Napoli
 */

#ifndef _pacsStoreSCP_h_
#define _pacsStoreSCP_h_

#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/dimse.h>

#include <wx/wxprec.h>

#include <wx/wx.h>

#include <wx/thread.h>
#include <wx/string.h>

// Forward declarations
class pacsPreferences;
class _pacsStoreAcceptorThread;
class DcmFileFormat;
class wxEvtHandler;

DECLARE_EVENT_TYPE(wxEVT_STORE_NOTIFICATION, -1)

// Semplicemente presa dall'esempio storeSCP, utilizzata per passare
// dati alla callback. Probabilmente c'è un modo più elegante per farlo..
struct StoreCallbackData
{
  char* imageFileName;
  DcmFileFormat* dcmff;
  T_ASC_Association* assoc;
  void* pCallerPtr;
};

/**
 * \class pacsStoreSCP
 * \brief Classe per la gestione Store Service Provider
 */
class pacsStoreSCP
{
public:
	pacsStoreSCP(const pacsPreferences*, wxEvtHandler*);
	~pacsStoreSCP();

	/**
	 * \fn int Start()
	 * \brief Avvia il server
	 * \return Vale 1 se l'operazione è andata a buon fine, 0 altrimenti
	 */
	int	Start();

	/**
	 * \fn void Stop()
	 * \brief Ferma il server
	 */
	void Stop();

	/**
	 * \fn T_ASC_Network *getNetwork()
	 * \brief Restituisce il puntatore all'oggetto di tipo T_ASC_Network
	 * \return Puntatore all'oggetto di tipo T_ASC_Network
	 */
	inline T_ASC_Network *getNetwork() {
		return m_pNetwork;
	}

	/**
	 * \fn pacsPreferences *getPrefs()
	 * \brief Restituisce il puntatore all'oggetto di tipo pacsPreferences
	 * \return Puntatore all'oggetto di tipo pacsPreferences
	 */
	inline const pacsPreferences *getPrefs() {
		return m_pPrefs;
	}

	/**
	 * \fn wxEvtHandler *getParent()
	 * \brief Restituisce il puntatore all'oggetto di tipo wxEvtHandler che gestirà i messaggi
	 * \return Puntatore all'oggetto di tipo wxEvtHandler
	 */
	inline wxEvtHandler *getParent() {
		return m_pParent;
	}

protected:
	/** 
	 * \var const pacsPreferences *m_pcPrefs
	 * \brief Parametri per l'avvio del server per il download delle immagini
	 */
	pacsPreferences *m_pPrefs;

	/** 
	 * \var wxEvtHandler *m_pParent
	 * \brief Necessario per notificare dello scaricamento l'interfaccia grafica
	 */
	wxEvtHandler *m_pParent;

private:
	/** 
	 * \var T_ASC_Network* network
	 * \brief Usato per inizializzare/rilasciare la rete
	 */
	T_ASC_Network	*m_pNetwork;

	/**
	*	\brief Classe che accetta le connessioni al server store
	*/
	class _pacsStoreAcceptorThread : public wxThread
	{
	public:
		_pacsStoreAcceptorThread() : m_pCaller(NULL) {};
		~_pacsStoreAcceptorThread(){m_pCaller = NULL; cout << "_pacsStoreAcceptorThread::Entry - Thread distrutto!" << endl;  };

		void Create(pacsStoreSCP *pCaller, const wxString &sSavePath /*di che parametri abbiamo bisogno qui?*/);

	private:
		/** 
		* \var _pacsStoreAcceptorThread *m_pAcceptor
		* \brief Puntatore all'instanza del thread che accetta le connessioni in ingresso
		*/
		pacsStoreSCP *m_pCaller;

		/** 
		* \var wxString m_sSaveToPath
		* \brief Contiene un valido percorso in cui salvare i files ricevuti
		*/
		wxString m_sSaveToPath;

		void* Entry();

		/**
		 * \fn OFCondition ProcessAssociations
		 * \param T_ASC_Network *net Puntatore all'oggetto contenente le informazioni sulla rete
		 * \brief Gestisce i tentativi di associazione e i messaggi scambiati
		 * \return Oggetto OFCondition contenente lo stato
		 */
		OFCondition	ProcessAssociations(T_ASC_Network *net/*, DcmAssociationConfiguration& asccfg*/);

		/**
		 * \fn OFCondition ProcessCommands
		 * \param T_ASC_Association *assoc Puntatore all'oggetto contenente le informazioni sull'associazione
		 * \brief Gestisce i messaggi (C-ECHORQ C-STORERQ) scambiati
		 * \return Oggetto OFCondition contenente lo stato
		 */
		OFCondition	ProcessCommands(T_ASC_Association *assoc);

		/**
		 * \fn OFCondition HandleEchoRQ
		 * \brief Gestisce il messaggio C-ECHORQ, rispondendo alla richiesta
		 * \return Oggetto OFCondition contenente lo stato
		 */
		OFCondition	HandleEchoRQ(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID);

		/**
		 * \fn OFCondition HandleStoreRQ
		 * \brief Gestisce il messaggio C-STORERQ
		 * \return Oggetto OFCondition contenente lo stato
		 */		
		OFCondition HandleStoreRQ(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID);
	};

	/** 
	 * \var _pacsStoreAcceptorThread *m_pAcceptor
	 * \brief Puntatore all'instanza del thread che accetta le connessioni in ingresso
	 */
	_pacsStoreAcceptorThread *m_pAcceptor;
};



#endif // _pacsStoreSCP_h_