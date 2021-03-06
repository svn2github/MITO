/**
 * \file volumeRenderingFilter.h
 * \brief File per la realizzazione del volume rendering
 * \author ICAR-CNR Napoli
 */

#ifndef _volumeRenderingFilter_h
#define _volumeRenderingFilter_h

#include "itkVtkFilter.h"
#include <vtkRenderer.h>

/** 
 * \class volumeRenderingFilter
 * \brief Classe per la realizzazione del volume rendering. Deriva da itkVtkFilter
 */
class volumeRenderingFilter : itkVtkFilter {
public:
	/** Costruttore con parametri identificativo del dato e gestore dati */
	volumeRenderingFilter(unsigned int idData, dataHandler *dataHandler);

	/** Distruttore */
	~volumeRenderingFilter();

	/**
	 * \fn vtkVolume* compute(long wl, long ww, int clut, int minPixel)
	 * \brief Esegue il volume rendering
	 * \param wl Definisce il window level
	 * \param ww Definisce il window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param minPixel Minimo valore dei pixel dell'immagine
	 * \return Il volume ricostruito
	 */
	vtkVolume* compute(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	/**
	 * \fn vtkVolume* computeRgb(long wl, long ww, int clut, int minPixel)
	 * \brief Esegue il volume rendering in formato RGB
	 * \param wl Definisce il window level
	 * \param ww Definisce il window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param minPixel Minimo valore dei pixel dell'immagine
	 * \return Il volume ricostruito
	 */
	vtkVolume* computeRgb(long wl, long ww, int clut, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	/**
	 * \fn vtkVolume* computeTextures3D(long wl, long ww, int clut, int minPixel, double sampleDistance)
	 * \brief Esegue il volume rendering con 3D texture mapping
	 * \param wl Definisce il window level
	 * \param ww Definisce il window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param minPixel Minimo valore dei pixel dell'immagine
	 * \param sampleDistance Passo di campionamento
	 * \return Il volume ricostruito
	 */
	vtkVolume* computeTextures3D(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	/**
	 * \fn vtkVolume* computeGPU(long wl, long ww, int clut, int minPixel, double sampleDistance)
	 * \brief Esegue il volume ray-casting GPU based
	 * \param wl Definisce il window level
	 * \param ww Definisce il window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param minPixel Minimo valore dei pixel dell'immagine
	 * \param sampleDistance passo di campionamento
	 * \return Il volume ricostruito
	 */
	vtkVolume* computeGPU(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower, int availableVRAM);

	/**
	 * \fn vtkVolume *computeFixedPoint(long wl, long ww, int clut, int minPixel)
	 * \brief Esegue il volume rendering con vtkFixedPointVolumeRayCastMapper
	 * \param wl Definisce il window level
	 * \param ww Definisce il window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param minPixel Minimo valore dei pixel dell'immagine
	 * \return Il volume ricostruito
	 */
	vtkVolume *computeFixedPoint(long wl, long ww, int clut, int minPixel, float sampleDistance, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	/**
	 * \fn vtkVolume* computeTextures2D(long wl, long ww, int clut, int minPixel)
	 * \brief Esegue il volume rendering con 2D texture mapping
	 * \param wl Definisce il window level
	 * \param ww Definisce il window width
	 * \param clut Indica che tipo di CLUT applicare al volume
	 * \param minPixel Minimo valore dei pixel dell'immagine
	 * \return Il volume ricostruito
	 */
	vtkVolume* computeTextures2D(long wl, long ww, int clut, int minPixel, bool shading, double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	/**
	 * \fn bool volumeRenderingFilter::is3DtextureSupported()
	 * \brief restituisce true se il 3d texture mapping � supportato
	 * \return true o false
	 */
	bool is3DtextureSupported(vtkRenderer *renderer);

	bool isGPUraycastingSupported(vtkRenderWindow *renderWindow);

	int computeAvailableGPUMemory();
};

#endif _volumeRenderingFilter_h