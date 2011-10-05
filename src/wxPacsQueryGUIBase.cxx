///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxPacsQueryGUIBase.h"

///////////////////////////////////////////////////////////////////////////

wxPacsQueryGUIBase::wxPacsQueryGUIBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 780,-1 ), wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Double-click on a record  to start the download or use CTRL+LMB for multiple selection"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer2->Add( m_staticText6, 0, wxALL, 5 );
	
	m_pTreeList = new wxTreeListCtrl(this, wxID_LIST, wxDefaultPosition, wxDefaultSize, wxTR_MULTIPLE|wxTR_HIDE_ROOT|wxTR_ROW_LINES| wxTR_HAS_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT );
	m_pTreeList->SetMinSize( wxSize( 600,-1 ) );
	
	bSizer2->Add( m_pTreeList, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer2->Add( bSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	bSizer1->SetMinSize( wxSize( 160,-1 ) ); 
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("General") ), wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Patient Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	sbSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_txtPatName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_txtPatName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Patient ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	sbSizer2->Add( m_staticText4, 0, wxALL, 5 );
	
	m_txtPatID = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_txtPatID, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Birthdate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sbSizer2->Add( m_staticText3, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer21->AddGrowableCol( 1 );
	fgSizer21->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_datePatBirthdate = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT|wxDP_DROPDOWN );
	m_datePatBirthdate->Enable( false );
	
	fgSizer21->Add( m_datePatBirthdate, 0, wxALL|wxEXPAND, 5 );
	
	m_cbBirthdate = new wxCheckBox( this, wxID_CBBIRTHDATE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer21->Add( m_cbBirthdate, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer2->Add( fgSizer21, 1, wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Additional Parameters") ), wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Modality"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	sbSizer3->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );
	
	wxArrayString m_cbStudyModalityChoices;
	m_cbStudyModality = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_cbStudyModalityChoices, 0 );
	m_cbStudyModality->SetSelection( 0 );
	sbSizer3->Add( m_cbStudyModality, 0, wxALL|wxEXPAND, 5 );
	
	m_cbStudyDate = new wxCheckBox( this, wxID_CBSTUDYDATE, wxT("Study Date"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer3->Add( m_cbStudyDate, 0, wxALL, 5 );
	
	m_radioBtn1 = new wxRadioButton( this, wxID_RADIOTODAY, wxT("Today"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_radioBtn1->Enable( false );
	
	sbSizer3->Add( m_radioBtn1, 0, wxALL, 5 );
	
	m_radioBtn2 = new wxRadioButton( this, wxID_RADIOYESTERDAY, wxT("Yesterday"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_radioBtn2->Enable( false );
	
	sbSizer3->Add( m_radioBtn2, 0, wxALL, 5 );
	
	m_radioBtn3 = new wxRadioButton( this, wxID_RADIOLASTWEEK, wxT("Last Week"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_radioBtn3->Enable( false );
	
	sbSizer3->Add( m_radioBtn3, 0, wxALL, 5 );
	
	m_radioBtn4 = new wxRadioButton( this, wxID_RADIOLASTMONTH, wxT("Last Month"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_radioBtn4->Enable( false );
	
	sbSizer3->Add( m_radioBtn4, 0, wxALL, 5 );
	
	m_radioBtn5 = new wxRadioButton( this, wxID_RADIOBETWEEN, wxT("Between:"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
	m_radioBtn5->Enable( false );
	
	sbSizer3->Add( m_radioBtn5, 0, wxALL, 5 );
	
	m_dateStudyFrom = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT|wxDP_DROPDOWN );
	m_dateStudyFrom->Enable( false );
	
	sbSizer3->Add( m_dateStudyFrom, 0, wxALL|wxEXPAND, 5 );
	
	m_dateStudyTo = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT|wxDP_DROPDOWN );
	m_dateStudyTo->Enable( false );
	
	sbSizer3->Add( m_dateStudyTo, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer3, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
	
	m_buttonDownload = new wxButton( this, wxID_ANY, wxT("Download"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_buttonDownload, 0, wxALIGN_LEFT, 5 );
	
	m_buttonQuery = new wxButton( this, wxID_ANY, wxT("Query"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_buttonQuery, 0, wxALIGN_RIGHT, 5 );
	
	bSizer1->Add( sbSizer4, 0, wxALIGN_BOTTOM|wxEXPAND|wxSHAPED, 5 );
	
	fgSizer2->Add( bSizer1, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();
	
	// Connect Events
	m_buttonDownload->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxPacsQueryGUIBase::OnDownloadBtn ), NULL, this );
	m_buttonQuery->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxPacsQueryGUIBase::OnQueryBtn ), NULL, this );
}

wxPacsQueryGUIBase::~wxPacsQueryGUIBase()
{
	// Disconnect Events
	m_buttonDownload->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxPacsQueryGUIBase::OnDownloadBtn ), NULL, this );
	m_buttonQuery->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxPacsQueryGUIBase::OnQueryBtn ), NULL, this );
}
