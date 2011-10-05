/**
 * \file segmentationManager.h
 * \brief Gestione segmentazione
 * \author ICAR-CNR Napoli
 */

#ifndef _segmentationManager_h_
#define _segmentationManager_h_

#include "viewerHandler.h"
#include "infoFilter.h"
#include <vtkImageData.h>

/**
 * \class segmentationManager
 * \brief Classe per la gestione della Segmentazione
 *
 * Questa classe si occupa di gestire tutte le funzionalità inerenti la segmentazione
 */
class segmentationManager {
	/** 
	 * \var viewerHandler* _viewerHandler
	 * \brief Gestore dei visualizzatori
	 */
	viewerHandler* _viewerHandler;

	/** 
	 * \var int _idData
	 * \brief identificativo del dato
	 */
	unsigned int _idData;

	/** 
	 * \var unsigned int _idViewer
	 * \brief Identificativo del visualizzatore
	 */
	unsigned int _idViewer;

	/** 
	 * \var bool _verFlip
	 * \brief Indica se è stato effettuato un flip verticale
	 */
	bool _verFlip;

	/** 
	 * \var bool _verFlip
	 * \brief Indica se è stato effettuato un flip orizzontale
	 */
	bool _horFlip;

	/** 
	 * \var int _x
	 * \brief Coordinata x del pixel
	 */
	int _x;

	/** 
	 * \var int _y
	 * \brief Coordinata y del pixel
	 */
	int _y;

	/** 
	 * \var short _pixelValue
	 * \brief Intensità del pixel
	 */
	short _pixelValue;

	/** 
	 * \var ImageType::IndexType _seed
	 * \brief Seme
	 */
	ImageType::IndexType _seed;

public:
	/** Costruttore con parametro viewerHandler */
	segmentationManager(viewerHandler* viewerHandler);

	/** Distruttore */
	~segmentationManager();
	
	/**
	 * \fn void initializeSegmentationManager()
	 * \brief Inizializza il gestore della segmentazione
	 */
	void initializeSegmentationManager();

	/**
	 * \fn bool computeConnectedThresholdAlgorithm()
	 * \brief Effettua la segmentazione secondo la tecnica Connected Threshold
	 * \return true se l'operazione è andata a buon fine
	 */
	bool computeConnectedThresholdAlgorithm();

	/**
	 * \fn bool computeNeighborhoodConnectedAlgorithm()
	 * \brief Effettua la segmentazione secondo la tecnica Neighborhood Connected
	 * \return true se l'operazione è andata a buon fine
	 */
	bool computeNeighborhoodConnectedAlgorithm();

	/**
	 * \fn bool computeConfidenceConnectedAlgorithm()
	 * \brief Effettua la segmentazione secondo la tecnica Confidence Connected
	 * \return true se l'operazione è andata a buon fine
	 */
	bool computeConfidenceConnectedAlgorithm();

	/**
	 * \fn void setFlipState(bool verFlip, bool horFlip)
	 * \brief Setta lo stato del flip
	 * \param verFlip Flip verticale
	 * \param horFlip Flip orizzontale
	 */
	inline void setFlipState(bool verFlip, bool horFlip) {
		_verFlip = verFlip;
		_horFlip = horFlip;
	}

private:
	/**
	 * \fn void computeFlipping(vtkImageData* image, char axis)
	 * \brief Effettua il flipping con VTK
	 * \param image Immagine da flippare
	 * \param axis Asse ripetto il quale effettuare il flipping: può essere 'x', 'y' o 'z'
	 */
	void computeFlipping(vtkImageData* image, char axis);

};

#endif _segmentationManager_h_