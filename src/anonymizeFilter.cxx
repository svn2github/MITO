/**
 * \file anonymizeFilter.cxx
 * \brief Filtro per l'anonimizzazione di immagini DICOM
 * \author ICAR-CNR Napoli
 */

#include "anonymizeFilter.h"
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesWriter.h>


anonymizeFilter::anonymizeFilter(bool rgb) {
	_rgb = rgb;
}


void anonymizeFilter::anonymizeImage(string inputFilename, string outputFilename, anonymizeData* aData) {
	if(!_rgb) {
		ReaderType::Pointer reader = ReaderType::New();
		ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
		
		reader->SetFileName(inputFilename.c_str());
		reader->SetImageIO(gdcmImageIO);
		
		try {
			reader->Update();
		}
		catch (itk::ExceptionObject) {
			return;
		}

		ImageType::Pointer inputImage = reader->GetOutput();
		DictionaryType & dictionary = inputImage->GetMetaDataDictionary();

		std::string entryId = "";
		std::string entryvalue = "";

		if(strcmp(aData->accessionNumber.c_str(), "")) {
			entryId = ACCESSION_NUMBER;
			entryvalue = aData->accessionNumber;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->acquisitionDate.c_str(), "")) {
			entryId = ACQUISITION_DATE;
			entryvalue = aData->acquisitionDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->imageDate.c_str(), "")) {
			entryId = IMAGE_DATE;
			entryvalue = aData->imageDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->imageTime.c_str(), "")) {
			entryId = IMAGE_TIME;
			entryvalue = aData->imageTime;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->institutionName.c_str(), "")) {
			entryId = INSTITUTION_NAME;
			entryvalue = aData->institutionName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientAge.c_str(), "")) {
			entryId = PATIENT_AGE;
			entryvalue = aData->patientAge;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientBirthdate.c_str(), "")) {
			entryId = PATIENT_BIRTHDATE;
			entryvalue = aData->patientBirthdate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientId.c_str(), "")) {
			entryId = PATIENT_ID;
			entryvalue = aData->patientId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientName.c_str(), "")) {
			entryId = PATIENT_NAME;
			entryvalue = aData->patientName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientSex.c_str(), "")) {
			entryId = PATIENT_SEX;
			entryvalue = aData->patientSex;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientWeight.c_str(), "")) {
			entryId = PATIENT_WEIGHT;
			entryvalue = aData->patientWeight;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->performingPhysician.c_str(), "")) {
			entryId = PERFORMING_PHYSICIAN;
			entryvalue = aData->performingPhysician;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->physicianOfRecord.c_str(), "")) {
			entryId = PHYSICIAN_OF_RECORD;
			entryvalue = aData->physicianOfRecord;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->referringPhysician.c_str(), "")) {
			entryId = REFERRING_PHYSICIAN;
			entryvalue = aData->referringPhysician;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->seriesDate.c_str(), "")) {
			entryId = SERIES_DATE;
			entryvalue = aData->seriesDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->seriesTime.c_str(), "")) {
			entryId = SERIES_TIME;
			entryvalue = aData->seriesTime;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->studyDate.c_str(), "")) {
			entryId = STUDY_DATE;
			entryvalue = aData->studyDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->studyTime.c_str(), "")) {
			entryId = STUDY_TIME;
			entryvalue = aData->studyTime;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialCoordinatingCenterName.c_str(), "")) {
			entryId = TRIAL_COORDINATING_CENTER_NAME;
			entryvalue = aData->trialCoordinatingCenterName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialProtocolId.c_str(), "")) {
			entryId = TRIAL_PROTOCOL_ID;
			entryvalue = aData->trialProtocolId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialProtocolName.c_str(), "")) {
			entryId = TRIAL_PROTOCOL_NAME;
			entryvalue = aData->trialProtocolName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSiteId.c_str(), "")) {
			entryId = TRIAL_SITE_ID;
			entryvalue = aData->trialSiteId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSiteName.c_str(), "")) {
			entryId = TRIAL_SITE_NAME;
			entryvalue = aData->trialSiteName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSponsorName.c_str(), "")) {
			entryId = TRIAL_SPONSOR_NAME;
			entryvalue = aData->trialSponsorName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSubjectId.c_str(), "")) {
			entryId = TRIAL_SUBJECT_ID;
			entryvalue = aData->trialSubjectId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSubjectReadingId.c_str(), "")) {
			entryId = TRIAL_SUBJECT_READING_ID;
			entryvalue = aData->trialSubjectReadingId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialTimePointDescription.c_str(), "")) {
			entryId = TRIAL_TIME_POINT_DESCRIPTION;
			entryvalue = aData->trialTimePointDescription;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialTimePointId.c_str(), "")) {
			entryId = TRIAL_TIME_POINT_ID;
			entryvalue = aData->trialTimePointId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}

		entryId = RESCALE_INTERCEPT;
		entryvalue = "0";
		itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);

		typedef itk::ImageFileWriter <ImageType>  WriterType;

		WriterType::Pointer writer = WriterType::New();

		writer->SetInput(inputImage);
		writer->SetFileName(outputFilename.c_str());
		writer->SetImageIO(gdcmImageIO);

		try {
			writer->Update();
		}
		catch (itk::ExceptionObject &) {
			return;
		}
	}
	else {
		typedef itk::RGBPixel <unsigned char> RGBPixelType;
		typedef itk::Image <RGBPixelType, Dimension> RGBImageType;

		typedef itk::ImageFileReader <RGBImageType> RGBReaderType;

		RGBReaderType::Pointer reader = RGBReaderType::New();
		ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
		
		reader->SetFileName(inputFilename.c_str());
		reader->SetImageIO(gdcmImageIO);
		
		try {
			reader->Update();
		}
		catch (itk::ExceptionObject) {
			return;
		}

		RGBImageType::Pointer inputImage = reader->GetOutput();
		DictionaryType & dictionary = inputImage->GetMetaDataDictionary();

		std::string entryId = "";
		std::string entryvalue = "";

		if(strcmp(aData->accessionNumber.c_str(), "")) {
			entryId = ACCESSION_NUMBER;
			entryvalue = aData->accessionNumber;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->acquisitionDate.c_str(), "")) {
			entryId = ACQUISITION_DATE;
			entryvalue = aData->acquisitionDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->imageDate.c_str(), "")) {
			entryId = IMAGE_DATE;
			entryvalue = aData->imageDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->imageTime.c_str(), "")) {
			entryId = IMAGE_TIME;
			entryvalue = aData->imageTime;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->institutionName.c_str(), "")) {
			entryId = INSTITUTION_NAME;
			entryvalue = aData->institutionName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientAge.c_str(), "")) {
			entryId = PATIENT_AGE;
			entryvalue = aData->patientAge;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientBirthdate.c_str(), "")) {
			entryId = PATIENT_BIRTHDATE;
			entryvalue = aData->patientBirthdate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientId.c_str(), "")) {
			entryId = PATIENT_ID;
			entryvalue = aData->patientId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientName.c_str(), "")) {
			entryId = PATIENT_NAME;
			entryvalue = aData->patientName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientSex.c_str(), "")) {
			entryId = PATIENT_SEX;
			entryvalue = aData->patientSex;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->patientWeight.c_str(), "")) {
			entryId = PATIENT_WEIGHT;
			entryvalue = aData->patientWeight;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->performingPhysician.c_str(), "")) {
			entryId = PERFORMING_PHYSICIAN;
			entryvalue = aData->performingPhysician;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->physicianOfRecord.c_str(), "")) {
			entryId = PHYSICIAN_OF_RECORD;
			entryvalue = aData->physicianOfRecord;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->referringPhysician.c_str(), "")) {
			entryId = REFERRING_PHYSICIAN;
			entryvalue = aData->referringPhysician;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->seriesDate.c_str(), "")) {
			entryId = SERIES_DATE;
			entryvalue = aData->seriesDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->seriesTime.c_str(), "")) {
			entryId = SERIES_TIME;
			entryvalue = aData->seriesTime;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->studyDate.c_str(), "")) {
			entryId = STUDY_DATE;
			entryvalue = aData->studyDate;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->studyTime.c_str(), "")) {
			entryId = STUDY_TIME;
			entryvalue = aData->studyTime;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialCoordinatingCenterName.c_str(), "")) {
			entryId = TRIAL_COORDINATING_CENTER_NAME;
			entryvalue = aData->trialCoordinatingCenterName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialProtocolId.c_str(), "")) {
			entryId = TRIAL_PROTOCOL_ID;
			entryvalue = aData->trialProtocolId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialProtocolName.c_str(), "")) {
			entryId = TRIAL_PROTOCOL_NAME;
			entryvalue = aData->trialProtocolName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSiteId.c_str(), "")) {
			entryId = TRIAL_SITE_ID;
			entryvalue = aData->trialSiteId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSiteName.c_str(), "")) {
			entryId = TRIAL_SITE_NAME;
			entryvalue = aData->trialSiteName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSponsorName.c_str(), "")) {
			entryId = TRIAL_SPONSOR_NAME;
			entryvalue = aData->trialSponsorName;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSubjectId.c_str(), "")) {
			entryId = TRIAL_SUBJECT_ID;
			entryvalue = aData->trialSubjectId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialSubjectReadingId.c_str(), "")) {
			entryId = TRIAL_SUBJECT_READING_ID;
			entryvalue = aData->trialSubjectReadingId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialTimePointDescription.c_str(), "")) {
			entryId = TRIAL_TIME_POINT_DESCRIPTION;
			entryvalue = aData->trialTimePointDescription;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}
		if(strcmp(aData->trialTimePointId.c_str(), "")) {
			entryId = TRIAL_TIME_POINT_ID;
			entryvalue = aData->trialTimePointId;
			itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		}

		entryId = RESCALE_INTERCEPT;
		entryvalue = "0";
		itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);

		typedef itk::ImageFileWriter <RGBImageType>  RGBWriterType;

		RGBWriterType::Pointer writer = RGBWriterType::New();

		writer->SetInput(inputImage);
		writer->SetFileName(outputFilename.c_str());
		writer->SetImageIO(gdcmImageIO);

		try {
			writer->Update();
		}
		catch (itk::ExceptionObject &) {
			return;
		}
	}
}


void anonymizeFilter::anonymizeSeries(string inputDirectory, string outputDirectory, anonymizeData* aData) {
	ImageIOType::Pointer gdcmIO = ImageIOType::New();
	NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();

	namesGenerator->SetInputDirectory(inputDirectory);

	if(!_rgb) {
		const SeriesReaderType::FileNamesContainer & filenames =  namesGenerator->GetInputFileNames();

		unsigned int numberOfFilenames =  filenames.size();

		if(numberOfFilenames) {
			SeriesReaderType::Pointer reader = SeriesReaderType::New();

			reader->SetImageIO(gdcmIO);
			reader->SetFileNames(filenames);

			try	{
				reader->Update();
			}
			catch (itk::ExceptionObject &) {
				return;
			}
			ImageType::Pointer inputImage = reader->GetOutput();

			typedef DictionaryType*  DictionaryRawPointer;
			typedef std::vector<DictionaryRawPointer> DictionaryArrayType;
			typedef const DictionaryArrayType* DictionaryArrayRawPointer;

			DictionaryType* dictionary;
			DictionaryArrayRawPointer dictionaryArray = reader->GetMetaDataDictionaryArray();
			std::string entryId = "";
			std::string entryvalue = "";

			for(int i=0; i < dictionaryArray->size(); i++)  {
				dictionary = dictionaryArray->at(i);

				if(strcmp(aData->accessionNumber.c_str(), "")) {
					entryId = ACCESSION_NUMBER;
					entryvalue = aData->accessionNumber;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->acquisitionDate.c_str(), "")) {
					entryId = ACQUISITION_DATE;
					entryvalue = aData->acquisitionDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->imageDate.c_str(), "")) {
					entryId = IMAGE_DATE;
					entryvalue = aData->imageDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->imageTime.c_str(), "")) {
					entryId = IMAGE_TIME;
					entryvalue = aData->imageTime;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->institutionName.c_str(), "")) {
					entryId = INSTITUTION_NAME;
					entryvalue = aData->institutionName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientAge.c_str(), "")) {
					entryId = PATIENT_AGE;
					entryvalue = aData->patientAge;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientBirthdate.c_str(), "")) {
					entryId = PATIENT_BIRTHDATE;
					entryvalue = aData->patientBirthdate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientId.c_str(), "")) {
					entryId = PATIENT_ID;
					entryvalue = aData->patientId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientName.c_str(), "")) {
					entryId = PATIENT_NAME;
					entryvalue = aData->patientName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientSex.c_str(), "")) {
					entryId = PATIENT_SEX;
					entryvalue = aData->patientSex;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientWeight.c_str(), "")) {
					entryId = PATIENT_WEIGHT;
					entryvalue = aData->patientWeight;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->performingPhysician.c_str(), "")) {
					entryId = PERFORMING_PHYSICIAN;
					entryvalue = aData->performingPhysician;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->physicianOfRecord.c_str(), "")) {
					entryId = PHYSICIAN_OF_RECORD;
					entryvalue = aData->physicianOfRecord;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->referringPhysician.c_str(), "")) {
					entryId = REFERRING_PHYSICIAN;
					entryvalue = aData->referringPhysician;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->seriesDate.c_str(), "")) {
					entryId = SERIES_DATE;
					entryvalue = aData->seriesDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->seriesTime.c_str(), "")) {
					entryId = SERIES_TIME;
					entryvalue = aData->seriesTime;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->studyDate.c_str(), "")) {
					entryId = STUDY_DATE;
					entryvalue = aData->studyDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->studyTime.c_str(), "")) {
					entryId = STUDY_TIME;
					entryvalue = aData->studyTime;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialCoordinatingCenterName.c_str(), "")) {
					entryId = TRIAL_COORDINATING_CENTER_NAME;
					entryvalue = aData->trialCoordinatingCenterName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialProtocolId.c_str(), "")) {
					entryId = TRIAL_PROTOCOL_ID;
					entryvalue = aData->trialProtocolId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialProtocolName.c_str(), "")) {
					entryId = TRIAL_PROTOCOL_NAME;
					entryvalue = aData->trialProtocolName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSiteId.c_str(), "")) {
					entryId = TRIAL_SITE_ID;
					entryvalue = aData->trialSiteId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSiteName.c_str(), "")) {
					entryId = TRIAL_SITE_NAME;
					entryvalue = aData->trialSiteName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSponsorName.c_str(), "")) {
					entryId = TRIAL_SPONSOR_NAME;
					entryvalue = aData->trialSponsorName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSubjectId.c_str(), "")) {
					entryId = TRIAL_SUBJECT_ID;
					entryvalue = aData->trialSubjectId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSubjectReadingId.c_str(), "")) {
					entryId = TRIAL_SUBJECT_READING_ID;
					entryvalue = aData->trialSubjectReadingId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialTimePointDescription.c_str(), "")) {
					entryId = TRIAL_TIME_POINT_DESCRIPTION;
					entryvalue = aData->trialTimePointDescription;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialTimePointId.c_str(), "")) {
					entryId = TRIAL_TIME_POINT_ID;
					entryvalue = aData->trialTimePointId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}

				/*
				entryId = RESCALE_INTERCEPT;
				entryvalue = "0";
				itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				*/
			}
			
			typedef short OutputPixelType;
			const unsigned int OutputDimension = 2;

			typedef itk::Image <OutputPixelType, OutputDimension> Image2DType;

			typedef itk::ImageSeriesWriter <ImageType, Image2DType> SeriesWriterType;

			SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
			seriesWriter->SetInput(inputImage);
			seriesWriter->SetImageIO(gdcmIO);

			namesGenerator->SetOutputDirectory(outputDirectory);

			seriesWriter->SetFileNames(namesGenerator->GetOutputFileNames());
			seriesWriter->SetMetaDataDictionaryArray(dictionaryArray);

			try {
				seriesWriter->Update();
			}
			catch( itk::ExceptionObject & ) {
				return;
			}
		}
		else return;
	}
	else {
		typedef itk::RGBPixel <unsigned char> RGBPixelType;
		typedef itk::Image <RGBPixelType, Dimension> RGBImageType;
		typedef itk::ImageSeriesReader <RGBImageType> RGBSeriesReaderType;

		const RGBSeriesReaderType::FileNamesContainer & filenames =  namesGenerator->GetInputFileNames();

		unsigned int numberOfFilenames =  filenames.size();

		if(numberOfFilenames) {
			RGBSeriesReaderType::Pointer reader = RGBSeriesReaderType::New();

			reader->SetImageIO(gdcmIO);
			reader->SetFileNames(filenames);

			try	{
				reader->Update();
			}
			catch (itk::ExceptionObject &) {
				return;
			}
			RGBImageType::Pointer inputImage = reader->GetOutput();

			typedef DictionaryType*  DictionaryRawPointer;
			typedef std::vector<DictionaryRawPointer> DictionaryArrayType;
			typedef const DictionaryArrayType* DictionaryArrayRawPointer;

			DictionaryType* dictionary;
			DictionaryArrayRawPointer dictionaryArray = reader->GetMetaDataDictionaryArray();
			std::string entryId = "";
			std::string entryvalue = "";

			for(int i=0; i < dictionaryArray->size(); i++)  {
				dictionary = dictionaryArray->at(i);

				if(strcmp(aData->accessionNumber.c_str(), "")) {
					entryId = ACCESSION_NUMBER;
					entryvalue = aData->accessionNumber;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->acquisitionDate.c_str(), "")) {
					entryId = ACQUISITION_DATE;
					entryvalue = aData->acquisitionDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->imageDate.c_str(), "")) {
					entryId = IMAGE_DATE;
					entryvalue = aData->imageDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->imageTime.c_str(), "")) {
					entryId = IMAGE_TIME;
					entryvalue = aData->imageTime;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->institutionName.c_str(), "")) {
					entryId = INSTITUTION_NAME;
					entryvalue = aData->institutionName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientAge.c_str(), "")) {
					entryId = PATIENT_AGE;
					entryvalue = aData->patientAge;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientBirthdate.c_str(), "")) {
					entryId = PATIENT_BIRTHDATE;
					entryvalue = aData->patientBirthdate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientId.c_str(), "")) {
					entryId = PATIENT_ID;
					entryvalue = aData->patientId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientName.c_str(), "")) {
					entryId = PATIENT_NAME;
					entryvalue = aData->patientName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientSex.c_str(), "")) {
					entryId = PATIENT_SEX;
					entryvalue = aData->patientSex;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->patientWeight.c_str(), "")) {
					entryId = PATIENT_WEIGHT;
					entryvalue = aData->patientWeight;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->performingPhysician.c_str(), "")) {
					entryId = PERFORMING_PHYSICIAN;
					entryvalue = aData->performingPhysician;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->physicianOfRecord.c_str(), "")) {
					entryId = PHYSICIAN_OF_RECORD;
					entryvalue = aData->physicianOfRecord;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->referringPhysician.c_str(), "")) {
					entryId = REFERRING_PHYSICIAN;
					entryvalue = aData->referringPhysician;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->seriesDate.c_str(), "")) {
					entryId = SERIES_DATE;
					entryvalue = aData->seriesDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->seriesTime.c_str(), "")) {
					entryId = SERIES_TIME;
					entryvalue = aData->seriesTime;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->studyDate.c_str(), "")) {
					entryId = STUDY_DATE;
					entryvalue = aData->studyDate;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->studyTime.c_str(), "")) {
					entryId = STUDY_TIME;
					entryvalue = aData->studyTime;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialCoordinatingCenterName.c_str(), "")) {
					entryId = TRIAL_COORDINATING_CENTER_NAME;
					entryvalue = aData->trialCoordinatingCenterName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialProtocolId.c_str(), "")) {
					entryId = TRIAL_PROTOCOL_ID;
					entryvalue = aData->trialProtocolId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialProtocolName.c_str(), "")) {
					entryId = TRIAL_PROTOCOL_NAME;
					entryvalue = aData->trialProtocolName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSiteId.c_str(), "")) {
					entryId = TRIAL_SITE_ID;
					entryvalue = aData->trialSiteId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSiteName.c_str(), "")) {
					entryId = TRIAL_SITE_NAME;
					entryvalue = aData->trialSiteName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSponsorName.c_str(), "")) {
					entryId = TRIAL_SPONSOR_NAME;
					entryvalue = aData->trialSponsorName;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSubjectId.c_str(), "")) {
					entryId = TRIAL_SUBJECT_ID;
					entryvalue = aData->trialSubjectId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialSubjectReadingId.c_str(), "")) {
					entryId = TRIAL_SUBJECT_READING_ID;
					entryvalue = aData->trialSubjectReadingId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialTimePointDescription.c_str(), "")) {
					entryId = TRIAL_TIME_POINT_DESCRIPTION;
					entryvalue = aData->trialTimePointDescription;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}
				if(strcmp(aData->trialTimePointId.c_str(), "")) {
					entryId = TRIAL_TIME_POINT_ID;
					entryvalue = aData->trialTimePointId;
					itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
				}

				//entryId = RESCALE_INTERCEPT;
				//entryvalue = "0";
				//itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
			}

			const unsigned int OutputDimension = 2;
			typedef itk::Image <RGBPixelType, OutputDimension> RGBImage2DType;

			typedef itk::ImageSeriesWriter <RGBImageType, RGBImage2DType> RGBSeriesWriterType;

			RGBSeriesWriterType::Pointer seriesWriter = RGBSeriesWriterType::New();
			seriesWriter->SetInput(inputImage);
			seriesWriter->SetImageIO(gdcmIO);

			namesGenerator->SetOutputDirectory(outputDirectory);

			seriesWriter->SetFileNames(namesGenerator->GetOutputFileNames());
			seriesWriter->SetMetaDataDictionaryArray(dictionaryArray);

			try {
				seriesWriter->Update();
			}
			catch( itk::ExceptionObject & ) {
				return;
			}
		}
		else return;
	}
}