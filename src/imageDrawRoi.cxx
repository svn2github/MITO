/**
 * \file imageDrawROI.cxx
 * \brief Creazione ROI
 * \author ICAR-CNR Napoli
 */
#include "imageDrawROI.h"           
        
/** 
 * \def SET_PIXEL(x, y, color) { ptr = &outPtr[(y)*nxnc + (x)*nc]; ptr[0] = color[0]; ptr[1] = color[1]; ptr[2] = color[2];}
 * \brief Macro per la colorazione del pixel per la visualizzazione
 */
#define SET_PIXEL(x, y, color) { ptr = &outPtr[(y)*nxnc + (x)*nc]; ptr[0] = color[0]; ptr[1] = color[1]; ptr[2] = color[2];}

// Costruttore
imageDrawRoi::imageDrawRoi() {    

	this->_firstPoint = NULL;
	this->_lastPoint = NULL; 

	this->_numPoints = 0; 
	this->_numSelectedPoints = 0; 

	/* Raggio dei punti del contorno delle ROI: rosso */
	this->_pointRadius = 0; 
	
	/* Colore dei punti del contorno delle ROI: rosso */
	this->_pointColor[0] = 1;
	this->_pointColor[1] = 0;
	this->_pointColor[2] = 0;

	/* Colore dei punti selezionati del contorno delle ROI: giallo */
	this->_selectedPointColor[0] = 1;
	this->_selectedPointColor[1] = 1;
	this->_selectedPointColor[2] = 0;

	/* Colore delle linee del contorno delle ROI: rosso */
	this->_lineColor[0] = 1;
	this->_lineColor[1] = 0;
	this->_lineColor[2] = 0;

	this->_drawSelectBox = 0;
	this->_sbox._x = 0;
	this->_sbox._y = 0;
	this->_sbox1._x = 0;
	this->_sbox1._y = 0; 
	this->_sbox2._x = 0; 
	this->_sbox2._y = 0; 

	this->_drawRectangle = 0;
	this->_sRect._x = 0;
	this->_sRect._y = 0;
	this->_sRect1._x = 0;
	this->_sRect1._y = 0; 
	this->_sRect2._x = 0; 
	this->_sRect2._y = 0; 

	this->_points = vtkPoints::New(); 

	/* Forma della ROI: poligonale */
	this->_shape = ROI_SHAPE_POLYGON; 

	this->_closed = 0;
	this->_slice = 0;
	this->_pixelSpacingX = 0;
	this->_pixelSpacingY = 0;
	this->_sliceThickness = 0;
}   

// Distruttore 
imageDrawRoi::~imageDrawRoi() {   
	this->_points->Delete();
	point *p1,*p2;
	p1 = this->_firstPoint;
	while(p1!=this->_lastPoint){
		p2 = p1;
		p1 = p1->getNext();
		delete p2;
	}
	delete this->_lastPoint; 
}   

// Get dei punti del contorno della ROI disegnata 
vtkPoints* imageDrawRoi::getPoints() {   
	point *p = this->_firstPoint;
	int n=0; 
	this->_points->Reset();  
	while (p) { 
		n++;  
		this->_points->InsertNextPoint(p->_x, p->_y, 0); 
		p = p->getNext(); 
	}   
	return this->_points;  
} 

// Set dell'estensione dell'immagine 
void imageDrawRoi::setMinMax(int *outExt)
{
	/* estensione minima lungo x */
	_xMin = outExt[0];  

	/* estensione massima lungo x */
	_xMax = outExt[1];

	/* estensione minima lungo y */
	_yMin = outExt[2]; 

	/* estensione massima lungo y */
	_yMax = outExt[3];
}

// Set della forma poligonale della ROI 
void imageDrawRoi::setShapeToPolygon() 
{       
	this->_shape = ROI_SHAPE_POLYGON; 
	this->Modified();
}

// Set della forma rettangolare della ROI 
void imageDrawRoi::setShapeToRectangle() 
{       
	this->_shape = ROI_SHAPE_RECTANGLE; 
	this->Modified();
}

// Set dello stato del rettangolo 
void imageDrawRoi::setRectangle(int rectangle)  
{ 
	this->_drawRectangle = rectangle;
	if(rectangle == 0)
	{
		this->_sRect._x = 0;
		this->_sRect._y = 0;
		this->_sRect1._x = 0;
		this->_sRect1._y = 0; 
		this->_sRect2._x = 0; 
		this->_sRect2._y = 0;
	}
}

// Inserisce il primo punto del box di selezione 
void imageDrawRoi::startSelectBox(int x, int y)
{    
	this->_sbox._x = x;  
	this->_sbox._y = y;  
}    

// Disegna il box di selezione 
void imageDrawRoi::dragSelectBox(int x, int y)
{    
	this->_drawSelectBox = 1; 
	/* Calcolo delle coordinate */
	if (x < this->_sbox._x) {
		_sbox1._x = x;        
		_sbox2._x = _sbox._x;        
	} else {        
		_sbox1._x = _sbox._x;        
		_sbox2._x = x;        
	}          
	if (y < this->_sbox._y) {      
		_sbox1._y = y;        
		_sbox2._y = _sbox._y;        
	} else {        
		_sbox1._y = _sbox._y;        
		_sbox2._y = y;        
	}               
	this->Modified();          
}

 //	Inserisce l'ultimo punto del box di selezione e seleziona i punti interni al 
 void imageDrawRoi::endSelectBox(int x, int y)      
{          
	this->_drawSelectBox = 0;        
	if (x < this->_sbox._x) {      
		_sbox1._x = x;        
		_sbox2._x = _sbox._x;        
	} else {        
		_sbox1._x = _sbox._x;        
		_sbox2._x = x;        
	}        
	if (y < this->_sbox._y) {    
		_sbox1._y = y;      
		_sbox2._y = _sbox._y;      
	} else {      
		_sbox1._y = _sbox._y;      
		_sbox2._y = y;      
	}        
	int r = this->_pointRadius;     
	point *p = this->_firstPoint;     
	while (p != NULL) {
		/* Controlla se il punto p è interno al box di selezione */
		if (p->_x+r >= _sbox1._x && p->_x-r <= _sbox2._x &&
		p->_y+r >= _sbox1._y && p->_y-r <= _sbox2._y)  
		{ 
			/* Se il punto p non è selezionato lo seleziona */
			if (!p->isSelected()) 
			{  
				p->select();  
				this->_numSelectedPoints++;  
				this->Modified();  
			}  
		}
		/* punto successivo */
		p = p->getNext();
	}  
	this->Modified();  
}  

// Inserisce il primo punto della ROI rettangolare 
void imageDrawRoi::startRectangle(int x, int y)
{    
	this->_sRect._x = x;  
	this->_sRect._y = y;
	this->_sRect1=_sRect;
	this->_sRect2=_sRect;

	point *p = new point(x, y);
	this->_firstPoint = p;   
	this->_lastPoint = p;   
	this->_numPoints++;     
	p->select();     
	this->_numSelectedPoints++;     
	this->Modified();       
}    

// Disegna la ROI rettangolare 
void imageDrawRoi::dragRectangle(int x, int y)
{    
	if (x < this->_sRect._x) {      
		_sRect1._x = x;        
		_sRect2._x = _sRect._x;        
	} else {        
		_sRect1._x = _sRect._x;        
		_sRect2._x = x;        
	}          
	if (y < this->_sRect._y) {      
		_sRect1._y = y;        
		_sRect2._y = _sRect._y;        
	} else {        
		_sRect1._y = _sRect._y;        
		_sRect2._y = y;        
	}              
	this->Modified();          
}          

// Inserisce l'ultimo punto della ROI rettangolare
void imageDrawRoi::endRectangle(int x, int y)     
{   
	this->_drawRectangle = 1;        
	if (x < this->_sRect._x) {      
		_sRect1._x = x;        
		_sRect2._x = _sRect._x;        
	} else {        
		_sRect1._x = _sRect._x;
		_sRect2._x = x;        
	}        
	if (y < this->_sRect._y) {
		_sRect1._y = y;      
		_sRect2._y = _sRect._y;
	} else {      
		_sRect1._y = _sRect._y;
		_sRect2._y = y;      
	} 
	point *p = NULL;        
	point *p1 = this->_firstPoint;        
	p = p1->getNext();   
	if (p==NULL){
		// In base al primo e ultimo punto inserito calcola gli altri due punti
		point *p3 = new point(x,y);     
		this->_numPoints++;
		p3->select(); 
		this->_numSelectedPoints++;     
		point *p2= new point(p3->_x, p1->_y);
		this->_numPoints++;
		p2->select();         
		this->_numSelectedPoints++;            
		point *p4= new point(p1->_x, p3->_y);
		this->_numPoints++;             
		p4->select();         
		this->_numSelectedPoints++;
		p1 = this->_firstPoint;
		p1->_next = p2;
		p2->_next = p3;
		p3->_next = p4; 
		this->_lastPoint = p4;
		this->Modified();
	}
}  

// Sposta un punto del contorno della ROI rettangolare 
void imageDrawRoi::moveRectangle(int x, int y)     
{  
	// Per lo spostamento è necessario la selezione di un solo punto
	if (this->_numSelectedPoints ==1)      
	{  
		point *p = this->_firstPoint;
		
		while (p)
		{
			// Controlla se le nuove coordinate del punto selezionato
			if( p->_x == x || p->_y == y)
				return;
			p = p->getNext(); 
		}
		point *pNext;
		point *pPrev = this->_lastPoint;
		point *pSel = this->_firstPoint;

		// Trova il punto selezionato e calcola il precedente e il successivo
		while (!pSel->isSelected())
		{	
			pPrev = pSel;
			pSel = pSel->getNext(); 
		}
		// Se il punto selezionato è l'ultimo il successivo è il primo
		if(pSel == this->_lastPoint)
			pNext = this->_firstPoint;
		else
			pNext = pSel->getNext();
		// Calcola le nuove coordinate del successivo e del precedente
		if(pNext->_x==pSel->_x)
			pNext->_x = x;
		else pNext->_y = y;
		if(pPrev->_x==pSel->_x)
			pPrev->_x = x;
		else pPrev->_y = y;
		pSel->_x = x;
		pSel->_y = y;
	}
	this->Modified(); 
}  

// Seleziona tutti i punti del contorno della ROI 
void imageDrawRoi::selectAllPoints() 
{  
	point *p = this->_firstPoint; 
	while (p != NULL) {
		p->select();    
		p = p->getNext();  
	}    
	this->_numSelectedPoints = this->_numPoints;  
	this->Modified();    
}    

// Deseleziona tutti i punti del contorno della ROI 
void imageDrawRoi::deselectAllPoints()   
{    
	point *p = this->_firstPoint; 
	while (p != NULL) {
		p->deselect();    
		p = p->getNext();  
	}    
	this->_numSelectedPoints = 0;         
	this->Modified();           
}           

// Inserimento di un punto (x,y) tra due punti esistenti   
void imageDrawRoi::insertPoint(int x, int y)       
{           
	this->deselectAllPoints(); 

	// Seleziona solo il punto dopo il quale inserire (x, y)
	/* Caso 1: Numero di punti >= 3 */
	if (this->_numPoints >= 3) {
		/* Trova il punto di controllo p più vicino a (x, y) 
		 *	Trova i punti di controllo pLeft e pRigh, precedente e successivo di p
		 *	Calcola la distanza di (x, y) da pLeft e pRigh
		 *	Se pLeft è più vicino, lo seleziona, altrimenti seleziona p 
		 */
		point *p = this->_firstPoint;      
		int dx = x - p->_x;    
		int dy = y - p->_y;    
		int distsq = dx * dx + dy * dy;
		int mindistsq = distsq;      
		point *minp = this->_firstPoint;      
		p = p->getNext();       
		point *pLeft = this->_firstPoint;      
		point *minpLeft = pLeft;      
		while (p != NULL) {     
			dx = x - p->_x;     
			dy = y - p->_y;     
			distsq = dx * dx + dy * dy;
			if (distsq < mindistsq) {    
				minp = p;      
				mindistsq = distsq;      
				minpLeft = pLeft;      
			}        
			pLeft = p;      
			p = p->getNext();      
		} 
		/* Caso particolare: p è il primo punto */
		if (minp == this->_firstPoint)     
		{        
			minpLeft = this->_lastPoint;      
			point *minpRight = minp->getNext();     
			int dxLeft = x - minpLeft->_x;   
			int dyLeft = y - minpLeft->_y;   
			int dxRight = x - minpRight->_x;   
			int dyRight = y - minpRight->_y;    
			int leftDistsq = dxLeft * dxLeft + dyLeft * dyLeft;
			int rightDistsq = dxRight * dxRight + dyRight * dyRight;
			if (leftDistsq < rightDistsq)      
			{ 
				/* Controlla se il contorno della ROI è chiuso */
				if (this->_closed)        
				{    
					/* Se il contorno è chiuso seleziona l'ultimo punto */
					this->_lastPoint->select();         
					this->_numSelectedPoints++;         
					this->insertAfterSelectedPoint(x, y);        
				}         
				else   
				{ 
					/* altrimenti crea un nuovo punto che diventa il primo */
					point *p = new point(x,y);    
					p->_next = this->_firstPoint;      
					this->_firstPoint = p;      
					p->select();        
					this->_numSelectedPoints++;        
					this->_numPoints++;        
				}        
			}        
			else        
			{        
				minp->select();        
				this->_numSelectedPoints++;        
				this->insertAfterSelectedPoint(x, y);       
			}          
			return;          
		}

		/* Caso particolare: p è l'ultimo punto */
		else if (minp == this->_lastPoint)      
		{           
			point *minpRight = this->_firstPoint;       
			int dxLeft = x - minpLeft->_x;     
			int dyLeft = y - minpLeft->_y;     
			int dxRight = x - minpRight->_x;     
			int dyRight = y - minpRight->_y;     
			int leftDistsq = dxLeft * dxLeft + dyLeft * dyLeft; 
			int rightDistsq = dxRight * dxRight + dyRight * dyRight; 
			if (leftDistsq < rightDistsq) 
				minpLeft->select();      
			else 
				minp->select();        
		}         
		else         
		{         
			point *minpRight = minp->getNext();      
			int dxLeft = x - minpLeft->_x;    
			int dyLeft = y - minpLeft->_y;    
			int dxRight = x - minpRight->_x;    
			int dyRight = y - minpRight->_y;    
			int leftDistsq = dxLeft * dxLeft + dyLeft * dyLeft;
			int rightDistsq = dxRight * dxRight + dyRight * dyRight;
			if (leftDistsq < rightDistsq) 
				minpLeft->select();     
			else 
				minp->select();        
		}         
		this->_numSelectedPoints++;         
	}

	/* Caso 2: Numero di punti = 2 */
	else if (this->_numPoints == 2) { 
		/* Seleziona il primo punto */
		point *p = this->_firstPoint;        
		p->select();           
		this->_numSelectedPoints++;           
	}
	/* Inserisce (x,y) dopo il punto selezionato */
	this->insertAfterSelectedPoint(x, y);          
}           

// Inserisce un nuovo punto(x,y) dopo il punto selezionato.
// Se non esistono punti, crea un nuovo punto e lo seleziona.
// Se esistono punti, ma nessuno è selezionato, seleziona l'ultimo. 
void imageDrawRoi::insertAfterSelectedPoint(int x, int y)       
{           
	point *p1 = this->_firstPoint;        
	point *p2 = NULL;        
	point *sel = NULL;
	/* Lista vuota */
	if (p1 == NULL) { 

		/* Aggiunge e seleziona il nuovo punto*/
		point *p = new point(x, y);
		this->_firstPoint = p;   
		this->_lastPoint = p;   
		this->_numPoints++; 
		p->select();     
		this->_numSelectedPoints++;     
		this->Modified();     
		return;     
	}
	/* Caso generale */
	p2 = p1;   
	while (p1) {   
		if (p1->isSelected()) {   
			sel = p1;   
			}     
		p2 = p1;   
		p1 = p1->getNext();   
	}     
	if (!sel) {   
		p2->select();     
		sel = p2;   
	}

	/* Aggiunge il nuovo punto e lo seleziona*/
	p1 = sel;   
	p2 = p1->getNext();   
	point *p = new point(x, y);  
	p1->_next = p;
	p->_next = p2;       
	if (!p2)        
		this->_lastPoint = p;       
	this->_numPoints++;         
	this->deselectAllPoints();         
	p->select();         
	this->_numSelectedPoints++;         
	this->Modified(); 
}         

// Cancella i punti selezionati del contorno della ROI 
void imageDrawRoi::deleteSelectedPoints()        
{         
	point *p1 = this->_firstPoint;
	point *p2;  
	if (!p1) return; 
	/* Primo punto selezionato */
	while (p1 && p1->isSelected())
	{   
		p2 = p1->getNext(); 
		delete p1;  
		this->_numPoints--;   
		this->_numSelectedPoints--;   
		p1 = p2; 
		this->_firstPoint = p1; 
		this->Modified();   
	}     
	if (p1 == NULL) {  
		this->_lastPoint = NULL;    
		return;      
	} 
	/* Cancella i punti selezionati dopo il primo */
	p2 = p1->getNext();    
	while (p2)     
	{      
		if (p2->isSelected())     
		{      
			if (p2->getNext() == NULL)   
			{      
				this->_lastPoint = p1;    
				this->_lastPoint->select();       
				p1->_next = NULL;     
				delete p2;      
				this->_numPoints--;       
				this->_numSelectedPoints--;       
				this->Modified();       
				return;       
			}       
			p1->_next = p2->getNext();     
			delete p2;      
			this->_numPoints--;       
			this->_numSelectedPoints--;       
			this->Modified();       
			p2 = p1->getNext(); 
		}   
		else {  
			p1 = p2; 
			if (p2)  
				p2 = p2->getNext(); 
		}   
	}   
}   

// Cancella tutti i punti del contorno della ROI 
void imageDrawRoi::deleteAllPoints()  
{ 
	if(this->_firstPoint==NULL)
		return;
	point *p1 = this->_firstPoint;
	point *p2;  
	while (p1) { 
		p2 = p1->getNext();  
		delete p1;   
		p1 = p2;  
	}    
	this->_firstPoint = this->_lastPoint = NULL;
	this->_numPoints = this->_numSelectedPoints = 0;
	this->Modified();    
} 

// Calcola la lunghezza del contorno della ROI
float imageDrawRoi::lengthTot()
{
	if(this->_numPoints <2)	
		return 0;
	float l = 0;
	point *p0 = this->_firstPoint;
	point *p1 = p0->getNext(); 
	point* pn = this->_lastPoint;
	int i;
	for (i=0; i<this->_numPoints-1; i++) 
	{ 
		l += length(p0, p1); 
		p0 = p1;
		p1 = p0->getNext();
	}
	if (this->_closed == 1)
		l += length(pn, this->_firstPoint);
   return l;
}					

// Calcolo della lunghezza del segmento che unisce due punti
float imageDrawRoi::length(point* mesureA, point* mesureB)
{
	float mesureLength;
	double coteA, coteB;	
	coteA = fabs((double)(mesureA->_x - mesureB->_x));
	coteB = fabs((double)(mesureA->_y - mesureB->_y));	
	if(_pixelSpacingX != 0 && _pixelSpacingY != 0)
	{
		coteA *= _pixelSpacingX;
		coteB *= _pixelSpacingY;
	}
	if( coteA == 0) mesureLength = coteB;
	else if( coteB == 0) mesureLength = coteA;
	else mesureLength = coteB / (sin (atan( coteB / coteA)));
	if(_pixelSpacingX != 0 && _pixelSpacingY != 0)
	{
		mesureLength /= 10.0;
	}
	return mesureLength;
}

// Calcola il volume della ROI
float imageDrawRoi::volume()
{
	return area() * (_sliceThickness/10);
}

// Calcola l'area della ROI
float imageDrawRoi::area()
{
   float area = 0;
   /* l'area viene calcolata nel caso di ROI chiusa e 
    *	formata da più di tre punti */
   if(this->_closed==0||this->_numPoints <3){	
		return 0;
   }
	point *p0 = this->_firstPoint;
	point *p1 = p0->getNext(); 
	point *p2 = p1->getNext();
	point* pn = this->_lastPoint;
	for (int i=0; i<this->_numPoints; i++) 
   {
		area += p1->_x * p2->_y;
		area -= p1->_x * p0->_y;
		p0 = p1;
		p1 = p2;
		if (p2==pn)
			p2 = this->_firstPoint;
		else if (p2==this->_firstPoint)
			p2 = this->_firstPoint->getNext();
		else p2 = p1->getNext();
	}
	area /= 2;
	if( _pixelSpacingX != 0 && _pixelSpacingY != 0)
	{
		area *= _pixelSpacingX * _pixelSpacingY;
		area /= 100; // da mm2 a cm2
	}
	return area < 0 ? -area : area;
}

// Calcola l'angolo formato dalla ROI
float imageDrawRoi::angle()
{
	if (this->_numPoints <3)
		return 0;
		point *p1 = this->_firstPoint;
		point *p0 = p1->getNext(); 
		point *p2 = this->_lastPoint;
		float ax, ay, bx, by, val, angle;
		ax = p1->_x - p0->_x;
		ay = p1->_y - p0->_y;
		bx = p2->_x - p0->_x;
		by = p2->_y - p0->_y;
		if(ax==0 && bx==0) return 0;
		val = ((ax * bx) + (ay * by)) / (sqrt(ax * ax + ay * ay) * sqrt(bx * bx + 
				by * by));
		angle = acos(val)*180/vtkMath::Pi();
		return angle;
}

// Sposta tutti i punti selezionati del contorno della ROI
void imageDrawRoi::moveSelectedPoints(int x, int y)
{   
	if (this->_numSelectedPoints >0)      
	{
		/* Calcola la variazione delle coordinate del primo punto. 
		 *	Questa variazione corrisponde allo spostamento da effettuare.
		 *	Se le nuove coordinate dei punti selezionati cadono nell'intervallo
		 *	di estensione dell'immagine, la ROI viene spostata, altrimenti la
		 *	ROI rimane ferma.	
		 */
		point *psel = this->_firstPoint;      
		while (!psel->isSelected())        
			psel = psel->getNext();       
		int dx = x - psel->_x;              
		int dy = y - psel->_y;   
		point *p = this->_firstPoint; 
		while (p) { 
			if (p->isSelected()) {  
				if (p->_x + dx > _xMax || p->_x + dx < _xMin || 
					p->_y + dy > _yMax || p->_y + dy < _yMin ) {  
					return; 
				}
			}
			p = p->getNext();  
		} 
		p = this->_firstPoint; 
		while (p) {  
			if (p->isSelected()) {  
				p->_x += dx;  
				p->_y += dy;   
			}    
			p = p->getNext();  
		}    
		this->Modified();    
	}    
}

// Converte il colore da rgb (0,1) a short
void imageDrawRoi::convertColor(float *f, long *c)   
{            
	//verde
	if(f[0]==1&& f[1]==0 && f[2]==0){
		c[0] = 0; 
		c[1] = 255; 
		c[2] = 0; 
	}
	else if(f[0]==1&& f[1]==1 && f[2]==0){
		c[0] = 0; 
		c[1] = 0; 
		c[2] = 255; 
	}
}            

// Disegna una linea tra due punti del contorno della ROI
void imageDrawRoi::drawLine(int xx1, int yy1, int xx2, int yy2, long  color[3], long *outPtr, int pNxnc, int pNc)
{            
	long *ptr;          
	int dx, dy, dy2, dx2, r, dydx2;      
	int x, y, xInc;         
	int nxnc = pNxnc, nc=pNc;        
	int x1, y1, x2, y2;        
	if (yy1 <= yy2) {   
		x1 = xx1;     
		y1 = yy1;     
		x2 = xx2;     
		y2 = yy2;     
	} else {     
		x1 = xx2;     
		y1 = yy2;     
		x2 = xx1;     
		y2 = yy1;     
	}

	/* Calcola la variazione di x e di y */
	dx = abs(x2 - x1);   
	dy = abs(y2 - y1);
	dx2 = dx << 1;   
	dy2 = dy << 1;   
	if (x1 < x2)       
		xInc = 1;        
	else          
		xInc = -1;        
	x = x1;        
	y = y1;

	/* Se la linea è orizzontale, colora tutti i pixel tra x1 e x2 */
	if (dy == 0)       
	{          
		if (x1 < x2) {      
			for (x=x1; x < x2; x++)
				SET_PIXEL(x, y1, color);        
		} else {   
			for (x=x2; x < x1; x++)
				SET_PIXEL(x, y1, color);   
		}     
	}

	/* Se la linea è verticale, colora tutti i pixel tra y1 e y2 */     
	else if (dx == 0) 
	{     
		for (y=y1; y < y2; y++)
			SET_PIXEL(x1, y, color);   
	}

	/* 
	 *	Alrimenti calcola la posizione dei punti intermedi della 
	 *	linea e colora i pixel corrispondenti 
	 */
	else if (dy <= dx) 
	{     
		dydx2 = (dy-dx) << 1; 
		r = dy2 - dx;   	   
		SET_PIXEL(x, y, color);     
		if (x1 < x2)    
		{       
			while (x < x2)    
			{       
				x += xInc;     
				if (r <= 0)    
					r += dy2;     
				else {      
					y++;       
					r += dydx2;     
				}   
				SET_PIXEL(x, y, color); 
			}   
		}   
		else   
		{   
			while (x > x2)
			{   
				x += xInc; 
				if (r <= 0)
					r += dy2; 
				else {  
					y++;   
					r += dydx2; 
				}   
				SET_PIXEL(x, y, color); 
			}       
		}       
	}       
	else       
	{       
		dydx2 = (dx-dy) << 1;   
		r = dx2 - dy;   
		SET_PIXEL(x, y, color);     
		while (y < y2)    
		{       
			y++;         
			if (r <= 0)      
				r += dx2;       
			else {        
				x += xInc;       
				r += dydx2;       
			}         
			SET_PIXEL(x, y, color);       
		}         
	}         
}         

// Disegna il box di selezione
void imageDrawRoi::drawSelectBox(vtkImageData *outData, int outExt[6])     
{         
	long color[3];
	long *outPtr = (long*)outData->GetScalarPointerForExtent(outExt);       
	long nxnc, nc, nx;    
	point p1, p2, p3, p4;   
	long xMin, xMax, yMin, yMax;   
	xMin = outExt[0];     
	xMax = outExt[1];     
	yMin = outExt[2];     
	yMax = outExt[3];     
	nx = xMax - xMin + 1;
	
	/* Calcola il numero di componenti scalari */
	nc = outData->GetNumberOfScalarComponents(); 

	/* Calcola la lunghezza della riga */
	nxnc = nx*nc; 
	
	/* Converte il colore in SelectedPointColor */ 
	convertColor(this->_selectedPointColor, color);     
	p1._x = _sbox1._x;      
	p1._y = _sbox1._y;      
	p2._x = _sbox2._x;      
	p2._y = _sbox1._y;      
	p3._x = _sbox2._x;      
	p3._y = _sbox2._y;      
	p4._x = _sbox1._x;      
	p4._y = _sbox2._y;      

	/* Se i punti sono interni all'immagine, disegna il box di selezione */
	if (p1._x >= xMin && p1._x <= xMax &&
		p1._y >= yMin && p1._y <= yMax &&
		p2._x >= xMin && p2._x <= xMax &&
		p2._y >= yMin && p2._y <= yMax &&
		p3._x >= xMin && p3._x <= xMax &&
		p3._y >= yMin && p3._y <= yMax &&
		p4._x >= xMin && p4._x <= xMax &&
		p4._y >= yMin && p4._y <= yMax) 
	{       
		drawLine(p1._x, p1._y, p2._x, p2._y, color, outPtr, nxnc, nc);
		drawLine(p2._x, p2._y, p3._x, p3._y, color, outPtr, nxnc, nc);
		drawLine(p3._x, p3._y, p4._x, p4._y, color, outPtr, nxnc, nc);
		drawLine(p4._x, p4._y, p1._x, p1._y, color, outPtr, nxnc, nc);
	}       
}       

// Disegna la ROI rettangolare sull'immagine VTK
void imageDrawRoi::drawRectangle(vtkImageData* outData, int outExt[6])     
{         
	long color[3]; 
	long *outPtr = (long *)outData->GetScalarPointerForExtent(outExt);       
	long nxnc, nc, nx;    
	point p1, p2, p3, p4; 
	point *pP1,*pP2,*pP3,*pP4;   
	long xMin, xMax, yMin, yMax;   
	xMin = outExt[0];     
	xMax = outExt[1];     
	yMin = outExt[2];     
	yMax = outExt[3];     
	nx = xMax - xMin + 1; 

	/* Calcola il numero di componenti scalari */
	nc = outData->GetNumberOfScalarComponents(); 

	/* Calcola la lunghezza della riga */
	nxnc = nx*nc; 

	/* Converte il colore in lineColor */ 
	convertColor(this->_lineColor, color); 

	/* 
	 *	Se drawRectangle è nullo vuol dire che siamo nella fase di disegno.
	 *  Viene disegnato il rettangolo considerando le coordinate dei punti
	 *	temporanei (il valore di tali punti cambia, perchè l'utente non ha
	 *	ancora fissato l'ultimo punto del contorno della ROI).
	 */
	if(_drawRectangle==0)
	{
		p1._x = _sRect1._x;      
		p1._y = _sRect1._y;      
		p2._x = _sRect2._x;      
		p2._y = _sRect1._y;      
		p3._x = _sRect2._x;      
		p3._y = _sRect2._y;      
		p4._x = _sRect1._x;      
		p4._y = _sRect2._y; 
		if (p1._x >= xMin && p1._x <= xMax &&
			p1._y >= yMin && p1._y <= yMax &&
			p2._x >= xMin && p2._x <= xMax &&
			p2._y >= yMin && p2._y <= yMax &&
			p3._x >= xMin && p3._x <= xMax &&
			p3._y >= yMin && p3._y <= yMax &&
			p4._x >= xMin && p4._x <= xMax &&
			p4._y >= yMin && p4._y <= yMax) 
		{       
			drawLine(p1._x, p1._y, p2._x, p2._y, color, outPtr, nxnc, nc);
			drawLine(p2._x, p2._y, p3._x, p3._y, color, outPtr, nxnc, nc);
			drawLine(p3._x, p3._y, p4._x, p4._y, color, outPtr, nxnc, nc);
			drawLine(p4._x, p4._y, p1._x, p1._y, color, outPtr, nxnc, nc);
		}       
	} 
	else{

		/*
		 *	In caso contrario viene disegnato il rettangolo considerando
		 *	le coordinate dei punti effettivi del contorno della ROI 
		 */
		pP1 = this->_firstPoint; 
		if (!pP1) return; 
		pP2 = pP1->getNext(); 	
		pP3 = pP2->getNext(); 
		pP4 = pP3->getNext(); 
		if (pP1->_x >= xMin && pP1->_x <= xMax &&
			pP1->_y >= yMin && pP1->_y <= yMax &&
			pP2->_x >= xMin && pP2->_x <= xMax &&
			pP2->_y >= yMin && pP2->_y <= yMax &&
			pP3->_x >= xMin && pP3->_x <= xMax &&
			pP3->_y >= yMin && pP3->_y <= yMax &&
			pP4->_x >= xMin && pP4->_x <= xMax &&
			pP4->_y >= yMin && pP4->_y <= yMax) 
		{       
			drawLine(pP1->_x, pP1->_y, pP2->_x, pP2->_y, color, outPtr, nxnc, nc);
			drawLine(pP2->_x, pP2->_y, pP3->_x, pP3->_y, color, outPtr, nxnc, nc);
			drawLine(pP3->_x, pP3->_y, pP4->_x, pP4->_y, color, outPtr, nxnc, nc);
			drawLine(pP4->_x, pP4->_y, pP1->_x, pP1->_y, color, outPtr, nxnc, nc);
		}
	}
}

// Disegna le linee del contorno della ROI sull'immagine VTK
void imageDrawRoi::drawLines(vtkImageData *outData, int outExt[6])
{       
	long color[3]; 
	long *outPtr = (long *)outData->GetScalarPointerForExtent(outExt);       
	int r = this->_pointRadius;    
	int nx, nc, nxnc;    
	point *p1, *p2;     
	long xMin, xMax, yMin, yMax;   
	xMin = outExt[0];     
	xMax = outExt[1];     
	yMin = outExt[2];     
	yMax = outExt[3];     
	nx = xMax - xMin + 1;

	/* Calcola il numero di componenti scalari */
	nc = outData->GetNumberOfScalarComponents();

	/* Calcola la lunghezza della riga */
	nxnc = nx*nc; 

	/* Converte il colore in LineColor */
	convertColor(this->_lineColor, color);  
	p1 = this->_firstPoint; 
	if (!p1) return; 
	p2 = p1->getNext(); 
	while (p2 != NULL) {
		if (this->Get_shape() == ROI_SHAPE_POLYGON)
		{   
			/* 
			 *	Disegna la linea tra i punti p1 e p2 se le loro coordinate 
			 *  sono interne all'immagine 
			 */  
			if (p1->_x >= xMin && p1->_x <= xMax &&
				p1->_y >= yMin && p1->_y <= yMax && 
				p2->_x >= xMin && p2->_x <= xMax && 
				p2->_y >= yMin && p2->_y <= yMax)  
			{        
				drawLine(p1->_x, p1->_y, p2->_x, p2->_y, color, outPtr, nxnc, nc); 
			}        
		}          
		p1 = p2;     
		p2 = p1->getNext();     
	}
	/* Se la ROI è chiusa unisce il primo punto all'ultimo */
	if (this->_closed == 1)
	{
		p1 = this->_firstPoint; 
		p2 = this->_lastPoint;
		drawLine(p1->_x, p1->_y, p2->_x, p2->_y, color, outPtr, nxnc, nc);
	}
}       
// Disegna i punti del contorno della ROI sull'immagine VTK
void imageDrawRoi::drawPoints(vtkImageData *outData, int outExt[6]) {       
	int x, y, x1, x2, y1, y2; 
	long color[3], *ptr;
	outExt[4]=_slice;
	long *outPtr = (long *)outData->GetScalarPointerForExtent(outExt);       
	int r = this->_pointRadius;
	point *p;
	int nx, nc, nxnc;   
	int xMin, xMax, yMin, yMax;  
	xMin = outExt[0];    
	xMax = outExt[1];    
	yMin = outExt[2];    
	yMax = outExt[3];    
	nx = xMax - xMin + 1;

	/* Calcola il numero di componenti scalari */
	nc = outData->GetNumberOfScalarComponents(); 

	/* Calcola la lunghezza della riga */
	nxnc = nx*nc;    
	p = this->_firstPoint;

	/* 
	 *	Se p è un punto selezionato converte il colore in SelectedPointColor
	 *  altrimenti in PointColor. 
	 *	Controlla se le coordinate del punto si trovano all'interno dell'immagine.
	 *  In caso affermativo colora i pixel corrispondenti al punto.
	*/
	while (p)     
	{      
		if (p->isSelected())     
			convertColor(this->_selectedPointColor, color);     
		else                
			convertColor(this->_pointColor, color);                             
		x1 = p->_x - r;            
		x2 = p->_x + r;            
		y1 = p->_y - r;            
		y2 = p->_y + r; 
		if (x1 >= xMin && x1 <= xMax && y1 >= yMin && y1 <= yMax &&
			x2 >= xMin && x2 <= xMax && y2 >= yMin && y2 <= yMax)  
		{                
			for (y=y1; y <= y2; y++)           
				for (x=x1; x <= x2; x++) 
					SET_PIXEL(x, y, color);              
		}       
		p = p->getNext();     
	}       
}       

int imageDrawRoi::RequestData( vtkInformation*  request, vtkInformationVector**  inputVector, vtkInformationVector*  outputVector){
	int value = this->vtkImageInPlaceFilter::RequestData(request,inputVector,outputVector);
	vtkDataObject *out = vtkDataObject::New();
	this->ExecuteData(out);
	out->Delete();
	return value;
}

//Esecuzione della pipeline VTK       
void imageDrawRoi::ExecuteData(vtkDataObject *out)
{       
	if ( this->GetInput()->GetDataObjectType() != VTK_IMAGE_DATA )   
	{ 
		vtkWarningMacro ("was sent non-image data data object"); 
		return;        
	}        

	vtkImageData *inData = (vtkImageData *) this->GetInput(0); 
	vtkImageData *outData = this->GetOutput(0);

	/* Calcolo dell'estensione */
	int *outExt = outData->GetWholeExtent(); 

	outExt[4] = _slice;
	int x1; 
	x1 = inData->GetNumberOfScalarComponents();      
	if (!(x1 == 3 || x1 == 4)) 
	{        
		vtkErrorMacro("Input has " <<x1<< " components instead of 3 or 4.");
		return;       
	} 
	
	x1 = inData->GetScalarType();
	/*if (x1 != VTK_LONG)    
	{       
		vtkErrorMacro("Input is type "<<x1<< " instead of long.");
		return;       
	}*/
	switch (this->Get_shape())   
	{ 
		/* ROI poligonale: disegna le linee e i punti del contorno */
		case ROI_SHAPE_POLYGON:
			drawLines(outData, outExt);   
			drawPoints(outData, outExt);   
		break; 

		/* ROI rettangolare: disegna il rettangolo e i punti */
		case ROI_SHAPE_RECTANGLE:  
			drawRectangle(outData, outExt); 
			drawPoints(outData, outExt);  
		break;  	
	} 

	/* Se drawSelectBox è selezionato, disegna il box di selezione */
	if (this->_drawSelectBox)     
	{      
		drawSelectBox(outData, outExt);     
	} 
}