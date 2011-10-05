/**
 * \file point.h
 * \brief File per la gestione di un punto 
 * \author ICAR-CNR Napoli
 */

#ifndef _point_h_
#define _point_h_

/** 
 * \class point
 *  \brief Classe per la gestione di un punto
 */
class point {

public:
	/** Costruttore */
	point() {   
		this->_x = 0;  
		this->_y = 0;  
		this->_selected = 0;  
		this->_next = NULL;  
	}    

	/** Costruttore con parametri da assegnare a _x e _y */
	point(int xa, int ya) {
		this->_x = xa;  
		this->_y = ya;  
		this->_selected = 0;  
		this->_next = NULL;  
	}    

	/** Distruttore */
	~point() {   
		this->_next = NULL;   
	}     

	/**
	 * \fn void select() 
	 * \brief Setta lo stato selezionato (_selected = 1)
	 */
	void select() 
	{
		this->_selected = 1;
	}; 

	/**
	 * \fn void deselect() 
	 * \brief Setta lo stato deselezionato (_selected = )
	 */
	void deselect() {
		this->_selected = 0;
	} ;

	/**
	 * \fn void deselect() 
	 * \brief Restituisce lo stato
	 * \return La variabile _selected
	 */
	int isSelected() {
		return this->_selected;
	};  

	/**
	 * \fn point* getNext()
	 * \brief Restituisce il puntatore al punto successivo
	 * \return Puntatore al punto successivo
	 */
	point* getNext() {
		return this->_next;
	}; 

	/**
	 * \fn void set3dCoord(float x, float y, float z)
	 * \brief Assegna le tre coordinate
	 * \param x Coordinata x
	 * \param y Coordinata y
	 * \param z Coordinata z
	 */
	void set3dCoord(float x, float y, float z) {
		this->_x0=x;     
		this->_y0=y;     
		this->_z0=z;     
	};    

public:     
	/** 
	 * \var int _x
	 * \brief Coordinata x
	 */
	int _x;

	/** 
	 * \var int _y
	 * \brief Coordinata x
	 */
	int _y;       

	/** 
	 * \var int _x0
	 * \brief Coordinata x0
	 */
	float _y0;

	/** 
	 * \var int _y0
	 * \brief Coordinata y0
	 */
	float _z0;

	/** 
	 * \var int _z0
	 * \brief Coordinata z0
	 */
	float _x0;

	/** 
	 * \var point *_next
	 * \brief Puntatore al prossimo punto
	 */
	point *_next;

	private:
	/** 
	 * \var int _selected
	 * \brief Stato: 0 deselezionato, 1 selezionato
	 */
	int _selected;
};        

#endif _point_h_