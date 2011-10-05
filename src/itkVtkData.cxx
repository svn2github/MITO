/**
 * \file itkVtkData.cxx
 * \brief File per la definizione dei dati in ITK e in VTK
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "itkVtkData.h"

//solo per il minimo
#include <algorithm>

/*
* Il costruttore inizializza a null tutti i puntatori
*/
itkVtkData::itkVtkData() {
	_itkImage = NULL;
	_itkRgbImage = NULL;
	_vtkImage = NULL;
	_itkVtkConnector = NULL;
	_itkVtkRgbConnector = NULL;
	_vtkActor = NULL;
	_vtkVolume = NULL;
	_thumbnails = NULL;
	_slicesNumber = 0;
	_rgb = false;
	_offset = 0.0;
	_downUp = false;
	_wl = 128.0;
	_ww = 256.0;

	// by Nello per la gestione del progressDialog
	wait = NULL;
}

/*
* Il distruttore elimina il vtkActor se diverso da null e il vtkVolume
*/
itkVtkData::~itkVtkData() {
	if(_vtkActor)
		_vtkActor->Delete();
	if(_vtkVolume)
		_vtkVolume->Delete();
	if(_thumbnails)
		delete [] _thumbnails;
}


// by Nello per la gestione del progressDialog
void itkVtkData::setBusyWindow(wxProgressDialog* wait)
{
	this->wait = wait;
}

void itkVtkData::updateBusyWindow(int newValue)
{
	if ( wait != NULL)
	{
		char perc[5];
		itoa(newValue,perc,10);
		char msg[] = "please wait, creating thumbnails...   %";
		int msgPercPos = 38;
		if (newValue<10)
		{
			msg[msgPercPos-1]=perc[0];						
		}
		else
		{
			msg[msgPercPos-2]=perc[0];	msg[msgPercPos-1]=perc[1];
		}
		
		if (newValue < 0) wait->Pulse();		
		else wait->Update( newValue, msg );		
	}
}
void itkVtkData::clearBusyWindow()
{
	this->wait = NULL;
}
//---------------------------------------


/*
* Apre un immagine dicom\n
* Il metodo richiama il metodo openSeries se il fileName è una directory
*/
bool itkVtkData::open(string fileName, bool read) {
	wxFileName wxFilename((wxString)fileName.c_str(), wxPATH_NATIVE);

	//if is dicom
	//else return false
	//if (wxFilename.DirExists())
	if (!wxFilename.FileExists() && wxFilename.DirExists())
		return openSeries(fileName, read);
	else {
		ReaderType::Pointer itkReader = ReaderType::New();
		ImageIOType::Pointer itkDicomIO = ImageIOType::New();
		
		itkReader->SetFileName(fileName.c_str());
		itkReader->SetImageIO(itkDicomIO);

		if((fileName.find(".dcm", fileName.size()-4) == -1) && (itkDicomIO->CanReadFile(fileName.c_str()) == false))
			return false;

		else {

			try {
				//itkReader->UpdateLargestPossibleRegion();
				itkReader->UpdateOutputInformation();
			}
			catch (itk::ExceptionObject) {
				return false;
			}

			_itkImage = ImageType::New();
			_itkImage = itkReader->GetOutput();
			


			// Lettura Header DICOM: immagine RGB?
			const DictionaryType & dictionary = _itkImage->GetMetaDataDictionary();
			std::string entryId = "0028|0002";
			std::string tagvalue;
			DictionaryType::ConstIterator tagItr = dictionary.Find( entryId );
			MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
			if( entryvalue )
				tagvalue = entryvalue->GetMetaDataObjectValue();
			_rgb = false;
			if(!tagvalue.compare("3")) _rgb = true;

			if(_rgb) {
				_itkImage = 0;

				RGBReaderType::Pointer itkRgbReader = RGBReaderType::New();
				itkRgbReader->SetFileName(fileName.c_str());
				itkRgbReader->SetImageIO(itkDicomIO);

				if(!read) {
					try {
						//itkRgbReader->UpdateLargestPossibleRegion();
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}
					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkRgbReader->UpdateLargestPossibleRegion();
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}

					_itkRgbImage = itkRgbReader->GetOutput();
					_itkRgbImage = computeRgbFlipping(_itkRgbImage);
					_itkVtkRgbConnector = RGBConnectorType::New();
					_itkVtkRgbConnector->SetInput(_itkRgbImage);
					_vtkImage = _itkVtkRgbConnector->GetOutput();

					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateRgbThumbnails();
				}

				//convertRgbToGrayVtkImage(true);
			}
			else {
				if(!read) {
					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkReader->UpdateLargestPossibleRegion();
						itkReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}

					//_itkImage = computeFlipping(_itkImage);
					_itkImage = computeFlipping(itkReader->GetOutput());
					_itkVtkConnector = ConnectorType::New();
					_itkVtkConnector->SetInput(_itkImage);
					_vtkImage = _itkVtkConnector->GetOutput();
					
					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateThumbnails();

				}
			}
			readDicomHeader(itkDicomIO);
			readDicomEntries(itkDicomIO);

			return true;
		}
	}
}

bool itkVtkData::open(wxArrayString *pPaths, bool read)
{
	if(!pPaths)
		return false;

	if(pPaths->GetCount() > 1)
		{
		DICOMSeriesFileNames::Pointer dicomSeriesFileNames = DICOMSeriesFileNames::New();

		SeriesReaderType::Pointer itkReader = SeriesReaderType::New();
		ImageIOType::Pointer itkDicomIO = ImageIOType::New();

		vector<string> fileNames;
		size_t fileCount = pPaths->GetCount();
		char pTmp[255];
		for(size_t k = 0; k < fileCount; k++)
		{
	//		string tmpString = fileName.c_str();
			strcpy(pTmp, pPaths->Item(k));
			fileNames.push_back(pTmp);
		}

		if (fileNames.size()) {
			wxFileName test(pPaths->Item(0));
			strcpy(pTmp, test.GetPath());
			dicomSeriesFileNames->SetDirectory(pTmp);

			// ORDINAMENTO PER IMAGE POSITION
			dicomSeriesFileNames->SetFileNameSortingOrderToSortByImagePositionPatient();

			//itkReader->SetFileNames(fileNames);
			itkReader->SetFileNames(dicomSeriesFileNames->GetFileNames());
			itkReader->SetImageIO(itkDicomIO);

			// BUG: (A. Placitelli) itkReader assume che siano presenti
			// sempre almeno due files nella serie, condizione che non è sempre
			// rispettata. Quindi questa chiamata fallisce..
			try {
				//itkReader->UpdateLargestPossibleRegion();
				itkReader->UpdateOutputInformation();
			}
			catch (itk::ExceptionObject &/*error*/) {
				return false;
			}

			// by Nello per la gestione del progressDialog
			//this->updateBusyWindow( 3 );

			/*try {
				itkReader->Update();
			}
			catch (itk::ExceptionObject) {
				return EXIT_FAILURE;
			}*/

			_itkImage = itkReader->GetOutput();

			const DictionaryType & dictionary = itkDicomIO->GetMetaDataDictionary();
			std::string entryId = "0028|0002";
			std::string tagvalue;
			DictionaryType::ConstIterator tagItr = dictionary.Find( entryId );
			MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
			if( entryvalue )
				tagvalue = entryvalue->GetMetaDataObjectValue();
			_rgb = false;
			if(!tagvalue.compare("3"))
				_rgb = true;
			/*entryId = "0020|1041";
			tagItr = dictionary.Find( entryId );
			entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
			if( entryvalue )
				tagvalue = entryvalue->GetMetaDataObjectValue();*/

			if(_rgb) {
				_itkImage = 0;
				RGBSeriesReaderType::Pointer itkRgbReader = RGBSeriesReaderType::New();
				itkRgbReader->SetFileNames(fileNames);
				itkRgbReader->SetImageIO(itkDicomIO);

				if(!read) {
					try {
						itkRgbReader->UpdateLargestPossibleRegion();
					}
					catch (itk::ExceptionObject) {
						return false;
					}
					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkRgbReader->UpdateLargestPossibleRegion();
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}

					/*try {
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return EXIT_FAILURE;
					}*/
				
					_itkRgbImage = itkRgbReader->GetOutput();
					_itkRgbImage = computeRgbFlipping(_itkRgbImage);
					_itkVtkRgbConnector = RGBConnectorType::New();
					_itkVtkRgbConnector->SetInput(_itkRgbImage);
					_vtkImage = _itkVtkRgbConnector->GetOutput();
					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateRgbThumbnails();
				}
				readDicomHeader(itkDicomIO);
				readDicomEntries(itkDicomIO);

				// Legge la prima slice
				RGBReaderType::Pointer reader = RGBReaderType::New();
				ImageIOType::Pointer dicomIO = ImageIOType::New();

				wxString tmpFile(pPaths->Item(0));

				reader->SetFileName(tmpFile);
				reader->SetImageIO(dicomIO);
				reader->Update();

				RGBImageType::Pointer itkRgbImage = RGBImageType::New();
				itkRgbImage = reader->GetOutput();

				const DictionaryType & dictionary3 = itkRgbImage->GetMetaDataDictionary();
				entryId = "0008|0060";
				std::string tagvalue3;
				tagItr = dictionary3.Find( entryId );
				entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
				if( entryvalue )
					tagvalue3 = entryvalue->GetMetaDataObjectValue();

				if(tagvalue3.compare("US")) {

					// Lettura Header DICOM della prima slice: location
					const DictionaryType & dictionaryFirstSlice = itkRgbImage->GetMetaDataDictionary();
					entryId = "0020|1041";
					std::string tagvalueFirstSlice;
					tagItr = dictionaryFirstSlice.Find( entryId );
					//if(tagItr != NULL)
					if(tagItr != dictionaryFirstSlice.End())
						entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
					if( entryvalue )
						tagvalueFirstSlice = entryvalue->GetMetaDataObjectValue();
					
					// Legge la seconda slice
					tmpFile.assign(pPaths->Item(0));

					reader->SetFileName(tmpFile);
					reader->SetImageIO(dicomIO);
					reader->Update();

					itkRgbImage = reader->GetOutput();

					// Lettura Header DICOM della seconda slice: location
					const DictionaryType & dictionarySecondSlice = itkRgbImage->GetMetaDataDictionary();
					entryId = "0020|1041";
					std::string tagvalueSecondSlice;
					tagItr = dictionarySecondSlice.Find( entryId );
					//if(tagItr != NULL) 
					if(tagItr != dictionarySecondSlice.End()) 
						entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
					if( entryvalue )
						tagvalueSecondSlice = entryvalue->GetMetaDataObjectValue();

					
					_offset = atof(tagvalueSecondSlice.c_str()) - atof(tagvalueFirstSlice.c_str());

					//_offset = atof(tagvalue2.c_str()) - atof(tagvalue.c_str());
					if(_offset < 0) {
						_offset = -_offset;
						_downUp = true;
					}
					else
						_downUp = false;
					//_offset = _offset / (_slicesNumber - 1);
				}

				//convertRgbToGrayVtkImage(true);
			}
			else {
				if(!read) {
					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkReader->UpdateLargestPossibleRegion();
						itkReader->Update();
					}
					catch (itk::ExceptionObject & error) {
						return false;
					}

					// by Nello per la gestione del progressDialog
					//this->updateBusyWindow( 5 );

					//_itkImage = computeFlipping(_itkImage);
					_itkImage = computeFlipping(itkReader->GetOutput());
					_itkVtkConnector = ConnectorType::New();
					_itkVtkConnector->SetInput(_itkImage);
					_vtkImage = _itkVtkConnector->GetOutput();

					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateThumbnails();
				}
				readDicomHeader(itkDicomIO);
				readDicomEntries(itkDicomIO);

				// Legge la prima slice
				ReaderType::Pointer reader = ReaderType::New();
				ImageIOType::Pointer dicomIO = ImageIOType::New();

				wxString tmpFile(pPaths->Item(0));

				reader->SetFileName(tmpFile.c_str());
				reader->SetImageIO(dicomIO);
				reader->Update();

				ImageType::Pointer itkImage = ImageType::New();
				itkImage = reader->GetOutput();

				// Lettura Header DICOM della prima slice: location
				const DictionaryType & dictionaryFirstSlice = itkImage->GetMetaDataDictionary();
				entryId = "0020|1041";
				std::string tagvalueFirstSlice;
				tagItr = dictionaryFirstSlice.Find( entryId );
				//if(tagItr != NULL) 
				if(tagItr != dictionaryFirstSlice.End())	// Compatibilità VS208
					entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
				if( entryvalue )
					tagvalueFirstSlice = entryvalue->GetMetaDataObjectValue();

				// Legge la seconda slice
				tmpFile.assign(pPaths->Item(1));

				reader->SetFileName(tmpFile.c_str());
				reader->SetImageIO(dicomIO);
				reader->Update();

				itkImage = reader->GetOutput();

				// Lettura Header DICOM della seconda slice: location
				const DictionaryType & dictionarySecondSlice = itkImage->GetMetaDataDictionary();
				entryId = "0020|1041";
				std::string tagvalueSecondSlice;
				tagItr = dictionarySecondSlice.Find( entryId );
				//if(tagItr != NULL) 
				if(tagItr != dictionaryFirstSlice.End())	// Compatibilità VS208
					entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
				if( entryvalue )
					tagvalueSecondSlice = entryvalue->GetMetaDataObjectValue();

				
				_offset = atof(tagvalueSecondSlice.c_str()) - atof(tagvalueFirstSlice.c_str());
				if(_offset < 0) {
					_offset = -_offset;
					_downUp = true;
				}
				else
					_downUp = false;
				//_offset = _offset / (_slicesNumber - 1);
			}
			_imageViewPlane = calculateImageViewPlane(itkDicomIO);
		} 
		else 
			return false;
	} else 
	{
		ReaderType::Pointer itkReader = ReaderType::New();
		ImageIOType::Pointer itkDicomIO = ImageIOType::New();
		
		itkReader->SetFileName(pPaths->Item(0));
		itkReader->SetImageIO(itkDicomIO);

		if(itkDicomIO->CanReadFile(pPaths->Item(0)) == false)
			return false;

		else {

			try {
				//itkReader->UpdateLargestPossibleRegion();
				itkReader->UpdateOutputInformation();
			}
			catch (itk::ExceptionObject) {
				return false;
			}

			_itkImage = ImageType::New();
			_itkImage = itkReader->GetOutput();
			


			// Lettura Header DICOM: immagine RGB?
			const DictionaryType & dictionary = _itkImage->GetMetaDataDictionary();
			std::string entryId = "0028|0002";
			std::string tagvalue;
			DictionaryType::ConstIterator tagItr = dictionary.Find( entryId );
			MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
			if( entryvalue )
				tagvalue = entryvalue->GetMetaDataObjectValue();
			_rgb = false;
			if(!tagvalue.compare("3")) _rgb = true;

			if(_rgb) {
				_itkImage = 0;

				RGBReaderType::Pointer itkRgbReader = RGBReaderType::New();
				itkRgbReader->SetFileName(pPaths->Item(0));
				itkRgbReader->SetImageIO(itkDicomIO);

				if(!read) {
					try {
						//itkRgbReader->UpdateLargestPossibleRegion();
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}
					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkRgbReader->UpdateLargestPossibleRegion();
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}

					_itkRgbImage = itkRgbReader->GetOutput();
					_itkRgbImage = computeRgbFlipping(_itkRgbImage);
					_itkVtkRgbConnector = RGBConnectorType::New();
					_itkVtkRgbConnector->SetInput(_itkRgbImage);
					_vtkImage = _itkVtkRgbConnector->GetOutput();

					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateRgbThumbnails();
				}

				//convertRgbToGrayVtkImage(true);
			}
			else {
				if(!read) {
					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkReader->UpdateLargestPossibleRegion();
						itkReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}

					//_itkImage = computeFlipping(_itkImage);
					_itkImage = computeFlipping(itkReader->GetOutput());
					_itkVtkConnector = ConnectorType::New();
					_itkVtkConnector->SetInput(_itkImage);
					_vtkImage = _itkVtkConnector->GetOutput();
					
					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateThumbnails();

				}
			}
			readDicomHeader(itkDicomIO);
			readDicomEntries(itkDicomIO);

			return true;
		}
	}


	return true;
}

/*
* Apre una serie di immagini dicom
* Il metodo è richiamato dal metodo open
*/
bool itkVtkData::openSeries(string fileName, bool read) {
/*	wxDir wxDirectory(fileName.c_str());
	wxString fileInDir;
	if (wxDirectory.IsOpened()) {
		ImageIOType::Pointer itkDicomIO = ImageIOType::New();
		SeriesReaderType::Pointer itkReader = SeriesReaderType::New();
		itkReader->SetImageIO(itkDicomIO);

		NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
		namesGenerator->SetUseSeriesDetails( true );
		namesGenerator->SetDirectory( (wxDirectory.GetName()).c_str() );

		typedef std::vector < std::string > SeriesIdContainer;
		const SeriesIdContainer & seriesUID = namesGenerator->GetSeriesUIDs();
		SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
		SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
		while( seriesItr != seriesEnd ) {
			seriesItr++;
		}
		std::string seriesIdentifier;
		seriesIdentifier = seriesUID.begin()->c_str();
		typedef std::vector< std::string > FileNamesContainer;
		FileNamesContainer fileNames;
		fileNames = namesGenerator->GetFileNames( seriesIdentifier );
		itkReader->SetFileNames( fileNames );
*/
	DICOMSeriesFileNames::Pointer dicomSeriesFileNames = DICOMSeriesFileNames::New();
	

	SeriesReaderType::Pointer itkReader = SeriesReaderType::New();
	ImageIOType::Pointer itkDicomIO = ImageIOType::New();
	wxDir wxDirectory(fileName.c_str());
	wxString fileInDir;
	if (wxDirectory.IsOpened()) {
		vector<string> fileNames;
		bool cont = wxDirectory.GetFirst(&fileInDir);
		while (cont)
		{
			string tmpString = fileName.c_str();
			tmpString += "/";
			tmpString += fileInDir.c_str();
			wxString tmpFile(tmpString.c_str());
			//if (itkDicomIO->CanReadFile(tmpFile.c_str()) == true)
			if((tmpString.find(".dcm", tmpString.size()-4) != -1) || ((itkDicomIO->CanReadFile(tmpFile.c_str()) == true) && (tmpString.find("DICOMDIR", tmpString.size()-8) == -1)))
				fileNames.push_back(tmpString);
			cont = wxDirectory.GetNext(&fileInDir);
		}
		if (fileNames.size()) {
			dicomSeriesFileNames->SetDirectory(fileName.c_str());
			
			// ORDINAMENTO PER IMAGE POSITION
			dicomSeriesFileNames->SetFileNameSortingOrderToSortByImagePositionPatient();
			
			//itkReader->SetFileNames(fileNames);
			itkReader->SetFileNames(dicomSeriesFileNames->GetFileNames());
			itkReader->SetImageIO(itkDicomIO);

			try {
				//itkReader->UpdateLargestPossibleRegion();
				itkReader->UpdateOutputInformation();
			}
			catch (itk::ExceptionObject) {
				return false;
			}

			/*try {
				itkReader->Update();
			}
			catch (itk::ExceptionObject) {
				return EXIT_FAILURE;
			}*/

			_itkImage = itkReader->GetOutput();

			const DictionaryType & dictionary = itkDicomIO->GetMetaDataDictionary();
			std::string entryId = "0028|0002";
			std::string tagvalue;
			DictionaryType::ConstIterator tagItr = dictionary.Find( entryId );
			MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
			if( entryvalue )
				tagvalue = entryvalue->GetMetaDataObjectValue();
			_rgb = false;
			if(!tagvalue.compare("3"))
				_rgb = true;
			/*entryId = "0020|1041";
			tagItr = dictionary.Find( entryId );
			entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
			if( entryvalue )
				tagvalue = entryvalue->GetMetaDataObjectValue();*/

			if(_rgb) {
				_itkImage = 0;
				RGBSeriesReaderType::Pointer itkRgbReader = RGBSeriesReaderType::New();
				itkRgbReader->SetFileNames(fileNames);
				itkRgbReader->SetImageIO(itkDicomIO);

				if(!read) {
					try {
						itkRgbReader->UpdateLargestPossibleRegion();
					}
					catch (itk::ExceptionObject) {
						return false;
					}
					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkRgbReader->UpdateLargestPossibleRegion();
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}

					/*try {
						itkRgbReader->Update();
					}
					catch (itk::ExceptionObject) {
						return EXIT_FAILURE;
					}*/
				
					_itkRgbImage = itkRgbReader->GetOutput();
					_itkRgbImage = computeRgbFlipping(_itkRgbImage);
					_itkVtkRgbConnector = RGBConnectorType::New();
					_itkVtkRgbConnector->SetInput(_itkRgbImage);
					_vtkImage = _itkVtkRgbConnector->GetOutput();
					RGBImageType::SizeType size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateRgbThumbnails();
				}
				readDicomHeader(itkDicomIO);
				readDicomEntries(itkDicomIO);

				// Legge la prima slice
				RGBReaderType::Pointer reader = RGBReaderType::New();
				ImageIOType::Pointer dicomIO = ImageIOType::New();
				wxDirectory.GetFirst(&fileInDir);
				
				string tmpString = fileName.c_str();
				tmpString += "/";
				tmpString += fileInDir.c_str();
				wxString tmpFile(tmpString.c_str());

				reader->SetFileName(tmpString.c_str());
				reader->SetImageIO(dicomIO);
				reader->Update();

				RGBImageType::Pointer itkRgbImage = RGBImageType::New();
				itkRgbImage = reader->GetOutput();

				const DictionaryType & dictionary3 = itkRgbImage->GetMetaDataDictionary();
				entryId = "0008|0060";
				std::string tagvalue3;
				tagItr = dictionary3.Find( entryId );
				entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
				if( entryvalue )
					tagvalue3 = entryvalue->GetMetaDataObjectValue();

				if(tagvalue3.compare("US")) {

					// Lettura Header DICOM della prima slice: location
					const DictionaryType & dictionaryFirstSlice = itkRgbImage->GetMetaDataDictionary();
					entryId = "0020|1041";
					std::string tagvalueFirstSlice;
					tagItr = dictionaryFirstSlice.Find( entryId );
					//if(tagItr != NULL)
					if(tagItr != dictionaryFirstSlice.End())
						entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
					if( entryvalue )
						tagvalueFirstSlice = entryvalue->GetMetaDataObjectValue();
					
					// Legge la seconda slice
					wxDirectory.GetNext(&fileInDir);
					tmpString = fileName.c_str();
					tmpString += "/";
					tmpString += fileInDir.c_str();
					tmpFile.assign(tmpString.c_str());

					reader->SetFileName(tmpString.c_str());
					reader->SetImageIO(dicomIO);
					reader->Update();

					itkRgbImage = reader->GetOutput();

					// Lettura Header DICOM della seconda slice: location
					const DictionaryType & dictionarySecondSlice = itkRgbImage->GetMetaDataDictionary();
					entryId = "0020|1041";
					std::string tagvalueSecondSlice;
					tagItr = dictionarySecondSlice.Find( entryId );
					//if(tagItr != NULL) 
					if(tagItr != dictionarySecondSlice.End()) 
						entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
					if( entryvalue )
						tagvalueSecondSlice = entryvalue->GetMetaDataObjectValue();

					
					_offset = atof(tagvalueSecondSlice.c_str()) - atof(tagvalueFirstSlice.c_str());

					//_offset = atof(tagvalue2.c_str()) - atof(tagvalue.c_str());
					if(_offset < 0) {
						_offset = -_offset;
						_downUp = true;
					}
					else
						_downUp = false;
					//_offset = _offset / (_slicesNumber - 1);
				}

				//convertRgbToGrayVtkImage(true);
			}
			else {
				if(!read) {
					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
				}
				else {
					try {
						//itkReader->UpdateLargestPossibleRegion();
						itkReader->Update();
					}
					catch (itk::ExceptionObject) {
						return false;
					}

					//_itkImage = computeFlipping(_itkImage);
					_itkImage = computeFlipping(itkReader->GetOutput());
								
					_itkVtkConnector = ConnectorType::New();
					_itkVtkConnector->SetInput(_itkImage);
					_vtkImage = _itkVtkConnector->GetOutput();

					ImageType::SizeType size = _itkImage->GetLargestPossibleRegion().GetSize();
					_slicesNumber = size[2];
					generateThumbnails();
				}
				readDicomHeader(itkDicomIO);
				readDicomEntries(itkDicomIO);

				// Legge la prima slice
				ReaderType::Pointer reader = ReaderType::New();
				ImageIOType::Pointer dicomIO = ImageIOType::New();
				wxDirectory.GetFirst(&fileInDir);
				
				string tmpString = fileName.c_str();
				tmpString += "/";
				tmpString += fileInDir.c_str();
				wxString tmpFile(tmpString.c_str());

				reader->SetFileName(tmpString.c_str());
				reader->SetImageIO(dicomIO);
				reader->Update();

				ImageType::Pointer itkImage = ImageType::New();
				itkImage = reader->GetOutput();

				// Lettura Header DICOM della prima slice: location
				const DictionaryType & dictionaryFirstSlice = itkImage->GetMetaDataDictionary();
				entryId = "0020|1041";
				std::string tagvalueFirstSlice;
				tagItr = dictionaryFirstSlice.Find( entryId );
				//if(tagItr != NULL) 
				if(tagItr != dictionaryFirstSlice.End()) 
					entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
				if( entryvalue )
					tagvalueFirstSlice = entryvalue->GetMetaDataObjectValue();

				// Legge la seconda slice
				wxDirectory.GetNext(&fileInDir);
				tmpString = fileName.c_str();
				tmpString += "/";
				tmpString += fileInDir.c_str();
				tmpFile.assign(tmpString.c_str());

				reader->SetFileName(tmpString.c_str());
				reader->SetImageIO(dicomIO);
				reader->Update();

				itkImage = reader->GetOutput();

				// Lettura Header DICOM della seconda slice: location
				const DictionaryType & dictionarySecondSlice = itkImage->GetMetaDataDictionary();
				entryId = "0020|1041";
				std::string tagvalueSecondSlice;
				tagItr = dictionarySecondSlice.Find( entryId );
				//if(tagItr != NULL) 
				if(tagItr != dictionarySecondSlice.End()) 
					entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
				if( entryvalue )
					tagvalueSecondSlice = entryvalue->GetMetaDataObjectValue();

				
				_offset = atof(tagvalueSecondSlice.c_str()) - atof(tagvalueFirstSlice.c_str());
				if(_offset < 0) {
					_offset = -_offset;
					_downUp = true;
				}
				else
					_downUp = false;
				//_offset = _offset / (_slicesNumber - 1);
			}
			_imageViewPlane = calculateImageViewPlane(itkDicomIO);
		} 
		else return false;
	}
	else return false;

	return true;
}

void itkVtkData::resetVtkData() {
	if(!_rgb) {
		_itkVtkConnector = ConnectorType::New();
		_itkVtkConnector->SetInput(_itkImage);
		_vtkImage = _itkVtkConnector->GetOutput();
	}
	else {
		_itkVtkRgbConnector = RGBConnectorType::New();
		_itkVtkRgbConnector->SetInput(_itkRgbImage);
		_vtkImage = _itkVtkRgbConnector->GetOutput();
	}
}

void itkVtkData::generateThumbnails() {
	ImageType::RegionType inputRegion = _itkImage->GetLargestPossibleRegion();
	ImageType::SizeType regionSize = inputRegion.GetSize();
	regionSize[2] = 0;
	ImageType::IndexType start = inputRegion.GetIndex();
	_thumbnails = new wxImage[_slicesNumber];

	for (int index = 0; index < _slicesNumber; index++)
	{
		start[2] = index;
		ImageType::RegionType desiredRegion;
		desiredRegion.SetSize(regionSize);
		desiredRegion.SetIndex(start);
	
		ExtractImageType::Pointer filter = ExtractImageType::New();
		filter->SetExtractionRegion(desiredRegion);
		filter->SetInput(_itkImage);
		RescaleType::Pointer rescale = RescaleType::New();
		rescale->SetInput(filter->GetOutput());
		rescale->SetOutputMinimum(0);
		rescale->SetOutputMaximum(255);
		CastType::Pointer cast = CastType::New();
		cast->SetInput(rescale->GetOutput());
		cast->Update();
	
		ImageUChar::Pointer image = ImageUChar::New();
		image =	cast->GetOutput();
		ImageUChar::RegionType region = image->GetLargestPossibleRegion();
		ImageUChar::SizeType   size   = region.GetSize();
		const ImageUChar::SpacingType& spacing = image->GetSpacing();
	
		unsigned int reserved         =   0;
		unsigned int sizeOfHeader     =  40;
		unsigned int imageWidth       =  size[0];
		unsigned int imageHeight      =  size[1];
		const unsigned int lineSizePadded = (imageWidth % 4) ?  4 - imageWidth % 4 + imageWidth : imageWidth;
		const unsigned int imageSizeInPixels = lineSizePadded * imageHeight;

		unsigned short int bitPlanes        =   1;
		unsigned short int bitsPerPixel     =   8;

		unsigned int compressionType  =  0;
		unsigned int sizeCompressed   =  imageSizeInPixels;
		unsigned int horizontalResolution = static_cast< unsigned int >( 1000.0 / spacing[0] );
		unsigned int verticalResolution   = static_cast< unsigned int >( 1000.0 / spacing[1] );

		unsigned int numberOfColorsUsed      = 256;
		unsigned int numberOfImportantColors = 256;

		const unsigned int numberOfColorComponents  = 4;

		unsigned int offsetToPicture  =   54 + numberOfColorsUsed * numberOfColorComponents;
		unsigned int fileSize         =   offsetToPicture + imageSizeInPixels;

		unsigned char *palette = new unsigned char[numberOfColorsUsed * numberOfColorComponents];

		for(unsigned int i=0; i<numberOfColorsUsed; i++)
		{
			unsigned int index = i * numberOfColorComponents;
			for(unsigned char j=0; j<numberOfColorComponents; j++)
			{
				palette[index+j] = i;
			}
		}

		wxMemoryOutputStream bmp;
		bmp.Write("BM",2); 
		bmp.Write((char *)&fileSize, 4 );
		bmp.Write((char *)&reserved, 4 );
		bmp.Write((char *)&offsetToPicture, 4 );
		bmp.Write((char *)&sizeOfHeader, 4 );
		bmp.Write((char *)&imageWidth, 4 );
		bmp.Write((char *)&imageHeight, 4 );
		bmp.Write((char *)&bitPlanes, 2 );
		bmp.Write((char *)&bitsPerPixel, 2 );
		bmp.Write((char *)&compressionType, 4 );
		bmp.Write((char *)&sizeCompressed, 4 );
		bmp.Write((char *)&horizontalResolution, 4 );
		bmp.Write((char *)&verticalResolution, 4 );
		bmp.Write((char *)&numberOfColorsUsed, 4 );
		bmp.Write((char *)&numberOfImportantColors, 4 );
		bmp.Write((char *)palette, numberOfColorsUsed * numberOfColorComponents );
		delete [] palette;

		unsigned char* line = new unsigned char [lineSizePadded];
		for(unsigned int c=0; c<lineSizePadded; c++)
		{
			line[c] = 0;
		}
	
		IteratorType it( image, region );
		it.SetDirection(0);
		it.GoToBegin();
		while( !it.IsAtEnd() )
		{
			it.GoToBeginOfLine();
			unsigned int i=0;
			while( !it.IsAtEndOfLine() )
			{
				line[i] = it.Get();
				++it;
				++i;
			}
			bmp.Write((unsigned char *)line, lineSizePadded );
			it.NextLine();
		}
		wxMemoryInputStream is(bmp);
		wxImage* thumb = new wxImage(is);
		_thumbnails[index] = *thumb;
		bmp.Close();
		delete [] line;
		delete thumb;

		// by Nello per la gestione del progressDialog
		float perc = ((index+1.0)/_slicesNumber)*99;
		this->updateBusyWindow( perc );
	}
	// by Nello per la gestione del progressDialog
	this->clearBusyWindow();	
}

void itkVtkData::generateRgbThumbnails() {
	RGBImageType::RegionType inputRegion = _itkRgbImage->GetLargestPossibleRegion();
	RGBImageType::SizeType regionSize = inputRegion.GetSize();
	regionSize[2] = 0;
	RGBImageType::IndexType start = inputRegion.GetIndex();
	_thumbnails = new wxImage[_slicesNumber];
	for (int index = 0; index < _slicesNumber; index++)
	{
		start[2] = index;
		RGBImageType::RegionType desiredRegion;
		desiredRegion.SetSize(regionSize);
		desiredRegion.SetIndex(start);

		RGBExtractImageType::Pointer filter = RGBExtractImageType::New();
		filter->SetExtractionRegion(desiredRegion);
		filter->SetInput(_itkRgbImage);

		typedef itk::RGBToLuminanceImageFilter < RGBImageType2D, ImageUChar > ToScalar;
		ToScalar::Pointer toScalar = ToScalar::New();
		toScalar->SetInput(filter->GetOutput());
		toScalar->Update();
	
		RGBImageType2D::Pointer image = RGBImageType2D::New();
		image = filter->GetOutput();

		RGBImageType2D::RegionType region = image->GetLargestPossibleRegion();
		RGBImageType2D::SizeType size   = region.GetSize();
		const RGBImageType2D::SpacingType& spacing = image->GetSpacing();

		unsigned int reserved         =   0;
		unsigned int sizeOfHeader     =  40;
		unsigned int imageWidth       =  size[0];
		unsigned int imageHeight      =  size[1];
		const unsigned int lineSizePadded = (imageWidth % 4) ?  4 - imageWidth % 4 + imageWidth : imageWidth;
		const unsigned int imageSizeInPixels = lineSizePadded * imageHeight;
		unsigned short int bitPlanes        =   1;
		unsigned short int bitsPerPixel     =   24;
		unsigned int compressionType  =  0;
		unsigned int sizeCompressed   =  imageSizeInPixels;
		unsigned int horizontalResolution = static_cast< unsigned int >( 1000.0 / spacing[0] );
		unsigned int verticalResolution   = static_cast< unsigned int >( 1000.0 / spacing[1] );
		unsigned int numberOfColorsUsed      = 256;
		unsigned int numberOfImportantColors = 256;
		const unsigned int numberOfColorComponents  = 4;
		unsigned int offsetToPicture  =   54;
		unsigned int fileSize         =   offsetToPicture + imageSizeInPixels;

		wxMemoryOutputStream bmp;
		bmp.Write("BM",2);
		bmp.Write((char *)&fileSize, 4 );
		bmp.Write((char *)&reserved, 4 );
		bmp.Write((char *)&offsetToPicture, 4 );
		bmp.Write((char *)&sizeOfHeader, 4 );
		bmp.Write((char *)&imageWidth, 4 );
		bmp.Write((char *)&imageHeight, 4 );
		bmp.Write((char *)&bitPlanes, 2 );
		bmp.Write((char *)&bitsPerPixel, 2 );
		bmp.Write((char *)&compressionType, 4 );
		bmp.Write((char *)&sizeCompressed, 4 );
		bmp.Write((char *)&horizontalResolution, 4 );
		bmp.Write((char *)&verticalResolution, 4 );
		bmp.Write((char *)&numberOfColorsUsed, 4 );
		bmp.Write((char *)&numberOfImportantColors, 4 );

		unsigned char* line = new unsigned char[lineSizePadded*3];
		for(unsigned int c=0; c<lineSizePadded*3; c++)
		{
			line[c] = 0;
		}

		typedef itk::ImageLinearConstIteratorWithIndex<RGBImageType2D> IteratorType;
		IteratorType it( image, region );
		it.SetDirection(0);
		it.GoToBegin();
		while( !it.IsAtEnd() )
		{
			it.GoToBeginOfLine();
			unsigned int i=0;
			while( !it.IsAtEndOfLine() )
			{
				line[i] = it.Get().GetBlue();
				line[++i] = it.Get().GetGreen();
				line[++i] = it.Get().GetRed();
				++it;
				++i;
			}
			bmp.Write((unsigned char*)line, lineSizePadded*3 );
			it.NextLine();
		}
		wxMemoryInputStream is(bmp);
		wxImage* thumb = new wxImage(is);
		_thumbnails[index] = *thumb;
		bmp.Close();
		delete [] line;
		delete thumb;
	}
}

ImageType::Pointer itkVtkData::computeFlipping(ImageType::Pointer itkImage) 
{ 
	FlipImageType::Pointer filter = FlipImageType::New();
	typedef FlipImageType::FlipAxesArrayType FlipAxesArrayType; 
	FlipAxesArrayType flipArray;
	flipArray[0]=0;
	flipArray[1]=1;
	flipArray[2]=0;
	filter->SetFlipAxes( flipArray );
	filter->SetInput(itkImage);
	filter->Update();
	
	return filter->GetOutput(); 
}

RGBImageType::Pointer itkVtkData::computeRgbFlipping(RGBImageType::Pointer itkRgbImage) { 
	RGBFlipImageType::Pointer filter = RGBFlipImageType::New();
	typedef RGBFlipImageType::FlipAxesArrayType RGBFlipAxesArrayType; 
	RGBFlipAxesArrayType flipArray;
	flipArray[0]=0;
	flipArray[1]=1;
	flipArray[2]=0;
	filter->SetFlipAxes( flipArray );
	filter->SetInput(itkRgbImage);
	filter->Update();
	
	return filter->GetOutput(); 
}

int itkVtkData::readDicomHeader(ImageIOType::Pointer dicomIO) {
	const DictionaryType & itkDic = dicomIO->GetMetaDataDictionary();
	DictionaryType::ConstIterator itr = itkDic.Begin();
	DictionaryType::ConstIterator end = itkDic.End();
	string tagKey;
	string tagValue;

	while( itr != end )	{
		itk::MetaDataObjectBase::Pointer entry = itr->second;
		MetaDataStringType::Pointer entryvalue = dynamic_cast<MetaDataStringType *>( entry.GetPointer() ) ;
		if( entryvalue ) {
			tagKey = itr->first;
			tagValue = entryvalue->GetMetaDataObjectValue();
			_strVecDicomKey.push_back(tagKey);
			_strVecDicomValue.push_back(tagValue);
		}
		++itr;
	}

	return (int)_strVecDicomKey.size();
}


string itkVtkData::readDicomEntry(ImageIOType::Pointer dicomIO, string entryId) {
	const DictionaryType & itkDic = dicomIO->GetMetaDataDictionary();
	DictionaryType::ConstIterator tagItr = itkDic.Find( entryId );
	DictionaryType::ConstIterator end = itkDic.End();
	if( tagItr != end )
	{
		MetaDataStringType::ConstPointer entryvalue =
		dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer() );
		if( entryvalue )
			return entryvalue->GetMetaDataObjectValue();
	}
	return "";
}

void itkVtkData::setDicomEntry(ImageIOType::Pointer dicomIO, string entryId, string entryvalue) {
	DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();
	itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
}

int itkVtkData::readDicomEntries(ImageIOType::Pointer dicomIO) {
	//0. Patient Name
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0010|0010"));
	//1. Patient ID
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0010|0020"));
	//2. Age
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0010|1010"));
	//3. Accession Number
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|0050"));
	//4. Study description
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|1030"));
	//5. Modality
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|0060"));
	//6. Study ID
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0020|0010"));
	//7. Date Acquired
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|0022"));
	//8. Number of slices
	ImageType::SizeType size;
	if(!_rgb) size = _itkImage->GetLargestPossibleRegion().GetSize();
	else size = _itkRgbImage->GetLargestPossibleRegion().GetSize();
	char sliceNumber[5];
	itoa(size[2],sliceNumber,10);
	_strVecDicomEntries.push_back(sliceNumber);
	//9. Referring Physician
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|0090"));
	//10. Performing
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|1050"));
	//11. Institution
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|0080"));
	//12. Date of Birth
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0010|0030"));
	//13. Image row
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0028|0010"));
	//14. Image column
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0028|0011"));
	//15. Slice Location
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0020|1041"));
	//16. Thickness
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0018|0050"));
	//17. Pixel spacing
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0028|0030"));
	//18. Image Position
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0020|0032"));
	//19. Image Orientation
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0020|0037"));
	//20. Series Description
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0008|103e"));
	//21. Protocol Name
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0018|1030"));

/*	//Window Level
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0028|1050"));
	//Window Width
	_strVecDicomEntries.push_back(readDicomEntry(dicomIO,"0028|1051"));*/

	string wl = readDicomEntry(dicomIO,"0028|1050");
	string ww = readDicomEntry(dicomIO,"0028|1051");
	if(strcmp(wl.c_str(),""))
		_wl = atof(wl.c_str());
	if(strcmp(ww.c_str(),""))
		_ww = atof(ww.c_str());

	return (int)_strVecDicomEntries.size();
}

int itkVtkData::getMinPixelValue()
{			
	double range[2];
	_vtkImage->GetScalarRange(range);
	return (int)range[0];
}

int itkVtkData::getMaxPixelValue() {			
	double range[2];
	_vtkImage->GetScalarRange(range);
	return (int)range[1];
}

float itkVtkData::computeSamplingDistance(int value) 
{	
	double spacing[3]; 
	_vtkImage->GetSpacing( spacing ); 
	
	float returnValue;
	if (value==0) // massimo supersampling - minspacing / 8
	{
		returnValue = (float)((*std::min_element(spacing,spacing+3))/8.0);
	}
	else if (value==1) // medio supersampling - minspacing / 4
	{
		returnValue = (float)((*std::min_element(spacing,spacing+3))/4.0);
	}
	else if (value==2) // basso supersampling - minspacing / 2
	{
		returnValue = (float)((*std::min_element(spacing,spacing+3))/2.0);
	}
	else if (value==3) // ritorna il minimo valore dello spacing
	{
		returnValue = (float)*std::min_element(spacing,spacing+3);
	}
	else // ritorna un multiplo della diagonale del voxel ( diagonal * sqrt(value) con 2 <= value <= 5) 
	{
		value = value - 2;
		double voxelDiagonal = sqrt( 
			spacing[0] * spacing[0] + 
			spacing[1] * spacing[1] + 
			spacing[2] * spacing[2] ); 
		returnValue = (float)(voxelDiagonal*sqrt((float)value));
	}

	return returnValue;
}

void itkVtkData::convertRgbToGrayVtkImage(bool deleteRgbImage) {
	if (_itkVtkConnector.IsNotNull() && _itkVtkConnector->GetReferenceCount() != 0) {
		_vtkImage = _itkVtkConnector->GetOutput();
	}
	else if (_itkRgbImage.IsNotNull() && (_itkRgbImage->GetReferenceCount() != 0)) {
		typedef itk::RGBToLuminanceImageFilter < RGBImageType, ImageType > ToScalar;

		ToScalar::Pointer filter = ToScalar::New();
		filter->SetInput(_itkRgbImage);
		filter->Update();
		_itkImage = filter->GetOutput();
		_itkVtkConnector = ConnectorType::New();
		_itkVtkConnector->SetInput(_itkImage);
		_itkVtkConnector->Update();
		_vtkImage = _itkVtkConnector->GetOutput();

		if(deleteRgbImage) {
			_rgb = false;
			_itkRgbImage = 0;
			_itkVtkRgbConnector = 0;
		}
	}
}


void itkVtkData::convertGrayToRgbVtkImage(bool deleteImage) {
	if (_itkVtkRgbConnector.IsNotNull() && _itkVtkRgbConnector->GetReferenceCount() != 0) {
		_vtkImage = _itkVtkRgbConnector->GetOutput();
	}
	else if (_itkImage.IsNotNull() && (_itkImage->GetReferenceCount() != 0)) {
		typedef itk::ComposeRGBImageFilter < ImageType, RGBImageType > ToRGB;

		ToRGB::Pointer filter = ToRGB::New();
		filter->SetInput(_itkImage);
		filter->Update();
		_itkRgbImage = filter->GetOutput();
		_itkVtkRgbConnector = RGBConnectorType::New();
		_itkVtkRgbConnector->SetInput(_itkRgbImage);
		_itkVtkRgbConnector->Update();
		_vtkImage = _itkVtkRgbConnector->GetOutput();

		if(deleteImage) {
			_rgb = true;
			_itkImage = 0;
			_itkVtkConnector = 0;
		}
	}
}


string itkVtkData::calculateImageViewPlane(ImageIOType::Pointer dicomIO) {
	string iop = readDicomEntry(dicomIO,"0020|0037");

	float cosines[6];
	int pos1 = 0;
	int pos2 = 0;
	int pos3 = 0;
	int pos4 = 0;
	int pos5 = 0;

	if (strcmp("",iop.c_str())!=0){
		pos1 = iop.find("\\",0);
		pos2 = iop.find("\\", pos1+1);
		pos3 = iop.find("\\", pos2+1);
		pos4 = iop.find("\\", pos3+1);
		pos5 = iop.find("\\", pos4+1);

		cosines[0] = atof(iop.substr(0,pos1).c_str());
		cosines[1] = atof(iop.substr(pos1+1,pos2).c_str());
		cosines[2] = atof(iop.substr(pos2+1,pos3).c_str());
		cosines[3] = atof(iop.substr(pos3+1,pos4).c_str());
		cosines[4] = atof(iop.substr(pos4+1,pos5).c_str());
		cosines[5] = atof(iop.substr(pos5+1,iop.size()).c_str());
	}

	string label;
	string rowAxis = getMajorAxisFromPatientRelativeDirectionCosine(cosines[0],cosines[1],cosines[2]);
	string colAxis = getMajorAxisFromPatientRelativeDirectionCosine(cosines[3],cosines[4],cosines[5]);
	if (rowAxis != "" && colAxis != "") {
		if      ((!rowAxis.compare("R") || !rowAxis.compare("L")) && (!colAxis.compare("A") || !colAxis.compare("P"))) label="AXIAL";
		else if ((!colAxis.compare("R") || !colAxis.compare("L")) && (!rowAxis.compare("A") || !rowAxis.compare("P"))) label="AXIAL";
	
		else if ((!rowAxis.compare("R") || !rowAxis.compare("L")) && (!colAxis.compare("H") || !colAxis.compare("F"))) label="CORONAL";
		else if ((!colAxis.compare("R") || !colAxis.compare("L")) && (!rowAxis.compare("H") || !rowAxis.compare("F"))) label="CORONAL";
	
		else if ((!rowAxis.compare("A") || !rowAxis.compare("P")) && (!colAxis.compare("H") || !colAxis.compare("F"))) label="SAGITTAL";
		else if ((!colAxis.compare("A") || !colAxis.compare("P")) && (!rowAxis.compare("H") || !rowAxis.compare("F"))) label="SAGITTAL";
	}
	else {
		label="OBLIQUE";
	}
	return label;
}


string itkVtkData::getMajorAxisFromPatientRelativeDirectionCosine(double x,double y,double z) {
	string axis;
	double obliquityThresholdCosineValue = 0.001;
		
	string orientationX = x < 0 ? "R" : "L";
	string orientationY = y < 0 ? "A" : "P";
	string orientationZ = z < 0 ? "F" : "H";

	double absX = abs(x);
	double absY = abs(y);
	double absZ = abs(z);

	// The tests here really don't need to check the other dimensions,
	// just the threshold, since the sum of the squares should be == 1.0
	// but just in case ...
	
	if (absX>obliquityThresholdCosineValue && absX>absY && absX>absZ) {
		axis=orientationX;
	}
	else if (absY>obliquityThresholdCosineValue && absY>absX && absY>absZ) {
		axis=orientationY;
	}
	else if (absZ>obliquityThresholdCosineValue && absZ>absX && absZ>absY) {
		axis=orientationZ;
	}
	else
		axis=orientationX;

	return axis;
}