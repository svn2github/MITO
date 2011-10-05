/**
 * \file wxVtkViewer3d.h
 * \brief File per la gestione del visualizzatore 3D
 * \author ICAR-CNR Napoli
 */

#ifndef _wxVtkViewer3d_h_
#define _wxVtkViewer3d_h_

#include "wxVtkViewer.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkTextActor.h>


enum algorithmType3d {
	undefined,
	rayCast,
    surfaceRendering
};

/**
 * \class wxVtkViewer3d
 * \brief Classe per la gestione del visualizzatore 3D. Deriva da wxVtkViewer
 */
class wxVtkViewer3d : public wxVtkViewer {
protected:
	/** 
	 * \var vtkRenderer *_ren
	 * \brief Renderer VTK
	 */
	vtkRenderer *_ren;

	/** 
	 * \var vtkRenderWindow *_rWin
	 * \brief Finestra conentente il renderer VTK
	 */
	vtkRenderWindow *_rWin;

	/** 
	 * \var itkVtkData* _itkVtkData
	 * \brief Dato da visualizzare
	 */
	itkVtkData *_itkVtkData;

	/** 
	 * \var vtkTextActor *_leftTopInfo
	 * \brief Informazioni poste in alto a sinistra del visualizzatore
	 */
	vtkTextActor *_leftTopInfo;

	/** 
	 * \var algorithmType3d _algorithmType
	 * \brief Tipo di algoritmo di ricostruzione 3d
	 */
	algorithmType3d _algorithmType;

public:
	/** Costruttore con parametri la finestra padre e l'identificativo della finestra */
	wxVtkViewer3d(wxWindow *parent, wxWindowID id);

	/** Costruttore con parametri la finestra padre, una finestra e l'identificativo della finestra */
	wxVtkViewer3d(wxWindow *parent, wxWindow *gui, wxWindowID id);

	/** Distruttore */
	~wxVtkViewer3d();

	/**
	 * \fn void showItkVtkData(itkVtkData *itkVtkData)
	 * \brief Visualizza un dato itkVtkData
	 * \param itkVtkData Dato da visualizzare
	 */
	void showItkVtkData(itkVtkData *itkVtkData);

	/**
	 * \fn void updateViewer()
	 * \brief Aggiorna il visualizzatore
	 */
	void updateViewer();

	/**
	 * \fn void removeSingleAppData()
	 * \brief Rimuove un dato dal visualizzatore
	 */
	void removeSingleAppData();

	/**
	 * \fn void Delete()
	 * \brief Distrugge il visualizzatore
	 */
	void Delete();

	/**
	 * \fn void setTextActor()
	 * \brief Visualizza le informazioni sul visualizzatore
	 */
	void setTextActor();

	/**
	 * \fn void setText()
	 * \brief Visualizza informazioni sul visualizzatore
	 */
	void setText();

	/**
	 * \fn void showLeftTopInfo()
	 * \brief Rende visibili le informazioni sul visualizzatore
	 */
	void showLeftTopInfo();

	/**
	 * \fn void showLeftTopInfo()
	 * \brief Nasconde le informazioni sul visualizzatore
	 */
	void hideLeftTopInfo();

	/**
	 * \fn algorithmType3d getAlgorithm()
	 * \brief Ritorna il nome dell'algoritmo di ricostruzione 3D
	 * \return Tipo dell'algoritmo di ricostruzione 3D
	 */
	algorithmType3d getAlgorithm();

	/**
	 * \fn void setAlgorithm(algorithmType3d algorithmType)
	 * \brief Assegna il nome dell'algoritmo di ricostruzione 3D
	 * \param algorithmType Indica il tipo di algoritmo di ricostruzione 3d
	 */
	void setAlgorithm(algorithmType3d algorithmType);

	/**
	 * \fn double getXPos(long mouseXPos, long mouseYPos)
	 * \brief Restituisce la coordinata x a partire dalla posizione del puntatore del mouse
	 * \param mouseXPos Coordinata x della posizione del puntatore del mouse
	 * \param mouseYPos Coordinata y della posizione del puntatore del mouse
	 * \return Coordinata x
	 */
	inline double getXPos(long mouseXPos, long mouseYPos) {
		return 0;
	}
	/**
	 * \fn double getYPos(long mouseXPos, long mouseYPos)
	 * \brief Restituisce la coordinata y a partire dalla posizione del puntatore del mouse
	 * \param mouseXPos Coordinata x della posizione del puntatore del mouse
	 * \param mouseYPos Coordinata y della posizione del puntatore del mouse
	 * \return Coordinata y
	 */
	inline double getYPos(long mouseXPos, long mouseYPos) {
		return 0;
	}

	/**
	 * \fn int getCurrentSlice()
	 * \brief Restituisce il numero della slice corrente
	 * \return Numero della slice corrente
	 */
	inline int getCurrentSlice() {
		return 0;
	}

	/**
	 * \fn unsigned int getActiveIdSingleData()
	 * \brief Restituisce l'identificativo del dato attivo
	 * \return Identificativo del dato
	 */
	inline unsigned int getActiveIdSingleData() {
		if (_itkVtkData)
			return _itkVtkData->getIdData();
		else
			return 0;
	}

	/**
	 * \fn vtkRenderWindow* getRenderWindow()
	 * \brief Restituisce il puntatore alla finestra contenente il renderer
	 * \return Puntatore alla finestra contenente il renderer
	 */
	inline vtkRenderWindow* getRenderWindow() {
		return _rWin;
	}

	/**
	 * \fn vtkRenderer* getRenderer()
	 * \brief Restituisce il al renderer
	 * \return Puntatore al renderer
	 */
	inline vtkRenderer* getRenderer() {
		return _ren;
	}

};


#endif _wxVtkViewer3d_h_