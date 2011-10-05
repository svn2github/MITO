/**
 * \file wxWLWWDialog.h
 * \brief File per la creazione del dialog per la modifica di window level e window width
 * \author ICAR-CNR Napoli
 */

#ifndef _wxWLWWDialog_h_
#define _wxWLWWDialog_h_

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/textctrl.h>
#include <wx/dialog.h>


/** 
 * \class wxWLWWDialog
 * \brief Implementa una finestra di dialogo per la modifica di window level e window width
 */
class wxWLWWDialog : public wxDialog  {

	DECLARE_CLASS(wxWLWWDialog)

private:
	/** 
	 * \var wxTextCtrl *_wxWL
	 * \brief Text Control contenente il window level
	 */
	wxTextCtrl *_wxWL;

	/** 
	 * \var wxTextCtrl *_wxWW
	 * \brief Text Control contenente il window width
	 */
	wxTextCtrl *_wxWW;

	/** 
	 * \var double _wl
	 * \brief Window level
	 */
	double _wl;

	/** 
	 * \var double _ww
	 * \brief Window width
	 */
	double _ww;

public:

	/** 
	 * \brief Costruttore della classe.
	 * 
	 * Costruisce la finestra di dialogo
	 */
	wxWLWWDialog(wxWindow* window);

			
	/** 
	 * \brief Ritorna il valore del window level
	 *
	 * \return Window level
	 */
	inline double getWL() {
		return _wl;
	}

	/** 
	 * \brief Ritorna il valore del window width
	 *
	 * \return Window width
	 */
	inline double getWW() {
		return _ww;
	}


private:
	DECLARE_EVENT_TABLE()

	/**
	 * \brief Metodo invocato dopo aver cliccato il bottone OK
	 */
	void onOkDown(wxCommandEvent& event);

	/**
	 * \brief Metodo invocato dopo aver cliccato il bottone Cancel
	 */
	void onCancelDown(wxCommandEvent& event);

};

enum {
	okButton,
	cancelButton
};

#endif _wxWLWWDialog_h_