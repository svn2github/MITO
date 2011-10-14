/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMitoInteractorStyleTrackball.h,v $

  Author:	 Luigi Gallo (ICAR-CNR)

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMitoInteractorStyleTrackball - interactive manipulation of the camera
// .SECTION Description
// vtkInteractorStyleWII allows the user to interactively manipulate
// (rotate, pan, etc.) the camera, the viewpoint of the scene,
// by using the WII controller.

#ifndef __vtkMitoInteractorStyleTrackball_h
#define __vtkMitoInteractorStyleTrackball_h

#include "vtkInteractorStyleTrackballCamera.h"

class vtkRenderWindowInteractor;

class VTK_RENDERING_EXPORT vtkMitoInteractorStyleTrackball : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkMitoInteractorStyleTrackball *New();
  vtkTypeRevisionMacro(vtkMitoInteractorStyleTrackball,vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent);  

  virtual void OnMouseMove();

  // These methods for the different interactions in different modes
  // are overridden in subclasses to perform the correct motion. Since
  // they are called by OnTimer, they do not have mouse coord parameters
  // (use interactor's GetEventPosition and GetLastEventPosition)
  virtual void Rotate();
  virtual void Spin();

protected:	

	vtkMitoInteractorStyleTrackball();
	~vtkMitoInteractorStyleTrackball();
  

private:
  vtkMitoInteractorStyleTrackball(const vtkMitoInteractorStyleTrackball&);  // Not implemented.
  void operator=(const vtkMitoInteractorStyleTrackball&);  // Not implemented.
};

#endif //__vtkMitoInteractorStyleTrackball_h
