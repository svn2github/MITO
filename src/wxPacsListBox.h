/**
 * \file wxPacsListBox.h
 * \brief File per la gestione di una lista per la memorizzazione di informazioni provenienti dal PACS
 * \author ICAR-CNR Napoli
 */

#ifndef _wxPacsListBox_h_
#define _wxPacsListBox_h_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/listbox.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

/**
* \brief Struttura che definisce un campo della lista
*/
struct PacsObject {
	wxString str;
	void* data;
	PacsObject* next;
};

/**
* \brief Definizione di un puntatore alla struttura PacsObject
*/
typedef PacsObject* Ptr;

/**
 * \class wxPacsListBox
 * \brief Classe atta alla gestione di una lista. Deriva da wxListBox
 */
class wxPacsListBox : public wxListBox {
private:
	/** 
	 * \var Ptr ptrPacsObject
	 * \brief Puntatore alla struttura PacsObject
	 */
	Ptr ptrPacsObject;

    /**
	* \fn void* GetPacsDataRic(const Ptr p, const wxString str)
	* \brief Ottiene un dato dalla lista attraverso una ricerca ricorsiva
	* \param p Puntatore alla lista corrente
	* \param str Stringa da ricercare
	* \return Il dato trovato
	*/
	void* GetPacsDataRic(const Ptr p, const wxString str);

public:
	/** Costruttore con parametri la finestra padre, l'identificativo, il titolo,  la posizione, la dimensione, lo stile, il validator e il nome della finestra */
    wxPacsListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox");

	/** Distruttore */
	~wxPacsListBox();                 

    /**
	* \fn void Add(const wxString str, void* data)
	* \brief Aggiunge un campo nella lista
	* \param str Stringa da inserire
	* \param data Dato da inserire
	*/
	void Add(const wxString str, void* data);

    /**
	* \fn void Add(const wxString str, void* data)
	* \brief Restituisce un elemento dalla lista, invocando il metodo privato GetPacsDataRic
	* \param str Stringa da ricercare
	* \return Il dato trovato
	*/
	void* GetPacsData(const wxString str);

    /**
	* \fn void Add(const wxString str, void* data)
	* \brief Restituisce il puntatore alla lista
	* \return Il puntatore alla lista
	*/
	inline Ptr GetPtrObject() const {
		return ptrPacsObject;
	}
};

#endif _wxPacsListBox_h_