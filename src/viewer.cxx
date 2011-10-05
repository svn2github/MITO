/**
 * \file viewer.cxx
 * \brief File per la gestione dei dati da visualizzare
 * \author ICAR-CNR Napoli
 */

#include "viewer.h"

viewer::viewer(viewerDrawing *viewerDrawing, unsigned int idViewer) {
	_viewerDrawing = viewerDrawing;
	_idViewer = idViewer;
	_mouseX = 0;
	_mouseY = 0;
}

viewer::~viewer() {
	if (_viewerDrawing)
		_viewerDrawing->Delete();
}

void viewer::show(string title, int x, int y, int w, int h) {
	_viewerDrawing->show(_idViewer, title, x, y, w, h);
}