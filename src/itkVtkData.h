/**
 * \file itkVtkData.h
 * \brief File per la definizione dei dati in ITK e in VTK
 * \author ICAR-CNR Napoli
 */

#ifndef _itkVtkData_h_
#define _itkVtkData_h_


//by Nello
#include <wx/wx.h>
#include <wx/progdlg.h>


#include "appData.h"
#include "itkImageToVTKImageFilter.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkFlipImageFilter.h>
#include <itkImageLinearConstIteratorWithIndex.h> 
#include <itkImageDuplicator.h> 
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkDICOMSeriesFileNames.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkComposeRGBImageFilter.h>
#include <vtkImageData.h>
#include <vtkActor.h>
#include <vtkVolume.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/mstream.h>
#include <wx/image.h>
#include <iostream>


using namespace std;

const unsigned int Dimension = 3;

typedef itk::Image <unsigned char, 2 > ImageUChar;
typedef itk::ImageLinearConstIteratorWithIndex<ImageUChar> IteratorType;

typedef short PixelType;
typedef itk::Image<PixelType, Dimension> ImageType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageSeriesReader<ImageType> SeriesReaderType;
typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;
typedef itk::FlipImageFilter<ImageType> FlipImageType; 
typedef itk::Image <PixelType, 2 > ImageType2D;
typedef itk::ExtractImageFilter< ImageType, ImageType2D > ExtractImageType;
typedef itk::RescaleIntensityImageFilter < ImageType2D > RescaleType;
typedef itk::CastImageFilter < ImageType2D, ImageUChar > CastType;
typedef itk::ImageDuplicator<ImageType> Duplicator;

typedef itk::RGBPixel < unsigned char > RGBPixelType;
typedef itk::Image < RGBPixelType, Dimension > RGBImageType;
typedef itk::ImageFileReader < RGBImageType > RGBReaderType;
typedef itk::ImageSeriesReader < RGBImageType > RGBSeriesReaderType;
typedef itk::ImageToVTKImageFilter < RGBImageType > RGBConnectorType;
typedef itk::FlipImageFilter<RGBImageType> RGBFlipImageType; 
typedef itk::Image <RGBPixelType, 2 > RGBImageType2D;
typedef itk::ExtractImageFilter< RGBImageType, RGBImageType2D > RGBExtractImageType;
typedef itk::RescaleIntensityImageFilter < RGBImageType2D > RGBRescaleType;
typedef itk::CastImageFilter < RGBImageType2D, ImageUChar > RGBCastType;
typedef itk::ImageDuplicator<RGBImageType> RGBDuplicator;

typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames NamesGeneratorType;
typedef itk::DICOMSeriesFileNames DICOMSeriesFileNames;
typedef itk::MetaDataDictionary DictionaryType;
typedef itk::MetaDataObject<string> MetaDataStringType;


/** \class itkVtkData
 * \brief Classe che implementa l'appData con ITK e VTK
 */
class itkVtkData : public appData {

protected:
	/** 
	 * \var ImageType::Pointer _itkImage
	 * \brief Immagine in scala di grigi in formato ITK
	 */
	ImageType::Pointer _itkImage;

	/** 
	 * \var RGBImageType::Pointer _itkRgbImage
	 * \brief Immagine RGB in formato ITK
	 */
	RGBImageType::Pointer _itkRgbImage;

	/** 
	 * \var vtkImageData *_vtkImage
	 * \brief Immagine in formato VTK
	 */
	vtkImageData *_vtkImage;

	/**
	 * \var ConnectorType::Pointer _itkVtkConnector
	 * \brief Connettore ITK-VTK in formato scala di grigi
	 */
	ConnectorType::Pointer _itkVtkConnector;

	/**
	 * \var RGBConnectorType::Pointer _itkVtkRgbConnector
	 * \brief Connettore ITK-VTK in formato RGB
	 */
	RGBConnectorType::Pointer _itkVtkRgbConnector;

	/**
	 * \var vtkActor* _vtkActor
	 * \brief Volume utilizzato per il Surface Rendering
	 */
	vtkActor* _vtkActor;

	/** 
	 * \var vtkVolume* _vtkVolume
	 * \brief Volume utilizzato per il Volume Rendering
	 */
	vtkVolume* _vtkVolume;

	/** 
	 * \var vector<string> _strVecDicomEntries
	 * \brief Lista entry dell'header DICOM
	 */
	vector<string> _strVecDicomEntries;

	/** 
	 * \var vector<string> _strVecDicomKey
	 * \brief Lista chiavi dell'header DICOM
	 */
	vector<string> _strVecDicomKey;

	/** 
	 * \var vector<string> _strVecDicomValue
	 * \brief Lista tag dell'header DICOM
	 */
	vector<string> _strVecDicomValue;


	/** 
	 * \var string _imageViewPlane
	 * \brief Memorizza il piano a partire dal quale è stata effettuata l'acquisizione
	 */
	string _imageViewPlane;

	/** 
	 * \var bool _rgb
	 * \brief Indica se un'immagine è in RGB o in scala di grigi
	 */
	bool _rgb;

	/** 
	 * \var double _offset
	 * \brief Distanza tra due slice in una serie
	 */
	double _offset;

	/** 
	 * \var bool _downUp
	 * \brief Indica se un'immagine è stata scansita dal basso verso l'alto (true) o viceversa (false)
	 */
	bool _downUp;

	//--- by Nello ---
	/** 
	 * \var wxProgressDialog * wait
	 * \brief eventuale finestra di attesa da aggiornare durante le elaborazioni
	 */
	wxProgressDialog * wait;
	//----------------

public:


	//--- by Nello ---	
	void setBusyWindow(wxProgressDialog* wait);
	void updateBusyWindow(int newValue);
	void clearBusyWindow();
	//----------------


	/** Costruttore */
	itkVtkData();

	/** Distruttore */
	~itkVtkData();

	/**
	 * \fn int itkVtkData::readDicomHeader(ImageIOType::Pointer dicomIO)
	 * \brief Legge l'header DICOM
	 * \param dicomIO Parametro da cui leggere il dizionario
	 * \return Dimensione dell'header
	 */
	int itkVtkData::readDicomHeader(ImageIOType::Pointer dicomIO);

	/**
	 * \fn int readDicomEntries(ImageIOType::Pointer dicomIO)
	 * \brief Legge le entry dall'header DICOM
	 * \param dicomIO Parametro da cui leggere il dizionario
	 * \return Dimensione delle entry
	 */
	int readDicomEntries(ImageIOType::Pointer dicomIO);
	
	/**
	 * \fn bool open(string fileName, bool read)
	 * \brief Apre un file DICOM
	 * \param fileName percorso del file da aprire
	 * \param read indica se leggere l'immagine o sono l'header
	 * \return true se l'operazione è andata a buon fine
	 */
	bool open(string fileName, bool read);
	bool open(wxArrayString *pPaths, bool read);

	/**
	 * \fn ImageType::Pointer computeFlipping(ImageType::Pointer itkImage)
	 * \brief Applica un flipping alla serie/immagine caricata in scala di grigi
	 * \param itkImage Immagine da flippare in scala di grigi
	 * \return Puntatore all'immagine flippata in scala di grigi
	 */
	ImageType::Pointer computeFlipping(ImageType::Pointer itkImage);

	/**
	 * \fn ImageType::Pointer computeRgbFlipping(ImageType::Pointer itkImage)
	 * \brief Applica un flipping alla serie/immagine caricata in RGB
	 * \param itkRgbImage Immagine da flippare in RGB
	 * \return Puntatore all'immagine flippata in RGB
	 */
	RGBImageType::Pointer computeRgbFlipping(RGBImageType::Pointer itkRgbImage);

	/**
	 * \fn ImageType::Pointer getItkImage()
	 * \brief Restituisce il puntatore all'immagine ITK
	 * \return Puntatore all'immagine ITK
	 */
	inline ImageType::Pointer getItkImage() {
		return _itkImage;
	}

	/**
	 * \fn RGBImageType::Pointer getItkRgbImage()
	 * \brief Restituisce il puntatore all'immagine ITK in formato RGB
	 * \return Puntatore all'immagine RGB
	 */
	inline RGBImageType::Pointer getItkRgbImage() {
		return _itkRgbImage;
	}

	/**
	 * \fn ImageType::Pointer getItkImage()
	 * \brief Restituisce il puntatore all'immagine VTK
	 * \return Puntatore all'immagine VTK
	 */
	inline vtkImageData *getVtkImage() {
		return _vtkImage;
	}


	/**
	 * \fn ConnectorType::Pointer getItkVtkConnector()
	 * \brief Restituisce il puntatore al connettore ITK->VTK in formnato scala di grigi
	 * \return Puntatore al connettore ITK->VTK in formato scala di grigi
	 */
	inline ConnectorType::Pointer getItkVtkConnector() {
		if (_itkVtkConnector.IsNull())
			_itkVtkConnector = ConnectorType::New();
		return _itkVtkConnector;
	}

	/**
	 * \fn RGBConnectorType::Pointer getItkVtkRgbConnector()
	 * \brief Restituisce il puntatore al connettore ITK->VTK in formato RGB
	 * \return Puntatore al connettore ITK->VTK in formato RGB
	 */
	inline RGBConnectorType::Pointer getItkVtkRgbConnector() {
		if (_itkVtkRgbConnector.IsNull())
			_itkVtkRgbConnector = RGBConnectorType::New();
		return _itkVtkRgbConnector;
	}

	/**
	 * \fn void setItkImage(ImageType::Pointer itkImage)
	 * \brief Assegna un'immagine ITK
	 * \param itkImage Immagine ITK da assegnare
	 */
	inline void setItkImage(ImageType::Pointer itkImage) {
		_itkImage = itkImage;
	}

	/**
	 * \fn void setItkRgbImage(RGBImageType::Pointer itkRGBImage)
	 * \brief Assegna un'immagine ITK in formato RGB
	 * \param itkRgbImage Puntatore all'immagine ITK in formato RGB
	 */
	inline void setItkRgbImage(RGBImageType::Pointer itkRgbImage) {
		_itkRgbImage = itkRgbImage;
	}

	/**
	 * \fn void setVtkImage(vtkImageData *vtkImageData)
	 * \brief Assegna un'immagine VTK
	 * \param vtkImageData Immagine VTK da assegnare
	 */
	inline void setVtkImage(vtkImageData *vtkImageData) {
		_vtkImage = vtkImageData;
	}

	/**
	 * \fn void convertRgbToGrayVtkImage(bool deleteRgbImage)
	 * \brief Connette all'immagine VTK un'immagine in formato scala di grigi\n
	 * a partire da un'immagine RGB o dalla precedente immagine in scala di grigi
	 * \param deleteRgbImage Indica se eliminare l'immagine di partenza
	 */
	void convertRgbToGrayVtkImage(bool deleteRgbImage);

	/**
	 * \fn void convertGrayToRgbVtkImage(bool deleteImage)
	 * \brief Connette all'immagine VTK un'immagine in formato RGB\n
	 * a partire da un'immagine in scala di grigi o dalla precedente immagine RGB
	 * \param deleteImage Indica se eliminare l'immagine di partenza
	 */
	void convertGrayToRgbVtkImage(bool deleteImage);

	/**
	 * \fn void deleteImage()
	 * \brief Elimina l'immagine ITK in formato scala di grigi
	 */
	inline void deleteImage() {
		_itkImage = 0;
		_itkVtkConnector = 0;
	}

	/**
	 * \fn void deleteRgbImage()
	 * \brief Elimina l'immagine ITK in formato RGB
	 */
	inline void deleteRgbImage() {
		_itkRgbImage = 0;
		_itkVtkRgbConnector = 0;
	}

	/**
	 * \fn vtkActor *getVtkActor()
	 * \brief Restituisce il puntatore al vtkActor
	 * \return Puntatore al vtkActor
	 */
	inline vtkActor *getVtkActor() {
		return _vtkActor;
	}


	/**
	 * \fn void setVtkActor(vtkActor *vtkActor)
	 * \brief Assegna un vtkActor
	 * \param vtkActor Valore da assegnare a vtkActor
	 */
	inline void setVtkActor(vtkActor *vtkActor) {
		if (_vtkActor)
			_vtkActor->Delete();
		_vtkActor = vtkActor;
	}


	/**
	 * \fn vtkVolume *getVtkVolume()
	 * \brief Restituisce il puntatore a _vtkVolume
	 * \return Puntatore a _vtkVolume
	 */
	inline vtkVolume *getVtkVolume() {
		return _vtkVolume;
	}


	/**
	 * \fn void setVtkVolume(vtkVolume *vtkVolume)
	 * \brief Assegna un vtkVolume
	 * \param vtkVolume Valore da assegnare
	 */
	inline void setVtkVolume(vtkVolume *vtkVolume) {
		if (_vtkVolume)
			_vtkVolume->Delete();
		_vtkVolume = vtkVolume;
	}

	/**
	 * \fn double getOffset()
	 * \brief Restituisce il valore dell'offset
	 * \return Valore dell'offset
	 */
	inline double getOffset() {
		return _offset;
	}

	/**
	 * \fn double getOffset()
	 * \brief Restituisce il valore della variabile _downUp
	 * \return Valore dell'attributo _downUp
	 */
	inline bool getDownUp() {
		return _downUp;
	}

	/**
	 * \fn void setOffset(double offset)
	 * \brief Assegna il valore dell'offset
	 * \param offset Valore dell'offset da assegnare
	 */
	inline void setOffset(double offset) {
		_offset = offset;
	}

	/**
	 * \fn void setOffset(bool downUp)
	 * \brief Assegna il valore della variabile _downUp
	 * \param downUp Valore da assegnare
	 */
	inline void setDownUp(bool downUp) {
		_downUp = downUp;
	}

	/**
	 * \fn int getHeaderLength()
	 * \brief Restituisce la lunghezza dell'header
	 * \return Dimensione dell'header
	 */
	inline int getHeaderLength() {
		return _strVecDicomKey.size();
	}

	/**
	 * \fn string getHeaderKey(int index)
	 * \brief Restituisce una chiave dell'header
	 * \param index Indice della chiave
	 * \return Chiave dell'header
	 */
	inline string getHeaderKey(int index) {
		return _strVecDicomKey[index];
	}

	/**
	 * \fn string getHeaderValue(int index)
	 * \brief Restituisce un tag dell'header
	 * \param index Indice della chiave
	 * \return Tag dell'header
	 */
	inline string getHeaderValue(int index) {
		return _strVecDicomValue[index];
	}

	/**
	 * \fn string getHeaderEntry(int index)
	 * \brief Restituisce un entry dell'header
	 * \param index Indice della chiave
	 * \return Entry dell'header
	 */	
	inline string getHeaderEntry(int index) {
		return _strVecDicomEntries[index];
	}

	/**
	 * \fn string getHeaderEntriesLength()
	 * \brief Restituisce la dimensione degli entry dell'header
	 * \return Dimensione degli entry dell'header
	 */
	inline int getHeaderEntriesLength() {
		return _strVecDicomEntries.size();
	}

	/**
	 * \fn void resetVtkData()
	 * \brief Resetta l'immagine nel filtro VTK
	 */
	void resetVtkData();

	/**
	 * \fn void copyHeaderInfo(vector<string> strVecDicomEntries, vector<string> strVecDicomKey,vector<string> strVecDicomValue)
	 * \brief Assegna le informazioni dell'header
	 * \param strVecDicomEntries Entry dell'header
	 * \param strVecDicomKey Chiave dell'header
	 * \param strVecDicomValue Tag dell'header
	 */
	inline void copyHeaderInfo(vector<string> strVecDicomEntries, vector<string> strVecDicomKey, vector<string> strVecDicomValue) {
		_strVecDicomEntries = strVecDicomEntries;
		_strVecDicomKey = strVecDicomKey;
		_strVecDicomValue = strVecDicomValue;
	}

	/**
	 * \fn vector<string> getDicomEntries()
	 * \brief Restituisce il valore della variabile _strVecDicomEntries
	 * \return Valore della variabile _strVecDicomEntries
	 */
	inline vector<string> getDicomEntries() {
		return _strVecDicomEntries;
	}

	/**
	 * \fn vector<string> getDicomKey()
	 * \brief Restituisce il valore della variabile _strVecDicomKey
	 * \return Valore della variabile _strVecDicomKey
	 */
	inline vector<string> getDicomKey() {
		return _strVecDicomKey;
	}

	/**
	 * \fn vector<string> getDicomValue()
	 * \brief Restituisce il valore della variabile _strVecDicomValue
	 * \return Valore della variabile _strVecDicomValue
	 */
	inline vector<string> getDicomValue() {
		return _strVecDicomValue;
	}
	
	/**
	 * \fn int getSliceNumber()
	 * \brief Restituisce il numero delle immagini
	 * \return Numero delle immagini
	 */
	inline int getSliceNumber() {
		return _slicesNumber;
	}
	
	/**
	 * \fn int setSliceNumber(int sliceNr)
	 * \brief Assegna il numero delle immagini
	 * \param sliceNr Numero delle immagini
	 */
	inline void setSliceNumber(int sliceNr) {
		_slicesNumber = sliceNr;
	}

	/**
	 * \fn wxImage* getThumbnails()
	 * \brief Restituisce il puntatore a _thumbnails
	 * \return Puntatore a _thumbnails
	 */
	inline wxImage* getThumbnails() {
		return _thumbnails;
	}

	/**
	 * \fn bool getRgb()
	 * \brief Restituisce il valore della variabile _rgb
	 * \return Valore della variabile _rgb
	 */
	inline bool getRgb() {
		return _rgb;
	}

	/**
	 * \fn double getWl()
	 * \brief Restituisce il valore del window level
	 * \return Valore del window level
	 */
	inline double getWl() {
		return _wl;
	}

	/**
	 * \fn double getWw()
	 * \brief Restituisce il valore del window width
	 * \return Valore del window width
	 */
	inline double getWw() {
		return _ww;
	}

	/**
	 * \fn void setWw()
	 * \brief Assegna il valore del window width
	 * \param ww Valore del window width
	 */
	inline void setWw(double ww) {
		_ww = ww;
	}

	/**
	 * \fn void setWl()
	 * \brief Assegna il valore del window level
	 * \param wl Valore del window level
	 */
	inline void setWl(double wl) {
		_wl = wl;
	}

	/**
	 * \fn void setRgb(bool rgb)
	 * \brief Indica se l'immagine è RGB o meno
	 * \param rgb Vale true se l'immagine è RGB, false altrimenti
	 */
	inline void setRgb(bool rgb) {
		_rgb = rgb;
	}

	/**
	 * \fn int getMinPixelValue()
	 * \brief Utilizzato per determinare il pixel a valore minimo nell'immagine
	 * \return Valore minimo
	 */
	int getMinPixelValue();

	/**
	 * \fn int getMaxPixelValue()
	 * \brief Utilizzato per determinare il pixel a valore massimo nell'immagine
	 * \return Valore massimo
	 */
	int getMaxPixelValue();

	/**
	 * \fn string getImageViewPlane()
	 * \brief Restituisce il piano di vista a partire dal quale è stata effettuata l'acquisizione
	 * \return Valore massimo
	 */
	inline string getImageViewPlane() {
		return _imageViewPlane;
	}

	/**
	 * \fn void setImageViewPlane(string imageViewPlane)
	 * \brief Assegna il piano di vista a partire dal quale è stata effettuata l'acquisizione
	 * \param imageViewPlane Piano di vista
	 */
	inline void setImageViewPlane(string imageViewPlane) {
		_imageViewPlane = imageViewPlane;
	}

	/**
	 * \fn string setDicomEntry(ImageIOType::Pointer dicomIO, string entryId, string entryvalue)
	 * \brief Setta un entry nell'header DICOM
	 * \param dicomIO Parametro su cui scrivere il dizionario
	 * \param entryId Identificativo dell'entry da scrivere
	 * \param entryvalue Identificativo dell'entry da scrivere
	 */
	void setDicomEntry(ImageIOType::Pointer dicomIO, string entryId, string entryvalue);

private:
	/**
	 * \fn bool openSeries(string fileName, bool read)
	 * \brief Apre una serie DICOM
	 * \param fileName percorso della directory da aprire
	 * \param read indica se leggere l'immagine o sono l'header
	 * \return true se l'operazione è andata a buon fine
	 */
	bool openSeries(string fileName, bool read);

	/**
	 * \fn void generateThumbnails()
	 * \brief Genera i thumbnails a partire dalla serie/immagine caricata in formato scala di grigi
	 */
	void generateThumbnails();

	/**
	 * \fn void generateRgbThumbnails()
	 * \brief Genera i thumbnails a partire dalla serie/immagine caricata in formato RGB
	 */
	void generateRgbThumbnails();

	/**
	 * \fn string readDicomEntry(ImageIOType::Pointer dicomIO, string entryId)
	 * \brief Legge un entry dall'header DICOM
	 * \param dicomIO Parametro da cui leggere il dizionario
	 * \param entryId Identificativo dell'entry da leggere
	 * \return Entry letto
	 */
	string readDicomEntry(ImageIOType::Pointer dicomIO, string entryId);

	/**
	 * \fn string calculateImageViewPlane(ImageIOType::Pointer dicomIO)
	 * \brief Calcola il piano secondo il quale è stata effettuata l'acquisizione
	 * \param dicomIO Parametro contenente il dizionario
	 * \return Stringa contenente il piano
	 */
	string calculateImageViewPlane(ImageIOType::Pointer dicomIO);

	/**
	 * \fn string getMajorAxisFromPatientRelativeDirectionCosine(double x,double y,double z)
	 * \brief Restituisce la direzione secondo la quale è stata effettuata l'acquisizione
	 * \param double x Posizione X
	 * \param double y Posizione Y
	 * \param double z Posizione Z
	 * \return Direzione secondo la quale è stata effettuata l'acquisizione
	 */
	string getMajorAxisFromPatientRelativeDirectionCosine(double x, double y, double z);

	/** 
	 * \var int _slicesNumber
	 * \brief Numero di immagini
	 */
	int _slicesNumber;

	/** 
	 * \var double _wl
	 * \brief Window Level
	 */
	double _wl;

	/** 
	 * \var double _ww
	 * \brief Window Width
	 */
	double _ww;

	/** 
	 * \var wxImage* _thumbnails
	 * \brief Gestore dei thumbnails
	 */
	wxImage* _thumbnails;
};

#endif _itkVtkData_h_