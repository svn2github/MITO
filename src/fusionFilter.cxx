/**
 * \file fusionFilter.cxx
 * \brief File per la gestione delle tecniche di fusione
 * \author ICAR-CNR Napoli
 */

#include "fusionFilter.h"
#include "itkFusion.h"
#include "itkFusion2.h"
#include "itkFusion3.h"
#include "itkWavelet.h"
#include <itkRescaleIntensityImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkComposeRGBImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkUnaryFunctorImageFilter.h>
#include <itkVotingBinaryHoleFillingImageFilter.h>
#include <itkBinaryMedianImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkRGBPixel.h>

fusionFilter::fusionFilter(string path1, string path2, unsigned int idData0, dataHandler *dataHandler) {
	itkVtkData data1, data2;
	int rows1, rows2;

	_itkDicomIO = ImageIOType::New();
	ImageIOType::Pointer itkDicomIO2 = ImageIOType::New();


	wxDir wxDirectory(path1.c_str());
	if(wxDirectory.Exists(path1.c_str())) {
		SeriesReaderType::Pointer itkReaderSeries1 = SeriesReaderType::New();
		itkReaderSeries1->SetImageIO(_itkDicomIO);

		wxString fileInDir;
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
		itkReaderSeries1->SetFileNames( fileNames );
		itkReaderSeries1->SetImageIO(_itkDicomIO);
		itkReaderSeries1->UpdateLargestPossibleRegion();
		_itkImage1 = ImageType::New();
		_itkImage1 = itkReaderSeries1->GetOutput();
	}
	else {
		ReaderType::Pointer itkReader1 = ReaderType::New();
		itkReader1->SetFileName(path1.c_str());
		itkReader1->SetImageIO(_itkDicomIO);
		itkReader1->UpdateLargestPossibleRegion();
		_itkImage1 = ImageType::New();
		_itkImage1 = itkReader1->GetOutput();
	}

	data1.setItkImage(_itkImage1);
	data1.setRgb(false);
	data1.readDicomEntries(_itkDicomIO);
	rows1 = atoi(data1.getHeaderEntry(13).c_str());
	

	wxDir wxDirectory2(path2.c_str());
	if(wxDirectory2.Exists(path2.c_str())) {
		SeriesReaderType::Pointer itkReaderSeries2 = SeriesReaderType::New();
		itkReaderSeries2->SetImageIO(itkDicomIO2);

		wxString fileInDir2;
		NamesGeneratorType::Pointer namesGenerator2 = NamesGeneratorType::New();
		namesGenerator2->SetUseSeriesDetails( true );
		namesGenerator2->SetDirectory( (wxDirectory2.GetName()).c_str() );

		typedef std::vector < std::string > SeriesIdContainer;
		const SeriesIdContainer & seriesUID2 = namesGenerator2->GetSeriesUIDs();
		SeriesIdContainer::const_iterator seriesItr2 = seriesUID2.begin();
		SeriesIdContainer::const_iterator seriesEnd2 = seriesUID2.end();
		while( seriesItr2 != seriesEnd2 ) {
			seriesItr2++;
		}
		std::string seriesIdentifier2;
		seriesIdentifier2 = seriesUID2.begin()->c_str();
		typedef std::vector< std::string > FileNamesContainer;
		FileNamesContainer fileNames2;
		fileNames2 = namesGenerator2->GetFileNames( seriesIdentifier2 );
		itkReaderSeries2->SetFileNames( fileNames2 );
		itkReaderSeries2->SetImageIO(itkDicomIO2);
		itkReaderSeries2->UpdateLargestPossibleRegion();
		_itkImage2 = ImageType::New();
		_itkImage2 = itkReaderSeries2->GetOutput();
	}
	else {
		ReaderType::Pointer itkReader2 = ReaderType::New();
		itkReader2->SetFileName(path2.c_str());
		itkReader2->SetImageIO(itkDicomIO2);
		itkReader2->UpdateLargestPossibleRegion();
		_itkImage2 = ImageType::New();
		_itkImage2 = itkReader2->GetOutput();
	}

	data2.setItkImage(_itkImage2);
	data2.setRgb(false);
	data2.readDicomEntries(itkDicomIO2);
	_wl2 = data2.getWl();
	_ww2 = data2.getWw();
	rows2 = atoi(data2.getHeaderEntry(13).c_str());

	if(rows1 < rows2)
		_itkDicomIO = itkDicomIO2;
	_itkVtkData0 = (itkVtkData*) dataHandler->getData(idData0);
}


void fusionFilter::computeFusion(InputRealType param1, InputRealType param2, InputRealType param3, fusionType fusionType, bool applyClut, LivelloDec level,ruleType ruleType,SizeWindow sizewin,WaveletType waveletType) {
	_applyClut = applyClut;
	_fusionType = fusionType;
	_levelDec = level + 1;
	_fusionRule = ruleType;
	_sizeWindow = sizewin;
	_waveletType = waveletType;

	switch (_fusionType) {
		case media_pesata:
			MediaPesata(param1, param2);
			break;
		case wavelet:
			switch(_waveletType) {
				case 0:
					HaarWavelet(param1, param2);
					break;
				default:
					DaubNWavelet(param1, param2);
					break;
			}
			break;
	}
}


void fusionFilter::MediaPesata(InputRealType pesoA, InputRealType pesoB) {
	if (_applyClut) {
		MediaPesataClut(pesoA, pesoB);
		return;
	}
	if (_itkImage1) {
		typedef itk::ResampleImageFilter < ImageType, ImageType > ResampleType;
		typedef itk::FuseImageFilter < ImageType > FusionFilter;

		ImageType::Pointer im1 = _itkImage1;
		ImageType::Pointer im2 = _itkImage2;
		ImageType::SizeType size1 = im1->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType size2 = im2->GetLargestPossibleRegion().GetSize();

		if (size1[2] == 1 && size2[2] == 1) {
			PreStadio2D(im1, im2);
		}
		//Controllo sulla dimensione per il resample
		
		ResampleType::Pointer resample = ResampleType::New();
		FusionFilter::Pointer fusion = FusionFilter::New();
		if (size1[0]<size2[0]) {
			resample->SetInput( im1 );
			resample->SetSize( im2->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im2->GetOrigin() );
			resample->SetOutputSpacing( im2->GetSpacing() );
			resample->Update();
			
		
			fusion->SetInput1(resample->GetOutput());
			fusion->SetInput2(im2);
			fusion->GetFunctor().SetPesoA( pesoA );
			fusion->GetFunctor().SetPesoB( pesoB );
			fusion->Update();
		}
		else {
			resample->SetInput( im2 );
			resample->SetSize( im1->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im1->GetOrigin() );
			resample->SetOutputSpacing( im1->GetSpacing() );
			resample->Update();

			
			fusion->SetInput1(im1);
			fusion->SetInput2(resample->GetOutput());
			fusion->GetFunctor().SetPesoA( pesoA );
			fusion->GetFunctor().SetPesoB( pesoB );
			fusion->Update();
		}
		update(fusion->GetOutput());
	
	
	}
}


void fusionFilter::HaarWavelet(InputRealType pesoA, InputRealType pesoB) {
	if (_applyClut) {
		HaarWaveletClut(pesoA, pesoB);
		return;
	}
	if (_itkImage1) {
		int sizeW = _sizeWindow, waveletDepth = _levelDec;

		typedef itk::RescaleIntensityImageFilter < ImageType, ImageType > RescaleType;
		typedef itk::ResampleImageFilter < ImageType, ImageType > ResampleType;
		typedef itk::WaveletFusionImageFilter < ImageType > FusionFilter;
		typedef itk::HaarWaveletTransformImageFilter < ImageType > FfWaveletType;
		typedef itk::HaarBackwardWaveletTransformImageFilter < ImageType > BwWaveletType;
		//typedef itk::SigmoidImageFilter<ImageType,ImageType> SigmoidFilterType;	


		ImageType::Pointer im1 = _itkImage1;
		ImageType::Pointer im2 = _itkImage2;
		ImageType::SizeType size1 = im1->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType size2 = im2->GetLargestPossibleRegion().GetSize();

		if (size1[2] == 1 && size2[2] == 1) {
			PreStadio2D(im1, im2);
		}
		
		/*SigmoidFilterType::Pointer rescale1 = SigmoidFilterType::New();	
		rescale1->SetOutputMinimum( 0 );
		rescale1->SetOutputMaximum( 500 );
		rescale1->SetAlpha( 100 );
		rescale1->SetBeta( 130 );
		rescale1->SetInput( im1 );
		rescale1->Update();
		
		SigmoidFilterType::Pointer rescale2 = SigmoidFilterType::New();
		rescale2->SetOutputMinimum( 0 );
		rescale2->SetOutputMaximum( 500 );
		rescale2->SetAlpha( 50 );
		rescale2->SetBeta( 130 );
		rescale2->SetInput( im2 );
		rescale2->Update();*/
		
		ResampleType::Pointer resample = ResampleType::New();
		FfWaveletType::Pointer ff1 = FfWaveletType::New();
		FfWaveletType::Pointer ff2 = FfWaveletType::New();

		if (size1[0]<size2[0]) {
			resample->SetInput( im1 );
			resample->SetSize( im2->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im2->GetOrigin() );
			resample->SetOutputSpacing( im2->GetSpacing() );
			resample->Update();

			ff1->SetInput( resample->GetOutput() );
			ff2->SetInput( im2 );
		}
		else {
			resample->SetInput( im2 );
			resample->SetSize( im1->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im1->GetOrigin() );
			resample->SetOutputSpacing( im1->GetSpacing() );
			resample->Update();

			ff1->SetInput( im1 );
			ff2->SetInput( resample->GetOutput() );
		}

	
		ff1->SetWaveletDepth(waveletDepth);
		ff1->Update();

		ff2->SetWaveletDepth(waveletDepth);
		ff2->Update();
        
		FusionFilter::Pointer fusion = FusionFilter::New();
		fusion->SetInput(ff1->GetOutput());
		fusion->SetInput2(ff2->GetOutput());
		fusion->SetPesoA(pesoA);
		fusion->SetPesoB(pesoB);
		fusion->SetWaveletDepth(waveletDepth);
		fusion->SetSizeW(sizeW);

		switch (_fusionRule) {
			case Sostituzione_dettaglio:
				fusion->SetFusionRule(FusionFilter::rule1);
				break;
			case Modulo_max:
				fusion->SetFusionRule(FusionFilter::rule2);
				break;
			case Consistenza_media:
				fusion->SetFusionRule(FusionFilter::rule3);
				break;
			case Consistenza_var:
				fusion->SetFusionRule(FusionFilter::rule4);
				break;
		}
		fusion->Update();
		
		BwWaveletType::Pointer bw = BwWaveletType::New();

		if (_fusionRule == Consistenza_media ||_fusionRule == Consistenza_var) {
		// verifica della consistenza: sfruttiamo un filtro a votazione di maggioranza per ottenere una mappa di decisione
	
			typedef itk::BinaryMedianImageFilter<ImageType, ImageType > FilterType;
			FilterType::Pointer filter = FilterType::New();
		
			ImageType::SizeType indexRadius;
			indexRadius[0] = sizeW; // radius along x
			indexRadius[1] = sizeW; // radius along y
			indexRadius[2] = 0; // radius along y
			filter->SetRadius( indexRadius );
			filter->SetBackgroundValue( 0 );
			filter->SetForegroundValue( 255 );
			filter->SetInput( fusion->GetOutput() );
			filter->Update();
	
			typedef itk::WaveletFusionConsImageFilter < ImageType > ConsFusionFilter;
			ConsFusionFilter::Pointer consfusion = ConsFusionFilter::New();
			consfusion->SetInput(ff1->GetOutput());
			consfusion->SetInput2(ff2->GetOutput());
			consfusion->SetInput3(filter->GetOutput());
			consfusion->SetPesoA(pesoA);
			consfusion->SetPesoB(pesoB);
			consfusion->SetWaveletDepth(waveletDepth);
			consfusion->SetSizeW(sizeW);

			switch (_fusionRule) {
				case Consistenza_media:
					consfusion->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consfusion->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consfusion->Update();

			bw->SetInput(consfusion->GetOutput());
			bw->SetWaveletDepth(waveletDepth);
			bw->Update();
			
			update(bw->GetOutput());
		}
		else {
			bw->SetInput(fusion->GetOutput());
			bw->SetWaveletDepth(waveletDepth);
			bw->Update();

			update(bw->GetOutput());

		}

	}	
}


void  fusionFilter::MediaPesataClut(InputRealType pesoA, InputRealType pesoB) {
	if (_itkImage1) {
		typedef itk::ResampleImageFilter < ImageType, ImageType > ResampleType;
		typedef itk::RescaleIntensityImageFilter < ImageType, ImageType > RescaleType;
		typedef itk::CLUTFuseImageFilter < ImageType, RGBImageType > FusionFilter;
		//typedef itk::SigmoidImageFilter<ImageType,ImageType> SigmoidFilterType;	


		ImageType::Pointer im1 = _itkImage1;
		ImageType::Pointer im2 = _itkImage2;
		ImageType::SizeType size1 = im1->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType size2 = im2->GetLargestPossibleRegion().GetSize();

		if (size1[2] == 1 && size2[2] == 1) {
			PreStadio2D(im1, im2);
		}

		RescaleType::Pointer rescale2 = RescaleType::New();
		rescale2->SetInput( im2 );
		rescale2->SetOutputMinimum( 0 );
		rescale2->SetOutputMaximum( 255 );

		/*SigmoidFilterType::Pointer rescale1 = SigmoidFilterType::New();		
		rescale1->SetOutputMinimum( 0 );
		rescale1->SetOutputMaximum( 255 );
		rescale1->SetAlpha( 50 );
		rescale1->SetBeta( 130 );
		rescale1->SetInput( im1 );*/

		RescaleType::Pointer rescale1 = RescaleType::New();
		rescale1->SetOutputMinimum( 0 );
		rescale1->SetOutputMaximum( 255 );
		rescale1->SetInput( im1 );

		ResampleType::Pointer resample = ResampleType::New();
		FusionFilter::Pointer fusion = FusionFilter::New();

		if (size1[0]<size2[0]) {
			resample->SetInput( rescale1->GetOutput() );
			//resample->SetInput( im1 );
			resample->SetSize( im2->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im2->GetOrigin() );
			resample->SetOutputSpacing( im2->GetSpacing() );

			fusion->SetInput1(resample->GetOutput());
			fusion->SetInput2(rescale2->GetOutput());
		}
		else {
			resample->SetInput( rescale2->GetOutput() );
			resample->SetSize( im1->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im1->GetOrigin() );
			resample->SetOutputSpacing( im1->GetSpacing() );

			fusion->SetInput1(rescale1->GetOutput());
			//fusion->SetInput1(im1);
			fusion->SetInput2(resample->GetOutput());
		}
		
		fusion->GetFunctor().SetPesoA( pesoA );
		fusion->GetFunctor().SetPesoB( pesoB );
		fusion->Update();
		update(fusion->GetOutput());
	}
}


void fusionFilter::HaarWaveletClut(InputRealType pesoA, InputRealType pesoB) {
	if (_itkImage1) {
		typedef itk::Image <unsigned char, Dimension> ScalarType;
		typedef itk::ResampleImageFilter <ImageType, ImageType> ResampleType;
		typedef itk::RescaleIntensityImageFilter <ImageType, ImageType> RescaleType;
		typedef itk::RescaleIntensityImageFilter <ImageType, ScalarType> RescaleToScalarType;
		typedef itk::RescaleIntensityImageFilter <ScalarType, ImageType> RescaleToImageType;
		typedef itk::Functor::ApplyClut <unsigned char> ApplyClutType;
		typedef itk::UnaryFunctorImageFilter <ScalarType, ScalarType, ApplyClutType> ToClut;
		typedef itk::HaarWaveletTransformImageFilter < ImageType > FWavelet;
		typedef itk::HaarBackwardWaveletTransformImageFilter < ImageType > BWavelet;
		typedef itk::WaveletFusionConsImageFilter <ImageType> ConsFusionFilter;
		typedef itk::WaveletFusionImageFilter <ImageType> WFusion;
		typedef itk::BinaryMedianImageFilter <ImageType, ImageType> BinaryFilterType;
		typedef itk::ComposeRGBImageFilter <ScalarType> ToRGB;

		int sizeW = _sizeWindow+2, waveletDepth = _levelDec;
		int	N = 8;

		//Livello di decomposizione della wavelet
		switch(_waveletType) {
			case 1:
				N = 4;
				break;
			case 2:
				N = 8;
				break;
			case 3:
				N = 20;
				break;
		}

		ImageType::SizeType size1 = _itkImage1->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType size2 = _itkImage2->GetLargestPossibleRegion().GetSize();

		if (size1[2] == 1 && size2[2] == 1) {
			PreStadio2D(_itkImage1, _itkImage2);
		}		

		//Interpolazione delle immagini di una serie rispetto all'altra
		ResampleType::Pointer resample = ResampleType::New();
		
		//Riscalamento dell'intensità dei pixel della seconda serie
		RescaleType::Pointer rescale2 = RescaleType::New();
		
		//Riscalamento dell'intensità dei pixel della seconda serie per la CLUT
		RescaleToScalarType::Pointer rescale2Clut = RescaleToScalarType::New();

		if (size1[0] < size2[0]) {
			//Interpolazione
			resample->SetInput(_itkImage1);
			resample->SetSize(_itkImage2->GetLargestPossibleRegion().GetSize());
			resample->SetOutputOrigin(_itkImage2->GetOrigin());
			resample->SetOutputSpacing(_itkImage2->GetSpacing());
			resample->Update();
            			
			//Riscalamento
			rescale2->SetInput(_itkImage2);
			rescale2Clut->SetInput(_itkImage2);
		}
		else {
			//Interpolazione
			resample->SetInput(_itkImage2);
			resample->SetSize(_itkImage1->GetLargestPossibleRegion().GetSize());
			resample->SetOutputOrigin(_itkImage1->GetOrigin());
			resample->SetOutputSpacing(_itkImage1->GetSpacing());
			resample->Update();
			
			//Riscalamento
			rescale2->SetInput(resample->GetOutput());
			rescale2Clut->SetInput(resample->GetOutput());
		}
		
		//Riscalamento
		rescale2->SetOutputMinimum(0);
		rescale2->SetOutputMaximum(500);
		rescale2->Update();

		rescale2Clut->SetOutputMinimum(0);
		rescale2Clut->SetOutputMaximum(500);
		rescale2Clut->Update();

		//Applicazione della CLUT alla seconda serie
		ToClut::Pointer toR2 = ToClut::New();
		toR2->GetFunctor().SetChannel(1);
		toR2->SetInput(rescale2Clut->GetOutput());
		toR2->Update();

		ToClut::Pointer toG2 = ToClut::New();
		toG2->GetFunctor().SetChannel(2);
		toG2->SetInput(rescale2Clut->GetOutput());
		toG2->Update();

		ToClut::Pointer toB2 = ToClut::New();
		toB2->GetFunctor().SetChannel(3);
		toB2->SetInput(rescale2Clut->GetOutput());
		toB2->Update();

		//Riscalamento dei canali della CLUT
		RescaleToImageType::Pointer rescale2R = RescaleToImageType::New();
		rescale2R->SetInput(toR2->GetOutput());
		rescale2R->SetOutputMinimum(0);
		rescale2R->SetOutputMaximum(500);
		rescale2R->Update();

		RescaleToImageType::Pointer rescale2G = RescaleToImageType::New();
		rescale2G->SetInput(toG2->GetOutput());
		rescale2G->SetOutputMinimum(0);
		rescale2G->SetOutputMaximum(500);
		rescale2G->Update();

		RescaleToImageType::Pointer rescale2B = RescaleToImageType::New();
		rescale2B->SetInput(toB2->GetOutput());
		rescale2B->SetOutputMinimum(0);
		rescale2B->SetOutputMaximum(500);
		rescale2B->Update();

		//Forward Wavelet della prima serie
		FWavelet::Pointer w1 = FWavelet::New();
		w1->SetInput(_itkImage1);
		w1->SetWaveletDepth(waveletDepth);
		w1->Update();

		//Forward Wavelet della seconda serie
		FWavelet::Pointer w2 = FWavelet::New();
		w2->SetInput(rescale2->GetOutput());
		w2->SetWaveletDepth(waveletDepth);
		w2->Update();

		//Forward Wavelet della seconda serie dopo aver applicato la CLUT
		FWavelet::Pointer w2R = FWavelet::New();
		w2R->SetInput(rescale2R->GetOutput());
		w2R->SetWaveletDepth(waveletDepth);
		w2R->Update();

		FWavelet::Pointer w2G = FWavelet::New();
		w2G->SetInput(rescale2G->GetOutput());
		w2G->SetWaveletDepth(waveletDepth);
		w2G->Update();

		FWavelet::Pointer w2B = FWavelet::New();
		w2B->SetInput(rescale2B->GetOutput());
		w2B->SetWaveletDepth(waveletDepth);
		w2B->Update();

		//Fusione delle due decomposizioni Wavelet
		WFusion::Pointer fusion = WFusion::New();
		fusion->SetInput(w1->GetOutput());
		fusion->SetInput2(w2->GetOutput());
		fusion->SetWaveletDepth(waveletDepth);
		fusion->SetSizeW(sizeW);
		fusion->SetPesoA(pesoA);
		fusion->SetPesoB(pesoB);

		switch (_fusionRule) {
			case Sostituzione_dettaglio:
				fusion->SetFusionRule(WFusion::rule1);
				break;
			case Modulo_max:
				fusion->SetFusionRule(WFusion::rule2);
				break;
			case Consistenza_media:
				fusion->SetFusionRule(WFusion::rule3);
				break;
			case Consistenza_var:
				fusion->SetFusionRule(WFusion::rule4);
				break;
		}
		fusion->Update();

		//Backward Wavelet dei tre canali R, G e B
		BWavelet::Pointer bWR = BWavelet::New();
		BWavelet::Pointer bWG = BWavelet::New();
		BWavelet::Pointer bWB = BWavelet::New();

		if(_fusionRule == Consistenza_media || _fusionRule == Consistenza_var) {
			//Filtro di maggioranza
			BinaryFilterType::Pointer binaryFilter = BinaryFilterType::New();

			ImageType::SizeType indexRadius;
			indexRadius[0] = sizeW;		//radius along x
			indexRadius[1] = sizeW;		//radius along y
			indexRadius[2] = 0;			//radius along z
			
			//Applicazione della verifica di consistenza
			binaryFilter->SetRadius(indexRadius);
			binaryFilter->SetBackgroundValue(0);
			binaryFilter->SetForegroundValue(255);	
			binaryFilter->SetInput(fusion->GetOutput());
			binaryFilter->Update();

			//Fusione con verifica della consistenza
			ConsFusionFilter::Pointer consFusionR = ConsFusionFilter::New();
			ConsFusionFilter::Pointer consFusionG = ConsFusionFilter::New();
			ConsFusionFilter::Pointer consFusionB = ConsFusionFilter::New();

			//Fusione del canale rosso
			consFusionR->SetInput(w1->GetOutput());
			consFusionR->SetInput2(w2R->GetOutput());
			consFusionR->SetInput3(binaryFilter->GetOutput());
			consFusionR->SetPesoA(pesoA);
			consFusionR->SetPesoB(pesoB);
			consFusionR->SetWaveletDepth(waveletDepth);
			consFusionR->SetSizeW(sizeW);
			switch(_fusionRule) {
				case Consistenza_media:
					consFusionR->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consFusionR->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consFusionR->Update();

			//Fusione del canale verde
			consFusionG->SetInput(w1->GetOutput());
			consFusionG->SetInput2(w2G->GetOutput());
			consFusionG->SetInput3(binaryFilter->GetOutput());
			consFusionG->SetPesoA(pesoA);
			consFusionG->SetPesoB(pesoB);
			consFusionG->SetWaveletDepth(waveletDepth);
			consFusionG->SetSizeW(sizeW);
			switch (_fusionRule) {
				case Consistenza_media:
					consFusionG->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consFusionG->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consFusionG->Update();

			//Fusione del canale blu
			consFusionB->SetInput(w1->GetOutput());
			consFusionB->SetInput2(w2B->GetOutput());
			consFusionB->SetInput3(binaryFilter->GetOutput());
			consFusionB->SetPesoA(pesoA);
			consFusionB->SetPesoB(pesoB);
			consFusionB->SetWaveletDepth(waveletDepth);
			consFusionB->SetSizeW(sizeW);
			switch (_fusionRule) {
				case Consistenza_media:
					consFusionB->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consFusionB->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consFusionB->Update();

			//Antitrasformata Wavelet dei 3 canali della serie fusa
			bWR->SetInput(consFusionR->GetOutput());
			bWR->SetWaveletDepth(waveletDepth);
			bWR->Update();

			bWG->SetInput(consFusionG->GetOutput());
			bWG->SetWaveletDepth(waveletDepth);
			bWG->Update();

			bWB->SetInput(consFusionB->GetOutput());
			bWB->SetWaveletDepth(waveletDepth);
			bWB->Update();
		}
		else {		
			//Fusione del canale rosso
			WFusion::Pointer fusionR = WFusion::New();
			fusionR->SetInput(w1->GetOutput());
			fusionR->SetInput2(w2R->GetOutput());
			fusionR->SetWaveletDepth(waveletDepth);
			fusionR->SetSizeW(sizeW);
			fusionR->SetPesoA(pesoA);
			fusionR->SetPesoB(pesoB);
			switch (_fusionRule) {
				case Sostituzione_dettaglio:
					fusionR->SetFusionRule(WFusion::rule1);
					break;
				case Modulo_max:
					fusionR->SetFusionRule(WFusion::rule2);
					break;
				case Consistenza_media:
					fusionR->SetFusionRule(WFusion::rule3);
					break;
				case Consistenza_var:
					fusionR->SetFusionRule(WFusion::rule4);
					break;
			}
			fusionR->Update();

			//Fusione del canale verde
			WFusion::Pointer fusionG = WFusion::New();
			fusionG->SetInput(w1->GetOutput());
			fusionG->SetInput2(w2G->GetOutput());
			fusionG->SetWaveletDepth(waveletDepth);
			fusionG->SetSizeW(sizeW);
			fusionG->SetPesoA(pesoA);
			fusionG->SetPesoB(pesoB);
			switch (_fusionRule) {
				case Sostituzione_dettaglio:
					fusionG->SetFusionRule(WFusion::rule1);
					break;
				case Modulo_max:
					fusionG->SetFusionRule(WFusion::rule2);
					break;
				case Consistenza_media:
					fusionG->SetFusionRule(WFusion::rule3);
					break;
				case Consistenza_var:
					fusionG->SetFusionRule(WFusion::rule4);
					break;
			}
			fusionG->Update();

			//Fusione del canale blu
			WFusion::Pointer fusionB = WFusion::New();
			fusionB->SetInput(w1->GetOutput());
			fusionB->SetInput2(w2B->GetOutput());
			fusionB->SetWaveletDepth(waveletDepth);
			fusionB->SetSizeW(sizeW);
			fusionB->SetPesoA(pesoA);
			fusionB->SetPesoB(pesoB);
			switch (_fusionRule) {
				case Sostituzione_dettaglio:
					fusionB->SetFusionRule(WFusion::rule1);
					break;
				case Modulo_max:
					fusionB->SetFusionRule(WFusion::rule2);
					break;
				case Consistenza_media:
					fusionB->SetFusionRule(WFusion::rule3);
					break;
				case Consistenza_var:
					fusionB->SetFusionRule(WFusion::rule4);
					break;

			}
			fusionB->Update();
			
			//Antitrasformata Wavelet dei 3 canali della serie fusa
			bWR->SetInput(fusionR->GetOutput());
			bWR->SetWaveletDepth(waveletDepth);
			bWR->Update();

			bWG->SetInput(fusionG->GetOutput());
			bWG->SetWaveletDepth(waveletDepth);
			bWG->Update();

			bWB->SetInput(fusionB->GetOutput());
			bWB->SetWaveletDepth(waveletDepth);
			bWB->Update();
		}
		
		//Riscalamento dei 3 canali della serie fusa
		RescaleToScalarType::Pointer rescaleToR = RescaleToScalarType::New();
		rescaleToR->SetInput(bWR->GetOutput());
		rescaleToR->SetOutputMinimum(0);
		rescaleToR->SetOutputMaximum(500);
		rescaleToR->Update();

		RescaleToScalarType::Pointer rescaleToG = RescaleToScalarType::New();
		rescaleToG->SetInput(bWG->GetOutput());
		rescaleToG->SetOutputMinimum(0);
		rescaleToG->SetOutputMaximum(500);
		rescaleToG->Update();

		RescaleToScalarType::Pointer rescaleToB = RescaleToScalarType::New();
		rescaleToB->SetInput(bWB->GetOutput());
		rescaleToB->SetOutputMinimum(0);
		rescaleToB->SetOutputMaximum(500);
		rescaleToB->Update();

		//Composizione della serie fusa in formato RGB
		ToRGB::Pointer toRgb = ToRGB::New();
		toRgb->SetInput1(rescaleToR->GetOutput());
		toRgb->SetInput2(rescaleToG->GetOutput());
		toRgb->SetInput3(rescaleToB->GetOutput());
		toRgb->Update();

		//Visualizzazione della serie fusa
		update(toRgb->GetOutput());
	}

/*	if (_itkImage1) {
		int sizeW = _sizeWindow, waveletDepth = _levelDec;
		typedef itk::Image < unsigned char, Dimension > ScalarType;
		typedef itk::ResampleImageFilter < ImageType, ImageType > ResampleType;
		typedef itk::RescaleIntensityImageFilter < ImageType, ImageType > Rescale;
		typedef itk::RescaleIntensityImageFilter < ImageType, ScalarType > RescaleToImageType;
		typedef itk::Functor::ApplyClut < unsigned char > ApplyClutType;
		typedef itk::UnaryFunctorImageFilter < ScalarType, ScalarType, ApplyClutType > ToClut;
		typedef itk::HaarWaveletTransformImageFilter < ImageType > FWavelet;
		typedef itk::WaveletFusionImageFilter < ImageType > WFusion;
		typedef itk::HaarBackwardWaveletTransformImageFilter < ImageType > BWavelet;
		typedef itk::ComposeRGBImageFilter < ScalarType > ToRGB;
		typedef itk::SigmoidImageFilter<ImageType,ImageType> SigmoidFilterType;
		typedef itk::SigmoidImageFilter<ImageType, ScalarType > CastToImageType;
		typedef itk::SigmoidImageFilter<ScalarType, ImageType> CastToScalarType;
		
		ImageType::Pointer im1 = _itkImage1;
		ImageType::Pointer im2 = _itkImage2;
		ImageType::SizeType size1 = im1->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType size2 = im2->GetLargestPossibleRegion().GetSize();

		if (size1[2] == 1 && size2[2] == 1) {
			PreStadio2D(im1, im2);
		}

		ResampleType::Pointer resample = ResampleType::New();
		RescaleToImageType::Pointer rescale = RescaleToImageType::New();
		//SigmoidFilterType::Pointer rescale1 = SigmoidFilterType::New();


		if (size1[0] < size2[0] ) {
			resample->SetInput(im1);
			resample->SetSize( im2->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im2->GetOrigin() );
			resample->SetOutputSpacing( im2->GetSpacing() );
			resample->Update();
            			
			rescale->SetInput(im2);
			rescale1->SetInput( resample ->GetOutput());
		}
		else {
			resample->SetInput(im2);
			resample->SetSize( im1->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im1->GetOrigin() );
			resample->SetOutputSpacing( im1->GetSpacing() );
			resample->Update();
			
			rescale->SetInput(resample->GetOutput());
			rescale1->SetInput(im1);
		}
		
		rescale->SetOutputMinimum( 0 );
		rescale->SetOutputMaximum( 255 );
		rescale->Update();

		ToClut::Pointer toR = ToClut::New();
		toR->GetFunctor().SetChannel(1);
		toR->SetInput(rescale->GetOutput());
		toR->Update();

		ToClut::Pointer toG = ToClut::New();
		toG->GetFunctor().SetChannel(2);
		toG->SetInput(rescale->GetOutput());
		toG->Update();

		ToClut::Pointer toB = ToClut::New();
		toB->GetFunctor().SetChannel(3);
		toB->SetInput(rescale->GetOutput());
		toB->Update();

		rescale1->SetOutputMinimum( 0 );
		rescale1->SetOutputMaximum( 255 );
		rescale1->SetAlpha( 100 );
		rescale1->SetBeta( 130 );

		CastToScalarType::Pointer rescaleR = CastToScalarType::New();
		rescaleR->SetInput(toR->GetOutput());
		rescaleR->SetOutputMinimum(0);
		rescaleR->SetOutputMaximum(255);
		rescaleR->SetAlpha( 45 );
		rescaleR->SetBeta( 130 );
		rescaleR->Update();

		CastToScalarType::Pointer rescaleG = CastToScalarType::New();
		rescaleG->SetInput(toG->GetOutput());
		rescaleG->SetOutputMinimum(0);
		rescaleG->SetOutputMaximum(255);
		rescaleG->SetAlpha( 45 );
		rescaleG->SetBeta( 130 );
		rescaleG->Update();

		CastToScalarType::Pointer rescaleB = CastToScalarType::New();
		rescaleB->SetInput(toB->GetOutput());
		rescaleB->SetOutputMinimum(0);
		rescaleB->SetOutputMaximum(255);
		rescaleB->SetAlpha( 45 );
		rescaleB->SetBeta( 130 );
		rescaleB->Update();

		FWavelet::Pointer w1 = FWavelet::New();
		w1->SetInput(rescale1->GetOutput());
		w1->SetWaveletDepth(waveletDepth);
		w1->Update();

		FWavelet::Pointer wR = FWavelet::New();
		wR->SetInput(rescaleR->GetOutput());
		wR->SetWaveletDepth(waveletDepth);
		wR->Update();

		FWavelet::Pointer wG = FWavelet::New();
		wG->SetInput(rescaleG->GetOutput());
		wG->SetWaveletDepth(waveletDepth);
		wG->Update();

		FWavelet::Pointer wB = FWavelet::New();
		wB->SetInput(rescaleB->GetOutput());
		wB->SetWaveletDepth(waveletDepth);
		wB->Update();

		WFusion::Pointer fR = WFusion::New();
		fR->SetInput(w1->GetOutput());
		fR->SetInput2(wR->GetOutput());
		fR->SetWaveletDepth(waveletDepth);
		fR->SetSizeW(sizeW);
		fR->SetPesoA(pesoA);
		fR->SetPesoB(pesoB);
		
		switch (_fusionRule) {
			case Sostituzione_dettaglio:
				fR->SetFusionRule(WFusion::rule1);
				break;
			case Modulo_max:
				fR->SetFusionRule(WFusion::rule2);
				break;
			case Consistenza_media:
				fR->SetFusionRule(WFusion::rule3);
				break;
			case Consistenza_var:
				fR->SetFusionRule(WFusion::rule4);
				break;		
		}
		fR->Update();

		WFusion::Pointer fG = WFusion::New();
		fG->SetInput(w1->GetOutput());
		fG->SetInput2(wG->GetOutput());
		fG->SetWaveletDepth(waveletDepth);
		fG->SetSizeW(sizeW);
		fG->SetPesoA(pesoA);
		fG->SetPesoB(pesoB);

		switch (_fusionRule) {
			case Sostituzione_dettaglio:
				fG->SetFusionRule(WFusion::rule1);
				break;
			case Modulo_max:
				fG->SetFusionRule(WFusion::rule2);
				break;
			case Consistenza_media:
				fG->SetFusionRule(WFusion::rule3);
				break;
			case Consistenza_var:
				fG->SetFusionRule(WFusion::rule4);
				break;
		}
		fG->Update();

		WFusion::Pointer fB = WFusion::New();
		fB->SetInput(w1->GetOutput());
		fB->SetInput2(wB->GetOutput());
		fB->SetWaveletDepth(waveletDepth);
		fB->SetSizeW(sizeW);
		fB->SetPesoA(pesoA);
		fB->SetPesoB(pesoB);

		switch (_fusionRule) {
			case Sostituzione_dettaglio:
				fB->SetFusionRule(WFusion::rule1);
				break;
			case Modulo_max:
				fB->SetFusionRule(WFusion::rule2);
				break;
			case Consistenza_media:
				fB->SetFusionRule(WFusion::rule3);
				break;
			case Consistenza_var:
				fB->SetFusionRule(WFusion::rule4);
				break;
		}
		fB->Update();

		BWavelet::Pointer bR = BWavelet::New();
		BWavelet::Pointer bG = BWavelet::New();
		BWavelet::Pointer bB = BWavelet::New();

		if (_fusionRule == Consistenza_media||_fusionRule == Consistenza_var) {
		// verifica della consistenza: sfruttiamo un filtro a votazione di maggioranza per ottenere una mappa di decisione
	
			typedef itk::BinaryMedianImageFilter<ImageType, ImageType > FilterType;
			FilterType::Pointer filterR = FilterType::New();
			FilterType::Pointer filterG = FilterType::New();
			FilterType::Pointer filterB = FilterType::New();
		
			ImageType::SizeType indexRadius;
			indexRadius[0] = sizeW; // radius along x
			indexRadius[1] = sizeW; // radius along y
			indexRadius[2] = 0; // radius along y
			
			filterR->SetRadius( indexRadius );
			filterR->SetBackgroundValue( 0);
			filterR->SetForegroundValue( 255 );	
			filterR->SetInput( fR->GetOutput() );
			filterR->Update();

			filterG->SetRadius( indexRadius );
			filterG->SetBackgroundValue( 0 );
			filterG->SetForegroundValue( 255 );
			filterG->SetInput( fG->GetOutput() );
			filterG->Update();
	
			filterB->SetRadius( indexRadius );
			filterB->SetBackgroundValue( 0 );
			filterB->SetForegroundValue( 255 );
			filterB->SetInput( fB->GetOutput() );
			filterB->Update();

			typedef itk::WaveletFusionConsImageFilter < ImageType > ConsFusionFilter;
			ConsFusionFilter::Pointer consfusionR = ConsFusionFilter::New();
			ConsFusionFilter::Pointer consfusionG = ConsFusionFilter::New();
			ConsFusionFilter::Pointer consfusionB = ConsFusionFilter::New();
			
			consfusionR->SetInput(w1->GetOutput());
			consfusionR->SetInput2(wR->GetOutput());
			consfusionR->SetInput3(filterR->GetOutput());
			consfusionR->SetPesoA(pesoA);
			consfusionR->SetPesoB(pesoB);
			consfusionR->SetWaveletDepth(waveletDepth);
			consfusionR->SetSizeW(sizeW);

			switch (_fusionRule) {
				case Consistenza_media:
					consfusionR->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consfusionR->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consfusionR->Update();

			consfusionG->SetInput(w1->GetOutput());
			consfusionG->SetInput2(wG->GetOutput());
			consfusionG->SetInput3(filterG->GetOutput());
			consfusionG->SetPesoA(pesoA);
			consfusionG->SetPesoB(pesoB);
			consfusionG->SetWaveletDepth(waveletDepth);
			consfusionG->SetSizeW(sizeW);

			switch (_fusionRule) {
				case Consistenza_media:
					consfusionG->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consfusionG->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consfusionG->Update();

			consfusionB->SetInput(w1->GetOutput());
			consfusionB->SetInput2(wB->GetOutput());
			consfusionB->SetInput3(filterB->GetOutput());
			consfusionB->SetPesoA(pesoA);
			consfusionB->SetPesoB(pesoB);
			consfusionB->SetWaveletDepth(waveletDepth);
			consfusionB->SetSizeW(sizeW);

			switch (_fusionRule) {
				case Consistenza_media:
					consfusionB->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consfusionB->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consfusionB->Update();
			
			bR->SetInput(consfusionR->GetOutput());
			bR->SetWaveletDepth(waveletDepth);
			bR->Update();

			bG->SetInput(consfusionG->GetOutput());
			bG->SetWaveletDepth(waveletDepth);
			bG->Update();

			bB->SetInput(consfusionB->GetOutput());
			bB->SetWaveletDepth(waveletDepth);
			bB->Update();
		}
		else {
			bR->SetInput(fR->GetOutput());
			bR->SetWaveletDepth(waveletDepth);
			bR->Update();

			bG->SetInput(fG->GetOutput());
			bG->SetWaveletDepth(waveletDepth);
			bG->Update();

			bB->SetInput(fB->GetOutput());
			bB->SetWaveletDepth(waveletDepth);
			bB->Update();
		}
		
		CastToImageType::Pointer rescaleToR = CastToImageType::New();
		rescaleToR->SetInput(bR->GetOutput());
		rescaleToR->SetOutputMinimum(0);
		rescaleToR->SetOutputMaximum(255);
		rescaleToR->SetAlpha( 30 );
		rescaleToR->SetBeta( 65 );

		CastToImageType::Pointer rescaleToG = CastToImageType::New();
		rescaleToG->SetInput(bG->GetOutput());
		rescaleToG->SetOutputMinimum(0);
		rescaleToG->SetOutputMaximum(255);
		rescaleToG->SetAlpha( 30 );
		rescaleToG->SetBeta( 65 );

		CastToImageType::Pointer rescaleToB = CastToImageType::New();
		rescaleToB->SetInput(bB->GetOutput());
		rescaleToB->SetOutputMinimum(0);
		rescaleToB->SetOutputMaximum(255);
		rescaleToB->SetAlpha( 30 );
		rescaleToB->SetBeta( 65 );

		ToRGB::Pointer toRgb = ToRGB::New();
		toRgb->SetInput1(rescaleToR->GetOutput());
		toRgb->SetInput2(rescaleToG->GetOutput());
		toRgb->SetInput3(rescaleToB->GetOutput());
		toRgb->Update(); 

		update(toRgb->GetOutput());
	}*/
}


void fusionFilter::DaubNWavelet(InputRealType pesoA, InputRealType pesoB) {
	if (_applyClut)  {
		DaubNWaveletClut(pesoA, pesoB);
		return;
	}
	if (_itkImage1) {
		int sizeW = _sizeWindow, waveletDepth = _levelDec;
		int N=8;

		switch (_waveletType) {
			case 1:
				N=4;
				break;
			case 2:
				N=8;
				break;
			case 3:
				N=20;
				break;
		}

		typedef itk::RescaleIntensityImageFilter < ImageType, ImageType > RescaleType;
		typedef itk::ResampleImageFilter < ImageType, ImageType > ResampleType;
		typedef itk::WaveletFusionImageFilter < ImageType > FusionFilter;
		typedef itk::DaubNWaveletTransformImageFilter < ImageType > FfWaveletType;
		typedef itk::DaubNBackwardWaveletTransformImageFilter < ImageType > BwWaveletType;
		//typedef itk::SigmoidImageFilter<ImageType,ImageType> SigmoidFilterType;	


		ImageType::Pointer im1 = _itkImage1;
		ImageType::Pointer im2 = _itkImage2;
		ImageType::SizeType size1 = im1->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType size2 = im2->GetLargestPossibleRegion().GetSize();

		if (size1[2] == 1 && size2[2] == 1) {
			PreStadio2D(im1, im2);
		}


		/*SigmoidFilterType::Pointer rescale1 = SigmoidFilterType::New();	
		rescale1->SetOutputMinimum( 0 );
		rescale1->SetOutputMaximum( 500 );
		rescale1->SetAlpha( 100 );
		rescale1->SetBeta( 130 );
		rescale1->SetInput( im1 );
		rescale1->Update();
		
		SigmoidFilterType::Pointer rescale2 = SigmoidFilterType::New();
		rescale2->SetOutputMinimum( 0 );
		rescale2->SetOutputMaximum( 500 );
		rescale2->SetAlpha( 50 );
		rescale2->SetBeta( 130 );
		rescale2->SetInput( im2 );
		rescale2->Update();*/
		
		ResampleType::Pointer resample = ResampleType::New();
		FfWaveletType::Pointer ff1 = FfWaveletType::New();
		FfWaveletType::Pointer ff2 = FfWaveletType::New();

		if (size1[0]<size2[0]) {
			resample->SetInput( im1 );
			resample->SetSize( im2->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im2->GetOrigin() );
			resample->SetOutputSpacing( im2->GetSpacing() );
			resample->Update();

			ff1->SetInput( resample->GetOutput() );
			ff2->SetInput( im2 );
		}
		else {
			resample->SetInput(  im2 );
			resample->SetSize( im1->GetLargestPossibleRegion().GetSize() );
			resample->SetOutputOrigin(  im1->GetOrigin() );
			resample->SetOutputSpacing( im1->GetSpacing() );
			resample->Update();

			ff1->SetInput( im1 );
			ff2->SetInput( resample->GetOutput() );
		}

		ff1->SetWaveletDepth(waveletDepth);
		ff1->SetWaveletType(N);
		ff1->Update();

		ff2->SetWaveletDepth(waveletDepth);
		ff2->SetWaveletType(N);
		ff2->Update();
		
		FusionFilter::Pointer fusion = FusionFilter::New();
		fusion->SetInput(ff1->GetOutput());
		fusion->SetInput2(ff2->GetOutput());
		fusion->SetPesoA(pesoA);
		fusion->SetPesoB(pesoB);
		fusion->SetWaveletDepth(waveletDepth);
		fusion->SetSizeW(sizeW);
		
		switch (_fusionRule)  {
			case Sostituzione_dettaglio:
				fusion->SetFusionRule(FusionFilter::rule1);
				break;
			case Modulo_max:
				fusion->SetFusionRule(FusionFilter::rule2);
				break;
			case Consistenza_media:
				fusion->SetFusionRule(FusionFilter::rule3);
				break;
			case Consistenza_var:
				fusion->SetFusionRule(FusionFilter::rule4);
				break;
		}
		
		fusion->Update();
		
		BwWaveletType::Pointer bw = BwWaveletType::New();

		if (_fusionRule == Consistenza_media ||_fusionRule == Consistenza_var) {
		// verifica della consistenza: sfruttiamo un filtro a votazione di maggioranza per ottenere una mappa di decisione
	
			typedef itk::BinaryMedianImageFilter<ImageType, ImageType > FilterType;
			FilterType::Pointer filter = FilterType::New();
		
			ImageType::SizeType indexRadius;
			indexRadius[0] = sizeW; // radius along x
			indexRadius[1] = sizeW; // radius along y
			indexRadius[2] = 0;		// radius along y
			filter->SetRadius( indexRadius );
			filter->SetBackgroundValue( 0 );
			filter->SetForegroundValue( 255 );
			filter->SetInput( fusion->GetOutput() );
			filter->Update();
	
			typedef itk::WaveletFusionConsImageFilter < ImageType > ConsFusionFilter;
			ConsFusionFilter::Pointer consfusion = ConsFusionFilter::New();
			consfusion->SetInput(ff1->GetOutput());
			consfusion->SetInput2(ff2->GetOutput());
			consfusion->SetInput3(filter->GetOutput());
			consfusion->SetPesoA(pesoA);
			consfusion->SetPesoB(pesoB);
			consfusion->SetWaveletDepth(waveletDepth);
			consfusion->SetSizeW(sizeW);
			
			switch (_fusionRule) {
				case Consistenza_media:
					consfusion->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consfusion->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consfusion->Update();

			bw->SetInput(consfusion->GetOutput());
			bw->SetWaveletDepth(waveletDepth);
			bw->SetWaveletType(N);
			bw->Update();
			
			update(bw->GetOutput());
		}
		else  {
			bw->SetInput(fusion->GetOutput());
			bw->SetWaveletDepth(waveletDepth);
			bw->SetWaveletType(N);
			bw->Update();
			update(bw->GetOutput());
		}

	}	
}
		

void fusionFilter::DaubNWaveletClut(InputRealType pesoA, InputRealType pesoB) {
	if (_itkImage1) {
		typedef itk::Image <unsigned char, Dimension> ScalarType;
		typedef itk::ResampleImageFilter <ImageType, ImageType> ResampleType;
		typedef itk::RescaleIntensityImageFilter <ImageType, ImageType> RescaleType;
		typedef itk::RescaleIntensityImageFilter <ImageType, ScalarType> RescaleToScalarType;
		typedef itk::RescaleIntensityImageFilter <ScalarType, ImageType> RescaleToImageType;
		typedef itk::Functor::ApplyClut <unsigned char> ApplyClutType;
		typedef itk::UnaryFunctorImageFilter <ScalarType, ScalarType, ApplyClutType> ToClut;
		typedef itk::DaubNWaveletTransformImageFilter <ImageType> FWavelet;
		typedef itk::DaubNBackwardWaveletTransformImageFilter <ImageType> BWavelet;
		typedef itk::WaveletFusionConsImageFilter <ImageType> ConsFusionFilter;
		typedef itk::WaveletFusionImageFilter <ImageType> WFusion;
		typedef itk::BinaryMedianImageFilter <ImageType, ImageType> BinaryFilterType;
		typedef itk::ComposeRGBImageFilter <ScalarType> ToRGB;

		int sizeW = _sizeWindow+2, waveletDepth = _levelDec;
		int	N = 8;

		//Livello di decomposizione della wavelet
		switch(_waveletType) {
			case 1:
				N = 4;
				break;
			case 2:
				N = 8;
				break;
			case 3:
				N = 20;
				break;
		}

		ImageType::SizeType size1 = _itkImage1->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType size2 = _itkImage2->GetLargestPossibleRegion().GetSize();

		if (size1[2] == 1 && size2[2] == 1) {
			PreStadio2D(_itkImage1, _itkImage2);
		}		

		//Interpolazione delle immagini di una serie rispetto all'altra
		ResampleType::Pointer resample = ResampleType::New();
		
		//Riscalamento dell'intensità dei pixel della seconda serie
		RescaleType::Pointer rescale2 = RescaleType::New();
		
		//Riscalamento dell'intensità dei pixel della seconda serie per la CLUT
		RescaleToScalarType::Pointer rescale2Clut = RescaleToScalarType::New();

		if (size1[0] < size2[0]) {
			//Interpolazione
			resample->SetInput(_itkImage1);
			resample->SetSize(_itkImage2->GetLargestPossibleRegion().GetSize());
			resample->SetOutputOrigin(_itkImage2->GetOrigin());
			resample->SetOutputSpacing(_itkImage2->GetSpacing());
			resample->Update();
            			
			//Riscalamento
			rescale2->SetInput(_itkImage2);
			rescale2Clut->SetInput(_itkImage2);
		}
		else {
			//Interpolazione
			resample->SetInput(_itkImage2);
			resample->SetSize(_itkImage1->GetLargestPossibleRegion().GetSize());
			resample->SetOutputOrigin(_itkImage1->GetOrigin());
			resample->SetOutputSpacing(_itkImage1->GetSpacing());
			resample->Update();
			
			//Riscalamento
			rescale2->SetInput(resample->GetOutput());
			rescale2Clut->SetInput(resample->GetOutput());
		}
		
		//Riscalamento
		rescale2->SetOutputMinimum(0);
		rescale2->SetOutputMaximum(500);
		rescale2->Update();

		rescale2Clut->SetOutputMinimum(0);
		rescale2Clut->SetOutputMaximum(500);
		rescale2Clut->Update();

		//Applicazione della CLUT alla seconda serie
		ToClut::Pointer toR2 = ToClut::New();
		toR2->GetFunctor().SetChannel(1);
		toR2->SetInput(rescale2Clut->GetOutput());
		toR2->Update();

		ToClut::Pointer toG2 = ToClut::New();
		toG2->GetFunctor().SetChannel(2);
		toG2->SetInput(rescale2Clut->GetOutput());
		toG2->Update();

		ToClut::Pointer toB2 = ToClut::New();
		toB2->GetFunctor().SetChannel(3);
		toB2->SetInput(rescale2Clut->GetOutput());
		toB2->Update();

		//Riscalamento dei canali della CLUT
		RescaleToImageType::Pointer rescale2R = RescaleToImageType::New();
		rescale2R->SetInput(toR2->GetOutput());
		rescale2R->SetOutputMinimum(0);
		rescale2R->SetOutputMaximum(500);
		rescale2R->Update();

		RescaleToImageType::Pointer rescale2G = RescaleToImageType::New();
		rescale2G->SetInput(toG2->GetOutput());
		rescale2G->SetOutputMinimum(0);
		rescale2G->SetOutputMaximum(500);
		rescale2G->Update();

		RescaleToImageType::Pointer rescale2B = RescaleToImageType::New();
		rescale2B->SetInput(toB2->GetOutput());
		rescale2B->SetOutputMinimum(0);
		rescale2B->SetOutputMaximum(500);
		rescale2B->Update();

		//Forward Wavelet della prima serie
		FWavelet::Pointer w1 = FWavelet::New();
		w1->SetInput(_itkImage1);
		w1->SetWaveletDepth(waveletDepth);
		w1->SetWaveletType(N);
		w1->Update();

		//Forward Wavelet della seconda serie
		FWavelet::Pointer w2 = FWavelet::New();
		w2->SetInput(rescale2->GetOutput());
		w2->SetWaveletDepth(waveletDepth);
		w2->SetWaveletType(N);
		w2->Update();

		//Forward Wavelet della seconda serie dopo aver applicato la CLUT
		FWavelet::Pointer w2R = FWavelet::New();
		w2R->SetInput(rescale2R->GetOutput());
		w2R->SetWaveletDepth(waveletDepth);
		w2R->SetWaveletType(N);
		w2R->Update();

		FWavelet::Pointer w2G = FWavelet::New();
		w2G->SetInput(rescale2G->GetOutput());
		w2G->SetWaveletDepth(waveletDepth);
		w2G->SetWaveletType(N);
		w2G->Update();

		FWavelet::Pointer w2B = FWavelet::New();
		w2B->SetInput(rescale2B->GetOutput());
		w2B->SetWaveletDepth(waveletDepth);
		w2B->SetWaveletType(N);
		w2B->Update();

		//Fusione delle due decomposizioni Wavelet
		WFusion::Pointer fusion = WFusion::New();
		fusion->SetInput(w1->GetOutput());
		fusion->SetInput2(w2->GetOutput());
		fusion->SetWaveletDepth(waveletDepth);
		fusion->SetSizeW(sizeW);
		fusion->SetPesoA(pesoA);
		fusion->SetPesoB(pesoB);

		switch (_fusionRule) {
			case Sostituzione_dettaglio:
				fusion->SetFusionRule(WFusion::rule1);
				break;
			case Modulo_max:
				fusion->SetFusionRule(WFusion::rule2);
				break;
			case Consistenza_media:
				fusion->SetFusionRule(WFusion::rule3);
				break;
			case Consistenza_var:
				fusion->SetFusionRule(WFusion::rule4);
				break;
		}
		fusion->Update();

		//Backward Wavelet dei tre canali R, G e B
		BWavelet::Pointer bWR = BWavelet::New();
		BWavelet::Pointer bWG = BWavelet::New();
		BWavelet::Pointer bWB = BWavelet::New();

		if(_fusionRule == Consistenza_media || _fusionRule == Consistenza_var) {
			//Filtro di maggioranza
			BinaryFilterType::Pointer binaryFilter = BinaryFilterType::New();

			ImageType::SizeType indexRadius;
			indexRadius[0] = sizeW;		//radius along x
			indexRadius[1] = sizeW;		//radius along y
			indexRadius[2] = 0;			//radius along z
			
			//Applicazione della verifica di consistenza
			binaryFilter->SetRadius(indexRadius);
			binaryFilter->SetBackgroundValue(0);
			binaryFilter->SetForegroundValue(255);	
			binaryFilter->SetInput(fusion->GetOutput());
			binaryFilter->Update();

			//Fusione con verifica della consistenza
			ConsFusionFilter::Pointer consFusionR = ConsFusionFilter::New();
			ConsFusionFilter::Pointer consFusionG = ConsFusionFilter::New();
			ConsFusionFilter::Pointer consFusionB = ConsFusionFilter::New();

			//Fusione del canale rosso
			consFusionR->SetInput(w1->GetOutput());
			consFusionR->SetInput2(w2R->GetOutput());
			consFusionR->SetInput3(binaryFilter->GetOutput());
			consFusionR->SetPesoA(pesoA);
			consFusionR->SetPesoB(pesoB);
			consFusionR->SetWaveletDepth(waveletDepth);
			consFusionR->SetSizeW(sizeW);
			switch(_fusionRule) {
				case Consistenza_media:
					consFusionR->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consFusionR->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consFusionR->Update();

			//Fusione del canale verde
			consFusionG->SetInput(w1->GetOutput());
			consFusionG->SetInput2(w2G->GetOutput());
			consFusionG->SetInput3(binaryFilter->GetOutput());
			consFusionG->SetPesoA(pesoA);
			consFusionG->SetPesoB(pesoB);
			consFusionG->SetWaveletDepth(waveletDepth);
			consFusionG->SetSizeW(sizeW);
			switch (_fusionRule) {
				case Consistenza_media:
					consFusionG->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consFusionG->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consFusionG->Update();

			//Fusione del canale blu
			consFusionB->SetInput(w1->GetOutput());
			consFusionB->SetInput2(w2B->GetOutput());
			consFusionB->SetInput3(binaryFilter->GetOutput());
			consFusionB->SetPesoA(pesoA);
			consFusionB->SetPesoB(pesoB);
			consFusionB->SetWaveletDepth(waveletDepth);
			consFusionB->SetSizeW(sizeW);
			switch (_fusionRule) {
				case Consistenza_media:
					consFusionB->SetFusionRule(ConsFusionFilter::rule3);
					break;
				case Consistenza_var:
					consFusionB->SetFusionRule(ConsFusionFilter::rule4);
					break;
			}
			consFusionB->Update();

			//Antitrasformata Wavelet dei 3 canali della serie fusa
			bWR->SetInput(consFusionR->GetOutput());
			bWR->SetWaveletDepth(waveletDepth);
			bWR->SetWaveletType(N);
			bWR->Update();

			bWG->SetInput(consFusionG->GetOutput());
			bWG->SetWaveletDepth(waveletDepth);
			bWG->SetWaveletType(N);
			bWG->Update();

			bWB->SetInput(consFusionB->GetOutput());
			bWB->SetWaveletDepth(waveletDepth);
			bWB->SetWaveletType(N);
			bWB->Update();
		}
		else {		
			//Fusione del canale rosso
			WFusion::Pointer fusionR = WFusion::New();
			fusionR->SetInput(w1->GetOutput());
			fusionR->SetInput2(w2R->GetOutput());
			fusionR->SetWaveletDepth(waveletDepth);
			fusionR->SetSizeW(sizeW);
			fusionR->SetPesoA(pesoA);
			fusionR->SetPesoB(pesoB);
			switch (_fusionRule) {
				case Sostituzione_dettaglio:
					fusionR->SetFusionRule(WFusion::rule1);
					break;
				case Modulo_max:
					fusionR->SetFusionRule(WFusion::rule2);
					break;
				case Consistenza_media:
					fusionR->SetFusionRule(WFusion::rule3);
					break;
				case Consistenza_var:
					fusionR->SetFusionRule(WFusion::rule4);
					break;
			}
			fusionR->Update();

			//Fusione del canale verde
			WFusion::Pointer fusionG = WFusion::New();
			fusionG->SetInput(w1->GetOutput());
			fusionG->SetInput2(w2G->GetOutput());
			fusionG->SetWaveletDepth(waveletDepth);
			fusionG->SetSizeW(sizeW);
			fusionG->SetPesoA(pesoA);
			fusionG->SetPesoB(pesoB);
			switch (_fusionRule) {
				case Sostituzione_dettaglio:
					fusionG->SetFusionRule(WFusion::rule1);
					break;
				case Modulo_max:
					fusionG->SetFusionRule(WFusion::rule2);
					break;
				case Consistenza_media:
					fusionG->SetFusionRule(WFusion::rule3);
					break;
				case Consistenza_var:
					fusionG->SetFusionRule(WFusion::rule4);
					break;
			}
			fusionG->Update();

			//Fusione del canale blu
			WFusion::Pointer fusionB = WFusion::New();
			fusionB->SetInput(w1->GetOutput());
			fusionB->SetInput2(w2B->GetOutput());
			fusionB->SetWaveletDepth(waveletDepth);
			fusionB->SetSizeW(sizeW);
			fusionB->SetPesoA(pesoA);
			fusionB->SetPesoB(pesoB);
			switch (_fusionRule) {
				case Sostituzione_dettaglio:
					fusionB->SetFusionRule(WFusion::rule1);
					break;
				case Modulo_max:
					fusionB->SetFusionRule(WFusion::rule2);
					break;
				case Consistenza_media:
					fusionB->SetFusionRule(WFusion::rule3);
					break;
				case Consistenza_var:
					fusionB->SetFusionRule(WFusion::rule4);
					break;

			}
			fusionB->Update();
			
			//Antitrasformata Wavelet dei 3 canali della serie fusa
			bWR->SetInput(fusionR->GetOutput());
			bWR->SetWaveletType(N);
			bWR->SetWaveletDepth(waveletDepth);
			bWR->Update();

			bWG->SetInput(fusionG->GetOutput());
			bWG->SetWaveletType(N);
			bWG->SetWaveletDepth(waveletDepth);
			bWG->Update();

			bWB->SetInput(fusionB->GetOutput());
			bWB->SetWaveletType(N);
			bWB->SetWaveletDepth(waveletDepth);
			bWB->Update();
		}
		
		//Riscalamento dei 3 canali della serie fusa
		RescaleToScalarType::Pointer rescaleToR = RescaleToScalarType::New();
		rescaleToR->SetInput(bWR->GetOutput());
		rescaleToR->SetOutputMinimum(0);
		rescaleToR->SetOutputMaximum(500);
		rescaleToR->Update();

		RescaleToScalarType::Pointer rescaleToG = RescaleToScalarType::New();
		rescaleToG->SetInput(bWG->GetOutput());
		rescaleToG->SetOutputMinimum(0);
		rescaleToG->SetOutputMaximum(500);
		rescaleToG->Update();

		RescaleToScalarType::Pointer rescaleToB = RescaleToScalarType::New();
		rescaleToB->SetInput(bWB->GetOutput());
		rescaleToB->SetOutputMinimum(0);
		rescaleToB->SetOutputMaximum(500);
		rescaleToB->Update();

		//Composizione della serie fusa in formato RGB
		ToRGB::Pointer toRgb = ToRGB::New();
		toRgb->SetInput1(rescaleToR->GetOutput());
		toRgb->SetInput2(rescaleToG->GetOutput());
		toRgb->SetInput3(rescaleToB->GetOutput());
		toRgb->Update();

		//Visualizzazione della serie fusa
		update(toRgb->GetOutput());
	}
}


void fusionFilter::PreStadio2D(ImageType::Pointer im1, ImageType::Pointer im2) {
	ImageType::PointType origin1 = im1->GetOrigin();
	origin1.SetElement(2, 0);
	im1->SetOrigin(origin1);

	ImageType::PointType origin2 = im2->GetOrigin();
	origin2.SetElement(2, 0);
	im2->SetOrigin(origin2);
}