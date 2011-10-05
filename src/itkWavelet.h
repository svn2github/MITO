/**
 * \file itkWavelet.h
 * \brief File per la gestione delle tecniche di fusione con le wavelet
 * \author ICAR-CNR Napoli
 */

#ifndef _itkWavelet_h_
#define _itkWavelet_h_

#include "itkDaub.h"
#include <itkImageToImageFilter.h>

/*
 *	According to K & R, the value of i % j is undefined for negative operands.
 *	It is often the case, however, that we want the result to be cyclical, so
 *	that -1 mod 5 = 4, for example.  The following macro will do this in a
 *	fairly machine-independent manner.  (Notice that we ignore the mathematical
 *	possibility of j <= 0, since in such a case there's probably something
 *	wrong with the code.)
 */
#define MOD(i, j) ( (i) >= 0 ? (i) % (j) : ((j) - ((-(i)) % (j))) % (j) )

/**
 * \namespace itk
 */
namespace itk {

	/**
	 * \class HaarWaveletTransformImageFilter
	 * \brief Filtro ITK che effettua la trasformata wavelet dell'immagine di ingresso utilizzando i filtri di Haar
	 */
	template < class TImage >
	class HaarWaveletTransformImageFilter : public ImageToImageFilter < TImage, TImage > {

	public:
		typedef HaarWaveletTransformImageFilter Self;
		typedef ImageToImageFilter < TImage, TImage > Superclass;
		typedef SmartPointer < Self > Pointer;
		typedef const SmartPointer < Self > ConstPointer;

		itkNewMacro(Self);

		/**
		 * \brief Setta il livello di dettaglio della decomposizione multilivello
		 *
		 * \param value Livello di dettaglio desiderato
		 */
		inline void SetWaveletDepth (int value) { 
			m_WaveletDepth = value - 1; 
		}

		/**
		 * \brief Metodo che prepara l'immagine d'uscita, necessaria per la corretta esecuzione nella pipeline di ITK
		 */
		inline void GenerateOutputInformation(){
				Superclass::GenerateOutputInformation();

				ImageConstPointer inputImage  = this->GetInput();
				ImagePointer      outputImage = this->GetOutput();

				outputImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );
				outputImage->SetSpacing( inputImage->GetSpacing() );
				outputImage->SetOrigin( inputImage->GetOrigin() );
				outputImage->SetDirection( inputImage->GetDirection() );
		}

		/**
		 * \brief Metodo che coordina la trasformata wavelet dell'immagine e alloca lo spazio in memoria per contenere l'immagine d'uscita
		 */
		inline void GenerateData(){
			ImageConstPointer  inputImage  = this->GetInput();
			ImagePointer       outputImage = this->GetOutput();

			RegionType outputRegion = outputImage->GetLargestPossibleRegion();

			outputImage->SetRegions( outputRegion );
			outputImage->Allocate();

			TImage::SizeType size = inputImage->GetLargestPossibleRegion().GetSize();
			_sizeData = size[0];
			TImage::IndexType pixelIndex;

			_data = new val * [size[0]];
			for (int i=0; i<size[0]; i++)
				_data[i] = new val[size[1]];

			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						_data[i][j] = inputImage->GetPixel(pixelIndex);
					}
				}
				int n,n2;
				for (n=size[1],n2=size[0]; n>=size[1]/((double)pow((double)2,m_WaveletDepth))||n2>=size[0]/((double)pow((double)2,m_WaveletDepth)); n/=2,n2/=2) 
				{

					for (int i=0; i<n; i++)
						transform_row(_data, i, n);
					for (int i=0; i<n2; i++)
						transform_col(_data, i, n2);
				}

				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						outputImage->SetPixel(pixelIndex, _data[i][j]);
					}
				}
			}
		}

	protected:

		/**
		 * \brief Costruttore protetto del filtro
		 *
		 * Serve per settare il livello di dettaglio al valore di default
		 */
		HaarWaveletTransformImageFilter () {
			m_WaveletDepth = 1;
		};

		/**
		 * \brief Distruttore virtuale del filtro
		 */
		virtual ~HaarWaveletTransformImageFilter () {
			if(_sizeData != 0) {
				for (int i=0; i<_sizeData; i++)
					delete[] _data[i];
				delete[] _data;
			}
		};

	private:
		typedef typename TImage::Pointer ImagePointer;
		typedef typename TImage::ConstPointer ImageConstPointer;
		typedef typename TImage::RegionType RegionType;
		typedef typename TImage::SizeType SizeType;
		typedef typename TImage::IndexType IndexType;
		typedef typename TImage::PixelType val;

		int m_WaveletDepth;
		val** _data;
		unsigned int _sizeData;

		HaarWaveletTransformImageFilter (const Self &);
		void operator = (const Self &);

		/**
		 * \brief Metodo che effettua la trasformata wavelet di un array
		 *
		 * \param array Array di cui calcolare la trasformata wavelet
		 * \param size	Dimensione dell'array
		 */
		inline void do_haar(val * array, int size){
			val * avg = new val[size];
			val * diff = new val[size];
			int i, count = size;

				for (i=0; i<count/2; i++) {
					avg[i] = (array[2*i] + array[2*i + 1]) / 2;
					diff[i] = array[2*i] - avg[i];
				}

				for (i=0; i<count/2; i++) {
					array[i] = avg[i];
					array[i + count/2] = diff[i];
			//	}
			}
			delete[] avg;
			delete[] diff;
		}

		/**
		 * \brief Metodo che effettua la trasformata wavelet di una riga dell'immagine
		 *
		 * \param data	Matrice dei pixel dell'immagine da trasformare
		 * \param row	Indice della riga da trasformare
		 * \param size	Dimensione della riga da trasformare
		 */
		inline void transform_row(val ** data, int row, int size){
			do_haar(data[row], size);
		}

		/**
		 * \brief Metodo che effettua la trasformata wavelet di una colonna dell'immagine
		 *
		 * \param data	Matrice dei pixel dell'immagine da trasformare
		 * \param col	Indice della colonna da trasformare
		 * \param size	Dimensione della colonna da trasformare
		 */
		inline void transform_col(val ** data, int col, int size){
			val * coldata = new val[size];
			int i;

			for (i=0; i<size; i++)
				coldata[i] = data[i][col];

			do_haar(coldata, size);

			for (i=0; i<size; i++)
				data[i][col] = coldata[i];

			delete[] coldata;
		}
	};

    /**
	 * \class HaarBackwardWaveletTransformImageFilter
	 * \brief Filtro ITK che effettua l'antitrasformata wavelet dell'immagine di ingresso utilizzando i filtri di Haar
	 */ 
	template < class TImage >
	class HaarBackwardWaveletTransformImageFilter : public ImageToImageFilter < TImage, TImage > {

	public:
		typedef HaarBackwardWaveletTransformImageFilter Self;
		typedef ImageToImageFilter < TImage, TImage > Superclass;

		typedef SmartPointer < Self > Pointer;
		typedef const SmartPointer < Self > ConstPointer;

		itkNewMacro(Self);

		/**
		 * \brief Setta il livello di dettaglio della decomposizione multilivello
		 */
		void SetWaveletDepth (int value) { 
			m_WaveletDepth = value - 1; 
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
		 * \brief Metodo che coordina l'antitrasformata wavelet dell'immagine e alloca lo spazio in memoria per contenere l'immagine d'uscita
		 */
		inline void GenerateData() {
			ImageConstPointer  inputImage  = this->GetInput();
			ImagePointer       outputImage = this->GetOutput();

			RegionType outputRegion = outputImage->GetLargestPossibleRegion();

			outputImage->SetRegions( outputRegion );
			outputImage->Allocate();

			TImage::SizeType size = inputImage->GetLargestPossibleRegion().GetSize();
			TImage::IndexType pixelIndex;

			val ** data = new val*[size[0]];
			for (int i=0; i < size[0]; i++)
				data[i] = new val[size[1]];

			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						data[i][j] = inputImage->GetPixel(pixelIndex);
					}
				}
			int n,n2;
			for (n=size[1]/((double)pow((double)2,m_WaveletDepth)),n2=size[0]/((double)pow((double)2,m_WaveletDepth)); n<=size[1]||n2<=size[0]; n*=2,n2*=2) 
			{
				for (int i=0; i<n; i++)
					untransform_row(data, i, n);
				for (int i=0; i<n2; i++)
					untransform_col(data, i, n2);
			}
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						outputImage->SetPixel(pixelIndex, data[i][j]);
					}
				}
			}
			for (int i=0; i<size[0]; i++)
				delete[] data[i];
			delete[] data;
		}

	protected:

		/**
		 * \brief Costruttore protetto del filtro
		 *
		 * Setta il livello di dettaglio di default
		 */
		HaarBackwardWaveletTransformImageFilter () {
			m_WaveletDepth = 1;
		};

		/**
		 * \brief Distruttore virtuale protetto
		 */
		virtual ~HaarBackwardWaveletTransformImageFilter () {};

	private:
		typedef typename TImage::Pointer ImagePointer;
		typedef typename TImage::ConstPointer ImageConstPointer;
		typedef typename TImage::RegionType RegionType;
		typedef typename TImage::SizeType SizeType;
		typedef typename TImage::IndexType IndexType;
		typedef typename TImage::PixelType val;

		int m_WaveletDepth;

		HaarBackwardWaveletTransformImageFilter (const Self &);
		void operator = (const Self &);

		/**
		 * \brief Metodo che effettua l'antitrasformata wavelet di un array
		 *
		 * \param array Array da antitrasformare
		 * \param size	Dimensione dell'array
		 */
		inline void undo_haar(val * array, int size) {
			val * tmp = new val[size];
			int i, count = size;

				for (i=0; i<count/2; i++) {
					tmp[2*i]     = array[i] + array[i + count/2];
					tmp[2*i + 1] = array[i] - array[i + count/2];
				}

				for (i=0; i<count; i++)
					array[i] = tmp[i];
			
			delete[] tmp;
		}

		/**
		 * \brief Metodo che effettua l'antitrasformata di una riga dell'immagine in ingresso
		 *
		 * \param data	Matrice dei pixel dell'immagine da antitrasformare
		 * \param row	Indice della riga da antitrasformare
		 * \param size	Dimensione della riga da antitrasformare
		 */
		inline void untransform_row(val ** data, int row, int size) {
			undo_haar(data[row], size);
		}

		/**
		 * \brief Metodo che effettua l'antitrasformata di una colonna dell'immagine in ingresso
		 *
		 * \param data	Matrice dei pixel dell'immagine da antitrasformare
		 * \param col	Indice della colonna da antitrasformare
		 * \param size	Dimensione della colonna da antitrasformare
		 */
		inline void untransform_col(val ** data, int col, int size) {
			val * coldata = new val[size];
			int i;

			for (i=0; i<size; i++)
				coldata[i] = data[i][col];

			undo_haar(coldata, size);

			for (i=0; i<size; i++)
				data[i][col] = coldata[i];
			delete[] coldata;
		}
	};


	/**
	 * \class DaubNWaveletTransformImageFilter
	 * \brief Filtro ITK che effettua la trasformata wavelet dell'immagine di ingresso utilizzando i filtri di Daubechies DN
	 */
	template < class TImage >
	class DaubNWaveletTransformImageFilter : public ImageToImageFilter < TImage, TImage > {

	public:
		typedef DaubNWaveletTransformImageFilter Self;
		typedef ImageToImageFilter < TImage, TImage > Superclass;
		typedef SmartPointer < Self > Pointer;
		typedef const SmartPointer < Self > ConstPointer;
		

		itkNewMacro(Self);

		/**
		 * \brief Setta il livello di dettaglio della decomposizione multilivello
		 *
		 * \param value Livello di dettaglio desiderato
		 */
		inline void SetWaveletDepth (int value) { 
			m_WaveletDepth = value - 1; 
		}
		inline void SetWaveletType (int num) { 
			DaubType= num; 
		}
		/**
		 * \brief Metodo che prepara l'immagine d'uscita, necessaria per la corretta esecuzione nella pipeline di ITK
		 */
		inline void GenerateOutputInformation(){
				Superclass::GenerateOutputInformation();

				ImageConstPointer inputImage  = this->GetInput();
				ImagePointer      outputImage = this->GetOutput();

				outputImage->SetLargestPossibleRegion( inputImage->GetLargestPossibleRegion() );
				outputImage->SetSpacing( inputImage->GetSpacing() );
				outputImage->SetOrigin( inputImage->GetOrigin() );
				outputImage->SetDirection( inputImage->GetDirection() );
		}

		/**
		 * \brief Metodo che coordina la trasformata wavelet dell'immagine e alloca lo spazio in memoria per contenere l'immagine d'uscita
		 */
		inline void GenerateData(){
			ImageConstPointer  inputImage  = this->GetInput();
			ImagePointer       outputImage = this->GetOutput();

			RegionType outputRegion = outputImage->GetLargestPossibleRegion();

			outputImage->SetRegions( outputRegion );
			outputImage->Allocate();

			TImage::SizeType size = inputImage->GetLargestPossibleRegion().GetSize();
			_sizeData = size[0];
			TImage::IndexType pixelIndex;

			_data = new val * [size[0]];
			for (int i=0; i<size[0]; i++)
				_data[i] = new val[size[1]];

			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						_data[i][j] = inputImage->GetPixel(pixelIndex);
					}
				}
				
				int n,n2;
				//questo ciclo serve per rendere multilivello la trasformata
				for (n=size[1],n2=size[0]; n>=size[1]/((double)pow((double)2,m_WaveletDepth))||n2>=size[0]/((double)pow((double)2,m_WaveletDepth)); n/=2,n2/=2) 
				{
					for (int i=0; i<n; i++)
						transform_row(_data, i, n);
					for (int i=0; i<n2; i++)
						transform_col(_data, i, n2);
				}

				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						outputImage->SetPixel(pixelIndex, _data[i][j]);
					}
				}
			}
		}

	protected:

		/**
		 * \brief Costruttore protetto del filtro
		 *
		 * Serve per settare il livello di dettaglio al valore di default
		 * e per inizializzare i coefficienti delle funzioni di scala e wavelet
		 */
		DaubNWaveletTransformImageFilter () {
			m_WaveletDepth = 1;
		};

		/**
		 * \brief Distruttore virtuale del filtro
		 */
		virtual ~DaubNWaveletTransformImageFilter () {
			if(_sizeData != 0) {
				for (int i=0; i<_sizeData; i++)
					delete[] _data[i];
				delete[] _data;
			}
		};
	
	private:
		typedef typename TImage::Pointer ImagePointer;
		typedef typename TImage::ConstPointer ImageConstPointer;
		typedef typename TImage::RegionType RegionType;
		typedef typename TImage::SizeType SizeType;
		typedef typename TImage::IndexType IndexType;
		typedef typename TImage::PixelType val;
	

		int m_WaveletDepth;
		int DaubType;
		val** _data;
		unsigned int _sizeData;
		waveletfilter wfltr;	//struttura contenente il filtro desiderato


		DaubNWaveletTransformImageFilter (const Self &);
		void operator = (const Self &);

		/**
		 * \brief Metodo che effettua la trasformata wavelet di un array
		 *
		 * \param array Array di cui calcolare la trasformata wavelet
		 * \param size	Dimensione dell'array
		 */
		/* wfltr_convolve -- perform one convolution of a (general) wavelet transform */
		
		
		inline void do_daubN( val * aIn, const int n )	{
	
			switch (DaubType) {
				case 4:		{
					wfltr.cH = cHDaubechies_4;
					wfltr.cHtilde = cHDaubechies_4;
					wfltr.nH = N_ELEM(cHDaubechies_4);
					wfltr.nHtilde = N_ELEM(cHDaubechies_4);
					wfltr.offG=1;
					wfltr.offGtilde=1;
					wfltr.offH=1;
					wfltr.offHtilde=1;
						
						//cHDaubechies_4,
						//N_ELEM(cHDaubechies_4),
						/* offsets di H, G, Htilde, e Gtilde, rispettivamente */
						
				};
				break;

				case 8:		{
					wfltr.cH = cHDaubechies_8;
					wfltr.cHtilde = cHDaubechies_8;
					wfltr.nH = N_ELEM(cHDaubechies_8);
					wfltr.nHtilde = N_ELEM(cHDaubechies_8);
					wfltr.offG=1;
					wfltr.offGtilde=1/*5*/;
					wfltr.offH=1;
					wfltr.offHtilde=1/*5*/;
						//cHDaubechies_8,
						//N_ELEM(cHDaubechies_8),
						/* offsets di H, G, Htilde, e Gtilde, rispettivamente */
						
				};
				break;

				case 20:
					{
					wfltr.cH = cHDaubechies_20;
					wfltr.cHtilde = cHDaubechies_20;
					wfltr.nH = N_ELEM(cHDaubechies_20);
					wfltr.nHtilde = N_ELEM(cHDaubechies_20);
					wfltr.offG=1/*2*/;
					wfltr.offGtilde=1/*16*/;
					wfltr.offH=1/*2*/;
					wfltr.offHtilde=1/*16*/;
						//cHDaubechies_20,
						//N_ELEM(cHDaubechies_20),
						/* offsets di H, G, Htilde, e Gtilde, rispettivamente */
					
				};
				break;
			}

			int incA=1;			/* in: spacing of elements in aIn[] and aXf[] */
			int nDiv2 = n / 2;
			int iA, jG, jH, i;
			double sum;
			bool flip;
			double* aTmp1D = new double[n];
			
			
			/*
			*	According to Daubechies:
			*
			*	H is the analyzing smoothing filter.
			*	G is the analyzing detail filter.
			*	Htilde is the reconstruction smoothing filter.
			*	Gtilde is the reconstruction detail filter.
			*/

			/* the reconstruction detail filter is the mirror of the analysis smoothing filter */
			int nGtilde = wfltr.nH;

			/* the analysis detail filter is the mirror of the reconstruction smoothing filter */
			int nG = wfltr.nHtilde;

			/*
			*	A single step of the analysis is summarized by:
			*		aTmp1D[0..n/2-1] = H * a (smooth component)
			*		aTmp1D[n/2..n-1] = G * a (detail component)
			*/
			for (i = 0; i < nDiv2; i++) {
				/*
				*	aTmp1D[0..nDiv2-1] contains the smooth components.
				*/
				sum = 0.0;
				for (jH = 0; jH < wfltr.nH; jH++) {
					/*
					*	Each row of H is offset by 2 from the previous one.
					*
					*	We assume our data is periodic, so we wrap the aIn[] values
					*	if necessary.  If we have more samples than we have H
					*	coefficients, we also wrap the H coefficients.
					*/
					iA = MOD(2 * i + jH - wfltr.offH, n);
					sum += wfltr.cH[jH] * aIn[incA * iA];
				}
				aTmp1D[i] = sum;

				/*
				*	aTmp1D[nDiv2..n-1] contains the detail components.
				*/
				sum = 0.0;
				flip = TRUE;
				for (jG = 0; jG < nG; jG++) {
					/*
					*	We construct the G coefficients on-the-fly from the
					*	Htilde coefficients.
					*
					*	Like H, each row of G is offset by 2 from the previous
					*	one.  As with H, we also allow the coefficents of G to
					*	wrap.
					*
					*	Again as before, the aIn[] values may wrap.
					*/
					iA = MOD(2 * i + jG - wfltr.offG, n);
					if (flip)
						sum -= wfltr.cHtilde[nG - 1 - jG] * aIn[incA * iA];
					else
						sum += wfltr.cHtilde[nG - 1 - jG] * aIn[incA * iA];
					flip = !flip;
				}
				aTmp1D[nDiv2 + i] = sum;
			}
			for (int k = 0; k < n; k++) {
				aIn[k] = aTmp1D[k];
			}
			delete [] aTmp1D;
		}
	
	
		/**
		 * \brief Metodo che effettua la trasformata wavelet di una riga dell'immagine
		 *
		 * \param data	Matrice dei pixel dell'immagine da trasformare
		 * \param row	Indice della riga da trasformare
		 * \param size	Dimensione della riga da trasformare
		 */
		inline void transform_row(val ** data, int row, int n){ //al posto di n prima c'era size
			// int n;
		//questo ciclo serve per rendere multilivello la trasformata
          //   for (n=size; n>=size/((double)pow((double)2,m_WaveletDepth)); n/=2)
				do_daubN(data[row], n);
		}

		/**
		 * \brief Metodo che effettua la trasformata wavelet di una colonna dell'immagine
		 *
		 * \param data	Matrice dei pixel dell'immagine da trasformare
		 * \param col	Indice della colonna da trasformare
		 * \param size	Dimensione della colonna da trasformare
		 */
		inline void transform_col(val ** data, int col, int size){
			val * coldata = new val[size];
			int i;
			// questo ciclo mi trasforma il vettore colonna in vettore riga
			for (i=0; i<size; i++)
				coldata[i] = data[i][col];
			// il ciclo serve per applicare iterativamente la trasformata unidimensionale 
			// dimezzando di volta in volta la dimensione del vettore
		//	for (n=size; n>=size/((double)pow((double)2,m_WaveletDepth)); n/=2)
				do_daubN(coldata, size);

			for (i=0; i<size; i++)
				data[i][col] = coldata[i];
			
			delete[] coldata;
		}
	};

/**
	 * \class DaubBackwardWaveletTransformImageFilter
	 * \brief Filtro ITK che effettua l'antitrasformata wavelet dell'immagine di ingresso utilizzando i filtri di Daubechies D4
	 */ 
	template < class TImage >
	class DaubNBackwardWaveletTransformImageFilter : public ImageToImageFilter < TImage, TImage > {

	public:
		typedef DaubNBackwardWaveletTransformImageFilter Self;
		typedef ImageToImageFilter < TImage, TImage > Superclass;

		typedef SmartPointer < Self > Pointer;
		typedef const SmartPointer < Self > ConstPointer;

		itkNewMacro(Self);

		/**
		 * \brief Setta il livello di dettaglio della decomposizione multilivello
		 */
		void SetWaveletDepth (int value) { 
			m_WaveletDepth = value - 1; 
		}
		inline void SetWaveletType (int num) { 
			DaubType= num; 
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
		 * \brief Metodo che coordina l'antitrasformata wavelet dell'immagine e alloca lo spazio in memoria per contenere l'immagine d'uscita
		 */
		inline void GenerateData() {
			ImageConstPointer  inputImage  = this->GetInput();
			ImagePointer       outputImage = this->GetOutput();

			RegionType outputRegion = outputImage->GetLargestPossibleRegion();

			outputImage->SetRegions( outputRegion );
			outputImage->Allocate();

			TImage::SizeType size = inputImage->GetLargestPossibleRegion().GetSize();
			TImage::IndexType pixelIndex;

			val ** data = new val*[size[0]];
			for (int i=0; i < size[0]; i++)
				data[i] = new val[size[1]];

			for (int z=0; z<size[2]; z++) {
				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						data[i][j] = inputImage->GetPixel(pixelIndex);
					}
				}

				int n,n2;
			for (n=size[1]/((double)pow((double)2,m_WaveletDepth)),n2=size[0]/((double)pow((double)2,m_WaveletDepth)); n<=size[1]||n2<=size[0]; n*=2,n2*=2) 
			{
				for (int i=0; i<n; i++)
					untransform_row(data, i, n);
				for (int i=0; i<n2; i++)
					untransform_col(data, i, n2);
			}

				for (int i=0; i<size[0]; i++) {
					for (int j=0; j<size[1]; j++) {
						pixelIndex[0] = i;
						pixelIndex[1] = j;
						pixelIndex[2] = z;
						outputImage->SetPixel(pixelIndex, data[i][j]);
					}
				}
			}
			for (int i=0; i<size[0]; i++)
				delete[] data[i];
			delete[] data;
		}

	protected:

		/**
		 * \brief Costruttore protetto del filtro
		 *
		 * Setta il livello di dettaglio di default
		 */
		DaubNBackwardWaveletTransformImageFilter () {
			m_WaveletDepth = 1;
		};

		/**
		 * \brief Distruttore virtuale protetto
		 */
		virtual ~DaubNBackwardWaveletTransformImageFilter () {};

	private:
		typedef typename TImage::Pointer ImagePointer;
		typedef typename TImage::ConstPointer ImageConstPointer;
		typedef typename TImage::RegionType RegionType;
		typedef typename TImage::SizeType SizeType;
		typedef typename TImage::IndexType IndexType;
		typedef typename TImage::PixelType val;

		int m_WaveletDepth;
		int DaubType;
		waveletfilter wfltr;	//struttura contenente il filtro desiderato
		DaubNBackwardWaveletTransformImageFilter (const Self &);
		void operator = (const Self &);

		/**
		 * \brief Metodo che effettua l'antitrasformata wavelet di un array
		 *
		 * \param array Array da antitrasformare
		 * \param size	Dimensione dell'array
		 */

		inline void undo_daubN(val * aIn, const int n)	
			{
			switch (DaubType) 
				{
				case 4:		
					{
					wfltr.cH = cHDaubechies_4;
					wfltr.cHtilde = cHDaubechies_4;
					wfltr.nH = N_ELEM(cHDaubechies_4);
					wfltr.nHtilde = N_ELEM(cHDaubechies_4);
					wfltr.offG=1;
					wfltr.offGtilde=1;
					wfltr.offH=1;
					wfltr.offHtilde=1;
						
						//cHDaubechies_4,
						//N_ELEM(cHDaubechies_4),
						/* offsets di H, G, Htilde, e Gtilde, rispettivamente */
						
					};
					break;

					case 8:		
						{
						wfltr.cH = cHDaubechies_8;
						wfltr.cHtilde = cHDaubechies_8;
						wfltr.nH = N_ELEM(cHDaubechies_8);
						wfltr.nHtilde = N_ELEM(cHDaubechies_8);
						wfltr.offG=1;
						wfltr.offGtilde=1 /*5*/;
						wfltr.offH=1;
						wfltr.offHtilde=1 /*5*/;
						//cHDaubechies_8,
						//N_ELEM(cHDaubechies_8),
						/* offsets di H, G, Htilde, e Gtilde, rispettivamente */
						
						};
					break;

					case 20:
						{
						wfltr.cH = cHDaubechies_20;
						wfltr.cHtilde = cHDaubechies_20;
						wfltr.nH = N_ELEM(cHDaubechies_20);
						wfltr.nHtilde = N_ELEM(cHDaubechies_20);
						wfltr.offG=1 /*2*/;
						wfltr.offGtilde=1 /*16*/;
						wfltr.offH=1 /*2*/;
						wfltr.offHtilde=1 /*16*/;
						//cHDaubechies_20,
						//N_ELEM(cHDaubechies_20),
						/* offsets di H, G, Htilde, e Gtilde, rispettivamente */
					
						};
					break;
					}
			
			
				int incA=1;			/* in: spacing of elements in aIn[] and aXf[] */
				int nDiv2 = n / 2;
				int iA, iHtilde, iGtilde, j, i;
				bool flip;
				double* aTmp1D = new double[n];
				
				/* the reconstruction detail filter is the mirror of the analysis smoothing filter */
				int nGtilde = this->wfltr.nH;

				/* the analysis detail filter is the mirror of the reconstruction smoothing filter */
				int nG = wfltr.nHtilde;
					/*
					*	The inverse transform is a little trickier to do efficiently.
					*	A single step of the reconstruction is summarized by:
					*
					*		aTmp1D = Htilde^t * aIn[incA * (0..n/2-1)]
					*				+ Gtilde^t * aIn[incA * (n/2..n-1)]
					*
					*	where x^t is the transpose of x.
					*/
				for (i = 0; i < n; i++) 
					{
					aTmp1D[i] = 0.0;	/* necessary */
					}
				for (j = 0; j < nDiv2; j++) 
					{
					for (iHtilde = 0; iHtilde < wfltr.nHtilde; iHtilde++) 
						{
						
						/*
						*	Each row of Htilde is offset by 2 from the previous one.
						*/
						
						iA = MOD(2 * j + iHtilde - wfltr.offHtilde, n);
						aTmp1D[iA] += wfltr.cHtilde[iHtilde] * aIn[incA * j];
						}
					flip = TRUE;
					for (iGtilde = 0; iGtilde < nGtilde; iGtilde++) 
						{

						/*
					*	As with Htilde, we also allow the coefficents of Gtilde,
					*	which is the mirror of H, to wrap.
					*
					*	We assume our data is periodic, so we wrap the aIn[] values
					*	if necessary.  If we have more samples than we have Gtilde
					*	coefficients, we also wrap the Gtilde coefficients.
					*/
						iA = MOD(2 * j + iGtilde - wfltr.offGtilde, n);
						if (flip) 
							{
							aTmp1D[iA] -= wfltr.cH[nGtilde - 1 - iGtilde]* aIn[incA * (j + nDiv2)];
							} 
						else 
							{
							aTmp1D[iA] += wfltr.cH[nGtilde - 1 - iGtilde]* aIn[incA * (j + nDiv2)];
							}
						flip = !flip;
						}
					}
				
				for (int k = 0; k < n; k++)
					{
					aIn[incA * k] = aTmp1D[k];
					}
				delete[] aTmp1D;
				}
		
	
		/**
		 * \brief Metodo che effettua l'antitrasformata di una riga dell'immagine in ingresso
		 *
		 * \param data	Matrice dei pixel dell'immagine da antitrasformare
		 * \param row	Indice della riga da antitrasformare
		 * \param size	Dimensione della riga da antitrasformare
		 */
		inline void untransform_row(val ** data, int row, int size) {
		//	 int n;
		//	 for (n=size/((double)pow((double)2,m_WaveletDepth)); n<=size; n*=2)
				undo_daubN(data[row], size);
		}

		/**
		 * \brief Metodo che effettua l'antitrasformata di una colonna dell'immagine in ingresso
		 *
		 * \param data	Matrice dei pixel dell'immagine da antitrasformare
		 * \param col	Indice della colonna da antitrasformare
		 * \param size	Dimensione della colonna da antitrasformare
		 */
		inline void untransform_col(val ** data, int col, int size) {
			val * coldata = new val[size];
			int i;

			for (i=0; i<size; i++)
				coldata[i] = data[i][col];
			
			undo_daubN(coldata, size);

			for (i=0; i<size; i++)
				data[i][col] = coldata[i];
			
			delete[] coldata;
		}
	};

}
#endif _itkWavelet_h_