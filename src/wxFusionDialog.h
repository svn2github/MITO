/**
 * \file wxFusionDialog.h
 * \brief File per la creazione del dialog per la selezione dei parametri per la fusione di immaigni
 * \author ICAR-CNR Napoli
 */

#ifndef _wxFusionDialog_h_
#define _wxFusionDialog_h_

#include "fusionFilter.h"
#include <wx/textctrl.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/slider.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

using namespace std;

/** 
 * \class wxFusionDialog
 * \brief Implementa una finestra di dialogo per la selezione dei parametri di fusione.
 * 
 * Permette di selezionare la tecnica di fusione, i file o le serie da fondere, l'applicazione della CLUT, i pesi per la fusione.
 */
class wxFusionDialog : public wxDialog  {

	DECLARE_CLASS(wxFusionDialog)

private:
	/** 
	 * \var wxListBox * _listIdData
	 * \brief Lista contenente gli identificativi delle immagini
	 */
	wxListBox * _listIdData;

	/** 
	 * \var wxTextCtrl * _textIdData1
	 * \brief Identificativo della prima immagine/serie
	 */
	wxTextCtrl * _textIdData1;

	/** 
	 * \var wxTextCtrl * _textIdData2
	 * \brief Identificativo della seconda immagine/serie
	 */
	wxTextCtrl * _textIdData2;

	/** 
	 * \var wxTextCtrl * _pesoData1
	 * \brief Casella contentente il peso da assegnare alla prima immagine/serie
	 */
	wxTextCtrl * _pesoData1;

	/** 
	 * \var wxTextCtrl * _pesoData2
	 * \brief Casella contentente il peso da assegnare alla seconda immagine/serie
	 */
	wxTextCtrl * _pesoData2;

	/** 
	 * \var wxSlider * _slideWeight
	 * \brief Slider atta a settare i pesi
	 */
	wxSlider * _slideWeight;

	/** 
	 * \var wxRadioBox * _fusionType
	 * \brief Radiobox atto a consentire la scelta della modalità di fusione
	 */
	wxRadioBox * _fusionType;

	/** 
	 * \var wxCheckBox * _checkCLUT
	 * \brief Checkbox che attiva/disattiva l'applicazione della CLUT alla seconda immagine/serie
	 */
	wxCheckBox * _checkCLUT;


	/** 
	 * \var int _idSel1
	 * \brief Identificativo del primo dato selezionato
	 */
	int _idSel1;

	/** 
	 * \var int _idSel2
	 * \brief Identificativo del secondo dato selezionato
	 */
	int _idSel2;

	/** 
	 * \var int _peso1
	 * \brief Peso da assegnare alla prima immagine/serie
	 */
	float _peso1;

	/** 
	 * \var int _peso2
	 * \brief Peso da assegnare alla seconda immagine/serie
	 */
	float _peso2;

	/** 
	 * \var fusionFilter::fusionType _algoType
	 * \brief Memorizza la tecnica scelta con cui effettuare la fusione
	 */
	fusionFilter::fusionType _algoType;

	/** 
	 * \var fusionFilter::ruleType _ruleType
	 * \brief Memorizza la regola di fusione scelta
	 */
	fusionFilter::ruleType _ruleType; 

	/** 
	 * \var fusionFilter::WaveletType _waveletType
	 * \brief Memorizza il tipo di wavelet scelto
	 */
	fusionFilter::WaveletType _waveletType;

	/** 
	 * \var wxChoice * _levelChoice
	 * \brief Livello di decomposizione wavelet scelto
	 */
	wxChoice * _levelChoice;

	/** 
	 * \var fusionFilter::LivelloDec _level;
	 * \brief Memorizza il livello di decomposizione wavelet scelto
	 */
	fusionFilter::LivelloDec _level;



public:

	/** 
	 * \brief Costruttore della classe.
	 * 
	 * Costruisce la finestra di dialogo
	 */
	wxFusionDialog(wxWindow* win);

	/** 
	 * \brief Setta l'elenco delle immagini e delle serie disponibili per la fusione.
	 *
	 * \param choices L'elenco delle immagini e delle serie disponibili per la fusione
	 */
	inline void setListData(const wxArrayString & choices) {
		_listIdData->Set(choices);
	}
			
	/** 
	 * \brief Ritorna la prima immagine o serie scelta per la fusione.
	 *
	 * \return ID della prima immagine o serie scelta per la fusione
	 */
	inline int getIdData1() {
		return _idSel1;
	}

	/** 
	 * \brief Ritorna la seconda immagine o serie scelta per la fusione.
	 *
	 * \return ID della seconda immagine o serie scelta per la fusione
	 */
	inline int getIdData2() {
		return _idSel2;
	}


	/** 
	 * \brief Ritorna il peso per la prima immagine o serie scelta per la fusione.
	 *
	 * \return Peso per la prima immagine o serie scelta per la fusione
	 */
	inline float getPeso1() {
		return _peso1;
	}

	/** 
	 * \brief Ritorna il peso per la seconda immagine o serie scelta per la fusione.
	 *
	 * \return Peso per la seconda immagine o serie scelta per la fusione
	 */
	inline float getPeso2() {
		return _peso2;
	}

	/** 
	 * \brief Ritorna il tipo di tecnica scelto per effettuare la fusione.
	 *
	 * \return Tecnica di fusione scelta
	 */
	inline fusionFilter::fusionType getFusionType() {
		return _algoType;
	}

	/** 
	 * \brief Ritorna la regola di fusione scelta
	 *
	 * \return Regola di fusione scelta
	 */
	inline fusionFilter::ruleType getFusionRule() {
		return _ruleType;
	}

	/** 
	 * \brief Ritorna il tipo di wavelet da utilizzare per la fusione
	 *
	 * \return Tipo di wavelet da utilizzare per la fusione
	 */
	inline fusionFilter::WaveletType getWaveletType() {
		return _waveletType;
	}

	/** 
	 * \brief Ritorna un valore booleano che indica se applicare o meno la CLUT
	 *
	 * \return Valore booleano che indica se applicare o meno la CLUT
	 */
	inline bool getApplyClut() {
		return _checkCLUT->GetValue();
	}

	inline fusionFilter::LivelloDec getLevel() {
		return _level;
	}

private:
	DECLARE_EVENT_TABLE()

	/** 
	 * \brief Metodo invocato per settare l'ID della prima immagine o serie
	 */
	void onSetIdData1( wxCommandEvent& event );

	/** 
	 * \brief Metodo invocato per settare l'ID della seconda immagine o serie
	 */
	void onSetIdData2( wxCommandEvent& event );


	/** 
	 * \brief Metodo invocato quando viene premuto il pulsante OK
	 */
	void onOkDown( wxCommandEvent& event );

	/** 
	 * \brief Metodo invocato quando viene premuto il pulsante CANCEL
	 */
	void onCancelDown( wxCommandEvent& event );

	/** 
	 * \brief Metodo invocato quando viene fatta scrollare la barra per l'impostazione dei pesi
	 */
	void onScroll( wxScrollEvent& event );

	/** 
	 * \brief Metodo invocato quando viene scelta la tecnica di fusione
	 */
	void onFusionType( wxCommandEvent& event );

	/** 
	 * \brief Metodo invocato quando viene scelto il livello di decomposizione wavelet
	 */
	void onLivelloDec( wxCommandEvent& event );


};

enum {
	fusionType_radioBox,
	setIdData1_button,
	setIdData2_button,
	weight_slider,
	ok_buttonFusion,
	cancel_buttonFusion
};

#endif _wxFusionDialog_h_