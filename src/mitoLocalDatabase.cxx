/**
 * \file mitoLocalDatabase.cpp
 * \brief Questa classe gestisce il database di immagini locale
 * \author ICAR-CNR Napoli
 * \todo Creare una classe separata per le funzioni DICOMDIR?
 */

#include "mitoLocalDatabase.h"
#include "mitoDefinitions.h"
#include "treelistctrl.h"

#include <wx/arrstr.h>
#include <wx/xml/xml.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/filename.h>
#include <wx/string.h>

#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdicdir.h>

// TEMP: Definita in pacsDataType.cxx
void FormatDicomDate(char *pOut, const char *pDateIn);

mitoLocalDatabase::mitoLocalDatabase()
{
	m_localDatabase.clear();
}

mitoLocalDatabase::~mitoLocalDatabase()
{
	list<wxArrayString*>::iterator iteList = m_localDatabase.begin();

	while (iteList != m_localDatabase.end()) 
	{
		wxArrayString *pData = *iteList;
		if (pData) 
		{
			pData->Clear();

			delete pData;
		}

		++iteList;
	}

	m_localDatabase.clear();
}

bool mitoLocalDatabase::AddFileToDatabase(wxArrayString *pInfo)
{
	if(!pInfo)
		return false;

	m_localDatabase.push_back(pInfo);

	return true;
}

bool mitoLocalDatabase::RemoveFromDatabase(wxArrayString *pInfo, bool bDeleteWholeStudy, bool bPhysicallyRemove)
{
	if(!pInfo)
		return false;

	wxString studyUID = pInfo->Item(MITO_STUDY_UID);
	wxString seriesUID = pInfo->Item(MITO_SERIES_UID);
	wxString sFilePath;
	
	list<wxArrayString*>::iterator iteList = m_localDatabase.begin();

	while (iteList != m_localDatabase.end()) 
	{
		wxArrayString *pData = *iteList;
		wxASSERT(pData);

		// Siamo sicuri che basti un confronto a livelli di UID
		// per garantire l'ugualianza?
		bool bSameStudy = (pData->Item(MITO_STUDY_UID) == studyUID);
		bool bSameSeries = (pData->Item(MITO_SERIES_UID) == seriesUID);

		// Cancelliamo il nodo/file solo nel caso lo studyUID sia
		// lo stesso
		if( (bDeleteWholeStudy && bSameStudy) 
			|| (!bDeleteWholeStudy && bSameSeries))
		{
			iteList = m_localDatabase.erase( iteList ) ;

			// Eliminiamo il file dall'HDD
			if(bPhysicallyRemove)
			{
				sFilePath = pData->Item(MITO_SAVEPATH);
				unlink(sFilePath);
			}

			delete pData;
			continue;
		}

		++iteList;
	}

	if(bDeleteWholeStudy && bPhysicallyRemove)
	{
		//wxString filePath((const char *)pInfo->Item(18));
		wxFileName dirName = wxFileName::DirName(sFilePath);
		dirName.Normalize();
		
		if(!dirName.Rmdir())
		{
			// Impossibile rimuovere la cartella..
		}
	}

	return true;
}

wxArrayString *mitoLocalDatabase::GetFilePaths(wxArrayString *pInfo, bool bWholeStudy)
{
	if(!pInfo)
		return NULL;

	wxString studyUID = pInfo->Item(MITO_STUDY_UID);
	wxString seriesUID = pInfo->Item(MITO_SERIES_UID);
	
	list<wxArrayString*>::iterator iteList = m_localDatabase.begin();

	wxArrayString *pRetArray = new wxArrayString();

	while (iteList != m_localDatabase.end()) 
	{
		wxArrayString *pData = *iteList;
		wxASSERT(pData);

		// Siamo sicuri che basti un confronto a livelli di UID
		// per garantire l'ugualianza?
		bool bSameStudy = (pData->Item(MITO_STUDY_UID) == studyUID);
		bool bSameSeries = (pData->Item(MITO_SERIES_UID) == seriesUID);

		// Preleviamo il percorso del file solo nel caso lo studyUID sia
		// lo stesso
		if( (bWholeStudy && bSameStudy) 
			|| (!bWholeStudy && bSameSeries))
		{
			pRetArray->Add(pData->Item(MITO_SAVEPATH));
		}

		++iteList;
	}

	return pRetArray;
}

bool mitoLocalDatabase::SaveIndexToFile(const char *pszFileName)
{
	wxString tmpPropValue;
	wxXmlDocument xmlToSave;
	wxString sLastStudyUID;
	wxString sLastSeriesUID;
	wxXmlNode *pLastStudy = NULL;
	wxXmlNode *pLastSeries = NULL;
	wxXmlNode *pRoot = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("database"));
	if(!pRoot)
		return false;

	// Il numero di immagini non è presente tra i tag del singolo file,
	// dobbiamo conteggiarle manualmente..
	int iSeriesImgs = 0;
	int iStudyImgs = 0;

	// Set encoding
	xmlToSave.SetEncoding(wxT("utf-8"));
	xmlToSave.SetFileEncoding(wxT("utf-8"));

	xmlToSave.SetRoot(pRoot);

	list<wxArrayString*>::iterator iteList = m_localDatabase.begin();

	while (iteList != m_localDatabase.end()) 
	{
		wxArrayString *pData = *iteList;
		if (!pData) 
			return false;

		// Creiamo la struttura del file indice XML
		if(sLastStudyUID != pData->Item(MITO_STUDY_UID))
		{
			// Abbiamo un nuovo studio, creiamo un nuovo nodo
			sLastStudyUID = pData->Item(MITO_STUDY_UID);

			// Salviamo il numero di immagini dello studio precedente (se non è il primo e l'ultimo questo)
			if(pLastStudy)
			{
				pLastStudy->AddProperty(wxT("images"), wxString::Format("%i", iStudyImgs));
			}

			pLastStudy = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("study"));
			if(!pLastStudy)
				return false;

			pLastStudy->AddProperty(wxT("uid"),		wxT(sLastStudyUID));

			pLastStudy->AddProperty(wxT("patName"), wxT(pData->Item(MITO_PATIENT_NAME)));
			pLastStudy->AddProperty(wxT("patID"),	wxT(pData->Item(MITO_PATIENT_ID)));
			pLastStudy->AddProperty(wxT("patAge"),	wxT(pData->Item(MITO_PATIENT_AGE)));
			pLastStudy->AddProperty(wxT("patBirthdate"),	wxT(pData->Item(MITO_PATIENT_BIRTHDATE)));

			pLastStudy->AddProperty(wxT("accession"),	wxT(pData->Item(MITO_ACCESSION)));
			pLastStudy->AddProperty(wxT("description"),	wxT(pData->Item(MITO_STUDY_DESCRIPTION)));
			pLastStudy->AddProperty(wxT("modality"),	wxT(pData->Item(MITO_STUDY_MODALITY)));
			pLastStudy->AddProperty(wxT("date"),		wxT(pData->Item(MITO_STUDY_DATE)));
			
			//pLastStudy->AddProperty(wxT("images"),		wxT(pData->Item(8)));
			
			pLastStudy->AddProperty(wxT("referring"),	wxT(pData->Item(MITO_REFERRING_PHYSICIAN)));
			pLastStudy->AddProperty(wxT("performing"),	wxT(pData->Item(MITO_PERFORMING_PHYSICIAN)));
			pLastStudy->AddProperty(wxT("institution"),	wxT(pData->Item(MITO_INSITUTION)));

			pRoot->AddChild(pLastStudy);

			iStudyImgs = 0;
		}
		
		if(pLastStudy)
		{
			// Stiamo salvando ancora lo studio precedente
			if(sLastSeriesUID != pData->Item(MITO_SERIES_UID))
			{
				// Nuova serie
				sLastSeriesUID = pData->Item(MITO_SERIES_UID);

				// Salviamo il numero di immagini dello studio precedente (se non è il primo e l'ultimo questo)
				if(pLastSeries)
				{
					pLastSeries->AddProperty(wxT("images"), wxString::Format("%i", iSeriesImgs));
				}

				pLastSeries = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("series"));
				if(!pLastSeries)
					return false;

				pLastSeries->AddProperty(wxT("uid"), wxT(sLastSeriesUID));

				pLastSeries->AddProperty(wxT("bodypart"), wxT(pData->Item(MITO_BODYPART)));
				pLastSeries->AddProperty(wxT("description"), wxT(pData->Item(MITO_SERIES_DESCRIPTION)));
				pLastSeries->AddProperty(wxT("modality"), wxT(pData->Item(MITO_STUDY_MODALITY)));
				pLastSeries->AddProperty(wxT("date"), wxT(pData->Item(MITO_SERIES_DATE)));
				
				// Il numero di immagini non è presente tra i tag nel file, quindi dobbiamo
				// conteggiarli da soli..
				//pLastSeries->AddProperty(wxT("images"), wxT(pData->Item(12)));
				
				pLastSeries->AddProperty(wxT("referring"), wxT(pData->Item(MITO_REFERRING_PHYSICIAN)));
				pLastSeries->AddProperty(wxT("performing"), wxT(pData->Item(MITO_PERFORMING_PHYSICIAN)));
				pLastSeries->AddProperty(wxT("institution"), wxT(pData->Item(MITO_INSITUTION)));

				pLastStudy->AddChild(pLastSeries);

				iSeriesImgs = 0;
			}
			
			// Se abbiamo
			if(pLastSeries)
			{
				// Sono instanze appartenenti alla serie attuale
				wxXmlNode *pInstance = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("instance"));
				if(!pInstance)
					return false;

				pInstance->AddProperty(wxT("path"), wxT(pData->Item(MITO_SAVEPATH)));

				pLastSeries->AddChild(pInstance);

				if(!pLastSeries->GetPropVal(wxT("path"), tmpPropValue))
				{
					// Salviamo i percorsi delle serie
					wxString filePath((const char *)pData->Item(MITO_SAVEPATH));
					wxFileName dirName = wxFileName::DirName(filePath);
					dirName.RemoveLastDir();
					dirName.Normalize();

					pLastSeries->AddProperty(wxT("path"), dirName.GetPath());
				}

				iStudyImgs++;
				iSeriesImgs++;
			}
		}

		iteList++;
	}

	// Nel caso ci fosse un solo studio o una sola serie, salviamo qui il numero di immagini
	if(pLastStudy && pLastStudy->GetPropVal(wxT("images"), tmpPropValue))
	{
		pLastStudy->AddProperty(wxT("images"), wxString::Format("%i", iStudyImgs));
	}

	if(pLastSeries && pLastSeries->GetPropVal(wxT("images"), tmpPropValue))
	{
		pLastSeries->AddProperty(wxT("images"), wxString::Format("%i", iSeriesImgs));
	}

	return xmlToSave.Save(wxT(pszFileName));
}

bool mitoLocalDatabase::LoadIndexFromFile(const char *pszFileName, wxTreeListCtrl *pAlbum)
{
	wxASSERT(pszFileName && pAlbum);

	if(!pszFileName || !pAlbum)
		return false;

	wxXmlDocument xmlToLoad;

	// Per evitare il fastidioso warning "pszFileName" does not exist da parte di wxXmlDocument, controlliamo
	// autonomamente l'esistenza del file
	if(!wxFileName::FileExists(wxT(pszFileName)))
		return false;

	if(!xmlToLoad.Load(pszFileName))
		return false;

	if(xmlToLoad.GetRoot()->GetName() != wxT("database"))
		return false;

	// Effettuiamo il parsing del file..
	wxXmlNode *pChild = xmlToLoad.GetRoot()->GetChildren();
	while (pChild) 
	{
		// Per ora supportiamo solo elementi di tipo "study" a livello di root.
		// In futuro il formato del file potrebbe però cambiare..
		if (pChild->GetName() != wxT("study")) 
		{
			pChild = pChild->GetNext();
			continue;
		}

		if(!ParseStudy(pChild, pAlbum))
		{
			if(wxMessageBox("There was an error while parsing local database cache index. Do you want to skip the error and continue parsing?", "Warning", wxYES_NO | wxICON_EXCLAMATION ) == wxNO)
				return false;
		}

		pChild = pChild->GetNext();
	}

	return true;
}

bool mitoLocalDatabase::ImportDicomDir(const char *pszDirPath, wxTreeListCtrl *pAlbum)
{
	DcmDicomDir dir(pszDirPath);

	OFCondition ret = dir.error();
	if(ret.bad()) 
	{
		return false;
	}

	DcmDirectoryRecord root = dir.getRootRecord();
	DcmDirectoryRecord *rec = root.nextSub(NULL);
	
	while ( rec != NULL ) 
	{
		switch ( rec->getRecordType() ) 
		{
		case ERT_Patient:
			OnDicomDir_Patient(rec, pAlbum);
			break;
	
		case ERT_HangingProtocol:
			// FALLTHROUGH
		case ERT_Private:
			break;
		
		default:
			std::cout << "WARNING: Bad DICOMDIR Record type[" << rec->getRecordType() << "] found\n";
		}

		rec = root.nextSub(rec);
	}


	return true;
}

bool mitoLocalDatabase::ParseStudy(wxXmlNode *pStudy, wxTreeListCtrl *pAlbum)
{
	wxASSERT(pStudy && pAlbum);

	if(!pStudy || !pAlbum)
		return false;

	wxTreeItemId studyItem = AddStudyToAlbum(pStudy, pAlbum);

	// Verifichiamo se questo studio contiene qualche serie..
	wxXmlNode *pSeries = pStudy->GetChildren();
	while (pSeries) 
	{
		if (pSeries->GetName() != wxT("series")) 
		{
			pSeries = pSeries->GetNext();
			continue;
		}

		if(!ParseSeries(pSeries, pAlbum, studyItem))
		{
			if(wxMessageBox("There was an error while parsing local database cache index. Do you want to skip the error and continue parsing?", "Warning", wxYES_NO | wxICON_EXCLAMATION ) == wxNO)
				return false;
		}
	
		pSeries = pSeries->GetNext();
	}

	return true;
}

bool mitoLocalDatabase::ParseSeries(wxXmlNode *pSeries, wxTreeListCtrl *pAlbum, wxTreeItemId parentStudy)
{
	wxASSERT(pSeries && pAlbum);

	if(!pSeries || !pAlbum)
		return false;

	wxTreeItemId seriesItem = AddSeriesToAlbum(pSeries, pAlbum, parentStudy);

	// Verifichiamo se questo studio contiene qualche serie..
	wxXmlNode *pImage = pSeries->GetChildren();
	while (pImage) 
	{
		if (pImage->GetName() != wxT("instance")) 
		{
			pSeries = pSeries->GetNext();
			continue;
		}

		wxArrayString *pInfo = AddNodeToDatabase(pImage);
		if(!pInfo)
			return false;

		// Associamo un wxStringArray al seriesItem in modo
		// da avere sempre disponibili UID studio, paziente e serie
		// per le operazioni di SEND e Delete local cache
		if(!pAlbum->GetItemDataPtr(seriesItem))
		{
			pAlbum->SetItemDataPtr(seriesItem, pInfo);
		}

		if(!pAlbum->GetItemDataPtr(parentStudy))
		{
			// NB: Dobbiamo DUPLICARE la struttura pInfo
			// perchè nel caso in cui venga eliminato un nodo
			// dall'album, se utilizzassimo lo stesso pInfo
			// chiamando GetItemDataPtr potremmo ricevere un
			// puntatore corrottto

			// TODO: Sarebbe molto meglio usare per il motivo di sopra
			// una classe che porta il conto dei riferimenti.. in modo
			// che si autoelimini quando il refcount è < 1.. Il memory
			// leak presente nelle strutture dati è dovuto a questo hack.
			// Infatti pNewInfo non è nella cache locale, ma è riferita solo
			// dall'album.. quindi il distruttore non la elimina!
			wxArrayString *pNewInfo = new wxArrayString(*pInfo);
			if(pNewInfo)
				pAlbum->SetItemDataPtr(parentStudy, pNewInfo);
		}
	
		pImage = pImage->GetNext();
	}

	return true;
}

wxArrayString *mitoLocalDatabase::AddNodeToDatabase(wxXmlNode *pNode)
{
	wxASSERT(pNode);

	wxArrayString *pInfo = new wxArrayString();
	if(!pInfo)
		return NULL;

	wxString szValue;

	// Prendiamo le altre informazioni dall'elemento parent XML (Series)
	wxXmlNode *pParentSeries = pNode->GetParent();
	wxASSERT(pParentSeries);

	wxXmlNode *pParentStudy = pParentSeries->GetParent();
	wxASSERT(pParentStudy);	

	// Patient Name
	szValue = pParentStudy->GetPropVal(wxT("patName"), wxT(""));
	pInfo->Add(szValue);

	// Patient ID
	szValue = pParentStudy->GetPropVal(wxT("patID"), wxT(""));
	pInfo->Add(szValue);

	// Patient Birthdate
	szValue = pParentStudy->GetPropVal(wxT("patBirthdate"), wxT(""));
	pInfo->Add(szValue);

	// Patient Age
	szValue = pParentStudy->GetPropVal(wxT("patAge"), wxT(""));
	pInfo->Add(szValue);

	// Study UID
	szValue = pParentStudy->GetPropVal(wxT("uid"), wxT("xml-parsing-error"));
	pInfo->Add(szValue);

	// Study description
	szValue = pParentStudy->GetPropVal(wxT("description"), wxT(""));
	pInfo->Add(szValue);

	// Study Modality
	szValue = pParentStudy->GetPropVal(wxT("modality"), wxT(""));
	pInfo->Add(szValue);

	// Study date
	szValue = pParentStudy->GetPropVal(wxT("date"), wxT(""));
	pInfo->Add(szValue);

	// Study images
	szValue = pParentStudy->GetPropVal(wxT("images"), wxT(""));
	pInfo->Add(szValue);
	
	// Series UID
	szValue = pParentSeries->GetPropVal(wxT("uid"), wxT(""));
	pInfo->Add(szValue);

	// Body part
	szValue = pParentSeries->GetPropVal(wxT("bodypart"), wxT("Series"));
	pInfo->Add(szValue);

	// Series Description
	szValue = pParentSeries->GetPropVal(wxT("description"), wxT(""));
	pInfo->Add(szValue);

	// Series Images
	szValue = pParentSeries->GetPropVal(wxT("images"), wxT(""));
	pInfo->Add(szValue);

	// Series Date
	szValue = pParentSeries->GetPropVal(wxT("date"), wxT(""));
	pInfo->Add(szValue);

	// Prendiamo le ultime informazioni dallo studio..

	// Accession
	szValue = pParentStudy->GetPropVal(wxT("accession"), wxT(""));
	pInfo->Add(szValue);

	// Referring
	szValue = pParentStudy->GetPropVal(wxT("referring"), wxT(""));
	pInfo->Add(szValue);

	// Performing
	szValue = pParentStudy->GetPropVal(wxT("performing"), wxT(""));
	pInfo->Add(szValue);

	// Institution
	szValue = pParentStudy->GetPropVal(wxT("institution"), wxT(""));
	pInfo->Add(szValue);

	// Path
	szValue = pNode->GetPropVal(wxT("path"), wxT(""));
	pInfo->Add(szValue);

	if(!AddFileToDatabase(pInfo))
		wxASSERT(1);

	return pInfo;
}

wxTreeItemId mitoLocalDatabase::AddStudyToAlbum(wxXmlNode *pStudy, wxTreeListCtrl *pAlbum)
{
	wxASSERT(pStudy && pAlbum);

	// Verifichiamo che il tree list abbia l'elemento root, altrimenti creiamolo
	wxTreeItemId rootItem = pAlbum->GetRootItem();
	if(!rootItem.IsOk())
	{
		rootItem = pAlbum->AddRoot (_T("Root"));
	}

	wxString szValue;
	char szDateTemp[16];

	// Patient Name
	szValue = pStudy->GetPropVal(wxT("patName"), wxT(""));
	wxTreeItemId studyItem = pAlbum->AppendItem(rootItem, szValue);	

	// Birthdate
	szValue = pStudy->GetPropVal(wxT("patBirthdate"), wxT(""));
	FormatDicomDate(szDateTemp, szValue);
	pAlbum->SetItemText(studyItem, 1, szDateTemp);	

	// Modality
	szValue = pStudy->GetPropVal(wxT("modality"), wxT(""));
	pAlbum->SetItemText(studyItem, 2, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Description
	szValue = pStudy->GetPropVal(wxT("description"), wxT(""));
	pAlbum->SetItemText(studyItem, 3, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Patient ID
	szValue = pStudy->GetPropVal(wxT("patID"), wxT(""));
	pAlbum->SetItemText(studyItem, 4, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Patient Age
	szValue = pStudy->GetPropVal(wxT("patAge"), wxT(""));
	pAlbum->SetItemText(studyItem, 5, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Accession
	szValue = pStudy->GetPropVal(wxT("accession"), wxT(""));
	pAlbum->SetItemText(studyItem, 6, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Date
	szValue = pStudy->GetPropVal(wxT("date"), wxT(""));
	FormatDicomDate(szDateTemp, szValue);
	pAlbum->SetItemText(studyItem, 7, szDateTemp);	

	// Imm
	szValue = pStudy->GetPropVal(wxT("images"), wxT(""));
	pAlbum->SetItemText(studyItem, 8, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Referring
	szValue = pStudy->GetPropVal(wxT("referring"), wxT(""));
	pAlbum->SetItemText(studyItem, 9, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Performing
	szValue = pStudy->GetPropVal(wxT("performing"), wxT(""));
	pAlbum->SetItemText(studyItem, 10, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Institution
	szValue = pStudy->GetPropVal(wxT("institution"), wxT(""));
	pAlbum->SetItemText(studyItem, 11, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Path
	szValue = pStudy->GetPropVal(wxT("path"), wxT(""));
	pAlbum->SetItemText(studyItem, 12, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	return studyItem;
}

wxTreeItemId mitoLocalDatabase::AddSeriesToAlbum(wxXmlNode *pSeries, wxTreeListCtrl *pAlbum, wxTreeItemId parentStudy)
{
	wxASSERT(pSeries && pAlbum);

	if(!parentStudy.IsOk())
	{
		return NULL;
	}

	wxString szValue;
	char szDateTemp[16];

	// Body part
	szValue = pSeries->GetPropVal(wxT("bodypart"), wxT("Series"));
	wxTreeItemId seriesItem = pAlbum->AppendItem(parentStudy, (szValue != wxT("")) ? szValue : wxT("Series"));	

	// Empty (Birthdate)
	pAlbum->SetItemText(seriesItem, 1, wxT(""));	

	// Modality
	szValue = pSeries->GetPropVal(wxT("modality"), wxT(""));
	pAlbum->SetItemText(seriesItem, 2, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Description
	szValue = pSeries->GetPropVal(wxT("description"), wxT(""));
	pAlbum->SetItemText(seriesItem, 3, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// ID
	szValue = pSeries->GetPropVal(wxT("uid"), wxT(""));
	pAlbum->SetItemText(seriesItem, 4, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Empty
	//szValue = pSeries->GetPropVal(wxT("patAge"), wxT(""));
	pAlbum->SetItemText(seriesItem, 5, wxT(""));	

	// Empty
	//szValue = pSeries->GetPropVal(wxT("accession"), wxT(""));
	pAlbum->SetItemText(seriesItem, 6, wxT(""));	

	// Date
	szValue = pSeries->GetPropVal(wxT("date"), wxT(""));
	FormatDicomDate(szDateTemp, szValue);
	pAlbum->SetItemText(seriesItem, 7, szDateTemp);	

	// Imm
	szValue = pSeries->GetPropVal(wxT("images"), wxT(""));
	pAlbum->SetItemText(seriesItem, 8, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Referring
	szValue = pSeries->GetPropVal(wxT("referring"), wxT(""));
	pAlbum->SetItemText(seriesItem, 9, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Performing
	szValue = pSeries->GetPropVal(wxT("performing"), wxT(""));
	pAlbum->SetItemText(seriesItem, 10, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Institution
	szValue = pSeries->GetPropVal(wxT("institution"), wxT(""));
	pAlbum->SetItemText(seriesItem, 11, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	// Path
	szValue = pSeries->GetPropVal(wxT("path"), wxT(""));
	pAlbum->SetItemText(seriesItem, 12, (szValue != wxT("")) ? szValue : wxT("N/A"));	

	return seriesItem;
}

bool mitoLocalDatabase::OnDicomDir_Patient(DcmDirectoryRecord *patRec, wxTreeListCtrl *pAlbum)
{
	bool studyFound = false;

	DcmDirectoryRecord *subRec;

	wxASSERT(patRec->getRecordType()==ERT_Patient);
	
	subRec = patRec->nextSub(NULL);
	while ( subRec ) 
	{
		switch ( subRec->getRecordType() ) 
		{
		case ERT_Study:
			studyFound = OnDicomDir_Study(patRec, subRec, pAlbum);
			break;
		case ERT_Private:
			break;
		default:
			std::cout << "WARNING: Bad DICOMDIR SubRecord type[" << subRec->getRecordType() << "] for Patient found\n";
		}
		
		subRec = patRec->nextSub(subRec);
	}

	return studyFound;
}

bool mitoLocalDatabase::OnDicomDir_Study(DcmDirectoryRecord *patRec, DcmDirectoryRecord *studyRec, wxTreeListCtrl *pAlbum)
{
	// Verifichiamo che il tree list abbia l'elemento root, altrimenti creiamolo
	wxTreeItemId rootItem = pAlbum->GetRootItem();
	if(!rootItem.IsOk())
	{
		rootItem = pAlbum->AddRoot (_T("Root"));
	}

	bool seriesFound = false;

	DcmDirectoryRecord *subRec;

	wxASSERT(studyRec->getRecordType() == ERT_Study);
	
	subRec = studyRec->nextSub(NULL);
	while(subRec && !seriesFound) 
	{
		switch(subRec->getRecordType()) 
		{
		case ERT_Series:
			// Check if valid series (contains at least 1 image)
			seriesFound = true;

			if(!OnDicomDir_Series(patRec, studyRec, subRec, pAlbum))
				return false;

			break;
		case ERT_Private:
			break;
		default:
			std::cout << "WARNING: Bad DICOMDIR SubRecord type[" << subRec->getRecordType() << "] for Study found\n";
		}

		subRec = studyRec->nextSub(subRec);
	}

	// Inseriamo questo studio nel browser
	wxString szValue;
	const char *pTmp = NULL;

	// Patient Name
	if(studyRec->findAndGetString(DCM_PatientsName, pTmp).bad())
	{
		CERR << "mitoLocalDatabase::OnDicomDir_Study - findAndGetString (DCM_PatientsName) error" << endl;
	}

	szValue = pTmp;

	wxTreeItemId studyItem = pAlbum->AppendItem(rootItem, szValue);	
/*
	// Birthdate
	szValue = pStudy->GetPropVal(wxT("patBirthdate"), wxT(""));
	FormatDicomDate(szDateTemp, szValue);
	pAlbum->SetItemText(studyItem, 1, szDateTemp);	

	// Modality
	szValue = pStudy->GetPropVal(wxT("modality"), wxT(""));
	pAlbum->SetItemText(studyItem, 2, szValue);	

	// Description
	szValue = pStudy->GetPropVal(wxT("description"), wxT(""));
	pAlbum->SetItemText(studyItem, 3, szValue);	

	// Patient ID
	szValue = pStudy->GetPropVal(wxT("patID"), wxT(""));
	pAlbum->SetItemText(studyItem, 4, szValue);	

	// Patient Age
	szValue = pStudy->GetPropVal(wxT("patAge"), wxT(""));
	pAlbum->SetItemText(studyItem, 5, szValue);	

	// Accession
	szValue = pStudy->GetPropVal(wxT("accession"), wxT(""));
	pAlbum->SetItemText(studyItem, 6, szValue);	

	// Date
	szValue = pStudy->GetPropVal(wxT("date"), wxT(""));
	FormatDicomDate(szDateTemp, szValue);
	pAlbum->SetItemText(studyItem, 7, szDateTemp);	

	// Imm
	szValue = pStudy->GetPropVal(wxT("images"), wxT(""));
	pAlbum->SetItemText(studyItem, 8, szValue);	

	// Referring
	szValue = pStudy->GetPropVal(wxT("referring"), wxT(""));
	pAlbum->SetItemText(studyItem, 9, szValue);	

	// Performing
	szValue = pStudy->GetPropVal(wxT("performing"), wxT(""));
	pAlbum->SetItemText(studyItem, 10, szValue);	

	// Institution
	szValue = pStudy->GetPropVal(wxT("institution"), wxT(""));
	pAlbum->SetItemText(studyItem, 11, szValue);	

	// Path
	szValue = pStudy->GetPropVal(wxT("path"), wxT(""));
	pAlbum->SetItemText(studyItem, 12, szValue);	*/

	/*
	if(seriesFound) 
	{
		DcmDataset study;
		DcmElement *el;
		
		if ( studyRec->findAndCopyElement(DCM_SpecificCharacterSet, el) == ECC_Normal )
			study.insert(el);
		
		if ( studyRec->findAndCopyElement(DCM_StudyInstanceUID, el) == ECC_Normal )
			study.insert(el);
		
		if ( studyRec->findAndCopyElement(DCM_StudyDate, el) == ECC_Normal )
			study.insert(el);
		
		if ( studyRec->findAndCopyElement(DCM_StudyTime, el) == ECC_Normal )
			study.insert(el);
		
		if ( studyRec->findAndCopyElement(DCM_StudyDescription, el) == ECC_Normal )
			study.insert(el);
		
		if ( patRec->findAndCopyElement(DCM_PatientsName, el) == ECC_Normal )
			study.insert(el);
		
		if ( patRec->findAndCopyElement(DCM_PatientsBirthDate, el) == ECC_Normal )
			study.insert(el);
		
		if ( patRec->findAndCopyElement(DCM_PatientsSex, el) == ECC_Normal )
			study.insert(el);
		
		//resultslot(create_query_study(&study, std::string("DICOMDIR:") + dicomdir));
	}*/

	return seriesFound;
}

bool mitoLocalDatabase::OnDicomDir_Series(DcmDirectoryRecord *patRec, DcmDirectoryRecord *studyRec, DcmDirectoryRecord *seriesRec, wxTreeListCtrl *pAlbum)
{
	return true;
}