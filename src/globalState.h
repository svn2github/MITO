/**
 * \file globalState.h
 * \brief File per la definizione di nuovi tipi utilizzati per la ROI
 * \author ICAR-CNR Napoli
 */

#ifndef _globalState_h_
#define _globalState_h_

#ifndef __itkImageToVTKImageFilter_h
#include "itkImageToVTKImageFilter.h"
#endif
#include <vector>
#include <vtkSetGet.h>

/**
* \brief Tipo enumerativo che rappresenta le differenti modalità con cui è possibile selezionare una ROI
*/
enum toolbarModeType 
{
	POLYGON_DRAWING_MODE,
	RECTANGLE_MODE,
	PENCIL_MODE,
	ANGLE_MODE,
	EXTRACTION_MODE,
	LENGTH_MODE,
};


/**
* \brief Tipo enumerativo che rappresenta i differenti stati relativi alle funzionalità per la ROI
*/
enum stateRoi
{
	INACTIVE_STATE,
	DRAWING_STATE,
	EDITING_STATE
};

#endif _globalState_h_