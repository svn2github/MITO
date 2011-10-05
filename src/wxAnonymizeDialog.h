/**
 * \file wxAnonymizeDialog.h
 * \brief File per la creazione del dialog per l'anonimizzazione
 * \author ICAR-CNR Napoli
 */

#ifndef _wxAnonymizeDialog_h_
#define _wxAnonymizeDialog_h_

#include "itkVtkData.h"

#define SYMBOL_ANONYMIZEDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxDIALOG_MODAL
#define SYMBOL_ANONYMIZEDIALOG_TITLE _("Anonymize")
#define SYMBOL_ANONYMIZEDIALOG_IDNAME ID_DIALOG
#define SYMBOL_ANONYMIZEDIALOG_SIZE wxSize(700, 650)
#define SYMBOL_ANONYMIZEDIALOG_POSITION wxDefaultPosition
#define ID_DIALOG 1000
#define id_CheckPatientName 1001
#define id_PatientName 1002
#define id_CheckPatientId 1003
#define id_PatientId 1004
#define id_CheckPatientAge 1005
#define id_PatientAge 1006
#define id_CheckPatientBirthdate 1007
#define id_PatientBirthdate 1008
#define id_CheckInstitutionName 1009
#define id_InstitutionName 1010
#define id_CheckStudyDate 1011
#define id_StudyDate 1012
#define id_CheckStudyTime 1013
#define id_StudyTime 1014
#define id_CheckAcquisitionDate 1015
#define id_AcquisitionDate 106
#define id_CheckSeriesDate 1017
#define id_SeriesDate 1018
#define id_CheckSeriesTime 1019
#define id_SeriesTime 1020
#define id_CheckImageDate 1021
#define id_ImageDate 1022
#define id_CheckImageTime 1023
#define id_ImageTime 1024
#define id_CheckReferringPhysician 1025
#define id_ReferringPhysician 1026
#define id_CheckAccessionNumber 1027
#define id_AccessionNumber 1028
#define id_CheckPatientSex 1029
#define id_PatientSex 1030
#define id_CheckPatientWeight 1031
#define id_PatientWeight 1032
#define id_CheckTrialSponsorName 1033
#define id_TrialSponsorName 1034
#define id_CheckTrialProtocolId 1035
#define id_TrialProtocolId 1036
#define id_CheckTrialProtocolName 1037
#define id_TrialProtocolName 1038
#define id_CheckTrialSiteId 1039
#define id_TrialSiteId 1040
#define id_CheckTrialSiteName 1041
#define id_TrialSiteName 1042
#define id_CheckTrialSubjectReadingId 1043
#define id_TrialSubjectReadingId 1044
#define id_CheckTrialSubjectId 1045
#define id_TrialSubjectId 1046
#define id_CheckTrialTimePointId 1047
#define id_TrialTimePointId 1048
#define id_CheckTrialTimePointDescription 1049
#define id_TrialTimePointDescription 1050
#define id_CheckTrialCoordinatingCenterName 1051
#define id_TrialCoordinatingCenterName 1052
#define id_CheckPerformingPhysician 1053
#define id_PerformingPhysician 1054
#define id_CheckPhysicianOfRecord 1055
#define id_PhysicianOfRecord 1056

#define ID_SAVE 1057


/**
 * \class wxAnonymizeDialog
 * \brief Dialog per l'anonimizzazione delle immagini o delle serie. Deriva da wxDialog
 */
class wxAnonymizeDialog: public wxDialog {    

private:
	/** 
	 * \var string _inputPath
	 * \brief Percorso del file o della directory da anonimizzare
	 */
	string _inputPath;

	/** 
	 * \var bool _series
	 * \brief Indica se il dato è un'immagine o una serie
	 */
	bool _series;

	/** 
	 * \var bool _rgb
	 * \brief Indica se l'immagine o la serie è in RGB o in scala di grigi
	 */
	bool _rgb;

	/** 
	 * \var wxCheckBox* _checkBoxPatientName
	 * \brief Checkbox per il nome del paziente
	 */
	wxCheckBox* _checkBoxPatientName;

	/** 
	 * \var wxTextCtrl* _textPatientName
	 * \brief Casella di testo per il nome del paziente
	 */
	wxTextCtrl* _textPatientName;

	/** 
	 * \var wxCheckBox* _checkBoxPatientId
	 * \brief Checkbox per l'identificativo del paziente
	 */
	wxCheckBox* _checkBoxPatientId;

	/** 
	 * \var wxTextCtrl* _textPatientId
	 * \brief Casella di testo per l'identificativo del paziente
	 */
	wxTextCtrl* _textPatientId;

	/** 
	 * \var wxCheckBox* _checkBoxPatientAge
	 * \brief Checkbox per l'età del paziente
	 */
	wxCheckBox* _checkBoxPatientAge;

	/** 
	 * \var wxTextCtrl* _textPatientAge
	 * \brief Casella di testo per l'età del paziente
	 */
	wxTextCtrl* _textPatientAge;	

	/** 
	 * \var wxCheckBox* _checkBoxPatientBirthdate
	 * \brief Checkbox per la data di nascita del paziente
	 */
	wxCheckBox* _checkBoxPatientBirthdate;

	/** 
	 * \var wxTextCtrl* _textPatientBirthdate
	 * \brief Casella di testo per la data di nascita del paziente
	 */
	wxTextCtrl* _textPatientBirthdate;

	/** 
	 * \var wxCheckBox* _checkBoxInstitutionName
	 * \brief Checkbox per il nome dell'istituto
	 */
	wxCheckBox* _checkBoxInstitutionName;

	/** 
	 * \var wxTextCtrl* _textInstitutionName
	 * \brief Casella di testo per il nome dell'istituto
	 */
	wxTextCtrl* _textInstitutionName;

	/** 
	 * \var wxCheckBox* _checkBoxStudyDate
	 * \brief Checkbox per la data in cui è stato effettuato lo studio
	 */
	wxCheckBox* _checkBoxStudyDate;

	/** 
	 * \var wxTextCtrl* _textStudyDate
	 * \brief Casella di testo per la data in cui è stato effettuato lo studio
	 */
	wxTextCtrl* _textStudyDate;

	/** 
	 * \var wxCheckBox* _checkBoxStudyTime
	 * \brief Checkbox per l'ora in cui è stato effettuato lo studio
	 */
	wxCheckBox* _checkBoxStudyTime;

	/** 
	 * \var wxTextCtrl* _textStudyTime
	 * \brief Casella di testo per l'ora in cui è stato effettuato lo studio
	 */
	wxTextCtrl* _textStudyTime;

	/** 
	 * \var wxCheckBox* _checkBoxAcquisitionDate
	 * \brief Checkbox per la data di acquisizione
	 */
	wxCheckBox* _checkBoxAcquisitionDate;

	/** 
	 * \var wxTextCtrl* _textAcquisitionDate
	 * \brief Casella di testo per la data di acquisizione
	 */
	wxTextCtrl* _textAcquisitionDate;

	/** 
	 * \var wxCheckBox* _checkBoxSeriesDate
	 * \brief Checkbox per la data in cui è stata effettuata la serie
	 */
	wxCheckBox* _checkBoxSeriesDate;

	/** 
	 * \var wxTextCtrl* _textSeriesDate
	 * \brief Casella di testo per la data in cui è stata effettuata la serie
	 */
	wxTextCtrl* _textSeriesDate;

	/** 
	 * \var wxCheckBox* _checkBoxSeriesTime
	 * \brief Checkbox per l'ora in cui è stata effettuata la serie
	 */
	wxCheckBox* _checkBoxSeriesTime;

	/** 
	 * \var wxTextCtrl* _textSeriesTime
	 * \brief Casella di testo per l'ora in cui è stata effettuata la serie
	 */
	wxTextCtrl* _textSeriesTime;

	/** 
	 * \var wxCheckBox* _checkBoxImageDate
	 * \brief Checkbox per la data in cui è stata effettuata l'immagine
	 */
	wxCheckBox* _checkBoxImageDate;

	/** 
	 * \var wxTextCtrl* _textImageDate
	 * \brief Casella di testo per la data in cui è stata effettuata l'immagine
	 */
	wxTextCtrl* _textImageDate;

	/** 
	 * \var wxCheckBox* _checkBoxImageTime
	 * \brief Checkbox per l'ora in cui è stata effettuata l'immagine
	 */
	wxCheckBox* _checkBoxImageTime;

	/** 
	 * \var wxTextCtrl* _textImageTime
	 * \brief Casella di testo per l'ora in cui è stata effettuata l'immagine
	 */
	wxTextCtrl* _textImageTime;

	/** 
	 * \var wxCheckBox* _checkBoxReferringPhysician
	 * \brief Checkbox per il nome del medico refertante
	 */
	wxCheckBox* _checkBoxReferringPhysician;

	/** 
	 * \var wxTextCtrl* _textReferringPhysician
	 * \brief Casella di testo per il nome del medico refertante
	 */
	wxTextCtrl* _textReferringPhysician;

	/** 
	 * \var wxCheckBox* _checkBoxAccessionNumber
	 * \brief Checkbox per il numero dell'assunzione
	 */
	wxCheckBox* _checkBoxAccessionNumber;

	/** 
	 * \var wxTextCtrl* _textAccessionNumber
	 * \brief Casella di testo per il numero dell'assunzione
	 */
	wxTextCtrl* _textAccessionNumber;

	/** 
	 * \var wxCheckBox* _checkBoxPatientSex
	 * \brief Checkbox per il sesso del paziente
	 */
	wxCheckBox* _checkBoxPatientSex;

	/** 
	 * \var wxTextCtrl* _textPatientSex
	 * \brief Casella di testo per il sesso del paziente
	 */
	wxTextCtrl* _textPatientSex;

	/** 
	 * \var wxCheckBox* _checkBoxPatientWeight
	 * \brief Checkbox per il peso del paziente
	 */
	wxCheckBox* _checkBoxPatientWeight;

	/** 
	 * \var wxTextCtrl* _textPatientWeight
	 * \brief Casella di testo per il peso del paziente
	 */
	wxTextCtrl* _textPatientWeight;

	/** 
	 * \var wxCheckBox* _checkBoxTrialSponsorName
	 * \brief Checkbox per il nome del trial sponsor
	 */
	wxCheckBox* _checkBoxTrialSponsorName;

	/** 
	 * \var wxTextCtrl* _textTrialSponsorName
	 * \brief Casella di testo per il nome del trial sponsor
	 */
	wxTextCtrl* _textTrialSponsorName;

	/** 
	 * \var wxCheckBox* _checkBoxTrialProtocolId
	 * \brief Checkbox per l'identificativo del trial protocol
	 */
	wxCheckBox* _checkBoxTrialProtocolId;

	/** 
	 * \var wxTextCtrl* _textTrialProtocolId
	 * \brief Casella di testo per l'identificativo del trial protocol
	 */
	wxTextCtrl* _textTrialProtocolId;

	/** 
	 * \var wxCheckBox* _checkBoxTrialSiteId
	 * \brief Checkbox per l'identificativo del trial site
	 */
	wxCheckBox* _checkBoxTrialSiteId;

	/** 
	 * \var wxTextCtrl* _textTrialSiteId
	 * \brief Casella di testo per l'identificativo del trial site
	 */
	wxTextCtrl* _textTrialSiteId;

	/** 
	 * \var wxCheckBox* _checkBoxTrialSiteName
	 * \brief Checkbox per il nome del paziente trial site
	 */
	wxCheckBox* _checkBoxTrialSiteName;

	/** 
	 * \var wxTextCtrl* _textTrialSiteName
	 * \brief Casella di testo per il nome del paziente trial site
	 */
	wxTextCtrl* _textTrialSiteName;

	/** 
	 * \var wxCheckBox* _checkBoxTrialSubjectReadingId
	 * \brief Checkbox per l'identificarivo del trial subject reading
	 */
	wxCheckBox* _checkBoxTrialSubjectReadingId;

	/** 
	 * \var wxTextCtrl* _textTrialSubjectReadingId
	 * \brief Casella di testo per l'identificarivo del trial subject reading
	 */
	wxTextCtrl* _textTrialSubjectReadingId;

	/** 
	 * \var wxCheckBox* _checkBoxTrialSubjectId
	 * \brief Checkbox per l'identificativo del trial subject
	 */
	wxCheckBox* _checkBoxTrialSubjectId;

	/** 
	 * \var wxTextCtrl* _textTrialSubjectId
	 * \brief Casella di testo per l'identificativo del trial subject
	 */
	wxTextCtrl* _textTrialSubjectId;

	/** 
	 * \var wxCheckBox* _checkBoxTrialTimePointId
	 * \brief Checkbox per l'identificativo del trial time point
	 */
	wxCheckBox* _checkBoxTrialTimePointId;

	/** 
	 * \var wxTextCtrl* _textTrialTimePointId
	 * \brief Casella di testo per l'identificativo del trial time point
	 */
	wxTextCtrl* _textTrialTimePointId;

	/** 
	 * \var wxCheckBox* _checkBoxTrialTimePointDescription
	 * \brief Checkbox per la descrizione del trial time point
	 */
	wxCheckBox* _checkBoxTrialTimePointDescription;

	/** 
	 * \var wxTextCtrl* _textTrialTimePointDescription
	 * \brief Casella di testo per la descrizione del trial time point
	 */
	wxTextCtrl* _textTrialTimePointDescription;

	/** 
	 * \var wxCheckBox* _checkBoxTrialCoordinatingCenterName
	 * \brief Checkbox per il nome del trial coordinating name
	 */
	wxCheckBox* _checkBoxTrialCoordinatingCenterName;

	/** 
	 * \var wxTextCtrl* _textTrialCoordinatingCenterName
	 * \brief Casella di testo per il nome del trial coordinating name
	 */
	wxTextCtrl* _textTrialCoordinatingCenterName;

	/** 
	 * \var wxCheckBox* _checkBoxTrialProtocolName
	 * \brief Checkbox per il nome del trial protocol
	 */
	wxCheckBox* _checkBoxTrialProtocolName;

	/** 
	 * \var wxTextCtrl* _textTrialProtocolName
	 * \brief Casella di testo per il nome del trial protocol
	 */
	wxTextCtrl* _textTrialProtocolName;

	/** 
	 * \var wxCheckBox* _checkBoxPerformingPhysician
	 * \brief Checkbox per il nome del medico che ha realizzato l'esame
	 */
	wxCheckBox* _checkBoxPerformingPhysician;

	/** 
	 * \var wxTextCtrl* _textPerformingPhysician
	 * \brief Casella di testo per il nome del medico che ha realizzato l'esame
	 */
	wxTextCtrl* _textPerformingPhysician;

	/** 
	 * \var wxCheckBox* _checkBoxPhysicianOfRecord
	 * \brief Checkbox per il nome del medico proprio del paziente
	 */
	wxCheckBox* _checkBoxPhysicianOfRecord;

	/** 
	 * \var wxTextCtrl* _textPhysicianOfRecord
	 * \brief Casella di testo per il nome del medico proprio del paziente
	 */
	wxTextCtrl* _textPhysicianOfRecord;
	
public:
	/** Costruttore con parametri la finestra padre, l'identificativo, il titolo,  la posizione, la dimensione e lo stile del dialog */
	wxAnonymizeDialog(wxWindow* parent, wxWindowID id = SYMBOL_ANONYMIZEDIALOG_IDNAME, const wxString& caption = SYMBOL_ANONYMIZEDIALOG_TITLE, const wxPoint& pos = SYMBOL_ANONYMIZEDIALOG_POSITION, const wxSize& size = SYMBOL_ANONYMIZEDIALOG_SIZE, long style = SYMBOL_ANONYMIZEDIALOG_STYLE);

	/**
	* \fn bool Create(wxWindow* parent, wxWindowID id = SYMBOL_ANONYMIZEDIALOG_IDNAME, const wxString& caption = SYMBOL_ANONYMIZEDIALOG_TITLE, const wxPoint& pos = SYMBOL_ANONYMIZEDIALOG_POSITION, const wxSize& size = SYMBOL_ANONYMIZEDIALOG_SIZE, long style = SYMBOL_ANONYMIZEDIALOG_STYLE)
	* \brief Crea la finestra
	* \param parent Finestra padre
	* \param id Identificativo del dialog
	* \param caption Titolo del dialog
	* \param pos Posizione del dialog
	* \param size Dimensione del dialog
	* \param style Stile del dialog
	* \return true se l'operazione va buon fine
	*/
    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_ANONYMIZEDIALOG_IDNAME, const wxString& caption = SYMBOL_ANONYMIZEDIALOG_TITLE, const wxPoint& pos = SYMBOL_ANONYMIZEDIALOG_POSITION, const wxSize& size = SYMBOL_ANONYMIZEDIALOG_SIZE, long style = SYMBOL_ANONYMIZEDIALOG_STYLE);

	/**
	* \fn void CreateControls()
	* \brief Crea i controlli
	*/
	void CreateControls();
	
	/**
	* \fn void onSave(wxCommandEvent& event)
	* \brief Salva l'immagine o la serie anonimizzata
	* \param event Evento
	*/
	void onSave(wxCommandEvent& event);

	/**
	* \fn void EnableCheck(wxCommandEvent& event)
	* \brief Spunta i checkbox selezionati
	* \param event Evento
	*/
	void EnableCheck(wxCommandEvent& event);

	/**
	* \fn wxBitmap GetBitmapResource(const wxString& name)
	* \brief Restituisce le risorse bitmap
	* \param name Nome bitmap
	* \return Risorsa bitmap
	*/
    wxBitmap GetBitmapResource(const wxString& name);

	/**
	* \fn wxIcon GetIconResource(const wxString& name)
	* \brief Restituisce le icone
	* \param name Nome icona
	* \return Icona
	*/
	wxIcon GetIconResource(const wxString& name);

	/**
	* \fn static bool ShowToolTips()
	* \brief Attiva/disattiva la visualizzazione dei tooltip
	* \return true se la visualizzazione è attivata
	*/
	static bool ShowToolTips();

	/**
	* \fn void setInputPath(string inputPath)
	* \brief Assegna il percorso di ingresso
	* \param inputPath Percorso di ingresso da assegnare
	*/
	inline void setInputPath(string inputPath) {
		_inputPath = inputPath;
	}

	/**
	* \fn void setSeries(bool series)
	* \brief Setta la variabile _series
	* \param series Valore da assegnare alla variabile _series
	*/
	inline void setSeries(bool series) {
		_series = series;
	}

	/**
	* \fn void setRgb(bool rgb)
	* \brief Setta la variabile _rgb
	* \param rgb Valore da assegnare alla variabile _rgb
	*/
	inline void setRgb(bool rgb) {
		_rgb = rgb;
	}

private:
	DECLARE_EVENT_TABLE()

};

#endif _wxAnonymizeDialog_h_