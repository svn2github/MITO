/**
 * \file wxWiimanager.h
 * \brief File per la gestione del Manager che controlla tutti i Wiimote rilevabili
 */

#ifndef _wxWiiManager_h_
#define _wxWiiManager_h_

#include "wxWiiDriver.h"
#include "wxWiiManagerDialog.h"

#include <vtkRenderer.h>
#include <vtkCamera.h>

//#include <vtkConeSource.h> 
#include <vtkSphereSource.h> 

#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

#include <vtkAxisActor2D.h>


//#define  maxLatency		32
class	 wxVtkViewer3d;


class wxWiiManager
{       
public:	

	struct Wiimote
	{
		wxWiiDriver*		driver;
		bool				WiiStarted;			
		int					idWii;
		int					HIDTableIndex;
		int					Mode;
		int					pointerColor;
		int					batteryLevel;
		bool				isOnSurface;

		/** 
		* \var		int latencyEvent
		* \brief	Viene utilizzata nel caso si vogliano invocare eventi vtk con una cadenza diversa rispetto ai 
		* \			corrispondenti eventi provenienti dal Wiimote. Invoco un evento vtk ogni latencyEvent eventi 
		* \			ricevuti dal wiimote.
		*/
		//int latencyEvent;

		/** 
		* \var		int timeoutEvent
		* \brief	Quando utilizzata, impostandola pari a latencyEvent, ad ogni evento ripetitivo ricevuto dal 
		* \			Wiimote timeoutEvent viene decrementata e solo quando si annulla viene invocato il corrispondente 
		* \			evento vkt.
		*/
		//int timeoutEvent;

		/** 
		* \var		float lastWiiX[5]
		* \brief	Utilizzato per implementare un filtro che limiti le piccole oscillazioni del Wiimote non volute.
		*/
		float lastWiiX[5];

		/** 
		* \var		float lastWiiY[5]
		* \brief	Utilizzato per implementare un filtro che limiti le piccole oscillazioni del Wiimote non volute.
		*/
		float lastWiiY[5];

		/** 
		* \var		float lastIR_X[5]
		* \brief	Utilizzato per memorizzare il valore precedentemente puntato dal Wiimtoe.
		*/
		float lastIR_X[5];

		/** 
		* \var		float lastIR_Y[5]
		* \brief	Utilizzato per memorizzare il valore precedentemente puntato dal Wiimtoe.
		*/
		float lastIR_Y[5];

		/** 
		* \var		float WiiXo
		* \brief	Utilizzato per imporre al Wii un valore iniziale diverso da zero nel calcolo dell' inclinazione.
		*/
		float WiiXo;

		/** 
		* \var		float WiiYo
		* \brief	Utilizzato per imporre al Wii un valore iniziale diverso da zero nel calcolo dell' inclinazione.
		*/
		float WiiYo;

		/** 
		* \var		float IR_Xo
		* \brief	Utilizzato per imporre al Wii un valore iniziale diverso da zero nella rilevazione 
		* \			dell'ascissa dei led infrarossi.
		*/
		float IR_Xo;

		/** 
		* \var		float IR_Yo
		* \brief	Utilizzato per imporre al Wii un valore iniziale diverso da zero nella rilevazione 
		* \			dell'ordinata dei led infrarossi.
		*/
		float IR_Yo;

		// PRISM Needed
		__int64	lastVelocityTime;

		float interval;

		float xPositionVelocity,
			  yPositionVelocity,
			  zPositionVelocity;

		float invCDratio,
			  invCDRollRatio;
		
		float minDISP,
			  maxDISP,
			  minRotDISP,
			  maxRotDISP;
		// FINE PRISM


		// Pointer Data
		bool				renderDuty;		
//		vtkConeSource*		pointerGeometry;
		vtkSphereSource*	pointerGeometry;
		vtkDataSetMapper*	pointerMapper;
		vtkActor*			pointer;

		Wiimote*			next;
	};

	/**
	* \var		bool _isSceneRotatingIR_1
	* \brief	Vale true se è selezionata la modalità _isSceneRotatingIR_1
	*/
	bool _isSceneRotatingIR_1;

	vtkRenderer*	renderer;
	vtkCamera*		camera;
	wxVtkViewer3d*	viewer3d;

	/**
	* \var		int WiiPointing_num;
	* \brief	Utilizzata per conteggiare il numero di Wiimote che si trovano nello modalità Pointing.	
	*/
    int WiiPointing_num;

	/**
	* \var		double _pointerRadius
	* \brief	raggio iniziale della sfera rappresentante ogni pointer
	*/
	double _pointerRadius;

	static wxWiiManager* getIstance(wxVtkViewer3d* viewer3d);
    
	/** Distruttore */
	~wxWiiManager() 
	{ 			
		Close(); 
		Istance = NULL; 
		CloseHandle(sem);
	}

		
	/**
	* \fn		bool Start()
	* \brief	Crea ed Avvia un thread per ogni
	* \			Wiimote presente nella lista dei Wii da connettere
	*/
	bool Start();
    
	/**
	* \fn		bool Close()
	* \brief	Chiede ad ogni tread attivo di terminarsi
	* \			ed elimina tutte le strutture associate ad ogni Wii connesso		
	*/
	bool Close();

	/**
	* \fn		inline void setPointerRadius(double radius)
	* \brief	Imposta il valore di _pointerRadius
	*/
	inline void setPointerRadius(double radius) {
		if (radius>0) _pointerRadius = radius;
	}

	/**
	* \fn		bool SetupGui()
	* \brief	Visualizza un dialog per decide se e quali Wii connettere tra quelli rilevati
	*/
	bool SetupGui(wxWindow* win);		

	/**
	* \fn		bool QuickSetupGui()
	* \brief	Avvio rapido della modalità Wii -- connette tutti i Wiimote rilevati
	*/
	bool QuickSetupGui(wxWindow* win);	

	/**
	* \fn		void DisableTheOthersWii(int idWii)
	* \brief	Disabilita l'invio degli eventi da tutti i Wii aventi id diverso da quello indicato in ingresso.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii che non deve essere disabilitato.	
	*/
	void DisableTheOthersWii(int idWii);

	/**
	* \fn		void EnableTheOthersWii(int idWii)
	* \brief	Abilita l'invio degli eventi da tutti i Wii aventi id diverso da quello indicato in ingresso.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii che non deve essere abilitato.	
	*/
	void EnableTheOthersWii(int idWii);

	/**
	* \fn		bool AnotherWiiDoingRender(int idWii)
	* \brief	Ritorna true se esiste già un Wiimote responsabile del rendering della scena.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii escluso alla ricerca.	
	*/
	bool AnotherWiiDoingRender(int idWii);	

	/**
	* \fn		void ReleaseRenderDuty()
	* \brief	Toglie la responsabilità del rendering al Wii che la detiene.		
	*/
	void ReleaseRenderDuty();		
    
	/**
	* \fn		Wiimote* findWii(int idWii)
	* \brief	Ritorna il puntatore alla struttura che fa da interfaccia al Wiimote cercato.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii ricercato.	
	*/
	Wiimote* findWii(int idWii);

	/**
	* \fn		bool getLeds(int idWii, bool & Led1, bool & Led2, bool & Led3, bool & Led4)
	* \brief	Ritorna lo status dei led del wiimote identificato da idWii.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii interessato.
	* \			Led1			Varrà true o false se il relativo led del Wii è acceso o meno.
	* \			Led2			Varrà true o false se il relativo led del Wii è acceso o meno.
	* \			Led3			Varrà true o false se il relativo led del Wii è acceso o meno.
	* \			Led4			Varrà true o false se il relativo led del Wii è acceso o meno.
	*/
	bool getLeds(int idWii, bool & Led1, bool & Led2, bool & Led3, bool & Led4) ;		

	/**
	* \fn		bool getLed1(int idWii)
	* \brief	Ritorna true o false se il Led1 del wiimote, identificato da idWii, è acceso o meno.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool getLed1(int idWii);	

	/**
	* \fn		bool getLed2(int idWii)
	* \brief	Ritorna true o false se il Led2 del wiimote, identificato da idWii, è acceso o meno.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool getLed2(int idWii); 

	/**
	* \fn		bool getLed3(int idWii)
	* \brief	Ritorna true o false se il Led3 del wiimote, identificato da idWii, è acceso o meno.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool getLed3(int idWii);

	/**
	* \fn		bool getLed4(int idWii)
	* \brief	Ritorna true o false se il Led4 del wiimote, identificato da idWii, è acceso o meno.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool getLed4(int idWii);

	/**
	* \fn		bool setLeds(int idWii, bool Led1, bool Led2, bool Led3, bool Led4)
	* \brief	Imposta lo status dei led del wiimote identificato da idWii.
	* \
	* \param	idWii			Identificativo numerico indicante il Wii interessato.
	* \			Led1			Se vale true o false il relativo led del Wii verrà acceso o meno.
	* \			Led2			Se vale true o false il relativo led del Wii verrà acceso o meno.
	* \			Led3			Se vale true o false il relativo led del Wii verrà acceso o meno.
	* \			Led4			Se vale true o false il relativo led del Wii verrà acceso o meno.
	*/
	bool setLeds(int idWii, bool Led1, bool Led2, bool Led3, bool Led4);

	/**
	* \fn		bool setLed1(bool value, int idWii)
	* \brief	Imposta lo stato del Led1 del wiimote identificato da idWii.
	* \
	* \param	value			Se vale true o false il relativo il Led1 del Wii verrà acceso o meno.
	* \			idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool setLed1(bool value, int idWii);

	/**
	* \fn		bool setLed2(bool value, int idWii)
	* \brief	Imposta lo stato del Led2 del wiimote identificato da idWii.
	* \
	* \param	value			Se vale true o false il relativo il Led2 del Wii verrà acceso o meno.
	* \			idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool setLed2(bool value, int idWii);

	/**
	* \fn		bool setLed3(bool value, int idWii)
	* \brief	Imposta lo stato del Led3 del wiimote identificato da idWii.
	* \
	* \param	value			Se vale true o false il relativo il Led3 del Wii verrà acceso o meno.
	* \			idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool setLed3(bool value, int idWii);

	/**
	* \fn		bool setLed4(bool value, int idWii)
	* \brief	Imposta lo stato del Led4 del wiimote identificato da idWii.
	* \
	* \param	value			Se vale true o false il relativo il Led4 del Wii verrà acceso o meno.
	* \			idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	bool setLed4(bool value, int idWii);

	/**
	* \fn		void setVibration(int cont, int idWii)
	* \brief	Chiede al Wiimote, identificato da idWii, di vibrare per un tempo proporzionale a cont.
	* \
	* \param	cont			Indica la durata della vibrazione.
	* \			idWii			Identificativo numerico indicante il Wii interessato.		
	*/
	void setVibration(int cont, int idWii);	


private:

	wxEvtHandler* WiiEvtHandler; 

	static wxWiiManager* Istance;
	
	bool WiiManagerStarted;

    HANDLE sem;
    
	Wiimote*	first;

	int Wii_num;	

    /** Costruttore con parametri l' eventHandler a cui i wii devono mandare i loro eventi, ed il renderer della scena. */
	wxWiiManager(wxVtkViewer3d* viewer3d);
	/*
	{
		this->WiiEvtHandler = WiiEvtHandler;
		WiiManagerStarted = false;
		this->viewer3d	= viewer3d;
		this->renderer	= viewer3d->getRenderer();
		this->camera	= renderer->GetActiveCamera();

		first = (Wiimote*) NULL;
		sem = CreateEvent(NULL, FALSE, TRUE, NULL);
		Wii_num =0;
	}
	*/
	
        
	/**
	* \fn		int RilevaWii()
	* \brief	Ricerca nella HID device table di Windows se ci sono Wiimote collegati e nel caso li aggiunge 
	* \			nella lista dei Wii rilevati.
	*/
	int RilevaWii();
	
	/**
	* \fn		bool AddWii(int	idWii, int HIDTableIndex)
	* \brief	Aggiunge, alla lista dinamica dei Wiimote rilevati, il Wii trovato nella riga HIDTableIndex. 
	* \
	* \param	idWii			Identificativo numerico da associare al Wii trovato.
	* \			HIDTableIndex	Indice della riga nella HID device table, gestita da Windows,
	* \							contenente l' HID device associato al particolare Wii.
	* \			batteryLevel	Percentuale di carica residua delle batterie del Wiimote.
	*/
	bool AddWii(int	idWii, int HIDTableIndex, int batteryLevel);
    
	/**
	* \fn		void InitSelectedWii()
	* \brief	Inizializza le strutture associate ad ogni Wii che non erano state ancora inizializzate.
	* \			Viene richiama dopo che la lista dei Wii rilevati è stata scremata dai Wii non selezionati.
	*/
	void InitSelectedWii();

	/**
	* \fn		bool RemoveWii(int idWii)
	* \brief	Rimuove, dalla lista dinamica dei Wiimote rilevati, il Wii identificato da idWii. 
	* \
	* \param	idWii			Identificativo numerico associato al Wii da eliminare.
	*/
	bool RemoveWii(int idWii);
	
};

#endif