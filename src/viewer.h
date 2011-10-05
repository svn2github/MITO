/**
 * \file viewer.h
 * \brief File per la gestione dei dati da visualizzare
 * \author ICAR-CNR Napoli
 */

#ifndef _viewer_h_
#define _viewer_h_

#include <string>
#include "appData.h"
#include "viewerDrawing.h"

using namespace std;

/** 
 * \class viewer
 * \brief Classe atta alla gestione del dato da visualizzare
 */
class viewer {
	/** 
	 * \var unsigned int _idViewer
	 * \brief Identificativo del visualizzatore
	 */
	unsigned int _idViewer;

	/** 
	 * \var long _mouseX
	 * \brief Coordinata x della posizione del mouse
	 */
	long _mouseX;

	/** 
	 * \var long _mouseY
	 * \brief Coordinata y della posizione del mouse
	 */
	long _mouseY;

protected:
	/** 
	 * \var viewerDrawing* _viewerDrawing
	 * \brief Responsabile della visualizzazione del dato
	 */
	viewerDrawing* _viewerDrawing;

public:
	/** Costruttore con parametri viewerDrawing e idViewer */
	viewer(viewerDrawing *viewerDrawing, unsigned int idViewer);

	/** Distuttore */
	virtual ~viewer();

	/**
	 * \fn void show(string title, int x, int y, int w, int h)
	 * \brief Visualizza il dato contenuto nel visualizzatore
	 * \param title Titolo
	 * \param x Coordinata x
	 * \param y Coordinata y
	 * \param w Lunghezza
	 * \param h Altezza
	 */
	void show(string title, int x, int y, int w, int h);

	/**
	 * \fn unsigned int getIdViewer()
	 * \brief Restituisce l'identificativo del visualizzatore
	 * \return Identificativo del visualizzatore
	 */
	inline unsigned int getIdViewer() {
		return _idViewer;
	}

	/**
	 * \fn viewerDrawing *getViewerDrawing()
	 * \brief Restituisce l'oggetto _viewerDrawing
	 * \return L'oggetto _viewerDrawing
	 */
	inline viewerDrawing *getViewerDrawing() {
		return _viewerDrawing;
	}

	/**
	 * \fn appData *newAppData()
	 * \brief Crea un nuovo dato in _viewerDrawing
	 * \return Il puntatore al dato creato
	 */
	inline appData *newAppData() {
		return _viewerDrawing->newAppData();
	}

	/**
	 * \fn unsigned int getActiveIdSingleData()
	 * \brief Restituisce l'identificativo del dato attivo
	 * \return L'identificativo del dato
	 */
	inline unsigned int getActiveIdSingleData() {
		return _viewerDrawing->getActiveIdSingleData();
	}

	/**
	 * \fn void showAppData(appData *appData)
	 * \brief Visualizza un dato specificato
	 * \param appData Il dato da visualizzare
	 */
	inline void showAppData(appData *appData) {
		_viewerDrawing->showAppData(appData);
	}

	/**
	 * \fn void updateViewer()
	 * \brief Aggiorna il visualizzatore
	 */
	inline void updateViewer() {
		if (_viewerDrawing)
			_viewerDrawing->updateViewer();
	}

	/**
	 * \fn void removeSingleAppData()
	 * \brief Rimuove un dato
	 */
	inline void removeSingleAppData() {
		_viewerDrawing->removeSingleAppData();
	}

	/**
	 * \fn void setMouseX(long x)
	 * \brief Assegna la coordinata x della posizione del puntatore del mouse
	 * \param x Coordinata x
	 */
	inline void setMouseX(long x) {
		_mouseX = x;
	}

	/**
	 * \fn void setMouseY(long y)
	 * \brief Assegna la coordinata y della posizione del puntatore del mouse
	 * \param y Coordinata y
	 */
	inline void setMouseY(long y) {
		_mouseY = y;
	}

	/**
	 * \fn long getMouseX()
	 * \brief Restituisce la coordinata x della posizione del puntatore del mouse
	 * \return Coordinata x
	 */
	inline long getMouseX() {
		return _mouseX;
	}

	/**
	 * \fn long getMouseY()
	 * \brief Restituisce la coordinata y della posizione del puntatore del mouse
	 * \return Coordinata y
	 */
	inline long getMouseY() {
		return _mouseY;
	}

	/**
	 * \fn double getXPos()
	 * \brief Restituisce la coordinata x
	 * \return Coordinata x
	 */
	inline double getXPos() {
		return _viewerDrawing->getXPos(_mouseX, _mouseY);
	}

	/**
	 * \fn double getYPos()
	 * \brief Restituisce la coordinata y
	 * \return Coordinata y
	 */
	inline double getYPos() {
		return _viewerDrawing->getYPos(_mouseX, _mouseY);
	}
};

#endif _viewer_h_