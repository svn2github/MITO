/**
 * \file appData.h
 * \brief File per la gestione dei dati
 * \author ICAR-CNR Napoli
 */

#ifndef _appData_h_
#define _appData_h_

#include <string>

using namespace std;

class wxArrayString;

/** 
 * \class appData
 * \brief Classe astratta responsabile della gestione dei dati 
 */
class appData {

protected:
	/** 
	 * \var int _idData
	 * \brief Identificatore del dato
	 */
	unsigned int _idData;

	/** 
	 * \var string _name
	 * \brief Nome del dato
	 */
	string _name;

public:

	/** Costruttore */
	appData();

	/** Distruttore */
	virtual ~appData();

	/**
	 * \fn unsigned int getIdData()
	 * \brief Restituisce il valore della variabile _idData
	 * \return Valore della variabile _idData
	 */
	inline unsigned int getIdData() {
		return _idData;
	}

	/**
	 * \fn void setIdData(unsigned int idData)
	 * \brief Assegna l'idData
	 * \param idData Identificativo del dato
	 */
	inline void setIdData(unsigned int idData) {
		_idData = idData;
	}

	/**
	 * \fn string getName()
	 * \brief Restituisce il nome associato al dato
	 * \return Valore della variabile _name
	 */
	inline string getName() {
		return _name;
	}

	/**
	 * \fn void setName(string name)
	 * \brief Assegna un nome al dato
	 * \param name Nome del dato
	 */
	inline void setName(string name) {
		_name = name;
	}

	/**
	 * \fn virtual bool open(string fileName, bool read) = 0
	 * \brief Metodo virtuale puro: restituisce true se il file è aperto correttamente
	 * \param fileName Nome del file o della directory
	 * \param read indica se leggere l'immagine o sono l'header
	 * \return true se l'operazione è andata a buon fine
	 */
	virtual bool open(string fileName, bool read) = 0;

	/**
	 * \fn virtual bool open(wxArrayString *pPaths, bool read) = 0
	 * \brief Metodo virtuale puro: restituisce true se il file è aperto correttamente
	 * \param pPaths Percorsi
	 * \param read indica se leggere l'immagine o sono l'header
	 * \return true se l'operazione è andata a buon fine
	 */
	virtual bool open(wxArrayString *pPaths, bool read) = 0;

	/**
	 * \fn virtual int getHeaderLength() = 0
	 * \brief Metodo virtuale puro: restituisce la dimensione dell'header
	 * \return Dimensione dell'header
	 */
	virtual int getHeaderLength() = 0;

	/**
	 * \fn virtual string getHeaderKey(int index) = 0
	 * \brief Metodo virtuale puro: restituisce una chiave dell'header
	 * \param index Indice del tag
	 * \return Chiave
	 */
	virtual string getHeaderKey(int index) = 0;

	/**
	 * \fn virtual string getHeaderValue(int index) = 0
	 * \brief Metodo virtuale puro: restituisce il valore di un tag dell'header
	 * \param index Indice del tag
	 * \return Tag
	 */
	virtual string getHeaderValue(int index) = 0;

	/**
	 * \fn virtual string getHeaderEntry(int index) = 0
	 * \brief Metodo virtuale puro: restituisce un entry dell'header
	 * \param index Indice dell'entry
	 * \return Entry
	 */
	virtual string getHeaderEntry(int index) = 0;

	/**
	 * \fn virtual int getHeaderEntriesLength() = 0
	 * \brief Metodo virtuale puro: restituisce la dimensione degli entry dell'header
	 * \return Dimensione entry
	 */
	virtual int getHeaderEntriesLength() = 0;
};

#endif _appData_h_