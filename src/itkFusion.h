/**
 * \file itkFusion.h
 * \brief File per la gestione della fusione con la tecnica della media pesata
 * \author ICAR-CNR Napoli
 */

#ifndef _itkFusion_h_
#define _itkFusion_h_

#include <itkNumericTraits.h>
#include <itkBinaryFunctorImageFilter.h>

/** \namespace itk */
namespace itk {
	namespace Functor {

		/**
		 * \class ApplyClut
		 * \brief Functor ITK che trasforma un pixel applicando una CLUT
		 *
		 * Questo functor effettua una trasformazione del valore di intensità luminosa di un filtro applicando una CLUT precalcolata, il tipo di CLUT dipende dal canale scelto per il pixel
		 */
		template < class TInput >
		class ApplyClut {

		public:

			/**
			 * \brief Imposta il calane per l'applicazione della CLUT
			 *
			 * La CLUT è costituita da tre canali RGB, impostando il canale desiderato è possibile trasformare il pixel utilizzando la CLUT del canale corrispondente
			 * 
			 * \param value Indice del canale. R=0, G=1, B=2
			 */
			inline void SetChannel(int value) {
				m_channel = value;
				GenerateCLUT();
			}

			/**
			 * \brief Operatore che effettua la trasformazione del pixel
			 *
			 * \param A Valore di intensità del pixel da trasformare
			 */
			inline TInput operator () (const TInput & A) {
				if (m_channel == 1)
					return _r[A];
				else if (m_channel == 2)
					return _g[A];
				else if (m_channel == 3)
					return _b[A];
				return 0;
			}

		private:
			int m_channel;

			/** Tabella CLUT per il canale R */
			int _r [256];

			/** Tabella CLUT per il canale G */
			int _g [256];

			/** Tabella CLUT per il canale B */
			int _b [256];

			/**
			 * \brief Metodo che genera le tabelle CLUT
			 *
			 * Le tabelle CLUT, viste nel loro insieme effettuano una trasformazione da scala di grigio a colori con gradazioni che partono dal <i>nero</i> (0,0,0) passando per <i>rosso</i> (255,0,0), <i>giallo</i> (255,255,0) e arrivando a <i>bianco</i> (255,255,255)
			 * E' da notare che i valori medi della tabella CLUT completa sono uguali al corrispondente valore in scala di grigio. 
			 */
			inline void GenerateCLUT() {
				/*for (int i=0; i<256; i++) {
					_r[i] = min(255, i*3);
					_g[i] = min(255, max(0, (i-85)*3));
					_b[i] = min(255, max(0, (i-170)*3));
						
				}*/
				/*for (int i=0; i<256; i++) {
					_r[i] = min(255, i*3);
					_g[i] = min(255, max(0, (i-85)*3));
					_b[i] = min(255, max(0, (i-170)*3));
					
				}*/
				for (int i=0; i<128; i++) {
					_r[i] = i*2;
					_g[i] = 0;
					_b[i] = 0;
						
				}
				for (int i=128; i<192; i++) {
					_r[i] = 255;
					_g[i] = (i-128)*4;
					_b[i] = 0;
						
				}
				for (int i=192; i<256; i++) {
					_r[i] = 255;
					_g[i] = 255;
					_b[i] = (i-192)*4;
				}
			}
		};

		/**
		 * \class WeightBackFront
		 * \brief Implementa l'operazione di combinazione aritmetica dei pixel utilizzando la media pesata
		 */
		template < class TInput >
		class WeightBackFront {

		public:
			
			/**
			 * \brief Tipo per la gestione dei pesi
			 */
			typedef typename NumericTraits < TInput >::RealType InputRealType;

			/**
			 * \brief Operatore che implementa l'operazione di fusione
			 * 
			 * \param A Valore del pixel per la prima immagine
			 * \param B Valore del pixel per la seconda immagine
			 */
			inline TInput operator () (const TInput & A, const TInput & B) {
				return fuseOperation(InputRealType(A), InputRealType(B));
			}
			
			/**
			 * \brief Setta il peso per il primo pixel
			 *
			 * \param value Valore del peso
			 */
			inline void SetPesoA(const InputRealType & value) {
				m_PesoA = value;
			}
			
			/**
			 * \brief Setta il peso per il secondo pixel
			 *
			 * \param value Valore del peso
			 */
			inline void SetPesoB(const InputRealType & value) {
				m_PesoB = value;
			}

		private:
			InputRealType m_PesoA, m_PesoB;
		
			/**
			 * \brief Metodo che implementa la media pesata dei pixel in ingresso
			 *
			 * \param a Valore del pixel della prima immagine
			 * \param b valore del pixel della seconda immagine
			 */
			inline TInput fuseOperation (InputRealType a, InputRealType b) {
				InputRealType a1 = m_PesoA * a;
				InputRealType b1 = m_PesoB * b;
				InputRealType o = a1;
				if ((a1+b1) > NumericTraits<TInput>::max() )
					o = NumericTraits<TInput>::max();
				else
					o += b1;
				return static_cast <TInput> (o);
			}
		};	

		/**
		 * \class CLUTFuser
		 * \briefFunctor ITK che implementa la fusione di due pixel con applicazione della CLUT
		 *
		 * L'applicazione della CLUT viene effettuata solo sul valore del pixel della seconda immagine
		 * La CLUT applicata è costruita come per ApplyClut
		 */
		template <class TInput>
		class CLUTFuser {

		public:

			/** Tipo del pixel in uscita */
			typedef RGBPixel<TInput> OutputType;
			
			typedef typename NumericTraits < TInput >::RealType InputRealType;//luca

			/** Costruttore del Functor. Costruisce la CLUT */
			CLUTFuser() {
				GenerateCLUT();
			}
			
			/**
			 * \brief Operatore che effettua la fusione con applicazione della CLUT
			 *
			 * \param A Pixel della prima immagine
			 * \param B Pixel della seconda imamgine
			 */
			inline OutputType operator () (const TInput & A, const TInput & B) {
				OutputType clutPixel;
				
				clutPixel.Set((m_PesoA*A+m_PesoB*_r[B]), (m_PesoA*A+m_PesoB*_g[B]), (m_PesoA*A+m_PesoB*_b[B]));
				return clutPixel;
			}
			/**
			 * \brief Setta il peso per il primo pixel
			 *
			 * \param value Valore del peso
			 */
			
			//luca: inserire i pesi nella media pesata con clut

			inline void SetPesoA(const InputRealType & value) {
				m_PesoA = value;
			}
			
			/**
			 * \brief Setta il peso per il secondo pixel
			 *
			 * \param value Valore del peso
			 */
			inline void SetPesoB(const InputRealType & value) {
				m_PesoB = value;
			}
		private:
			
			InputRealType m_PesoA, m_PesoB;//luca

			/** Tabella CLUT per il canale R */
			int _r [256];

			/** Tabella CLUT per il canale G */
			int _g [256];

			/** Tabella CLUT per il canale B */
			int _b [256];
			
			/** Metodo che genera la CLUT. Vedi ApplyClut::GenerateCLUT */
			void GenerateCLUT() {
				for (int i=0; i<256; i++) {
					_r[i] = min(255, i*3);
					_g[i] = min(255, max(0, (i-85)*3));
					_b[i] = min(255, max(0, (i-170)*3));
					
				}
			}
		};
	}

	/**
	 * \class FuseImageFilter
	 * \brief Classe che effettua la fusione con combinazione aritmetica
	 */
	template < class TInputImage >
	class ITK_EXPORT FuseImageFilter : public BinaryFunctorImageFilter < TInputImage, TInputImage, TInputImage, Functor::WeightBackFront < ITK_TYPENAME TInputImage::PixelType > > {

	public:
		typedef FuseImageFilter Self;
		typedef BinaryFunctorImageFilter < TInputImage, TInputImage, TInputImage, Functor::WeightBackFront < ITK_TYPENAME TInputImage::PixelType > > Superclass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		itkNewMacro(Self);

	protected:

		///	Costruttore protetto
		FuseImageFilter() {};

		/// Distruttore virtuale protetto
		virtual ~FuseImageFilter() {}

	private:
		FuseImageFilter(const Self &);
		void operator = (const Self &);	
	};

	/**
	 * \class CLUTFuseImageFilter
	 * \brief Classe che effettua la combinazione aritmetica delle immagini con applicazione della CLUT sulla seconda immagine
	 */
	template < typename TInputImage, typename TOutputImage=Image < RGBPixel < ITK_TYPENAME TInputImage::PixelType >, ::itk::GetImageDimension < TInputImage > ::ImageDimension > >
	class ITK_EXPORT CLUTFuseImageFilter : public BinaryFunctorImageFilter < TInputImage, TInputImage, TOutputImage, Functor::CLUTFuser < ITK_TYPENAME TInputImage::PixelType > > {

	public:
		typedef CLUTFuseImageFilter Self;
		typedef BinaryFunctorImageFilter < TInputImage, TInputImage, TOutputImage, Functor::CLUTFuser < ITK_TYPENAME TInputImage::PixelType > > Superclass;
		typedef SmartPointer <Self> Pointer;
		typedef SmartPointer <const Self> ConstPointer;
		
		itkNewMacro(Self);
	
	protected:

		/// Costruttore protetto
		CLUTFuseImageFilter() {};

		/// Distruttore virtuale protetto
		virtual ~CLUTFuseImageFilter() {};
	
	private:
		CLUTFuseImageFilter(const Self &);
		void operator = (const Self &);
	};
}

#endif _itkFusion_h_