/**
 * \file wxVtkViewer.h
 * \brief File per la gestione del visualizzatore
 * \author ICAR-CNR Napoli
 */

#ifndef _wxVtkViewer_h_
#define _wxVtkViewer_h_

#include "appWxVtkInteractor.h"
#include "itkVtkData.h"
#include "viewerDrawing.h"

/**
 * \class wxVtkViewer
 * \brief Classe astratta per la gestione del visualizzatore. Deriva da viewerDrawing
 */
class wxVtkViewer : public viewerDrawing {

protected:
	/** 
	 * \var appWxVtkInteractor *_iren
	 * \brief Oggetto atto a far interagire l'utente con la finestra
	 */
	appWxVtkInteractor *_iren;

	/** 
	 * \var wxWindow *_parent
	 * \brief Finestra padre
	 */
	wxWindow *_parent;

public:
	/** Costruttore con parametri la finestra padre, una finestra e l'identificativo della finestra */
	wxVtkViewer(wxWindow* parent, wxWindow *gui, wxWindowID id);

	/** Distruttore */
	virtual ~wxVtkViewer();

	/**
	* \fn void destroyWindow()
	* \brief Distrugge la finestra
	*/
	void destroyWindow();

	/**
	 * \fn void show(unsigned int idViewer, string title, int x, int y, int w, int h)
	 * \brief Visualizza il dato contenuto nel visualizzatore
	 * \param idViewer Identificativo del visualizzatore
	 * \param title Titolo
	 * \param x Coordinata x
	 * \param y Coordinata y
	 * \param w Lunghezza
	 * \param h Altezza
	 */
	void show(unsigned int idViewer, string title, int x, int y, int w, int h);

	/**
	 * \fn appData *newAppData()
	 * \brief Crea un nuovo dato
	 * \return Il puntatore al nuovo dato
	 */
	appData *newAppData();

	/**
	 * \fn void showAppData(appData *appData)
	 * \brief Visualizza un dato
	 * \param appData Dato da visualizzare
	 */
	void showAppData(appData *appData);

	/**
	 * \fn int getSliceNumber(appData *appData)
	 * \brief Restituisce il numero delle slice
	 * \return Numero delle slice
	 */
	int getSliceNumber(appData *appData); 

	/**
	 * \fn virtual void showItkVtkData(itkVtkData *itkVtkData) = 0
	 * \brief Metodo virtuale puro che visualizza un dato itkVtkData
	 * \param itkVtkData Dato da visualizzare
	 */
	virtual void showItkVtkData(itkVtkData *itkVtkData) = 0;

	/**
	 * \fn unsigned int getActiveIdSingleData()
	 * \brief Metodo virtuale puro che restituisce l'identificativo del dato attivo
	 * \return Identificativo del dato
	 */
	virtual unsigned int getActiveIdSingleData() = 0;

	/**
	 * \fn virtual void updateViewer() = 0
	 * \brief Metodo virtuale puro che aggiorna il visualizzatore
	 */
	virtual void updateViewer() = 0;

	/**
	 * \fn virtual void removeSingleAppData() = 0
	 * \brief Metodo virtuale puro che rimuove un dato dal visualizzatore
	 */
	virtual void removeSingleAppData() = 0;

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
	 * \fn wxWindow *getWxWindow()
	 * \brief Restituisce il puntatore all'oggetto atto a far interagire l'utente con la finestra
	 */
	inline wxWindow *getWxWindow() {
		return _iren;
	}
};

#endif _wxVtkViewer_h_