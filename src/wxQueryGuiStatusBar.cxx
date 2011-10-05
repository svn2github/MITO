/**
 * \file wxQueryGuiStatusBar.cpp
 * \brief Classe per la nuova status bar contenente la progress bar
 * \author ICAR-CNR Napoli
 */

#include "wxQueryGuiStatusBar.h"

// Event table
BEGIN_EVENT_TABLE(wxQueryGuiStatusBar, wxStatusBar)
  EVT_SIZE(wxQueryGuiStatusBar::OnSize)
END_EVENT_TABLE()

/*
 * Constructor
 */
wxQueryGuiStatusBar::wxQueryGuiStatusBar(wxWindow *parent, wxWindowID id, long style, const wxString &name) 
: wxStatusBar(parent, id, style, name)
{
  m_pProgressBar = new wxGauge(this, -1, 100, wxPoint(0, 0), wxDefaultSize, wxGA_HORIZONTAL | wxGA_SMOOTH);

  // Bah. Non funziona OnSize sennò..
  wxSizeEvent dummy;
  OnSize(dummy);
  
  m_pProgressBar->Disable();
}

/*
 * Destructor
 */
wxQueryGuiStatusBar::~wxQueryGuiStatusBar()
{
  if (m_pProgressBar)
    delete m_pProgressBar;

  m_pProgressBar = NULL;
}

/*
 * Size event handler
 */
void wxQueryGuiStatusBar::OnSize(wxSizeEvent &event)
{
  wxRect r;
  GetFieldRect(0, r);
  m_pProgressBar->SetSize(r);
  SetProgress(r.GetWidth());
}

/*
 * Set progress bar value
 */
void wxQueryGuiStatusBar::SetProgress(int progress)
{
  m_pProgressBar->SetValue(progress);
}

/*
 * Set progress bar range and value
 */
void wxQueryGuiStatusBar::SetProgress(int progress, int range)
{
	if(m_pProgressBar)
	{
		m_pProgressBar->SetRange(range);
		m_pProgressBar->SetValue(progress);
	} 
}

int  wxQueryGuiStatusBar::GetValue() {
	int ret=0;
	if(m_pProgressBar)
		ret = m_pProgressBar->GetValue();
	return ret;
}

void wxQueryGuiStatusBar::PulseProgress()
{
	if(m_pProgressBar)
	{
		m_pProgressBar->Pulse();
	}
}

void wxQueryGuiStatusBar::Stop()
{
	if(m_pProgressBar)
	{
		m_pProgressBar->SetValue(0);
		m_pProgressBar->Disable();
	}
}

void wxQueryGuiStatusBar::Restart()
{
	if(m_pProgressBar)
	{
		m_pProgressBar->Enable();
	}
}