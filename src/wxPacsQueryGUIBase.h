///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wxPacsQueryGUIBase__
#define __wxPacsQueryGUIBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "treelistctrl.h"
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_LIST 1000
#define wxID_CBBIRTHDATE 1001
#define wxID_CBSTUDYDATE 1002
#define wxID_RADIOTODAY 1003
#define wxID_RADIOYESTERDAY 1004
#define wxID_RADIOLASTWEEK 1005
#define wxID_RADIOLASTMONTH 1006
#define wxID_RADIOBETWEEN 1007

///////////////////////////////////////////////////////////////////////////////
/// Class wxPacsQueryGUIBase
///////////////////////////////////////////////////////////////////////////////
class wxPacsQueryGUIBase : public wxFrame 
{
	private:
	
	protected:
		wxStaticText* m_staticText6;
		wxTreeListCtrl *m_pTreeList;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_txtPatName;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_txtPatID;
		wxStaticText* m_staticText3;
		wxDatePickerCtrl* m_datePatBirthdate;
		wxCheckBox* m_cbBirthdate;
		wxStaticText* m_staticText2;
		wxChoice* m_cbStudyModality;
		wxCheckBox* m_cbStudyDate;
		wxRadioButton* m_radioBtn1;
		wxRadioButton* m_radioBtn2;
		wxRadioButton* m_radioBtn3;
		wxRadioButton* m_radioBtn4;
		wxRadioButton* m_radioBtn5;
		wxDatePickerCtrl* m_dateStudyFrom;
		wxDatePickerCtrl* m_dateStudyTo;
		wxButton* m_buttonDownload;
		wxButton* m_buttonQuery;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnDownloadBtn( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnQueryBtn( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		wxPacsQueryGUIBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("PACS Query/Retrieve"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 780,602 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~wxPacsQueryGUIBase();
	
};

#endif //__wxPacsQueryGUIBase__
