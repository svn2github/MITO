/**
 * \file exportFilter.cxx
 * \brief File per il salvataggio delle immagini
 * \author ICAR-CNR Napoli
 */

#include "exportFilter.h"
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesWriter.h>

//#include "djdecode.h"     /* for dcmjpeg decoders */
#include "ximage.h"
#include "dcmtk/dcmimgle/dcmimage.h"     /* for dcmimage JPEG plugin */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */
#include "dcmtk/dcmdata/dcfilefo.h"

/*#include "pacsCommunicator.h"
#include <vtkPNGWriter.h>
#include <vtkDICOMImageReader.h>
#include <vtkJPEGWriter.h>
#include <vtkImageShiftScale.h>
#include <itkJPEGImageIO.h>*/


exportFilter::exportFilter(bool rgb) {
	_rgb = rgb;
}

void exportFilter::exportImageToDicom(itkVtkData *pInputData, string outputFilename) 
{
	if(!pInputData)
		return;

	ImageIOType::Pointer gdcmImageIO = ImageIOType::New();

	if(!_rgb) 
	{
		ImageType::Pointer inputImage = pInputData->getItkImage();
		/*
		DictionaryType & dictionary = inputImage->GetMetaDataDictionary();

		std::string entryId = "0028|1052";
		std::string entryvalue = "0";
		itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		*/

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

		RGBImageType::Pointer inputImage = pInputData->getItkRgbImage();
		/*
		DictionaryType & dictionary = inputImage->GetMetaDataDictionary();

		std::string entryId = "0028|1052";
		std::string entryvalue = "0";
		itk::EncapsulateMetaData<std::string>(dictionary, entryId, entryvalue);
		*/

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

/* // A. Placitelli: Vecchia funzione, riapre un'immagien già aperta
void exportFilter::exportImageToDicom(string inputFilename, string outputFilename) {
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

		std::string entryId = "0028|1052";
		std::string entryvalue = "0";
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

		std::string entryId = "0028|1052";
		std::string entryvalue = "0";
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
}*/

/*
void exportFilter::exportImageToJpg(itkVtkData *pInputData, string outputFilename) 
{
	typedef itk::Image <RGBPixelType, 2 > RGBImageType2D;
	typedef itk::ImageFileReader < RGBImageType2D > RGBReaderType2D;

	RGBReaderType2D::Pointer reader = RGBReaderType2D::New();
	reader->SetFileName(inputFilename.c_str());
	
	try {
		reader->Update();
	}
	catch (itk::ExceptionObject) {
		return;
	}

	RGBImageType2D::Pointer inputImage = reader->GetOutput();

	typedef itk::ImageFileWriter <RGBImageType2D>  WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput(inputImage);
	writer->SetFileName(outputFilename.c_str());

	try {
		writer->Update();
	}
	catch (itk::ExceptionObject &) {
		return;
	}
}
*/

void exportFilter::exportImageToBmp(string inputFilename, string outputFilename) {
	CxImage cxImage;

	DcmFileFormat *dfile = new DcmFileFormat();
	OFCondition cond = dfile->loadFile(inputFilename.c_str(), EXS_Unknown, EGL_withoutGL,DCM_MaxReadLength);
	
	if (cond.bad()) {
		return;
    }
	
	E_TransferSyntax xfer = dfile->getDataset()->getOriginalXfer();
    DicomImage *di = new DicomImage(dfile, xfer, CIF_AcrNemaCompatibility ,	0, 1);
	
	if (di->getStatus() != EIS_Normal)
        return;

	di->setMinMaxWindow();

	di->writeBMP(outputFilename.c_str(),24);
	
/*	if(cxImage.Load("c:\\from_dicom.bmp",CXIMAGE_FORMAT_BMP))

		cxImage.Save(outputFilename.c_str(),CXIMAGE_FORMAT_JPG);*/
}

void exportFilter::exportSeriesToDicom(string inputDirectory, string outputDirectory) {
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
	
			typedef short OutputPixelType;
			const unsigned int OutputDimension = 2;

			typedef itk::Image <OutputPixelType, OutputDimension> Image2DType;

			typedef itk::ImageSeriesWriter <ImageType, Image2DType> SeriesWriterType;

			SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
			seriesWriter->SetInput(inputImage);
			seriesWriter->SetImageIO(gdcmIO);

			namesGenerator->SetOutputDirectory(outputDirectory);

			seriesWriter->SetFileNames(namesGenerator->GetOutputFileNames());

			/*
			typedef DictionaryType*  DictionaryRawPointer;
			typedef std::vector<DictionaryRawPointer> DictionaryArrayType;
			typedef const DictionaryArrayType* DictionaryArrayRawPointer;

			DictionaryType* dictionary;
			DictionaryArrayRawPointer dictionaryArray = reader->GetMetaDataDictionaryArray();
			std::string entryId = "0028|1052";
			std::string entryvalue = "0";

			for(int i=0; i < dictionaryArray->size(); i++)  {
				dictionary = dictionaryArray->at(i);
				itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
			}

			seriesWriter->SetMetaDataDictionaryArray(dictionaryArray);
			*/

			try {
				seriesWriter->Update();
			}
			catch(itk::ExceptionObject &) {
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

			const unsigned int OutputDimension = 2;
			typedef itk::Image <RGBPixelType, OutputDimension> RGBImage2DType;

			typedef itk::ImageSeriesWriter <RGBImageType, RGBImage2DType> RGBSeriesWriterType;

			RGBSeriesWriterType::Pointer seriesWriter = RGBSeriesWriterType::New();
			seriesWriter->SetInput(inputImage);
			seriesWriter->SetImageIO(gdcmIO);

			namesGenerator->SetOutputDirectory(outputDirectory);

			seriesWriter->SetFileNames(namesGenerator->GetOutputFileNames());
			
			/*
			typedef DictionaryType*  DictionaryRawPointer;
			typedef std::vector<DictionaryRawPointer> DictionaryArrayType;
			typedef const DictionaryArrayType* DictionaryArrayRawPointer;

			DictionaryType* dictionary;
			DictionaryArrayRawPointer dictionaryArray = reader->GetMetaDataDictionaryArray();
			std::string entryId = "0028|1052";
			std::string entryvalue = "0";

			for(int i=0; i < dictionaryArray->size(); i++)  {
				dictionary = dictionaryArray->at(i);
				itk::EncapsulateMetaData<std::string>((*dictionary), entryId, entryvalue);
			}

			seriesWriter->SetMetaDataDictionaryArray(dictionaryArray);
			*/

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


void exportFilter::exportSeriesToBmp(string inputDirectory, string outputDirectory) {
	NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
	namesGenerator->SetInputDirectory(inputDirectory);
	typedef itk::ImageSeriesReader<ImageType> SeriesReaderType;
	const SeriesReaderType::FileNamesContainer & filenames =  namesGenerator->GetInputFileNames();
	std::vector<std::string> inputFilenames = namesGenerator->GetInputFileNames();

	namesGenerator->SetOutputDirectory(outputDirectory);
	std::vector<std::string> outputFilenamesTemp = namesGenerator->GetOutputFileNames();
	string outputFilename;
	int i = 0;
	while(outputFilenamesTemp.size()) {
		outputFilename = outputFilenamesTemp.back();
		outputFilenamesTemp.pop_back();

		CxImage cxImage;
		DcmFileFormat *dfile = new DcmFileFormat();
		OFCondition cond = dfile->loadFile(inputFilenames.back().c_str(), EXS_Unknown, EGL_withoutGL,DCM_MaxReadLength);
		inputFilenames.pop_back();
		if (cond.bad()) {
			return;
		}

		E_TransferSyntax xfer = dfile->getDataset()->getOriginalXfer();
		DicomImage *di = new DicomImage(dfile, xfer, CIF_AcrNemaCompatibility ,	0, 1);

		if (di->getStatus() != EIS_Normal)
			return;

		outputFilename.replace(outputFilename.size()-3,outputFilename.size()-1,"bmp");
		di->setMinMaxWindow();
		di->writeBMP(outputFilename.c_str(),24);
		i++;
	}
}


/*
void exportFilter::exportSeriesToJpg(string inputDirectory, string outputDirectory) {
	NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
	namesGenerator->SetInputDirectory(inputDirectory);

	typedef itk::ImageSeriesReader<RGBImageType> SeriesReaderType;
	const SeriesReaderType::FileNamesContainer & filenames =  namesGenerator->GetInputFileNames();

	unsigned int numberOfFilenames = filenames.size();

	if(numberOfFilenames) {
		SeriesReaderType::Pointer reader = SeriesReaderType::New();

		reader->SetFileNames(filenames);

		try	{
			reader->Update();
		}
		catch (itk::ExceptionObject &) {
			return;
		}
		RGBImageType::Pointer inputImage = reader->GetOutput();

		typedef itk::Image < RGBPixelType, 2 > RGBImage2DType;
		typedef itk::ImageSeriesWriter <RGBImageType, RGBImage2DType> SeriesWriterType;

		SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
		seriesWriter->SetInput(inputImage);

		namesGenerator->SetOutputDirectory(outputDirectory);

		std::vector<std::string> outputFilenamesTemp = namesGenerator->GetOutputFileNames();
		std::vector<std::string> outputFilenames;
		string temp;

		while(outputFilenamesTemp.size()) {
			temp = outputFilenamesTemp.back();
			outputFilenamesTemp.pop_back();
			temp.replace(temp.size()-3,temp.size()-1,"jpg");
			outputFilenames.push_back(temp);
		}
		seriesWriter->SetFileNames(outputFilenames);

		try {
			seriesWriter->Update();
		}
		catch( itk::ExceptionObject & ) {
			return;
		}
	}
	else return;
}
*/