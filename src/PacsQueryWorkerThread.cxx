/**
 * \file PacsQueryWorkerThread.cxx
 * \brief Thread che gestisce le query al pacs, usato da wxPacsQueryGui
 * \author ICAR-CNR Napoli
 */


#include "PacsQueryWorkerThread.h"
#include "wxPacsQueryGui.h"

#include <wx/busyinfo.h>

DEFINE_EVENT_TYPE(wxEVT_THREAD)

PacsQueryWorkerThread::PacsQueryWorkerThread(wxEvtHandler* pParent, wxTextCtrl *pOutBrowser, bool bDaemon)
:  wxThread(), m_pParent(pParent), m_pStudyBrowser(NULL), m_bDaemon(bDaemon), m_pStartNode(NULL)
{
	setOutput(pOutBrowser);
	//setOutput(NULL);	// Visto che siamo in ambiente multithread, evitiamo che interferisca
}

PacsQueryWorkerThread::~PacsQueryWorkerThread()
{
	m_pStartNode = NULL;
}

wxThread::ExitCode PacsQueryWorkerThread::Entry()
{
	/*
		La logica di questa classe è molto semplice:

		1) Se non abbiamo specificato m_pStudyBrowser ma c'è m_pStartNode, allora
		   vogliamo effettuare una richiesta C-MOVE
	    2) Se abbiamo specificato m_pStudyBrowser e m_pStartNode, allora stiamo
		   facendo una followup query (per trovare le serie a partire dagli studi)
	    3) Se Abbiamo specificato solo m_pStudyBrowser, vogliamo effettuare una
		   query a livello di studio.
	*/

	// Clear all browsers and lists
	if(!m_bDaemon) 
		clearAll();

	// m_pParent è in realtà la finestra chiamante, ovvero wxPacsQueryGui!
	wxPacsQueryGUI *pQueryGui = (wxPacsQueryGUI*)m_pParent;

	switch(m_eAction)
	{
	case E_PACS_CMOVESINGLE:
		{
			if(m_pStartNode)		
				send(NULL, m_pStartNode);
		}break;
	case E_PACS_CMOVEMULTI:
		{
			list<PacsData*>::iterator iteList = m_downloadQueue.begin();

			while (iteList != m_downloadQueue.end()) 
			{
				PacsData *pData = *iteList;
				if (pData) 
				{
					send(NULL, pData);
				}

				++iteList;
			}

			m_downloadQueue.clear();		

		}break;
	case E_PACS_FOLLOWUP:
		{
			if(!m_pStudyBrowser || !m_pStartNode)
				break;

			// Followup query
			queryFollowUp(m_pStartNode, m_pStartNode->qLev, pQueryGui->getLists());

			wxMutexGuiEnter();

			wxTreeItemId lvlUp = m_pStudyBrowser->GetSelection();
			if(lvlUp.IsOk())
			{
				m_pStudyBrowser->DeleteChildren(lvlUp);

				pQueryGui->getList(1)->display(m_pStudyBrowser, SERIES);

				// Activate send menuitem and etc..
				//pQueryGui->Raise();
			}

			wxMutexGuiLeave();			
		}break;
	case E_PACS_STUDYQUERY:
		{
			if(!m_pStudyBrowser || !m_pStartNode)
				break;

			// Make normal study query
			if(!query((StudyData*)m_pStartNode, pQueryGui->getLists(), true))
			{
				// Errore query.. notifichiamo l'interfaccia grafica
				wxCommandEvent evt(wxEVT_THREAD, wxID_ANY);
				evt.SetInt(EVT_MITO_QUERY_ERROR);
				m_pParent->AddPendingEvent(evt);		

				break;
			}
			wxMutexGuiEnter();

			// Display results
			pQueryGui->getList(0)->display(m_pStudyBrowser, STUDY);

			// Activate send menuitem and etc..
			//pQueryGui->Raise();

			wxMutexGuiLeave();
		}break;
	default:
		break;
	}

	// thread ends: notify parent
	wxCommandEvent evt(wxEVT_THREAD, wxID_ANY);
	evt.SetInt(EVT_MITO_QUERY_TERMINATA);	// ID dell'errore (utilizziamolo per comunicare qualcosa al main thread!)
	m_pParent->AddPendingEvent(evt);

	return EXIT_SUCCESS;
}
