/**
 * \file exportFilter.h
 * \brief File per il salvataggio delle immagini
 * \author ICAR-CNR Napoli
 */

#ifndef _exportFilter_h_
#define _exportFilter_h_

#include "itkVtkFilter.h"

/**
 * \class exportFilter
 * \brief Filtro per il salvataggio delle immagini
 */
class exportFilter {

private:
	/** 
	 * \var bool _rgb
	 * \brief Indica se l'immagine DICOM è in RGB o in scala di grigi
	 */
	bool _rgb;

public:
	/** Costruttore con parametro rgb */
	exportFilter(bool rgb);

	/**
	 * \fn void exportImageToDicom(string inputFilename, string outputFilename)
	 * \brief Salva un'immagine in formato DICOM
	 * \param inputFilename Nome del file origine
	 * \param outputFilename Nome del file destinazione
	 */
	//void exportImageToDicom(string inputFilename, string outputFilename);
	void exportImageToDicom(itkVtkData *pInputData, string outputFilename);

	/**
	 * \fn void exportImageToBmp(string inputFilename, string outputFilename)
	 * \brief Salva un'immagine in formato BMP
	 * \param inputFilename Nome del file origine
	 * \param outputFilename Nome del file destinazione
	 */
	void exportImageToBmp(string inputFilename, string outputFilename);
	//void exportImageToBmp(itkVtkData *pInputData, string outputFilename);

	/**
	 * \fn void exportSeriesToDicom(string inputDirectory, string outputDirectory)
	 * \brief Salva una serie in formato DICOM
	 * \param inputDirectory Nome della directory origine
	 * \param outputDirectory Nome della directory destinazione
	 */
	void exportSeriesToDicom(string inputDirectory, string outputDirectory);
	//void exportSeriesToDicom(string inputDirectory, string outputDirectory, itkVtkData *pInputData);

	/**
	 * \fn void exportSeriesToBmp(string inputDirectory, string outputDirectory)
	 * \brief Salva una serie in formato BMP
	 * \param inputDirectory Nome della directory origine
	 * \param outputDirectory Nome della directory destinazione
	 */
	void exportSeriesToBmp(string inputDirectory, string outputDirectory);

	/**
	 * \fn bool getRgb()
	 * \brief Restituisce il valore della variabile _rgb
	 * \return Valore della variabile _rgb
	 */
	inline bool getRgb() {
		return _rgb;
	}

	/**
	 * \fn bool getRgb()
	 * \brief Assegna un valore alla variabile _rgb
	 * \param rgb Valore da assegnare alla variabile _rgb
	 */
	inline void setRgb(bool rgb) {
		_rgb = rgb;
	}
};

#endif _exportFilter_h_