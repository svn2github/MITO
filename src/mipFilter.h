/**
 * \file mipFilter.h
 * \brief File per la creazione e la gestione della tecnica MIP (Maximum Intensity Projection)
 * \author ICAR-CNR Napoli
 */

#ifndef _mipFilter_h_
#define _mipFilter_h_

#include "itkVtkFilter.h"

/**
 * \class mipFilter
 * \brief Filtro per la realizzazione del Maximum Intensity Projection che specializza itkVtkFilter
 * 
 * mipFilter è un filtro che viene utilizzato per ottenere, utilizzando
 * la tecnica del RayCasting, i voxel a maggiore intensità presenti in un volume.
 * Il massimo valore viene determinato in modo scalare avendo settato 
 * il MaximizeMethod nel vtkVolumeRayCastMIPFunction. Il colore e l'opacita 
 * ottenuti come risultato nel view plane dipendono rispettivamente dalle funzioni 
 * di trasferimento definite nel vtkVolumeProperty
 */
class mipFilter : itkVtkFilter {

public:
	/** Costruttore con parametri identificativo del dato e gestore dei visualizzatori */
	mipFilter(unsigned int idData, dataHandler *dataHandler);

	/** Distruttore */
	~mipFilter();

	/**
	 * vtkVolume* compute(long wl, long ww, int clut, int minPixel, bool gaussFilter)
	 * \brief Utilizzato per effettuare l'operazione di filtraggio per realizzare il MIP tramite il RayCasting
	 * \param wl Window level
	 * \param ww Window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param minPixel Minimo valore dell'intensità dei pixel
	 * \param gaussFilter Abilita il filtro di smoothing gaussiano
	 */
    vtkVolume* compute(long wl, long ww, int clut, int minPixel, bool gaussFilter, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	/**
	 * vtkVolume* computeRgb(long wl, long ww, int clut, bool gaussFilter)
	 * \brief Utilizzato per effettuare l'operazione di filtraggio per realizzare il MIP tramite il RayCasting in formato RGB
	 * \param wl Window level
	 * \param ww Window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param gaussFilter Abilita il filtro di smoothing gaussiano
	 */
    vtkVolume* computeRgb(long wl, long ww, int clut, bool gaussFilter, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);
};

#endif _mipFilter_h_