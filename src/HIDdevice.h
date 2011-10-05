/**
 * \file HIDdevice.h
 * \brief File per la gestione di un generico Human Interface Device
 */

#ifndef _HIDdevice_h_
#define _HIDdevice_h_

#include <windows.h>

/** 
 * \class HIDdevice
 * \brief Classe che gestisce la connessione, l'input e l'output di un generico dispositivo H.I.D.
 */
class HIDdevice {
	
	/** 
	 * \var		HANDLE mHandle
	 * \brief	mHandle
	 */
	HANDLE mHandle;

	/** 
	 * \var		HANDLE mEvent
	 * \brief	mEvent
	 */
	HANDLE mEvent;

	/** 
	 * \var		OVERLAPPED mOverlapped
	 * \brief	mOverlapped is a struct that contains information used in asynchronous (or overlapped) 
	 * \		input and output (I/O).
	 */
	OVERLAPPED mOverlapped;		 

	/** 
	 * \var		bool mConnected
	 * \brief	mConnected vale true nel caso gli handle necessari alla gestione del device siano già stati
	 * \		creati, altrimenti vale false.
	 */
	bool mConnected;


public:
	
	/** Costruttore  */
	HIDdevice();

	/** Distruttore */
	~HIDdevice();

	/**
	 * \fn		bool Disconnect()
	 * \brief	Disconnette il dispositivo, chiudendo gli handle che ne permettono la gestione.
	 */
	bool Disconnect();

	/**
	 * \fn		bool IsConnected()
	 * \brief	Permette di conoscere il valore di mConnected.
	 */
	bool IsConnected() const {return mConnected;}

	/**
	 * \fn		bool FindADevice(unsigned short device_id, unsigned short vendor_id, int & FoundIndex, int StartIndex = 0)
	 * \brief	Ricerca nella Device Table di windows, il primo HID device che soddisfi i criteri di ricerca.
	 * \
	 * \param	device_id	Identificativo del dispositivo da cercare.
	 * \		vendor_id	Idendificativo del fornitore del dispositivo da cercare.
	 * \		FoundIndex	Parametro di output contenente l'indice di riga (della Device Table) in cui è
	 * \					stato trovato il dispositivo cercato. Se la ricerca da esito negativo, FoundIndex
	 * \					varrà "-1".
	 * \		StartIndex	Identificativo dell'indice di riga (della Device Table) a partire dal quale dev'essere
	 * \					effettuata la ricerca del dispositivo.
	 */
	bool FindADevice(unsigned short device_id, unsigned short vendor_id, int & FoundIndex, int StartIndex = 0);
    
	/**
	 * \fn		bool WriteToDevice(unsigned const char * buffer, int num_bytes)
	 * \brief	Se il device è connesso, invia al device una comunicazione formata da un pacchetto di bytes.
	 * \
	 * \param	buffer		Puntatore al buffer contenente il pacchetto da inviare al dispositivo.
	 * \		num_bytes	Indica la lunghezza del pacchetto da inviare.
	 */
	bool WriteToDevice(unsigned const char * buffer, int num_bytes);

	/**
	 * \fn		bool ReadFromDevice(unsigned const char * buffer, int max_bytes, int & bytes_read, int timeout=50)
	 * \brief	Se il device è connesso, riceve dal device una comunicazione formata da un pacchetto di bytes.
	 * \
	 * \param	buffer		Puntatore al buffer contenente il pacchetto ricevuto dal dispositivo.
	 * \		bytes_read	Indica la lunghezza del pacchetto ricevuto.
	 * \		max_bytes	Indica la lunghezza massima del pacchetto ricevibile.
	 * \		timeout		Indica il tempo massimo in attesa della ricezione.
	 */
	bool ReadFromDevice(unsigned const char * buffer, int max_bytes, int & bytes_read, int timeout=50);
};

#endif
