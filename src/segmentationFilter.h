/**
 * \file segmentationFilter.h
 * \brief File per la definizione dei dati in ITK e in VTK
 * \author ICAR-CNR Napoli
 */

#ifndef _segmentationFilter_h_
#define _segmentationFilter_h_

#include "itkVtkFilter.h"


/** 
 * \class segmentationFilter
 * \brief Filtro ITK per la segmentazione di immagini ITK. Deriva da itkVtkFilter
 */
class segmentationFilter : itkVtkFilter {
public:

	/**
	* \brief Tipo enumerativo che rappresenta le differenti tecniche di segmentazione
	*/
	enum algoType {
		notDefined,
		connectedThreshold,
		neighborhoodConnected,
		confidenceConnected
	};

	/** Costruttore con parametri identificativo del dato e gestore dei dati */
	segmentationFilter(unsigned int idData, dataHandler *dataHandler);

	/** Distruttore */
	~segmentationFilter();

	/**
	* \fn void computeRegionGrowing(ImageType::IndexType xy, PixelType seedLowerLimit, PixelType seedUpperLimit, float xRadius, float yRadius, float multiplier, int numOfIterations, algoType algoType_)
	* \brief Esegue la segmentazione
	* \param xy Coordinate del seme
	* \param seedLowerLimit Limite inferiore del seme
	* \param seedUpperLimit Limite superiore del seme
	* \param xRadius Lunghezza orizzontale del raggio
	* \param yRadius Lunghezza verticale del raggio
	* \param multiplier Moltiplicatore 
	* \param numOfIterations Numero di iterazioni
	* \param algoType_ Tecnica di segmenrazione
	*/
	void computeRegionGrowing(ImageType::IndexType xy, PixelType seedLowerLimit, PixelType seedUpperLimit, float xRadius, float yRadius, float multiplier, int numOfIterations, algoType algoType_);

	/**
	* \fn void setItkVtkData(itkVtkData* data)
	* \brief Assegna il dato
	* \param data Dato da assegnare
	*/
	inline void setItkVtkData(itkVtkData* data) {
		setItkVtkData(data);
	}
};

#endif _segmentationFilter_h_