/**
 * \file wxSurfaceRenderingGui.h
 * \brief File per la creazione della finestra 3D contenente il risultato del surface rendering
 * \author ICAR-CNR Napoli
 */

#ifndef _wxSurfaceRenderingGui_h_
#define _wxSurfaceRenderingGui_h_


#include "wx2dGui.h"
#include "wxVtkViewer3d.h"
#include "wxWiiManager.h"
#include <vtkAxes.h>

/** 
 * \class wxSurfaceRenderingGui
 * \brief Classe che estende la classe wxFrame e gestisce la visualizzazione del risultato del surface rendering
 */
class wxSurfaceRenderingGui : public wxFrame {
	/** 
	 * \var wxToolBar* _surfaceToolbar
	 * \brief Toolbar
	 */
	wxToolBar* _surfaceToolbar;

	/** 
	 * \var wxRadioBox* _itemRadioBoxProjectionType
	 * \brief Radiobox per il cambio del tipo di proiezione
	 */
	wxRadioBox* _itemRadioBoxProjectionType;

	/** 
	 * \var wxComboBox* _comboBoxZoom
	 * \brief Combobox per la scelta dello zoom
	 */
	//wxComboBox* _comboBoxZoom;

	/** 
	 * \var wxMenu* _menuProjectionType
	 * \brief Menu per la scelta del tipo di proiezione
	 */
	wxMenu* _menuProjectionType;

	/** 
	 * \var wxMenu* _menuStereoMode
	 * \brief Menu per la scelta del tipo di stereoscopia
	 */
	wxMenu* _menuStereoMode;

	/** 
	 * \var wxVtkViewer3d* _viewer3d
	 * \brief Visualizzatore 3D
	 */
	wxVtkViewer3d* _viewer3d;

	/** 
	 * \var unsigned int _idViewer
	 * \brief Identificativo del visualizzatore
	 */
	unsigned int _idViewer;

	/** 
	 * \var unsigned int _idData
	 * \brief Identificativo del dato
	 */
	unsigned int _idData;

	/** 
	 * \var wxMainGui* _mainGui
	 * \brief Finestra principale
	 */
	wxMainGui* _mainGui;

	/** 
	 * \var short _selectedButton
	 * \brief Indica quale bottone è stato selezionato nella toolbar
	 */
	short _selectedButton;

	/** 
	 * \var double _zoomValuePerspectiveSR
	 * \brief valore di zoom attivo Perspective mode
	 */
	//double _zoomValuePerspectiveSR;

	/** 
	 * \var double _zoomValueParallelSR
	 * \brief valore di zoom attivo Parallel mode
	 */
	//double _zoomValueParallelSR;

		/** 
	 * \var double _viewAngle
	 * \brief valore di zoom nel Perspective mode
	 */
	//double _viewAngle;

	/** 
	 * \var double _parallelScale
	 * \brief valore di zoom nel Parallel mode
	 */
	//double _parallelScale;

	/** 
	 * \var double _distance
	 * \brief distanza iniziale tra camera e volume
	 */
	double _distance;

	/** 
	 * \var int _POD
	 * \brief Distanza tra l'osservatore e lo schermo
	 */
	int _POD;

	/** 
	 * \var int _parallax
	 * \brief valore finto della parallasse
	 */
	int _parallax;


	/** 
	 * \var double _globalZoomFactor
	 * \brief fattore di zoom a cui si trova la scena
	 */
	double _globalZoomFactor;

	/** 
	 * \var vtkActor*  _3DCursor
	 * \brief Actor rappresentante il cursore 3D
	 */
	vtkActor*  _3DCursor;	
	
	/** 
	 * \var vtkActor*  _3DAxesActor
	 * \brief Actor rappresentante gli assi 3D
	 */
	vtkActor*	_3DAxesActor;
	vtkAxes*	_3DAxes;

	/** 
	 * \var bool _3DCursorIsOnModelSurface;
	 * \brief vale true se il cursore 3D si trova sulla superficie del modello
	 */
	bool _3DCursorIsOnModelSurface;

	/** 
	 * \var vtkCellLocator*	_cellLocator
	 * \brief Struttura utilizzata per la ricerca spaziale di celle 3D
	 */
	vtkCellLocator*	_cellLocator;	

	/** 
	 * \fn void getBoundsDepthInfo(double & closest, double & farthest)
	 * \brief Fornisce in formazioni sui bound del modello principale
	 * \param closest in uscita restituisce il depth del bound più vicino alla camera
	 * \	  farthest in uscita contiene il depth del bound più lontano dalla camera
	 */
	void getBoundsDepthInfo(double & closest, double & farthest);

	/** 
	 * \fn void create3DCursor()
	 * \brief crea gli oggetti necessari per la gestione del cursore 3D
	 */
	void create3DCursor();

	/** 
	 * \fn void destroy3DCursor()
	 * \brief distrugge gli oggetti necessari per la gestione del cursore 3D
	 */
	void destroy3DCursor();

	
	/** 
	 * \fn void create3DAxes()
	 * \brief crea gli assi 3D
	 */
	void create3DAxes();

	/** 
	 * \fn void create3DAxes()
	 * \brief distrugge gli assi 3D
	 */
	void destroy3DAxes();

	double	_shadingAmbient;
	double	_shadingDiffuse;
	double	_shadingSpecular;
	double	_shadingSpecularPower;
	wxColour _shadingColourAmbient;
	wxColour _shadingColourDiffuse;
	wxColour _shadingColourSpecular;

public:

	/** Costruttore con parametri il titolo, la posizione e la dimensione della finestra */
	wxSurfaceRenderingGui(const wxString& title, const wxPoint& pos, const wxSize& size);

	/** Distruttore */
	~wxSurfaceRenderingGui();

	inline wxColour getShadingColourAmbient() {
		return _shadingColourAmbient;
	}

	inline wxColour getShadingColourDiffuse() {
		return _shadingColourDiffuse;
	}

	inline wxColour getShadingColourSpecular() {
		return _shadingColourSpecular;
	}

	inline void setShadingColourAmbient(wxColour shadingColourAmbient) {
		_shadingColourAmbient = shadingColourAmbient;
	}

	inline void setShadingColourDiffuse(wxColour shadingColourDiffuse) {
		_shadingColourDiffuse = shadingColourDiffuse;
	}

	inline void setShadingColourSpecular(wxColour shadingColourSpecular) {
		_shadingColourSpecular = shadingColourSpecular;
	}


	/**
	* \fn void show(unsigned int idData)
	* \brief Metodo che mostra la finestra contenente il visualizzatore
	* \param idData L'identificativo del dato da visualizzare
	*/
	void show(float contourValue, double decimateValue, double smoothValue);

	/**
	* \fn void CreateSurfaceToolbar()
	* \brief Questo metodo crea la toolbar
	*/
	void CreateSurfaceToolbar();

	/**
	* \fn void Createmenu()
	* \brief Metodo che crea il menu della finestra
	*/
	void Createmenu();

	/**
	* \fn wxBitmap GetBitmapResource(const wxString& name)
	* \brief Metodo che ricava una wxBitmap a partire dal percorso in cui si trova il file xpm (se incluso)
	* \param name Nome della bitmap
	*/
	wxBitmap GetBitmapResource(const wxString& name);

	/**
	* \fn wxVtkViewer3d* new3dViewer(wxMainGui* mainGui)
	* \brief Associa il puntatore mainGui alla finestra padre e crea un nuovo visualizzatore 3D associato a mainGui
	* \param mainGui Finestra principale
	* \return Puntatore al visualizzatore 3D
	*/
	wxVtkViewer3d* new3dViewer(wxMainGui* mainGui);

	/**
	* \fn void onChar(wxKeyEvent & event)
	* \brief gestisce gli eventi associati alla pressione di un tasto
	* \param event Evento
	*/
	void onChar(wxKeyEvent & event);

	/**
	* \fn void onFullScreen(wxCommandEvent& event)
	* \brief Visualizza il volume a schermo intero
	* \param event Evento
	*/
	void onFullScreen(wxCommandEvent& event);

	/**
	* \fn void onWiiConfiguration(wxCommandEvent& event)
	* \brief Configura i Wiimote
	* \param event Evento
	*/
	void onWiiConfiguration(wxCommandEvent& event);

	/**
	* \fn void onWii(wxCommandEvent& event)
	* \brief Abilita l'utilizzo del Wiimote
	* \param event Evento
	*/
	void onWii(wxCommandEvent& event);

	/**
	* \fn onModifyCDRatioFunction(wxCommandEvent& WXUNUSED(event))
	* \brief modifica la funzione CDRatio
	*/
	void onModifyCDRatioFunction(wxCommandEvent& WXUNUSED(event));

	/**
	* \fn onModifyMotionFactor(wxCommandEvent& WXUNUSED(event))
	* \brief modifica il Motion Factor
	*/
	void onModifyMotionFactor(wxCommandEvent& WXUNUSED(event));

	void onWiiChangeSceneIRInteraction(wxCommandEvent& event);

	/**
	* \fn wxVtkViewer3d* get_viewer3d()
	* \brief ritorna il valore di _viewer3d
	* \param event Evento
	*/
	wxVtkViewer3d* get_viewer3d() { return _viewer3d; };

	/**
	* \fn void onStereo(wxCommandEvent& event)
	* \brief Imposta la modalità stereoscopica
	* \param event Evento
	*/
	void onStereo(wxCommandEvent& event);

	/**
	* \fn void onChangeInputDevice(wxCommandEvent& event)
	* \brief Cambia il dispositivo di input utilizzato
	* \param event Evento
	*/
	void onChangeInputDevice(wxCommandEvent& event);

	/**
	* \fn void onEyeAngle(wxCommandEvent& event)
	* \brief Imposta l'angolo di visualizzazione stereoscopica
	* \param event Evento
	*/
	void onEyeAngle(wxCommandEvent& event);

	/**
	* \fn void onCursorType(wxCommandEvent& event)
	* \brief Imposta il tipo di cursore 2D/3D
	* \param event Evento
	*/
	void onCursorType(wxCommandEvent& event);

	/**
	* \fn void onStereoModeAnaglyph(wxCommandEvent& event)
	* \brief Imposta la modalità stereoscopica anaglifica
	* \param event Evento
	*/
	void onStereoModeAnaglyph(wxCommandEvent& event);

	/**
	* \fn void onStereoModeActivePassive(wxCommandEvent& event)
	* \brief Imposta la modalità stereoscopica per schede grafiche quad-buffered
	* \param event Evento
	*/
	void onStereoModeActivePassive(wxCommandEvent& event);

	/**
	* \fn void onStereoModeCheckerboard(wxCommandEvent& event)
	* \brief Imposta la modalità stereoscopica Checkerboard
	* \param event Evento
	*/
	void onStereoModeCheckerboard(wxCommandEvent& event);

	/**
	* \fn void updateStereoView(bool doARender)
	* \brief aggiorna eye angle e parallax della camera		
	* \param doARender richiede o meno un render
	*/
	void updateStereoView(bool doARender = false);

	/**
	* \fn void setInitialDistance()
	* \brief setta la distanza iniziale tra camera e volume
	*/
	void setInitialDistance();

	/**
	* \fn void setInitialProjectionValues()
	* \brief setta il viewAngle e la parallelScale iniziali
	*/
	void setInitialProjectionValues();

	/**
	* \fn void onProjectionTypeParallel(wxCommandEvent& event)
	* \brief Imposta il tipo di proiezione parallela
	* \param event Evento
	*/
	void onProjectionTypeParallel(wxCommandEvent& event);

	/**
	* \fn void onProjectionTypePerspective(wxCommandEvent& event)
	* \brief Imposta il tipo di proiezione prospettica
	* \param event Evento
	*/
	void onProjectionTypePerspective(wxCommandEvent& event);

	void onProjectionTypeEndoscopy(wxCommandEvent& event);

	/**
	* \fn void onResetView(wxCommandEvent& event)
	* \brief Resetta il modello volumenntrico
	* \param event Evento
	*/
	void onResetView(wxCommandEvent& event);

	/**
	* \fn void onDolly(wxCommandEvent& event)
	* \brief Abilita la funzionalità dolly
	* \param event Evento
	*/
	void onDolly(wxCommandEvent& event);

	/**
	* \fn void onZoom(wxCommandEvent& event)
	* \brief Abilita la funzionalità zoom
	* \param event Evento
	*/
	void onZoom(wxCommandEvent& event);

	/**
	* \fn void updateZoom(double factor, bool doARender)
	* \brief Esegue uno zoom con un fattore factor
	* \param double factor, bool doARender
	*/
	void updateZoom(double factor, bool doARender = true);

	/**
	* \fn void onRotate(wxCommandEvent& event)
	* \brief Abilita la funzionalità rotate
	* \param event Evento
	*/
	void onRotate(wxCommandEvent& event);

	/**
	* \fn void onMove(wxCommandEvent& event)
	* \brief Abilita la funzionalità move
	* \param event Evento
	*/
	void onMove(wxCommandEvent& event);

	/**
	* \fn void onRotateAround(wxCommandEvent& event)
	* \brief Abilita la funzionalità rotate around
	* \param event Evento
	*/
	void onRotateAround(wxCommandEvent& event);

	/**
	* \fn void onAxialView(wxCommandEvent& event)
	* \brief Imposta la vista assiale del modello volumetrico
	* \param event Evento
	*/
	void onAxialView(wxCommandEvent& event);

	/**
	* \fn void onCoronalView(wxCommandEvent& event)
	* \brief Imposta la vista coronale del modello volumetrico
	* \param event Evento
	*/
	void onCoronalView(wxCommandEvent& event);

	/**
	* \fn void onSagittalView(wxCommandEvent& event)
	* \brief Imposta la vista sagittale del modello volumetrico
	* \param event Evento
	*/
	void onSagittalView(wxCommandEvent& event);

	/**
	* \fn void onOppositeSagittalView(wxCommandEvent& event)
	* \brief Imposta la vista sagittale opposta del modello volumetrico
	* \param event Evento
	*/
	void onOppositeSagittalView(wxCommandEvent& event);

	/**
	* \fn void onShowAboutDialog(wxCommandEvent& WXUNUSED(event))
	* \brief Visualizza le informazioni su MITO
	* \param event Evento
	*/
	void onShowAboutDialog(wxCommandEvent& WXUNUSED(event));


	/**
	* \fn void onModifyObserverDistance(wxCommandEvent& event)
	* \brief Modifica la distanza tra osservatore e schermo
	* \param event Evento
	*/
	void onModifyObserverDistance(wxCommandEvent& event);

	/**
	* \fn void onClose(wxCommandEvent& event)
	* \brief Chiude la finestra
	* \param event Evento
	*/
	void onClose(wxCommandEvent& event);

	/**
	* \fn void onQuit(wxCloseEvent& event)
	* \brief Chiude la finestra
	* \param event Evento
	*/
	void onQuit(wxCloseEvent& event);

	/**
	* \fn void onActivate(wxActivateEvent& event)
	* \brief Indica che la finestra 3D è attiva
	* \param event Evento
	*/
	void onActivate(wxActivateEvent& event);


	/**
	* \fn void setIdViewer(unsigned int idViewer)
	* \brief Metodo che assegna l'identificativo del visualizzatore
	* \param idViewer Identificativo del visualizzatore
	*/
	inline void setIdViewer(unsigned int idViewer) {
			_idViewer=idViewer;
	}

	/**
	* \fn void setIdData(unsigned int idData)
	* \brief Assegna l'identificativo del dato
	* \param idData Identificativo del dato
	*/
	inline void setIdData(unsigned int idData) {
		_idData = idData;
	}

	/**
	* \fn inline double getGlobalZoomFactor()
	* \brief restituisce il valore attuale del _globalZoomFactor
	*/
	inline double getGlobalZoomFactor() {
		return _globalZoomFactor;
	}

	/**
	* \fn inline double setGlobalZoomFactor(double factor)
	* \brief imposta il nuovo valore di _globalZoomFactor
	* \param factor il nuovo valore da impostare
	*/
	inline void setGlobalZoomFactor(double factor) {
		_globalZoomFactor = factor;
	}

	/**
	* \fn bool intersectingModelSurface(long x, long y, double* point)
	* \brief costruisce un raggio ortogonale allo schermo passante nel punto (x,y) dello schermo
	* \		 e restituisce true se tale raggio interseca la superficie del modello, altrimenti 
	* \		 restituisce false. Se restituisce true, point conterrà il punto di intersezione 
	* \		 espresso in coordinate spaziali.
	* \param x l'ascissa in coordinate video del raggio da costruire
	* \		 y l'ordinata in coordinate video del raggio da costruire
	* \		 point il punto spaziale di intersezione sulla superficie del modello
	*/
	bool intersectingModelSurface(long x, long y, double* point);

	/**
	* \fn bool set3DCursorPosition(long x, long y, bool flipY = false)
	* \brief imposta la nuova posizione del cursore 3D, restituisce true se la nuova posizione ricade 
	* \		 sulla superficie del modello principale, altrimenti restituisce false
	* \param x l'ascissa in coordinate video della nuova posizione del cursore 3D
	* \		 y l'ordinata in coordinate video della nuova posizione del cursore 3D
	* \		 flipY indica se la y dev'essere invertita o meno rispetto a size[1]
	*/
	bool set3DCursorPosition(long x, long y, bool flipY = false);

	/**
	* \fn void get3DCursorDisplayPosition(int & x, int & y, bool flipY = false);
	* \brief calcola la posizione in coordinate display del cursore 3D
	* \param x conterrà l'ascissa in coordinate display della posizione del cursore 3D
	* \		 y conterrà l'ordinata in coordinate display della posizione del cursore 3D
	* \		 flipY indica se la y dev'essere invertita o meno rispetto a size[1]
	*/
	void get3DCursorDisplayPosition(int & x, int & y, bool flipY = false);

	/**
	* \fn bool is3DCursorBeyondFocalPlane()
	* \brief restituisce true se il cursore 3D si trova oltre il piano focale, false altrimenti
	*/
	bool is3DCursorBeyondFocalPlane();

	/**
	* \fn inline bool is3DCursorOnModelSurface()
	* \brief restituisce il valore di _3DCursorIsOnModelSurface
	*/
	inline bool is3DCursorOnModelSurface() {
		return _3DCursorIsOnModelSurface;
	}

	/**
	* \fn void show3DCursor()
	* \brief rende visibile il cursore 3D
	*/
	void show3DCursor();

	/**
	* \fn void hide3DCursor()
	* \brief rende invisibile il cursore 3D
	*/
	void hide3DCursor();

	/**
	* \fn void hide3DAxes()
	* \brief rende invisibile gli assi 3D
	*/
	void hide3DAxes();


	/**
	* \fn void show3DAxes()
	* \brief rende visibile gli assi cursore 3D
	*/
	void show3DAxes();

	/**
	* \fn int set3DCursorRGBColor(double R, double G, double B)
	* \brief setta il colore del cursore 3D
	* \param R,G,B terna RGB
	*/
	void set3DCursorRGBColor(double R, double G, double B);

	void updateShading(double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	void onChangeShadingParameters(wxCommandEvent& event);

	void computeSurfaceRendering(float contourValue, double decimateValue, double smoothValue);

	inline void getShadingValues(double &ambient, double &diffuse, double &specular, double &specularPower) {
		ambient = _shadingAmbient;
		diffuse = _shadingDiffuse;
		specular = _shadingSpecular;
		specularPower = _shadingSpecularPower;
	}

	inline void getDecimateSmoothingValues(double &decimate, double &smooth) {
		decimate = _decimateValue;
		smooth = _smoothValue;
	}


	/**
	* \fn void setMinPixel(unsigned int minPixel)
	* \brief Assegna la minima intensità del pixel
	* \param minPixel Minima intensità del pixel
	*/
	inline void setMinPixel(unsigned int minPixel) {
		_minPixel = minPixel;
	}

	/**
	* \fn void setMaxPixel(unsigned int maxPixel)
	* \brief Assegna la massima intensità del pixel
	* \param maxPixel Massima intensità del pixel
	*/
	inline void setMaxPixel(unsigned int maxPixel) {
		_maxPixel = maxPixel;
	}

	/**
	* \fn int getMinPixel()
	* \brief Restituisce la minima intensità del pixel
	* \return Minima intensità del pixel
	*/
	inline int getMinPixel() {
		return _minPixel;
	}

	/**
	* \fn int getMaxPixel()
	* \brief Restituisce la massima intensità del pixel
	* \return Massima intensità del pixel
	*/
	inline int getMaxPixel() {
		return _maxPixel;
	}

	inline float getContourValue() {
		return _contourValue;
	}

	inline void setContourValue(float contourValue) {
		_contourValue = contourValue;
	}

	inline void setPOD(int POD) {
		_POD = POD;
	}

	inline int getPOD() {
		return _POD;
	}

private:
		
	DECLARE_EVENT_TABLE()

	/** 
	 * \var int _minPixel
	 * \brief Minima intensità dei pixel
	 */
	int _minPixel;

	/** 
	 * \var int _maxPixel
	 * \brief Massima intensità dei pixel
	 */
	int _maxPixel;

	float _contourValue;
	double _decimateValue;
	double _smoothValue;

	double _initialOrientation[3];
	double _initialScale[3];
	double _initialPosition[3];

};

enum {
	id3d_zoomTool = 0,
	id3d_dollyTool,
	id3d_rotateTool,
	id3d_moveTool,
	id3d_3drotTool,
	id3d_WiiTool,
	id3d_WiiConfigurationTool,
	id3d_WiiChangeSceneIRInteraction,
	id3d_ModifyCDRatioFunction,
	id3d_ModifyMotionFactor,
	id3d_AxialTool,
	id3d_CorTool,
	id3d_SagTool,
	id3d_sagoppTool,
	id3d_surfaceToolbar3D,
	id3d_fileCloseWindow,
	id3d_3dviewerSurface,
	id3d_RADIOBUTTONGROUPInputDevice1,
	id3d_RADIOBUTTONGROUPInputDevice2,
	id3d_stereo,
	id3d_perspective,
	id3d_parallel,
	id3d_endoscopy,
	id3d_RADIOBOXProjectionType,
	id3d_RADIOBUTTONGROUPCursorType1,
	id3d_RADIOBUTTONGROUPCursorType2,
	id3d_shadingChangeButton,
	id3d_shadingAmbientText,
	id3d_shadingSpecularText,
	id3d_shadingDiffuseText,
	id3d_contourValueText,
	id3d_HelpAbout,
	m_3d_3dviewerResetToInitialView,
	m_3d_3dviewerCoronalView,
	m_3d_3dviewerLeftSagittalView,
	m_3d_3dviewerRightSagittalView,
	m_3d_3dviewerAxialView,
	m_3d_3dviewerFullScreen,
	m_3d_CorTool,
	m_3d_SagTool,
	m_3d_sagoppTool,
	m_3d_AxialTool,
	m_3d_VOISR,
	m_3d_ZoomSR,
	m_3d_DollySR,
	m_3d_RotateSR,
	m_3d_MoveSR,
	m_3d_RotateAroundSR,
	m_3d_Mouse_Button_FunctionSR,
	m_3d_StereoMode,
	m_3d_StereoModeOff,
	m_3d_StereoModeOn,
	m_3d_StereoModeAnaglyph,
	m_3d_StereoModeActivePassive,
	m_3d_StereoModeCheckerboard,
	m_3d_ModifyObserverDistance
};


#endif _wxSurfaceRenderingGui_h_


