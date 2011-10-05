/**
 * \file itkFusion2.h
 * \brief File per la gestione della fusione con la tecnica wavelet senza verifica della consistenza
 * \author ICAR-CNR Napoli
 */

#ifndef _itkFusion2_h_
#define _itkFusion2_h_

#include <itkImageToImageFilter.h>
#include <itkJoinImageFilter.h>
#include <itkImageToHistogramGenerator.h>
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
	class WaveletFusionImageFilter : public ImageToImageFilter < TImage, TImage > {

	public:

		/** Enumerazione delle regole di fusione
		 */
		enum fusionRuleType {
			rule1,				/*!< Regola di sostituzione completa del dettaglio */
			rule2,				/*!< Regola del massimo modulo */
			rule3,				/*!< Regola del massimo modulo con verifica di consistenza */
			rule4				/*!< Regola della varianza con verifica di consistenza */

		};
		
		typedef WaveletFusionImageFilter Self;
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
			ImagePointer       outputImage = this->GetOutput();

			RegionType outputRegion = outputImage->GetLargestPossibleRegion();

			outputImage->SetRegions( outputRegion );
			outputImage->Allocate();

			fusionRules(inputImage, inputImage2, outputImage);
			

		}

	protected:

		/**
		 * \biref Costruttore protetto del filtro
		 *
		 * Setta i valori di default per i parametri del filtro
		 */
		WaveletFusionImageFilter () {
			m_sizeW = 1;
			m_WaveletDepth = 1;
			m_FusionRule = rule1;
		};

		/**
		 * \brief Distruttore virtuale protetto
		 */
		virtual ~WaveletFusionImageFilter () {};

	private:

		typedef typename TImage::ConstPointer ImageConstPointer;
		typedef typename TImage::RegionType RegionType;
		typedef typename TImage::SizeType SizeType;
		typedef typename TImage::IndexType IndexType;
		typedef typename TImage::PixelType val;

		ImagePointer m_input2;
		InputRealType m_PesoA, m_PesoB;
		int m_WaveletDepth;
		int m_sizeW;
		fusionRuleType m_FusionRule;

		WaveletFusionImageFilter (const Self &);
		void operator = (const Self &);

		/**
		 * \brief Metodo che implementa le tecniche di fusione
		 *
		 * Questo metodo effettua la fusione delle immagini d'ingrsso e produce l'imamgine d'uscita utilizzando la regola di fusione opportuna
		 *
		 * \param imA Prima immagine in ingresso al filtro
		 * \param imB Seconda immagine in ingresso al filtro
		 * \param imR Immagine risultante dalla fusione delle due precedenti
		 */
		inline void fusionRules(ImageConstPointer imA, ImageConstPointer imB, ImagePointer imR) 
			{
			TImage::SizeType size = imA->GetLargestPossibleRegion().GetSize();
			TImage::IndexType pixelIndex;
			TImage::IndexType CentralpixelIndex;
			
			val max1, max2, valore;
/****************************************************************
*			Regola 1: Sostituzione completa del dettaglio		*
*																*
*																*
****************************************************************/
			if (m_FusionRule == rule1) 
				{
				for (int z=0; z<size[2]; z++) 
					{
					for (int i=0; i<size[0]; i++) 
						{
						for (int j=0; j<size[1]; j++) 
							{
							CentralpixelIndex[0] = i;
							CentralpixelIndex[1] = j;
							CentralpixelIndex[2] = z;

							if (i < size[0]/((double)pow((double)2,m_WaveletDepth)) && j < size[1]/((double)pow((double)2,m_WaveletDepth))) 
								{
								valore = imB->GetPixel(CentralpixelIndex);
								}
							else
								{
								valore = imA->GetPixel(CentralpixelIndex);
								}
							imR->SetPixel(CentralpixelIndex, valore);
							}
						}
					}
				}
/****************************************************************
*			Regola 2: Massimo Modulo (Modulus Maxima)			*
*																*
*																*
****************************************************************/			
			else if (m_FusionRule == rule2) 
				{
				for (int z=0; z<size[2]; z++) 
					{
					for (int i=0; i<size[0]; i++) 
						{
						for (int j=0; j<size[1]; j++) 
							{
							CentralpixelIndex[0] = i;
							CentralpixelIndex[1] = j;
							CentralpixelIndex[2] = z;
						
							max1 = m_PesoA * imA->GetPixel(CentralpixelIndex);
							max2 = m_PesoB * imB->GetPixel(CentralpixelIndex);
				
							for (int h = max<int>(0, i-m_sizeW); h <= min<int>(size[0]-1, i+m_sizeW); h++) 
								{
								for (int k = max<int>(0, j-m_sizeW); k <= min<int>(size[1]-1, j+m_sizeW); k++) 
									{
									pixelIndex[0] = h;
									pixelIndex[1] = k;
									pixelIndex[2] = z;

									max1 = max < val > (abs(max1), abs(m_PesoA * imA->GetPixel(pixelIndex)));
									max2 = max < val > (abs(max2), abs(m_PesoB * imB->GetPixel(pixelIndex)));

									}
								}
							valore = m_PesoA * imA->GetPixel(CentralpixelIndex) + m_PesoB * imB->GetPixel(CentralpixelIndex); 

							if (max1 >= max2) 
								{
								if (i >= size[0]/((double)pow((double)2,m_WaveletDepth)) || j >= size[1]/((double)pow((double)2,m_WaveletDepth)))
									valore = imA->GetPixel(CentralpixelIndex);
								}			
							else //max2 > max1
								{
								if (i >= size[0]/((double)pow((double)2,m_WaveletDepth)) || j >= size[1]/((double)pow((double)2,m_WaveletDepth)))
									valore = imB->GetPixel(CentralpixelIndex);
								}								
							imR->SetPixel(CentralpixelIndex, valore);
							}
						}
					}
				}
/****************************************************************
*			Regola 3: Creazione della Mappa di Decisione		*
*	(fase preliminare alla applicazione delle regole 3,			*
*	contenute in itkFusion3.h)									*
*																*
****************************************************************/
			else if (m_FusionRule == rule3)  // Calcolo della Decision map per la successiva verifica di consistenza
				{
				for (int z=0; z<size[2]; z++)
					{
					for (int i=0; i<size[0]; i++) 
						{
						for (int j=0; j<size[1]; j++) 
							{
							CentralpixelIndex[0] = i;
							CentralpixelIndex[1] = j;
							CentralpixelIndex[2] = z;
						
							max1 = m_PesoA * imA->GetPixel(CentralpixelIndex);
							max2 = m_PesoB * imB->GetPixel(CentralpixelIndex);
					
							for (int h = max<int>(0, i-m_sizeW); h <= min<int>(size[0]-1, i+m_sizeW); h++) 
								{
								for (int k = max<int>(0, j-m_sizeW); k <= min<int>(size[1]-1, j+m_sizeW); k++) 
									{
									pixelIndex[0] = h;
									pixelIndex[1] = k;
									pixelIndex[2] = z;

									max1 = max < val > (abs(max1), abs(m_PesoA * imA->GetPixel(pixelIndex)));
									max2 = max < val > (abs(max2), abs(m_PesoB * imB->GetPixel(pixelIndex)));

									}
								}
							//valore =0;

							if (max1 >= max2) 
								 {//luca: ho disattivato questi if perchè in effetti sono inutili:generare la mappa anche per i dettagli va bene
								if (i >= size[0]/((double)pow((double)2,m_WaveletDepth)) || j >= size[1]/((double)pow((double)2,m_WaveletDepth)))
									valore = 255;
								}			
							else //max2 > max1
								{
								if (i >= size[0]/((double)pow((double)2,m_WaveletDepth)) || j >= size[1]/((double)pow((double)2,m_WaveletDepth)))
									valore = 0;
								}
																
							imR->SetPixel(CentralpixelIndex, valore);
							
							}
						}
					}
				}


/****************************************************************
*			Regola 4: Creazione della Mappa di Decisione		*
*	(fase preliminare alla applicazione delle regole 4,			*
*	contenute in itkFusion3.h)									*
*																*
****************************************************************/
			else if(m_FusionRule == rule4) {
				typedef typename TImage::PixelType val;
				TImage::IndexType pixelIndex;

				val valore;
				int dim = ((double)pow((double)(2*m_sizeW + 1),2));
				short *winval1 = new short[dim];
				short *winval2 = new short[dim];

				for(int z = 0; z < size[2]; z++) {
					for(int i = 0; i < size[0]; i++) {
						for(int j = 0; j < size[1]; j++) {
							CentralpixelIndex[0] = i;
							CentralpixelIndex[1] = j;
							CentralpixelIndex[2] = z;
						
							//Approssimazioni
							if(i < size[0]/((double)pow((double)2,m_WaveletDepth)) && j < size[1]/((double)pow((double)2,m_WaveletDepth))) {
								short sum1 = 0, sum2 = 0;
								int count = 0;
								winval1[count] = m_PesoA * imA->GetPixel(CentralpixelIndex);
								winval2[count] = m_PesoB * imB->GetPixel(CentralpixelIndex);

								for (int h = max<int>(0, i-m_sizeW); h <= min<int>(size[0]/((double)pow((double)2,m_WaveletDepth))-1, i+m_sizeW); h++) {
									for (int k = max<int>(0, j-m_sizeW); k <= min<int>(size[1]/((double)pow((double)2,m_WaveletDepth))-1, j+m_sizeW); k++) {
										pixelIndex[0] = h;
										pixelIndex[1] = k;
										pixelIndex[2] = z;

										winval1[count] = m_PesoA * imA->GetPixel(pixelIndex);
										winval2[count] = m_PesoB * imB->GetPixel(pixelIndex);
										sum1 = sum1 + winval1[count];
										sum2 = sum2 + winval2[count];
										count++;
									}
								}

								double act1 = 0, act2 = 0;
								for(int n = 0; n < dim; n++) {
									act1 = act1 + (winval1[n] - sum1) * (winval1[n] - sum1);
									act2 = act2 + (winval2[n] - sum2) * (winval2[n] - sum2);
								}
								if(abs(act1) > abs(act2))
									valore = 255;
								else if(abs(act1) < abs(act2))
									valore = 0;
								else
									valore = 128;
							}
							
							//Dettagli
							else {
								max1 = m_PesoA * imA->GetPixel(CentralpixelIndex);
								max2 = m_PesoB * imB->GetPixel(CentralpixelIndex);
						
								for (int h = max<int>(0, i-m_sizeW); h <= min<int>(size[0]-1, i+m_sizeW); h++) {
									for (int k = max<int>(0, j-m_sizeW); k <= min<int>(size[1]-1, j+m_sizeW); k++) {
										pixelIndex[0] = h;
										pixelIndex[1] = k;
										pixelIndex[2] = z;

										max1 = max < val > (abs(max1), abs(m_PesoA * imA->GetPixel(pixelIndex)));
										max2 = max < val > (abs(max2), abs(m_PesoB * imB->GetPixel(pixelIndex)));
									}
								}
								if (max1 >= max2)
									valore = 255;
								else
									valore = 0;
							}
							imR->SetPixel(CentralpixelIndex, valore);
						}
					}
				}
				delete [] winval1;
				delete [] winval2;
			}
		}
	};
}

#endif _itkFusion2_h_