/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkInteractorStyleWII.h,v $

  Author:	 Luigi Gallo (ICAR-CNR), Aniello Minutolo (ICAR-CNR)

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInteractorStyleWII - interactive manipulation of the camera
// .SECTION Description
// vtkInteractorStyleWII allows the user to interactively manipulate
// (rotate, pan, etc.) the camera, the viewpoint of the scene,
// by using the WII controller.

#ifndef __vtkInteractorStyleWII_h
#define __vtkInteractorStyleWII_h

#include "vtkInteractorStyle.h"

class VTK_RENDERING_EXPORT vtkInteractorStyleWII : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleWII *New();
  vtkTypeRevisionMacro(vtkInteractorStyleWII,vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Event bindings controlling the effects of pressing mouse buttons
  // or moving the mouse.
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  //virtual void OnMiddleButtonDown();
  //virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  // virtual void OnMouseWheelForward();
  // virtual void OnMouseWheelBackward();

  // aggiunti da me per il controllo della tastiera
  // virtual void OnChar();
  virtual void OnKeyDown();
  virtual void OnKeyUp();

  //Wiimote Events
  virtual void onWiiMotion();
  virtual void onWiiADown();
  virtual void onWiiAUp();
  virtual void onWiiBDown();
  virtual void onWiiBUp();
  virtual void onWiiMinusDown();
  virtual void onWiiMinusUp();
  virtual void onWiiHomeDown();
  virtual void onWiiHomeUp();
  virtual void onWiiPlusDown();
  virtual void onWiiPlusUp();
  virtual void onWiiOneDown();
  virtual void onWiiOneUp();
  virtual void onWiiTwoDown();
  virtual void onWiiTwoUp();
  virtual void onWiiCrossDown();
  virtual void onWiiCrossUp();


  // These methods for the different interactions in different modes
  // are overridden in subclasses to perform the correct motion. Since
  // they are called by OnTimer, they do not have mouse coord parameters
  // (use interactor's GetEventPosition and GetLastEventPosition)
  virtual void Rotate();
  virtual void Spin();
    
  virtual void Pan();

  virtual void Dolly();
  
protected:
  vtkInteractorStyleWII();
  ~vtkInteractorStyleWII();

  double MotionFactor;

  virtual void Dolly(double factor);

private:
  vtkInteractorStyleWII(const vtkInteractorStyleWII&);  // Not implemented.
  void operator=(const vtkInteractorStyleWII&);  // Not implemented.
};

#endif
