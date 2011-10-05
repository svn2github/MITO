/**
 * \file viewerDrawing.h
 * \brief File per la visualizzazione dei dati contenuti nei viewer
 * \author ICAR-CNR Napoli
 */

#ifndef _viewerDrawing_h_
#define _viewerDrawing_h_

#include "appData.h"
#include <string>

using namespace std;

/**
 * \class viewerDrawing
 * \brief Classe astratta per la visualizzazione dei dati contenuti nei viewer
 */
class viewerDrawing {
protected:
	/* \var int _idViewer
	 * \brief Identificativo del visualizzatore
	 */
	unsigned int _idViewer;

public:
	/** Costruttore */
	viewerDrawing();

	/** Distruttore */
	virtual ~viewerDrawing();

	/**
	 * \fn virtual void show(unsigned int idViewer, string title, int x, int y, int w, int h) = 0
	 * \brief Metodo virtuale puro per la visualizzazione del dato contenuto nel visualizzatore
	 * \param idViewer Identificativo del visualizzatore
	 * \param title Titolo
	 * \param x Coordinata x
	 * \param y Coordinata y
	 * \param w Lunghezza
	 * \param h Altezza
	 */
	virtual void show(unsigned int idViewer, string title, int x, int y, int w, int h) = 0;

	/**
	 * \fn unsigned int getIdViewer()
	 * \brief Restituisce l'identificativo del visulizzatore
	 * \return Identificativo del visulizzatore
	 */
	inline unsigned int getIdViewer() {
		return _idViewer;
	}

	/**
	 * \fn void Delete()
	 * \brief Elimina il visualizzatore
	 */
	virtual void Delete() {};

	/**
	 * \fn void Delete()
	 * \brief Distrugge la finestra
	 */
	virtual void destroyWindow(){};

	/**
	 * \fn virtual appData *newAppData() = 0
	 * \brief Metodo virtuale puro che crea un nuovo dato
	 * \return Il puntatore al nuovo dato
	 */
	virtual appData *newAppData() = 0;

	/**
	 * \fn virtual void showAppData(appData *appData) = 0
	 * \brief Metodo virtuale puro che visualizza un dato
	 * \param appData Dato da visualizzare
	 */
	virtual void showAppData(appData *appData) = 0;

	/**
	 * \fn virtual unsigned int getActiveIdSingleData() = 0
	 * \brief Metodo virtuale puro che restituisce l'identificativo del dato attivo
	 * \return Identificativo del dato
	 */
	virtual unsigned int getActiveIdSingleData() = 0;

	/**
	 * \fn virtual void removeSingleAppData() = 0
	 * \brief Metodo virtuale puro che rimuove un dato
	 */
	virtual void removeSingleAppData() = 0;

	/**
	 * \fn virtual void updateViewer() = 0
	 * \brief Metodo virtuale puro che aggiorna il visualizzatore
	 */
	virtual void updateViewer() = 0;

	/**
	 * \fn virtual double getXPos(long mouseXPos, long mouseYPos) = 0
	 * \brief Metodo virtuale puro che restituisce la coordinata x a partire dalla posizione del puntatore del mouse
	 * \param mouseXPos Coordinata x della posizione del puntatore del mouse
	 * \param mouseYPos Coordinata y della posizione del puntatore del mouse
	 * \return Coordinata x
	 */
	virtual double getXPos(long mouseXPos, long mouseYPos) = 0;

	/**
	 * \fn virtual double getYPos(long mouseXPos, long mouseYPos) = 0
	 * \brief Metodo virtuale puro che restituisce la coordinata y a partire dalla posizione del puntatore del mouse
	 * \param mouseXPos Coordinata x della posizione del puntatore del mouse
	 * \param mouseYPos Coordinata y della posizione del puntatore del mouse
	 * \return Coordinata y
	 */
	virtual double getYPos(long mouseXPos, long mouseYPos) = 0;

	/**
	 * \fn virtual int getCurrentSlice() = 0
	 * \brief Metodo virtuale puro che restituisce il numero della slice corrente
	 * \return Numero della slice corrente
	 */
	virtual int getCurrentSlice() = 0;

	/**
	 * \fn virtual void nextSlice()
	 * \brief Carica la slice successiva
	 */
	virtual void nextSlice() {};

	/**
	 * \fn virtual void prevSlice()
	 * \brief Carica la slice precedente
	 */
	virtual void prevSlice() {};

	/**
	 * \fn virtual void setSlice(int zSlice)
	 * \brief Assegna una slice
	 * \param zSlice Slice da assegnare
	 */
	virtual void setSlice(int zSlice) {};

	/**
	 * \fn virtual int getSliceNumber(appData *appData) = 0
	 * \brief Metodo virtuale puro che restituisce il numero delle slice
	 * \return Numero delle slice
	 */
	virtual int getSliceNumber(appData *appData) = 0;
};

#endif _viewerDrawing_h_