/**
 * \file wxVolumeRenderingGui.h
 * \brief File per la creazione della finestra 3D contenente il risultato del volume rendering
 * \author ICAR-CNR Napoli
 */

#ifndef _wxVolumeRenderingGui_h_
#define _wxVolumeRenderingGui_h_

#include "wxVtkViewer3d.h"
#include "wxMainGui.h"
#include "mipFilter.h"
//#include "minipFilter.h"
#include "volumeRenderingFilter.h"
#include <vtkBoxWidget.h>
#include <vtkPlanes.h>
#include "wxWiiManager.h"

#include <vtkAxes.h>

enum techniqueRayCasting {
	VolumeRendering,
	VolumeFixedPointRendering,
	MIP,
    VolumeTextureRendering,
	GPURendering,
};

/**
 * \class wxVolumeRenderingGui
 * \brief Classe che estende la classe wxFrame e gestisce la visualizzazione del risultato del volume rendering
 */
class wxVolumeRenderingGui : public wxFrame {
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
	 * \var wxMainGui* _mainGui
	 * \brief Puntatore alla finestra principale
	 */
	wxMainGui* _mainGui;

	/** 
	 * \var wxRadioBox* _itemRadioBoxProjectionType
	 * \brief Radiobox per il cambio del tipo di proiezione
	 */
	wxRadioBox* _itemRadioBoxProjectionType;

	
public: 

	/** 
	 * \var wxComboBox* _comboBoxClut
	 * \brief Combobox per la scelta della CLUT da applicare al volume rendering o al mip
	 */
	wxComboBox* _comboBoxClut;

	/** 
	 * \var wxComboBox* _comboBoxRendering
	 * \brief Combobox per la scelta del rendering mode
	 */
	wxComboBox* _comboBoxRendering;

	wxSlider* _itemSlider;

	/** 
	 * \var wxComboBox* _comboBoxZoom
	 * \brief Combobox per la scelta dello zoom
	 */
	//wxComboBox* _comboBoxZoom;

	// PROVA
	//wxComboBox* _comboBoxSampleDistance;


private:

	wxStaticText* _textWLlabel;
	wxStaticText* _textWWlabel;

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
	 * \var unsigned int _idData
	 * \brief Identificativo del dato
	 */
	unsigned int _idData;

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

	/** 
	 * \var double _minOpacityValue
	 * \brief Minimo valore dell'opacità
	 */
	double _minOpacityValue;

	/** 
	 * \var double _maxOpacityValue
	 * \brief Massimo valore dell'opacità
	 */
	double _maxOpacityValue;

	/** 
	 * \var short _selectedButton
	 * \brief Indica quale bottone è stato selezionato nella toolbar
	 */
	short _selectedButton;

	/** 
	 * \var techniqueRayCasting _renderingTechnique
	 * \brief Tecnica di ricostruzione 3D: Volume Rendering, Volume Texture Rendering, MIP...
	 */
	techniqueRayCasting _renderingTechnique;

	double _initialOrientation[3];
	double _initialScale[3];
	double _initialPosition[3];

	/** 
	 * \var bool _3DtextureSupported
	 * \brief Indica se il 3D texture mapping è abilitato o meno
	 */
	//bool _3DtextureSupported;

	/** 
	 * \var bool _tissueVisionSelected;
	 * \brief Indica se è stata selezionata una delle visioni di tessuto predefinite
	 */
	//bool _tissueVisionSelected;

	/** 
	 * \var long _wl
	 * \brief Window level
	 */
	long _wl;

	/** 
	 * \var long _ww
	 * \brief Window width
	 */
	long _ww;

	/** 
	 * \var double _distance
	 * \brief Distanza iniziale tra camera e volume
	 */
	double _distance;

	/** 
	 * \var int _POD
	 * \brief Distanza tra l'osservatore e lo schermo
	 */
	int _POD;

	int _availableVRAM;

	/** 
	 * \var int _croppingInitialized
	 * \brief specifica se il cropping box e' stato o meno inizializzato
	 */
	int _croppingInitialized;

	/** 
	 * \var vtkPlanes* _clippingPlanes
	 * \brief i 6 piani che racchiudono il cropping box
	 */
	vtkPlanes* _clippingPlanes;

	/** 
	 * \var vtkBoxWidget* _boxWidget
	 * \brief cropping box per l'estrazione di VOI
	 */
	vtkBoxWidget* _boxWidget;

	/**
	 * \var wxPanel* _mainPanel
	 * \brief Pannello contenente il visualizzatore
	 */
	wxPanel* _mainPanel;

	/** 
	 * \var wxBoxSizer* _imageBoxSizer
	 * \brief Contenitore contenente il pannello _maiPanel
	 */
	wxBoxSizer* _imageBoxSizer;

	/** 
	 * \var int _clut
	 * \brief Indica che tipo di CLUT applicare al volume
	 */
	int _clut;

	/** 
	 * \var wxToolBar* _toolbar
	 * \brief Toolbar
	 */
	wxToolBar* _toolbar;

	/** 
	 * \var double _zoomValuePerspective
	 * \brief valore di zoom attivo Perspective mode
	 */
	//double _zoomValuePerspective;

	/** 
	 * \var double _zoomValueParallel
	 * \brief valore di zoom attivo Parallel mode
	 */
	//double _zoomValueParallel;

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
	 * \var int _parallax
	 * \brief valore finto della parallasse
	 */
	int _parallax;

	/** 
	 * \var double _globalZoomFactor;
	 * \brief valore globale di zoom
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
	 * \var float _sampleDistance
	 * \brief sample distance per il ray casting
	 */
	float _sampleDistance;

	bool	_isShadeActive;
	double	_shadingAmbient;
	double	_shadingDiffuse;
	double	_shadingSpecular;
	double	_shadingSpecularPower;

	/** 
	 * \fn void getBoundsDepthInfo(double & closest, double & farthest)
	 * \brief Fornisce in formazioni sui bound del modello principale
	 * \param closest in uscita restituisce il depth del bound più vicino alla camera
	 * \	  farthest in uscita contiene il depth del bound più lontano dalla camera
	 */
	void getBoundsDepthInfo(double & closest, double & farthest);

	/** 
	 * \fn bool isThisVoxelVisible(double scalar)
	 * \brief determina la visibilità o meno del voxel in esame
	 * \param scalar valore scalare associato al voxel in esame
	 */
	bool isThisVoxelVisible(double scalar);

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

	void check3DcursorCapability();

public:

	/** Costruttore con parametri il titolo, la posizione e la dimensione della finestra */
	wxVolumeRenderingGui(const wxString& title, const wxPoint& pos, const wxSize& size);

	/** Distruttore */
	~wxVolumeRenderingGui();

	/**
	* \fn void CreateReconstructionToolbar()
	* \brief Questo metodo crea la toolbar
	*/
	void CreateReconstructionToolbar();

	/**
	* \fn void Createmenu()
	* \brief Metodo che crea il menù della finestra
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
	* \brief Associare il puntatore mainGui alla finestra padre e crea un nuovo wxVtkViewer3d associato al mainGui
	* \param mainGui Finestra principale
	* \return Visualizzatore 3D
	*/
	wxVtkViewer3d* new3dViewer(wxMainGui* mainGui);

	/**
	* \fn void show()
	* \brief Metodo che mostra la finestra co il visualizzatore:
	*/
	void show();

	/**
	* \fn void onResetView(wxCommandEvent& event)
	* \brief Resetta il dato 
	* \param event Evento
	*/
	void onResetView(wxCommandEvent& event);

	/**
	* \fn void onChangeRendering(wxCommandEvent& event)
	* \brief Cambia la tecnica di ricostruzione 3D: Volume Rendering/MIP
	* \param event Evento
	*/
	void onChangeRendering(wxCommandEvent& event);

	void onLODslider(wxCommandEvent& event);

	void onActivateShading(wxCommandEvent& event);

	void onChangeShadingParameters(wxCommandEvent& event);

	/**
	* \fn void onChangeInputDevice(wxCommandEvent& event)
	* \brief Cambia il dispositivo di input utilizzato
	* \param event Evento
	*/
	void onChangeInputDevice(wxCommandEvent& event);

	/**
	* \fn void onCLUT(wxCommandEvent& event)
	* \brief Cambia la CLUT
	* \param event Evento
	*/
	void onCLUT(wxCommandEvent& event);


	/**
	* \fn di prova per vedere coma cambia il volume al cambiare della sample distance
	*/
	void onSampleDistance(wxCommandEvent& event);


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
	* \fn void onVolumeRendering(wxCommandEvent& event)
	* \brief Imposta il Volume Rendering quale tecnica di ricostruzione 3D
	* \param event Evento
	*/
	void onVolumeRendering(wxCommandEvent& event);

	/**
	* \fn void onTextureRendering(wxCommandEvent& event)
	* \brief Imposta il TextureRendering quale tecnica di ricostruzione 3D
	* \param event Evento
	*/
	void onTextureRendering(wxCommandEvent& event);

	/**
	* \fn void onGPURendering(wxCommandEvent& event)
	* \brief Imposta il GPU-based rendering quale tecnica di ricostruzione 3D
	* \param event Evento
	*/
	void onGPURendering(wxCommandEvent& event);

	/**
	* \fn void onFixedPointRendering(wxCommandEvent& event)
	* \brief Imposta il FixedPointRendering quale tecnica di ricostruzione 3D
	* \param event Evento
	*/
	void onFixedPointRendering(wxCommandEvent& event);

	/**
	* \fn void onMIP(wxCommandEvent& event)
	* \brief Imposta il MIP quale tecnica di ricostruzione 3D
	* \param event Evento
	*/
	void onMIP(wxCommandEvent& event);

	/**
	* \fn void onMinIP(wxCommandEvent& event)
	* \brief Imposta il MinIP quale tecnica di ricostruzione 3D
	* \param event Evento
	*/
	//void onMinIP(wxCommandEvent& event);

	/**
	* \fn void onWindowLevel(wxCommandEvent& event)
	* \brief Imposta la funzionalità window level/width
	* \param event Evento
	*/
	void onWindowLevel(wxCommandEvent& event);

	/**
	* \fn void onMove(wxCommandEvent& event)
	* \brief Imposta la funzionalità move
	* \param event Evento
	*/
	void onMove(wxCommandEvent& event);

	/**
	* \fn void onDolly(wxCommandEvent& event)
	* \brief Imposta la funzionalità dolly
	* \param event Evento
	*/
	void onDolly(wxCommandEvent& event);

	/**
	* \fn void onZoom(wxCommandEvent& event)
	* \brief Imposta la funzionalità zoom
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
	* \brief Imposta la funzionalità rotate
	* \param event Evento
	*/
	void onRotate(wxCommandEvent& event);

	/**
	* \fn void onRotateAround(wxCommandEvent& event)
	* \brief Imposta la funzionalità rotate around
	* \param event Evento
	*/
	void onRotateAround(wxCommandEvent& event);


	/**
	* \fn void onAxialView(wxCommandEvent& event)
	* \brief Imposta la vista assiale
	* \param event Evento
	*/
	void onAxialView(wxCommandEvent& event);

	/**
	* \fn void onCoronalView(wxCommandEvent& event)
	* \brief Imposta la vista coronale
	* \param event Evento
	*/
	void onCoronalView(wxCommandEvent& event);

	/**
	* \fn void onSagittalView(wxCommandEvent& event)
	* \brief Imposta la vista sagittale
	* \param event Evento
	*/
	void onSagittalView(wxCommandEvent& event);

	/**
	* \fn void onSagittalView(wxCommandEvent& event)
	* \brief Imposta la vista sagittale opposta
	* \param event Evento
	*/
	void onOppositeSagittalView(wxCommandEvent& event);

	/**
	* \fn void onVOI(wxCommandEvent& event)
	* \brief Esegue la selezione di una VOI (Volume Of Interest)
	* \param event Evento
	*/
	void onVOI(wxCommandEvent& event);

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
	* \brief Imposta la modalità stereoscopica checkerboard
	* \param event Evento
	*/
	void onStereoModeCheckerboard(wxCommandEvent& event);

	/**
	* \fn void onProjectionTypeParallel(wxCommandEvent& event)
	* \brief Imposta il tipo di proiezione parallela
	* \param event Evento
	*/
	void onProjectionTypeParallel(wxCommandEvent& event);

	/**
	* \fn void onProjectionTypeEndoscopy(wxCommandEvent& event)
	* \brief Imposta il tipo di proiezione endoscopica
	* \param event Evento
	*/
	void onProjectionTypeEndoscopy(wxCommandEvent& event);

	/**
	* \fn void onProjectionTypePerspective(wxCommandEvent& event)
	* \brief Imposta il tipo di proiezione prospettica
	* \param event Evento
	*/
	void onProjectionTypePerspective(wxCommandEvent& event);

	/**
	* \fn void updateVolume(bool doARender)
	* \brief aggiorna il volume
	* \param bool doARender
	*/
	void updateVolume(bool doARender=true);

	void updateShading(double shadingAmbient, double shadingDiffuse, double shadingSpecular, double shadingSpecularPower);

	/**
	* \fn void updateStereoView(bool doARender)
	* \brief aggiorna eye angle e parallax della camera		
	* \param doARender richiede o meno un render
	*/
	void updateStereoView(bool doARender = false);

	/**
	* \fn void updateStereoView()
	* \brief aggiorna eye angle e parallax della camera in base al valore dy
	* \param dy offset tra Y attuale e precedente del mouse
	*/
	//void updateStereoView(int dy);

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
	* \fn void onModifyObserverDistance(wxCommandEvent& event)
	* \brief Modifica la distanza tra osservatore e schermo
	* \param event Evento
	*/
	void onModifyObserverDistance(wxCommandEvent& event);

	/**
	* \fn void onModifyWLWW(wxCommandEvent& event)
	* \brief Modifica window level e window width
	* \param event Evento
	*/
	void onModifyWLWW(wxCommandEvent& event);

	/**
	* \fn void updateClippingPlanes(bool updateFocalPoint)
	* \brief aggiorna i 6 piani dal box widget
	* \param bool updateFocalPoint
	*/
	void updateClippingPlanes(bool updateFocalPoint=false);

	/**
	* \fn void initializeCroppingBox()
	* \brief inizializza il cropping box
	*/
	void initializeCroppingBox();

	/**
	* \fn void onShowAboutDialog(wxCommandEvent& WXUNUSED(event))
	* \brief Visualizza le informazioni su MITO
	* \param event Evento
	*/
	void onShowAboutDialog(wxCommandEvent& WXUNUSED(event));

	/**
	* \fn void onQuit(wxCloseEvent& event)
	* \brief Chiude la finestra
	* \param event Evento
	*/
	void onQuit(wxCloseEvent& event);

	/**
	* \fn void onClose(wxCommandEvent& event)
	* \brief Chiude la finestra
	* \param event Evento
	*/
	void onClose(wxCommandEvent& event);

	/**
	* \fn showMouseCursor()
	* \brief Mostra il cursore del mouse
	*/
	void showMouseCursor();

	/**
	* \fn hideMouseCursor()
	* \brief Nasconde il cursore del mouse
	*/
	void hideMouseCursor();
	
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

	void onModifyAvailableVRAM(wxCommandEvent& WXUNUSED(event));
	
	/**
	* \fn onEnterToolbar()
	* \brief mostra il cursore del mouse quando si trova sulla toolbar
	* \param wxMouseEvent Evento
	*/
	void onEnterToolbar(wxMouseEvent& event);

	/**
	* \fn void setRenderingTechnique(techniqueRayCasting renderingTechnique)
	* \brief Assegna la tecnica di rendering
	* \param renderingTechnique Tecnica di rendering da assegnare
	*/
	void setRenderingTechnique(techniqueRayCasting renderingTechnique);

	/**	
	* \fn void getRenderingTechnique(int renderingTechnique)
	* \brief Restituisce la tecnica di rendering
	* \return Tecnica di rendering
	*/
	inline int getRenderingTechnique() {
		return _renderingTechnique;
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

	/**
	* \fn void setWlWw(long wl, long ww)
	* \brief Assegna window level e window width
	* \param wl Window level
	* \param ww Window width
	*/
	inline void setWlWw(long wl, long ww) {
		_wl = wl;
		_ww = ww;
		const wxString textWL = wxString::Format( wxT("%i"), (int)_wl);
		const wxString textWW = wxString::Format( wxT("%i"), (int)_ww);
		_textWLlabel->SetLabel(textWL);
		_textWWlabel->SetLabel(textWW);
		if(_viewer3d) ((appWxVtkInteractor*)_viewer3d->getWxWindow())->setWlWw(wl,ww);
	}

	inline void getShadingValues(double &ambient, double &diffuse, double &specular, double &specularPower) {
		ambient = _shadingAmbient;
		diffuse = _shadingDiffuse;
		specular = _shadingSpecular;
		specularPower = _shadingSpecularPower;
	}

	inline void setSampleDistance(float sampleDistance) {
		_sampleDistance = sampleDistance;
	}

	inline double getGlobalZoomFactor() {
		return _globalZoomFactor;
	}

	inline void setGlobalZoomFactor(double factor) {
		_globalZoomFactor = factor;
	}

	inline void setPOD(int POD) {
		_POD = POD;
	}

	inline int getPOD() {
		return _POD;
	}

	/**
	* \fn bool intersectingModelSurface(long x, long y, double point[3])
	* \brief costruisce un raggio ortogonale allo schermo passante nel punto (x,y) dello schermo
	* \		 e restituisce true se tale raggio interseca la superficie del modello, altrimenti 
	* \		 restituisce false. Se restituisce true, point conterrà il punto di intersezione 
	* \		 espresso in coordinate spaziali.
	* \param x l'ascissa in coordinate video del raggio da costruire
	* \		 y l'ordinata in coordinate video del raggio da costruire
	* \		 point il punto spaziale di intersezione sulla superficie del modello
	*/
	bool intersectingModelSurface(long x, long y, double point[3]);

	// bfdhbadadfb
	bool isPointOutsideCroppingBox(double point[3]);

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
	* \fn void hide3DCursor()
	* \brief rende invisibile il cursore 3D
	*/
	void hide3DCursor();


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
	* \fn void setCLUT(int clut)
	* \brief Indica il tipo di CLUT
	* \param clut Tipo di CLUT
	*/
	inline void setCLUT(int clut) {
		_clut = clut;
		((appWxVtkInteractor*)_viewer3d->getWxWindow())->setCLUT(_clut);
	}

	
	/**
	* \fn int set3DCursorRGBColor(double R, double G, double B)
	* \brief setta il colore del cursore 3D
	* \param R,G,B terna RGB
	*/
	void set3DCursorRGBColor(double R, double G, double B);

	void checkSuitableMappers();

	void computeOpacityColor();

private:
	DECLARE_EVENT_TABLE()
};

enum {
	IDREC3d_WLWTool = 0,
	IDREC3d_moveTool,
	IDREC3d_dollyTool,
	IDREC3d_zoomTool,
	IDREC3d_RotateTool,
	IDREC3d_3drotTool,
	IDREC3d_3drotcamTool,
	IDREC3d_LengthTool,
	IDREC3d_PointTool,
	IDREC3d_VOITool,
	IDREC3d_SkinTool,
	IDREC3d_MuscleTool,
	IDREC3d_BoneTool,
	IDREC3d_WiiTool,
	IDREC3d_WiiConfigurationTool,
	IDREC3d_WiiChangeSceneIRInteraction,
	IDREC3d_ModifyCDRatioFunction,
	IDREC3d_ModifyMotionFactor,
	IDREC3d_BestTool,
	IDREC3d_CroppingTool,
	IDREC3d_OrientTool,
	IDREC3d_LODslider,
	IDREC3d_fileCloseWindow,
	IDREC3d_3dviewerCoronalView,
	IDREC3d_3dviewerLeftSagittalView,
	IDREC3d_3dviewerRightSagittalView,
	IDREC3d_3dviewerAxialView,
	IDREC3d_3dFullScreen,
	IDREC3d_3dviewerBackgroundColour,
	IDREC3d_WindowMinimize,
	IDREC3d_WindowResizeWindow,
	IDREC3d_WindowTileWindows,
	IDREC3d_WindowCloseAllViewers,
	IDREC3d_WindowBringAllToFront,
	IDREC3d_HelpUserManual,
	IDREC3d_HelpReportBug,
	IDREC3d_HelpRequestFeature,
	IDREC3d_HelpAbout,
	IDREC3d_3dviewerResetToInitialView,
	IDREC3d_RADIOBUTTONGROUPInputDevice1,
	IDREC3d_RADIOBUTTONGROUPInputDevice2,
	IDREC3d_stereo,
	IDREC3d_perspective,
	IDREC3d_parallel,
	IDREC3d_endoscopy,
	IDREC3d_COMBOBOXClut,
	IDREC3d_COMBOBOXRenderingMode,
	IDREC3d_RADIOBUTTONGROUPCursorType1,
	IDREC3d_RADIOBUTTONGROUPCursorType2,
	IDREC3d_3DcursorText,
	IDREC3d_shadingCheckBox,
	IDREC3d_shadingChangeButton,
	IDREC3d_shadingAmbientText,
	IDREC3d_shadingSpecularText,
	IDREC3d_shadingDiffuseText,
	m_3d_WindowLevel,
	m_3d_Move,
	m_3d_Dolly,
	m_3d_Zoom,
	m_3d_Rotate,
	m_3d_RotateAround,
	m_3d_voi,
	m_3d_VolumeRendering,
	m_3d_TextureRendering,
	m_3d_GPURendering,
	m_3d_FixedPointRendering,
	m_3d_MIP,
	m_3d_RenderingMode,
	m_3d_StereoModeVR,
	m_3d_StereoModeAnaglyphVR,
	m_3d_StereoModeActivePassiveVR,
	m_3d_StereoModeCheckerboardVR,
	m_3d_ModifyObserverDistanceVR,
	m_3d_ModifyWLWWVR,
	m_3d_ModifyAvailableVRAM
};


#endif _wxVolumeRenderingGui_h_