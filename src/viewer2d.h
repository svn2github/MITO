/**
 * \file viewer2d.h
 * \brief File per la gestione dei dati da visualizzare in modalità 2D
 * \author ICAR-CNR Napoli
 */

#ifndef _viewer2d_h_
#define _viewer2d_h_

#include "viewer.h"

/** 
 * \class viewer2d
 * \brief Classe atta alla gestione del dato da visualizzare in modalità 2D. Deriva da viewer
 */
class viewer2d : public viewer {

public:
	/** Costruttore con parametri viewerDrawing e identificativo del visualizzatore */
	viewer2d(viewerDrawing *viewerDrawing, unsigned int id);

	/** Distruttore */
	virtual ~viewer2d();

	/**
	 * \fn void nextSlice()
	 * \brief Carica la slice successiva
	 */
	inline void nextSlice() {
		_viewerDrawing->nextSlice();
	}

	/**
	 * \fn void prevSlice()
	 * \brief Carica la slice precedente
	 */
	inline void prevSlice() {
		_viewerDrawing->prevSlice();
	}

	/**
	 * \fn int getCurrentSlice()
	 * \brief Restituisce la slice corrente
	 * \return La slice corrente
	 */
	inline int getCurrentSlice() {
		return _viewerDrawing->getCurrentSlice();
	}

	/**
	 * \fn void setSlice(int zSlice)
	 * \brief Assegna una slice
	 * \param zSlice La slice da assegnare
	 */
	inline void setSlice(int zSlice) {
		_viewerDrawing->setSlice(zSlice);
	}

	/**
	 * \fn int getSliceNumber(appData* appData)
	 * \brief Restituisce il numero di slice di un dato specificato
	 * \param appData Dato da cui calcolare il numero di slice
	 * \return Numero slice
	 */
	inline int getSliceNumber(appData* appData) {
		return _viewerDrawing->getSliceNumber(appData);
	}
};

#endif _viewer2d_h_