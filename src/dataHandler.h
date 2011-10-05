/**
 * \file dataHandler.h
 * \brief File per la gestione dei dati appData
 * \author ICAR-CNR Napoli
 */

#ifndef _dataHandler_h_
#define _dataHandler_h_

#include "appData.h"
#include <list>

using namespace std;


/**
 * \class dataHandler
 * \brief Classe responsabile della gestione dei dati appData
 */
class dataHandler {

private:
	/**
	 * \brief Struttura per associare dati e visualizzatori
	 *
	 * La struttura dataItem contiene il puntatore appData,
	 * l'identificativo del viewer idViewer e il booleano isReference
	 * è vero se l'appData punta a quella di un altro elemento
	 */
	struct dataItem {
		appData* appData;
		unsigned int idViewer;
		bool isReference;
	};

	/** 
	 * \var list<dataItem*> _dataList
	 * \brief Lista di dataItem
	 */
	list<dataItem*> _dataList;

	/** 
	 * \var int _idDataCounter
	 * \brief Contatore degli elementi della lista _dataItem
	 */
	unsigned int _idDataCounter;

public:
	/** Costruttore */
	dataHandler();

	/** Distruttore */
	~dataHandler();

	/**
	 * \fn unsigned int newData(appData* appData, unsigned int idViewer, bool isReference = false)
	 * \brief Crea un nuovo dato nella lista e ne ritorna l'identificatore
	 * \param appData Dato da creare
	 * \param idViewer Identificatore del visualizzatore del dato
	 * \param isReference Se il dato inserito è una reference, il valore deve essere impostato a false, true altrimenti
	 * \return Identificativo del dato
	 */
	unsigned int newData(appData* appData, unsigned int idViewer, bool isReference = false);

	/**
	 * \fn appData* getData(unsigned int idData)
	 * \brief Restituisce il puntatore all'appData, NULL se l'identificativo è errato
	 * \param idData Identificativo del dato da restituire
	 * \return Dato
	 */
	appData* getData(unsigned int idData);

	/**
	 * \fn void deleteData(unsigned int idData)
	 * \brief Elimina prima tutte le reference al dato e poi il dato a cui è associato l'appData
	 * \param idData Identificativo del dato da eliminare
	 */
	void deleteData(unsigned int idData);

	/**
	 * \fn list<unsigned int>* getIdViewerByIdData(unsigned int idData)
	 * \brief Restituisce la lista dei visualizzatori che mostrano un determinato dato
	 * \param idData Identificativo del dato
	 * \return Lista visualizzatori
	 */
	list<unsigned int>* getIdViewerByIdData(unsigned int idData);

	/**
	 * \fn list<unsigned int>* getIdDataByIdViewer(unsigned int idViewer)
	 * \brief Restituisce la lista dei dati mostrati dal viewer idViewer
	 * \param idViewer Identificativo del visualizzatore
	 * \return Lista dati
	 */
	list<unsigned int>* getIdDataByIdViewer(unsigned int idViewer);

	/**
	 * \fn void deleteAllData()
	 * \brief Elimina tutti i dati dalla lista
	 */
	void deleteAllData();

	/**
	 * \fn dataHandler::dataItem* getDataItem(unsigned int idData)
	 * \brief Restituisce il dataItem o NULL
	 * \param idData Identificativo del dato
	 * \return Puntatore al dataItem
	 */
	dataHandler::dataItem* getDataItem(unsigned int idData);

	/**
	 * \fn list < unsigned int > *dataHandler::getAllIdData()
	 * \brief Restituisce tutti i dati
	 * \return Puntatore alla lista dei dati
	 */
	list < unsigned int > *dataHandler::getAllIdData();
};

#endif _dataHandler_h_