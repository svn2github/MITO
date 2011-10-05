/**
 * \file flipFilter.h
 * \brief File per la realizzazione del flip delle immagini
 * \author ICAR-CNR Napoli
 */

#ifndef _flipFilter_h_
#define _flipFilter_h_

#include "itkVtkFilter.h"
#include <itkFlipImageFilter.h>
#include <vtkImageFlip.h>


/** \class flipFilter
* \brief Filtro per il flipping di immagini. Deriva da itkVtkFilter
*/
class flipFilter : itkVtkFilter {

public:
	/** Costruttore con parametri identificativo del dato e gestore dei visualizzatori */
	flipFilter(unsigned int idData, dataHandler* dataHandler);

	/** Distruttore */
	~flipFilter();

	/**
	 * \fn void computeITK(char axis)
	 * \brief Effettua il flip di un'immagine DICOM con ITK; i valori possibili sono 'x', 'y' e 'z'
	 * \param axis Asse rispetto al quale effettuare il flip
	 */
	void computeITK(char axis);

	/**
	 * \fn void computeITK(char axis)
	 * \brief Effettua il flip di un'immagine DICOM con VTK; i valori possibili sono 'x', 'y' e 'z'
	 * \param axis Asse rispetto al quale effettuare il flip
	 */
	void computeVTK(char axis);
};

#endif _flipFilter_h_