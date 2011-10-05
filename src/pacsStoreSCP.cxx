/**
 * \file pacsCommunicator.cxx
 * \brief File per la comunicazione con i PACS
 * \author ICAR-CNR Napoli
 */

#include "pacsStoreSCP.h"
#include "pacsCommunicator.h"

#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#include <dcmtk/ofstd/ofstd.h>

// Sottolibreria rete di DCMTK
#include <dcmtk/dcmnet/diutil.h>
#include "dcmtk/dcmdata/dcvrsl.h"

#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>


#include <iostream>
#include <ctype.h>

#include <wx/filename.h>
#include <wx/dir.h>

// Implementiamo l'evento per comunicare col thread principale
DEFINE_EVENT_TYPE(wxEVT_STORE_NOTIFICATION)

// Quest'altra funzioneestrae tutte le informazioni di cui
// abbiamo bisogno per la cache locale.. (il vecchio album.txt). 
wxArrayString *ExtractRelevantInformations(DcmDataset *pDataset, const char *pFilePath)
{
	if(!pDataset)
		return NULL;

	wxArrayString *pArray = new wxArrayString();
	if(!pArray)
		return NULL;

	const char *pTemp = NULL;

	// Nome paziente (0)
	OFCondition cond = pDataset->findAndGetString( DCM_PatientsName, pTemp, OFFalse );
	if( cond.bad() )
	{
		pArray->Add( _T(""));
	}else
		pArray->Add(pTemp);

	// ID paziente (1)
	cond = pDataset->findAndGetString( DCM_PatientID, pTemp, OFFalse );
	if( cond.bad() )
	{
		delete pArray;
		return NULL;
	}

	pArray->Add(pTemp);

	// patient birthdate (2)
	cond = pDataset->findAndGetString( DCM_PatientsBirthDate, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// età paziente (3)
	cond = pDataset->findAndGetString( DCM_PatientsAge, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// Study UID (4)
	cond = pDataset->findAndGetString( DCM_StudyInstanceUID, pTemp, OFFalse );
	if( cond.bad() )
	{
		delete pArray;
		return NULL;
	}else
		pArray->Add(pTemp);

	// descrizione (5)
	cond = pDataset->findAndGetString( DCM_StudyDescription, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// modality (6)
	cond = pDataset->findAndGetString( DCM_Modality /*DCM_ModalitiesInStudy ?*/, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// date (7)
	cond = pDataset->findAndGetString( DCM_StudyDate , pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// # imgs (8)
	cond = pDataset->findAndGetString( DCM_NumberOfStudyRelatedInstances , pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// Series UID (9)
	cond = pDataset->findAndGetString( DCM_SeriesInstanceUID, pTemp, OFFalse );
	if( cond.bad() )
	{
		delete pArray;
		return NULL;
	}

	pArray->Add(pTemp);

	// body part examined in series (10)
	cond = pDataset->findAndGetString( DCM_BodyPartExamined, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// descrizione serie (11)
	cond = pDataset->findAndGetString( DCM_SeriesDescription, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// # imgs (12)
	cond = pDataset->findAndGetString( DCM_NumberOfSeriesRelatedInstances, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// date (13)
	cond = pDataset->findAndGetString( DCM_SeriesDate, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// accession (14)
	cond = pDataset->findAndGetString( DCM_AccessionNumber, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// referring physician (15)
	cond = pDataset->findAndGetString( DCM_ReferringPhysiciansName, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// performing physician (16)
	cond = pDataset->findAndGetString( DCM_PerformingPhysiciansName, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// institution (17)
	cond = pDataset->findAndGetString( DCM_InstitutionName, pTemp, OFFalse );
	if( cond.bad() )
	{
		//delete pArray;
		//return NULL;
		pArray->Add(_T(""));
	}else
		pArray->Add(pTemp);

	// Save path  (18)
	pArray->Add(pFilePath);

	return pArray;
}

static void
storeSCPCallback(
    void *callbackData,
    T_DIMSE_StoreProgress *progress,
    T_DIMSE_C_StoreRQ *req,
    char * /*imageFileName*/, DcmDataset **imageDataSet,
    T_DIMSE_C_StoreRSP *rsp,
    DcmDataset **statusDetail)
    /*
     * This function.is used to indicate progress when storescp receives instance data over the
     * network. On the final call to this function (identified by progress->state == DIMSE_StoreEnd)
     * this function will store the data set which was received over the network to a file.
     * Earlier calls to this function will simply cause some information to be dumped to stdout.
     *
     * Parameters:
     *   callbackData  - [in] data for this callback function
     *   progress      - [in] The state of progress. (identifies if this is the initial or final call
     *                   to this function, or a call in between these two calls.
     *   req           - [in] The original store request message.
     *   imageFileName - [in] The path to and name of the file the information shall be written to.
     *   imageDataSet  - [in] The data set which shall be stored in the image file
     *   rsp           - [inout] the C-STORE-RSP message (will be sent after the call to this function)
     *   statusDetail  - [inout] This variable can be used to capture detailed information with regard to
     *                   the status information which is captured in the status element (0000,0900). Note
     *                   that this function does specify any such information, the pointer will be set to NULL.
     */
{
  DIC_UI sopClass;
  DIC_UI sopInstance;

  
/*// Questo blocco di codice può tornarci utile in caso l'utente voglia interrompere l'operazione?
  // determine if the association shall be aborted
  if( (opt_abortDuringStore && progress->state != DIMSE_StoreBegin) ||
      (opt_abortAfterStore && progress->state == DIMSE_StoreEnd) )
  {
    if (opt_verbose)
      printf("ABORT initiated (due to command line options)\n");
    ASC_abortAssociation((OFstatic_cast(StoreCallbackData*, callbackData))->assoc);
    rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
    return;
  }
*/

	// dump some information if required (depending on the progress state)
	switch (progress->state)
	{
		case DIMSE_StoreBegin:
			COUT << "storeSCPCallback - Ricevuto DIMSE_StoreBegin" << endl;
			break;
		case DIMSE_StoreEnd:
			COUT << "storeSCPCallback - Ricevuto DIMSE_StoreEnd" << endl;
			break;
		default:
			COUT << ".";
			break;
	}
	fflush(stdout);

	// if this is the final call of this function, save the data which was received to a file
	// (note that we could also save the image somewhere else, put it in database, etc.)
	if (progress->state == DIMSE_StoreEnd)
	{
		// do not send status detail information
		*statusDetail = NULL;

		// remember callback data
		StoreCallbackData *cbdata = OFstatic_cast(StoreCallbackData *, callbackData);

		// Concerning the following line: an appropriate status code is already set in the resp structure,
		// it need not be success. For example, if the caller has already detected an out of resources problem
		// then the status will reflect this.  The callback function is still called to allow cleanup.
		//rsp->DimseStatus = STATUS_Success;

		// we want to write the received information to a file only if this information
		// is present and the options opt_bitPreserving and opt_ignore are not set.
		if ((imageDataSet)&&(*imageDataSet)/*&&(!opt_bitPreserving) //Perchè?*/)
		{
			// Estraiamo l'iD della serie e quello dello studio per segnalare al thread
			// principale l'avvenuto download
			const char *tmpStudyUID = NULL;
			const char *tmpSeriesUID = NULL;
			static OFString szOldSeriesUID;
			static OFString szOldStudyUID;

			OFCondition ec1 = (*imageDataSet)->findAndGetString( DCM_StudyInstanceUID, tmpStudyUID, OFFalse );
			OFCondition ec2 = (*imageDataSet)->findAndGetString( DCM_SeriesInstanceUID, tmpSeriesUID, OFFalse );
			if( ec1.bad() || ec2.bad() )
			{
				fprintf(stderr, "storescp: No study instance UID found in data set.\n");
				rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
				return;
			}

			pacsStoreSCP *pStoreSrv = (pacsStoreSCP*)(cbdata->pCallerPtr);

			// Spostiamo il file appena scaricato
			wxFileName dirname( pStoreSrv->getPrefs()->localDirectory, wxPATH_NATIVE );
			dirname.Normalize();

			wxString fullPath(dirname.GetFullPath());

			// Elimina l'ultimo separatore di percorso dal nome della directory
			if (!fullPath.empty() && (fullPath.Last() == _T(PATH_SEPARATOR)) )
			{
				// chop off the last (back)slash
				fullPath.Truncate(fullPath.length() - 1);
			}

			fullPath += wxT(PATH_SEPARATOR);
			fullPath += tmpStudyUID;
			fullPath += wxT(PATH_SEPARATOR);
			fullPath += tmpSeriesUID;

			// Creiamo l'albero delle directory..
			if(!wxFileName::Mkdir(fullPath, 511, wxPATH_MKDIR_FULL))
			{
				// C'è stato un errore..
				ASC_abortAssociation((OFstatic_cast(StoreCallbackData*, callbackData))->assoc);
				rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
				return;
			}

			OFString fileName;

			fileName = (const char *)fullPath;
			fileName += PATH_SEPARATOR;
			fileName += cbdata->imageFileName;

			// Inviamo al thread principale i messaggi relativi allo studio
			if(szOldStudyUID.empty() || (szOldStudyUID != tmpStudyUID))
			{
				szOldStudyUID = tmpStudyUID;

				// Segnaliamo che stiamo ricevendo una nuova serie
				wxCommandEvent evt(wxEVT_STORE_NOTIFICATION, wxID_ANY);
				evt.SetInt(EVT_MITO_RECEIVING_NEW_STUDY);

				// Estraiamo le informazioni riguardo lo studio in modo 
				// da visualizzarle nell'album
				wxArrayString *pToDisplay = ExtractRelevantInformations(*imageDataSet, fileName.c_str());
				if(pToDisplay)
					evt.SetClientData(pToDisplay);

				if(pStoreSrv && pStoreSrv->getParent())
				{
					pStoreSrv->getParent()->AddPendingEvent(evt);
				}
			}

			// Vediamo che messaggi inviare al thread principale riguardo le serie
			// NB: Se abbiamo solo una serie in questo studio, il messaggio 
			// EVT_MITO_RECEIVED_SERIES non viene notificato al thread principale
			// Dovrei correggere questo comportamento, o almeno trovare un workaround,
			// ma non mi viene nulla in mente al momento.
			if(szOldSeriesUID.empty())
			{
				// E' il primo series UID ricevuto..
				szOldSeriesUID = tmpSeriesUID;

				// Segnaliamo che stiamo ricevendo una nuova serie
				wxCommandEvent evt(wxEVT_STORE_NOTIFICATION, wxID_ANY);
				evt.SetInt(EVT_MITO_RECEIVING_NEW_SERIES);

				// Estraiamo le informazioni riguardo la serie in modo 
				// da visualizzarle nell'album
				wxArrayString *pToDisplay = ExtractRelevantInformations(*imageDataSet, fileName.c_str());
				if(pToDisplay)
					evt.SetClientData(pToDisplay);

				if(pStoreSrv && pStoreSrv->getParent())
				{
					pStoreSrv->getParent()->AddPendingEvent(evt);
				}
				
			}else if(szOldSeriesUID != tmpSeriesUID)
			{
				// E' cambiato il series UID, quindi stiamo ricevendo
				// una nuova serie!
				szOldSeriesUID = tmpSeriesUID;

				// Segnaliamo che abbiamo appena finito di ricevere una serie
				wxCommandEvent evt(wxEVT_STORE_NOTIFICATION, wxID_ANY);
				evt.SetInt(EVT_MITO_RECEIVED_SERIES);

				if(pStoreSrv && pStoreSrv->getParent())
				{
					pStoreSrv->getParent()->AddPendingEvent(evt);
				}

				// E poi segnaliamo che ne stiamo ricevendo un altra
				wxCommandEvent evt1(wxEVT_STORE_NOTIFICATION, wxID_ANY);
				evt1.SetInt(EVT_MITO_RECEIVING_NEW_SERIES);

				// Estraiamo le informazioni riguardo la serie in modo 
				// da visualizzarle nell'album
				wxArrayString *pToDisplay = ExtractRelevantInformations(*imageDataSet, fileName.c_str());
				if(pToDisplay)
					evt1.SetClientData(pToDisplay);

				if(pStoreSrv && pStoreSrv->getParent())
				{
					pStoreSrv->getParent()->AddPendingEvent(evt1);
				}
			}

			// Segnaliamo che abbiamo appena ricevuto una nuova immagine
			wxCommandEvent evt(wxEVT_STORE_NOTIFICATION, wxID_ANY);
			evt.SetInt(EVT_MITO_RECEIVED_IMAGE);

			// Estraiamo le informazioni riguardo la serie in modo 
			// da visualizzarle nell'album
			wxArrayString *pToDisplay = ExtractRelevantInformations(*imageDataSet, fileName.c_str());
			if(pToDisplay)
				evt.SetClientData(pToDisplay);

			if(pStoreSrv && pStoreSrv->getParent())
			{
				pStoreSrv->getParent()->AddPendingEvent(evt);
			}

			// determine the transfer syntax which shall be used to write the information to the file
			E_TransferSyntax xfer = (*imageDataSet)->getOriginalXfer();

			// store file either with meta header or as pure dataset
			OFCondition cond = cbdata->dcmff->saveFile(fileName.c_str(), xfer, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding, 0, 0, OFFalse);
			if (cond.bad())
			{
				fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName.c_str());
				rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
			}

			// check the image to make sure it is consistent, i.e. that its sopClass and sopInstance correspond
			// to those mentioned in the request. If not, set the status in the response message variable.
			if (rsp->DimseStatus == STATUS_Success)
			{
				// which SOP class and SOP instance ?
				if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, OFFalse))
				{
					fprintf(stderr, "storescp: Bad image file: %s\n", fileName.c_str());
					rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
				}
				else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
				{
					rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
				}
				else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
				{
					rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
				}
			}

		}


	}
}

pacsStoreSCP::pacsStoreSCP(const pacsPreferences* prefs, wxEvtHandler *pParent)
{
	m_pNetwork	= NULL;
	m_pAcceptor = NULL;
	m_pPrefs	= (pacsPreferences*)prefs;
	m_pParent	= pParent;
}

pacsStoreSCP::~pacsStoreSCP()
{
	/*
	if(m_pAcceptor)
	{
		// Dovrebbe essere già distrutto..
		// Quindi ci limitiamo a non fare niente. 
		// In effetti non ha molto senso conservare il puntatore a questo thread.. nd Alessio
		m_pAcceptor->Delete();
	}
	*/

	OFCondition cond = ASC_dropNetwork(&m_pNetwork);
	if (cond.bad())
	{
		DimseCondition::dump(cond);
	}

	if(m_pPrefs)
		m_pPrefs = NULL;

	m_pPrefs	= NULL;
	m_pParent	= NULL;
}

int pacsStoreSCP::Start()
{
#ifdef HAVE_GETEUID
	/* if port is privileged we must be as well */
	if (atoi(m_pPrefs->port) < 1024)
	{
		if (geteuid() != 0)
		{
			CERR << "pacsStoreSCP::Start - Impossibile ascoltare sulla porta " << m_pPrefs->port << endl;
			return 0;
		}
	}
#endif

	wxFileName dirname( m_pPrefs->localDirectory, wxPATH_NATIVE );
	dirname.Normalize();

	wxString fullPath(dirname.GetFullPath());

	if(!wxDir::Exists(fullPath))
	{
		CERR << "pacsStoreSCP::Start - Directory non valida o inesistente ('" << fullPath << "')" << endl;
//		return 0;
	}

	// Elimina l'ultimo separatore di percorso dal nome della directory
    if (!fullPath.empty() && (fullPath.Last() == _T(PATH_SEPARATOR)) )
    {
        // chop off the last (back)slash
        fullPath.Truncate(fullPath.length() - 1);
    }

	// Qui dovrebbe creare un alto thread per le ACCEPT
	m_pAcceptor = new _pacsStoreAcceptorThread();
	if(m_pAcceptor)
		m_pAcceptor->Create(this, fullPath);

	return 1;
}

void pacsStoreSCP::Stop()
{
	//if(m_pAcceptor)
	//	m_pAcceptor->Delete();

	OFCondition cond = ASC_dropNetwork(&m_pNetwork);
	if (cond.bad())
	{
		DimseCondition::dump(cond);
	}
}

/********* ALTRA CLASSE **********/

void pacsStoreSCP::_pacsStoreAcceptorThread::Create(pacsStoreSCP *pCaller, const wxString &sSavePath)
{
	if(!pCaller)
	{
		CERR << "_pacsStoreAcceptorThread::Create - Puntatore alla classe chiamante non valido!" << endl;
		return;
	}

	if(wxTHREAD_NO_ERROR != wxThread::Create())
	{
		CERR << "_pacsStoreAcceptorThread::Create - wxThread::Create fallito!" << endl;
		return;
	}

	// Salviamo un puntatore al chiamante..
	m_pCaller = pCaller;

	// Salva il percorso di download delle immagini
	m_sSaveToPath = sSavePath;

	Run();
}

void* pacsStoreSCP::_pacsStoreAcceptorThread::Entry()
{
	T_ASC_Network *pNet = NULL;	 

	// Make sure dictionary is loaded (??)
	if (!dcmDataDict.isDictionaryLoaded())
	{
		CERR << "_pacsStoreAcceptorThread::Entry - no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
	}

	// Se inizializzassimo il network nel thread chiamante, non funzionerebbe qui..
	OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, atoi(m_pCaller->getPrefs()->port), 30, &pNet);
	if (cond.bad())
	{
		CERR << "_pacsStoreAcceptorThread::Entry - Errore di rete." << endl;
		DimseCondition::dump(cond);
		return 0;
	}
	
#ifdef HAVE_GETUID
	/* return to normal uid so that we can't do too much damage in case
	* things go very wrong.   Only does someting if the program is setuid
	* root, and run by another user.  Running as root user may be
	* potentially disasterous if this program screws up badly.
	*/
	setuid(getuid());
#endif

	do
	{
		cond = ProcessAssociations(pNet);

	} while (cond.good());

	// Liberiamo le risorse associate al socket
	cond = ASC_dropNetwork(&pNet);
	if (cond.bad())
	{
		DimseCondition::dump(cond);
		return NULL;
	}

	cout << "_pacsStoreAcceptorThread::Entry - Thread terminato!" << endl;

	return NULL;
}

OFCondition pacsStoreSCP::_pacsStoreAcceptorThread::ProcessAssociations(T_ASC_Network *pNet)
{
	T_ASC_Association *pAssociation;
	char buf[BUFSIZ];
	const char* knownAbstractSyntaxes[] = { UID_VerificationSOPClass };
	const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
	int numTransferSyntaxes = 0;

	// Scegliamo le transfer syntax (non dovrebbe negoziarle con chi invia?) 
    if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
    {
		transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
		transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    }
    else
    {
		transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
		transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }

	transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
	numTransferSyntaxes = 3;

	OFCondition cond = ASC_receiveAssociation(pNet, &pAssociation, ASC_DEFAULTMAXPDU, NULL, NULL, OFFalse);
	if(cond.bad())
		return cond;
/*
	// DEBUG	( Da qui in poi dovrebbe "spawnare" un altro thread se vogliamo ricevere da SCU diverse!)
	printf("Parameters:\n");
	ASC_dumpParameters(pAssociation->params, COUT);

	DIC_AE callingTitle;
	DIC_AE calledTitle;
	ASC_getAPTitles(pAssociation->params, callingTitle, calledTitle, NULL);

	CERR << "called AE:  " << calledTitle << endl
		<< "calling AE: " << callingTitle << endl;
	// END DEBUG
	*/
	// accept the Verification SOP Class if presented 
	cond = ASC_acceptContextsWithPreferredTransferSyntaxes( pAssociation->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);
	if (cond.bad())
	{
		DimseCondition::dump(cond);

		cond = ASC_dropSCPAssociation(pAssociation);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
			// Cavolo non dovrebbe mai accadere..
		}

		cond = ASC_destroyAssociation(&pAssociation);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
			// Cavolo non dovrebbe mai accadere..
		}

		return cond;
	}

	cond = ASC_acceptContextsWithPreferredTransferSyntaxes( pAssociation->params, dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs, transferSyntaxes, numTransferSyntaxes);
	if (cond.bad())
	{
		DimseCondition::dump(cond);

		cond = ASC_dropSCPAssociation(pAssociation);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
			// Cavolo non dovrebbe mai accadere..
		}

		cond = ASC_destroyAssociation(&pAssociation);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
			// Cavolo non dovrebbe mai accadere..
		}

		return cond;
	}

	ASC_setAPTitles(pAssociation->params, NULL, NULL, m_pCaller->getPrefs()->callingAP);

	// Accettiamo o rifiutiamo l'associazione?
	cond = ASC_getApplicationContextName(pAssociation->params, buf);
	if ((cond.bad()) || strcmp(buf, UID_StandardApplicationContext) != 0)
	{
		// reject: the application context name is not supported
		T_ASC_RejectParameters rej =
		{
			ASC_RESULT_REJECTEDPERMANENT,
			ASC_SOURCE_SERVICEUSER,
			ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
		};

		COUT << "Association Rejected: bad application context name: " << buf << endl;

		cond = ASC_rejectAssociation(pAssociation, &rej);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
		}
		
		cond = ASC_dropSCPAssociation(pAssociation);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
			// Cavolo non dovrebbe mai accadere..
		}

		cond = ASC_destroyAssociation(&pAssociation);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
			// Cavolo non dovrebbe mai accadere..
		}

		return cond;

	}/*else if (opt_rejectWithoutImplementationUID && strlen(pAssociation->params->theirImplementationClassUID) == 0)
	{
		// Questo blocco rifiuta l'associazione nel caso in cui il richiedente 
		// non abbia inviato l'Implementation Class UID... 
		T_ASC_RejectParameters rej =
		{
			ASC_RESULT_REJECTEDPERMANENT,
			ASC_SOURCE_SERVICEUSER,
			ASC_REASON_SU_NOREASON
		};

		COUT << "Association Rejected: No Implementation Class UID provided" << endl;
		cond = ASC_rejectAssociation(pAssociation, &rej);
		if (cond.bad())
		{
			DimseCondition::dump(cond);
		}

		//goto cleanup;
		break;
	}*/
	else
	{
	/*#ifdef PRIVATE_STORESCP_CODE
		PRIVATE_STORESCP_CODE
	#endif*/

		cond = ASC_acknowledgeAssociation(pAssociation);
		if (cond.bad())
		{
			DimseCondition::dump(cond);

			cond = ASC_dropSCPAssociation(pAssociation);
			if (cond.bad())
			{
				DimseCondition::dump(cond);
				// Cavolo non dovrebbe mai accadere..
			}

			cond = ASC_destroyAssociation(&pAssociation);
			if (cond.bad())
			{
				DimseCondition::dump(cond);
				// Cavolo non dovrebbe mai accadere..
			}

			return cond;
		}
		
		// DEBUG
		COUT << "Association Acknowledged (Max Send PDV: " << pAssociation->sendPDVLength << ")" << endl;
		if (ASC_countAcceptedPresentationContexts(pAssociation->params) == 0)
			COUT << "    (but no valid presentation contexts)" << endl;

		ASC_dumpParameters(pAssociation->params, COUT);
		// END DEBUG
	}

	#ifdef BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX
	/* active the dcmPeerRequiresExactUIDCopy workaround code
	* (see comments in dimse.h) for a implementation class UID
	* prefix known to exhibit the buggy behaviour.
	*/
	if (0 == strncmp(pAssociation->params->theirImplementationClassUID,
		BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX,
		strlen(BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX)))
	{
		dcmEnableAutomaticInputDataCorrection.set(OFFalse);
		dcmPeerRequiresExactUIDCopy.set(OFTrue);
	}
	#endif

	cond = ProcessCommands(pAssociation);
	if (cond == DUL_PEERREQUESTEDRELEASE)
	{
		cout << "_pacsStoreAcceptorThread::Entry - Association Release" << endl;
		cond = ASC_acknowledgeRelease(pAssociation);
	}
	else if (cond == DUL_PEERABORTEDASSOCIATION)
	{
		cout << "_pacsStoreAcceptorThread::Entry - Association Aborted" << endl;
	}
	else
	{
		CERR << "_pacsStoreAcceptorThread::Entry - DIMSE Failure (aborting association)" << endl;
		cond = ASC_abortAssociation(pAssociation);
	}

	cond = ASC_dropSCPAssociation(pAssociation);
	if (cond.bad())
	{
		DimseCondition::dump(cond);
		// Cavolo non dovrebbe mai accadere..
	}

	cond = ASC_destroyAssociation(&pAssociation);
	if (cond.bad())
	{
		DimseCondition::dump(cond);
		// Cavolo non dovrebbe mai accadere..
	}

	// Ora inviamo un messaggio al thread principale
	// per dirgli che, per quanto riguarda questa associazione,
	// non abbiamo più dati da scambiare.
	wxCommandEvent evt(wxEVT_STORE_NOTIFICATION, wxID_ANY);
	evt.SetInt(EVT_MITO_RECEIVE_NOMORE);

	pacsStoreSCP *pStoreSrv = (pacsStoreSCP*)m_pCaller;
	if(pStoreSrv && pStoreSrv->getParent())
	{
		pStoreSrv->getParent()->AddPendingEvent(evt);
	}

	return cond;
}

OFCondition pacsStoreSCP::_pacsStoreAcceptorThread::ProcessCommands(T_ASC_Association *pAssociation)
{
  OFCondition cond = EC_Normal;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID = 0;
  DcmDataset *statusDetail = NULL;

  // start a loop to be able to receive more than one DIMSE command
  while( cond == EC_Normal || cond == DIMSE_NODATAAVAILABLE || cond == DIMSE_OUTOFRESOURCES )
  {
    cond = DIMSE_receiveCommand(pAssociation, DIMSE_BLOCKING, 0, &presID, &msg, &statusDetail);

    // check what kind of error occurred. If no data was
    // received, check if certain other conditions are met
    if( cond == DIMSE_NODATAAVAILABLE )
    {
		wxASSERT(false);	// Giusto per vedere quando si verifica..
	}

    // if the command which was received has extra status
    // detail information, dump this information
    if (statusDetail != NULL)
    {
      printf("Extra Status Detail: \n");
      statusDetail->print(COUT);
      delete statusDetail;
    }

    // check if peer did release or abort, or if we have a valid message
    if (cond == EC_Normal)
    {
      // in case we received a valid message, process this command
      // note that storescp can only process a C-ECHO-RQ and a C-STORE-RQ
      switch (msg.CommandField)
      {
        case DIMSE_C_ECHO_RQ:
          // process C-ECHO-Request
          cond = HandleEchoRQ(pAssociation, &msg, presID);
          break;
        case DIMSE_C_STORE_RQ:
          // process C-STORE-Request
          cond = HandleStoreRQ(pAssociation, &msg, presID);
          break;
        default:
          // we cannot handle this kind of message
          cond = DIMSE_BADCOMMANDTYPE;
          fprintf(stderr, "storescp: Cannot handle command: 0x%x\n", OFstatic_cast(unsigned, msg.CommandField));
          break;
      }
    }
  }

  return cond;
}

OFCondition pacsStoreSCP::_pacsStoreAcceptorThread::HandleEchoRQ(T_ASC_Association * pAssociation, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
	COUT << "_pacsStoreAcceptorThread::HandleEchoRQ - Ricevuto" << endl;
	DIMSE_printCEchoRQ(stdout, &msg->msg.CEchoRQ);

	// Risponde alla richiesta di ECHO
	OFCondition cond = DIMSE_sendEchoResponse(pAssociation, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);
	if (cond.bad())
	{
		CERR << "_pacsStoreAcceptorThread::HandleEchoRQ - Echo SCP Failed" << endl;
		DimseCondition::dump(cond);
	}

	return cond;
}

OFCondition pacsStoreSCP::_pacsStoreAcceptorThread::HandleStoreRQ(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
	OFCondition cond = EC_Normal;
	T_DIMSE_C_StoreRQ *req;
	char imageFileName[2048];

	// Il salvataggio dei files in locale, avviene in due passaggi. Nel primo passo, i files
	// vengono semplicemente salvati nella sottocartella specificata dall'utente. Una volta
	// che invece il file è stato scaricato e verificato, utilizziamo la tecnica sotto illustrata.
	// Salviamo il file utilizzando una particolare struttura delle cartelle di questo tipo:
	//		- Store_Root
	//			- Study UID
	//				- Series 1 UID
	//					- Instance 1
	//					- Instance 2
	//					- ...
	//				- Series 2 UID
	//					- Instance 1
	//			- Study N UID

	// assign the actual information of the C-STORE-RQ command to a local variable
	req = &msg->msg.CStoreRQ;

	/* create unique filename by generating a temporary UID and using ".X." as an infix
	char buf[70];
	dcmGenerateUniqueIdentifier(buf);
	sprintf(imageFileName, "%s%c%s.X.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID), buf, opt_fileNameExtension.c_str());
	*/

	// Utiliziamo l'UID del file in arrivo..
	//sprintf(imageFileName, "%s%c%s.%s%s", m_sSaveToPath, PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID), req->AffectedSOPInstanceUID, ".dcm");
	sprintf(imageFileName, "%s.%s%s", dcmSOPClassUIDToModality(req->AffectedSOPClassUID), req->AffectedSOPInstanceUID, ".dcm");
	
	// Stampiamo un pò di informazioni
	COUT << "_pacsStoreAcceptorThread::HandleStoreRQ - Ricevuto:" << endl;
	DIMSE_printCStoreRQ(stdout, req);

	StoreCallbackData callbackData;
	callbackData.assoc = assoc;
	callbackData.imageFileName = imageFileName;
	callbackData.pCallerPtr	= m_pCaller;
	DcmFileFormat dcmff;
	callbackData.dcmff = &dcmff;
	DcmDataset *dset = dcmff.getDataset();

	cond = DIMSE_storeProvider(assoc, presID, req, NULL, OFTrue, &dset,
	storeSCPCallback, &callbackData, DIMSE_BLOCKING, 0);

	// if some error occured, dump corresponding information and remove the outfile if necessary
	if (cond.bad())
	{
		fprintf(stderr, "storescp: Store SCP Failed:\n");
		DimseCondition::dump(cond);
		
		// remove file
		if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) 
			unlink(imageFileName);
	}

	// callbackData.pCallerPtr = NULL;

	return cond;
}