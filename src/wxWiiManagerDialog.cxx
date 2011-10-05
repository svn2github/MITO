#include "wxWiiManagerDialog.h"

BEGIN_EVENT_TABLE (wxWiiManagerDialog, wxDialog)

	EVT_CHECKBOX (-1, wxWiiManagerDialog::OnCheckBox)
	EVT_BUTTON	(rilevaPressed, wxWiiManagerDialog::OnRileva)

END_EVENT_TABLE()


void wxWiiManagerDialog::OnRileva(wxCommandEvent & event)
{
	EndModal(rilevaPressed);
}


void wxWiiManagerDialog::OnCheckBox(wxCommandEvent & event)
{
	wxWindow* win = (wxWindow*)event.GetEventObject();

	if ( win->GetLabel() == connectLabel )		//Evento generato da un checkBox "connect"
	{
		if (event.GetInt() == 0)	// Il checkBox "connect" che ha generato l'evento adesso vale false.
		{											
			wxCheckBox* allSelect = getAllSelectBox();		// Essendo stato deselezionato uno dei Wii, se allSelect
			if (allSelect->GetValue()==true) allSelect->SetValue(false);   // è selezionato dev'essere deselezionato.

			int i;

			for (i=1; i<=Wii_num; i++)	{									// Essendo stato deselezionato uno 
				wxCheckBox* connect = getCheckBox( getWiiPanel(i) );			// dei Wii,	controllo se resta 	
				if (connect->GetValue()==true) break;							// almeno un Wii selezionato,
			}																	// altrimenti devo disabilitare
			if (i>Wii_num) {													// il bottone "Avvia".
				wxButton* avvia = getStartButton();
				avvia->Enable(false);
			}
		}
		else	// Il checkBox "connect" che ha generato l'evento adesso vale true.
		{
			int i;
			for (i=1; i<=Wii_num; i++) {									// Essendo stato selezionato uno 
				wxCheckBox* check = getCheckBox( getWiiPanel(i) );				// dei Wii,	controllo se risultano 
				if (check->GetValue()==false) break;							// selezionati tutti i Wii.
			}
			if (i>Wii_num) {													// In tal caso allSelect dev'essere 
				wxCheckBox* allSelect = getAllSelectBox();						// selezionato.
				allSelect->SetValue(true);
			}

			wxButton* avvia = getStartButton();		//La selezione di un Wii abilita il bottone "Avvia".
			avvia->Enable(true);
		}
		        
		// Poichè è stato cambiato lo stato del checkBox "connect" che ha generato l'evento, 
		// di conseguenza devo cambiare lo stato del choiceColor ad esso associato.
		wxChoice* choice  = getChoiceColor( win->GetParent() );		
		choice->Enable(!choice->IsEnabled());

		// Poichè è stato cambiato lo stato del checkBox "connect" che ha generato l'evento, 
		// di conseguenza devo cambiare lo stato del batteryText ad esso associato.
		wxStaticText* batt = getBatteryText( win->GetParent() );		
		batt->Enable(!batt->IsEnabled());

	}
	else 
		if ( win->GetLabel() == selectAllLabel )	//Evento generato dal checkBox "selectAll"
		{
			if ( event.GetInt() == 1 )		// Il nuovo valore di "selectAll" è true.
			{
				for (int i=1; i<=Wii_num; i++)		// Visto che è stato selezionato selectAll, devo cambiare di 
				{									// conseguenza lo stato di tutti i Wii non selezionati.		

					wxCheckBox* connect = getCheckBox( getWiiPanel(i) );
					if (connect->GetValue()==false) {						// Se un Wii non risulta selezionato devo 
						connect->SetValue(true);							// selezionarlo e generare un evento 		                        
						wxCommandEvent e(wxEVT_COMMAND_CHECKBOX_CLICKED);	// associato a tale cambio di stato.
						e.SetEventObject(connect);
						e.SetInt(1);
						this->ProcessEvent(e);	
					}
				}                
			}
			else		// Il nuovo valore di "selectAll" è false.
			{
				for (int i=1; i<=Wii_num; i++)							// Visto che è stato deselezionato selectAll,
				{														// devo deselezionare di conseguenza tutti i 
					wxCheckBox* check = getCheckBox( getWiiPanel(i) );	// Wii. Poichè prima era selezionato selectAll, 
					check->SetValue(false);								// conosco già lo stato precedente di tutti i  
					wxCommandEvent e(wxEVT_COMMAND_CHECKBOX_CLICKED);	// Wii, quindi li deseleziono direttamente  
					e.SetEventObject(check);							// senza alcun test preventivo.
					this->ProcessEvent(e);	
				}
			}
		}

}

bool wxWiiManagerDialog::isWiiSelected(int i)
{
	bool retval = false;
	if ( (i>0) && (i<=Wii_num) )
	{
		wxCheckBox* connect = getCheckBox( getWiiPanel(i) );
		retval = connect->GetValue();
	}
	return retval;
}

int  wxWiiManagerDialog::getColorChosen(int i)
{
	int retval = wxNOT_FOUND;
	if ( (i>0) && (i<=Wii_num) )
	{
		wxChoice* color = getChoiceColor( getWiiPanel(i) );
		retval = color->GetSelection();
	}
	return retval;
}

wxWindow* wxWiiManagerDialog::getWiiPanel(int i)
{
	int zero = 0;	// variabile di comodo per evitare ambiguità (tra 0 e NULL) nel passaggio di parametri di GetItem.
	return this->GetSizer()->GetItem(zero)->GetSizer()->GetItem(1)->GetSizer()->GetItem(i-1)->GetWindow();
}

wxButton* wxWiiManagerDialog::getStartButton()
{
	int zero = 0;	// variabile di comodo per evitare ambiguità (tra 0 e NULL) nel passaggio di parametri di GetItem.
	return (wxButton*)this->GetSizer()->GetItem(1)->GetSizer()->GetItem(zero)->GetWindow();
}

wxCheckBox* wxWiiManagerDialog::getAllSelectBox()
{
	int zero = 0;	// variabile di comodo per evitare ambiguità (tra 0 e NULL) nel passaggio di parametri di GetItem.
	return (wxCheckBox*)this->GetSizer()->GetItem(zero)->GetSizer()->GetItem(zero)->GetSizer()->GetItem(zero)->GetWindow();
}

wxChoice* wxWiiManagerDialog::getRotationModeChoice() {
	return (wxChoice*)FindWindowById(idRotationMode,this);	
}

// Risale al choiceColor conoscendo il panel che lo contiene
wxChoice* wxWiiManagerDialog::getChoiceColor(wxWindow * win)
{
	return (wxChoice*)win->GetSizer()->GetItem(1)->GetSizer()->GetItem(1)->GetWindow();
}

// Risale al checkBox conoscendo il panel che lo contiene
wxCheckBox* wxWiiManagerDialog::getCheckBox(wxWindow * win)
{
    int zero = 0;	// variabile di comodo per evitare ambiguità (tra 0 e NULL) nel passaggio di parametri di GetItem.
	return (wxCheckBox*)win->GetSizer()->GetItem(zero)->GetSizer()->GetItem(zero)->GetWindow();
}

// Risale al batteryText conoscendo il panel che lo contiene
wxStaticText* wxWiiManagerDialog::getBatteryText(wxWindow * win)
{
	 int zero = 0;	// variabile di comodo per evitare ambiguità (tra 0 e NULL) nel passaggio di parametri di GetItem.
	return (wxStaticText*)win->GetSizer()->GetItem(zero)->GetSizer()->GetItem(1)->GetWindow();
}


wxWiiManagerDialog::wxWiiManagerDialog( wxWindow *parent, int Wii_num, int* batteryLevel, wxWindowID id, const wxString &title, 
									const wxPoint& position, const wxSize& size, long style) 
												: wxDialog( parent, id, title, position, size, style) 
{    
	this->Wii_num = Wii_num;
    
	// Contenitore del check selectAll Wii.
	wxBoxSizer* selectAllSizer = new wxBoxSizer(wxHORIZONTAL);	
	if (Wii_num > 0)  { 
		wxCheckBox* selectAll = new wxCheckBox( this, wxID_ANY, selectAllLabel, wxDefaultPosition, wxDefaultSize, 0 );
		selectAll->Show(false);
		selectAllSizer->Add(selectAll, 0, wxALL, 15);

		wxString choices[] = { _(" 2+1 DOF Rotation Mode "), _(" 3*1 DOF Rotation Mode ") };
		wxChoice* rotationMode = new wxChoice(this, idRotationMode, wxDefaultPosition, wxDefaultSize, 2, choices);
		rotationMode->SetSelection(0);		
		selectAllSizer->Add(rotationMode, 0, wxALL, 5);
	}


	//Contenitore di tutti i Wii trovati. Il suo layout è funzione del numero di Wii trovati.
	wxGridSizer* WiiFoundSizer = new wxGridSizer(0,1,0,0);		
	WiiFoundSizer->SetCols( (int)((Wii_num-1)/4) + 1 );	

	if (Wii_num==0)					//Se Wii_num è pari a 0, WiiFoundSizer conterrà soltanto il seguente messaggio.
	{
		WiiFoundSizer->Add( new wxStaticText(this, -1, "          No Wiimote detected.          ") , 0, wxALL, 5); 
	}
	else								//Altrimenti WiiFoundSizer conterrà Wii_num pannelli, uno per ogni Wii.
		for (int i=1;i<=Wii_num;i++)	
		{ 
			WiiFoundSizer->Add( createAWiiBox(this, i, batteryLevel[i-1]), 0, wxALL, 5);	            
		}	


	//Contenitore raggruppante i due contenitori precedentemente creati.
	wxStaticBox* staticBox = new wxStaticBox(this, wxID_ANY, "Wiimote Detection"); 	
	wxBoxSizer* WiiSizer = new wxStaticBoxSizer( staticBox, wxVERTICAL );
	WiiSizer->Add(selectAllSizer, 0, wxALL , 5);
	WiiSizer->Add(WiiFoundSizer, 0, wxALL | wxALIGN_CENTER, 5);


	//Contenitore dei bottoni.
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	
	wxButton* goButton = new wxButton(this, wxID_OK, "Start");							//Bottone "Start".
	goButton->Enable(false);
	buttonSizer->Add(goButton, 0, wxALL, 5);

	wxButton* againButton = new wxButton(this, rilevaPressed, rilevaButtonLabel);			//Bottone "Redetect".
	buttonSizer->Add(againButton, 0, wxALL, 5);

	wxButton* escButton = new wxButton(this, wxID_CANCEL, "Cancel");					//Bottone "Cancel".
	buttonSizer->Add(escButton, 0, wxALL, 5);


	//Contenitore principale, raggruppante i contenitori precedenti.
	wxBoxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
	MainSizer->Add(WiiSizer, 0, wxALL | wxALIGN_CENTER, 5);
	MainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 20);

	this->SetSizerAndFit(MainSizer);		
	this->Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

wxPanel* wxWiiManagerDialog::createAWiiBox(wxWindow * parent, int idWii, int batteryLevel)
{
	
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// checkbox per selezionare se connettere o meno il Wii.
	wxCheckBox* connect = new wxCheckBox( panel, wxID_ANY, connectLabel, wxDefaultPosition, wxDefaultSize, 0 );	

	// Testo indicante la percentuale di batteria rimanente.
	char batt[10];	_itoa(batteryLevel, batt, 10);
	wxStaticText * batteryText = new wxStaticText(panel, -1, wxString(" Battery ") + batt + wxString("%"));
	batteryText->Enable(false);

	wxBoxSizer* connectSizer = new wxBoxSizer( wxVERTICAL );				
	connectSizer->Add(connect, 0, wxALL|wxALIGN_CENTER, 5);	
	connectSizer->Add(batteryText, 0, wxALL|wxALIGN_CENTER, 5);	
	
	// Testo indicante la finalità del choice box sottostante.
	wxStaticText * labelColor = new wxStaticText(panel, -1, pointerColorLabel, wxDefaultPosition, wxDefaultSize); 
	labelColor->SetForegroundColour(labelColor->GetBackgroundColour());
	
	// choice box per la scelta dei colori del pointer.
	wxString choices[] = listOfColors;				
	wxChoice* choiceColor = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, num_Colors, choices);
	choiceColor->SetSelection( (idWii-1) % num_Colors);
	choiceColor->Enable(false);


	wxBoxSizer* ColorSizer = new wxBoxSizer( wxVERTICAL );		
	ColorSizer->Add(labelColor, 0, wxALL|wxALIGN_CENTER, 5);
	ColorSizer->Add(choiceColor, 0, wxALL|wxALIGN_CENTER, 5);

	// Nascondo il choice color e visualizzo semplicemente il colore del Wii
	choiceColor->Show(false);
	wxStaticText * thisWiiColor = new wxStaticText(panel, -1, choiceColor->GetStringSelection() , wxDefaultPosition, wxDefaultSize); 
	ColorSizer->Add(thisWiiColor, 0, wxALL|wxALIGN_CENTER, 5);
    

	char str[10];
	wxStaticBox* staticBox = new wxStaticBox(panel, wxID_ANY, wxString("Wiimote  ") + wxString(_itoa(idWii,str,10)) );

	wxBoxSizer* Sizer = new wxStaticBoxSizer( staticBox, wxHORIZONTAL );
	Sizer->Add(connectSizer, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	Sizer->Add(ColorSizer, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	
	panel->SetSizerAndFit(Sizer);

	return panel;
}