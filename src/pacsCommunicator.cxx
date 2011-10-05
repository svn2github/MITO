/**
 * \file pacsCommunicator.cxx
 * \brief File per la comunicazione con i PACS
 * \author ICAR-CNR Napoli
 */

//#include "vld.h"

#include "pacsCommunicator.h"
#include "pacsStoreSCP.h"
#include "pacsStoreSCU.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include <iostream>
#include <fstream>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <wx/utils.h>
#include <wx/busyinfo.h>

#include <wx/timer.h>


using namespace std;

//
// class pacsCommunicator
//
// pacsCommunicator ()
//  Default constructor.  Takes a pointer to pacsCommunicator Prefs.
//
pacsCommunicator::pacsCommunicator ()
{
    // Load preferences and initialize query module
	char* filename = PacsCFG;
	strcpy(prefsName,filename);
    prefs = new pacsPreferences(prefsName);
    qMod  = new pacsQuery(prefs);
    output = NULL;
}

//
// ~pacsCommunicator ()
//  Destructor, saves prefs if prefs->autoSave is set.
//
pacsCommunicator::~pacsCommunicator ()
{
    // Save prefs?
    if (prefs->autoSave)
        prefs->write(prefsName);

    delete prefs;
    delete qMod;

}

//
// queryFollowUp (_browser, _qLev)
//  Queries each selected item in _browser and clears forthcoming lists
//  If a data object contains a non-NULL FileList structure, it assumes
//  uses that instead of constructing a query.
//
bool pacsCommunicator::queryFollowUp (PacsData *pStart, const QueryLevel _qLev, PacsList ** lists)
{
	// Non ci conviene fare il clear della lista, altrimenti i record del treelist 
	// conterranno puntatori non validi..
    //lists[_qLev]->clear();	

	return (qMod->makeFollowupQuery(pStart, lists[_qLev])) ? true : false;
}

// 
// query (_name, _id)
//  Queries the server for given information.  Stores data in lists[0]
//
bool pacsCommunicator::query (const char *_name, const char *_patientBirthdate, const char *_studyDate, PacsList ** lists)
{
    char tmp[255];
    sprintf(tmp, "\nQuerying %s...", _name);
	writeOutput(tmp);
    if(!qMod->makeQuery(_name, _patientBirthdate, _studyDate, lists[0]))
		return false;

    writeOutput("\nQuery complete.");

	return true;
}

bool pacsCommunicator::query(StudyData *queryData, PacsList **lists, bool bDeleteStudyData)
{
	// Utilizziamo questo metodo se abbiamo già un oggetto di tipo PacsData
	// da cui attingere le chiavi per la query
	if(!queryData || !lists)
		return false;

	qMod->buildQuery(queryData);

	if(!qMod->sendQuery(lists[0]))
	{
		// TODO: Report error
		return false;
	}

	if(bDeleteStudyData)
	{
		delete queryData;	// Attenzione! Il puntatore a queryData diventa inutilizzabile..
	}

	return true;
}

//
// query (_dir)
//  Queries directory for patient information, stores in lists[0]
//
bool pacsCommunicator::query (const char *_dir, PacsList ** lists)
{
    char tmp[255];

    sprintf(tmp, "\nQuerying %s...", _dir);
    writeOutput(tmp);
    if(!qMod->makeQuery(_dir, lists[0]))
		return false;

    writeOutput("\nQuery complete.");
	
	return true;
}

//
// send (_server, _browser)
//  Place a move request for every selected object in _browser.
//  Send objects to _server.
//***********************
int pacsCommunicator::send (const char *_server, PacsData *pToMove)
{
	// Invia una richiesta di move (test)
    qMod->buildQuery(pToMove, true);

	return qMod->sendMoveRequest(_server);
}



//
// clearAll ()
//  Clear all lists in lists[]
//
void pacsCommunicator::clearAll ()
{

}

//
// setOutput (_browser)
//  Sets up the output browser, used for display of transfer status, etc
//
void pacsCommunicator::setOutput (wxTextCtrl *_output)
{
    output = _output;
}

//
// spawnStoreSCP ()
//  Start the storage server in a separate thread
//
pacsStoreSCP *pacsCommunicator::spawnStoreSCP(wxEvtHandler *pParent)
{
	writeOutput("\nAvvio dello STORE server in un thread separato...");

	pacsStoreSCP *pServer = new pacsStoreSCP(prefs, pParent);
	if(pServer)
	{
		if(pServer->Start())
		{
			return pServer;
		}

		delete pServer;
	}

	writeOutput("\npacsCommunicator::spawnStoreSCP - Impossibile avviare il server");	

	return NULL;
}

//
// writeOutput (_msg)
//  Add _msg to output browser (or stdout if it doesn't exist) and update
//
void pacsCommunicator::writeOutput (const char *_msg)
{
    if (output) {
        output->AppendText(_msg);
        output->SetInsertionPointEnd();
    }
    else
        std::cout << "Pacs Window : " << _msg << std::endl;
}

//
//  store()
//  Store images in a Pacs by using the network parameters 

int pacsCommunicator::store (const char *fileName)
{	
	pacsStoreSCU tmpSCU(prefs);

	return tmpSCU.storeRequest(fileName);
}
