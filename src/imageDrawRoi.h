/**
 * \file imageDrawRoi.h
 * \brief Creazione ROI
 * \author ICAR-CNR Napoli
 */

#ifndef _imageDrawRoi_h_
#define _imageDrawRoi_h_

#include <vtkImageData.h>  
#include <vtkImageInPlaceFilter.h>  
#include <vtkObjectFactory.h>          
#include <vtkPoints.h>
#include <vtkMath.h>

#ifndef _point_h_
#include "point.h" 
#include <vtkInformation.h> 
#include <vtkInformationVector.h> 
#endif  

/**
* \def ROI_SHAPE_POLYGON  
* \brief Definizione identificativo ROI_SHAPE_POLYGON
*/
#define ROI_SHAPE_POLYGON 0

/**
* \def ROI_SHAPE_RECTANGLE  
* \brief Definizione identificativo ROI_SHAPE_RECTANGLE
*/
#define ROI_SHAPE_RECTANGLE 1 

/**
 * \class imageDrawRoi
 * \brief Classe per la creazione delle ROI
 *
 * Questa classe è una specializzazione della classe VTK vtkImageInPlaceFilter, 
 * filtro che opera direttamente sull'immagine di input e nasconde la complessità 
 * della pipeline di esecuzione
 *
 */
class imageDrawRoi: public vtkImageInPlaceFilter  {       
public: 
	/** Costruttore */
	imageDrawRoi(); 

	/** Distruttore */
	~imageDrawRoi(); 

	/**
	 * \fn void startSelectBox(int x, int y)
	 * \brief Inserisce il primo punto del box di selezione
	 * \param x Coordinata x del primo punto
	 * \param y Coordinata y del primo punto
	 */
	void startSelectBox(int, int);
	
	/**
	 * \fn void dragSelectBox(int x, int y)
	 * \brief Disegna il box di selezione
	 * \param x Coordinata x del punto
	 * \param y Coordinata y del punto
	 */
	void dragSelectBox(int, int);

	/**
	 * \fn void endSelectBox(int x, int y)
	 * \brief Inserisce l'ultimo punto del box di selezione e seleziona i punti interni al box
	 * \param x Coordinata x dell'ultimo punto
	 * \param y Coordinata y dell'ultimo punto
	 */
	void endSelectBox(int, int);

	/**
	 * \fn void startRectangle(int x, int y)
	 * \brief Inserisce il primo punto della ROI rettangolare
	 * \param x Coordinata x del punto
	 * \param y Coordinata y del punto
	 */
	void startRectangle(int, int);

	/**
	 * \fn void dragRectangle(int x, int y)
	 * \brief Disegna la ROI rettangolare
	 * \param x Coordinata x del punto
	 * \param y Coordinata y del punto
	 */
	void dragRectangle(int, int);

	/**
	 * \fn void endRectangle(int x, int y)
	 * \brief Inserisce l'ultimo punto della ROI rettangolare
	 * \param x Coordinata x del punto
	 * \param y Coordinata y del punto
	 */
	void endRectangle(int, int);

	/**
	 * \fn void moveRectangle(int x, int y)
	 * \brief Sposta un punto del contorno della ROI rettangolare
	 * \param x Nuova coordinata x del punto selezionato
	 * \param y Nuova Coordinata y del punto selezionato
	 */
	void moveRectangle(int, int);

	/**
	 * \fn void insertPoint(int x, int y)
	 * \brief Inserisce un punto tra due punti adiacenti
	 * \param x Coordinata x del punto da inserire
	 * \param y Coordinata y del punto da inserire
	 */
	void insertPoint(int, int);

	/**
	 * \fn void insertAfterSelectedPoint(int x, int y)
	 * \brief Inserisce un punto dopo un punto selezionato
	 * \param x Coordinata x del punto
	 * \param y Coordinata y del punto
	 */
	void insertAfterSelectedPoint(int, int);

	/**
	 * \fn void moveSelectedPoints(int x, int y)
	 * \brief Sposta tutti i punti selezionati del contorno della ROI
	 * \param x Spostamento x dei punti rispetto al primo punto selezionato
	 * \param y Spostamento y dei punti rispetto al primo punto selezionato
	 */
	void moveSelectedPoints(int, int);

	/**
	 * \fn void deleteSelectedPoints()
	 * \brief Cancella i punti selezionati del contorno della ROI
	 */
	void deleteSelectedPoints();

	/**
	 * \fn void deleteAllPoints()
	 * \brief Cancella tutti i punti del contorno della ROI
	 */
	void deleteAllPoints(); 

	/**
	 * \fn void selectAllPoints()
	 * \brief Seleziona tutti i punti del contorno della ROI
	 */
	void selectAllPoints();

	/**
	 * \fn void deselectAllPoints()
	 * \brief Deseleziona tutti i punti selezionati del contorno della ROI
	 */
	void deselectAllPoints();

	/**
	 * \fn float angle()
	 * \brief Calcolo dell'angolo formato dalla ROI
	 * \return Il valore dell'angolo
	 */
	float angle();

	/**
	 * \fn float area()
	 * \brief Calcolo dell'area della ROI
	 * \return Il valore dell'area
	 */
	float area();

	/**
	 * \fn float volume()
	 * \brief Calcolo del volume della ROI
	 * \return Il valore del volume
	 */
	float volume();

	/**
	 * \fn float lengthTot()
	 * \brief Calcolo della lunghezza del contorno della ROI
	 * \return Il valore della lunghezza del contorno della ROI
	 */
	float lengthTot();

	/**
	 * \fn void setRectangle(int rectangle)
	 * \brief Set dello stato del rettangolo
	 * \param rectangle 0 stato inattivo o in fase di disegno, 1 stato attivo
	 */
	void setRectangle(int);

	/**
	 * \fn void setMinMax(int *outExt)
	 * \brief Set dell'estensione dell'immagine
	 * \param outExt Estensione dell'immagine
	 */
	void setMinMax(int*);

	/**
	 * \fn void setShapeToPolygon()
	 * \brief Set della forma poligonale della ROI
	 */
	void setShapeToPolygon(); 
	
	/**
	 * \fn void setShapeToRectangle()
	 * \brief Set della forma rettangolare della ROI
	 */
	void setShapeToRectangle();

	/**
	 * \fn vtkPoints* getPoints()
	 * \brief Get dei punti del contorno della ROI disegnata
	 * \return I punti del contorno della ROI
	 */
	vtkPoints* getPoints(); 

	/**
	 * \fn vtkSetMacro(_slice, int)
	 * \brief Set della slice corrente
	 * \param _slice Slice corrente
	 */
	vtkSetMacro(_slice, int);

	/**
	 * \fn vtkSetMacro(_pixelSpacingX, double)
	 * \brief Set del pixel spacing lungo x
	 * \param _pixelSpacingX Pixel spacing lungo x.
	 */
	vtkSetMacro(_pixelSpacingX, double);

	/**
	 * \fn vtkSetMacro(_pixelSpacingY, double)
	 * \brief Set del pixel spacing lungo y
	 * \param _pixelSpacingY Pixel spacing lungo y
	 */
	vtkSetMacro(_pixelSpacingY, double);

	/**
	 * \fn vtkSetMacro(_sliceThickness, double)
	 * \brief Set della slice thickness
	 * \param _sliceThickness Slice thickness (distanza tra due slice)
	 */
	vtkSetMacro(_sliceThickness, double);

	/**
	 * \fn vtkSetMacro(_closed, int)
	 * \brief Set della chiusura del poligono
	 * \param _closed Chiusura del poligono: 0 aperto, 1 chiuso
	 */
	vtkSetMacro(_closed, int);

	/**
	 * \fn void vtkSetVector3Macro(_pointColor, float)
	 * \brief Set del colore dei punti del contorno della ROI
	 * \param r Componente rossa
	 * \param g Componente verde
	 * \param b Componente blu
	 */
	vtkSetVector3Macro(_pointColor, float); 

	/**
	 * \fn void vtkSetVector3Macro(_selectedPointColor, float)
	 * \brief Set del colore dei punti selezionati del contorno della ROI
	 * \param r Componente rossa
	 * \param g Componente verde
	 * \param b Componente blu
	 */
	vtkSetVector3Macro(_selectedPointColor, float); 

	/**
	 * \fn vtkSetVector3Macro(_lineColor, float)  
	 * \brief Set del colore delle linee del contorno della ROI
	 * \param r Componente rossa
	 * \param g Componente verde
	 * \param b Componente blu
	 */
	vtkSetVector3Macro(_lineColor, float); 

	/**
	 * \fn 	vtkSetMacro(_pointRadius, int);
	 * \brief Set del raggio dei punti del contorno della ROI
	 * \param _pointRadius Raggio dei punti del contorno della ROI
	 */
	vtkSetMacro(_pointRadius, int);

	/**
	 * \fn vtkGetMacro(_numSelectedPoints, int)
	 * \brief Get del numero dei punti selezionati del contorno della ROI
	 * \return Il numero di punti selezionati del contorno della ROI
	 */
	vtkGetMacro(_numSelectedPoints, int); 

	/**
	 * \fn vtkGetMacro(_numPoints, int)
	 * \brief Get del numero dei punti del contorno della ROI
	 * \return Il numero di punti del contorno della ROI
	 */
	vtkGetMacro(_numPoints, int);  

	/**
	 * \fn vtkGetMacro(_shape, int)
	 * \brief Get della forma della ROI
	 * \return La forma della ROI: poligonale o rettangolare
	 */
	vtkGetMacro(_shape, int);

	/**
	 * \fn int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
	 * \brief Reimplementa il metodo della classe vtkImageInPlaceFilter
	 * \param request Richiesta
	 * \param inputVector Vettore di ingresso
	 * \param outputVector Vettore di uscita
	 * \return Valore intero
	 */
	int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);

protected:

	/** 
	 * \var vtkPoints *_points
	 * \brief Punti del contorno della ROI
	 */
	vtkPoints* _points; 

	/** 
	 * \var point *_firstPoint
	 * \brief Primo punto del contorno della ROI
	 */
	point* _firstPoint;

	/** 
	 * \var point *_lastPoint
	 * \brief Ultimo punto del contorno della ROI
	 */

	point* _lastPoint;
	/** 
	 * \var int _drawSelectBox
	 * \brief Box di selezione: 0 selezione inattiva o in fase di selezione, 1 fine selezione.
	 */

	int _drawSelectBox;
	/** 
	 * \var point _sbox
	 * \brief Primo punto del box di selezione in fase di selezione
	 */

	point _sbox;
	/** 
	 * \var point _sbox1
	 * \brief Primo punto del box di selezione alla fine della selezione
	 */
	point _sbox1;

	/** 
	 * \var point _sbox2
	 * \brief Secondo punto del box di selezione alla fine della selezione
	 */
	point _sbox2;

	/** 
	 * \var int _drawRectangle
	 * \brief Stato del rettangolo: 0 inattivo o in fase di disegno, 1 stato attivo
	 */
	int _drawRectangle;

	/** 
	 * \var point _sRect
	 * \brief Primo punto del rettangolo in fase di disegno
	 */
	point _sRect;

	/** 
	 * \var point _sRect1
	 * \brief Primo punto del rettangolo nello stato attivo
	 */
	point _sRect1;

	/** 
	 * \var point _sRect2
	 * \brief Secondo punto del rettangolo nello stato attivo
	 */
	point _sRect2;

	/** 
	 * \var int _xMin
	 * \brief Estensione minima dell'immagine lungo x
	 */	
	int _xMin;

	/** 
	 * \var int _xMax
	 * \brief Estensione massima dell'immagine lungo x
	 */	
	int _xMax;

	/** 
	 * \var int _yMin
	 * \brief Estensione minima dell'immagine lungo y
	 */	
	int _yMin;

	/** 
	 * \var int _yMax
	 * \brief Estensione massima dell'immagine lungo y
	 */	
	int _yMax;

	/** 
	 * \var int _numPoints
	 * \brief Numero di punti del contorno della ROI
	 */	
	int _numPoints;

	/** 
	 * \var int _numSelectedPoints
	 * \brief Numero di punti selezionati del contorno della ROI
	 */	
	int _numSelectedPoints;

	/** 
	 * \var int _pointRadius
	 * \brief Raggio dei punti del contorno della ROI
	 */	
	int _pointRadius;
	
	/** 
	 * \var int _shape
	 * \brief Forma della ROI: poligonale o rettangolare
	 */	
	int _shape;

	/** 
	 * \var int _closed
	 * \brief Chiusura del poligono: 0 aperto, 1 chiuso
	 */	
	int _closed;

	/** 
	 * \var int _slice
	 * \brief Slice corrente, valore della Z Slice
	 */	
	int _slice;

	/** 
	 * \var double _pixelSpacingX
	 * \brief Pixel spacing lungo x
	 */
	double _pixelSpacingX;

	/** 
	 * \var double _pixelSpacingY
	 * \brief Pixel spacing lungo y.
	 */
	double _pixelSpacingY;

	/** 
	 * \var double _sliceThickness
	 * \brief Slice Thickness (distanza tra due slice)
	 */
	double _sliceThickness;
	
	/** 
	 * \var float _pointColor[3]
	 * \brief Colore dei punti del contorno della ROI
	 */
	float _pointColor[3]; 
	
	/** 
	 * \var float _selectedPointColor[3]
	 * \brief Colore dei punti selezionati del contorno della ROI
	 */
	float _selectedPointColor[3];

	/** 
	 * \var float _lineColor[3]
	 * \brief Colore delle linee del contorno della ROI
	 */
	float _lineColor[3];

	/**
	 * \fn float length(point* mesureA, point* mesureB)
	 * \brief Calcolo della lunghezza del segmento che unisce due punti
	 * \param mesureA Coordinate del primo punto del segmento
	 * \param mesureB Coordinate del secondo punto del segmento
	 * \return Il valore della lunghezza del segmento che unisce due punti
	 */
	float length(point*, point*);

	/**
	 * \fn void convertColor(float *f, long *c)
	 * \brief Converte il colore da RGB (0,1) a byte (0,255)
	 * \param f Colore RGB
	 * \param c Colore in byte
	 */
	void convertColor(float*, long*);
	
	/**
	 * \fn void drawLine(int xx1, int yy1, int xx2, int yy2, long color[3],long *outPtr, int pNxnc, int pNc)
	 * \brief Disegna una linea tra due punti del contorno della ROI
	 * \param xx1 Coordinata x del primo punto della linea del contorno della ROI
	 * \param yy1 Coordinata y del primo punto della linea del contorno della ROI
	 * \param xx2 Coordinata x del secondo punto della linea del contorno della ROI
	 * \param yy2 Coordinata y del secondo punto della linea del contorno della ROI
	 * \param color Colore della linea del contorno della ROI
	 * \param outPtr Puntatore alla posizione nella vtkImageData
	 * \param pNxnc Lunghezza riga
	 * \param pNc Numero di componenti scalari
	 */
	void drawLine(int xx1, int yy1, int xx2, int yy2, long color[3], long *outPtr, int pNxnc, int pNc);

	/**
	 * \fn void drawPoints(vtkImageData *outData, int extent[6])     
	 * \brief Disegna i punti del contorno della ROI sull'immagine VTK
	 * \param outData Immagine VTK in uscita
	 * \param extent Estensione dell'immagine VTK
	 */
	void drawPoints(vtkImageData *outData, int extent[6]); 

	/**
	 * \fn void drawLines(vtkImageData *outData, int extent[6])     
	 * \brief Disegna le linee del contorno della ROI sull'immagine VTK
	 * \param outData Immagine VTK in uscita
	 * \param extent Estensione dell'immagine VTK
	 */
	void drawLines(vtkImageData *outData, int extent[6]); 
	
	/**
	 * \fn 	void drawSelectBox(vtkImageData *outData, int extent[6])     
	 * \brief Disegna il box di selezione
	 * \param outData Immagine VTK in uscita
	 * \param extent Estensione dell'immagine VTK
	 */
	void drawSelectBox(vtkImageData *outData, int extent[6]);

	/**
	 * \fn void drawRectangle(vtkImageData *outData, int extent[6])     
	 * \brief Disegna la ROI rettangolare sull'immagine VTK
	 * \param outData Immagine VTK in uscita
	 * \param extent Estensione dell'immagine VTK
	 */
	void drawRectangle(vtkImageData *outData, int extent[6]); 

	/**
	 * \fn void ExecuteData(vtkDataObject *out)     
	 * \brief Esecuzione della pipeline VTK.
	 * \param out Rappresentazione dei dati di visualizzazione.
	 */
	void ExecuteData(vtkDataObject*);
};        

#endif _imageDrawRoi_h_