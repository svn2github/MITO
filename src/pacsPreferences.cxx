/**
 * \file pacsPreferences.cxx
 * \brief File per la definizione dei parametri necessari alla comunicazione con il PACS
 * \author ICAR-CNR Napoli
 */


//
// This class will read preferences and store them back to disk
//

#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <fstream>
#include <string>
#include "pacsPreferences.h"


// CONSTRUCTORS
//
// pacsPreferences()
//      Initializes with default values
pacsPreferences::pacsPreferences()
{
    setDefaults();
}

// pacsPreferences (_file)
//      Reads data in from a file
pacsPreferences::pacsPreferences (char *_file)
{
    if (!read(_file))
        setDefaults();
}

// pacsPreferences (_prefs)
//      The copy constructor
pacsPreferences::pacsPreferences (pacsPreferences* _prefs)
{
        strcpy(server,     _prefs->server);
        strcpy(calledAP,   _prefs->calledAP);
        strcpy(callingAP,  _prefs->callingAP);
        strcpy(storageAP,  _prefs->storageAP);
        strcpy(serverPort, _prefs->serverPort);

        strcpy(localDirectory, _prefs->localDirectory);
        strcpy(port,  _prefs->port);
        strcpy(maxPDU, _prefs->maxPDU);
		strcpy(storageServer,"");
		strcpy(storeClient,"send_image -a ");
	    strcat(storeClient,callingAP);
        strcat(storeClient," -c ");
        strcat(storeClient,calledAP);
   	    strcat(storeClient," -m ");
   	    strcat(storeClient,maxPDU);
   	    strcat(storeClient," ");
        strcat(storeClient,server);
   	    strcat(storeClient," ");
   	    strcat(storeClient,serverPort);

/*		dcmOptions = DCM_ORDERLITTLEENDIAN;
*/
        autoSave    = _prefs->autoSave;
        storageExec = _prefs->storageExec;
}

// ~pacsPreferences()
//      The destructor
pacsPreferences::~pacsPreferences() {
}

// PUBLIC FUNCTIONS
//
// setDefaults()
//    sets to default values
void pacsPreferences::setDefaults ()
{
    server[0]     = '\0';
    calledAP[0]   = '\0';
    callingAP[0]  = '\0';
    storageAP[0]  = '\0';
    serverPort[0] = '\0';

    localDirectory[0] = '\0';
    storageServer[0]  = '\0';
    port[0] = '\0';
    maxPDU[0]    = '\0';
    storeClient[0]  = '\0';
    autoSave  = 0;
	storageExec = 0;
//    dcmOptions = DCM_ORDERLITTLEENDIAN;
	strcpy(logFile,"pacs.log");
}

// read (_filename)
//      reads data in from a file, returns 1 if everything went ok
int pacsPreferences::read (char *_filename)
{
	std::ifstream file;
    char buf[S_LEN];

    file.open(_filename, std::ios::in);
    if(!file) {
        std::cout << "Prefs::read : Error reading preferences from " 
          << _filename << std::endl;
    	return 0;
    }

    file.getline(buf,       S_LEN);     // the first line is bullshit

    // network settings
    file.getline(server,     S_LEN);    // server's nodename
    file.getline(serverPort, S_LEN);    // server's port
    file.getline(calledAP,   S_LEN);    // server's AP title
    file.getline(callingAP,  S_LEN);    // our AP title
    file.getline(storageAP,  S_LEN);    // storage server's AP title

    // directories and paths
    file.getline(localDirectory, S_LEN*2);
    file.getline(port,  S_LEN);
    file.getline(maxPDU,  S_LEN);

	strcpy(storageServer,"storescp -od ");
	strcat(storageServer,localDirectory);
	strcat(storageServer," ");
	strcat(storageServer," -v -aet ");
	//strcat(storageServer," -aet ");
    strcat(storageServer,storageAP);
	strcat(storageServer," -pdu ");
    strcat(storageServer,maxPDU);
    strcat(storageServer," ");
    strcat(storageServer,port);
//	strcat(storageServer," > ");
//	strcat(storageServer,logFile);
 
	strcpy(storeClient,"send_image -a ");
    strcat(storeClient,callingAP);
    strcat(storeClient," -c ");
    strcat(storeClient,calledAP);
    strcat(storeClient," -m ");
    strcat(storeClient,maxPDU);
    strcat(storeClient," ");
    strcat(storeClient,server);
    strcat(storeClient," ");
    strcat(storeClient,serverPort);
//	strcat(storeClient," > ");
//	strcat(storeClient,logFile);

   // non-string values, options, etc
    file.getline(buf, S_LEN);       autoSave    = atoi(buf);
    file.getline(buf, S_LEN);       storageExec = atoi(buf);

    file.close();
    return 1;
}

// write(_filename)
//    writes prefs back to disk, returns 1 if everything went ok
int pacsPreferences::write (char *_filename)
{
    std::ofstream file(_filename, std::ios::out);

    if (!file) {
    	std::cout << "Prefs: Error opening " << _filename
    	     << ", to write preferences to disk. " << std::endl;
    	return 0;
    }

    file << "Configuration File - do not edit!" << std::endl;
    file << server                   << std::endl;
    file << serverPort               << std::endl;
    file << calledAP                 << std::endl;
    file << callingAP                << std::endl;
    file << storageAP                << std::endl;
    file << localDirectory           << std::endl;
    file << port		             << std::endl;
    file << maxPDU                   << std::endl;
    file << autoSave                 << std::endl;
    file << storageExec              << std::endl;
    
    file.close();
    return 1;
}

