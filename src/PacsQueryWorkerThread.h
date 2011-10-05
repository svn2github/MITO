/**
 * \file PacsQueryWorkerThread.h
 * \brief Header classe worker thread che gestisce le query al pacs, usato da wxPacsQueryGui
 * \author ICAR-CNR Napoli
 */

#ifndef _PacsQueryWorkerThread_h_
#define _PacsQueryWorkerThread_h_

#include "pacsCommunicator.h"

#include <wx/thread.h>
#include <list>

DECLARE_EVENT_TYPE(wxEVT_THREAD, -1)

enum E_PACS_WORKERQUERY_ACTION  { E_PACS_CMOVESINGLE, E_PACS_CMOVEMULTI, E_PACS_FOLLOWUP, E_PACS_STUDYQUERY };

// Forward declarations
class wxTreeListCtrl;

class PacsQueryWorkerThread : public wxThread, public pacsCommunicator
{
public:
	PacsQueryWorkerThread(wxEvtHandler* , wxTextCtrl *, bool bDaemon = false);
	~PacsQueryWorkerThread();

	inline void setAction(E_PACS_WORKERQUERY_ACTION eAction) {
		m_eAction = eAction;
	}

	inline void setStudyBrowser(wxTreeListCtrl *pStudyBrowser) {
		m_pStudyBrowser = pStudyBrowser;
	}

	inline void setStartQueryNode(PacsData *pNode) {
		m_pStartNode = pNode;
	}

	inline void setDownloadQueue(list<PacsData*> &downloadQueue) {
		m_downloadQueue = downloadQueue;
	}

protected:
	E_PACS_WORKERQUERY_ACTION	m_eAction;
	wxEvtHandler		*m_pParent;
	wxTreeListCtrl		*m_pStudyBrowser; 
	list<PacsData*>		m_downloadQueue;
	bool				m_bDaemon;

	PacsData			*m_pStartNode;

	wxThread::ExitCode	Entry();
};

#endif // _PacsQueryWorkerThread_h_