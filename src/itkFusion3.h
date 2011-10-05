/**
 * \file itkFusion3.h
 * \brief File per la gestione della fusione con la tecnica wavelet con verifica della consistenza
 * \author ICAR-CNR Napoli
 */

#ifndef _itkFusion3_h_
#define _itkFusion3_h_

#include <itkImageToImageFilter.h>


/**
 * \namespace itk
 */
namespace itk {

	/**
	 * \class WaveletFusionImageFilter
	 * \brief Filtro ITK per la fusione di immagini trasformate con la trasformata wavelet
	 *
	 * Questo filtro ITK effettua la fusione di immagini preliminarmente trasformate con la trasformata wavelet.
	 * Implementa due diverse regole di fusione: regola con verifica di consistenza e regola con sostituzione completa del dettaglio
	 */
	template < class TImage >
	class WaveletFusionConsImageFilter : public ImageToImageFilter < TImage, TImage > {

	public:
		enum fusionRuleType {
			rule3,				/*!< Regola del massimo modulo con verifica di consistenza con approssimazione mediata*/
			rule4				/*!< Regola del massimo modulo con verifica di consistenza con approssimazione calcolata con varianza locale*/
		};
		typedef WaveletFusionConsImageFilter Self;
		typedef ImageToImageFilter < TImage, TImage > Superclass;
		typedef SmartPointer < Self > Pointer;
		typedef const SmartPointer < Self > ConstPointer;

		/** Tipo di immagine passata come Template */
		typedef typename TImage::Pointer ImagePointer;
		
		/** Tipo per la gestione dei pesi */
		typedef itk::NumericTraits < short >::RealType InputRealType;

		itkNewMacro(Self);

		/**
		 * \brief Setta il peso per la prima immagine
		 *
		 * \param valore Valore del peso per la prima immagine
		 */
		inline void SetPesoA(InputRealType valore) {
			m_PesoA = valore;
		}

		/**
		 * \brief Setta il peso per la seconda immagine
		 *
		 * \param valore Valore del peso per la prima immagine
		 */
		inline void SetPesoB(InputRealType valore) {
			m_PesoB = valore;
		}

		/**
		 * \brief Setta la prima immagine d'ingresso
		 *
		 * \param input1 Puntatore alla prima immagine in igresso
		 */
		inline void SetInput1(ImagePointer input1) {
			SetInput(input1);
		}

		/**
		 * \brief Setta la seconda immagine d'ingresso
		 *
		 * \param input2 Puntatore alla seconda immagine in igresso
		 */
		inline void SetInput2(ImagePointer input2) {
			m_input2 = input2;
		}

		inline void SetInput3(ImagePointer input3) {
			m_input3 = input3;
		}

		/**
		 * \brief Setta la dimensione della finestra per il calcolo dell'attività
		 *
		 * \param value Dimensione della finestra per il calcolo dell'attività
		 */
		inline void SetSizeW(int value) {
			m_sizeW = value;
		}

		/**
		 * \brief Setta il livello di dettaglio con cui è stata calcolata la trasformata wavelet
		 *
		 * \param value Livello di dettaglio con cui è stata calcolata la trasformata wavelet
		 */
		inline void SetWaveletDepth(int value) {
			m_WaveletDepth = value;
		}

		/**
		 * \brief Setta la regola di fusione da utilizzare
		 *
		 * \param value Regola di fusione da utilizzare
		 */

		inline void SetFusionRule(fusionRuleType value) {
			m_FusionRule = value;
		}

		/**
		 * \brief Metodo che prepara l'immagine d'uscita, necessaria per la corretta esecuzione nella pipeline di ITK
		 */
		inline void GenerateOutputInformation() {
			Superclass::GenerateOutputInformation();

			ImageConstPointer inputImage  = this->GetInput();
			ImagePointer      outputImage = this->GetOutput();

			outputImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );
			outputImage->SetSpacing( inputImage->GetSpacing() );
			outputImage->SetOrigin( inputImage->GetOrigin() );
			outputImage->SetDirection( inputImage->GetDirection() );
		}
		
		/**
		 * \brief Metodo che effettua la fusione delle due immagini d'ingresso e alloca lo spazio in memoria per l'immagine d'uscita
		 */
		inline void GenerateData() {
			ImageConstPointer  inputImage  = this->GetInput();
			ImageConstPointer  inputImage2 = this->m_input2;
			ImageConstPointer  inputImage3 = this->m_input3;
			ImagePointer       outputImage = this->GetOutput();

			RegionType outputRegion = outputImage->GetLargestPossibleRegion();

			outputImage->SetRegions( outputRegion );
			outputImage->Allocate();

			fusionRules(inputImage, inputImage2, inputImage3, outputImage);
			

		}

	protected:

		/**
		 * \biref Costruttore protetto del filtro
		 *
		 * Setta i valori di default per i parametri del filtro
		 */
		WaveletFusionConsImageFilter () {
			m_sizeW = 1;
			m_WaveletDepth = 1;
			m_FusionRule = rule3;
		};

		/**
		 * \brief Distruttore virtuale protetto
		 */
		virtual ~WaveletFusionConsImageFilter () {};

	private:

		typedef typename TImage::ConstPointer ImageConstPointer;
		typedef typename TImage::RegionType RegionType;
		typedef typename TImage::SizeType SizeType;
		typedef typename TImage::IndexType IndexType;
		typedef typename TImage::PixelType val;

		ImagePointer m_input2, m_input3;
		InputRealType m_PesoA, m_PesoB;
		int m_WaveletDepth;
		int m_sizeW;
		fusionRuleType m_FusionRule;

		WaveletFusionConsImageFilter (const Self &);
		void operator = (const Self &);

		/**
		 * \brief Metodo che implementa le tecniche di fusione
		 *
		 * Questo metodo effettua la fusione delle immagini d'ingrsso e produce l'imamgine d'uscita utilizzando la regola di fusione opportuna
		 *
		 * \param imA Prima immagine in ingresso al filtro
		 * \param imB Seconda immagine in ingresso al filtro
		 * \param imBin Terza immagine in ingresso al filtro
		 * \param imR Immagine risultante dalla fusione delle due precedenti
		 */
		inline void fusionRules(ImageConstPointer imA, ImageConstPointer imB, ImageConstPointer imBin, ImagePointer imR) {
			TImage::SizeType size = imA->GetLargestPossibleRegion().GetSize();
			TImage::IndexType pixelIndex;
			TImage::IndexType CentralpixelIndex;
			
			val valore;
		
/****************************************************************
*			Regola 3: Regola del massimo modulo con verifica	*
			di consistenza con approssimazione mediata			*
*																*
*																*
****************************************************************/	
		if(m_FusionRule == rule3) {
			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;

						if (i < size[0]/((double)pow((double)2,m_WaveletDepth)) &&  j < size[1]/((double)pow((double)2,m_WaveletDepth))){
							// il pixel appartiene all'approssimazione
							/*if(imBin->GetPixel(pixelIndex)==255) 
								{
								valore = 0.75*imA->GetPixel(pixelIndex)+0.25*imB->GetPixel(pixelIndex);
								//valore = m_PesoA*imA->GetPixel(pixelIndex)+m_PesoB*imB->GetPixel(pixelIndex);
								imR->SetPixel(pixelIndex, valore);
								}
							else //if(imBin->GetPixel(pixelIndex)==0)
								{*/
							//il coeff. di dettaglio dell'immagine fusa deve essere preso dalla prima immagine 
								//valore = 0.25*imA->GetPixel(pixelIndex)+0.75*imB->GetPixel(pixelIndex);
								valore = m_PesoA*imA->GetPixel(pixelIndex)+m_PesoB*imB->GetPixel(pixelIndex);
								imR->SetPixel(pixelIndex, valore);
								//}
						} 
						else 
							if(imBin->GetPixel(pixelIndex)==255) {
							//il coeff. di dettaglio dell'immagine fusa deve essere preso dalla prima immagine 
							valore = imA->GetPixel(pixelIndex);
							imR->SetPixel(pixelIndex, valore);
																	}
						else {
							valore = imB->GetPixel(pixelIndex);
							imR->SetPixel(pixelIndex, valore);	
							}
					}
				}
			}
		}
		
/****************************************************************
*			Regola 4: Regola del massimo modulo con verifica	*
			di consistenza con approssimazione calcolata sulla	*
*			stima della varianza								*
*																*
****************************************************************/	
		/*else if(m_FusionRule == rule4) {
		int dim=pow(2*m_sizeW + 1,2);
			
			short* winval1 = new short[dim];
			short* winval2 = new short[dim];   //luca
			int peso = 0;
			
			
			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						CentralpixelIndex[0] = i;
						CentralpixelIndex[1] = j;
						CentralpixelIndex[2] = z;

						if (i < size[0]/((double)pow((double)2,m_WaveletDepth)) && j < size[1]/((double)pow((double)2,m_WaveletDepth))) 
							{
								//il pixel appartiene all'approssimazione
								//calcolo le attività basandomi su varianza locale alla finestra
								
								
							short sum1=0,sum2=0;
							int count=0;
							winval1[count]=m_PesoA *imA->GetPixel(CentralpixelIndex); //è necessario per far funzionare la regola anche con finestra nulla
							winval2[count]=m_PesoB *imB->GetPixel(CentralpixelIndex);

							for (int h = max<int>(0, i-m_sizeW); h <= min<int>(size[0]/((double)pow((double)2,m_WaveletDepth))-1, i+m_sizeW); h++) {
								for (int k = max<int>(0, j-m_sizeW); k <= min<int>(size[1]/((double)pow((double)2,m_WaveletDepth))-1, j+m_sizeW); k++) {
									pixelIndex[0] = h;
									pixelIndex[1] = k;
									pixelIndex[2] = z;

								//aggiunta per il calcolo dell'approssimazione come varianza, vedi G.Piella
								winval1[count]=m_PesoA *imA->GetPixel(pixelIndex);
								winval2[count]=m_PesoB *imB->GetPixel(pixelIndex);
								sum1=sum1+winval1[count];
								sum2=sum2+winval2[count];
								count ++;
							}
						}

						double act1=0,act2=0;
						for(int n=0;n<dim;n++) 
							{
							act1=act1+(winval1[n]-sum1)*(winval1[n]-sum1);
							act2=act2+(winval2[n]-sum2)*(winval2[n]-sum2);
							}
							
						
						if(act1>=act2)
							peso = peso - 1;
						else
							peso = peso + 1;

						//if(act1>=act2) 
							//{
							//valore = imA->GetPixel(CentralpixelIndex);
							//imR->SetPixel(CentralpixelIndex, valore); 
						//	}
						//else 
						//	{
						//	valore = imB->GetPixel(CentralpixelIndex);
						//	imR->SetPixel(CentralpixelIndex, valore); 
						//	}
						}
						else if(imBin->GetPixel(CentralpixelIndex)==255) { //NB: questa parte di codice, relativa ai dettagli, è uguale per entrambe le regole
							//il coeff. di dettaglio dell'immagine fusa deve essere preso dalla prima immagine 
							valore = imA->GetPixel(CentralpixelIndex);
							imR->SetPixel(CentralpixelIndex, valore);
						}
						else {
							valore = imB->GetPixel(CentralpixelIndex);
							imR->SetPixel(CentralpixelIndex, valore);	
							}
							
						}
					}
			}

			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]/((double)pow((double)2,m_WaveletDepth)); i++) {
					for (int j=0; j<size[1]/((double)pow((double)2,m_WaveletDepth)); j++) {
						CentralpixelIndex[0] = i;
						CentralpixelIndex[1] = j;
						CentralpixelIndex[2] = z;
						

					}
				}
			}
			
				delete [] winval1;
				delete [] winval2;
		}*/

		else if(m_FusionRule == rule4) {
			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						CentralpixelIndex[0] = i;
						CentralpixelIndex[1] = j;
						CentralpixelIndex[2] = z;
						
						if(imBin->GetPixel(CentralpixelIndex) == 255) {
							valore = 0.8 * imA->GetPixel(CentralpixelIndex) + 0.2 * imB->GetPixel(CentralpixelIndex);
							imR->SetPixel(CentralpixelIndex, valore);
						}
						else if(imBin->GetPixel(CentralpixelIndex) == 0) {
							valore = 0.2 * imA->GetPixel(CentralpixelIndex) + 0.8 * imB->GetPixel(CentralpixelIndex);								
							imR->SetPixel(CentralpixelIndex, valore);	
						}
						else {
							valore = 0.5 * imA->GetPixel(CentralpixelIndex) + 0.5 * imB->GetPixel(CentralpixelIndex);
							imR->SetPixel(CentralpixelIndex, valore);	
						}

					}
				}
			}
		}
			
		}

		//}
	};
}

#endif _itkFusion3_h_