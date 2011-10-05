/**
 * \file itkVtkFilter.h
 * \brief File per la gestione dei dati in ITK e in VTK
 * \author ICAR-CNR Napoli
 */

#ifndef _itkVtkFilter_h_
#define _itkVtkFilter_h_

#include "itkVtkData.h"
#include "dataHandler.h"
#include <itkProcessObject.h> 

class itkVtkFilter {
	/** 
	 * \var unsigned int _idData
	 * \brief Identificativo del dato
	 */
	unsigned int _idData;

	/** 
	 * \var dataHandler* _dataHandler
	 * \brief Gestore dati
	 */
	dataHandler* _dataHandler;

	/** 
	 * \var itkVtkData* _itkVtkData
	 * \brief Dato
	 */
	itkVtkData* _itkVtkData;

	/** 
	 * \var bool _isValidData
	 * \brief E' true se il dato è valido, false altrimenti
	 */
	bool _isValidData;

public:
	/** Costruttore con parametri identificativo del dato e gestore dei visualizzatori */
	itkVtkFilter(unsigned int idData, dataHandler *dataHandler);

	/** Distruttore */
	virtual ~itkVtkFilter();

	/**
	 * \fn void update(itk::ImageSource<ImageType>::Pointer p)
	 * \brief Aggiorna il dato in scala di grigi sia in ITK che in VTK
	 * \param p Puntatore all'immagine sorgente
	 */
	void update(itk::ImageSource<ImageType>::Pointer p);

	/**
	 * \fn void update(itk::ImageSource<ImageType>::Pointer p)
	 * \brief Aggiorna il dato in scala di grigi sia in ITK che in VTK
	 * \param p Puntatore all'immagine ITK
	 */
	void update(ImageType::Pointer p);


	/**
	 * \fn void update(itk::ImageSource < RGBImageType >::Pointer p)
	 * \brief Aggiorna il dato RGB sia in ITK che in VTK
	 * \param p Puntatore all'immagine sorgente
	 */
	void update(itk::ImageSource < RGBImageType >::Pointer p);

	/**
	 * \fn void update(RGBImageType::Pointer p)
	 * \brief Aggiorna il dato RGB sia in ITK che in VTK
	 * \param p Puntatore all'immagine ITK
	 */
	void update(RGBImageType::Pointer p);

protected:
	/**
	 * \fn bool isValidData()
	 * \brief Indica se il dato è valido
	 * \return Vale true se il dato è valido, false altrimenti
	 */
	inline bool isValidData() {
		return _isValidData;
	}

	/**
	 * \fn itkVtkData *getItkVtkData()
	 * \brief Restituisce il dato
	 * \return Il dato _itkVtkData
	 */
	inline itkVtkData *getItkVtkData() {
		return _itkVtkData;
	}

	/**
	 * \fn void setItkVtkData(itkVtkData* data)
	 * \brief Assegna un dato
	 * \param data Il dato da assegnare
	 */
	inline void setItkVtkData(itkVtkData* data) {
		_itkVtkData = data;
	}
};

#endif _itkVtkFilter_h_