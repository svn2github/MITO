/**
 * \file pacsStoreSCU.cxx
 * \brief File per l'invio dei file dicom al PACS
 * \author ICAR-CNR Napoli
 */

/*
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
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
*/

#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#include <dcmtk/ofstd/ofstd.h>

// Sottolibreria rete di DCMTK
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcvrsl.h>

#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>


#include <iostream>
#include <ctype.h>

#include "pacsStoreSCU.h"

#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/string.h>


/*
	Senza troppi se, ne troppi ma, ho preso questo blocco di codice dallo storescu
	di dcmtk. Questo blocco si occupa di scegliere il giusto contesto per ogni transfer
	syntax elencata.
*/
static OFBool
isaListMember(OFList<OFString>& lst, OFString& s)
{
    OFListIterator(OFString) cur = lst.begin();
    OFListIterator(OFString) end = lst.end();

    OFBool found = OFFalse;

    while (cur != end && !found) {

        found = (s == *cur);

        ++cur;
    }

    return found;
}

static OFCondition
addStorePresentationContext(T_ASC_Parameters *params,
    int presentationContextId, const OFString& abstractSyntax,
    const OFString& transferSyntax,
    T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT)
{
    const char* c_p = transferSyntax.c_str();
    OFCondition cond = ASC_addPresentationContext(params, presentationContextId,
        abstractSyntax.c_str(), &c_p, 1, proposedRole);
    return cond;
}

static OFCondition
addStorePresentationContext(T_ASC_Parameters *params,
    int presentationContextId, const OFString& abstractSyntax,
    const OFList<OFString>& transferSyntaxList,
    T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT)
{
    // create an array of supported/possible transfer syntaxes
    const char** transferSyntaxes = new const char*[transferSyntaxList.size()];
    int transferSyntaxCount = 0;
    OFListConstIterator(OFString) s_cur = transferSyntaxList.begin();
    OFListConstIterator(OFString) s_end = transferSyntaxList.end();
    while (s_cur != s_end) {
        transferSyntaxes[transferSyntaxCount++] = (*s_cur).c_str();
        ++s_cur;
    }

    OFCondition cond = ASC_addPresentationContext(params, presentationContextId,
        abstractSyntax.c_str(), transferSyntaxes, transferSyntaxCount, proposedRole);

    delete[] transferSyntaxes;
    return cond;
}

static OFCondition
addStoragePresentationContexts(T_ASC_Parameters *params, OFList<OFString>& sopClasses)
{
    /*
     * Each SOP Class will be proposed in two presentation contexts (unless
     * the opt_combineProposedTransferSyntaxes global variable is true).
     * The command line specified a preferred transfer syntax to use.
     * This prefered transfer syntax will be proposed in one
     * presentation context and a set of alternative (fallback) transfer
     * syntaxes will be proposed in a different presentation context.
     *
     * Generally, we prefer to use Explicitly encoded transfer syntaxes
     * and if running on a Little Endian machine we prefer
     * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
     * Some SCP implementations will just select the first transfer
     * syntax they support (this is not part of the standard) so
     * organise the proposed transfer syntaxes to take advantage
     * of such behaviour.
     */

    // Which transfer syntax was preferred on the command line
    OFString preferredTransferSyntax;
    /* gLocalByteOrder is defined in dcxfer.h */
    if (gLocalByteOrder == EBO_LittleEndian) {
        /* we are on a little endian machine */
        preferredTransferSyntax = UID_LittleEndianExplicitTransferSyntax;
    } else {
        /* we are on a big endian machine */
        preferredTransferSyntax = UID_BigEndianExplicitTransferSyntax;
    }

    OFListIterator(OFString) s_cur;
    OFListIterator(OFString) s_end;


    OFList<OFString> fallbackSyntaxes;
    fallbackSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
    // Remove the preferred syntax from the fallback list
    fallbackSyntaxes.remove(preferredTransferSyntax);

    // created a list of transfer syntaxes combined from the preferred and fallback syntaxes
    OFList<OFString> combinedSyntaxes;
    s_cur = fallbackSyntaxes.begin();
    s_end = fallbackSyntaxes.end();
    combinedSyntaxes.push_back(preferredTransferSyntax);
    while (s_cur != s_end)
    {
        if (!isaListMember(combinedSyntaxes, *s_cur)) combinedSyntaxes.push_back(*s_cur);
        ++s_cur;
    }

    // add the (short list of) known storage sop classes to the list
    // the array of Storage SOP Class UIDs comes from dcuid.h
    for (int i=0; i<numberOfDcmShortSCUStorageSOPClassUIDs; i++) {
        sopClasses.push_back(dcmShortSCUStorageSOPClassUIDs[i]);
    }

    // thin out the sop classes to remove any duplicates.
    OFList<OFString> sops;
    s_cur = sopClasses.begin();
    s_end = sopClasses.end();
    while (s_cur != s_end) {
        if (!isaListMember(sops, *s_cur)) {
            sops.push_back(*s_cur);
        }
        ++s_cur;
    }

    // add a presentations context for each sop class / transfer syntax pair
    OFCondition cond = EC_Normal;
    int pid = 1; // presentation context id
    s_cur = sops.begin();
    s_end = sops.end();
    while (s_cur != s_end && cond.good()) {

        if (pid > 255) {
            CERR << "Too many presentation contexts" << endl;
            return ASC_BADPRESENTATIONCONTEXTID;
        }

        if (/*opt_combineProposedTransferSyntaxes*/ OFFalse) {
            cond = addStorePresentationContext(params, pid, *s_cur, combinedSyntaxes);
            pid += 2;   /* only odd presentation context id's */
        } else {

            // sop class with preferred transfer syntax
            cond = addStorePresentationContext(params, pid, *s_cur, preferredTransferSyntax);
            pid += 2;   /* only odd presentation context id's */

            if (fallbackSyntaxes.size() > 0) {
                if (pid > 255) {
                    CERR << "Too many presentation contexts" << endl;
                    return ASC_BADPRESENTATIONCONTEXTID;
                }

                // sop class with fallback transfer syntax
                cond = addStorePresentationContext(params, pid, *s_cur, fallbackSyntaxes);
                pid += 2;       /* only odd presentation context id's */
            }
        }
        ++s_cur;
    }

    return cond;
}
// ----------------

pacsStoreSCU::pacsStoreSCU (const pacsPreferences *_prefs)
{
	m_bStoreFailed	= OFTrue;	// Di default assumiamo stato di errore..
	m_iLastStatusCode = STATUS_Success;
	m_pNetwork	 = NULL;		// Network key
	m_pAssociation = NULL;		// Association key
	m_pAssocParams = NULL;		// Association params
    prefs        = _prefs;      // pointer to AP titles, etc.

#ifdef WIN32
    // DCMTK richiede la versione 1.1, non necessariamente la 2.0
	WSAData winSockData;
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    std::cout << WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    memset(localhost, 0, sizeof(localhost));
    gethostname(localhost, sizeof(localhost));

    setParams();                // set up params
}

//
// ~pacsStoreSCU
//    Frees memory, releases resources such as network, association
//    and DCM object
//
pacsStoreSCU::~pacsStoreSCU ()
{
	if(m_pAssociation)
	{
		ASC_releaseAssociation(m_pAssociation); // release association
		ASC_destroyAssociation(&m_pAssociation); // delete assoc structure
	}

	if(m_pNetwork)
		ASC_dropNetwork(&m_pNetwork); // delete net structure



#ifdef WIN32
    WSACleanup();
#endif
}

//
// storeRequest(const char *szFileName)
//    
int pacsStoreSCU::storeRequest(const char *szFileName)
{
	if(!wxFileName::FileExists(wxString(szFileName)))
	{
		CERR << "pacsStoreSCU::storeRequest - Il file selezionato (" << szFileName << ") non esiste." << endl;	
		return 0;
	}

	// Se inviamo più files, non ha senso riassociarci ogni volta..  ;)
	if(!m_pAssociation)
	{
		// In realtà dovremmo negoziare la transfer syntax a seconda di ciò che disponiamo..
        if (!requestAssociation(UID_VerificationSOPClass))
            return 0;  
	}

	OFCondition cond = Store(szFileName);

	return (cond.good()) ? 1 : 0;
}


//
// PRIVATE FUNCTIONS
//
// requestAssociation (_SOPClass)
//
int pacsStoreSCU::requestAssociation (char *_SOPClass)
{
	OFCondition cond;

	// Make sure dictionary is loaded (??)
	if (!dcmDataDict.isDictionaryLoaded())
	{
		CERR << "pacsStoreSCU::requestAssociation - no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
	}

	// Inizializza la rete se non è già inizializzata
	if(m_pNetwork == NULL)
	{
		cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &m_pNetwork);
		if (cond.bad()) 
		{
			DimseCondition::dump(cond);

            std::cout << "pacsStoreSCU::requestAssociation : "
                 << "Error initializinging network!" << std::endl;			
			
			return 0;
		}
	}

    // Setta il contesto
    //cond = AddPresentationContext(1, _SOPClass);
	OFList<OFString> sopClassUIDList;
    cond = addStoragePresentationContexts(m_pAssocParams, sopClassUIDList);
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
            std::cout << "Association Rejected:" << std::endl;
            ASC_printRejectParameters(stderr, &rej);
            return 0;
        } else {
            std::cout << "Association Request Failed:" << std::endl;
            DimseCondition::dump(cond);
            return 0;
        }
    }

    if (ASC_countAcceptedPresentationContexts(m_pAssocParams) == 0) 
	{
		std::cout << "No Acceptable Presentation Contexts" << std::endl;
        return 0;
    }

    return 1; 
}

void pacsStoreSCU::storeCallback(void * /*callbackData*/, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ * /*req*/)
{

	// DEBUG
    //if (opt_verbose) 
	{
        switch (progress->state) {
        case DIMSE_StoreBegin:
            printf("XMIT:"); break;
        case DIMSE_StoreEnd:
            printf("\n"); break;
        default:
            putchar('.'); break;
        }
        fflush(stdout);
    }
}

OFCondition	pacsStoreSCU::Store(const char *szFileName)
{
	/*
     * This function will read all the information from the given file,
     * figure out a corresponding presentation context which will be used
     * to transmit the information over the network to the SCP, and it
     * will finally initiate the transmission of all data to the SCP.
     *
     * Parameters:
     *   m_pAssociation - [in] The association (network connection to another DICOM application).
     *   szFileName - [in] Name of the file which shall be processed.
     */
    DIC_US msgId = m_pAssociation->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_StoreRQ req;
    T_DIMSE_C_StoreRSP rsp;
    DIC_UI sopClass;
    DIC_UI sopInstance;
    DcmDataset *statusDetail = NULL;

    //unsuccessfulStoreEncountered = OFTrue; // assumption

	{
        printf("--------------------------\n");
        printf("Sending file: %s\n", szFileName);
    }

    DcmFileFormat dcmff;
    OFCondition cond = dcmff.loadFile(szFileName);
    if (cond.bad()) 
	{
        CERR << "Bad DICOM file: " << szFileName << ": " << cond.text() << endl;
        return cond;
    }

    // figure out which SOP class and SOP instance is encapsulated in the file 
    if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(), sopClass, sopInstance, OFFalse)) 
	{
        CERR << "No SOP Class & Instance UIDs in file: " << szFileName << endl;
        return DIMSE_BADDATA;
    }

    // figure out which of the accepted presentation contexts should be used
    DcmXfer filexfer(dcmff.getDataset()->getOriginalXfer());

    /* special case: if the file uses an unencapsulated transfer syntax (uncompressed
     * or deflated explicit VR) and we prefer deflated explicit VR, then try
     * to find a presentation context for deflated explicit VR first.
     *//*
    if (filexfer.isNotEncapsulated() &&
        opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit)
    {
        filexfer = EXS_DeflatedLittleEndianExplicit;
    }*/

    if (filexfer.getXfer() != EXS_Unknown) 
		presId = ASC_findAcceptedPresentationContextID(m_pAssociation, sopClass, filexfer.getXferID());
    else 
		presId = ASC_findAcceptedPresentationContextID(m_pAssociation, sopClass);
    if (presId == 0) 
	{
        const char *modalityName = dcmSOPClassUIDToModality(sopClass);
        if (!modalityName) 
			modalityName = dcmFindNameOfUID(sopClass);
        if (!modalityName) 
			modalityName = "unknown SOP class";

        CERR << "No presentation context for: (" << modalityName << ") " << sopClass << endl;

        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* if required, dump general information concerning transfer syntaxes */
	{
        DcmXfer fileTransfer(dcmff.getDataset()->getOriginalXfer());
        T_ASC_PresentationContext pc;
        ASC_findAcceptedPresentationContext(m_pAssociation->params, presId, &pc);
        DcmXfer netTransfer(pc.acceptedTransferSyntax);
        printf("Transfer: %s -> %s\n",
            dcmFindNameOfUID(fileTransfer.getXferID()), dcmFindNameOfUID(netTransfer.getXferID()));
    }

    /* prepare the transmission of data */
    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    strcpy(req.AffectedSOPInstanceUID, sopInstance);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    /* if required, dump some more general information */
	{
        printf("Store SCU RQ: MsgID %d, (%s)\n", msgId, dcmSOPClassUIDToModality(sopClass));
    }

    /* finally conduct transmission of data */
    cond = DIMSE_storeUser(m_pAssociation, presId, &req,
        NULL, dcmff.getDataset(), storeCallback, NULL,
        DIMSE_BLOCKING, 0,
        &rsp, &statusDetail, NULL, DU_fileSize(szFileName));

    /*
     * If store command completed normally, with a status
     * of success or some warning then the image was accepted.
     */
    if (cond == EC_Normal && (rsp.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS(rsp.DimseStatus))) 
	{
        m_bStoreFailed = OFFalse;
    }

    /* remember the response's status for later transmissions of data */
    m_iLastStatusCode = rsp.DimseStatus;

    /* dump some more general information */
    if (cond == EC_Normal)
    {
        // DEBUG
		{
            DIMSE_printCStoreRSP(stdout, &rsp);
        }
    }
    else
    {
        CERR << "pacsStoreSCU::Store - Store Failed, file: " << szFileName << endl;
        DimseCondition::dump(cond);
    }

    /* dump status detail information if there is some */
    if (statusDetail != NULL) 
	{
        printf("  Status Detail:\n");
        statusDetail->print(COUT);
        delete statusDetail;
    }

	return cond;
}

//
// setParams ()
//  Sets up params
//
void pacsStoreSCU::setParams ()
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

