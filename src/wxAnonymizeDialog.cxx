/**
 * \file wxAnonymizeDialog.cxx
 * \brief File per la creazione del dialog per l'anonimizzazione
 * \author ICAR-CNR Napoli
 */




// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "wxAnonymizeDialog.h"
#include "anonymizeFilter.h"
#include <wx/busyinfo.h>


BEGIN_EVENT_TABLE( wxAnonymizeDialog, wxDialog )

	EVT_CHECKBOX(id_CheckPatientName, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckPatientId, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckPatientAge, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckPatientBirthdate, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckInstitutionName, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckStudyDate, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckStudyTime, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckAcquisitionDate, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckSeriesDate, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckSeriesTime, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckImageDate, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckImageTime, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckReferringPhysician, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckAccessionNumber, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckPatientSex, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckPatientWeight, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialSponsorName, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialProtocolId, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialProtocolName, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialSiteId, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialSiteName, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialSubjectReadingId, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialSubjectId, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialTimePointId, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialTimePointDescription, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckTrialCoordinatingCenterName, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckPerformingPhysician, wxAnonymizeDialog::EnableCheck)
	EVT_CHECKBOX(id_CheckPhysicianOfRecord, wxAnonymizeDialog::EnableCheck)
	EVT_BUTTON(ID_SAVE, wxAnonymizeDialog::onSave)

END_EVENT_TABLE()




wxAnonymizeDialog::wxAnonymizeDialog(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style) {
    Create(parent, id, caption, pos, size, style);
}



bool wxAnonymizeDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();

	_inputPath = "";
	_series = false;
	_rgb = false;

    return true;
}

/*!
 * Control creation for dialogAnonimize
 */

void wxAnonymizeDialog::CreateControls() {    
	wxAnonymizeDialog* itemDialog = this;

    _checkBoxPatientName = new wxCheckBox( itemDialog, id_CheckPatientName, _("Patient's Name"), wxPoint(34, 34), wxDefaultSize, 0 );
    _checkBoxPatientName->SetValue(false);
	_textPatientName = new wxTextCtrl( itemDialog, id_PatientName, _T("Anonymized"), wxPoint(170,30), wxSize(150,20), 0 );
	_textPatientName->Enable(false);

	_checkBoxPatientId = new wxCheckBox( itemDialog, id_CheckPatientId, _("Patient's ID"), wxPoint(34, 74), wxDefaultSize, 0 );
    _checkBoxPatientId->SetValue(false);
	_textPatientId = new wxTextCtrl( itemDialog, id_PatientId, _T("000"), wxPoint(170,70), wxSize(150,20), 0 );
	_textPatientId->Enable(false);

	_checkBoxPatientAge = new wxCheckBox( itemDialog, id_CheckPatientAge, _("Patient's Age"), wxPoint(34, 114), wxDefaultSize, 0 );
    _checkBoxPatientAge->SetValue(false);
	_textPatientAge = new wxTextCtrl( itemDialog, id_PatientAge, _T("000Y"), wxPoint(170,110), wxSize(150,20), 0 );
	_textPatientAge->Enable(false);

	_checkBoxPatientBirthdate = new wxCheckBox( itemDialog, id_CheckPatientBirthdate, _("Patient's Date of Birth"), wxPoint(34, 154), wxDefaultSize, 0 );
    _checkBoxPatientBirthdate->SetValue(false);
	_textPatientBirthdate = new wxTextCtrl( itemDialog, id_PatientBirthdate, _T("99999999"), wxPoint(170,150), wxSize(150,20), 0 );
	_textPatientBirthdate->Enable(false);
	
	_checkBoxInstitutionName = new wxCheckBox( itemDialog, id_CheckInstitutionName, _("Institution Name"), wxPoint(34, 194), wxDefaultSize, 0 );
    _checkBoxInstitutionName->SetValue(false);
	_textInstitutionName= new wxTextCtrl( itemDialog, id_InstitutionName, _T("NULL"), wxPoint(170,190), wxSize(150,20), 0 );
	_textInstitutionName->Enable(false);

	_checkBoxStudyDate = new wxCheckBox( itemDialog, id_CheckStudyDate, _("Study Date"), wxPoint(34, 234), wxDefaultSize, 0 );
    _checkBoxStudyDate->SetValue(false);
	_textStudyDate = new wxTextCtrl( itemDialog, id_StudyDate, _T("99999999"), wxPoint(170,230), wxSize(150,20), 0 );
	_textStudyDate->Enable(false);

	_checkBoxStudyTime = new wxCheckBox( itemDialog, id_CheckStudyTime, _("Study Time"), wxPoint(34, 274), wxDefaultSize, 0 );
    _checkBoxStudyTime->SetValue(false);
	_textStudyTime = new wxTextCtrl( itemDialog, id_StudyTime, _T("000000.00"), wxPoint(170,270), wxSize(150,20), 0 );
	_textStudyTime->Enable(false);
	
	_checkBoxAcquisitionDate = new wxCheckBox( itemDialog, id_CheckAcquisitionDate, _("Acquisition Date"), wxPoint(34, 314), wxDefaultSize, 0 );
    _checkBoxAcquisitionDate->SetValue(false);
	_textAcquisitionDate = new wxTextCtrl( itemDialog, id_AcquisitionDate, _T("99999999"), wxPoint(170,310), wxSize(150,20), 0 );
	_textAcquisitionDate->Enable(false);

	_checkBoxSeriesDate = new wxCheckBox( itemDialog, id_CheckSeriesDate, _("Series Date"), wxPoint(34, 354), wxDefaultSize, 0 );
    _checkBoxSeriesDate->SetValue(false);
	_textSeriesDate = new wxTextCtrl( itemDialog, id_SeriesDate, _T("99999999"), wxPoint(170,350), wxSize(150,20), 0 );
	_textSeriesDate->Enable(false);
	
	_checkBoxSeriesTime = new wxCheckBox( itemDialog, id_CheckSeriesTime, _("Series Time"), wxPoint(34, 394), wxDefaultSize, 0 );
    _checkBoxSeriesTime->SetValue(false);
	_textSeriesTime= new wxTextCtrl( itemDialog, id_SeriesTime, _T("000000.00"), wxPoint(170,390), wxSize(150,20), 0 );
	_textSeriesTime->Enable(false);

	_checkBoxImageDate = new wxCheckBox( itemDialog, id_CheckImageDate, _("Image Date"), wxPoint(34, 434), wxDefaultSize, 0 );
	_checkBoxImageDate->SetValue(false);
	_textImageDate = new wxTextCtrl( itemDialog, id_ImageDate, _T("99999999"), wxPoint(170,430), wxSize(150,20), 0 );
	_textImageDate->Enable(false);

	_checkBoxImageTime= new wxCheckBox( itemDialog, id_CheckImageTime, _("Image Time"), wxPoint(34, 474), wxDefaultSize, 0 );
    _checkBoxImageTime->SetValue(false);
	_textImageTime = new wxTextCtrl( itemDialog, id_ImageTime, _T("000000.00"), wxPoint(170,470), wxSize(150,20), 0 );
	_textImageTime->Enable(false);

	_checkBoxReferringPhysician = new wxCheckBox( itemDialog, id_CheckReferringPhysician, _("Referring Physician"), wxPoint(34, 514), wxDefaultSize, 0 );
	_checkBoxReferringPhysician->SetValue(false);
	_textReferringPhysician = new wxTextCtrl( itemDialog, id_ReferringPhysician, _T("NULL"), wxPoint(170,510), wxSize(150,20), 0 );
	_textReferringPhysician->Enable(false);
	
	_checkBoxAccessionNumber = new wxCheckBox( itemDialog, id_CheckAccessionNumber, _("Accession Number"), wxPoint(34, 554), wxDefaultSize, 0 );
    _checkBoxAccessionNumber->SetValue(false);
	_textAccessionNumber = new wxTextCtrl( itemDialog, id_AccessionNumber, _T("000"), wxPoint(170,550), wxSize(150,20), 0 );
	_textAccessionNumber->Enable(false);

	_checkBoxPatientSex = new wxCheckBox( itemDialog, id_CheckPatientSex, _("Patient's Sex"), wxPoint(350, 34), wxDefaultSize, 0 );
    _checkBoxPatientSex->SetValue(false);
	_textPatientSex= new wxTextCtrl( itemDialog, id_PatientSex, _T("O"), wxPoint(530,30), wxSize(150,20), 0 );
	_textPatientSex->Enable(false);

	_checkBoxPatientWeight= new wxCheckBox( itemDialog, id_CheckPatientWeight, _("Patient's Weight"), wxPoint(350, 74), wxDefaultSize, 0 );
    _checkBoxPatientWeight->SetValue(false);
	_textPatientWeight = new wxTextCtrl( itemDialog, id_PatientWeight, _T("00"), wxPoint(530,70), wxSize(150,20), 0 );
	_textPatientWeight->Enable(false);

	_checkBoxTrialSponsorName = new wxCheckBox( itemDialog, id_CheckTrialSponsorName, _("Trial Sponsor Name"), wxPoint(350, 114), wxDefaultSize, 0 );
    _checkBoxTrialSponsorName->SetValue(false);
	_textTrialSponsorName= new wxTextCtrl( itemDialog, id_TrialSponsorName, _T("NULL"), wxPoint(530,110), wxSize(150,20), 0 );
	_textTrialSponsorName->Enable(false);

	_checkBoxTrialProtocolId = new wxCheckBox( itemDialog, id_CheckTrialProtocolId, _("Trial Protocol ID"), wxPoint(350, 154), wxDefaultSize, 0 );
    _checkBoxTrialProtocolId->SetValue(false);
	_textTrialProtocolId = new wxTextCtrl( itemDialog, id_TrialProtocolId, _T("000"), wxPoint(530,150), wxSize(150,20), 0 );
	_textTrialProtocolId->Enable(false);

	_checkBoxTrialProtocolName = new wxCheckBox( itemDialog, id_CheckTrialProtocolName, _("Trial Protocol Name"), wxPoint(350, 194), wxDefaultSize, 0 );
    _checkBoxTrialProtocolName->SetValue(false);
	_textTrialProtocolName = new wxTextCtrl( itemDialog, id_TrialProtocolName, _T("NULL"), wxPoint(530,190), wxSize(150,20), 0 );
	_textTrialProtocolName->Enable(false);

	_checkBoxTrialSiteId = new wxCheckBox( itemDialog, id_CheckTrialSiteId, _("Trial Site ID"), wxPoint(350, 234), wxDefaultSize, 0 );
    _checkBoxTrialSiteId->SetValue(false);
	_textTrialSiteId = new wxTextCtrl( itemDialog, id_TrialSiteId, _T("000"), wxPoint(530,230), wxSize(150,20), 0 );
	_textTrialSiteId->Enable(false);

	_checkBoxTrialSiteName = new wxCheckBox( itemDialog, id_CheckTrialSiteName, _("Trial Site Name"), wxPoint(350, 274), wxDefaultSize, 0 );
    _checkBoxTrialSiteName->SetValue(false);
	_textTrialSiteName = new wxTextCtrl( itemDialog, id_TrialSiteName, _T("NULL"), wxPoint(530,270), wxSize(150,20), 0 );
	_textTrialSiteName->Enable(false);
	
	_checkBoxTrialSubjectReadingId = new wxCheckBox( itemDialog, id_CheckTrialSubjectReadingId, _("Trial Subject Reading ID"), wxPoint(350, 314), wxDefaultSize, 0 );
    _checkBoxTrialSubjectReadingId->SetValue(false);
	_textTrialSubjectReadingId = new wxTextCtrl( itemDialog, id_TrialSubjectReadingId, _T("000"), wxPoint(530,310), wxSize(150,20), 0 );
	_textTrialSubjectReadingId->Enable(false);

	_checkBoxTrialSubjectId= new wxCheckBox( itemDialog, id_CheckTrialSubjectId, _("Trial Subject ID"), wxPoint(350, 354), wxDefaultSize, 0 );
    _checkBoxTrialSubjectId->SetValue(false);
	_textTrialSubjectId = new wxTextCtrl( itemDialog, id_TrialSubjectId, _T("000"), wxPoint(530,350), wxSize(150,20), 0 );
	_textTrialSubjectId->Enable(false);

	_checkBoxTrialTimePointId = new wxCheckBox( itemDialog, id_CheckTrialTimePointId, _("Trial Time Point ID"), wxPoint(350, 394), wxDefaultSize, 0 );
    _checkBoxTrialTimePointId->SetValue(false);
	_textTrialTimePointId = new wxTextCtrl( itemDialog, id_TrialTimePointId, _T("000"), wxPoint(530,390), wxSize(150,20), 0 );
	_textTrialTimePointId->Enable(false);

	_checkBoxTrialTimePointDescription = new wxCheckBox( itemDialog, id_CheckTrialTimePointDescription, _("Trial Time Point Description"), wxPoint(350, 434), wxDefaultSize, 0 );
    _checkBoxTrialTimePointDescription->SetValue(false);
	_textTrialTimePointDescription = new wxTextCtrl( itemDialog, id_TrialTimePointDescription, _T("NULL"), wxPoint(530,430), wxSize(150,20), 0 );
	_textTrialTimePointDescription->Enable(false);

	_checkBoxTrialCoordinatingCenterName = new wxCheckBox( itemDialog, id_CheckTrialCoordinatingCenterName, _("Trial Coordinating Center Name"), wxPoint(350, 474), wxDefaultSize, 0 );
    _checkBoxTrialCoordinatingCenterName->SetValue(false);
	_textTrialCoordinatingCenterName = new wxTextCtrl( itemDialog, id_TrialCoordinatingCenterName, _T("NULL"), wxPoint(530,470), wxSize(150,20), 0 );
	_textTrialCoordinatingCenterName->Enable(false);

	_checkBoxPerformingPhysician = new wxCheckBox( itemDialog, id_CheckPerformingPhysician, _("Performing Physician"), wxPoint(350, 514), wxDefaultSize, 0 );
    _checkBoxPerformingPhysician->SetValue(false);
	_textPerformingPhysician = new wxTextCtrl( itemDialog, id_PerformingPhysician, _T("NULL"), wxPoint(530,510), wxSize(150,20), 0 );
	_textPerformingPhysician->Enable(false);

	_checkBoxPhysicianOfRecord = new wxCheckBox( itemDialog, id_CheckPhysicianOfRecord, _("Physician of Record"), wxPoint(350, 554), wxDefaultSize, 0 );
    _checkBoxPhysicianOfRecord->SetValue(false);
	_textPhysicianOfRecord = new wxTextCtrl( itemDialog, id_PhysicianOfRecord, _T("NULL"), wxPoint(530,550), wxSize(150,20), 0 );
	_textPhysicianOfRecord->Enable(false);

	wxStdDialogButtonSizer* itemStdDialogButtonSizer = new wxStdDialogButtonSizer;

	this->SetSizer(itemStdDialogButtonSizer);
	
	itemStdDialogButtonSizer->Realize();

	wxButton* itemButtonSave = new wxButton( this, ID_SAVE, _("&Save"), wxPoint(500,585), wxDefaultSize, 0 );
    itemStdDialogButtonSizer->AddButton(itemButtonSave);

    wxButton* itemButtonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxPoint(600,585), wxDefaultSize, 0 );
    itemStdDialogButtonSizer->AddButton(itemButtonCancel);
}


void wxAnonymizeDialog::EnableCheck(wxCommandEvent& event) {
	if(_checkBoxPatientName->GetValue()==true) {	
		_textPatientName->Enable(true);
	}
	else _textPatientName->Enable(false);
		
	if(_checkBoxPatientId->GetValue()==true) {	
		_textPatientId->Enable(true);
	}
	else _textPatientId->Enable(false);

	if(_checkBoxPatientAge->GetValue()==true) {
		_textPatientAge->Enable(true);
		//if(!strcmp(_textPatientAge->GetValue(), "nnnX, X={D|W|M|Y}")) _textPatientAge->SetValue("");
	}
	else _textPatientAge->Enable(false);

	if(_checkBoxPatientBirthdate->GetValue()==true) {
		_textPatientBirthdate->Enable(true);
		//if(!strcmp(_textPatientBirthdate->GetValue(), "yyyymmdd")) _textPatientBirthdate->SetValue("");
	}
	else _textPatientBirthdate->Enable(false);

	if(_checkBoxInstitutionName->GetValue()==true) {
		_textInstitutionName->Enable(true);
	}
	else _textInstitutionName->Enable(false);

	if(_checkBoxStudyDate->GetValue()==true) {
		_textStudyDate->Enable(true);
		//if(!strcmp(_textStudyDate->GetValue(), "yyyymmdd")) _textStudyDate->SetValue("");
	}
	else _textStudyDate->Enable(false);

	if(_checkBoxStudyTime->GetValue()==true) {
		_textStudyTime->Enable(true);
		//if(!strcmp(_textStudyTime->GetValue(), "hhmmss.frac")) _textStudyTime->SetValue("");
	}
	else _textStudyTime->Enable(false);

	if(_checkBoxAcquisitionDate->GetValue()==true) {
		_textAcquisitionDate->Enable(true);
		//if(!strcmp(_textAcquisitionDate->GetValue(), "yyyymmdd")) _textAcquisitionDate->SetValue("");
	}
	else _textAcquisitionDate->Enable(false);

	if(_checkBoxSeriesDate->GetValue()==true) {
		_textSeriesDate->Enable(true);
		//if(!strcmp(_textSeriesDate->GetValue(), "yyyymmdd")) _textSeriesDate->SetValue("");
	}
	else _textSeriesDate->Enable(false);
	
	if(_checkBoxSeriesTime->GetValue()==true) {
		_textSeriesTime->Enable(true);
		//if(!strcmp(_textSeriesTime->GetValue(), "hhmmss.frac")) _textSeriesTime->SetValue("");
	}
	else _textSeriesTime->Enable(false);

	if(_checkBoxImageDate->GetValue()==true) {
		_textImageDate->Enable(true);
		//if(!strcmp(_textImageDate->GetValue(), "yyyymmdd")) _textImageDate->SetValue("");
	}
	else _textImageDate->Enable(false);

	if(_checkBoxImageTime->GetValue()==true) {
		_textImageTime->Enable(true);
		//if(!strcmp(_textImageTime->GetValue(), "hhmmss.frac")) _textImageTime->SetValue("");
	}
	else _textImageTime->Enable(false);

	if(_checkBoxReferringPhysician->GetValue()==true) {
		_textReferringPhysician->Enable(true);
	}
	else _textReferringPhysician->Enable(false);

	if(_checkBoxAccessionNumber->GetValue()==true) {
		_textAccessionNumber->Enable(true);
	}
	else _textAccessionNumber->Enable(false);

	if(_checkBoxPatientSex->GetValue()==true) {
		_textPatientSex->Enable(true);
	}
	else _textPatientSex->Enable(false);

	if(_checkBoxPatientWeight->GetValue()==true) {
		_textPatientWeight->Enable(true);
	}
	else _textPatientWeight->Enable(false);

	if(_checkBoxTrialSponsorName->GetValue()==true) {
		_textTrialSponsorName->Enable(true);
	}
	else _textTrialSponsorName->Enable(false);

	if(_checkBoxTrialProtocolId->GetValue()==true) {
		_textTrialProtocolId->Enable(true);
	}
	else _textTrialProtocolId->Enable(false);

	if(_checkBoxTrialSiteId->GetValue()==true) {
		_textTrialSiteId->Enable(true);
	}
	else _textTrialSiteId->Enable(false);

	if(_checkBoxTrialSiteName->GetValue()==true) {
		_textTrialSiteName->Enable(true);
	}
	else _textTrialSiteName->Enable(false);

	if(_checkBoxTrialSubjectReadingId->GetValue()==true) {
		_textTrialSubjectReadingId->Enable(true);
	}
	else _textTrialSubjectReadingId->Enable(false);

	if(_checkBoxTrialSubjectId->GetValue()==true) {
		_textTrialSubjectId->Enable(true);
	}
	else _textTrialSubjectId->Enable(false);

	if(_checkBoxTrialProtocolName->GetValue()==true) {
		_textTrialProtocolName->Enable(true);
	}
	else _textTrialProtocolName->Enable(false);

	if(_checkBoxTrialTimePointId->GetValue()==true) {
		_textTrialTimePointId->Enable(true);
	}
	else _textTrialTimePointId->Enable(false);

	if(_checkBoxTrialTimePointDescription->GetValue()==true) {
		_textTrialTimePointDescription->Enable(true);
	}
	else _textTrialTimePointDescription->Enable(false);

	if(_checkBoxTrialCoordinatingCenterName->GetValue()==true) {
		_textTrialCoordinatingCenterName->Enable(true);
	}
	else _textTrialCoordinatingCenterName->Enable(false);

	if(_checkBoxPerformingPhysician->GetValue()==true) {
		_textPerformingPhysician->Enable(true);
	}
	else _textPerformingPhysician->Enable(false);

	if(_checkBoxPhysicianOfRecord->GetValue()==true) {
		_textPhysicianOfRecord->Enable(true);
	}
	else _textPhysicianOfRecord->Enable(false);
}


bool wxAnonymizeDialog::ShowToolTips() {
    return true;
}


wxBitmap wxAnonymizeDialog::GetBitmapResource(const wxString& name) {
    wxUnusedVar(name);
    return wxNullBitmap;
}


wxIcon wxAnonymizeDialog::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

void wxAnonymizeDialog::onSave(wxCommandEvent& event) {	
	bool check = false;
	anonymizeData *aData = new anonymizeData;

	if(_checkBoxPatientName->GetValue()==true) {
		aData->patientName = _textPatientName->GetValue();
		check = true;
	}
	else aData->patientName = "";
		
	if(_checkBoxPatientId->GetValue()==true) {	
		aData->patientId = _textPatientId->GetValue();
		check = true;
	}
	else aData->patientId = "";

	if(_checkBoxPatientAge->GetValue()==true) {
		aData->patientAge = _textPatientAge->GetValue();
		check = true;
	}
	else aData->patientAge = "";

	if(_checkBoxPatientBirthdate->GetValue()==true) {
		aData->patientBirthdate = _textPatientBirthdate->GetValue();
		check = true;
	}
	else aData->patientBirthdate = "";

	if(_checkBoxInstitutionName->GetValue()==true) {
		aData->institutionName = _textInstitutionName->GetValue();
		check = true;
	}
	else aData->institutionName = "";

	if(_checkBoxStudyDate->GetValue()==true) {
		aData->studyDate = _textStudyDate->GetValue();
		check = true;
	}
	else aData->studyDate = "";

	if(_checkBoxStudyTime->GetValue()==true) {
		aData->studyTime = _textStudyTime->GetValue();
		check = true;
	}
	else aData->studyDate = "";

	if(_checkBoxAcquisitionDate->GetValue()==true) {
		aData->acquisitionDate = _textAcquisitionDate->GetValue();
		check = true;
	}
	else aData->acquisitionDate = "";

	if(_checkBoxSeriesDate->GetValue()==true) {
		aData->seriesDate = _textSeriesDate->GetValue();
		check = true;
	}
	else aData->seriesDate = "";
	
	if(_checkBoxSeriesTime->GetValue()==true) {
		aData->seriesTime = _textSeriesTime->GetValue();
		check = true;
	}
	else aData->seriesTime = "";

	if(_checkBoxImageDate->GetValue()==true) {
		aData->imageDate = _textImageDate->GetValue();
		check = true;
	}
	else aData->imageDate = "";

	if(_checkBoxImageTime->GetValue()==true) {
		aData->imageTime = _textImageTime->GetValue();
		check = true;
	}
	else aData->imageTime = "";

	if(_checkBoxReferringPhysician->GetValue()==true) {
		aData->referringPhysician = _textReferringPhysician->GetValue();
		check = true;
	}
	else aData->referringPhysician = "";

	if(_checkBoxAccessionNumber->GetValue()==true) {
		aData->accessionNumber = _textAccessionNumber->GetValue();
		check = true;
	}
	else aData->accessionNumber = "";

	if(_checkBoxPatientSex->GetValue()==true) {
		aData->patientSex = _textPatientSex->GetValue();
		check = true;
	}
	else aData->patientSex = "";

	if(_checkBoxPatientWeight->GetValue()==true) {
		aData->patientWeight = _textPatientWeight->GetValue();
		check = true;
	}
	else aData->patientWeight = "";

	if(_checkBoxTrialSponsorName->GetValue()==true) {
		aData->trialSponsorName = _textTrialSponsorName->GetValue();
		check = true;
	}
	else aData->trialSponsorName = "";

	if(_checkBoxTrialProtocolId->GetValue()==true) {
		aData->trialProtocolId = _textTrialProtocolId->GetValue();
		check = true;
	}
	else aData->trialProtocolId = "";

	if(_checkBoxTrialSiteId->GetValue()==true) {
		aData->trialSiteId = _textTrialSiteId->GetValue();
		check = true;
	}
	else aData->trialSiteId = "";

	if(_checkBoxTrialSiteName->GetValue()==true) {
		aData->trialSiteName = _textTrialSiteName->GetValue();
		check = true;
	}
	else aData->trialSiteName = "";

	if(_checkBoxTrialSubjectReadingId->GetValue()==true) {
		aData->trialSubjectReadingId = _textTrialSubjectReadingId->GetValue();
		check = true;
	}
	else aData->trialSubjectReadingId = "";

	if(_checkBoxTrialSubjectId->GetValue()==true) {
		aData->trialSubjectId = _textTrialSubjectId->GetValue();
		check = true;
	}
	else aData->trialSubjectId = "";

	if(_checkBoxTrialProtocolName->GetValue()==true) {
		aData->trialProtocolName = _textTrialProtocolName->GetValue();
		check = true;
	}
	else aData->trialProtocolName = "";

	if(_checkBoxTrialTimePointId->GetValue()==true) {
		aData->trialTimePointId = _textTrialTimePointId->GetValue();
		check = true;
	}
	else aData->trialTimePointId = "";

	if(_checkBoxTrialTimePointDescription->GetValue()==true) {
		aData->trialTimePointDescription = _textTrialTimePointDescription->GetValue();
		check = true;
	}
	else aData->trialTimePointDescription = "";

	if(_checkBoxTrialCoordinatingCenterName->GetValue()==true) {
		aData->trialCoordinatingCenterName = _textTrialCoordinatingCenterName->GetValue();
		check = true;
	}
	else aData->trialCoordinatingCenterName = "";

	if(_checkBoxPerformingPhysician->GetValue()==true) {
		aData->performingPhysician = _textPerformingPhysician->GetValue();
		check = true;
	}
	else aData->performingPhysician = "";

	if(_checkBoxPhysicianOfRecord->GetValue()==true) {
		aData->physicianOfRecord = _textPhysicianOfRecord->GetValue();
		check = true;
	}
	else aData->physicianOfRecord = "";


	anonymizeFilter anonymize(_rgb);
	string outputPath;
	if(check) {
		if(!_series) {
			wxFileDialog *dlgFile = new wxFileDialog(this, "Save image", "", "", "DICOM Files(*.dcm;*.dicom)|*.dcm;*.dicom",  wxSAVE|wxOVERWRITE_PROMPT, wxDefaultPosition);
			if (dlgFile->ShowModal() == wxID_OK) {
				outputPath = dlgFile->GetPath();

				wxWindowDisabler disabler;
				wxBusyInfo wait("Please wait, saving...");
				wxBusyCursor cursor;

				anonymize.anonymizeImage(_inputPath, outputPath, aData);

				this->Raise();
			}
			dlgFile->Destroy();
			delete aData;
			this->Destroy();
		}
		else {
			wxDirDialog *dlgDir = new wxDirDialog(this, "Save series", "", wxSAVE|wxDD_NEW_DIR_BUTTON, wxDefaultPosition);
			if (dlgDir->ShowModal() == wxID_OK) {
				outputPath = dlgDir->GetPath();

				wxWindowDisabler disabler;
				wxBusyInfo wait("Please wait, saving ...");
				wxBusyCursor cursor;

				anonymize.anonymizeSeries(_inputPath, outputPath, aData);

				this->Raise();
			}
			dlgDir->Destroy();
			delete aData;
			this->Destroy();
		}
	}
	else {
		wxMessageDialog* messDialog = new wxMessageDialog(this, _T("_check one box at least."), _T("MITO"), wxOK | wxICON_INFORMATION);
		if (messDialog->ShowModal()==wxID_OK)
			messDialog->Destroy();
			delete aData;
	}
}