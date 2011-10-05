/**
 * \file viewerHandler.h
 * \brief File per la gestione dei visualizzatori
 * \author ICAR-CNR Napoli
 */

#ifndef _viewerHandler_h_
#define _viewerHandler_h_

#include <list>
#include "dataHandler.h"
#include "viewer2d.h"
#include "viewer3d.h"

using namespace std;

/** \class viewerHandler
 * \brief Classe astratta responsabile della gestione dei visualizzatori viewer 
 */
class viewerHandler {
	/** 
	 * \var list<viewer*> _viewerList
	 * \brief Lista visualizzatori
	 */
	list<viewer*> _viewerList;

	/** 
	 * \var unsigned int _idViewerCounter
	 * \brief Contatore visualizzatori
	 */
	unsigned int _idViewerCounter;

	/** 
	 * \var dataHandler _dataHandler
	 * \brief Gestore dati
	 */
	dataHandler _dataHandler;

public:
	/** Costruttore */
	viewerHandler();

	/** Distruttore */
	~viewerHandler();

	/**
	* \fn viewer *new2dViewer(viewerDrawing *viewerDrawing)
	* \brief Crea un nuovo visualizzatore 2D
	* \param viewerDrawing Oggetto che implementa il viewerDrawing
	* \return Restituisce il visualizzatore 2D
	*/
	viewer *new2dViewer(viewerDrawing *viewerDrawing);

	/**
	* \fn viewer *new3dViewer(viewerDrawing *viewerDrawing)
	* \brief Crea un nuovo visualizzatore 3D
	* \param viewerDrawing Oggetto che implementa il viewerDrawing
	* \return Restituisce il visualizzatore 3D
	*/
	viewer *new3dViewer(viewerDrawing *viewerDrawing);

	/**
	* \fn bool quitViewer(unsigned int idViewer)
	* \brief Chiude il visualizzatore
	* \param idViewer Identificativo del visualizzatore da chiudere
	* \return true se idViewer è valido, false altrimenti
	*/
	bool quitViewer(unsigned int idViewer);

	/**
	* \fn virtual unsigned int getActiveViewer() = 0
	* \brief Restituisce l'identificativo del visualizzatore attivo
	* \return Identificativo del visualizzatore
	*/
	virtual unsigned int getActiveViewer() = 0;

	/**
	* \fn dataHandler *getDataHandler()
	* \brief Restituisce il dataHandler
	* \return dataHandler
	*/
	inline dataHandler *getDataHandler() {
		return &_dataHandler;
	}

	/**
	* \fn unsigned int getViewerNumber()
	* \brief Restituisce il numero di visualizzatori
	* \return Numero visualizzatori
	*/
	inline unsigned int getViewerNumber() {
		return _idViewerCounter -1;
	}

	/**
	* \fn viewer *getViewer(unsigned int idViewer)
	* \brief Restituisce il visualizzatore
	* \param idViewer Identificativo del visualizzatore
	* \return Puntatore al visualizzatore
	*/
	viewer *getViewer(unsigned int idViewer);

	/**
	* \fn viewer2d *get2dViewer(unsigned int idViewer)
	* \brief Restituisce il viewer 2d, NULL se il viewer non è stato trovato
	* \param idViewer Identificativo del viewer da restituire
	* \return Puntatore al visualizzatore 2d
	*/
	inline viewer2d *get2dViewer(unsigned int idViewer) {
		return (viewer2d*)getViewer(idViewer);
	}

	/**
	* \fn void updateViewerByIdData(unsigned int idData)
	* \brief Aggiorna il visualizzatore a partire dal dato
	* \param idData Identificativo del dato
	*/
	void updateViewerByIdData(unsigned int idData);

	/**
	* \fn void removeSingleAppData(unsigned int idData)
	* \brief Rimuove un dato
	* \param idData Identificativo del dato da eliminare
	*/
	void removeSingleAppData(unsigned int idData);

	/**
	* \fn void updateViewer()
	* \brief Aggiorna il visualizzatore
	*/
	void updateViewer();

private:
	/**
	* \fn bool addViewer(viewer *viewer)
	* \brief Aggiunge un visualizzatore
	* \param viewer Puntatore al visualizzatore da aggiungere
	* \return true se l'operazione va a buon fine, false altrimenti
	*/
	bool addViewer(viewer *viewer);

	/**
	* \fn void deleteAllViewer()
	* \brief Elimina tutti i visualizzatori
	*/
	void deleteAllViewer();
};

#endif _viewerHandler_h_