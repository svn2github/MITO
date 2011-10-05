/**
 * \file viewerHandler.cxx
 * \brief File per la gestione dei visualizzatori
 * \author ICAR-CNR Napoli
 */

#include "viewerHandler.h"

/*
* Il costruttore inizializza _idViewerCounter a 1
*/
viewerHandler::viewerHandler() {
	_idViewerCounter = 1;
}

/* 
* Il distruttore richiama il metodo deleteAllViewer per elimare tutti i viewer mostrati
*/
viewerHandler::~viewerHandler() {
	deleteAllViewer();
}

/*
* Crea un nuovo visualizzatore 2D
*/
viewer *viewerHandler::new2dViewer(viewerDrawing *viewerDraw) {
	viewer2d *v = new viewer2d(viewerDraw, _idViewerCounter);
	
	if (addViewer(v)) {
		_idViewerCounter++;
		v->show("MITO - 2D Viewer", 0, 80, 600, 600);
	}
	else {
		delete v;
		v = NULL;
	}

	return v;
}

/*
* Crea un nuovo visualizzatore 3D
*/
viewer *viewerHandler::new3dViewer(viewerDrawing *viewerDrawing) {
	viewer3d *v = new viewer3d(viewerDrawing, _idViewerCounter);
	
	if (addViewer(v)) {
		_idViewerCounter++;
		v->show("MITO - 3D Viewer", 80, 160, 800, 400);
	}
	else {
		delete v;
		v = NULL;
	}

	return v;
}

/*
* Restituisce un viewer
*/
viewer *viewerHandler::getViewer(unsigned int idViewer) {
	list<viewer*>::iterator iteList = _viewerList.begin();
    viewer *v;

	while (iteList != _viewerList.end()) {
		v = *iteList;
		if (v->getIdViewer() == idViewer)
			return v;
		iteList++;
	}
	
	return NULL;
}

/*
* Aggiunge un viewer nella lista _viewerList
*/
bool viewerHandler::addViewer(viewer *viewer) {
	if (getViewer(viewer->getIdViewer()))
		return false;
	else	
		_viewerList.push_back(viewer);
	
	return true;
}

/*
* Elimina il viewer dalla lista _viewerList e ne esegue il delete
*/
bool viewerHandler::quitViewer(unsigned int idViewer) {
	viewer *viewer = getViewer(idViewer);

	if (viewer) {
		_viewerList.remove(getViewer(idViewer));
		delete viewer;
		return true;
	}
	else
		return false;
}

/*
* Elimina tutti i viewer contenuti nella lista _viewerList
*/
void viewerHandler::deleteAllViewer() {
	list<viewer*>::iterator iteList = _viewerList.begin();
    viewer *v;

	while (iteList != _viewerList.end()) {
		v = *iteList;
		if (v) {
			v->getViewerDrawing()->destroyWindow();
			delete v;
		}
		iteList++;
	}
	
	_viewerList.clear();
}

/*
* Esegue il metodo update di ciascun viewer che mostra dati idData
*/
void viewerHandler::updateViewerByIdData(unsigned int idData) {
	if (idData) {
		list<unsigned int> *l = getDataHandler()->getIdViewerByIdData(idData);
		unsigned int i;

		list<unsigned int>::iterator iteList = l->begin();
		while (iteList != l->end()) {
			i = *iteList;
			if (i)
				getViewer(i)->updateViewer();
			iteList++;
		}
		delete l;
	}
}

/*
* Rimuove da ciascun viewer un dato
*/
void viewerHandler::removeSingleAppData(unsigned int idData) {
	list<unsigned int> *l = getDataHandler()->getIdViewerByIdData(idData);
	unsigned int i;
	
	list<unsigned int>::iterator iteList = l->begin();
	while (iteList != l->end()) {
		i = *iteList;
		if (i)
			getViewer(i)->removeSingleAppData();
		iteList++;
	}
	delete l;
}

/*
* Esegue il metodo update di ciascun viewer
*/
void viewerHandler::updateViewer() {
	list<viewer*>::iterator iteList = _viewerList.begin();
    viewer *v;

	while (iteList != _viewerList.end()) {
		v = *iteList;
		if (v)
			v->updateViewer();
		iteList++;
	}
}