/**
 * \file wxVtkViewer2d.h
 * \brief File per la gestione del visualizzatore 2D
 * \author ICAR-CNR Napoli
 */

#ifndef _wxVtkViewer2d_h_
#define _wxVtkViewer2d_h_

#include "wxVtkViewer.h"
#include "dataHandler.h"
#include <vtkImageViewer2.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkCoordinate.h>
#include <vtkCamera.h>
#include <vtkTextActor.h> 
#include <wx/event.h>

/**
 * \class wxVtkViewer2d
 * \brief Classe per la gestione del visualizzatore 2D. Deriva da wxVtkViewer e vtkImageViewer2
 */
class wxVtkViewer2d : public wxVtkViewer, public vtkImageViewer2 {
protected:
	/** 
	 * \var itkVtkData* _itkVtkData
	 * \brief Dato da visualizzare
	 */
	itkVtkData* _itkVtkData;

	/** 
	 * \var vtkCoordinate* _coordinates
	 * \brief Coordinate
	 */
	vtkCoordinate* _coordinates;

	/** 
	 * \var vtkTextActor* _leftTopInfo
	 * \brief Informazioni poste in alto a sinistra del visualizzatore
	 */
	vtkTextActor* _leftTopInfo;

	/** 
	 * \var vtkTextActor* _rightTopInfo
	 * \brief Informazioni poste in alto a destra del visualizzatore
	 */
	vtkTextActor* _rightTopInfo;

	/** 
	 * \var vtkTextActor* _leftBottomInfo
	 * \brief Informazioni poste in basso a sinistra del visualizzatore
	 */
	vtkTextActor* _leftBottomInfo;

	/** 
	 * \var vtkTextActor* _rightBottomInfo
	 * \brief Informazioni poste in basso a destra del visualizzatore
	 */
	vtkTextActor* _rightBottomInfo;

	/** 
	 * \var vtkTextActor* _RoiLengthAreaVolume
	 * \brief Informazioni relative alla lunghezza, l'area ed il volume di una ROI
	 */
	vtkTextActor* _RoiLengthAreaVolume;

	/** 
	 * \var vtkTextActor* _RoiAngle
	 * \brief Informazioni relative all'angolo di un arco individuato da una ROI
	 */
	vtkTextActor* _RoiAngle;

	/** 
	 * \var double initialZoom
	 * \brief Impostazioni iniziali dello zoom
	 */
	double initialZoom;

	/** 
	 * \var PixelType _pixelValue
	 * \brief Intensità del pixel
	 */
	PixelType _pixelValue;

	/** 
	 * \var RGBPixelType _pixelValueRgb
	 * \brief Valore dei canali R, G e B del pixel
	 */
	RGBPixelType _pixelValueRgb;

	/** 
	 * \var int _x
	 * \brief Coordinata _x
	 */
	int _x;

	/** 
	 * \var int _y
	 * \brief Coordinata _y
	 */
	int _y;

	/** 
	 * \var bool _horFlip
	 * \brief Indica se l'immagine è stata flippata rispetto all'asse delle ascisse
	 */
	bool _horFlip;

	/**
	 * \var bool _verFlip
	 * \brief Indica se l'immagine è stata flippata rispetto all'asse delle ordinate
	 */
	bool _verFlip;

public:
	/** Costruttore con parametri la finestra padre, una finestra e l'identificativo della finestra */
	wxVtkViewer2d(wxWindow *parent, wxWindow *gui, wxWindowID id);

	/** Distruttore */
	~wxVtkViewer2d();

	/**
	 * \fn void showItkVtkData(itkVtkData *itkVtkData)
	 * \brief Visualizza un dato itkVtkData
	 * \param itkVtkData Dato da visualizzare
	 */
	void showItkVtkData(itkVtkData *itkVtkData);

	/**
	 * \fn void setText(infoFilter infoFilter, int posX, int posY)
	 * \brief Visualizza informazioni sul visualizzatore
	 * \param infoFilter Informazioni da visualizzare
	 * \param posX Ascissa della posizione dell'informazione sul visualizzatore
	 * \param posY Ordinata della posizione dell'informazione sul visualizzatore
	 */
	void setText(infoFilter infoFilter, int posX, int posY);

	/**
	 * \fn void Delete()
	 * \brief Distrugge il visualizzatore
	 */
	void Delete();

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
	 * \fn vtkInteractorStyleImage* getInteractor()
	 * \brief Restituisce lo stile dell'interactor
	 * \return Stile dell'interactor
	 */
	inline vtkInteractorStyleImage* getInteractor() {
		return this->InteractorStyle;	
	}

	/**
	 * \fn vtkCamera* getActiveCamera()
	 * \brief Restituisce la camera attiva
	 * \return Camera attiva
	 */
	inline vtkCamera* getActiveCamera() {
		return Renderer->GetActiveCamera();	
	}

	/**
	 * \fn void setActiveCamera(vtkCamera* camera)
	 * \brief Setta la camera attiva
	 * \param camera Camera da assegnare
	 */
	inline void setActiveCamera(vtkCamera* camera) {
		Renderer->SetActiveCamera(camera);	
	}

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
	 * \fn virtual void nextSlice()
	 * \brief Carica la slice successiva
	 */
	void nextSlice();

	/**
	 * \fn virtual void prevSlice()
	 * \brief Carica la slice precedente
	 */
	void prevSlice();

	/**
	 * \fn virtual void prevSlice()
	 * \brief Assegna la slice corrente e aggiorna il visualizzatore
	 */
	void setSlice(int zSlice);

	/**
	 * \fn double getXPos(long mouseXPos, long mouseYPos)
	 * \brief Restituisce la coordinata x a partire dalla posizione del puntatore del mouse
	 * \param mouseXPos Coordinata x della posizione del puntatore del mouse
	 * \param mouseYPos Coordinata y della posizione del puntatore del mouse
	 * \return Coordinata x
	 */
	double getXPos(long mouseXPos, long mouseYPos);

	/**
	 * \fn double getYPos(long mouseXPos, long mouseYPos)
	 * \brief Restituisce la coordinata y a partire dalla posizione del puntatore del mouse
	 * \param mouseXPos Coordinata x della posizione del puntatore del mouse
	 * \param mouseYPos Coordinata y della posizione del puntatore del mouse
	 * \return Coordinata y
	 */
	double getYPos(long mouseXPos, long mouseYPos);

	/**
	 * \fn int getCurrentSlice()
	 * \brief Restituisce il numero della slice corrente
	 * \return Numero della slice corrente
	 */
	inline int getCurrentSlice() {
		return Slice;
	}

	/**
	 * \fn itkVtkData* getItkVtkData()
	 * \brief Restituisce il puntatore al dato itkVtkData
	 * \return Puntatore al dato itkVtkData
	 */
	inline itkVtkData* getItkVtkData() {
		return _itkVtkData;
	}

	/**
	 * \fn int getCurrentX()
	 * \brief Restituisce l'ascissa corrente
	 * \return Ascissa corrente
	 */
	inline int getCurrentX() {
		return _x;
	}

	/**
	 * \fn int getCurrentPixelValue()
	 * \brief Restituisce l'intensità del pixel corrente
	 * \return Intensità del pixel corrente
	 */
	inline int getCurrentPixelValue() {
		return _pixelValue;
	}

	/**
	 * \fn int getCurrentY()
	 * \brief Restituisce l'ordinata corrente
	 * \return Ordinata corrente
	 */
	inline int getCurrentY() {
		return _y;
	}

	/**
	 * \fn void setVerFlip()
	 * \brief Applica un flip verticale all'immagine
	 */
	inline void setVerFlip() {
		_verFlip = ! _verFlip;
	}

	/**
	 * \fn void setVerFlip()
	 * \brief Applica un flip orizzontale all'immagine
	 */
	inline void setHorFlip() {
		_horFlip = ! _horFlip;
	}

	/**
	 * \fn bool getVerFlip()
	 * \brief Resituisce il valore della variabile _verFlip
	 * \return Valore della variabile _verFlip
	 */
	inline bool getVerFlip() {
		return _verFlip;
	}

	/**
	 * \fn bool getHorFlip()
	 * \brief Resituisce il valore della variabile _horFlip
	 * \return Valore della variabile _horFlip
	 */
	inline bool getHorFlip() {
		return _horFlip;
	}

	/**
	 * \fn void setRoiLengthAreaVolume(wxMouseEvent& event, float length, float area, float volume)
	 * \brief Assegna la lunghezza, l'area ed il volume di una regione di interesse (ROI)
	 * \param event Evento del mouse
	 * \param length Lunghezza della ROI
	 * \param area Area della ROI
	 * \param volume Volume della ROI
	 */
	void setRoiLengthAreaVolume(wxMouseEvent& event, float length, float area, float volume);

	/**
	 * \fn void setRoiAngle(wxMouseEvent& event, float angle)
	 * \brief Assegna l'angolo di un arco individuato da una ROI
	 * \param event Evento del mouse
	 * \param angle Angolo dell'arco individuato dalla ROI
	 */
	void setRoiAngle(wxMouseEvent& event, float angle);

	/**
	 * \fn void removeRoiLengthAreaVolume()
	 * \brief Rimuove le informazioni sulla lunghezza, l'area ed il volume di una regione di interesse (ROI)
	 */
	void removeRoiLengthAreaVolume();

	/**
	 * \fn void removeRoiAngle()
	 * \brief Rimuove le informazioni sull'angolo di un arco individuato da una ROI
	 */
	void removeRoiAngle();


private:
	/**
	 * \fn void setTextActor()
	 * \brief Visualizza le informazioni sul visualizzatore
	 */
	void setTextActor();

	/**
	 * \fn void setRoiLengthAreaVolumeActor(wxMouseEvent& event)
	 * \brief Visualizza le informazioni relative alla lunghezza, l'area ed il volume di una ROI sul visualizzatore
	 * \param event Evento del mouse
	 */
	void setRoiLengthAreaVolumeActor(wxMouseEvent& event);

	/**
	 * \fn void setRoiAngleActor(wxMouseEvent& event)
	 * \brief Visualizza le informazioni relative all'angolo di un arco individuato da una ROI sul visualizzatore
	 * \param event Evento del mouse
	 */
	void setRoiAngleActor(wxMouseEvent& event);
};

#endif _wxVtkViewer2d_h_