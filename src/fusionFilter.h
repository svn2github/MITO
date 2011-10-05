/**
 * \file fusionFilter.h
 * \brief File per la gestione delle tecniche di fusione
 * \author ICAR-CNR Napoli
 */

#ifndef _fusionFilter_h_
#define _fusionFilter_h_

#include "itkVtkFilter.h"
#include "itkFlip.h"
#include <itkImage.h>
#include <itkNumericTraits.h>


/** 
 * \class fusionFilter
 * \brief Filtro che implementa le varie tecniche di fusione.
 */
class fusionFilter {

public:

	/** 
	 * \brief Enumerazione delle tecniche di fusione implementate
	 */
	enum fusionType{
			not_defined,			/*!< Tecnica di fusione non definita */
			media_pesata,			/*!< Tecnica di fusione con combinazione aritmetica delle immagini */
			wavelet					/*!< Tecnica di fusione con trasformata wavelet Daubechies */
		};

	enum ruleType{
		Modulo_max,				
		Consistenza_media,
		Consistenza_var,
		Sostituzione_dettaglio
	};

	typedef int LivelloDec;		//definizioni di tipo necessarie per il passaggio di parametri dalla gui (luca)
	typedef int SizeWindow;
	typedef int WaveletType;

	/** 
	 * \brief Tipo di dato reale utilizzato per la gestione dei pesi delle immagini
	 */
	typedef itk::NumericTraits < short >::RealType InputRealType;

	/** 
	 * \brief Costruttore del filtro
	 *
	 * \param path1 Percorso della prima immagine o serie
	 * \param path2 Percorso della seconda immagine o serie
	 * \param idData0 Id della immagine o serie prodotta
	 * \param dataHandler Gestore dei dati
	 */
	fusionFilter (string path1, string path2, unsigned int idData0, dataHandler *dataHandler);

	/** 
	 * \brief Distruttore del filtro
	 */
	~fusionFilter() {};

	/** 
	 * \brief Metodo che effettua la fusione tra le immagini o le serie scelte con gli opportuni parametri fissati
	 *	
	 * \param param1		Peso per la prima immagine, di default 0.5
	 * \param param2		Peso per la seconda immagine, di default 0.5
	 * \param param3		Parametro vuoto, lasciato per sviluppi futuri, di defualt 0
	 * \param fusionType	Tecnica di fusione da implementare, di default <b>media_pesata</b>
	 * \param applyClut		Valore booleano che indica l'applicazione della Clut, di default FALSE
	 * \param leveldec		Livello di decomposizione
	 * \param ruleType		Regola di fusione
	 *\ param sizewin		Grandezza della finestra
	 *\ param DaubCoef		Livello della wavelet Daubechies
	 */
	void computeFusion (InputRealType param1 = 0.5, InputRealType param2 = 0.5, InputRealType param3 = 0, fusionType fusionType = media_pesata, bool applyClut = false, LivelloDec leveldec=1,ruleType ruleType=Modulo_max, SizeWindow sizewin=1, WaveletType waveletType=2);

private:
	/** 
	 * \brief Indica se applicare la CLUT
	 */
	bool _applyClut;

	/** 
	 * \brief Dato di uscita
	 */
	itkVtkData* _itkVtkData0;

	/** 
	 * \brief Prima immagine
	 */
	ImageType::Pointer _itkImage1;

	/** 
	 * \brief Seconda immagine
	 */
	ImageType::Pointer _itkImage2;

	/** 
	 * \brief Header DICOM
	 */
	ImageIOType::Pointer _itkDicomIO;


	/** 
	 * \brief Tipo di fusione
	 */
	fusionType _fusionType;

	/** 
	 * \brief Livello di decomposizione
	 */
	LivelloDec _levelDec;

	/** 
	 * \brief Grandezza della finestra
	 */
	SizeWindow _sizeWindow;

	/** 
	 * \brief Livello della wavelet Daubechies
	 */
	WaveletType _waveletType;

	/** 
	 * \brief Regola di fusione
	 */
	int _fusionRule;

	/** 
	 * \brief Window Level della seconda immagine
	 */
	double _wl2;

	/** 
	 * \brief Window Width della seconda immagine
	 */
	double _ww2;


	/**
	 * \brief Metodo che aggiorna la pipeline verso VTK
	 *
	 * \param image Immagine da visualizzare con VTK
	 */
	inline void update (ImageType::Pointer image) {
		if (_itkImage1) {
			ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
			_itkVtkData0->setRgb(false);
			_itkVtkData0->setItkImage(_itkVtkData0->computeFlipping(image));
			_itkVtkData0->setDicomEntry(_itkDicomIO,"0010|0020","");
			_itkVtkData0->setDicomEntry(_itkDicomIO,"0020|0010","");
			_itkVtkData0->setDicomEntry(_itkDicomIO,"0008|1030","");
			_itkVtkData0->readDicomEntries(_itkDicomIO);
			_itkVtkData0->setSliceNumber(size[2]);
			//if(_fusionType == media_pesata) _itkVtkData0->setWl(_wl2*0.8);
			//else _itkVtkData0->setWl(_wl2*1.2);
			//_itkVtkData0->setWw(_ww2);

			_itkVtkData0->setWl(140.0);
			_itkVtkData0->setWw(210.0);

			_itkVtkData0->getItkVtkConnector()->SetInput(_itkVtkData0->getItkImage());
			_itkVtkData0->setVtkImage(_itkVtkData0->getItkVtkConnector()->GetOutput());
		}
	};

	/**
	 * \brief Metodo che aggiorna la pipeline verso VTK
	 *
	 * \param rgbImage Immagine da visualizzare con VTK in formatro RGB
	 */
	inline void update (RGBImageType::Pointer rgbImage) {
		if (_itkImage1) {
			RGBImageType::SizeType size = rgbImage->GetLargestPossibleRegion().GetSize();
			_itkVtkData0->setRgb(true);
			_itkVtkData0->setItkRgbImage(_itkVtkData0->computeRgbFlipping(rgbImage));
			_itkVtkData0->setDicomEntry(_itkDicomIO,"0010|0020","");
			_itkVtkData0->setDicomEntry(_itkDicomIO,"0020|0010","");
			_itkVtkData0->setDicomEntry(_itkDicomIO,"0008|1030","");
			_itkVtkData0->readDicomEntries(_itkDicomIO);
			_itkVtkData0->setWl(140.0);
			_itkVtkData0->setWw(210.0);
			_itkVtkData0->setSliceNumber(size[2]);
			_itkVtkData0->getItkVtkRgbConnector()->SetInput(_itkVtkData0->getItkRgbImage());
			_itkVtkData0->setVtkImage(_itkVtkData0->getItkVtkRgbConnector()->GetOutput());
		}
	};

	/**
	 * \brief Metodo che effettua la fusione con la tecnica di combinazione aritmetica delle immagini
	 *
	 * \param pesoA Peso per la prima immagine
	 * \param pesoB Peso per la seconda immagine
	 */
	void MediaPesata (InputRealType pesoA, InputRealType pesoB);

	/**
	 * \brief Metodo che effettua la fusione con la trasformata wavelet
	 *
	 * \param pesoA Peso per la prima immagine
	 * \param pesoB Peso per la seconda immagine
	 */
	void HaarWavelet(InputRealType pesoA, InputRealType pesoB);

	/**
	 * \brief Metodo che effettua la fusione con la tecnica di combinazione aritmetica delle immagini applicando la Clut
	 *
	 * \param pesoA Peso per la prima immagine
	 * \param pesoB Peso per la seconda immagine
	 */
	void MediaPesataClut(InputRealType pesoA, InputRealType pesoB);

	/**
	 * \brief Metodo che effettua la fusione con la trasfromata wavelet applicando la Clut
	 *
	 * \param pesoA Peso per la prima immagine
	 * \param pesoB Peso per la seconda immagine
	 */
	void HaarWaveletClut(InputRealType pesoA, InputRealType pesoB);


	/**
	 * \brief Metodo che effettua la fusione con la trasformata wavelet secondo DN 
	 *
	 * \param pesoA Peso per la prima immagine
	 * \param pesoB Peso per la seconda immagine
	 */
	void DaubNWavelet(InputRealType pesoA, InputRealType pesoB);

	/**
	 * \brief Metodo che effettua la fusione con la trasformata wavelet secondo DN applicando la clut
	 *
	 * \param pesoA Peso per la prima immagine
	 * \param pesoB Peso per la seconda immagine
	 */
	void DaubNWaveletClut(InputRealType pesoA, InputRealType pesoB);


	/**
	 * \brief Metodo che effettua un riposizionamento delle immagini per la fusione 2D
	 *
	 * \param im1 Puntatore alla prima immagine
	 * \param im2 Puntatore alla seconda immagine
	 */
	void PreStadio2D(ImageType::Pointer im1, ImageType::Pointer im2);
};

#endif _fusionFilter_h_