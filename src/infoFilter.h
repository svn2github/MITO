/**
 * \file infoFilter.h
 * \brief File per la gestione delle informazioni sulle immagini
 * \author ICAR-CNR Napoli
 */

#ifndef _infoFilter_h
#define _infoFilter_h

#include "itkVtkFilter.h"

/**
 * \class infoFilter
 * \brief Classe responsabile della gestione delle informazioni sulle immagini. Deriva da itkVtkFilter
 */
class infoFilter : public itkVtkFilter {

private:
	/** 
	 * \var _sliceNumber
	 * \brief Numero slice
	 */
	int _sliceNumber;

public:
	/** Costruttore con parametri identificativo del dato, gestore dei dati e numero slice */
	infoFilter(unsigned int idData, dataHandler *dataHandler, int sliceNumber);

	/** Distruttore */
	~infoFilter();

	/**
	 * \fn PixelType getPixelValue(double x, double y)
	 * \brief Restituisce l'intensità di un pixel
	 * \param x Coordinata x
	 * \param y Coordinata y
	 * \return Intensità del pixel
	 */
	PixelType getPixelValue(double x, double y);

	/**
	 * \fn RGBPixelType getPixelValueRgb(double xPos, double yPos)
	 * \brief Restituisce i valori dei canali R, G e B di un pixel
	 * \param x Coordinata x
	 * \param y Coordinata y
	 * \return Intensità del pixel
	 */
	RGBPixelType getPixelValueRgb(double xPos, double yPos);

	/**
	 * \fn ImageType::PointType getImageOrigin()
	 * \brief Restituisce l'origine dell'immagine
	 * \return Origine dell'immagine
	 */
	ImageType::PointType getImageOrigin();

	/**
	 * \fn ImageType::PointType getImageOrigin()
	 * \brief Restituisce lo spacing dell'immagine
	 * \return Spacing dell'immagine
	 */
	ImageType::SpacingType getSpacingType();

	/**
	 * \fn double getCurrentX(double xPos)
	 * \brief Normalizza la posizione dell'ascissa specificata
	 * \param xPos Posizione dell'ascissa
	 * \return Ascissa specificata
	 */
	double getCurrentX(double xPos);

	/**
	 * \fn double getCurrentX(double xPos)
	 * \brief Normalizza la posizione dell'ordinata specificata
	 * \param xPos Posizione dell'ordinata
	 * \return Ordinata specificata
	 */
	double getCurrentY(double yPos);

	/**
	 * \fn ImageType::IndexType getXY(double x, double y)
	 * \brief Restituisce le coordinate x e y specificate in formato tridimensionale
	 * \param x Coordinata x
	 * \param y Coordinata y
	 * \return Coordinate tridimensionali
	 */
	ImageType::IndexType getXY(double x, double y);
};

#endif _infoFilter_h