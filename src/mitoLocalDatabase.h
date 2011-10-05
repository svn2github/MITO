/**
 * \file mitoLocalDatabase.h
 * \brief Questa classe gestisce il database di immagini locale
 * \author ICAR-CNR Napoli
 */

#ifndef mitoLocalDatabase_h_
#define mitoLocalDatabase_h_


#include <list>

using namespace std;

class wxArrayString;
class wxTreeListCtrl;
class wxXmlNode;
class wxTreeItemId;
class DcmDirectoryRecord;

/**
 * \class mitoLocalDatabase
 * \brief Classe per la gestione delle serie/studi presenti nella cache locale.
 */
class mitoLocalDatabase
{
public:
	mitoLocalDatabase();
	~mitoLocalDatabase();

	/**
	 * \fn bool AddFileToDatabase(wxArrayString *pInfo)
	 * \brief Aggiunge un nodo alla cache locale di file DICOM
	 * \param pInfo Contiene stringhe estratte dalla funzione ExtractAlbumInformations del pacsStoreSCP
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool AddFileToDatabase(wxArrayString *pInfo);
	
	/**
	 * \fn bool RemoveFromDatabase(wxArrayString *pInfo, bool bStudy = true)
	 * \brief Elimina un nodo dalla cache locale di file DICOM (fisicamente)
	 * \param pInfo Contiene stringhe estratte dalla funzione ExtractAlbumInformations del pacsStoreSCP
	 * \param bDeleteWholeStudy E' true se vogliamo eliminare tutto lo studio
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool RemoveFromDatabase(wxArrayString *pInfo, bool bDeleteWholeStudy = true, bool bPhysicallyRemove = false);

	/**
	 * \fn wxArrayString *GetFilePaths(wxArrayString *pInfo, bool bWholeStudy = true)
	 * \brief Ritorna un array contenente i percorsi dei files DICOM appartenenti allo studio/serie specificati in pInfo
	 * \param pInfo Contiene stringhe estratte dalla funzione ExtractAlbumInformations del pacsStoreSCP
	 * \param bWholeStudy E' true se vogliamo eliminare tutto lo studio
	 * \return true se non ci sono problemi, false altrimenti
	 */
	wxArrayString *GetFilePaths(wxArrayString *pInfo, bool bWholeStudy = true);

	/**
	 * \fn bool SaveIndexToFile(const char *pszFileName)
	 * \brief Aggiunge un nodo alla cache locale di file DICOM
	 * \param pInfo Contiene stringhe estratte dalla funzione ExtractAlbumInformations del pacsStoreSCP
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool SaveIndexToFile(const char *pszFileName);
	
	/**
	 * \fn bool LoadIndexFromFile(const char *pszFileName)
	 * \brief Aggiunge un nodo alla cache locale di file DICOM
	 * \param pszFileName Percorso al file indice
	 * \param pAlbum Contiene un puntatore al controllo che conterrà l'albero rappresentante gli studi in cache
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool LoadIndexFromFile(const char *pszFileName, wxTreeListCtrl *pAlbum);

	/**
	 * \fn bool ImportDicomDir(const char *pszDirPath, wxTreeListCtrl *pAlbum)
	 * \brief Importa una DICOMDIR nella cache locale
	 * \param pszDirPath Percorso completo alla DICOMDIR
	 * \param pAlbum Contiene un puntatore al controllo che conterrà l'albero rappresentante gli studi in cache
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool ImportDicomDir(const char *pszDirPath, wxTreeListCtrl *pAlbum);

	/**
	 * \fn list<wxArrayString*> GetList() const
	 * \brief Ritorna un puntatore alla lista della cache locale
	 * \return Il puntatore di tipo list<wxArrayString*> a m_localDatabase
	 */
	inline list<wxArrayString*> &GetList() {return m_localDatabase;}

private:

	/**
	 * \fn bool ParseStudy(wxXmlNode *pStudy, wxTreeListCtrl *pAlbum)
	 * \brief Effettua il parsing di un elemento "study"
	 * \param pStudy Contiene l'indirizzo dell'elemento "study"
	 * \param pAlbum Controllo in cui verranno salvate le informazioni caricate
	 * \return true se non ci sono stati errori, false altrimenti
	 */
	bool ParseStudy(wxXmlNode *pStudy, wxTreeListCtrl *pAlbum);

	/**
	 * \fn bool ParseSeries(wxXmlNode *pStudy, wxTreeListCtrl *pAlbum)
	 * \brief Effettua il parsing di un elemento "series"
	 * \param pSeries Contiene l'indirizzo dell'elemento "series"
	 * \param pAlbum Controllo in cui verranno salvate le informazioni caricate
	 * \return true se non ci sono stati errori, false altrimenti
	 */
	bool ParseSeries(wxXmlNode *pSeries, wxTreeListCtrl *pAlbum, wxTreeItemId parentStudy);

	/**
	 * \fn wxArrayString *AddNodeToDatabase(wxXmlNode *pNode)
	 * \brief Aggiunge l'elemento alla lista locale, estraendo le informazioni necessarie
	 * \param pNode Contiene l'indirizzo dell'elemento
	 * \return Ritorna un wxArrayString in caso di successo 
	 */
	wxArrayString *AddNodeToDatabase(wxXmlNode *pNode);
	
	wxTreeItemId AddStudyToAlbum(wxXmlNode *pStudy, wxTreeListCtrl *pAlbum);
	wxTreeItemId AddSeriesToAlbum(wxXmlNode *pSeries, wxTreeListCtrl *pAlbum, wxTreeItemId parentStudy);

	/**
	 * \fn bool OnDicomDir_Patient(DcmDirectoryRecord *patRec, wxTreeListCtrl *pAlbum)
	 * \brief Chiamato da ImportDicomDir quando viene incontrato un record paziente
	 * \param patRec Puntatore al record paziente dicomdir
	 * \param pAlbum Contiene un puntatore al controllo che conterrà l'albero rappresentante gli studi in cache
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool OnDicomDir_Patient(DcmDirectoryRecord *patRec, wxTreeListCtrl *pAlbum);

	/**
	 * \fn bool OnDicomDir_Study(DcmDirectoryRecord *patRec, DcmDirectoryRecord *studyRec, wxTreeListCtrl *pAlbum)
	 * \brief Chiamato da ImportDicomDir quando viene incontrato un record study 
	 * \param patRec Puntatore al record paziente dicomdir
	 * \param studyRec Puntatore al record study dicomdir
	 * \param pAlbum Contiene un puntatore al controllo che conterrà l'albero rappresentante gli studi in cache
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool OnDicomDir_Study(DcmDirectoryRecord *patRec, DcmDirectoryRecord *studyRec, wxTreeListCtrl *pAlbum);

	/**
	 * \fn bool OnDicomDir_Series(DcmDirectoryRecord *studyRec, DcmDirectoryRecord *studyRec, DcmDirectoryRecord *seriesRec, wxTreeListCtrl *pAlbum)
	 * \brief Chiamato da ImportDicomDir quando viene incontrato un record study 
	 * \param patRec Puntatore al record paziente dicomdir
	 * \param studyRec Puntatore al record study dicomdir
	 * \param seriesRec Puntatore al record series dicomdir
	 * \param pAlbum Contiene un puntatore al controllo che conterrà l'albero rappresentante gli studi in cache
	 * \return true se non ci sono problemi, false altrimenti
	 */
	bool OnDicomDir_Series(DcmDirectoryRecord *patRec, DcmDirectoryRecord *studyRec, DcmDirectoryRecord *seriesRec, wxTreeListCtrl *pAlbum);

	/** 
	 * \var list<wxStringArray*> m_localDatabase;
	 * \brief Gestisce le informazioni relative ai file dicom locali
	 */
	list<wxArrayString*> m_localDatabase;
};

#endif // mitoLocalDatabase_h_