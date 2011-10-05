/**
 * \file viewer3d.h
 * \brief File per la gestione dei dati da visualizzare in modalità 3D
 * \author ICAR-CNR Napoli
 */

#ifndef _viewer3d_h_
#define _viewer3d_h_

#include "viewer.h"

class viewer3d : public viewer {

public:
	/** Costruttore con parametri viewerDrawing e identificativo del visualizzatore */
	viewer3d(viewerDrawing* viewerDrawing, unsigned int id);

	/** Distruttore */
	~viewer3d();
};

#endif _viewer3d_h_