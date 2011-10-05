/**
 * \file appWxVtkInteractor.h
 * \brief File per l'interazione dell'utente con la GUI
 * \author ICAR-CNR Napoli
 */

#ifndef _appWxVtkInteractor_h_
#define _appWxVtkInteractor_h_

#include "colorTransferTable.h"
#include "roiManager.h"
#include "segmentationManager.h"
#include "viewerHandler.h"
#include "wxVTKRenderWindowInteractor.h"
#include <vtkColorTransferFunction.h>

#include	"wxWiimoteEvent.h"
#include	"wxWiiManager.h"	
#include	"vtkInteractorStyleWII.h"
//#include	"wxVocalDecoder.h"
#include	"vtkCellLocator.h"

#include "vtkDiskSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"

/**
 * \class appWxVtkInteractor
 * \brief Classe per l'interazione dell'utente con la GUI
 *
 * Questa classe deriva da wxVTKRenderWindowInteractor e gestisce
 * l'nterazione dell'utente con la GUI sia per le funzionalità
 * 2D (come per la ROI) che per quelle 3D
 */
class appWxVtkInteractor : public wxVTKRenderWindowInteractor {

private:
	/** 
	 * \var wxWindow* _parent
	 * \brief Finestra padre
	 */
	wxWindow* _parent;

	/** 
	 * \var wxPoint _actualPoint
	 * \brief Memorizza l'ultima posizione del mouse dopo un click
	 */
	wxPoint _actualPoint;

	/** 
	 * \var wxTimer *m_focusTimer
	 * \brief Timer per temporizzare il focus
	 */
	//wxTimer	*m_focusTimer;

	/** 
	 * \var roiManager* _roiManager
	 * \brief Gestore delle funzionalità per la ROI
	 */
	roiManager* _roiManager;

	/** 
	 * \var segmentationManager* _segmentationManager
	 * \brief Gestore delle funzionalità per la segmentazione
	 */
	segmentationManager* _segmentationManager;

	/** 
	 * \var viewerHandler* _viewerHandler
	 * \brief Gestore dei visualizzatori
	 */
	viewerHandler* _viewerHandler;

	/** 
	 * \var short _3dTechnique
	 * \brief indica la tecnica utilizzata per la creazione del modello 3D
	 */
	short _3dTechnique;

	/** 
	 * \var short _interactionType
	 * \brief Modalità di interazione
	 */
	short _interactionType;

	/** 
	 * \var int _clut
	 * \brief Indica il tipo di CLUT da applicare al volume
	 */
	int _clut;

	/** 
	 * \var long _wl
	 * \brief Valore del window level
	 */
	long _wl;

	/** 
	 * \var long _ww
	 * \brief Valore del window width
	 */
	long _ww;

	/** 
	 * \var bool _wlWwModified
	 * \brief Indica se sono stati modificati i valori di window level o window width
	 */
	bool _wlWwModified;

	/** 
	 * \var int _lastMousePosition[2]
	 * \brief posizione precedente (x,y) del mouse
	 */
	int _lastMousePosition[2];

	/** 
	 * \var bool _clockwise
	 * \brief indicativa del verso di rotazione
	 */
	bool _clockwise;

	/** 
	 * \var bool _3DCursorCanMove
	 * \brief indica se il cursore 3D può muoversi
	 */
	bool _3DCursorCanMove;

	/** 
	 * \var bool _3DCursorCanMove
	 * \brief indica se il cursore 3D è abilitato
	 */
	bool _3DCursorEnabled;

public:
	/** Costruttore con parametri GUI padre, identificativo della finestra e gestore dei visualizzatori */
	appWxVtkInteractor(wxWindow* parent, wxWindowID id, viewerHandler* vHandler);

	/** Distruttore */
	~appWxVtkInteractor();
	
	// Da dichiarare virtual nella classe padre

	/**
	 * \fn void OnMouseWheel(wxMouseEvent &event)
	 * \brief Invocato se si utilizza la rotella del mouse
	 * \param event Evento generato dal mouse
	 */
	void OnMouseWheel(wxMouseEvent &event);

	/**
	 * \fn void OnButtonDown(wxMouseEvent &event)
	 * \brief Invocato da un click del mouse
	 * \param event Evento generato dal mouse
	 */
	void OnButtonDown(wxMouseEvent &event);

	/**
	 * \fn void OnButtonUp(wxMouseEvent &event)
	 * \brief Invocato dopo il rilascio di un bottone del mouse
	 * \param event Evento generato dal mouse
	 */
	void OnButtonUp(wxMouseEvent &event);

	/**
	 * \fn void OnKeyDown(wxKeyEvent &event)
	 * \brief Invocato dopo aver premuto un tasto
	 * \param event Evento generato dalla tastiera
	 */
	void OnKeyDown(wxKeyEvent &event);

	/**
	 * \fn void OnMotion(wxMouseEvent &event)
	 * \brief Invocato dopo aver modificato la posizione del puntatore del mouse
	 * \param event Evento generato dal mouse
	 */
	void OnMotion(wxMouseEvent &event);

	/**
	 * \fn void OnEnterWindow(wxMouseEvent &event)
	 * \brief Invocato quando il puntatore del mouse entra nella finestra del viewer
	 * \param event Evento generato dal mouse
	 */
	void OnEnterWindow(wxMouseEvent &event);

	/**
	 * \fn void OnLeaveWindow(wxMouseEvent &event)
	 * \brief Invocato quando il puntatore del mouse esce dalla finestra del viewer
	 * \param event Evento generato dal mouse
	 */
	void OnLeaveWindow(wxMouseEvent &event);
//-----------------------------------------------Wii---------------------------------------------------------------------------------------------------

private:
	// numero di rendering al secondo
	int renderingRate;

// PRISM

//	void prismStyle1DFilter(int & lastVelocityTime, float interval, float & invCDRatio, float newX, float & oldX);
	void prismStyle1DFilter(float newPosition, float & oldPosition, float & invCDRatio, wxWiiManager::Wiimote* thisWii);
//	void prismStyle2DFilter(int & lastVelocityTime, float interval, float & invCDRatio, float newX, float & oldX, float newY, float & oldY);
	bool prismStyle2DFilter(float newX, float & oldX, float newY, float & oldY, float & invCDRatio, wxWiiManager::Wiimote* thisWii);
	
// fine PRISM

	/** 
	 * \var wxWiiManager* WiiManager
	 * \brief puntatore al Manager dei Wiimote collegati.	 
	 */
	wxWiiManager* WiiManager;

	double camPosition[3];
	double camViewUp[3];	

	// ultimo tempo di render
	__int64 _lastRenderTime;

	// frequenza di clock
	__int64 _clockFrequency;

	// per il log - lista che poi viene scritta sul file di log
	vector<string> list;

	// per il log - frequenza di clock
	__int64 _lastUpdatedTime;

	// per il log - generico istante di tempo
	__int64 _updatedTime;
	
	// per il log - istante iniziale dell'interazione
	__int64 _initialTime;


	/** 
	 * \bool enabledWiiX;
	 * \brief Utilizzarto per abilitare la rotazione lungo un asse per volta.
	 */
	bool enabled_angle_Rotation;

	/** 
	 * \bool enabledWiiY;
	 * \brief Utilizzarto per abilitare la rotazione lungo un asse per volta.
	 */
	bool enabled_ryf_Rotation;

	/** 
	 * \bool enabledWiiIR;
	 * \brief Utilizzarto per abilitare la rotazione lungo un asse per volta.
	 */
	bool enabled_rxf_Rotation;
	
	/** 
	 * \vtkActor2D*	interactionSignal
	 * \brief Attore 2D rappresentante il segnale di interazione.
	 */
	vtkActor2D*	interactionSignal;

	/** 
	 * \vtkTextActor* interactionSignalText
	 * \brief TextActor utilizzato per indicare il tipo di interazione in atto.
	 */
	vtkTextActor* interactionSignalText;	
	
	/** 
	 * \vtkActor2D*	rxfRotationAxis
	 * \brief Attore 2D rappresentante l'asse di rotazione rxf.
	 */
	vtkAxisActor2D*	rxfRotationAxis;

	/** 
	 * \vtkActor2D*	ryfRotationAxis
	 * \brief Attore 2D rappresentante l'asse di rotazione ryf.
	 */
	vtkAxisActor2D*	ryfRotationAxis;

	/** 
	 * \vtkActor2D*	angleRotationAxis
	 * \brief Mapper associato all' Actor2D rappresentante l'asse di rotazione angle.
	 */
	vtkActor2D*	angleRotationAxis;

	/** 
	 * \vtkActor2D* degreeRotationIndicator
	 * \brief Actor2D associato all' indicatore rappresentante i margini di rotazione imprimibili.
	 */
	vtkActor2D* degreeRotationIndicator;
	
	/**
	* \fn void	showInteractionSignal(int signalColor, char* text)	
	* \brief	Mostra l'indicatore del tipo di interazione in atto.
	* \param	signalColor		Indica il colore che deve avere l'indicatore.
	* \			text			Carattere che deve essere visualizzato dall'indicatore.
	*/
	void showInteractionSignal(int signalColor, char* text);

	/**
	* \fn	void hideInteractionSignal()
	* \brief	Nasconde l'indicatore del tipo di interazione in atto.
	*/
	void hideInteractionSignal();
	
	/**
	* \fn		showRotationIndicator(int rotationType, int signalColor = 0, float p0 = 0, float p = 0)
	* \brief	Mostra l'indicatore dell'oggetto in movimento lungo l'asse di rotazione scelto.
	* \param	rotationType	Indica l'asse di rotazione e quindi il tipo di movimento da effettuare.
	* \			signalColor		Indica il colore che deve avere l'indicatore in movimento.
	* \			p0				Posizione iniziale dell' oggetto 3D.
	* \			p				Posizione attuale  dell' oggetto 3D.
	*/
	void showRotationIndicator(int rotationType, int signalColor = 0, float p0 = 0, float p = 0);
	void showRotationIndicator(int rotationType, float p = 0, int signalColor = 0);

	/**
	* \fn		void hideRotationIndicator()
	* \brief	Nasconde l'indicatore dell'oggetto in movimento lungo l'asse di rotazione scelto.
	*/
	void hideRotationIndicator();

	/** 
	 * \bool InteractionSignalsAdded
	 * \brief Indica se gli indicatori visivi delle interazioni son stati aggiunti nella scena.
	 */
	bool InteractionSignalsAdded;

	/** 
	 * \bool isRenderingNeeded(int frameRate)
	 * \brief Indica se è necessario effettuare un render in base al frame rate in input
	 * \param frameRate Frame rate 
	 */
	bool isRenderingNeeded(int frameRate);

	/** 
	 * \void updateLastRenderTime()
	 * \brief Aggiorna all'attuale tempo di sistema, il valore del tempo a cui è stato effettuato l'ultimo render
	 * \param frameRate Frame rate 
	 */
	//void updateLastRenderTime(double delay=0);
	void updateLastRenderTime();

	
public:

	float _minDISP;
	float _maxDISP;

	/** 
	 * \var int idWiiEnabled
	 * \brief Indica l'id del Wiimote abilitato ad interagire col sistema. 
	 * \Porre a -1 per abilitare tutti i Wiimote
	 */
	int idWiiEnabled;

	bool _isSceneRotatingIR_1;

	/**
	 * \fn		wxWiiManager* getWiiManager(wxVtkViewer3d* viewer3d)
	 * \brief	Ritorna il puntatore al WiiManager, creando il manager se non esiste.
	 * \param	viewer3d	Puntatore al viewer 3d della scena, da passare al costruttore del WiiManager.
	 */
	wxWiiManager* getWiiManager(wxVtkViewer3d* viewer3d);

	/**
	 * \fn		void closeWiiManager()
	 * \brief	Distrugge il WiiManager.
	 */
	void closeWiiManager();

	/** 
	 * \var bool WiiManagerStarted
	 * \brief Vale true se il Manager dei Wii è operativo.
	 */
	bool WiiManagerStarted;


	/**
	 * \fn bool closeWiiAbility()
	 * \brief Se nessuno sta manipolando, chiude se occorre l'interazione vtkPointing
	 */
	bool closeWiiAbility();


	/**
	* \fn		void initInteractionSignals()
	* \brief	Inizializza ed aggiunge alla scena i segnali visivi relativi all' interazione.
	*/
	void initInteractionSignals();

	/**
	* \fn		void removeInteractionSignals()
	* \brief	Rimuove dalla scena i segnali visivi relativi all' interazione.
	*/
	void removeInteractionSignals();
	
	/**
	 * \fn void onWiiMotionIR(wxWiimoteEvent &event)
	 * \brief Invocato periodicamente dal Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiMotionIR			( wxWiimoteEvent & event );
	
	/**
	 * \fn void onWiiADown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante A del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiADown				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiAUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante A del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiAUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiBDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante B del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiBDown				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiBUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante B del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiBUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiMinusDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Minus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiMinusDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiMinusUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Minus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiMinusUp			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiHomeDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Home del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiHomeDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiHomeUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Home del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiHomeUp			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiPlusDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Plus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiPlusDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiPlusUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Plus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiPlusUp			( wxWiimoteEvent & event );	   

	/**
	 * \fn void onWiiOneDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante One del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiOneDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiOneUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante One del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiOneUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiTwoDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Two del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiTwoDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiTwoUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Two del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiTwoUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiCrossDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto la Croce del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiCrossDown			( wxWiimoteEvent & event ); 

	/**
	 * \fn void onWiiCrossUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato la Croce del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiCrossUp			( wxWiimoteEvent & event ); 

//------------------------------------------------------------------------------------------------------------------



	/**
	 * \fn void OnMotion(wxMouseEvent &event)
	 * \brief Invocato dopo aver modificato le dimensione della finestra
	 * \param event Evento generato dal ridimensionamento della finestra
	 */	
	void OnSize(wxSizeEvent &event);

	//void OnFocusTimer(wxTimerEvent& event);
	//void OnLeave(wxMouseEvent &event);

	/**
	 * \fn short getInteractionType()
	 * \brief Restituisce il valore della variabile _interactionType
	 * \return Valore della variabile _interactionType
	 */
	inline short getInteractionType() {
		return _interactionType;
	}

	/**
	 * \fn void setInteractionType(short interactionType)
	 * \brief Assegna l'interactionType
	 * \param interactionType Modalità di interazione da assegnare a _interactionType
	 */
	inline void setInteractionType(short interactionType) {
		_interactionType = interactionType;
	}

	/**
	 * \fn void set3dTechnique(short technique)
	 * \brief Assegna la tecnica 3D
	 * \param technique tecnica utilizzata per la creazione della rappresentazione 3d
	 */
	inline void set3dTechnique(short technique) {
		_3dTechnique = technique;
	}

	/**
	 * \fn short get3dTechnique()
	 * \brief Ritorna la 3dTechnique
	 * \return Valore della variabile 3dTechnique
	 */
	inline short get3dTechnique() {
		return _3dTechnique;
	}

	/**
	 * \fn void setWlWw(double wl, double ww)
	 * \brief Assegna window level e window width
	 * \param wl Window level
	 * \param ww Window width
	 */
	inline void setWlWw(double wl, double ww) {
		_wl = wl;
		_ww = ww;
	}

	/**
	 * \fn void getWl()
	 * \brief Restituisce il window level
	 */
	inline long getWl() {
		return _wl;
	}

	/**
	 * \fn void getWw()
	 * \brief Restituisce il window width
	 */
	inline long getWw() {
		return _ww;
	}

	/**
	 * \fn bool getWlWwModified()
	 * \brief Restituisce il valore della variabile _wlWwModified
	 * \reutrn Valore della variabile _wlWwModified
	 */
	inline bool getWlWwModified() {
		return _wlWwModified;
	}

	/**
	 * \fn void setWlWwModified()
	 * \brief Assegna il valore alla variabile _wlWwModified
	 * \param wlWwModified Valore da assegnare
	 */
	inline void setWlWwModified(bool wlWwModified) {
		_wlWwModified = wlWwModified;
	}

	/**
	 * \fn short getInteractionType()
	 * \brief Restituisce il gestore delle funzionalità per la ROI
	 * \return Gestore ROI
	 */
	inline roiManager* getRoiManager(){
		return _roiManager;
	}

	/**
	 * \fn short getInteractionType()
	 * \brief Restituisce il gestore delle funzionalità per la segmentazione
	 * \return Gestore segmentazione
	 */	
	inline segmentationManager* getSegmentationManager(){
		return _segmentationManager;
	}

	/**
	 * \fn void setSegmentationManager(segmentationManager* segmentationMngr)
	 * \brief Assegna il gestore delle funzionalità per la segmentazione
	 * \param segmentationMngr Gestore delle funzionalità per la segmentazione
	 */	
	inline void setSegmentationManager(segmentationManager* segmentationMngr) {
		_segmentationManager = segmentationMngr;
	}

	/**
	* \fn void setCLUT(int clut)
	* \brief Indica il tipo di CLUT
	* \param clut Tipo di CLUT
	*/
	inline void setCLUT(int clut) {
		_clut = clut;
	}

	/**
	 * \fn void 3DcursorOn()
	 * \brief Puntatore 3D attivo
	 */
	void set3DcursorOn() 
	{
		_3DCursorEnabled = true;
	}

	/**
	 * \fn void 3DcursorOff()
	 * \brief Puntatore 2D attivo
	 */
	void set3DcursorOff() 
	{
		_3DCursorEnabled = false;
	}

	/**
	 * \fn void get3DcursorMode()
	 * \brief Ritorna la modalità di puntamento
	 */
	bool get3DcursorMode() 
	{
		return _3DCursorEnabled;
	}

	void set3DcursorCanMove(bool move) 
	{
		_3DCursorCanMove = move;
	}

	/**
	 * \fn void dummyMethod()
	 * \brief metodo stupido e "temporaneo"
	*/
	void dummyMethod();

	DECLARE_EVENT_TABLE();
};

/**
* \brief Tipo enumerativo che rappresenta i differenti device di interazione utilizzabili
*/
enum interactionDevice {
	MouseDevice,
    WiimoteDevice
};

/**
* \brief Tipo enumerativo che rappresenta le differenti modalità con cui un utente può interagire con un finestra
*/
enum {
	all2d = 0,
	windowLevel2d,
	move2d,
	zoom2d,
	rotate2d,
	animate2d,
	length2d,
	angle2d,
	rectangle2d,
	polygon2d,
	pencil2d,
	connectedThreshold2d,
	neighborhoodConnected2d,
	confidenceConnected2d,
	windowLevel3d,
	move3d,
	dolly3dVR,
	dolly3dSR,

	zoom3dSR,
	zoom3dVR,

	rotate3d,
	rotateAround3d,
	rotateAround3dSR,
	voi3dVR,
	voi3dVRwii,
	voi3dSR,
	wii,
	wiiSR
};

/**
* \brief Tipo enumerativo che rappresenta il tipo di finestra 3d
*/
enum {
	Undecided = 0,
	VolumeRenderingMitoCinese,
	SurfaceRendering
};

//-----------------------------------------------Wii----------------------------------------------------------------
/**
* \brief Tipo enumerativo utilizzato per concedere il permesso di interazione a qualunque wiimote lo richieda 
* \e per rappresentare le differenti modalità con cui un wiimote può interagire nella scena 3d.
*/
enum {
	AnyWiiEnabled = -1,
	Waiting,
	SceneTranslating,
	SceneRotating,	
	SceneRotatingIR,
	SceneRotatingIR_1,
	SceneRolling,
	CameraDollyIn,
	CameraDollyOut,
	SceneZoomingIn,
	SceneZoomingOut,
	SceneVocalAcquiring,
	BoxWidgetSelecting,
	BoxWidgetTranslating,
	BoxWidgetRotating,
	Pointing
};
//------------------------------------------------------------------------------------------------------------------

#endif _appWxVtkInteractor_h_