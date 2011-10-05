/**
 * \file wxWiiDriver.h
 * \brief File per la gestione di un singolo Wiimote
 */

#ifndef _wxWiiDriver_h_
#define _wxWiiDriver_h_

#include <wx/wx.h>
#include "wx/thread.h"

#include "HIDdevice.h"


/** 
 * \class wxWiiDriver
 * \brief Classe che estende la classe wxThread creando un thread che gestisce autonomamente un singolo Wiimote. 
 * \
 */
class wxWiiDriver : public wxThread 
{

public:

	/** Costruttore  */
	wxWiiDriver(wxEvtHandler* Handler, HANDLE & sem, int idWii = 0, int HIDTableIndex = 0) : wxThread()
	{
		this->Handler = Handler;
		Led2 = true;
		Led1 = Led3 = Led4 = false;
		changeLedsStatus = false;
		
		VibrationCont = 0;
		motionCont = 0;
		

		closingThreadLoop  = false;
		closingThread  = false;
		stillLooping = true;

		this->idWii = idWii; 
		this->HIDTableIndex = HIDTableIndex;
		this->sem = sem;
		SendEvents = true;

		//lastMotionSendedTime=0;
        
		Init();
	}	
	
	/**
	* \fn		virtual wxThread::ExitCode Entry()
	* \brief	L'esecuzione del thread comincia qui.
	*/
	virtual wxThread::ExitCode Entry();
	
	/** Distruttore */
	~wxWiiDriver(){ Disconnect(); }

	/** 
	 * \var		bool Led1, Led2, Led3, Led4
	 * \brief	Indicano l'accensione o meno dei leds del Wiimote.
	 */
    bool Led1, Led2, Led3, Led4;

	/** 
	 * \var		bool changeLedsStatus
	 * \brief	Indica se il valori dei leds sono cambiati.
	 */
    bool changeLedsStatus;


	/** 
	 * \var		int	 VibrationCont
	 * \brief	Contatore utilizzato per far vibrare il Wii per un determinato numero di cicli del loop principale.
	 */
	int	 VibrationCont;		

	/** 
	 * \var		int motionCont
	 * \brief	Contatore utilizzato per tener conto dei motion inviati e non ancora processati.
	 */
	int motionCont;

    /** 
	 * \var		bool closingThreadLoop
	 * \brief	Se vale true allora il thread esce dal loop di interazione col Wiimote.
	 */
	bool closingThreadLoop;

	/** 
	 * \var		bool closingThread
	 * \brief	Se vale true allora viene effettuata la chiusura del thread.
	 */
	bool closingThread;

	/** 
	 * \var		bool stillLooping
	 * \brief	Se vale true allora il thread si trova ancora nel loop di interazione col Wiimote.
	 */
	bool stillLooping;

	/** 
	 * \var		int idWii
	 * \brief	Identificativo numerico del Wii, viene inserito in ogni evento generato dal Wii.
	 */
	int  idWii;

	/** 
	 * \var		int HIDTableIndex
	 * \brief	Indice che viene utilizzato per accedere alla Device Table di Windows e collegarsi al Wiimote.
	 */
	int HIDTableIndex;

	/** 
	 * \var		bool SendEvents
	 * \brief	Abilita o meno l'invio degli eventi verso MITO.
	 */
	bool SendEvents;

	/** 
	 * \var SYSTEMTIME st
	 * \brief Structur per la memorizzazione del tempo di sistema.
	 */
	//SYSTEMTIME st;

	//int lastMotionSendedTime;

	/*
	__int64 timeStamp;
	__int64 lastTimeStamp;
	__int64 clockFrequency;
	*/
	
	/**
	 * \fn		bool ConnectToDevice(int & FoundIndex, int StartIndex = 0)
	 * \brief	Si connette al primo Wiimote trovato nella Device Table di Windows a partire dalla riga StartIndex.
	 * \
	 * \param	FoundIndex	Parametro di output contenente l'indice di riga (della Device Table) in cui è
	 * \					stato trovato il dispositivo cercato. Se la ricerca da esito negativo, FoundIndex
	 * \					varrà "-1".
	 * \		StartIndex	Identificativo dell'indice di riga (della Device Table) a partire dal quale dev'essere
	 * \					effettuata la ricerca del dispositivo.
	 */
	bool ConnectToDevice(int & FoundIndex, int StartIndex = 0);

	/** 
	 * \fn		bool Disconnect()
	 * \brief	Disconnette il Wiimote liberando le strutture che ne permettono la gestione a basso livello.
	 */
	bool Disconnect();	

	/** 
	 * \fn		bool IsConnected() const
	 * \brief	Controlla lo stato di connesso o meno del Wiimote.
	 */
	bool IsConnected() const {return HID.IsConnected();}

	
	
	bool StartDataStream();
	bool StopDataStream();

	/** 
	 * \fn		bool SetVibration(bool vib_on)
	 * \brief	Invia al Wiimote un report che ne imposta la vibrazione.
	 * \
	 * \param	vib_on	Se vale true il paylod del paylod sarà tale da far vibrare il Wii,  
	 * \				altrimenti se vib_on vale false la vibrazione viene disabilitata.
	 */
	bool SetVibration(bool vib_on);

	/** 
	 * \fn		bool SetLEDs(bool led1, bool led2, bool led3, bool led4)
	 * \brief	Invia al Wiimote un report per impostare la configurazione dei leds del Wiimote.
	 * \
	 * \param	led1	Indica se il led1 del Wii dev'essere acceso o meno.
	 * \		led2	Indica se il led2 del Wii dev'essere acceso o meno.
 	 * \		led3	Indica se il led3 del Wii dev'essere acceso o meno.
 	 * \		led4	Indica se il led4 del Wii dev'essere acceso o meno.
	 */
	bool SetLEDs(bool led1, bool led2, bool led3, bool led4);

	/** 
	 * \fn		int GetInput(int timeout = 1)
	 * \brief	Legge i dati inviati dal Wiimote e ne interpreta il contenuto andando ad aggiornare le struct opportune.
	 * \
	 * \param	timeout		Indica il tempo massimo in attesa della ricezione.
	 */
	int  GetInput(int timeout = 1);
    
	/** 
	 * \fn		bool StartReportModeContinuousMotion()
	 * \brief	Richiede al Wii di inviare continuamente un report contenente lo status dei tasti  
	 * \		e delle angolazioni del Wii.
	 */
	bool StartReportModeContinuousMotion();

	/** 
	 * \fn		bool StartReportModeContinuousMotionIR()
	 * \brief	Richiede al Wii di inviare continuamente un report contenente lo status dei tasti,  
	 * \		delle angolazioni del Wii e le fonti luminose ad infrarossi rilevate dalla camera del Wii.
	 */
	bool StartReportModeContinuousMotionIR();

    /** 
	 * \fn		bool StartReportModeButtonPress()
	 * \brief	Richiede al Wii di inviare un report contenente lo status dei suoi tasti, e di inviarlo soltanto
	 * \		quando tale status varia.
	 */    
	bool StartReportModeButtonPress();

	/** 
	 * \fn		int  getWiiBatteryLevel()
	 * \brief	Richiede al Wii di inviare un report contenente informazioni di controllo, tra cui il livello 
	 * \		della batteria. Ritorna la percentuale di carica rimanente della batteria del Wii.
	 */    
	int  getWiiBatteryLevel();
    
	void GetCalibratedAcceleration(float & x, float & y, float &z);	
	
	int   lastIRLight1X, lastIRLight1Y, lastIRLight2X, lastIRLight2Y;
    
	struct tExpansionReport
	{
		bool mAttachmentPluggedIn;
		bool mIREnabled;
		bool mSpeakerEnabled;
		bool mLED1On;
		bool mLED2On;
		bool mLED3On;
		bool mLED4On;
		unsigned char mBatteryLevel;
		
		void Init()
		{
			mAttachmentPluggedIn = false;
			mIREnabled = false;
			mSpeakerEnabled = false;
			mLED1On = false;
			mLED2On = false;
			mLED3On = false;
			mLED4On = false;
			mBatteryLevel = 0;
		}
	};
    struct tButtonStatus
	{
		bool mA;
		bool mB;
		bool m1;
		bool m2;
		bool mPlus;
		bool mMinus;
		bool mHome;
		bool mUp;
		bool mDown;
		bool mLeft;
		bool mRight;
		
		void Init()
		{
			mA = mB = m1 = m2 = mPlus = mMinus = mHome = mUp = mDown = mLeft = mRight = false;
		}
	};

	struct tMotionReport
	{
		unsigned char mX;
		unsigned char mY;
		unsigned char mZ;
		
		void Init()
		{
			mX = mY = mZ = 0;
		}
	};
	struct tIRReport
	{
		unsigned short mP1X;
		unsigned short mP1Y;
		
		unsigned short mP2X;
		unsigned short mP2Y;

		unsigned char mP1Size;
		unsigned char mP2Size;

		bool mP1Found;
		bool mP2Found;

		void Init()
		{
			mP1X = mP1Y = mP2X = mP2Y = mP1Size = mP2Size = 0;
			mP1Found = mP2Found = false;
		}


	};

    
	const tButtonStatus & GetLastButtonStatus() const			{ return mLastButtonStatus; }
	const tMotionReport & GetLastMotionReport() const			{ return mLastMotionReport; }
	const tExpansionReport & GetLastExpansionReport() const		{ return mLastExpansionReport; }
	const tIRReport & GetLastIRReport() const					{ return mLastIRReport; }

private:
	
	wxEvtHandler* Handler;
	void GoWii();
	HANDLE sem;

	
	bool mIRRunning;
	bool mDataStreamRunning;

	// per settare il timeStamp
	__int64 clockFrequency;

	//__int64 lastWiimoteTime;

    //low level tasks
	bool UpdateOutput();
	bool SelectInputChannel(bool continuous, unsigned char channel);
	bool ReadMemory(unsigned int address, unsigned short size, unsigned char * buffer) const;	
	bool WriteMemory(unsigned int address, unsigned char size, const unsigned char * buffer);

	bool EnableIR();
	bool DisableIR();

	//output requests
	struct tOutputControls
	{
		bool mVibration;
		bool mLED1;
		bool mLED2;
		bool mLED3;
		bool mLED4;
		
		void Init()
		{
			mVibration = mLED1 = mLED2= mLED3= mLED4 = false;
		}
	};
	
	//parsing functions for input reports
	void ParseExpansionReport(const unsigned char * data);
	void ParseButtonReport(const unsigned char * data);
	void ParseMotionReport(const unsigned char * data);
	void ParseReadData(const unsigned char * data);
	void ParseIRReport(const unsigned char * data);
	
	//housekeeping functions
	void Init();
	void ClearBuffer();

	bool ReadData(unsigned int address, unsigned short size, unsigned char * buffer);	
	bool SendAReadRequestToWiimote(unsigned int address, unsigned short size, unsigned char * buffer);
	bool ReadCalibrationData();
    
	//flash reading vars
	struct tMemReadInfo
	{
		enum eReadStatus
		{
			READ_PENDING,
			READ_NONE,
			READ_COMPLETE,
			READ_ERROR
		} mReadStatus;
        
		unsigned char * mReadBuffer;
		unsigned short mTotalBytesToRead;
		unsigned short mBytesRead;
		unsigned short mBaseAddress;

		void Init()
		{
			mReadStatus = READ_NONE;
			mReadBuffer = NULL;
			mTotalBytesToRead = 0;
			mBytesRead = 0;
			mBaseAddress = 0;
		}
	
	} mReadInfo;

	//calibration data for the wiimote
    struct tAccelCalibrationData
	{
		unsigned char mXZero;
		unsigned char mYZero;
		unsigned char mZZero;
		unsigned char mXG;
		unsigned char mYG;
		unsigned char mZG;
		
		void Init()
		{
			mXZero = mYZero = mZZero = mXG = mYG = mZG= 0;
		}
	
	} mAccelCalibrationData;
        		
	//input states
	tExpansionReport	mLastExpansionReport;
	tButtonStatus		mLastButtonStatus;
	tMotionReport		mLastMotionReport;
	tIRReport			mLastIRReport;

	//output states
	tOutputControls		mOutputControls;
        
	//our communications device	
	HIDdevice HID;

	static const int mOutputBufferSize = 22;
	unsigned char mOutputBuffer[mOutputBufferSize];
	static const int mInputBufferSize = 22;
	unsigned char mInputBuffer[mInputBufferSize];

};


#endif

