/**
 * \file wxPacsListBox.cxx
 * \brief File per la gestione di una lista per la memorizzazione di informazioni provenienti dal PACS
 * \author ICAR-CNR Napoli
 */

#include "wxPacsListBox.h"


wxPacsListBox::wxPacsListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name) : wxListBox(parent, id, pos, size, choices, style, validator, name) {
	ptrPacsObject = 0;
}

wxPacsListBox::~wxPacsListBox() {
	Ptr p,q;
	for(p=ptrPacsObject; p!=0; p=q) {
		q=p->next;
		delete p;
	}
}

void wxPacsListBox::Add(const wxString str, void* data) {
	Ptr q = new PacsObject;
	q->str = str;
	q->data = data;
	q->next = ptrPacsObject;
	ptrPacsObject=q;
}

void* wxPacsListBox::GetPacsDataRic(const Ptr p, const wxString str) {
	if(p==0) return 0;
	else {
		if(str.Cmp(p->str) == 0) return p->data;
		else return GetPacsDataRic(p->next,str);
	}
}

void* wxPacsListBox::GetPacsData(const wxString str) {
	return GetPacsDataRic(ptrPacsObject,str);

}