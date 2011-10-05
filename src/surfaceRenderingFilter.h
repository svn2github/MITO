/**
 * \file surfaceRenderingFilter.h
 * \brief File per la realizzazione del surface rendering
 * \author ICAR-CNR Napoli
 */

#ifndef _surfaceRenderingFilter_h_
#define _surfaceRenderingFilter_h_

#include "itkVtkFilter.h"

/** 
 * \class surfaceRenderingFilter
 * \brief Classe per la realizzazione del surface rendering. Deriva da itkVtkFilter
 */
class surfaceRenderingFilter : itkVtkFilter {

public:
	/** Costruttore con parametri identificativo del dato e gestore dei dati */
	surfaceRenderingFilter(unsigned int idData, dataHandler *dataHandler);

	/** Distruttore */
	~surfaceRenderingFilter();

	/**
	* \fn vtkActor* compute(float contourValue)
	* \brief Esegue il surface rendering
	* \param contourValue Isovalore a partire dal quale creare le isosuperfici
	* \return Il modello volumetrico
	*/
	vtkActor* compute(float contourValue, double decimateValue, double smoothValue, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower, double* colourAmbient, double* colourDiffuse, double* colourSpecular );
};

#endif _surfaceRenderingFilter_h_