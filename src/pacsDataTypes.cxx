/**
 * \file pacsDataTypes.cxx
 * \brief File per la definizione dei dati secondo lo standard DICOM per la comunicazione con i PACS
 * \author ICAR-CNR Napoli
 */

//
// This document defines the basic units of data holding for each of
// the four levels of the DICOM query structure.  These are defined
// so as to eliminate the need to search DCM_OBJECTs for data when the
// the data needed is smaller and more static.
//

#include "pacsDataTypes.h"

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


#include "treelistctrl.h"

// Funzione di utilità... sarebbe ideale incapsularla da qualche parte,
// perchè qui è al di fuori di ogni principio di ingegneria del software..
void FormatDicomDate(char *pOut, const char *pDateIn) 
{
	if(strlen(pDateIn) != 8)
	{
		CERR << "FormatDicomDate - pDate non contiene 8 caratteri" << endl;
		strcpy(pOut, "");
		return;
	}

	// Giorno
	pOut[0] = pDateIn[6];
	pOut[1] = pDateIn[7];

	// Separatore
	pOut[2] = '/';

	// Mese
	pOut[3] = pDateIn[4];
	pOut[4] = pDateIn[5];

	// Separatore
	pOut[5] = '/';

	// Anno
	pOut[6] = pDateIn[0];
	pOut[7] = pDateIn[1];
	pOut[8] = pDateIn[2];
	pOut[9] = pDateIn[3];

	// Fine stringa
	pOut[10] = '\0';
}
/*
static void FormatDicomTime(std::string& time) 
{
	unsigned int i = time.find(".");
	if(i != std::string::npos) 
	{
		time = time.substr(0, i);
	}

	if(time.size() != 6) 
	{
		return;
	}

	std::string h = time.substr(0,2);
	std::string m = time.substr(2, 2);
	std::string s = time.substr(4, 2);
	
	time = h + ":" + m + ":" + s;
}*/

//
// class PacsList
//
// PacsList ()
//  Default constructor.  Builds an empty list.
//
PacsList::PacsList ()
{
    hd = tl = NULL;
    len = 0;
}

//
// ~PacsList ()
//  Destructor
//
PacsList::~PacsList ()
{
    clear();
}

//
// clear ()
//  Deletes all elements in list
//
void PacsList::clear ()
{
    while (len > 0)
        remove(hd);
}

//
// add (*_node)
//  Adds _node to the end of the list
//
void PacsList::add (PacsNode *_node)
{
    if (_node != NULL) {                // Error checking
        _node->next = NULL;             // No elements following the last

        if (len == 0) {                 // If list is empty,
            hd = tl = _node;            // head == tail == _node
            _node->prev = NULL;         // No previous elements
        }
        else {                          // ..if list is not empty,
            tl->next    = _node;        // last _node's next is _node
            _node->prev = tl;           // new _node's prev is tail
            tl          = _node;        // tail is _node (new node)
        }
        len++;                          // increment the length
    }
}

//
// remove (*_node)
//  Removes an element from the list and frees the memory
//
void PacsList::remove (PacsNode *_node)
{
    if (_node != NULL) {                // Error checking
        if (_node == hd)                // Check for special case, head
            hd = _node->next;
        else
            _node->prev->next = _node->next;
            
        if (_node == tl)                // Check for special case, tail
            tl = _node->prev;
        else
            _node->next->prev = _node->prev;

        len--;                          // Adjust list length
        delete _node;                   // Free memory
    }
}

//
// length ()
//  Return length of list
//
int PacsList::length () const
{
    return len;
}

//
// head ()
//  Return head of list
//
PacsNode* PacsList::head () const
{
    return hd;
}

//
// tail ()
//  Return tail of list
//
PacsNode* PacsList::tail () const
{
    return tl;
}

//
// find (_desc)
//  Searches list for _node with given key value
//
PacsNode* PacsList::find (const PacsNode *_node) const
{
    PacsNode *ptr;

    ptr = hd;
    while(ptr != NULL && strcmp(ptr->key, _node->key) != 0)  // !! SC EVAL !!
        ptr = ptr->next;

    return ptr;
}

//
// swap()
// Swaps two pointers
//
PacsNode* PacsList::swap(PacsNode *_node, QueryLevel _qLev) {
	PacsNode *temp;
	if (_node != NULL) {                // Error checking
        if(_node != hd) 
			_node->prev->next = _node->next;
		else
			hd = _node->next;
		if(_node->next == tl)
			tl = _node;
		_node->next->prev = _node->prev;
		_node->prev = _node->next;
		if(_node->next->next != NULL) _node->next->next->prev = _node;
		temp = _node->next->next;
		_node->next->next = _node;
		_node->next = temp;
    }
	return _node->next;

}



//
// sortList()
// Sorts a list
//
void PacsList::sortList(QueryLevel _qLev) {
	PacsNode *ptr, *last;
	int flag;

	last = NULL;

	flag = 1;
	if(hd != NULL) {

		switch(_qLev) {

			case STUDY: {
				while (flag == 1) {
					ptr = hd;
					flag = 0;
					while (ptr != last) {
						if((ptr->next != NULL) && strcmp(ptr->desc5, (ptr->next)->desc5) > 0) {
							swap(ptr, _qLev);
							flag = 1;
						}
						else if((ptr->next != NULL) && strcmp(ptr->desc5, (ptr->next)->desc5) == 0 && strcmp(ptr->desc2, (ptr->next)->desc2) > 0) {
							swap(ptr, _qLev);
							flag = 1;
						}
						else if((ptr->next != NULL) && strcmp(ptr->desc5, (ptr->next)->desc5) == 0 && strcmp(ptr->desc2, (ptr->next)->desc2) == 0 && strcmp(ptr->desc1, (ptr->next)->desc1) > 0) {
							swap(ptr, _qLev);
							flag = 1;
						}
						ptr = ptr->next;
					}
					//last = ptr;
				}
			}
			break;

			case SERIES: {
				while (flag == 1) {
					ptr = hd;
					flag = 0;
					while (ptr != last) {
						if((ptr->next != NULL) && strcmp(ptr->desc1, (ptr->next)->desc1) > 0) {
							swap(ptr, _qLev);
							flag = 1;
						}
						else if((ptr->next != NULL) && strcmp(ptr->desc1, (ptr->next)->desc1) == 0 && strcmp(ptr->desc2, (ptr->next)->desc2) > 0) {
							swap(ptr, _qLev);
							flag = 1;
						}
						ptr = ptr->next;
					}
					//last = ptr;
				}
			}
			break;
		}
	}
}


//
// display (_browser)
//  Displays all PacsData elements in list to a given browser
//
void PacsList::display(wxTreeListCtrl* _browser, QueryLevel _qLev) 
{
	// Dobbiamo un attimo rivedere la sortList, perchè fa uso dei campi "desc"
	//sortList(_qLev);

	switch(_qLev) 
	{
//		char* s;
		case STUDY: 
			{
			wxTreeItemId rootItem = _browser->GetRootItem();
			if(!rootItem.IsOk())
			{
				rootItem = _browser->AddRoot (_T("Root"));
			}

			for (PacsNode *ptr = hd; ptr != NULL; ptr = ptr->next) 
			{
				char szTemp[16];	// Un pochino piu grande del necessario..
				StudyData *pCurr = (StudyData*)ptr;

				wxTreeItemId studyItem = _browser->AppendItem(rootItem, _T((strlen(pCurr->patName) > 0) ? pCurr->patName : "N/A"));	
								
				// Patient birthdate
				if(strlen(pCurr->patBirthdate) > 0)
				{
					FormatDicomDate(szTemp, pCurr->patBirthdate);
					_browser->SetItemText (studyItem, 1, _T(szTemp));
				}else
					_browser->SetItemText (studyItem, 1, _T("N/A"));

				// Study ID
				_browser->SetItemText (studyItem, 2, _T((strlen(pCurr->id) > 0) ? pCurr->id : "N/A"));	

				// Study Date
				if(strlen(pCurr->date) > 0)
				{
					FormatDicomDate(szTemp, pCurr->date);
					_browser->SetItemText (studyItem, 3, _T(szTemp));
				}else
					_browser->SetItemText (studyItem, 3, _T("N/A"));

				_browser->SetItemText (studyItem, 4, _T((strlen(pCurr->modality) > 0) ? pCurr->modality : "N/A"));
				_browser->SetItemText (studyItem, 5, _T((strlen(pCurr->numImages) > 0) ? pCurr->numImages : ""));

				// Associamo la struttura dati all'elemento della riga
				// NB: Ho dovuto modificare la treelistrctrl..
				_browser->SetItemDataPtr(studyItem, (void*)ptr);

				// Inseriamo un children fittizio, in modo da permettere l'espansione della riga, 
				// che poi inoltrerà una nuova query al PACS
				_browser->AppendItem(studyItem, _T("Querying PACS server for series..."));
			}
		}
		break;
		case SERIES: 
			{
			wxTreeItemId rootItem = _browser->GetSelection();
			if(!rootItem.IsOk())
			{
				rootItem = _browser->AddRoot (_T("Root"));
			}

			StudyData *pParent = (StudyData*)_browser->GetItemDataPtr(rootItem);
			int seriesId = 0;
			for (PacsNode *ptr = hd; ptr != NULL; ptr = ptr->next) 
			{
				SeriesData *pCurr = (SeriesData*)ptr;
				
				if(!pParent || !pCurr->BelongsToStudy(pParent))
					continue;

				seriesId++;

				/*
				// Calcoliamo il numero delle immagini totale dello studio "padre"
				// 1 - Leggiamo dal record dello studio quante immagini ci sono
				// 2 - Incrementiamo il valore con il numero di immagini presenti in questa serie
				// 3 - Salviamo il nuovo numero nel record del tree list ctrl
				long lSeriesImages = (strlen(pCurr->imagesNumber) > 0) ? atol(pCurr->imagesNumber) : 0L;
				long lTotalStudyImages = 0L;

				wxString szImgsNo = _browser->GetItemText(rootItem, 5);
				szImgsNo.ToLong(&lTotalStudyImages);

				szImgsNo = wxString::Format("%li", lTotalStudyImages + lSeriesImages);
				_browser->SetItemText(rootItem, 5, szImgsNo);*/

				// Nel campo "Name" visualizziamo la descrizione della serie, se questa
				// non è presente allora il body part, altrimenti "Series i"
				wxString szName;
				if(strlen(pCurr->description) > 0)
					szName = wxString::Format("%s", pCurr->description);
				else if(strlen(pCurr->bodyPart) > 0)
					szName = wxString::Format("%s", pCurr->bodyPart);
				else
					szName = wxString::Format("Series %i", seriesId);

				wxTreeItemId item = _browser->AppendItem(rootItem, szName);
				_browser->SetItemText (item, 4, _T((strlen(pCurr->modality) > 0) ? pCurr->modality : "N/A"));
				_browser->SetItemText (item, 5, _T((strlen(pCurr->imagesNumber) > 0) ? pCurr->imagesNumber : "N/A"));

				// Associamo la struttura dati all'elemento della riga
				// NB: Ho dovuto modificare la treelistrctrl..
				_browser->SetItemDataPtr(item, (void*)ptr);
			}
		}
		break;
	}
/*
	// Alterniamo il colore dei records. Ho provato a farlo nel treelistctrl
	// ma la colorazione viene con dei "glitches" dovuti alla selezione..
	// In realtà coloriamo solo i figli di root, non i figli dei figli. 
	// Dovremmo usare una funzione ricorsiva altrimenti..
	bool bAlternate = false;

	if(!_browser->GetRootItem())
		return;

	wxTreeItemIdValue cookie;
	wxTreeItemId subItem = _browser->GetFirstChild(_browser->GetRootItem(), cookie);

	while(subItem)
	{
		// Alterniamo il colore dei records..
		_browser->SetItemBackgroundColour(subItem, (bAlternate) ? wxColor(218,254,253) : wxColor(255,255,255));

		subItem = _browser->GetNextChild (_browser->GetRootItem(), cookie);
		
		bAlternate = !bAlternate;
	}*/

	_browser->DoAlternateRowColors();
}

//
// class PacsNode
//
// PacsNode ()
//  Default constructor.  Merely sets fields to NULL.
//
PacsNode::PacsNode () {
    key = desc1 = desc2 = desc3 = NULL;
    next = prev = NULL;
}

PacsNode::~PacsNode () { }

//
// class FileData
//
// FileData ()
//  Default constructor.  Sets key and desc to name.
//
FileData::FileData () {
    key = desc1 = name;
}

//
// FileData (_name)
//  Initializes to given name.
//
FileData::FileData (char *_name) {
    strcpy(name, _name);
    key = desc1 = name;
}

//
// class PacsData
//
// PacsData (reqSize, optSize)
//  Allocates memory for the two internal DCM_ELEMENT lists
//
PacsData::PacsData (int _reqSize, int _optSize)
{
/*    reqSize = _reqSize;     reqList = new DCM_ELEMENT[_reqSize];
    optSize = _optSize;     optList = new DCM_FLAGGED_ELEMENT[_optSize];

    // Set 1 at bit position corresponding to elements 0.._optSize
    optAttr = (1 << _optSize) - 1;

    // Set the bitmask of each flagged element in optList to point to
    // bit position corresponding to its index in optList
    for (int i = 0; i < optSize; i++) {
        optList[i].flag        = (1 << i);
        optList[i].flagAddress = &optAttr;
    }
    */
    // The fileList is a list of dirent structs.  This list contains
    // filenames related to the given data object.  Ie, all files 
    // corresponding to patient 1178394.  Note that this is set up 
    // independantly of these classes.
    files = NULL;
}

//
// ~PacsData ()
//  Destructor -- deletes all memory associated with this class
//
PacsData::~PacsData ()
{
//    delete [] reqList;
  //  delete [] optList;
    if (files)
        delete files;
}

//
// display ()
//  Displays data contained within to stdout
//
void PacsData::display () const
{
//    int i;

/*    for (i = 0; i < reqSize; i++) 
        std::cout << reqList[i].description << ": "
             << reqList[i].d.string << std::endl;

    for (i = 0; i < optSize; i++)
        std::cout << optList[i].e.description << ": "
             << optList[i].e.d.string << std::endl;*/
}

//
// display (browser)
//  Displays data contained within to a browser display
//
/*wxArrayString* PacsData::display (wxPacsListBox *browser) const
{
   int i;
    char outStr[2*E_LEN];
	char date[11];
	
	wxArrayString *arrayReq = new wxArrayString();
	for (i = 0; i < reqSize; i++) {

		//if((strcmp(reqList[i].description,"Study Date") == 0) || (strcmp(reqList[i].description,"Patient Birthdate") == 0)){
		if((strcmp(reqList[i].description, "Patient Birthdate") == 0) && (strcmp(reqList[i].d.string, "") != 0)) {
			date[0]=reqList[i].d.string[6];
			date[1]=reqList[i].d.string[7];
			date[2]='/';
			date[3]=reqList[i].d.string[4];
			date[4]=reqList[i].d.string[5];
			date[5]='/';
			date[6]=reqList[i].d.string[0];
			date[7]=reqList[i].d.string[1];
			date[8]=reqList[i].d.string[2];
			date[9]=reqList[i].d.string[3];
			date[10]='\0';
			sprintf(outStr, "%s: %s", reqList[i].description, date);
		}

        else 
			sprintf(outStr, "%s: %s", reqList[i].description, reqList[i].d.string);

		arrayReq->Add(outStr);
    }

	browser->InsertItems(*arrayReq, 0);
	delete arrayReq;

	wxArrayString *arrayOpt =new wxArrayString();
    for (i = 0; i < optSize; i++) {
		if(((strcmp(optList[i].e.description, "Study Date") == 0) || (strcmp(optList[i].e.description, "Patient Birthdate") == 0)) && (strcmp(optList[i].e.d.string, "") != 0)) {
		//if((strcmp(optList[i].e.description, "Patient Birthdate") == 0) && (strcmp(optList[i].e.d.string, "") != 0)) {
			char date[11];
			date[0]=(optList[i].e.d.string)[6];
			date[1]=(optList[i].e.d.string)[7];
			date[2]='/';
			date[3]=(optList[i].e.d.string)[4];
			date[4]=(optList[i].e.d.string)[5];
			date[5]='/';
			date[6]=(optList[i].e.d.string)[0];
			date[7]=(optList[i].e.d.string)[1];
			date[8]=(optList[i].e.d.string)[2];
			date[9]=(optList[i].e.d.string)[3];
			date[10]='\0';
			optList[i].e.d.string = date;
		}
        sprintf(outStr, "%s: %s", optList[i].e.description,
                                  optList[i].e.d.string);
		arrayOpt->Add(outStr);
    }
	browser->InsertItems(*arrayOpt, browser->GetCount());
	delete arrayOpt;

	wxArrayString *array = new wxArrayString();
	array->Add(reqList[2].d.string);   // Patient Name
	array->Add(optList[0].e.d.string); // Modality
	array->Add(reqList[0].d.string);   // Study Instance UID
	array->Add(reqList[1].d.string);   // Series Instance UID

	return array;
}*/

//
// class PatientData
//
// PatientData ()
//  Default constructor.  Calls PacsData() to set up element list,
//  then modifies the elements accordingly.
//
PatientData::PatientData () : PacsData(2,2)
{
/*    buildElement(&reqList[0], DCM_PATNAME, "Patient Name", name);
    buildElement(&reqList[1], DCM_PATID, "Patient ID", id);

    buildElement(&optList[0].e, DCM_PATSEX,       "Sex",       sex);
    buildElement(&optList[1].e, DCM_PATBIRTHDATE, "Birthdate", birthdate);

    // Description of this object will be patient name and the
    // key value is the patient id
    desc1 = name;
    key  = id;*/
	
	// Ripuliamo gli oggetti
	strcpy(name, "");
	strcpy(id, "");
	strcpy(sex, "");
	strcpy(birthdate, "");

    // This is a patient level object
    qLev = PATIENT;
}

void PatientData::ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly) const
{
	if(!pDataset)
	{
		CERR << "PatientData::ExportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}

	// Obbligatori
	pDataset->putAndInsertString(DCM_PatientsName, name);
	pDataset->putAndInsertString(DCM_PatientID, id);

	// Facoltativi
	if(!bUseRequiredAttribsOnly)
	{
		pDataset->putAndInsertString(DCM_PatientsSex, sex);
		pDataset->putAndInsertString(DCM_PatientsBirthDate, birthdate);
	}
}

void PatientData::ImportDicomDataset(DcmDataset *pDataset)
{
	if(!pDataset)
	{
		CERR << "PatientData::ImportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}
	
	const char *pTmp = NULL;

	if(pDataset->findAndGetString(DCM_PatientsName, pTmp).bad())
	{
		CERR << "PatientData::ImportDicomDataset - findAndGetString (DCM_PatientsName) error" << endl;
	}

	// Forse possiamo fare in un modo meno pesante questa cosa?
	strncpy(name, pTmp, MIN(strlen(pTmp), E_LEN));

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_PatientID, pTmp).bad())
	{
		CERR << "PatientData::ImportDicomDataset - findAndGetString (DCM_PatientID) error" << endl;
	}

	strncpy(id, pTmp, MIN(strlen(pTmp), E_LEN));

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_PatientsSex, pTmp).bad())
	{
		CERR << "PatientData::ImportDicomDataset - findAndGetString (DCM_PatientsSex) error" << endl;
	}

	strncpy(sex, pTmp, MIN(strlen(pTmp), E_LEN));

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_PatientsBirthDate, pTmp).bad())
	{
		CERR << "PatientData::ImportDicomDataset - findAndGetString (DCM_PatientsBirthDate) error" << endl;
	}

	strncpy(birthdate, pTmp, MIN(strlen(pTmp), E_LEN));
}

//
// class StudyData
//
// StudyData ()
//  Default constructor.  Calls PacsData() to set up element list,
//  then modifies the elements accordingly.
//
StudyData::StudyData () : PacsData(4,8)
{
  /*  buildElement(&reqList[0], DCM_PATNAME, "Patient Name", patName);
    buildElement(&reqList[1], DCM_PATBIRTHDATE, "Patient Birthdate", patBirthdate);
    buildElement(&reqList[2], DCM_IDSTUDYDATE, "Study Date", date);
    buildElement(&reqList[3], DCM_RELSTUDYINSTANCEUID, "Study Instance UID", instanceUID);

    buildElement(&optList[0].e, DCM_PATID, "Patient ID", patID);
    buildElement(&optList[1].e, DCM_PATSEX, "Patient Sex", patSex);
    buildElement(&optList[2].e, DCM_RELSTUDYID,  "Study ID",   id);
    buildElement(&optList[3].e, DCM_IDACCESSIONNUMBER, "Study Accession Number", accessionNumber);
    buildElement(&optList[4].e, DCM_IDSTUDYDESCRIPTION, "Description", description);
    buildElement(&optList[5].e, DCM_IDREFERRINGPHYSICIAN, "Referring Physician", referringPhysician);
    buildElement(&optList[6].e, DCM_RELNUMBERSTUDYRELATEDIMAGES, "Study Related Images", numImages);
    buildElement(&optList[7].e, DCM_IDMODALITIESINSTUDY, "Modality", modality);

	// We will use the date to describe this object and the
    // study instance UID to be its key value;
	desc1 = date;
	desc2 = patBirthdate;
	desc3 = patSex;
	desc4 = modality;
	desc5 = patName;
    key  = instanceUID;
*/
	strcpy(patName, "");
	strcpy(patBirthdate, "");
	strcpy(date, "");
	strcpy(instanceUID, "");
	
	strcpy(patID, "");
	strcpy(patSex, "");
	strcpy(id, "");
	strcpy(accessionNumber, "");
	strcpy(description, "");
	strcpy(referringPhysician, "");
	strcpy(numImages, "");
	strcpy(modality, "");

    // This is a study level object
    qLev = STUDY;
}

void StudyData::ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly) const
{
	if(!pDataset)
	{
		CERR << "StudyData::ExportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}

	// Obbligatori
	pDataset->putAndInsertString(DCM_StudyInstanceUID, instanceUID);

	// Facoltativi
	if(!bUseRequiredAttribsOnly)
	{
		pDataset->putAndInsertString(DCM_PatientsName, patName);
		pDataset->putAndInsertString(DCM_PatientsBirthDate, patBirthdate);
		pDataset->putAndInsertString(DCM_StudyDate, date);

		pDataset->putAndInsertString(DCM_PatientID, patID);
		pDataset->putAndInsertString(DCM_PatientsSex, patSex);
		pDataset->putAndInsertString(DCM_StudyID, id);
		pDataset->putAndInsertString(DCM_AccessionNumber, accessionNumber);
		pDataset->putAndInsertString(DCM_StudyDescription, description);
		pDataset->putAndInsertString(DCM_ReferringPhysiciansName, referringPhysician);
		pDataset->putAndInsertString(DCM_NumberOfStudyRelatedInstances, numImages);		// DCM_RELNUMBERSTUDYRELATEDIMAGES ??
		pDataset->putAndInsertString(DCM_ModalitiesInStudy, modality);

	}
}

void StudyData::ImportDicomDataset(DcmDataset *pDataset)
{
	if(!pDataset)
	{
		CERR << "StudyData::ImportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}
	
	// *** OBBLIGATORI ***
	const char *pTmp = NULL;
	if(pDataset->findAndGetString(DCM_PatientsName, pTmp).good() && pTmp)
	{
		strcpy(patName, pTmp);	// Usiamo strncpy per evitare buffer overflow? Vedremo poi.. nd Alessio
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_PatientsName) error" << endl;

	if(pDataset->findAndGetString(DCM_PatientsBirthDate, pTmp).good() && pTmp)
	{
		strcpy(patBirthdate, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_PatientsBirthDate) error" << endl;

	if(pDataset->findAndGetString(DCM_StudyDate, pTmp).good() && pTmp)
	{
		strcpy(date, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_StudyDate) error" << endl;

	if(pDataset->findAndGetString(DCM_StudyInstanceUID, pTmp).good() && pTmp)
	{
		strcpy(instanceUID, pTmp);		
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_StudyInstanceUID) error" << endl;


	// *** FACOLTATIVI ***
	if(pDataset->findAndGetString(DCM_PatientID, pTmp).good() && pTmp)
	{
		strcpy(patID, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_PatientID) error" << endl;


	if(pDataset->findAndGetString(DCM_PatientsSex, pTmp).good() && pTmp)
	{
		strcpy(patSex, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_PatientsSex) error" << endl;

	if(pDataset->findAndGetString(DCM_StudyID, pTmp).good() && pTmp)
	{
		strcpy(id, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_StudyID) error" << endl;

	if(pDataset->findAndGetString(DCM_AccessionNumber, pTmp).good() && pTmp)
	{
		strcpy(accessionNumber, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_AccessionNumber) error" << endl;

	if(pDataset->findAndGetString(DCM_StudyDescription, pTmp).good() && pTmp)
	{
		strcpy(description, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_StudyDescription) error" << endl;

	if(pDataset->findAndGetString(DCM_ReferringPhysiciansName, pTmp).good() && pTmp)
	{
		strcpy(referringPhysician, pTmp);		
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_ReferringPhysiciansName) error" << endl;

	if(pDataset->findAndGetString(DCM_NumberOfStudyRelatedInstances, pTmp).good() && pTmp)	// Prima usavo DCM_ACR_NEMA_ImagesInStudy
	{
		strcpy(numImages, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_NumberOfStudyRelatedInstances) error" << endl;

	if(pDataset->findAndGetString(DCM_ModalitiesInStudy, pTmp).good() && pTmp)
	{
		strcpy(modality, pTmp);
		pTmp = NULL;
	}else
		CERR << "StudyData::ImportDicomDataset - findAndGetString (DCM_ModalitiesInStudy) error" << endl;

}

//
// class SeriesData
//
// SeriesData ()
//  Default constructor.  Calls PacsData() to set up element list,
//  then modifies the elements accordingly.
//
SeriesData::SeriesData () : PacsData(3,3)
{
  /*  buildElement(&reqList[0], DCM_RELSTUDYINSTANCEUID, "Study Instance UID", studyInstanceUID);
    buildElement(&reqList[1], DCM_RELSERIESINSTANCEUID, "Series Instance UID", instanceUID);
    buildElement(&reqList[2], DCM_PATNAME, "Patient Name", patName);

	buildElement(&optList[0].e, DCM_IDMODALITY, "Modality", modality);
    buildElement(&optList[1].e, DCM_ACQBODYPARTEXAMINED, "Body Part", bodyPart);
    buildElement(&optList[2].e, DCM_RELNUMBERSERIESRELATEDINST, "Number of images", imagesNumber);

    // The modality will describe this object while the 
    // series instance UID will be its key field
    desc1 = modality;
    desc2 = imagesNumber;
    key  = instanceUID;*/

	strcpy(studyInstanceUID, "");
	strcpy(instanceUID, "");
	strcpy(patName, "");

	strcpy(modality, "");
	strcpy(bodyPart, "");
	strcpy(imagesNumber, "");
	strcpy(description, "");

    // This is a series level object
    qLev = SERIES;
}

void SeriesData::ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly) const
{
	if(!pDataset)
	{
		CERR << "SeriesData::ExportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}

	// Obbligatori
	pDataset->putAndInsertString(DCM_StudyInstanceUID, studyInstanceUID);
	pDataset->putAndInsertString(DCM_SeriesInstanceUID, instanceUID);
	
	// Facoltativi
	if(!bUseRequiredAttribsOnly)
	{
		pDataset->putAndInsertString(DCM_PatientsName, patName);

		pDataset->putAndInsertString(DCM_Modality, modality);
		pDataset->putAndInsertString(DCM_BodyPartExamined, bodyPart);
		pDataset->putAndInsertString(DCM_NumberOfSeriesRelatedInstances, imagesNumber);
		pDataset->putAndInsertString(DCM_SeriesDescription, description);
	}
}

void SeriesData::ImportDicomDataset(DcmDataset *pDataset)
{
	if(!pDataset)
	{
		CERR << "SeriesData::ImportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}

	// *** OBBLIGATORI ***
	const char *pTmp = NULL;
	if(pDataset->findAndGetString(DCM_StudyInstanceUID, pTmp).good() && pTmp)
	{
		strcpy(studyInstanceUID, pTmp);	
		pTmp = NULL;
	}else
		CERR << "SeriesData::ImportDicomDataset - findAndGetString (DCM_StudyInstanceUID) error" << endl;

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_SeriesInstanceUID , pTmp).good() && pTmp)
	{
		strcpy(instanceUID, pTmp);	
		pTmp = NULL;
	}else
		CERR << "SeriesData::ImportDicomDataset - findAndGetString (DCM_SeriesInstanceUID ) error" << endl;

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_PatientsName , pTmp).good() && pTmp)
	{
		strcpy(patName, pTmp);	
		pTmp = NULL;
	}else
		CERR << "SeriesData::ImportDicomDataset - findAndGetString (DCM_PatientsName ) error" << endl;

	// *** FACOLTATIVI ***
	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_Modality, pTmp).good() && pTmp)
	{
		strcpy(modality, pTmp);	
		pTmp = NULL;
	}else
		CERR << "SeriesData::ImportDicomDataset - findAndGetString (DCM_Modality) error" << endl;

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_BodyPartExamined, pTmp).good() && pTmp)
	{
		strcpy(bodyPart, pTmp);	
		pTmp = NULL;
	}else
		CERR << "SeriesData::ImportDicomDataset - findAndGetString (DCM_BodyPartExamined) error" << endl;

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_NumberOfSeriesRelatedInstances, pTmp).good() && pTmp)
	{
		strcpy(imagesNumber, pTmp);	
		pTmp = NULL;
	}else
		CERR << "SeriesData::ImportDicomDataset - findAndGetString (DCM_NumberOfSeriesRelatedInstances) error" << endl;

	pTmp = NULL;
	if(pDataset->findAndGetString(DCM_SeriesDescription, pTmp).good() && pTmp)
	{
		strcpy(description, pTmp);	
		pTmp = NULL;
	}else
		CERR << "SeriesData::ImportDicomDataset - findAndGetString (DCM_SeriesDescription) error" << endl;
}

bool SeriesData::BelongsToStudy(StudyData *pParent)
{
	if(!pParent)
		return false;

	if(strcmp(studyInstanceUID, pParent->instanceUID))	
		return false;	// Gli Study Instance UID non coincidono

	if(strcmp(patName, pParent->patName))
		return false;	// I PatientsName non coincidono..

	return true;
}

//
// class ImageData
//
// ImageData ()
//  Default constructor.  Calls PacsData() to set up element list,
//  then modifies the elements accordingly.
//
ImageData::ImageData () : PacsData(4,4)
{
 /*   buildElement(&reqList[0], DCM_PATNAME, "Patient Name", patID);
    buildElement(&reqList[1], DCM_RELSTUDYINSTANCEUID,
                 "Study Instance UID", studyInstanceUID);
    buildElement(&reqList[2], DCM_RELSERIESINSTANCEUID,
                 "Series Instance UID", seriesInstanceUID);
    buildElement(&reqList[3], DCM_IDSOPINSTANCEUID,
                 "SOP Instance UID", sopInstanceUID);

    buildElement(&optList[0].e, DCM_RELIMAGENUMBER, "Image Number", number);
    buildElement(&optList[1].e, DCM_RELIMAGECOMMENTS, "Comments", comments);
    buildElement(&optList[2].e, DCM_ACQSLICETHICKNESS,
                 "Slice Thickness", sliceThickness);
    buildElement(&optList[3].e, DCM_ACQSLICETHICKNESS,
                 "Slice Spacing", sliceSpacing);

    // Describing element is image number, key value is the SOP instance UID
    desc1 = number;
    key  = sopInstanceUID;
*/
    // This is an image level object
    qLev = IMAGE;
}

void ImageData::ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly) const
{
	if(!pDataset)
	{
		CERR << "ImageData::ExportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}

}

void ImageData::ImportDicomDataset(DcmDataset *pDataset)
{
	if(!pDataset)
	{
		CERR << "ImageData::ImportDicomDataset - Puntatore pDataset non valido!" << endl;
		return;
	}

}
