/**
 * \file pacsPreferences.h
 * \brief File per la definizione dei parametri necessari alla comunicazione con il PACS
 * \author ICAR-CNR Napoli
 */


#ifndef PREFS_H
#define PREFS_H

#include "pacsDataTypes.h"

/**
 * \brief Specifica una dimensione
 */
#define S_LEN 255


/** \class pacsPreferences
 * \brief Classe che legge i parametri per la comunicazione con i PCAS e li memorizza sul disco
 */
class pacsPreferences {

public:
	/** Costruttore */
	pacsPreferences();

	/** Costruttore con parametro nome del file */
	pacsPreferences(char*);

	/** Costruttore di copia */
	pacsPreferences(pacsPreferences*);

	/** Distruttore */
	~pacsPreferences();

	/**
	 * \fn void setDefaults()
	 * \brief Configura i valori di defaults
	 */
	void setDefaults();

	/**
	 * \fn int read(char* _filename)
	 * \brief Legge i parametri da file
	 * \param _filename Nome del file
	 */
	int read(char*);

	/**
	 * \fn int write(char* _filename)
	 * \brief Scrive i parametri su un file
	 * \param _filename Nome del file
	 */
	int write(char*);

	/** 
	 * \var char server[S_LEN]
	 * \brief Nome dell'host del server
	 */
	char server[S_LEN];

	/** 
	 * \var char calledAP[S_LEN]
	 * \brief Nome dell'application entity del server
	 */
	char calledAP[S_LEN];

	/** 
	 * \var char callingAP[S_LEN]
	 * \brief Nome dell'application entity del client
	 */
	char callingAP[S_LEN];

	/** 
	 * \var char storageAP[S_LEN]
	 * \brief Nome dell'application entity del server per il download delle immagini
	 */
	char storageAP[S_LEN];

	/** 
	 * \var char serverPort[S_LEN]
	 * \brief Porta del server per il download delle immagini
	 */
	char serverPort[S_LEN];

	/** 
	 * \var char localDirectory[F_LEN]
	 * \brief Directory per i file locali
	 */
    char localDirectory[F_LEN*2];

	/** 
	 * \var char storageServer[F_LEN]
	 * \brief Percorso del server
	 */
	char storageServer[F_LEN];

	/** 
	 * \var int storageExec
	 * \brief Indice se avviare e arrestare il server automaticamente
	 */
	int  storageExec;

	/** 
	 * \var char storeClient[F_LEN]
	 * \brief Percorso del client che intende scaricare le immagini
	 */
	char storeClient[F_LEN];

	/** 
	 * \var char port[F_LEN]
	 * \brief Porta del server  per il download delle immagini
	 */
	char port[F_LEN];

	/** 
	 * \var char maxPDU[F_LEN]
	 * \brief PDU massimo
	 */
	char maxPDU[F_LEN];
  
	/** 
	 * \var long dcmOptions
	 * \brief Opzioni per le funzionalità DICOM
	 */
    unsigned long dcmOptions;

	/** 
	 * \var int autoSave
	 * \brief Indica se salvare i parametri all'uscita (!=0) o meno (=0)
	 */
    int autoSave;

	/** 
	 * \var char logFile[S_LEN]
	 * \brief Nome del file di log
	 */
	char logFile[S_LEN];

};

#endif
