/**
 * \file wxEventCatcher.h
 * \brief File per la cattura degli eventi
 * \author ICAR-CNR Napoli
 */

#ifndef _wxEventCatcher_h_
#define _wxEventCatcher_h_

#include "wxMainGui.h"

#include "wxWiimoteEvent.h"


/**
 * \class wxEventCatcher
 * \brief Questa classe si occupa di catturare gli eventi e di smistargli opportunamente. Deriva da wxEvtHandler
 */
class wxEventCatcher : public wxEvtHandler {
	/** 
	 * \var unsigned int _idViewer
	 * \brief Identificativo del visualizzatore
	 */
	unsigned int _idViewer;

	/** 
	 * \var wxMainGui* _mainGui
	 * \brief Finestra principale
	 */
	wxMainGui* _mainGui;

public:
	/** Costruttore con parametri l'identificativo del visualizzatore e una finestra */
	wxEventCatcher(unsigned int idViewer, wxWindow* mainGui);
	
	/** Distruttore */
	~wxEventCatcher();

private:

	/**
	 * \timeStamp dell'ultimo evento processato
	 */
	long lastWiimoteTimestamp[4];

	// frequenza di clock
	__int64 _clockFrequency;

	// per il log - lista che poi viene scritta sul file di log
	vector<string> list;

	// per il log - frequenza di clock
	__int64 _lastUpdatedTime;

	// per il log - generico istante di tempo
	__int64 _updatedTime;


	/**
	* \fn void onActivate(wxActivateEvent& event)
	* \brief Indica che la finestra è attiva
	* \param event Evento
	*/
	void onActivate(wxActivateEvent& event);

	/**
	* \fn void onQuit(wxCloseEvent& event)
	* \brief Chiude la finestra
	* \param event Evento
	*/
	void onQuit(wxCloseEvent& event);

	/**
	 * \fn void onMouseWheel(wxMouseEvent &event)
	 * \brief Invocato se si utilizza la rotella del mouse
	 * \param event Evento generato dal mouse
	 */
	void onMouseWheel(wxMouseEvent &event);

	/**
	 * \fn void onButtonDown(wxMouseEvent &event)
	 * \brief Invocato da un click del mouse
	 * \param event Evento generato dal mouse
	 */
	void onButtonDown(wxMouseEvent &event);

	/**
	 * \fn void onMotion(wxMouseEvent &event)
	 * \brief Invocato dopo aver modificato la posizione del puntatore del mouse
	 * \param event Evento generato dal mouse
	 */
	void onMotion(wxMouseEvent &event);

	/**
	 * \fn void LeaveWindow(wxMouseEvent &event)
	 * \brief Invocato quando il mouse lascia la finestra del viewer
	 * \param event Evento generato dal mouse
	 */
	void onLeaveWindow(wxMouseEvent &event);
	
	/**
	 * \fn void LeaveWindow(wxMouseEvent &event)
	 * \brief Invocato quando il mouse entra nella finestra del viewer
	 * \param event Evento generato dal mouse
	 */
	void onEnterWindow(wxMouseEvent &event);

	/**
	 * \fn void onWiiMotionIR(wxWiimoteEvent &event)
	 * \brief Invocato periodicamente dal Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiMotionIR			( wxWiimoteEvent & event );
	
	/**
	 * \fn void onWiiADown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante A del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiADown				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiAUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante A del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiAUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiBDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante B del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiBDown				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiBUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante B del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiBUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiMinusDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Minus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiMinusDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiMinusUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Minus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiMinusUp			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiHomeDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Home del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiHomeDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiHomeUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Home del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiHomeUp			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiPlusDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Plus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiPlusDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiPlusUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Plus del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiPlusUp			( wxWiimoteEvent & event );	   

	/**
	 * \fn void onWiiOneDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante One del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiOneDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiOneUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante One del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiOneUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiTwoDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto il pulsante Two del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiTwoDown			( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiTwoUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato il pulsante Two del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiTwoUp				( wxWiimoteEvent & event );

	/**
	 * \fn void onWiiCrossDown(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver premuto la Croce del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiCrossDown			( wxWiimoteEvent & event ); 

	/**
	 * \fn void onWiiCrossUp(wxWiimoteEvent &event)
	 * \brief Invocato dopo aver rilasciato la Croce del Wiimote
	 * \param event Evento generato dal Wiimote
	 */
	void onWiiCrossUp			( wxWiimoteEvent & event ); 

private:
  DECLARE_EVENT_TABLE()
};

#endif _wxEventCatcher_h_