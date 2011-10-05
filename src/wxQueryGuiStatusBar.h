/**
 * \file wxQueryGuiStatusBar.h
 * \brief Header per la nuova status bar contenente una progress bar
 * \author ICAR-CNR Napoli
 */

#ifndef _wxQueryGuiStatusBar_h_
#define _wxQueryGuiStatusBar_h_

#include <wx/gauge.h>
#include <wx/statusbr.h>

class wxQueryGuiStatusBar : public wxStatusBar
{
private:
  wxGauge *m_pProgressBar;

public:
  wxQueryGuiStatusBar(wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString &name = "statusBar");
  ~wxQueryGuiStatusBar();

  void OnSize(wxSizeEvent &event);
  void SetProgress(int progress);
  void SetProgress(int progress, int range);
  int  GetValue();
  void PulseProgress();
  
  void Stop();
  void Restart();

  DECLARE_EVENT_TABLE()
};

#endif //_wxQueryGuiStatusBar_h_