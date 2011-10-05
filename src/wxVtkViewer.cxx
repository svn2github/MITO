/**
 * \file wxVtkViewer.cxx
 * \brief File per la gestione del visualizzatore
 * \author ICAR-CNR Napoli
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "wxVtkViewer.h"
#include "wxEventCatcher.h"

wxVtkViewer::wxVtkViewer(wxWindow *parent, wxWindow *gui, wxWindowID id)
{
	_parent = parent;
	_iren = new appWxVtkInteractor(gui, id, (wxMainGui*)parent);
	_iren->UseCaptureMouseOn();
}

wxVtkViewer::~wxVtkViewer() {
	if (_idViewer)
		_iren->PopEventHandler(true);
	if (_iren)
		_iren->Delete();
	_parent = 0;
	
}

void wxVtkViewer::destroyWindow(){
	wxWindow* window = (wxWindow*)_iren->GetParent();
	while(!(window->IsTopLevel())){
		if(window->GetParent())
			window = window->GetParent(); 			
	}
	if(window!=_parent)
		window->Destroy();
}

void wxVtkViewer::show(unsigned int idViewer, string title, int x, int y, int w, int h) {
	_idViewer = idViewer;
	_iren->PushEventHandler(new wxEventCatcher(_idViewer, _parent));
	((wxMainGui*)_parent)->onViewerActivate(_idViewer);
}

appData *wxVtkViewer::newAppData() {
	itkVtkData *d = new itkVtkData();
	return d;
}

void wxVtkViewer::showAppData(appData *appData) {
	if (appData) {
		_iren->ReInitialize();
		itkVtkData *itkVtkData_ = (itkVtkData*)appData;
		showItkVtkData(itkVtkData_);
	}
}

int wxVtkViewer::getSliceNumber(appData *appData) {
	if (appData) {
		itkVtkData *itkVtkData_ = (itkVtkData*)appData;
		return itkVtkData_->getSliceNumber();
	}
	else return -1;
}


