/**
 * \file roiManager.h
 * \brief Gestione Roi
 * \author ICAR-CNR Napoli
 */

#ifndef _roiManager_h_
#define _roiManager_h_

#include "imageDrawRoi.h"
#include "globalState.h"
#include "itkImageToVTKImageFilter.h"
#include "roiMacro.h"
#include "itkVtkData.h"
#include <vtkImageAppendComponents.h>
#include <vtkImageShiftScale.h>
#include <wx/event.h>

/**
 * \class roiManager
 * \brief Classe per la gestione delle ROI
 *
 * Questa classe si occupa di gestire tutte le funzionalità inerenti le ROI:
 * crezione, modifica, estrazione e cancellazione; calcolo del volume, dell'area, 
 * dell'angolo e della lunghezza; modifica del colore dei punti e delle linee e 
 * del raggio dei punti.
 *
 */
class roiManager {
	
	/** 
	 * \var bool _verFlip
	 * \brief Tiene memoria delle operazioni di flip verticale effettuate
	 */
	bool _verFlip;

	/** 
	 * \var bool _horFlip
	 * \brief Tiene memoria delle operazioni di flip orizzontale effettuate
	 */
	bool _horFlip;

	/** 
	 * \var imageDrawRoi *_drroi
	 * \brief Gestore dell'immagine ROI
	 */
	imageDrawRoi *_drroi;

	/** 
	 * \var stateRoi _state
	 * \brief Stato corrente della ROI: inattivo, disegno, modifica
	 */
	stateRoi _state;

	/** 
	 * \var vtkImageAppendComponents *_appCom
	 * \brief Prende i componenti dai due input e li unisce in un unico output. Se 
	 *		  Input1 ha M componenti e Input2 ne ha N, l'output avrà M+N componenti 
	 *		  con Input1 prima di Input2
	 */
	vtkImageAppendComponents *_appCom;

	/** 
	 * \var vtkImageData* _vtk
	 * \brief Immagine nel formato VTK risultante dalla pipeline ITK-VTK
	 */
	vtkImageData* _vtk;

	/** 
	 * \var ImageType::SpacingType _spacing
	 * \brief Pixel spacing
	 */
	ImageType::SpacingType _spacing;

	/** 
	 * \var float _mm[2]
	 * \brief Coordinate (x, y) del punto in mm
	 */
	float _mm[2];

	/** 
	 * \var bool _rgb
	 * \brief Indica se l'immagine è in formato RGB
	 */
	bool _rgb;

	/** 
	 * \var vtkImageData* _vtkExtract
	 * \brief Immagine dopo l'estrazione nel formato VTK
	 */
	vtkImageData* _vtkExtract;

	/** 
	 * \var vtkImageData* _vtkImageRoi
	 * \brief Immagine nel formato VTK
	 */
	vtkImageData* _vtkImageRoi;

	/** 
	 * \var ImageType::SizeType _size
	 * \brief Dimensione dell'immagine
	 */
	ImageType::SizeType _size;

	/** 
	 * \var ImageType::Pointer _itkImageExtraction
	 * \brief Immagine nel formato ITK in scala di grigi
	 */
	ImageType::Pointer _itkImageExtraction;

	/** 
	 * \var RGBImageType::Pointer _itkRgbImageExtraction
	 * \brief Immagine nel formato ITK in RGB
	 */
	RGBImageType::Pointer _itkRgbImageExtraction;

	/** 
	 * \var int _typeExtraction
	 * \brief Tipo di estrazione: interna, esterna
	 */
	int _typeExtraction;

	/** 
	 * \var int _sliceExtraction
	 * \brief Slice da estrarre: tutte, corrente, intervallo
	 */
	int _sliceExtraction;

	/** 
	 * \var int _intervalMin
	 * \brief Valore minimo dell'intervallo di estrazione
	 */
	int _intervalMin;

	/** 
	 * \var int	_intervalMax;
	 * \brief Valore massimo dell'intervallo di estrazione
	 */
	int	_intervalMax;


	/** 
	 * \var int _pixelMin
	 * \brief Valore minimo del pixel
	 */
	int _pixelMin;

	/** 
	 * \var int _currentSlice
	 * \brief Slice corrente, valore della Z Slice
	 */	
	int _currentSlice;

	/** 
	 * \var ToolbarModeType _toolbarMode
	 * \brief Modalità della toolbar corrente
	 */
	toolbarModeType _toolbarMode;

	/**
	 * \fn void computeFlipping(vtkImageData* image, char axis)
	 * \brief Filtro che consente di effettuare il flipping di un immagine VTK
	 * \param image Immagine nel formato VTK
	 * \param axis Può assumere valore 'x', 'y' 
	 */
	void computeFlipping(vtkImageData*, char);

public:

    /** Costruttore con parametro itkVtkData */
	roiManager(itkVtkData* data);

	/** Distruttore */	
	~roiManager();

	/**
	 * \fn void OnButtonDown(short interactionType, wxMouseEvent& event, double x, double y)
	 * \brief Gestore degli eventi
	 * \param interactionType Identifica il tipo di ROI richiesta
	 * \param event Identifica l'evento occorso.
	 * \param x Identifica la coordinata x del mouse.
	 * \param y Identifica la coordinata y del mouse.
	 */
	void OnButtonDown(short interactionType, wxMouseEvent& event, double x, double y);

	/**
	 * \fn void OnButtonUp(short interactionType, wxMouseEvent& event, double x, double y)
	 * \brief Gestore degli eventi
	 * \param interactionType Identifica il tipo di ROI richiesta
	 * \param event Identifica l'evento occorso
	 * \param x Identifica la coordinata x del mouse
	 * \param y Identifica la coordinata y del mouse
	 */
	void OnButtonUp(short interactionType, wxMouseEvent& event, double x, double y);

	/**
	 * \fn void OnMotion(short interactionType, wxMouseEvent& event, double x, double y);
	 * \brief Gestore degli eventi
	 * \param interactionType identifica il tipo di Roi richiesta.
	 * \param event Identifica l'evento occorso.
	 * \param x Identifica la coordinata x del mouse.
	 * \param y Identifica la coordinata y del mouse.
	 */
	void OnMotion(short interactionType, wxMouseEvent& event, double x, double y);

	/**
	 * \fn void setSliceRoi(int slice_corrente)
	 * \brief Set della slice corrente
	 * \param slice_corrente Slice corrente
	 */
	void setSliceRoi(int);

	/**
	 * \fn vtkImageData* extraction()
	 * \brief Estrae la ROI
	 * \return L'immagine estratta
	 */
	vtkImageData* extraction();

	/**
	 * \fn int cnInclusion(float p[])
	 * \brief Determina se un punto è interno alla ROI
	 * \param p punto da verificare
	 * \return 0 se cn (crossing number) é pari (out), e 1 se dispari (in)
	 */
	int cnInclusion(float[]);

	/**
	 * \fn ImageDrawRoi* loadRoi(vtkImageData *vtkImage)
	 * \brief Carica la ROI
	 * \param vtkImage Immagine nel formato VTK
	 * \return L'immagine ROI
	 */
	imageDrawRoi* loadRoi(vtkImageData*);

	/**
	 * \fn vtkImageData* itkVtkImage(ImageType::Pointer itk)
	 * \brief Filtro che consente di collegare dinamicamente le pipeline di ITK e VTK in formato scala di grigi
	 * \param itk Immagine nel formato ITK
	 * \return L'immagine nel formato VTK
	 */
	vtkImageData* itkVtkImage(ImageType::Pointer);

	/**
	 * \fn vtkImageData* itkVtkRgbImage(RGBImageType::Pointer itk)
	 * \brief Filtro che consente di collegare dinamicamente le pipeline di ITK e VTK in formato RGB
	 * \param itk Immagine nel formato ITK in RGB
	 * \return L'immagine nel formato VTK
	 */
	vtkImageData* itkVtkRgbImage(RGBImageType::Pointer);

	/**
	 * \fn void deleteAll()
	 * \brief Cancella tutti i punti del contorno della ROI
	 */
	void deleteAll();

	/**
	 * \fn void selectAll()
	 * \brief Seleziona tutti i punti del contorno della ROI
	 */
	void selectAll();

	/**
	 * \fn float angle()
	 * \brief Calcolo dell'angolo formato dalla ROI
	 * \return Il valore dell'angolo
	 */
	float angle();

	/**
	 * \fn float area()
	 * \brief Calcolo dell'area della ROI
	 * \return Il valore dell'area
	 */
	float area();

	/**
	 * \fn float volume()
	 * \brief Calcolo del volume della Roi.
	 * \return il valore del volume.
	 */
	float volume();

	/**
	 * \fn float volumeExtraction()
	 * \brief Calcolo del volume di estrazione della ROI
	 * \return Il valore del volume di estrazione
	 */
	float volumeExtraction();

	/**
	 * \fn float length()
	 * \brief Calcolo della lunghezza del contorno della ROI
	 * \return Il valore della lunghezza
	 */	
	float length();

	/**
	 * \fn void setSelectedPointColor(double r, double g, double b)
	 * \brief Set del colore dei punti selezionati del contorno della ROI
	 * \param r Componente rossa
	 * \param g Componente verde
	 * \param b Componente blu
	 */
	void setSelectedPointColor(double, double, double);
	/**
	 * \fn void setLineColor(double r, double g, double b)
	 * \brief Set del colore delle linee del contorno della ROI
	 * \param r Componente rossa
	 * \param g Componente verde
	 * \param b Componente blu
	 */
	void setLineColor(double, double, double);

	/**
	 * \fn void setPointColor(double r, double g, double b)
	 * \brief Set del colore dei punti del contorno della ROI
	 * \param r Componente rossa
	 * \param g Componente verde
	 * \param b Componente blu
	 */
	void setPointColor(double, double, double);

	/**
	 * \fn void setPointRadius(double radius)
	 * \brief Set del raggio dei punti del contorno della ROI
	 * \param radius Raggio dei punti del contorno della ROI
	 */
	void setPointRadius(double);

	/**
	 * \fn float* convertPixeltoMm(int x, int y)
	 * \brief Converte le coordinate di un punto da pixel a mm
	 * \param x Coordinata x del punto
	 * \param y Coordinata y del punto
	 */
	float* convertPixeltoMm(int, int);

	/**
	 * \fn vtkImageData* getVtkRoi()
	 * \brief Get della Roi in formato VTK
	 * \return L'immagine in formato VTK
	 */
	vtkImageData* getVtkRoi();

	/**
	 * \fn void setVtkRoi(vtkImageData* vtkImage)
	 * \brief Set della ROI in formato VTK
	 * \param vtkImage L'immagine in formato VTK
	 */
	void setVtkRoi(vtkImageData*);

	/**
	 * \fn void setFlipState(bool verFlip, bool horFlip)
	 * \brief Set dello stato del flip verticale ed orizzontale
	 * \param verFlip Stato del flip verticale
	 * \param horFlip Stato del flip orizzontale
	 */
	inline void setFlipState(bool verFlip, bool horFlip){
		_verFlip = verFlip;
		_horFlip = horFlip;
	};

	/**
	 * \fn int getNumPoints()
	 * \brief Get del numero dei punti del contorno della ROI
	 * \return Il numero di punti del contorno della ROI
	 */
	int getNumPoints();
	
	/**
	 * \fn void roiSetMacro(_toolbarMode, toolbarModeType)
	 * \brief Set della modalità della toolbar
	 * \param _toolbarMode Modalità della toolbar
	 */
	roiSetMacro(_toolbarMode, toolbarModeType);

	/**
	 * \fn roiSetMacro(_sliceExtraction, int)
	 * \brief Set delle slice da estrarre
	 * \param _sliceExtraction Slice da estrarre: tutte, corrente, intervallo
	 */
	roiSetMacro(_sliceExtraction, int);
	
	/**
	 * \fn roiSetMacro(_intervalMin, int)
	 * \brief Set del valore min dell'intervallo di estrazione
	 * \param _intervalMin Valore minimo dell'intervallo di estrazione
	 */
	roiSetMacro(_intervalMin, int);

	/**
	 * \fn roiSetMacro(_intervalMax, int)
	 * \brief Set del valore max dell'intervallo di estrazione
	 * \param _intervalMax Valore massimo dell'intervallo di estrazione
	 */
	roiSetMacro(_intervalMax, int);
	/**
	 * \fn roiSetMacro(_typeExtraction, int)
	 * \brief Set del tipo di estrazione
	 * \param _typeExtraction Tipo di estrazione: interna, esterna
	 */
	roiSetMacro(_typeExtraction, int);
	
	/**
	 * \fn roiSetMacro(_currentSlice, int)
	 * \brief Set della slice corrente
	 * \param _sliceCorrente Slice corrente
	 */
	roiSetMacro(_currentSlice, int);

	/**
	 * \fn roiSetMacro(_state, stateRoi)
	 * \brief Set del tipo di estrazione
	 * \param _state Stato della ROI: inattivo, disegno, modifica
	 */
	roiSetMacro(_state, stateRoi);
	

};
#endif _roiManager_h_