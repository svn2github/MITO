/**
 * \file pacsQuery.cxx
 * \brief File per la realizzazione di query al PACS
 * \author ICAR-CNR Napoli
 */

// This class provides support for DICOM queries.
// The idea is to keep an instance of this class for each
// object -- each patient will have one instance and for 
// each resulting study, there will also exist an instance
// of this class.

#include <dcmtk/config/osconfig.h> // DCMTK:  make sure OS specific configuration is included first

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#include <dcmtk/ofstd/ofstdinc.h>

// Sottolibreria rete di DCMTK
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>

#include <iostream>
#include <ctype.h>

#include "pacsQuery.h"

#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/string.h>

typedef struct {
    T_ASC_Association *assoc;
    T_ASC_PresentationContextID presId;
} MyCallbackInfo;

// Questa funzione, presa da movescu.cc si occupa di scegliere 
// il tipo di formato della trasmissione e la SOP (?)
static OFCondition
addPresentationContext(T_ASC_Parameters *params,
                        T_ASC_PresentationContextID pid,
                        const char* abstractSyntax,
						E_TransferSyntax  networkTransferSyntax = EXS_Unknown)
{
    /*
    ** We prefer to use Explicitly encoded transfer syntaxes.
    ** If we are running on a Little Endian machine we prefer
    ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    ** Some SCP implementations will just select the first transfer
    ** syntax they support (this is not part of the standard) so
    ** organise the proposed transfer syntaxes to take advantage
    ** of such behaviour.
    **
    ** The presentation contexts proposed here are only used for
    ** C-FIND and C-MOVE, so there is no need to support compressed
    ** transmission.
    */

    const char* transferSyntaxes[] = { NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (networkTransferSyntax) {
    case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
    case EXS_LittleEndianExplicit:
        /* we prefer Little Endian Explicit */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    case EXS_BigEndianExplicit:
        /* we prefer Big Endian Explicit */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    default:
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } else {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    }

    return ASC_addPresentationContext(
        params, pid, abstractSyntax,
        transferSyntaxes, numTransferSyntaxes);
}

// CONSTRUCTORS
//
// pacsQuery (_prefs)
//    Creates an instance of the class.  Nothing is allocated.
//    A pointer to a preferences class is required to instantiate
//    the parameters member.
//
pacsQuery::pacsQuery (const pacsPreferences *_prefs)
{
	m_pNetwork	 = NULL;		// Network key
	m_pAssociation = NULL;		// Association key
	m_pAssocParams = NULL;		// Association params
	m_pQueryObject = NULL;		// Contiene le info sulla query
	qLev         = UNDEFINED;   // No query level yet
    prefs        = _prefs;      // pointer to AP titles, etc.

#ifdef WIN32
    // DCMTK richiede la versione 1.1, non necessariamente la 2.0
	WSAData winSockData;
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    COUT << WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    memset(localhost, 0, sizeof(localhost));
    gethostname(localhost, sizeof(localhost));

    setParams();                // set up params
}

//
// ~pacsQuery
//    Frees memory, releases resources such as network, association
//    and DCM object
//
pacsQuery::~pacsQuery ()
{
	if(m_pQueryObject)
	{
		delete m_pQueryObject;
		m_pQueryObject = NULL;
	}

	if(m_pAssociation)
	{
		ASC_releaseAssociation(m_pAssociation); // release association
		ASC_destroyAssociation(&m_pAssociation); // delete assoc structure
	}
/*
	if(m_pAssocParams)
	{
		ASC_destroyAssociationParameters(&m_pAssocParams);
	}*/

	// Questo puntatore diviene non valido quando m_pAssociation è distrutta
	// ma non viene automaticamente settato a NULL
	m_pAssocParams = NULL;	

	if(m_pNetwork)
		ASC_dropNetwork(&m_pNetwork); // delete net structure



#ifdef WIN32
    WSACleanup();
#endif
}

// PUBLIC FUNCTIONALITY
//
// void buildQuery
//    This is an overloaded function that will take the information in
//    the given node and copy it to the queryObject.  It does not set the
//    query level.
//
void pacsQuery::buildQuery (const PacsData *_data, bool bUseRequiredAttribsOnly)
{
    COUT << "pacsQuery::buildQuery called " << endl;
    _data->display();

	if(m_pQueryObject)
	{
		COUT << "pacsQuery::buildQuery: query gia inizializzata, scartiamola" << endl;
		
		delete m_pQueryObject;
		m_pQueryObject = NULL;
	}

	m_pQueryObject = new DcmDataset();

	// Imposta il livello della query al PACS
    switch (_data->qLev) 
	{
        case PATIENT:
			m_pQueryObject->putAndInsertString(DCM_QueryRetrieveLevel, "PATIENT");
            break;
        case STUDY:
			m_pQueryObject->putAndInsertString(DCM_QueryRetrieveLevel, "STUDY");
            break;
        case SERIES:
			m_pQueryObject->putAndInsertString(DCM_QueryRetrieveLevel, "SERIES");
            break;
        default:
            COUT << "pacsQuery::buildQuery : Error!" << endl;
            return;
    }
/*
	// Attenzione: dobbiamo inserire qui tutti le chiavi DICOM che vogliamo
	// siano popolate nella risposta da parte del PACS, anche se vuote (o NULL).
	m_pQueryObject->putAndInsertString(DCM_PatientsName, ((StudyData*)_data)->patName);
	m_pQueryObject->putAndInsertString(DCM_PatientsBirthDate, ((StudyData*)_data)->patBirthdate);
	m_pQueryObject->putAndInsertString(DCM_StudyDate, ((StudyData*)_data)->date);
	m_pQueryObject->putAndInsertString(DCM_PatientsSex, ((StudyData*)_data)->patSex);
*/

	_data->ExportDicomDataset(m_pQueryObject, bUseRequiredAttribsOnly);

	// Volendo possiamo aggiungere dopo la chiamata precedente, altre
	// chiavi a m_pQueryObject, magari per raffinare la ricerca.

	qLev = _data->qLev;
}

//
// buildFollowupQuery (_data)
//  Instead of querying for like objects, it takes _data and finds 
//  objects in a level lower.  For example, given a PatientData structure,
//  this function will find all corresponding studies.
//
void pacsQuery::buildFollowupQuery (const PacsData *_data)
{
    PacsData  *data;

    switch (_data->qLev) {
        case PATIENT:
            data = new StudyData;
            break;
        case STUDY:
            data = new SeriesData;
            break;
        case SERIES:
            data = new ImageData;
            break;
        default:
            CERR << "pacsQuery::buildFollowupQuery : Error!" << endl;
            return;
    }

	DcmDataset *pTmp = new DcmDataset();	// Temporaneo..
	if(!pTmp)
	{
		CERR << "pacsQuery::buildFollowupQuery : DcmDataset Error!" << endl; 
		return;
	}

	_data->ExportDicomDataset(pTmp);
	data->ImportDicomDataset(pTmp);

	delete pTmp;

    buildQuery(data);
    delete data;
}

//
// queryLevel ()
//  Returns qLev -- allows users to set up a query and see what type of 
//  query is going out.
//
QueryLevel pacsQuery::queryLevel () const
{
    return qLev;
}

//
// int sendQuery (qLev)
//    This constructs the necessary MSG elements and actually sends the 
//    query to the server.  Returns 0 if there was some sort of an error
//    in doing this.  The parameter, qLev, is the query level to use
//    (0=patient, 1=study, 2=series, 3=image)
//
int pacsQuery::sendQuery (PacsList *_lst) 
{
	OFCondition			cond;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ	req;
    T_DIMSE_C_FindRSP	rsp;

	char *qLevelName = NULL;
    
	// Update params in case prefs have changed since we initialized
    setParams();

    // query level specific functionality
    switch (qLev) 
	{
        case PATIENT:                         // patient query level
            qLevelName = UID_FINDPatientRootQueryRetrieveInformationModel;

			// request association
            if (!requestAssociation(qLevelName))
                return 0;        // error requesting association, quit
           break;

        case STUDY:                         // study query level
            qLevelName = UID_FINDStudyRootQueryRetrieveInformationModel;

			// request association
            if (!requestAssociation(qLevelName))
                return 0;        // error requesting association, quit
            break;
/*
        case SERIES:                         // series query level
            qLevelName = UID_FINDPatientRootQueryRetrieveInformationModel;

			// request association
            if (!requestAssociation(DICOM_SOPPATIENTQUERY_FIND))
                return 0;        // error requesting association, quit
            break;

        case IMAGE:                         // image query level
				// Qui cosa metto?
			break;*/

        default:
            qLevelName = UID_FINDPatientRootQueryRetrieveInformationModel;

			if(!requestAssociation(qLevelName))
				return 0;
            //COUT << "pacsQuery::sendQuery : Error!" << endl;
            //return 0;
    }

	if(!m_pAssociation)
		return 0;

	// Verifichiamo il "contesto" selezionato per l'associazione corrente
    presId = ASC_findAcceptedPresentationContextID(m_pAssociation, qLevelName);
    if (presId == 0) 
	{
		COUT << "No presentation context" << endl;
        return 0;
    }

    // Prepariamoci per l'invio del messaggio di richiesta C-FIND
	DIC_US msgId		= m_pAssociation->nextMsgID++;
    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, qLevelName);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

/*
	// PROVA
	COUT << "DUMP QUERY" << endl;
	m_pQueryObject->print(COUT);
	// FINE DEBUG 
*/
	DcmDataset *statusDetail = NULL;

    // Invia la query!
    cond = DIMSE_findUser(m_pAssociation, presId, &req, m_pQueryObject,
                          FindCallback, (void*)_lst/*this*/ ,	// Credo serva solo la lista nella FindCallback..
                          DIMSE_BLOCKING, 0,
                          &rsp, &statusDetail);


	if(statusDetail)
	{
        printf("  Status Detail:\n");
        statusDetail->print(COUT);

		delete statusDetail;
	}

	// Eliminiamo l'associazione
	if(m_pAssociation)
	{
		ASC_releaseAssociation(m_pAssociation); // release association
		ASC_destroyAssociation(&m_pAssociation); // delete assoc structure
		m_pAssociation = NULL; // ? togliere ?
	}

    return 1;
}

//
// int sendMoveRequest (_server)
//    This constructs the necessary MSG elements and actually sends the 
//    query to the server.  Returns 0 if there was some sort of an error
//    in doing this.  The _server parameter indicates a destination
//    storage server's AP title.
//
int pacsQuery::sendMoveRequest (const char *_server)
{
    T_DIMSE_C_MoveRQ    req;
    T_DIMSE_C_MoveRSP   rsp;
	char				*sopClass;
	const char			*server;

    // Update params in case prefs have changed since we initialized
	setParams();

	// Usare DIMSE_moveUser (movescu: 1364)
    if (!_server)
        server = prefs->storageAP;
    else
        server = _server;

    // Level specific code (patient, study, series, image)
	// NB: In effetti credo che sia un pò differente questa 
	// parte rispetto alle CTN. Il livello di query qui si
	// distingue principalmente a seconda delle chiavi passate.
	// 
	// Riprendendo una nota della documentazione:
	// "The query keys of a C-MOVE request should only contain the 
	// QueryRetrieveLevel attribute and one or more of the so-called 
	// "unique key attributes" (PatientID, StudyInstanceUID, 
	// SeriesInstanceUID and SOPInstanceUID)."
    switch(qLev) 
	{
        case PATIENT:	// patient move
            sopClass          = UID_MOVEPatientRootQueryRetrieveInformationModel;
            break;
        case STUDY:                             // study move
            sopClass          = UID_MOVEStudyRootQueryRetrieveInformationModel;
            break;
        case SERIES:                             // series move	( sarà questo?)
            sopClass          = UID_MOVEStudyRootQueryRetrieveInformationModel;
            break;
        case IMAGE:                             // image move
            sopClass          = UID_MOVEStudyRootQueryRetrieveInformationModel; //DICOM_SOPPATIENTQUERY_MOVE;
            break;
        default:
            COUT << "pacsQuery::sendMoveRequest : Error!!" << endl;
            return 0;
    }

    // request association
    if (!requestAssociation(sopClass))
        return 0;        // error requesting association, quit

    T_ASC_PresentationContextID presId = ASC_findAcceptedPresentationContextID(m_pAssociation, sopClass);
    if (presId == 0) 
	{
		COUT << "No presentation context" << endl;
        return 0;
    }

	COUT << endl << "pacsQuery::sendMoveRequest - Sending C-MOVE request..." << endl;

    DIC_US	msgId = m_pAssociation->nextMsgID++;
    DcmDataset	*rspIds = NULL;
    DcmDataset  *statusDetail = NULL;

	// Creiamo la richiesta
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;
    strcpy(req.MoveDestination, server);

	// Inviamo la richiesta C-MOVE al server PACS utilizzando i parametri specificati
	// da buildQuery.
	// NB: il parametro subOpCallback ? Noi non vogliamo che venga avviato un server
	// per lo store in questa sede! 
	OFCondition cond = DIMSE_moveUser(m_pAssociation, presId, &req, m_pQueryObject,
        MoveCallback, (void*)this, DIMSE_BLOCKING, 0, m_pNetwork, /*subOpCallback*/ NULL, NULL,
        &rsp, &statusDetail, &rspIds, /*opt_ignorePendingDatasets*/ OFTrue);

    if (cond == EC_Normal) 
	{
        DIMSE_printCMoveRSP(stdout, &rsp);
        
		if (rspIds != NULL) 
		{
            printf("Response Identifiers:\n");
            rspIds->print(COUT);
        }
    } else 
	{
        CERR << "Move Failed:" << endl;
        DimseCondition::dump(cond);
    }

    if (statusDetail != NULL) 
	{
        printf("  Status Detail:\n");
        statusDetail->print(COUT);
        
		delete statusDetail;
    }

    if (rspIds != NULL) 
		delete rspIds;

	// Eliminiamo l'associazione
	if(m_pAssociation)
	{
		ASC_releaseAssociation(m_pAssociation); // release association
		ASC_destroyAssociation(&m_pAssociation); // delete assoc structure
		// m_pAssociation = NULL; // ? togliere ?
	}

	if(m_pAssocParams)
	{
		//ASC_destroyAssociationParameters(&m_pAssocParams);
		m_pAssocParams = NULL;	// Necessario perchè ASC_destroyAssociation lo rende non valido!
	}

	return 1;
}

//
// makeQuery(_name, _id, _lst)
//  Makes a query for given patient.  This allows
//  for a very quick patient query to be made.  Return non-zero
//  value if successful.
//
int pacsQuery::makeQuery (const char *_name, const char *_patientBirthdate, const char *_studyDate, PacsList *_lst)
{
	StudyData data;

	// NB: Le date sono già nel formato aaaammgg perchè le formattiamo dal picker
    strcpy(data.patName, _name);
	strcpy(data.patBirthdate, _patientBirthdate);
	strcpy(data.date, _studyDate);

	data.qLev=STUDY;
    // Build and send the query
	buildQuery(&data);
    //return sendQuery(_lst);
	return sendQuery(_lst);
}

//
// makeQuery(_dir, _lst)
//  Queries a file path.  If _dir is a file, it will read that one file,
//  otherwise it will read the whole directory, recursively.
//
// Compatibile wxWidgets (Alessio P.)
int pacsQuery::makeQuery(const char *_dir, PacsList *_lst)
{
//    dirent     **files;             // file list returned by filename_list
/*    //int          numFiles;          // number of files in *files
    char         dir[F_LEN];        // contains corrected directory string
    //char         fullname[F_LEN];   // contains full path
    //char         tmp[F_LEN];
    DCM_OBJECT  *obj;               // DCM_OBJECT used for opening files
    PatientData *data;              // Holds data in file processed
    PatientData *findResult;        // Result of searching _lst
    FileData    *file;              // Used for adding to a PacsList

    // bad code follows -- check for . and .. and null filenames
    if (strlen(_dir) == 0)
        return 1;
    // end of bad code

    // Convert relative filename to absolute and clean it up a bit
    //fl_filename_expand(tmp, _dir);
    //fl_filename_absolute(dir, tmp);
	wxFileName dirname( _dir, wxPATH_NATIVE );
	dirname.Normalize();

    // Trash the trailing slash
    const int len = strlen(dir);
    if (dir[len - 1] == '/' || dir[len - 1] == '\\')
        dir[len - 1] = '\0';

    // Check to see if we have a directory or a file in dir
	if (dirname.IsDir()) {

		wxDir dir(dirname.GetFullPath());

		if ( !dir.IsOpened() )
		{
			return 1;	// Ignoriamo gli errori..
		}

		wxString filename;

		bool cont = dir.GetFirst(&filename);
		while ( cont )
		{
			printf("%s\n", filename.c_str());

			// .. and recursively call ourselves for every file in the list
			makeQuery(filename.c_str(), _lst);

			cont = dir.GetNext(&filename);
		}

    }
    else {
        // This is a file...
        data = new PatientData();       // ...so we open it
        if (DCM_OpenFile(dir, prefs->dcmOptions, &obj) == DCM_NORMAL) {
            DCM_ParseObject(&obj, data->reqList, data->reqSize,
                            data->optList, data->optSize, NULL);

            // We've pulled the data we need and can discard the object
            DCM_CloseObject(&obj);

            // Create a FileData entity for storing the filename
            file = new FileData(dir);

            // Check to see if this patient already exists in _lst
            // If so, delete what we just parsed and use it instead
            findResult = (PatientData*) _lst->find(data);
            if (findResult) {
                delete data;
                findResult->files->add(file);
            }
            else {
                data->files = new PacsList();
                data->files->add(file);
                _lst->add(data);
            }
        }
        else {
            COUT << "pacsQuery::makeQuery(_dir, _lst) : "
                 << "Unable to open file " << dir << endl;
            COND_DumpConditions();
        }
    }*/
    return 1;
}

//
// makeFollowupQuery (_data, _lst)
//  Takes an existing PacsData object and returns list of objects in
//  subsequent group.  Patient yeilds Study, etc.
//
int pacsQuery::makeFollowupQuery (const PacsData *_data, PacsList *_lst)
{
	if(_data->files)
	{
		// TODO
		return 0;
	}else
	{
        buildFollowupQuery(_data);
        return sendQuery(_lst);
	}

	return 0;
}

//
// PRIVATE FUNCTIONS
//
// requestAssociation (_SOPClass)
//    Requests an association with which to make a query.  SOPClass
//    should refer to a valid SOP class such as DICOM_SOPPATIENTQUERY_FIND
//
int pacsQuery::requestAssociation (char *_SOPClass)
{
	OFCondition cond;

	// Inizializza la rete se non è già inizializzata
	if(m_pNetwork == NULL)
	{
		if (!dcmDataDict.isDictionaryLoaded()) 
		{
			CERR << "pacsQuery::requestAssociation - Warning: no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
		}

		cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &m_pNetwork);
		if (cond.bad()) 
		{
			DimseCondition::dump(cond);

            COUT << "pacsQuery::requestAssociation - Error initializinging network!" << endl;			
			
			return 0;
		}
	}

    // Setta il contesto
    cond = addPresentationContext(m_pAssocParams, 1, _SOPClass);
    if(cond.bad()) 
	{
        DimseCondition::dump(cond);
        return 0;
    }

    cond = ASC_requestAssociation(m_pNetwork, m_pAssocParams, &m_pAssociation);
    if (cond.bad()) 
	{
        if (cond == DUL_ASSOCIATIONREJECTED) 
		{
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(m_pAssocParams, &rej);
            COUT << "pacsQuery::requestAssociation - Association Rejected:" << endl;
            ASC_printRejectParameters(stderr, &rej);
            return 0;
        } else {
            COUT << "pacsQuery::requestAssociation - Association Request Failed:" << endl;
            DimseCondition::dump(cond);
            return 0;
        }
    }

    if (ASC_countAcceptedPresentationContexts(m_pAssocParams) == 0) 
	{
		COUT << "pacsQuery::requestAssociation - No Acceptable Presentation Contexts" << endl;
        return 0;
    }

    return 1;
}

void pacsQuery::FindCallback(void* callbackData, T_DIMSE_C_FindRQ* request, int responseCount, T_DIMSE_C_FindRSP* rsp, DcmDataset *responseIdentifiers) 
{
/*	FindAssociation* caller = (FindAssociation*)callbackData;

	if(responseCount > caller->maxResults) {
		return;
	}
	
	DcmDataset* response = new DcmDataset(*responseIdentifiers);

	if(caller->bPushResults) {	
		caller->result.push(response);
	}
	caller->OnResponseReceived(response);*/

	// TEST
    printf("RESPONSE: %d (%s)\n", responseCount,
        DU_cfindStatusString(rsp->DimseStatus));

    // dump data set which was received 
    responseIdentifiers->print(COUT);

    // dump delimiter 
    printf("--------\n");
	// FINE TEST

	//Abbiamo passato un puntatore all'istanza di pacsQuery alla callback..
	PacsList *pacsList = (PacsList*)callbackData;
	PacsData *data;

	OFString str;
	OFCondition cond = responseIdentifiers->findAndGetOFString(DCM_QueryRetrieveLevel, str);
	if(cond.bad())
	{
		// errore?
		return;
	}

	// Crea un elemento PacsData per contenere i dati ritornati dal PACS
    if (str == "PATIENT")
    {
		data = (PacsData*) new PatientData;
		COUT << "Patient query" << endl;
    }
    else if (str == "STUDY")
    {
        data = (PacsData*) new StudyData;
        COUT << "Study query" << endl;
    }
    else if (str == "SERIES")
    {
        data = (PacsData*) new SeriesData;
        COUT << "Series query" << endl;
    }
    else if (str == "IMAGES")
    {
        data = (PacsData*) new ImageData;
        COUT << "Image query" << endl;
    }
    else
	{
		// errore?
		return;
	}

	// Memorizziamo i dati ricevuti..
	data->ImportDicomDataset(responseIdentifiers);

	pacsList->add(data);

/*
    // in case opt_extractResponsesToFile is set the responses shall be extracted to a certain file 
    if (opt_extractResponsesToFile) {
        char rspIdsFileName[1024];
        sprintf(rspIdsFileName, "rsp%04d.dcm", responseCount);
        writeToFile(rspIdsFileName, responseIdentifiers);
    }

    MyCallbackInfo *myCallbackData = OFstatic_cast(MyCallbackInfo *, callbackData);

    // should we send a cancel back ?? 
    if (opt_cancelAfterNResponses == responseCount)
    {
        if (opt_verbose)
        {
            printf("Sending Cancel RQ, MsgId: %d, PresId: %d\n", request->MessageID, myCallbackData->presId);
        }
        OFCondition cond = DIMSE_sendCancelRequest(myCallbackData->assoc, myCallbackData->presId, request->MessageID);
        if (cond.bad())
        {
            errmsg("Cancel RQ Failed:");
            DimseCondition::dump(cond);
        }
    }
*/
}

void pacsQuery::MoveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, int responseCount, T_DIMSE_C_MoveRSP *response)
{
    //OFCondition cond = EC_Normal;

	printf("pacsQuery::MoveCallback - Move Response %d: ", responseCount);
	DIMSE_printCMoveRSP(stdout, response);

	// Potrebbe tornarci utile questo codice per eventualmente interrompere 
	// le richieste onerose in termini di tempo
	/*
    if (opt_cancelAfterNResponses == responseCount) {
        if (opt_verbose) {
            printf("Sending Cancel RQ, MsgId: %d, PresId: %d\n",
                request->MessageID, myCallbackData->presId);
        }
        cond = DIMSE_sendCancelRequest(myCallbackData->assoc,
            myCallbackData->presId, request->MessageID);
        if (cond != EC_Normal) {
            errmsg("Cancel RQ Failed:");
            DimseCondition::dump(cond);
        }
    }*/
}

//
// setParams ()
//  Sets up params
//
void pacsQuery::setParams ()
{
	OFCondition cond;

	if(m_pAssocParams == NULL)
	{
		// Alloca il contenitore delle impostazioni di connessione
		cond = ASC_createAssociationParameters(&m_pAssocParams, ASC_DEFAULTMAXPDU);
		if (cond.bad()) 
		{
			DimseCondition::dump(cond);
			return;
		}
	}

	// NB: Non dovremmo controllare che prefs->X sia NULL?
	cond = ASC_setAPTitles(m_pAssocParams, prefs->callingAP, prefs->calledAP, NULL);
	if (cond.bad()) 
	{
		DimseCondition::dump(cond);
		return;
	}

	wxString destHost = wxString::Format("%s:%s", prefs->server, prefs->serverPort);
	cond = ASC_setPresentationAddresses(m_pAssocParams, localhost, destHost);
	if (cond.bad()) 
	{
		DimseCondition::dump(cond);
		return;
	}

	// Setta il tipo di trasporto della rete (potrebbe supportare OpenSSL)
    cond = ASC_setTransportLayerType(m_pAssocParams, OFFalse);
    if (cond.bad()) 
	{
        DimseCondition::dump(cond);
        return;
    }
}

