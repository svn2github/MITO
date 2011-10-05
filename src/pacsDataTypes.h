/**
 * \file pacsDataTypes.h
 * \brief File per la definizione dei dati secondo lo standard DICOM per la comunicazione con i PACS
 * \author ICAR-CNR Napoli
 */


// This document defines the basic units of data holding for each of
// the four levels of the DICOM query structure.  These are defined
// so as to eliminate the need to search DCM_OBJECTs for data when the
// the data needed is smaller and more static.
//
// The basic structure is this.  PacsNode provides the fundamental
// structure for doubly-linked organization.  This class is inherited
// by FileData and PacsData which provide means of storing filenames
// and patient information (through DCM_ELEMENTs) respectively.
// PacsData is inherited by PatientData, StudyData, SeriesData and
// ImageData.  These classes provide the memory allocation for data
// in DCM_ELEMENTs (although PacsData provides a means to access this
// information, it does not provide a means for allocating/freeing
// the memory used within.
//
// PacsList provides list functionality to organizing these structures.
//
// QueryLevel merely provides a convienent abstraction for the query level.
// Originally, functions were called with an integer representing the
// query level, but this is somewhat more descriptive.
//

#ifndef _pacsDataTypes_h_
#define _pacsDataTypes_h_

/**
 * \brief Specifica una dimensione
 */
#define E_LEN 255

/**
 * \brief Specifica una dimensione
 */
#define F_LEN 1024

//#include "wxPacsListBox.h"
/*#include <ctn_os.h>
#include <dicom.h>
#include <dicom_uids.h>
#include <condition.h>
#include <lst.h>
#include <dulprotocol.h>
#include <dicom_objects.h>
#include <dicom_messages.h>
#include <dicom_services.h>*/

// Forward declarations
class wxArrayString;
class DcmDataset;
class wxTreeListCtrl;

//
// Directory path separator
//
#ifdef WIN32
#define SLASH '\\'
#else
#define SLASH '/'
#endif


/**
* \brief Tipo enumerativo che rappresenta il livello di query per le funzioni per il PACS
*/
typedef enum {
    PATIENT,
    STUDY,
    SERIES,
    IMAGE,
    UNDEFINED
}   QueryLevel;


/** \class PacsNode
 * \brief Struttura nodo per organizzare i dati in una lista. La maggior parte dei tipi di dati deriva da PacsNode
 */
class PacsNode {

public:
	/** Costruttore */
	PacsNode();

	/** Distruttore */
	virtual ~PacsNode();

	/**
	 * \var PacsNode *prev
	 * \brief Puntatore al nodo precedente
	 */
    PacsNode *prev;

	/**
	 * \var PacsNode *next
	 * \brief Puntatore al nodo successivo
	 */
	PacsNode *next;

	/**
	 * \var char *desc1
	 * \brief Puntatore alla descrizione numero 1 del nodo
	 */
	char *desc1;

	/**
	 * \var char *desc2
	 * \brief Puntatore alla descrizione numero 2 del nodo
	 */
	char *desc2;

	/**
	 * \var char *desc3
	 * \brief Puntatore alla descrizione numero 3 del nodo
	 */
	char *desc3;

	/**
	 * \var char *desc4
	 * \brief Puntatore alla descrizione numero 4 del nodo
	 */
	char *desc4;

	/**
	 * \var char *desc5
	 * \brief Puntatore alla descrizione numero 5 del nodo
	 */
	char *desc5;

	/**
	 * \var char *key
	 * \brief Puntatore al valore chiave
	 */
	char *key;
};


/** \class PacsList
 * \brief Fornisce le funzionalità per interagire con i dati in una lista
 */
class PacsList {

public:
	/** Costruttore */
    PacsList();

	/** Distruttore */
	~PacsList();

	/**
	 * \fn void clear()
	 * \brief Elimina tutti i nodi
	 */
	void clear();

	/**
	 * \fn void add(PacsNode* _node)
	 * \brief Aggiunge un nodo
	 * \param _node Nodo da aggiungere
	 */
	void add(PacsNode*);

	/**
	 * \fn void add(PacsNode* _node)
	 * \brief Elimina un nodo
	 * \param _node Nodo da eliminare
	 */
	void remove(PacsNode*);

	/**
	 * \fn int length() const
	 * \brief Restituisce la lunghezza della lista
	 * \return Lunghezza della lista
	 */
	int length() const;

	/**
	 * \fn PacsNode* head() const
	 * \brief Restituisce la testa della lista
	 * \return Testa della lista
	 */
	PacsNode* head() const;

	/**
	 * \fn PacsNode* tail() const
	 * \brief Restituisce la coda della lista
	 * \return Coda della lista
	 */
	PacsNode* tail() const;

	/**
	 * \fn PacsNode* find(const PacsNode* _node) const
	 * \brief Cerca un nodo nellla lista
	 * \param _node Nodo da cercare
	 * \return Nodo trovato o testa
	 */	
	PacsNode* find(const PacsNode*) const;

	/**
	 * \fn void display (wxTreeListCtrl*, QueryLevel)
	 * \brief Visualizza indormazioni in un browser
	 * \param _browser Browser in cui visualizzare
	 * \param _qLev Livello della query
	 */	
	void display (wxTreeListCtrl*, QueryLevel);

//protected:
public:
	/**
	 * \var PacsNode *hd
	 * \brief Puntatore alla testa
	 */	
	PacsNode *hd;

	/**
	 * \var PacsNode *hd
	 * \brief Puntatore alla coda
	 */
	PacsNode *tl;

	/**
	 * \var int len
	 * \brief Lunghezza della lista
	 */
	int len;

private:
	/**
	 * \fn void sortList(QueryLevel _qLev)
	 * \brief Ordina una lista
	 * \param _qLev Livello della query
	 */	
	void sortList(QueryLevel);

	/**
	 * \fn PacsNode* swap(PacsNode*, QueryLevel)
	 * \brief Scambia due puntatori
	 * \param _node Nodo a partire dal quale scambiare i due puntatori
	 * \param _qLev Livello della query
	 */	
	PacsNode* swap(PacsNode*, QueryLevel);
};


/** \class FileData
 * \brief Memorizza un singolo file. Deriva da PacsNode
 */
class FileData : public PacsNode {

public:
	/** Costruttore */
    FileData();

	/** Costruttore con parametro name */
	FileData(char*);

	/**
	 * \var char name[F_LEN]
	 * \brief Nome del file
	 */
	char name[F_LEN];
};


/** \class PacsData
 * \brief Memorizza le informazioni in stile DCM_OBJECT attraverso una lista di DCM_ELEMENT. Deriva da PacsNode
 */
class PacsData : public PacsNode {

public:
	/** Costruttore con parametri dimensione degli array obbligatori e opzionali*/
	PacsData(int,int);

	/** Distruttore */
	~PacsData();

	/**
	 * \fn void display() const
	 * \brief Visualizza nello standard output
	 */	
	void display() const;

	/**
	 * \fn wxArrayString *display(wxPacsListBox* browser) const
	 * \brief Visualizza in un browser
	 * \param browser Browser in cui visualizzare i dati
	 * \return Un array di stringhe contenenti descrizioni dei dati visualizzati
	 */
	//wxArrayString *display(wxPacsListBox*) const;

	/**
	 * \fn virtual void	ExportDicomDataset(DcmDataset *pDataset)
	 * \brief Aggiunge gli attributi della classe, alla struttura DICOM usata per le query
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	virtual void ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false) const = 0;

	/**
	 * \fn virtual void	ImportDicomDataset(DcmDataset *pDataset)
	 * \brief Aggiunge gli attributi della classe, alla struttura DICOM usata per le query
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	virtual void ImportDicomDataset(DcmDataset *pDataset) = 0;

	/**
	 * \var int reqSize
	 * \brief Dimensione degli array obbligatori
	 */
	//int reqSize;

	/**
	 * \var int optSize
	 * \brief Dimensione degli array opzionali
	 */
	//int optSize;

	/**
	 * \var DCM_ELEMENT *reqList
	 * \brief Array di elementi obbligatori
	 */
	//DCM_ELEMENT *reqList;

	/**
	 * \var DCM_ELEMENT *reqList
	 * \brief Array di elementi opzionali
	 */
	//DCM_FLAGGED_ELEMENT *optList;

	/**
	 * \var long optAttr
	 * \brief Lista degli elementi in optList
	 */
	//long optAttr;

	/**
	 * \var long optMask
	 * \brief Maschera per optAttr
	 */
	//long optMask;

	/**
	 * \var QueryLevel qLev
	 * \brief Descrive il tipo di oggetto
	 */
	QueryLevel qLev;

	/**
	 * \var PacsList *files
	 * \brief Nomi di file correlati ai dati
	 */
	PacsList *files;
};



/** \class PatientData
 * \brief Deriva da PacsData e consente l'allocazione di memoria per le strutture interne relative ai pazienti
 */
class PatientData : public PacsData {

public:
	/** Costruttore */
	PatientData();

	/**
	 * \fn void	ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false)
	 * \brief Aggiunge gli attributi del paziente, alla struttura DICOM usata per le query
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false) const;

	/**
	 * \fn void	ImportDicomDataset(DcmDataset *pDataset)
	 * \brief Riempe i campi attributi della classe a partire dal recordset dicom
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ImportDicomDataset(DcmDataset *pDataset);

	/**
	 * \var char name[E_LEN]
	 * \brief Corrisponde a DCM_PATNAME
	 */
	char name[E_LEN];

	/**
	 * \var char id[E_LEN]
	 * \brief Corrisponde a DCM_PATID
	 */
	char id[E_LEN];

	/**
	 * \var char birthdate[E_LEN]
	 * \brief Corrisponde a DCM_PATBIRTHDATE
	 */
	char birthdate[E_LEN];

	/**
	 * \var char sex[E_LEN]
	 * \brief Corrisponde a DCM_PATSEX
	 */
	char sex[E_LEN];

	/**
	 * \var char numStudies[E_LEN]
	 * \brief Numero studi
	 */
	char numStudies[E_LEN];
};


/** \class StudyData
 * \brief Deriva da PacsData e consente l'allocazione di memoria per le strutture interne relative ai studi
 */
class StudyData : public PacsData {

public:
	/** Costruttore */
	StudyData();

	/**
	 * \fn virtual void	ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false)
	 * \brief Aggiunge gli attributi dello studio, alla struttura DICOM usata per le query
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false) const;

	/**
	 * \fn void	ImportDicomDataset(DcmDataset *pDataset)
	 * \brief Riempe i campi attributi della classe a partire dal recordset dicom
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ImportDicomDataset(DcmDataset *pDataset);

	/**
	 * \var char patID[E_LEN]
	 * \brief Corrisponde a DCM_PATID
	 */
	char patID[E_LEN];

	/**
	 * \var char patName[E_LEN]
	 * \brief Corrisponde a DCM_PATNAME
	 */
	char patName[E_LEN];

	/**
	 * \var char patSex[E_LEN]
	 * \brief Corrisponde a DCM_PATSEX
	 */	
	char patSex[E_LEN];

	/**
	 * \var char patBirthdate[E_LEN]
	 * \brief Corrisponde a DCM_PATBIRTHDATE
	 */
	char patBirthdate[E_LEN];

	/**
	 * \var char date[E_LEN]
	 * \brief Corrisponde a DCM_IDSTUDYDATE
	 */
	char date[E_LEN];

	/**
	 * \var char accessionNumber[E_LEN]
	 * \brief Corrisponde a DCM_IDACCESSIONNUMBER
	 */
	char accessionNumber[E_LEN];

	/**
	 * \var char referringPhysician[E_LEN]
	 * \brief Corrisponde a DCM_IDREFERRINGPHYSICIAN
	 */	
	char referringPhysician[E_LEN];
    
	/**
	 * \var char description[E_LEN]
	 * \brief Corrisponde a DCM_IDSTUDYDESCRIPTION
	 */
	char description[E_LEN];

	/**
	 * \var char id[E_LEN]
	 * \brief Corrisponde a DCM_RELSTUDYID
	 */
	char id[E_LEN];

	/**
	 * \var char instanceUID[E_LEN]
	 * \brief Corrisponde a DCM_RELSTUDYINSTANCEUID
	 */
	char instanceUID[E_LEN];

	/**
	 * \var char numImages[E_LEN]
	 * \brief Corrisponde a DCM_RELSTUDYNUMIMAGES
	 */
	char numImages[E_LEN];

	/**
	 * \var char modality[E_LEN]
	 * \brief Corrisponde a DCM_IDMODALITIESINSTUDY
	 */
	char modality[E_LEN];
};


/** \class SeriesData
 * \brief Deriva da PacsData e consente l'allocazione di memoria per le strutture interne relative alle serie
 */
class SeriesData : public PacsData {

public:
	/** Costruttore */
    SeriesData();

	/**
	 * \fn void	ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false)
	 * \brief Aggiunge gli attributi della serie, alla struttura DICOM usata per le query
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false) const;

	/**
	 * \fn void	ImportDicomDataset(DcmDataset *pDataset)
	 * \brief Riempe i campi attributi della classe a partire dal recordset dicom
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ImportDicomDataset(DcmDataset *pDataset);

	/**
	 * \fn bool BelongsToStudy(StudyData *pParent)
	 * \brief Verifica se questa serie è relativa allo studio pParent
	 * \param pParent Puntatore ad un oggetto di tipo StudyData su cui effettuare il confronto
	 */
	bool BelongsToStudy(StudyData *pParent);

	/**
	 * \var char patName[E_LEN]
	 * \brief Corrisponde a DCM_PATNAME
	 */
    char patName[E_LEN];

	/**
	 * \var char studyInstanceUID[E_LEN]
	 * \brief Corrisponde a DCM_RELSTUDYINSTANCEUID
	 */
	char studyInstanceUID[E_LEN];

	/**
	 * \var char instanceUID[E_LEN]
	 * \brief Corrisponde a DCM_RELSERIESINSTANCEUID
	 */
	char instanceUID[E_LEN];

	/**
	 * \var char modality[E_LEN]
	 * \brief Corrisponde a DCM_IDMODALITY
	 */
	char modality[E_LEN];

	/**
	 * \var char bodyPart[E_LEN]
	 * \brief Corrisponde a DCM_ACQBODYPARTEXAMINED
	 */
	char bodyPart[E_LEN];

	/**
	 * \var char imagesNumber[E_LEN]
	 * \brief Corrisponde a DCM_RELNUMBERSERIESRELATEDINST
	 */
	char imagesNumber[E_LEN];

	/**
	 * \var char description[E_LEN]
	 * \brief Corrisponde a DCM_SeriesDescription
	 */
	char description[E_LEN];
};


/** \class ImageData
 * \brief Deriva da PacsData e consente l'allocazione di memoria per le strutture interne relative alle immagini
 */
class ImageData : public PacsData {

public:
	/** Costruttore */
    ImageData();

	/**
	 * \fn void	ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false)
	 * \brief Aggiunge gli attributi dell'immagine, alla struttura DICOM usata per le query
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ExportDicomDataset(DcmDataset *pDataset, bool bUseRequiredAttribsOnly = false) const;

	/**
	 * \fn void	ImportDicomDataset(DcmDataset *pDataset)
	 * \brief Riempe i campi attributi della classe a partire dal recordset dicom
	 * \param pDataset Puntatore ad una instanza di DcmDataset contenente gli elementi della query
	 */
	void ImportDicomDataset(DcmDataset *pDataset);

	/**
	 * \var char patID[E_LEN]
	 * \brief Corrisponde a DCM_PATID
	 */
    char patID[E_LEN];

	/**
	 * \var char studyInstanceUID[E_LEN]
	 * \brief Corrisponde a DCM_RELSTUDYINSTANCEUID
	 */
	char studyInstanceUID[E_LEN];

	/**
	 * \var char seriesInstanceUID[E_LEN]
	 * \brief Corrisponde a DCM_RELSERIESINSTANCEUID
	 */
	char seriesInstanceUID[E_LEN];

	/**
	 * \var char sopInstanceUID[E_LEN]
	 * \brief Corrisponde a DCM_IDSOPINSTANCEUID
	 */
	char sopInstanceUID[E_LEN];

	/**
	 * \var char number[E_LEN]
	 * \brief Corrisponde a DCM_RELIMAGENUMBER
	 */
	char number[E_LEN];

	/**
	 * \var char comments[E_LEN]
	 * \brief Corrisponde a DCM_RELIMAGECOMMENTS
	 */
	char comments[E_LEN];

	/**
	 * \var char sliceThickness[E_LEN]
	 * \brief Corrisponde a DCM_ACQSLICETHICKNESS
	 */
	char sliceThickness[E_LEN];

	/**
	 * \var char sliceSpacing[E_LEN]
	 * \brief Corrisponde a DCM_ACQSLICESPACING
	 */
	char sliceSpacing[E_LEN];
};


#endif _PacsDataTypes_h_