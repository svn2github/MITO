/**
 * \file wx2dGui.h
 * \brief File per la gestione della finestra 2D
 * \author ICAR-CNR Napoli
 */

#ifndef _wx2dGui_h_
#define _wx2dGui_h_

#include "wxMainGui.h"
#include "wxVtkViewer2d.h"
#include <wx/checkbox.h>
#include <wx/toolbar.h>


/** 
 * \class wx2dGui
 * \brief Classe per la gestione della finestra 2D. Deriva da wxFrame
 */
class wx2dGui : public wxFrame {
	/** 
	 * \var wxSlider* _slider
	 * \brief Slider con cui è possibile scorrere le slice
	 */
	wxSlider* _slider;

	/** 
	 * \var wxPanel* _imageBoxPanel
	 * \brief Pannello contenente il visualizzatore
	 */
	wxPanel* _imageBoxPanel;

	/** 
	 * \var wxBoxSizer* _imageBoxSizer
	 * \brief Box contenente il pannello
	 */
	wxBoxSizer* _imageBoxSizer;

	/** 
	 * \var wxVtkViewer2d* _viewer2d
	 * \brief Visualizzatore 2D
	 */
	wxVtkViewer2d* _viewer2d;

	/** 
	 * \var wxMainGui* _mainGui
	 * \brief GUI principale
	 */
	wxMainGui* _mainGui;

	/** 
	 * \var bool _preview
	 * \brief Utilizzato per lo scorrimento automatico delle slice
	 */
	bool _preview;

	/** 
	 * \var bool _working
	 * \brief Indica che è in corso lo scorrimento automatico delle slice
	 */
	bool _working;

	/** 
	 * \var unsigned int _idViewer
	 * \brief Identificativo del visualizzatore
	 */
	unsigned int _idViewer;

	/** 
	 * \var wxCheckBox* _autoPlay
	 * \brief Checkbox per l'autoplay
	 */
	wxCheckBox* _autoPlay;

	/** 
	 * \var wxToolBar* _toolBar
	 * \brief Toolbar 
	 */
	wxToolBar* _toolBar;

	/** 
	 * \var wxMenu* _menuFileExport
	 * \brief Menù per il salvataggio delle immagini
	 */
	wxMenu* _menuFileExport;

	/** 
	 * \var short _selectedButton
	 * \brief Indica quale bottone è stato selezionato nella toolbar
	 */
	short _selectedButton;

public:
	/** Costruttore con parametri title, pos e size */
	wx2dGui(const wxString& title, const wxPoint& pos, const wxSize& size);

	/** Distruttore */
	~wx2dGui();

	/**
	* \fn void onAlbum(wxCommandEvent &event)
	* \brief Ritorna alla finestra principale
	* \param event Evento
	*/
	void onAlbum(wxCommandEvent &event);

	/**
	* \fn void onResetView(wxCommandEvent &event)
	* \brief Resetta il dato
	* \param event Evento
	*/
	void onResetView(wxCommandEvent &event);

	/**
	* \fn void onAutoPlay(wxCommandEvent &event)
	* \brief Scorre le slice automaticamente
	* \param event Evento
	*/
	void onAutoPlay(wxCommandEvent& event);

	/**
	* \fn void onPlay(wxCommandEvent &event)
	* \brief Scorre le slice
	* \param event Evento
	*/
	void onPlay(wxCommandEvent& event);

	/**
	* \fn void onStop(wxCommandEvent &event)
	* \brief Arresta lo scorrimento delle slice
	* \param event Evento
	*/
	void onStop(wxCommandEvent& event);

	/**
	* \fn void onViewHeader(wxCommandEvent &event)
	* \brief Visualizza l'header DICOM
	* \param event Evento
	*/
	void onViewHeader(wxCommandEvent& event);

	/**
	* \fn void onFlipVertical(wxCommandEvent &event)
	* \brief Effettua il flip verticale
	* \param event Evento
	*/
	void onFlipVertical(wxCommandEvent& event);

	/**
	* \fn void onFlipHorizontal(wxCommandEvent &event)
	* \brief Effettua il flip orizzontale
	* \param event Evento
	*/
	void onFlipHorizontal(wxCommandEvent& event);

	/**
	* \fn void onSelectRoi(wxCommandEvent &event)
	* \brief Seleziona una ROI
	* \param event Evento
	*/
	void onSelectRoi(wxCommandEvent& event);

	/**
	* \fn void onDeleteRoi(wxCommandEvent &event)
	* \brief Elimina una ROI
	* \param event Evento
	*/
	void onDeleteRoi(wxCommandEvent& event);

	/**
	* \fn void onExtractRoiInside(wxCommandEvent &event)
	* \brief Estrae il contenuto di una ROI
	* \param event Evento
	*/
	void onExtractRoiInside(wxCommandEvent& event);

	/**
	* \fn void onExtractRoiOutside(wxCommandEvent &event)
	* \brief Estrae tutto ciò che circonda una ROI
	* \param event Evento
	*/
	void onExtractRoiOutside(wxCommandEvent& event);	

	/**
	* \fn void onModifyWLWW(wxCommandEvent& event)
	* \brief Consente di modificare i valori di window level e window width
	* \param event Evento
	*/
	void onModifyWLWW(wxCommandEvent& event);

	/**
	* \fn void onAll(wxCommandEvent &event)
	* \brief Abilita le seguenti funzionalità: window level/width, move, zoom, rotate
	* \param event Evento
	*/
	void onAll(wxCommandEvent& event);

	/**
	* \fn void onWindowLevel(wxCommandEvent &event)
	* \brief Abilita la funzionalità window level/width
	* \param event Evento
	*/
	void onWindowLevel(wxCommandEvent& event);

	/**
	* \fn void onMove(wxCommandEvent &event)
	* \brief Abilita la funzionalità move
	* \param event Evento
	*/
	void onMove(wxCommandEvent& event);

	/**
	* \fn void onZoom(wxCommandEvent &event)
	* \brief Abilita la funzionalità zoom
	* \param event Evento
	*/
	void onZoom(wxCommandEvent& event);

	/**
	* \fn void onRotate(wxCommandEvent &event)
	* \brief Abilita la funzionalità rotate
	* \param event Evento
	*/
	void onRotate(wxCommandEvent& event);

	/**
	* \fn void onAnimate(wxCommandEvent &event)
	* \brief Consente di scorrere le slice con il mouse
	* \param event Evento
	*/
	void onAnimate(wxCommandEvent& event);	

	/**
	* \fn void onActivate(wxActivateEvent& event)
	* \brief Indica che la finestra 2D è attiva
	* \param event Evento
	*/
	void onActivate(wxActivateEvent& event);

	/**
	* \fn void onLength(wxCommandEvent &event)
	* \brief Calcola la lunghezza tra due punti specificati dall'utente
	* \param event Evento
	*/
	void onLength(wxCommandEvent& event);

	/**
	* \fn void onAngle(wxCommandEvent &event)
	* \brief Calcola l'angolo di un arco specificato dall'utente
	* \param event Evento
	*/
	void onAngle(wxCommandEvent& event);

	/**
	* \fn void onPolygon(wxCommandEvent &event)
	* \brief Consente all'utente di selezionare una ROI a forma di poligono e ne calcola il perimetro, l'area ed il volume
	* \param event Evento
	*/
	void onPolygon(wxCommandEvent& event);

	/**
	* \fn void onRectangle(wxCommandEvent &event)
	* \brief Consente all'utente di selezionare una ROI rettangolare e ne calcola il perimetro, l'area ed il volume
	* \param event Evento
	*/
	void onRectangle(wxCommandEvent& event);

	/**
	* \fn void onRectangle(wxCommandEvent &event)
	* \brief Consente all'utente di selezionare una ROI a mano libera e ne calcola il perimetro, l'area ed il volume
	* \param event Evento
	*/
	void onPencil(wxCommandEvent& event);

	/**
	* \fn void onSliderUpdate(wxCommandEvent &event)
	* \brief Aggiorna la posizione dello slider
	* \param event Evento
	*/
	void onSliderUpdate(wxCommandEvent& event);
	void OnMouseWheel(wxMouseEvent &event);


	/**
	* \fn void onNextSlice(wxCommandEvent &event)
	* \brief Carica la slice successiva
	* \param event Evento
	*/
	void onNextSlice(wxCommandEvent &event);

	/**
	* \fn void onPreviousSlice(wxCommandEvent &event)
	* \brief Carica la slice precedente
	* \param event Evento
	*/
	void onPreviousSlice(wxCommandEvent &event);

	/**
	* \fn void onAnonymize(wxCommandEvent &event)
	* \brief Anonimizza un'immagine o una serie
	* \param event Evento
	*/
	void onAnonymize(wxCommandEvent& event);

	/**
	* \fn void onExportToBmp(wxCommandEvent &event)
	* \brief Salva un'immagine o una serie in formato BMP
	* \param event Evento
	*/
	void onExportToBmp(wxCommandEvent& event);

	/**
	* \fn void onExportToDicom(wxCommandEvent &event)
	* \brief Salva un'immagine o una serie in formato DICOM
	* \param event Evento
	*/
	void onExportToDicom(wxCommandEvent& event);

	/**
	* \fn void onVolumeRendering(wxCommandEvent &event)
	* \brief Crea la finestra 3D contenente il risultato del volume rendering su una serie
	* \param event Evento
	*/
	void onVolumeRendering(wxCommandEvent& event);

	/**
	* \fn void onSurfaceRendering(wxCommandEvent &event)
	* \brief Crea la finestra 3D contenente il risultato del surface rendering su una serie
	* \param event Evento
	*/
	void onSurfaceRendering(wxCommandEvent& event);

	/**
	* \fn void onMaximumIntensityProjection(wxCommandEvent &event)
	* \brief Crea la finestra 3D contenente il risultato del Maximum Intensity Projection (MIP) di una serie
	* \param event Evento
	*/
	void onMaximumIntensityProjection(wxCommandEvent& event);

	/**
	* \fn void onMinimumIntensityProjection(wxCommandEvent &event)
	* \brief Crea la finestra 3D contenente il risultato del Minimum Intensity Projection (MinIP) di una serie
	* \param event Evento
	*/
	//void onMinimumIntensityProjection(wxCommandEvent& event);

	/**
	* \fn void onConnectedThreshold(wxCommandEvent &event)
	* \brief Avvia il processo di segmentazione secondo la tecnica connected threshold
	* \param event Evento
	*/
	void onConnectedThreshold(wxCommandEvent& event);

	/**
	* \fn void onNeighborhoodConnected(wxCommandEvent &event)
	* \brief Avvia il processo di segmentazione secondo la tecnica neighborhood connected
	* \param event Evento
	*/
	void onNeighborhoodConnected(wxCommandEvent& event);

	/**
	* \fn void onConfidenceConnected(wxCommandEvent &event)
	* \brief Avvia il processo di segmentazione secondo la tecnica confidence connected
	* \param event Evento
	*/
	void onConfidenceConnected(wxCommandEvent& event);

	/**
	* \fn void onClose(wxCommandEvent &event)
	* \brief Chiude la finestra 2D
	* \param event Evento
	*/
	void onClose(wxCommandEvent& event);

	/**
	* \fn void onShowAboutDialog(wxCommandEvent& WXUNUSED(event))
	* \brief Visualizza le informazioni su MITO
	* \param event Evento
	*/
	void onShowAboutDialog(wxCommandEvent& WXUNUSED(event));

	/**
	* \fn void onQuit(wxCloseEvent& event)
	* \brief Chiude la finestra 2D
	* \param event Evento
	*/
	void onQuit(wxCloseEvent& event);

	/**
	* \fn wxVtkViewer2d* new2dViewer(wxMainGui* mainGui)
	* \brief Crea un nuovo visualizzatore 2D
	* \param mainGui Finestra principale
	* \return Restituisce il visualizzatore 2D
	*/
	wxVtkViewer2d* new2dViewer(wxMainGui* mainGui);

	/**
	* \fn void show(itkVtkData *itkVtkData, bool showme)
	* \brief Visualizza il dato nella finestra 2D
	* \param itkVtkData Dato da visualizzare
	* \param showme vale true se la finestra deve essere mostrata
	*/
	void show(itkVtkData *itkVtkData, bool showme = true);

	/**
	* \fn void setIdViewer(unsigned int idViewer)
	* \brief Assegna l'identificativo di un visualizzatore
	* \param idViewer Identificativo del visualizzatore
	*/
	inline void setIdViewer(unsigned int idViewer) {
		_idViewer=idViewer;
	}

	/**
	* \fn wxSlider* getSlider()
	* \brief Restituisce lo slider corrente
	* \return Lo slider corrente
	*/
	inline wxSlider* getSlider() {
		return _slider;
	}

	void onInputMouse(wxCommandEvent& event);

	void onInputKinect(wxCommandEvent& event);

private:
	DECLARE_EVENT_TABLE()
};

enum {
	tool_album = 0,
	tool_all,
	tool_windowlevel,
	tool_move,
	tool_zoom,
	tool_rotate,
	tool_animate,
	tool_length,
	tool_angle,
	tool_polygon,
	tool_rectangle,
	tool_pencil,
	tool_connectedthreshold,
	tool_neighborhoodconnected,
	tool_confidenceconnected,
	tool_volumerendering,
	tool_surface,
	tool_mip,
	tool_mouse,
	tool_kinect,
	//tool_minip,
	tool_Max,
	id_2dImageBox,
	id_2dWindow,
	id_2dSlider,
	id_2dAutoPlay,
	id_2dPlay,
	id_2dStop,
	id_HorToolBar,
	m_2d_fileShowAlbum,
	m_2d_fileExport,
	m_2d_fileExportBMP,
	m_2d_fileExportDICOM,
	m_2d_fileHeader,
	m_2d_fileClose,
	m_2d_fileAnonymize,
	m_2d_Cut,	
	m_2d_Copy,
	m_2d_Paste,
	m_2d_Clear,
	m_2d_Select_All,
	m_2d_viewerNextSlice,
	m_2d_viewerPrevSlice,
	m_2d_viewerResetImageView,
	m_2d_viewerFlipImageHorizontal,
	m_2d_viewerFlipImageVertical,
	m_2d_ModifyWLWW,
	m_2d_All,
	m_2d_Zoom,
	m_2d_Move,
	m_2d_Rotate,
	m_2d_Window_Level_Width,
	m_2d_Animate,
	m_2d_Mouse_Button_Function,
	m_2d_Roi_Function,
	m_2d_Length,
	m_2d_Angle,
	m_2d_Rectangle,
	m_2d_Polygon,
	m_2d_Pencil,
	m_2d_Surface_Rendering,
	m_2d_Volume_Rendering,
	m_2d_Maximum_Intensity_Projection,
	m_2d_Minimum_Intensity_Projection,
	m_2d_Open_Roi,
	m_2d_Roi_Rename,
	m_2d_Save_Roi,
	m_2d_SelectRoi,
	m_2d_DeleteRoi,
	m_2d_ExtractRoi,
	m_2d_ExtractRoiInside,
	m_2d_ExtractRoiOutside,
	m_2d_Region,
	m_2d_ConnectedThreshold,
	m_2d_NeighborhoodConnected,
	m_2d_ConfidenceConnected,
	m_2d_HelpAbout
};

#endif _wx2dGui_h_