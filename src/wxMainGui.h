/**
 * \file wxMainGui.h
 * \brief File per la gestione della finestra principale
 * \author ICAR-CNR Napoli
 */

#ifndef _wxMainGui_h_
#define _wxMainGui_h_

#include "flipFilter.h"
#include "infoFilter.h"
#include "segmentationFilter.h"
#include "surfaceRenderingFilter.h"
#include "viewerHandler.h"
#include "volumeRenderingFilter.h"
#include "wxVtkViewer2d.h"
#include "wxVtkViewer3d.h"
#include "wxThumb.h"
#include "pacsCommunicator.h"

//#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include <list>

#include "treelistctrl.h"

// Forward declarations
class wxPacsPreferencesGui;
class wxTreeEvent;
class mitoLocalDatabase;
class wxPacsStoreGui;

/** 
 * \class wxMainGui
 * \brief Classe per la gestione della finestra principale. Deriva da wxFrame e da viewerHandler
 */
class wxMainGui : public wxFrame, public viewerHandler, public pacsCommunicator {

private:

	/** 
	 * \var wxWindow *_pacsWindow
	 * \brief Gestisce la finestra atta ad inviare query e a scaricare immagini dal PACS
	 */
	wxWindow *_pacsWindow;

	/** 
	 * \var wxPacsStoreGui *_storeWindow
	 * \brief Gestisce la finestra atta ad inviare immagini al PACS
	 */
	wxPacsStoreGui *_storeWindow;

	/** 
	 * \var unsigned int _idActiveViewer
	 * \brief Identificativo del visualizzatore attivo
	 */
	unsigned int _idActiveViewer;

	/** 
	 * \var unsigned int _idActiveViewer
	 * \brief Identificativo dell'anteprima
	 */
	unsigned int _idPreview;

	/** 
	 * \var string _pathDicomPacs
	 * \brief Percorso di un'immagine o di una serie scaricata dal PACS da aprire
	 */
	string _pathDicomPacs;

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
	 * \var segmentationFilter::algoType _isOpSegmentation
	 * \brief Indica, se definita, la tecnica di segmentazione
	 */
	segmentationFilter::algoType _isOpSegmentation;

	/** 
	 * \var wxThumbnails* _thumbnails
	 * \brief Puntatore al gestore dei thumbnail
	 */
	wxThumbnails* _thumbnails;

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
	 * \var wxListCtrl* _album
	 * \brief Gestore dell'album contenenti le informazioni relative alle immagini aperte recentemente
	 */
	//wxListCtrl* _album;
	wxTreeListCtrl *_album;
	wxTreeItemId	m_idLastSelectedTreeItem;

	/** 
	 * \var wxCheckBox* _autoPlay
	 * \brief Checkbox per l'autoplay
	 */
	wxCheckBox* _autoPlay;

	/** 
	 * \var bool _pacsModal
	 * \brief Indica se la finestra di query del PACS deve essere aperta in modalità modale o meno
	 */
	bool _pacsModal;

	/** 
	 * \var bool _pacsDaemon
	 * \brief Indica se la finestra di query del PACS deve essere aperta per servire il daemon
	 */
	bool _pacsDaemon;

	/** 
	 * \var bool bOpenedFirstSeries
	 * \brief Indica se abbiamo già aperto o no una serie per lo "scaricamento" corrente
	 */
	bool m_bOpenedFirstSeries;

	/** 
	 * \var wxArrayInt _seriesFailedPacs
	 * \brief Indica gli identificativi delle serie non scaricate dal PACS
	 */
	wxArrayInt _seriesFailedPacs;

	/** 
	 * \var wxPacsPreferencesGui *_pacsPreferencesGUI
	 * \brief Puntatore all'interfaccia per il cambio delle preferenze
	 */
	wxPacsPreferencesGui *_pacsPreferencesGUI;

	/** 
	 * \var pacsStoreSCP	*m_pStoreSrv
	 * \brief Puntatore all'interfaccia per il cambio delle preferenze
	 */
	pacsStoreSCP	*m_pStoreSrv;

	/** 
	 * \var mitoLocalDatabase	*m_pLocalCache
	 * \brief Puntatore all'interfaccia per la gestione delle immagini locali
	 */
	mitoLocalDatabase	*m_pLocalCache;

public:
	/** Costruttore con parametri title, pos e size */
	wxMainGui(const wxString& title, const wxPoint& pos, const wxSize& size);

	/** Distruttore */
	~wxMainGui();

	/**
	* \fn bool onViewerActivate(unsigned int idViewer)
	* \brief Setta l'identificativo del visualizzatore attivo
	* \param idViewer Identificativo del visualizzatore attivo
	* \return true se l'operazione va a buon fine
	*/
	bool onViewerActivate(unsigned int idViewer);

	/**
	* \fn bool onViewerQuit(unsigned int idViewer)
	* \brief Chiude un visualizzatore ed elimina tutti i dati relativi
	* \param idViewer Identificativo del visualizzatore da chiudere
	* \return true se l'operazione va a buon fine
	*/
	bool onViewerQuit(unsigned int idViewer);

	/**
	 * \fn void onMotion(unsigned int idViewer, wxMouseEvent &event)
	 * \brief Invocato dopo aver modificato la posizione del puntatore del mouse
	 * \param idViewer Identificativo del visualizzatore in cui è stato generato l'evento
	 * \param event Evento generato dal mouse
	 * \return true se l'operazione va a buon fine
	 */
	bool onMotion(unsigned int idViewer, wxMouseEvent &event);

	/**
	 * \fn bool onButtonDown(unsigned int idViewer, wxMouseEvent &event)
	 * \brief Invocato da un click del mouse
	 * \param idViewer Identificativo del visualizzatore in cui si è verificato l'evento
	 * \param event Evento generato dal mouse
	 */
	bool onButtonDown(unsigned int idViewer, wxMouseEvent &event);

	/**
	* \fn void onActivate(wxActivateEvent& event)
	* \brief Indica che la finestra principale è attiva
	* \param event Evento
	*/
	void onActivate(wxActivateEvent& event);

	/**
	* \fn void onAlbumActivated(wxTreeEvent& evt)
	* \brief Carica nel visualizzatore l'immagine o la serie indicata dall'evento
	* \param event Evento
	*/
	void onAlbumActivated(wxTreeEvent& evt);

	/**
	* \fn void onNew2dViewer(wxCommandEvent& WXUNUSED(event))
	* \brief Crea una finestra 2D
	* \param event Evento
	*/
	void onNew2dViewer(wxCommandEvent& WXUNUSED(event));

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
	* \fn void onOpenImage(wxCommandEvent& event)
	* \brief Carica un'immagine scelta dall'utente nel visualizzatore
	* \param event Evento
	*/
	void onOpenImage(wxCommandEvent& event);

	/**
	* \fn void onOpenSeries(wxCommandEvent& event)
	* \brief Carica una serie scelta dall'utente nel visualizzatore
	* \param event Evento
	*/
	void onOpenSeries(wxCommandEvent& event);

	/**
	* \fn void onOpenStudy(wxCommandEvent& event)
	* \brief Carica uno studio scelto dall'utente nel visualizzatore
	* \param event Evento
	*/
	void onOpenStudy(wxCommandEvent& event);

	/**
	* \fn void onImportDicomdir(wxCommandEvent& event)
	* \brief Carica una DICOMDIR scelta dall'utente nel visualizzatore
	* \param event Evento
	*/
	void onImportDicomdir(wxCommandEvent& event);

	/**
	* \fn void onFlipHorizontal(wxCommandEvent& event)
	* \brief Effettua il flip orizzontale dell'immagine o della serie corrente
	* \param event Evento
	*/
	void onFlipHorizontal(wxCommandEvent& event);

	/**
	* \fn void onFlipVertical(wxCommandEvent& event)
	* \brief Effettua il flip verticale dell'immagine o della serie corrente
	* \param event Evento
	*/
	void onFlipVertical(wxCommandEvent& event);

	/**
	* \fn void onNextSlice(wxCommandEvent& event)
	* \brief Carica la slice successiva a quella corrente
	* \param event Evento
	*/
	void onNextSlice(wxCommandEvent& event);

	/**
	* \fn void onPrevSlice(wxCommandEvent& event)
	* \brief Carica la slice precedente a quella corrente
	* \param event Evento
	*/
	void onPrevSlice(wxCommandEvent& event);

	/**
	* \fn void onViewHeader(wxCommandEvent& event)
	* \brief Visualizza l'header DICOM invocando il metodo void viewHeaderDicom(wxWindow* parent)
	* \param event Evento
	*/
	void onViewHeader(wxCommandEvent& event);

	/**
	* \fn void onSelectThumb(wxCommandEvent& event)
	* \brief Carica nel visualizzatore l'immagine corrispondente al thumbnail selezionato
	* \param event Evento
	*/
	void onSelectThumb(wxCommandEvent& event);

	/**
	* \fn void onSliderUpdate(wxCommandEvent& event)
	* \brief Aggiorna lo slider
	* \param event Evento
	*/
	void onSliderUpdate(wxCommandEvent& event);
	void OnMouseWheel(wxMouseEvent &event);

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
	* \fn void onDeleteExam(wxCommandEvent &event)
	* \brief Elimina un dato dall'album
	* \param event Evento
	*/
	void onDeleteExam(wxCommandEvent &event);

	/**
	* \fn void onResetView(wxCommandEvent &event)
	* \brief Resetta il dato
	* \param event Evento
	*/
	void onResetView(wxCommandEvent& event);

	/**
	* \fn void onQuery(wxCommandEvent &event)
	* \brief Apre la finestra che consente di inviare query al PACS
	* \param event Evento
	*/
	void onQuery(wxCommandEvent &event);

	/**
	* \fn void onPacsAutosavePreferences(wxCommandEvent &event)
	* \brief Imposta il salvataggio automatico delle impostazioni
	* \param event Evento
	*/
	void onPacsAutosavePreferences(wxCommandEvent &event);

	/**
	* \fn void onPacsOpenPreferences(wxCommandEvent &event)
	* \brief Apre la finestra per caricare un file di impostazioni
	* \param event Evento
	*/
	void onPacsOpenPreferences(wxCommandEvent &event);

	/**
	* \fn void onPacsSavePreferences(wxCommandEvent &event)
	* \brief Salva le impostazioni correnti nel file configurazione di default
	* \param event Evento
	*/
	void onPacsSavePreferences(wxCommandEvent &event);

	/**
	* \fn void onPacsSavePreferencesAs(wxCommandEvent &event)
	* \brief Salva le modifiche in un file di configurazione scelto dall'utente
	* \param event Evento
	*/
	void onPacsSavePreferencesAs(wxCommandEvent &event);

	/**
	* \fn void onPacsPreferences(wxCommandEvent &event)
	* \brief Apre la finestra per modificare le impostazioni di comunicazione con il PACS
	* \param event Evento
	*/
	void onPacsPreferences(wxCommandEvent &event);

	/**
	* \fn void onSend(wxCommandEvent &event)
	* \brief Apre la finestra che consente di inviare immagini al PACS
	* \param event Evento
	*/
	void onSend(wxCommandEvent &event);

	/**
	* \fn void onAnonymize(wxCommandEvent &event)
	* \brief Apre il dialog che consente di anonimizzare immagini o serie
	* \param event Evento
	*/
	void onAnonymize(wxCommandEvent &event);

	/**
	* \fn void onExportToDicom(wxCommandEvent &event)
	* \brief Salva immagini o serie in formato DICOM
	* \param event Evento
	*/
	void onExportToDicom(wxCommandEvent &event);

	/**
	* \fn void onExportToBmp(wxCommandEvent &event)
	* \brief Salva immagini o serie in formato BMP
	* \param event Evento
	*/
	void onExportToBmp(wxCommandEvent &event);

	/**
	* \fn void OnExportDicomdir(wxCommandEvent &event)
	* \brief Salva immagini o serie in formato DICOMDIR
	* \param event Evento
	*/
	void OnExportDicomdir(wxCommandEvent &event);

	/**
	* \fn void onImageFusion(wxCommandEvent &event)
	* \brief Effettua la fusione tra due immagini o due serie
	* \param event Evento
	*/
	void onImageFusion(wxCommandEvent &event);

	/**
	* \fn void onInitializeViewer()
	* \brief Apre un nuovo visualizzatore
	*/
	void onInitializeViewer();

	/**
	* \fn void onShowAboutDialog(wxCommandEvent& WXUNUSED(event))
	* \brief Visualizza le informazioni su MITO
	* \param event Evento
	*/
	void onShowAboutDialog(wxCommandEvent& WXUNUSED(event));

	/**
	* \fn void onClose(wxCommandEvent& event)
	* \brief Chiude la finestra principale
	* \param event Evento
	*/
	void onClose(wxCommandEvent& event);

	/**
	* \fn void onQuit(wxCloseEvent& event)
	* \brief Chiude la finestra principale
	* \param event Evento
	*/
	void onQuit(wxCloseEvent& event);

	/**
	* \fn void OnStoreServerNotification (wxCommandEvent& event)
	* \brief Gestisce i messaggi ricevuti dal thread del server per il download delle immagini
	* \param event Evento
	*/
	void OnStoreServerNotification (wxCommandEvent& event);

	/**
	* \fn void OnQueryGUINotification (wxCommandEvent& event)
	* \brief Gestisce i messaggi ricevuti dalla finestra di query
	* \param event Evento
	*/
	void OnQueryGUINotification (wxCommandEvent& event);

	/**
	* \fn void OnAlbumItemRightClick (wxTreeEvent& event)
	* \brief Chiamato quando si clicca di destro su un elemento dell'album
	* \param event Evento
	*/
	void OnAlbumItemRightClick (wxTreeEvent& event);

	/**
	* \fn void OnContextSendToPACS(wxCommandEvent& event)
	* \brief Chiamato quando si clicca nella voce del menu contestuale
	* \param event Evento
	*/
	void OnContextSendToPACS(wxCommandEvent& event);

	/**
	* \fn void OnContextDeleteLocal(wxCommandEvent& event)
	* \brief Chiamato quando si clicca nella voce del menu contestuale
	* \param event Evento
	*/
	void OnContextDeleteLocal(wxCommandEvent& event);

	/**
	* \fn void savePrefs(char *_file)
	* \brief Salva i parametri di connessione al PACS nel file specificato
	* \param _file File in cui salvare i parametri
	*/
	void savePrefs(char *_file);

	/**
	* \fn void saveAlbum()
	* \brief Salva l'album su file
	*/
	void saveAlbum();

	/**
	* \fn void loadAlbum()
	* \brief Carica l'album da file
	*/
	void loadAlbum();

	/**
	* \fn void updateAlbum()
	* \brief Visualizza informazioni sul visualizzatore
	*/
	void viewTextData();

	/**
	* \fn void viewHeaderDicom(wxWindow* parent)
	* \brief Visualizza l'header DICOM di un'immagine o di una serie
	* \param parent Finestra in cui visualizzare il dialog contenente l'header
	*/
	void viewHeaderDicom(wxWindow* parent);

	/**
	* \fn unsigned int getActiveViewer()
	* \brief Restituisce l'identificativo del visualizzatore attivo
	* \return Identificativo del visualizzatore attivo
	*/
	unsigned int getActiveViewer();

	/**
	* \fn wxTreeItemId searchRecord(string sStudyUID, string sSeriesUID)
	* \brief Ricerca un elemento nell'album e ne restituisce la handle
	* \param sStudyUID UID dello studio da ricercare
	* \param sSeriesUID UID della serie da ricercare
	* \param result Risultato della ricerca
	* \return 0 se non è stato trovato nell'album, 1 se è stato trovato lo studio, 2 se è stata trovata la serie.
	*/
	int searchRecord(string sStudyUID, string sSeriesUID, wxTreeItemId &result);

	/**
	* \fn void openDicomStudyPacs(string path, wxArrayInt seriesFailed)
	* \brief Apre uno studio scaricato dal PACS
	* \param path Percorso della directory contenente lo studio
	* \param seriesFailed Identificativi delle serie non scaricate
	*/
	void openDicomStudyPacs(string path, wxArrayInt seriesFailed);

	/**
	* \fn void deleteAlbumEntry(wxTreeItemId &itemId)
	* \brief Elimina una voce dall'album
	* \param itemId Item da eliminare
	* \param bDeleteFromHdd cancella i files dall'hard disk
	*/
	void deleteAlbumEntry(wxTreeItemId &itemId, bool bDeleteFromHdd);

	/**
	* \fn wxWindow* getPacsQueryGui()
	* \brief Restituisce la finestra per la ricezione di informazioni dal PACS
	*/
	inline wxWindow* getPacsQueryGui() {
		return _pacsWindow;
	}

	/**
	* \fn wxWindow* getPacsStoreGui()
	* \brief Restituisce la finestra per l'invio di immagini al PACS
	*/
	inline wxPacsStoreGui* getPacsStoreGui() {
		return _storeWindow;
	}

	/**
	* \fn pacsStoreSCP* getPacsStoreServer()
	* \brief Restituisce l'instanza attiva dello store server
	*/
	inline pacsStoreSCP* getPacsStoreServer() {
		return m_pStoreSrv;
	}

	/**
	* \fn wxSlider* getSlider()
	* \brief Restituisce lo slider
	*/
	inline wxSlider* getSlider() {
		return _slider;
	}

	/**
	* \fn bool getPacsModal()
	* \brief Restituisce il valore dell'attributo _pacsModal
	* \return Valore dell'attributo _pacsModal
	*/
	inline bool getPacsModal() {
		return _pacsModal;
	}

	/**
	* \fn void setPacsModal(bool pacsModal)
	* \brief Setta il valore della variabile _pacsModal
	* \param Valore da assegnare alla variabile _pacsModal
	*/
	inline void setPacsModal(bool pacsModal) {
		_pacsModal = pacsModal;
	}

	/**
	* \fn bool getPacsDaemon()
	* \brief Restituisce il valore dell'attributo _pacsDaemon
	* \return Valore dell'attributo _pacsDaemon
	*/
	inline bool getPacsDaemon() {
		return _pacsDaemon;
	}

	/**
	* \fn void setPacsDaemon(bool pacsDaemon)
	* \brief Setta il valore della variabile _pacsDaemon
	* \param Valore da assegnare alla variabile _pacsDaemon
	*/
	inline void setPacsDaemon(bool pacsDaemon) {
		_pacsDaemon = pacsDaemon;
	}

	/**
	* \fn void closeActiveSeries()
	* \brief Chiude la serie mostrata nel viewer
	*/
	void closeActiveSeries();

private:
	/**
	* \fn wxTreeItemId addStudyToAlbum(wxArrayString *pInfo, wxTreeItemId parentNode)
	* \brief Aggiunge, a partire dalle informazioni in pInfo, un nuovo studio all'album 
	* \param pInfo Dati da inserire nel record studio dell'album
	* \param parentNode Nodo sotto cui inserire lo studio
	*/
	wxTreeItemId addStudyToAlbum(wxArrayString *pInfo, wxTreeItemId parentNode);

	/**
	* \fn wxTreeItemId addSeriesToAlbum(wxArrayString *pInfo, wxTreeItemId parentNode)
	* \brief Aggiunge, a partire dalle informazioni in pInfo, un nuovo serie al parentNode dell'album 
	* \param pInfo Dati da inserire nel record serie dell'album
	* \param parentNode Nodo sotto cui inserire la serie
	*/
	wxTreeItemId addSeriesToAlbum(wxArrayString *pInfo, wxTreeItemId parentNode, int iSeriesImagesNum);

	DECLARE_EVENT_TABLE()
};

enum {
	Tool_importfile = 0,
	Tool_importfolder,
	Tool_importstudy,
	Tool_importdicomdir,
	Tool_queryretrieve,
	Tool_export,
	Tool_anonymizer,
	Tool_send,
	Tool_viewer,
	Tool_volumerendering,
	Tool_surface,
	Tool_trash,
	Tool_max,
	m_fileImport,
	m_fileImportFile,
	m_fileImportSeries,
	m_fileImportStudy,
	m_fileImportDicomdir,
	m_fileExport,
	m_fileExportBMP,
	m_fileExportDICOM,
	m_fileExportDICOMDIR,
	m_fileHeader,
	m_fileClose,
	m_fileAnonymize,
	m_fileDeleteSelectedExam,
	m_viewerNew,
	m_viewerNextSlice,
	m_viewerPrevSlice,
	m_viewerResetImageView,
	m_viewerFlipImageHorizontal,
	m_viewerFlipImageVertical,
	m_imageFusion,
	m_pacsSendToPacs,
	m_pacsQueryRetrieve,
	m_pacsAutosavePreferences,
	m_pacsOpenPreferences,
	m_pacsSavePreferences,
	m_pacsSavePreferencesAs,
	m_pacsPreferences,
	m_HelpAbout,
	id_ToolBar,
	id_Thumbnails,
	id_ImageBox,
	id_Thumb_Window,
    id_Albums_Window,
	id_ImageBox_Window,
	id_Slider,
	id_Albums,
	id_AutoPlay,
	id_Query,
	id_Send,
	id_Play,
	id_Stop,
	m_contextSendToPACS,
	m_contextDeleteLocal
};

#endif _wxMainGui_h_