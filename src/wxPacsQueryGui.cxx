/**
 * \file wxPacsQueryGUI.cpp
 * \brief Interfaccia grafica per effettuare le query al PACS e mostrare i risultati
 * \author ICAR-CNR Napoli
 */

#include "PacsQueryWorkerThread.h"
#include "pacsStoreSCP.h"	// Rimuovere quando il server store viene spostato 
#include "wxPacsQueryGUI.h"
#include "wxQueryGuiStatusBar.h"

#include "wxMainGui.h"

#define MITO_UPDATE_TIMER_FREQ	100	// Millisecondi
#define EVT_MITO_QUERY_UPDATE_STATUSBAR		667 //(EVT_MITO_QUERY_TERMINATA + 1)

// Implementiamo l'evento per comunicare col thread principale
DEFINE_EVENT_TYPE( wxEVT_QUERYGUI_NOTIFICATION )

BEGIN_EVENT_TABLE( wxPacsQueryGUI, wxPacsQueryGUIBase )

	// Messaggio inviato dal thread per la query al PACS per indicarne la fine
	EVT_COMMAND(wxID_ANY, wxEVT_THREAD, wxPacsQueryGUI::OnThreadExit)

	EVT_TREE_ITEM_ACTIVATED(wxID_LIST, wxPacsQueryGUI::OnListItemActivated)
	EVT_TREE_ITEM_EXPANDING(wxID_LIST, wxPacsQueryGUI::OnListItemExpanding)
	EVT_TREE_ITEM_GETTOOLTIP(wxID_LIST,	wxPacsQueryGUI::OnGetTooltip)

	// A causa di un bug delle wxWidgets su windows, dobbiamo implementare
	// la mutua esclusione dei radio button da soli..
	// http://docs.wxwidgets.org/2.8.6/wx_wxradiobutton.html
	EVT_RADIOBUTTON(wxID_RADIOTODAY, wxPacsQueryGUI::OnRadioTodayClick)
	EVT_RADIOBUTTON(wxID_RADIOYESTERDAY, wxPacsQueryGUI::OnRadioYesterdayClick)
	EVT_RADIOBUTTON(wxID_RADIOLASTWEEK, wxPacsQueryGUI::OnRadioLastWeekClick)
	EVT_RADIOBUTTON(wxID_RADIOLASTMONTH, wxPacsQueryGUI::OnRadioLastMonthClick)
	EVT_RADIOBUTTON(wxID_RADIOBETWEEN, wxPacsQueryGUI::OnRadioBetweenClick)

	EVT_CHECKBOX(wxID_CBBIRTHDATE, wxPacsQueryGUI::OnCbBirthdate)
	EVT_CHECKBOX(wxID_CBSTUDYDATE, wxPacsQueryGUI::OnCbStudyDate)

	EVT_CHAR_HOOK(wxPacsQueryGUI::OnKeyDown)
	EVT_TIMER(EVT_MITO_QUERY_UPDATE_STATUSBAR, wxPacsQueryGUI::OnTimer)
END_EVENT_TABLE()

wxPacsQueryGUI::wxPacsQueryGUI( wxWindow* parent )
:
wxPacsQueryGUIBase( parent ), m_pStatusTimer(0)
{
   	SetIcon(wxICON(MITO));

	m_pQueryWorkerThread = NULL;

	// Creiamo la status bar..
	_nImmaginiAttualmenteAttese = 0;
	CreateStatusBar();
	
	// Impostiamo il tree list
    m_pTreeList->AddColumn (_T("Name"), 100, wxALIGN_LEFT);
    m_pTreeList->AddColumn (_T("Birthdate"), 100, wxALIGN_LEFT);
    m_pTreeList->AddColumn (_T("Study ID"), 100, wxALIGN_LEFT);
    m_pTreeList->AddColumn (_T("Study Date"), 100, wxALIGN_LEFT);
    m_pTreeList->AddColumn (_T("Modality"), 100, wxALIGN_LEFT);
	m_pTreeList->AddColumn (_T("Images"), 100, wxALIGN_LEFT);

	// Impostiamo il modality combo
	m_cbStudyModality->Append (_T("All"));
	m_cbStudyModality->Append (_T("CT"));
	m_cbStudyModality->Append (_T("MR"));
	m_cbStudyModality->Append (_T("XA"));
	m_cbStudyModality->Append (_T("US"));
	m_cbStudyModality->Append (_T("CR"));
	m_cbStudyModality->Append (_T("NM"));
	m_cbStudyModality->Append (_T("RF"));
	m_cbStudyModality->Append (_T("OT"));
	m_cbStudyModality->Append (_T("DX"));
	m_cbStudyModality->Append (_T("MG"));
	m_cbStudyModality->Append (_T("PT"));
	m_cbStudyModality->Append (_T("SC"));
	m_cbStudyModality->SetSelection(0);		// Tutti

    // Initialize our lists
    for (int i = 0; i < 2; i++)
        m_Lists[i] = new PacsList;


	m_pStatusTimer = new wxTimer(this, EVT_MITO_QUERY_UPDATE_STATUSBAR);

	// Impostiamo il focus sul controllo patient name
	m_txtPatName->SetFocus();

}

wxPacsQueryGUI::~wxPacsQueryGUI()
{
	// Notifichiamo il thread principale che stiamo chiudendo la GUI
	wxCommandEvent evt1(wxEVT_QUERYGUI_NOTIFICATION, wxID_ANY);
	//evt1.SetInt(EVT_MITO_CLOSING_QUERYGUI);
	GetParent()->AddPendingEvent(evt1);


	// Eliminiamo le liste
	for (int i = 0; i < 2; i++) {
		m_Lists[i]->head();
		delete m_Lists[i];
	}

	// Nel caso m_pQueryWorker sia un valido puntatore
	// vuol dire che il thread della query ancora non ha finito, quindi attendiamo
	if(m_pQueryWorkerThread)
		m_pQueryWorkerThread->Delete();

	m_pQueryWorkerThread = NULL;

	if(m_pStatusTimer)
		delete m_pStatusTimer;

	m_pStatusTimer = NULL;
}

void wxPacsQueryGUI::OnQueryBtn( wxCommandEvent& event )
{
	if(m_pQueryWorkerThread)
	{
		// Arg c'è un problema, c'è già un thread allocato per fare la query!
		wxMessageBox("You are already performing a query.", "MITO Error", wxICON_EXCLAMATION);
		return;
	}

	// Evitiamo che l'utente faccia query troppo generiche
	// che risulterebbero nell'"insamamento" del PACS!
	if((m_txtPatName->GetValue() == wxT("%")) && !m_cbStudyDate->GetValue())
	{
		wxMessageBox("Your query it's too generic and may crash the PACS server. Please specify at least a data parameter and try again.", "MITO Error", wxICON_EXCLAMATION);
		return;
	}

	// Costruiamo un oggetto StudyData contenente le informazioni della query
	StudyData *pToQuery = new StudyData();
	
	strcpy(pToQuery->patName, m_txtPatName->GetValue());
	strcpy(pToQuery->patID, m_txtPatID->GetValue());

	if(m_cbBirthdate->GetValue())
		strcpy(pToQuery->patBirthdate, m_datePatBirthdate->GetValue().Format(_T("%Y%m%d")));

	wxString szModality = m_cbStudyModality->GetStringSelection();
	if(szModality != _T("All"))
		strcpy(pToQuery->modality, szModality);

	if(m_cbStudyDate->GetValue())
	{
		wxString szStudyDate;
		if(m_radioBtn1->GetValue())		// Today
		{
			wxDateTime today = wxDateTime::Today();

			szStudyDate = today.Format(_T("%Y%m%d"));
		}else if(m_radioBtn2->GetValue())	// Yesterday
		{
			wxDateTime yesterday = wxDateTime::Now();
			yesterday.SetDay(yesterday.GetDay() - 1);	// Scaliamo un giorno :)

			szStudyDate = yesterday.Format(_T("%Y%m%d"));
		}else if(m_radioBtn3->GetValue())	// Last Week
		{
			// Calcoliamo la data dell'ultimo sabato e, da quella, quella del lunedì precedente
			wxDateTime lastWeekEnd = wxDateTime::Now();
			lastWeekEnd.SetToPrevWeekDay(wxDateTime::WeekDay::Sun);

			wxDateTime lastWeekStart = lastWeekEnd;
			lastWeekStart.SetToWeekDayInSameWeek(wxDateTime::WeekDay::Mon);

			szStudyDate = lastWeekStart.Format(_T("%Y%m%d"));
			szStudyDate += _T("-");
			szStudyDate += lastWeekEnd.Format(_T("%Y%m%d"));

		}else if(m_radioBtn4->GetValue())	// Last Month
		{
			// Vediamo in che mese siamo e procediamo a ritroso
			wxDateTime::Month eMonth = wxDateTime::Now().GetMonth();
			int iYear = wxDateTime::Now().GetYear();

			if(eMonth == wxDateTime::Month::Jan)
			{
				eMonth = wxDateTime::Month::Dec;
				iYear = iYear - 1;
			}else
				eMonth = (wxDateTime::Month)(eMonth - 1);	// NON molto carino in questo modo..

			wxDateTime lastMonthDayEnd = wxDateTime::Now();
			lastMonthDayEnd.SetToLastMonthDay(eMonth, iYear);

			wxDateTime lastMonthDayStart = lastMonthDayEnd;
			lastMonthDayStart.SetDay(1);

			szStudyDate = lastMonthDayStart.Format(_T("%Y%m%d"));
			szStudyDate += _T("-");
			szStudyDate += lastMonthDayEnd.Format(_T("%Y%m%d"));

		}else if(m_radioBtn5->GetValue())	// Between
		{
			szStudyDate = m_dateStudyFrom->GetValue().Format(_T("%Y%m%d"));
			szStudyDate += _T("-");
			szStudyDate += m_dateStudyTo->GetValue().Format(_T("%Y%m%d"));
		}

		strcpy(pToQuery->date, szStudyDate);
	}

	// Prendiamo la configurazione da wxMainGui
	pacsPreferences *pPrefs = ((wxMainGui*)GetParent())->getPrefs();

	// Interroghiamo il PACS in un altro thread
	m_pQueryWorkerThread = new PacsQueryWorkerThread(this, NULL, false);
	m_pQueryWorkerThread->setPrefs(pPrefs);
	if(wxTHREAD_NO_ERROR == m_pQueryWorkerThread->Create())
	{
		// Disabilitiamo il pulsante Query finchè non è terminata
		m_buttonQuery->Disable();
		m_buttonDownload->Disable();
		m_pTreeList->DeleteRoot();
		m_pTreeList->Disable();
		m_Lists[0]->clear();
		m_Lists[1]->clear();

		m_pQueryWorkerThread->setAction(E_PACS_STUDYQUERY);
		m_pQueryWorkerThread->setStartQueryNode(pToQuery);
		m_pQueryWorkerThread->setStudyBrowser(m_pTreeList);

		m_pQueryWorkerThread->Run();

		m_pStatusTimer->Start(MITO_UPDATE_TIMER_FREQ, wxTIMER_CONTINUOUS);
	}
}

void wxPacsQueryGUI::OnDownloadBtn( wxCommandEvent& event )
{
	string sStudyUID;
	string sSeriesUID;
	wxTreeItemId albumEntry;
	wxArrayTreeItemIds selectedItems;

	wxMainGui *pMain = (wxMainGui*)GetParent();
	if(!pMain)
		return;	// ARGH!

	ssize_t numSelectedItems = m_pTreeList->GetSelections(selectedItems);
	if(!numSelectedItems)
		return;

	// Notifichiamo la finestra principale che stiamo per scaricare
	wxCommandEvent evt1(wxEVT_QUERYGUI_NOTIFICATION, wxID_ANY);
	evt1.SetInt(EVT_MITO_RECEIVE_START);
	GetParent()->AddPendingEvent(evt1);

	list<PacsData*> downloadQueue;

	for(ssize_t k = 0; k < numSelectedItems; k++)
	{
		wxTreeItemId curSelItem = selectedItems[k];
		if(!curSelItem.IsOk())
			continue;	// Saltiamo eventuali elementi corrotti..

		PacsData *pToDownload = (PacsData*)m_pTreeList->GetItemDataPtr(curSelItem);
		if(!pToDownload)
			continue;	// Saltiamo..

		// Verifichiamo che non sia già nell'album.. questo codice è identico a quello
		// riproposto in qualche funzione più sotto! TODO: Mettiamolo in qualche funzione
		// separata?
		switch(pToDownload->qLev)
		{
		case STUDY:
			{
				StudyData *pStudy = (StudyData*)pToDownload;
				sStudyUID = pStudy->instanceUID;
				sSeriesUID = "";

				_nImmaginiAttualmenteAttese = atoi(pStudy->numImages);
			}
			break;

		case SERIES:
			{
				SeriesData *pSeries = (SeriesData*)pToDownload;
				sStudyUID = pSeries->studyInstanceUID;
				sSeriesUID = pSeries->instanceUID;

				_nImmaginiAttualmenteAttese = atoi(pSeries->imagesNumber);
			}
			break;
		}

		bool bRemoveCache = false;
		int iRetCache = pMain->searchRecord(sStudyUID, sSeriesUID, albumEntry);
		switch(iRetCache)
		{
		case 2:	// E' stata trovata la serie! Eliminiamola dalla cache
			{
				bRemoveCache = true;
			}break;
		case 1:
			{
				if(!sSeriesUID.size())
				{
					bRemoveCache = true;
				}
			}break;
		}

		if(bRemoveCache)
		{
			// La serie/studio è già presente in cache!
			int iRes = wxMessageBox("The study you are trying to download is already in your local cache. Do you want to overwrite it?", "Warning", wxYES_NO | wxICON_EXCLAMATION );
			if(iRes == wxNO)
				return;

			// wxYES: cancelliamo la copia locale ed eliminiamola anche dall'album
			pMain->deleteAlbumEntry(albumEntry, true);
		}

		downloadQueue.push_back(pToDownload);
	}

	// E ora che abbiamo una lista di cose da scaricare.. scarichiamole!

	// Abbiamo richiesto il download dell'elemento..
	// Interroghiamo il PACS in un altro thread
	m_pQueryWorkerThread = new PacsQueryWorkerThread(this, NULL, false);
	m_pQueryWorkerThread->setPrefs(pMain->getPrefs());
	if(wxTHREAD_NO_ERROR == m_pQueryWorkerThread->Create())
	{
		// Disabilitiamo il pulsante Query finchè non è terminata
		m_buttonQuery->Disable();
		m_buttonDownload->Disable();
		m_pTreeList->Disable();

		wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
		if(pStatusBar)
			pStatusBar->Restart();

		m_pQueryWorkerThread->setAction(E_PACS_CMOVEMULTI);
		m_pQueryWorkerThread->setDownloadQueue(downloadQueue);

		m_pQueryWorkerThread->Run();

		m_pStatusTimer->Start(MITO_UPDATE_TIMER_FREQ, wxTIMER_CONTINUOUS);
	}
}

void wxPacsQueryGUI::OnThreadExit( wxCommandEvent & event )
{
	int iEvent = event.GetInt();

	switch(iEvent)
	{
	case EVT_MITO_QUERY_TERMINATA:
		{
			// Il thread ci segnala di essere terminato..
			m_pQueryWorkerThread = NULL;

			m_pTreeList->Enable();
			m_buttonQuery->Enable();
			m_buttonDownload->Enable();

			wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
			if(pStatusBar)
				pStatusBar->Stop();

			m_pStatusTimer->Stop();
		}break;
	case EVT_MITO_QUERY_ERROR:
		{
			// Il thread ci segnala di essere terminato..
			m_pQueryWorkerThread = NULL;

			m_pTreeList->Enable();
			m_buttonQuery->Enable();
			m_buttonDownload->Enable();

			wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
			if(pStatusBar)
				pStatusBar->Stop();

			m_pStatusTimer->Stop();

			wxMessageBox(wxT("MITO was unable to contact the specified PACS server. Please check your network connection or contact your PACS administrator for further informations."), wxT("Error"), wxICON_ERROR);
		}

	}
}

void wxPacsQueryGUI::OnGetTooltip ( wxTreeEvent& evt )
{
	wxTreeItemId item = evt.GetItem();
	if(!item.IsOk())
		return;

	evt.SetToolTip(_T("Test"));
	evt.Skip();
}

void wxPacsQueryGUI::OnListItemActivated ( wxTreeEvent& evt )
{
	wxTreeItemId item = evt.GetItem();
	if(!item.IsOk())
		return;

	PacsData *pToDownload = (PacsData*)m_pTreeList->GetItemDataPtr(item);
	if(!pToDownload)
		return;

	// Evitiamo di "espandere" questo elemento, vogliamo solo "scaricarlo" :)
	evt.Veto();		

	// Verifichiamo che ciò che vogliamo scaricare, non sia già nell'album!
	wxMainGui *pMain = (wxMainGui*)GetParent();
	if(!pMain)
		return;	// ARGH!

	wxTreeItemId albumEntry;

	string sStudyUID;
	string sSeriesUID;

	switch(pToDownload->qLev)
	{
	case STUDY:
		{
			StudyData *pStudy = (StudyData*)pToDownload;
			sStudyUID = pStudy->instanceUID;
			sSeriesUID = "";
		}
		break;

	case SERIES:
		{
			SeriesData *pSeries = (SeriesData*)pToDownload;
			sStudyUID = pSeries->studyInstanceUID;
			sSeriesUID = pSeries->instanceUID;
		}
		break;
	}

	bool bRemoveCache = false;
	int iRetCache = pMain->searchRecord(sStudyUID, sSeriesUID, albumEntry);
	switch(iRetCache)
	{
	case 2:	// E' stata trovata la serie! Eliminiamola dalla cache
		{
			bRemoveCache = true;
		}break;
	case 1:
		{
			if(!sSeriesUID.size())
			{
				bRemoveCache = true;
			}
		}break;
	}

	if(bRemoveCache)
	{
		// La serie/studio è già presente in cache!
		int iRes = wxMessageBox("The study you are trying to download is already in your local cache. Do you want to overwrite it?", "Warning", wxYES_NO | wxICON_EXCLAMATION );
		if(iRes == wxNO)
			return;

		// wxYES: cancelliamo la copia locale ed eliminiamola anche dall'album
		pMain->deleteAlbumEntry(albumEntry, true);
	}

	// Notifichiamo la finestra principale che stiamo per scaricare
	wxCommandEvent evt1(wxEVT_QUERYGUI_NOTIFICATION, wxID_ANY);
	evt1.SetInt(EVT_MITO_RECEIVE_START);
	GetParent()->AddPendingEvent(evt1);

	// Abbiamo richiesto il download dell'elemento..
	// Interroghiamo il PACS in un altro thread
	m_pQueryWorkerThread = new PacsQueryWorkerThread(this, NULL, false);
	m_pQueryWorkerThread->setPrefs(pMain->getPrefs());
	if(wxTHREAD_NO_ERROR == m_pQueryWorkerThread->Create())
	{
		// Disabilitiamo il pulsante Query finchè non è terminata
		m_buttonQuery->Disable();
		m_buttonDownload->Disable();
		m_pTreeList->Disable();

		wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
		if(pStatusBar)
			pStatusBar->Restart();

		m_pQueryWorkerThread->setAction(E_PACS_CMOVESINGLE);
		m_pQueryWorkerThread->setStartQueryNode(pToDownload);

		m_pQueryWorkerThread->Run();

		m_pStatusTimer->Start(MITO_UPDATE_TIMER_FREQ, wxTIMER_CONTINUOUS);
	}
}

void wxPacsQueryGUI::OnListItemExpanding ( wxTreeEvent& evt )
{
	wxTreeItemId item = evt.GetItem();
	if(!item.IsOk())
		return;

	evt.Skip();

	// Assicuriamoci di non aver già effettuato query per questo record
	wxTreeItemIdValue cookie;
	wxTreeItemId subItem = m_pTreeList->GetFirstChild(item, cookie);
	wxString subItemText = m_pTreeList->GetItemText(subItem, 0);
	if(subItemText != _T("Querying PACS server for series..."))
	{
		// Se abbiamo già effettuato una query per le serie, è inutile rifarla..
		return;
	}

	// Selezioniamo lo studio
	m_pTreeList->SelectItem(item);

	// Prendiamo la struttura dati associata all'oggetto
	StudyData *pStudy = (StudyData*)m_pTreeList->GetItemDataPtr(item);
	
	// Prendiamo la configurazione da wxMainGui
	pacsPreferences *pPrefs = ((wxMainGui*)GetParent())->getPrefs();

	// Interroghiamo il PACS in un altro thread
	m_pQueryWorkerThread = new PacsQueryWorkerThread(this, NULL, false);
	m_pQueryWorkerThread->setPrefs(pPrefs);
	if(wxTHREAD_NO_ERROR == m_pQueryWorkerThread->Create())
	{
		// Disabilitiamo il pulsante Query finchè non è terminata
		m_buttonQuery->Disable();
		m_buttonDownload->Disable();
		m_pTreeList->Disable();

		wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
		if(pStatusBar)
			pStatusBar->Restart();

		m_pQueryWorkerThread->setAction(E_PACS_FOLLOWUP);
		m_pQueryWorkerThread->setStudyBrowser(m_pTreeList);
		m_pQueryWorkerThread->setStartQueryNode(pStudy);

		m_pQueryWorkerThread->Run();

		m_pStatusTimer->Start(MITO_UPDATE_TIMER_FREQ, wxTIMER_CONTINUOUS);

	}
}

void wxPacsQueryGUI::OnRadioTodayClick(wxCommandEvent& evt)
{
	m_radioBtn2->SetValue(FALSE);
	m_radioBtn3->SetValue(FALSE);
	m_radioBtn4->SetValue(FALSE);
	m_radioBtn5->SetValue(FALSE);

	m_dateStudyFrom->Enable(FALSE);
	m_dateStudyTo->Enable(FALSE);

	evt.Skip();
}

void wxPacsQueryGUI::OnRadioYesterdayClick(wxCommandEvent& evt)
{
	m_radioBtn1->SetValue(FALSE);
	m_radioBtn3->SetValue(FALSE);
	m_radioBtn4->SetValue(FALSE);
	m_radioBtn5->SetValue(FALSE);

	m_dateStudyFrom->Enable(FALSE);
	m_dateStudyTo->Enable(FALSE);

	evt.Skip();
}

void wxPacsQueryGUI::OnRadioLastWeekClick(wxCommandEvent& evt)
{
	m_radioBtn1->SetValue(FALSE);
	m_radioBtn2->SetValue(FALSE);
	m_radioBtn4->SetValue(FALSE);
	m_radioBtn5->SetValue(FALSE);

	m_dateStudyFrom->Enable(FALSE);
	m_dateStudyTo->Enable(FALSE);

	evt.Skip();
}

void wxPacsQueryGUI::OnRadioLastMonthClick(wxCommandEvent& evt)
{
	m_radioBtn1->SetValue(FALSE);
	m_radioBtn2->SetValue(FALSE);
	m_radioBtn3->SetValue(FALSE);
	m_radioBtn5->SetValue(FALSE);

	m_dateStudyFrom->Enable(FALSE);
	m_dateStudyTo->Enable(FALSE);

	evt.Skip();
}

void wxPacsQueryGUI::OnRadioBetweenClick(wxCommandEvent& evt)
{
	m_radioBtn1->SetValue(FALSE);
	m_radioBtn2->SetValue(FALSE);
	m_radioBtn3->SetValue(FALSE);
	m_radioBtn4->SetValue(FALSE);

	m_dateStudyFrom->Enable(TRUE);
	m_dateStudyTo->Enable(TRUE);

	evt.Skip();
}

void wxPacsQueryGUI::OnCbBirthdate(wxCommandEvent& evt)
{
	m_datePatBirthdate->Enable(m_cbBirthdate->GetValue());

	evt.Skip();
}

void wxPacsQueryGUI::OnCbStudyDate(wxCommandEvent& evt)
{
	bool bEnable = m_cbStudyDate->GetValue();

	m_radioBtn1->Enable(bEnable);
	m_radioBtn2->Enable(bEnable);
	m_radioBtn3->Enable(bEnable);
	m_radioBtn4->Enable(bEnable);	
	m_radioBtn5->Enable(bEnable);	

	m_dateStudyFrom->Enable(FALSE);
	m_dateStudyTo->Enable(FALSE);

	evt.Skip();
}

void wxPacsQueryGUI::OnKeyDown(wxKeyEvent& event)
{
	// Se abbiamo premuto il tasto invio, inviamo la query
	if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER)
	{
		wxCommandEvent evtPlaceholder;
		OnQueryBtn(evtPlaceholder);
	}else
		event.Skip();
}

wxStatusBar *wxPacsQueryGUI::OnCreateStatusBar(int number, long style, wxWindowID id,  const wxString& name)
{
   wxStatusBar *status_bar = new wxQueryGuiStatusBar(this, id, style, name);
   status_bar->SetFieldsCount(number);
   return status_bar;
}

void wxPacsQueryGUI::OnTimer(wxTimerEvent &event)
{
	if(m_pQueryWorkerThread)
	{
		// Se il thread è ancora in esecuzione, allora stiamo lavorando
		// a qualcosa.. aggiorniamo la progress bar!
		//wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
		//if(pStatusBar)						
			//pStatusBar->PulseProgress();
	}
}	

void wxPacsQueryGUI::onMitoReceivedImage() {
	wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
	if(pStatusBar) {
		int value = pStatusBar->GetValue();
		if (value < _nImmaginiAttualmenteAttese) 
			pStatusBar->SetProgress(value+1,_nImmaginiAttualmenteAttese);
	}
}

void wxPacsQueryGUI::onMitoReceivedSeries() {
	wxQueryGuiStatusBar *pStatusBar = (wxQueryGuiStatusBar*)GetStatusBar();
	if(pStatusBar)
			pStatusBar->SetProgress(0);
}