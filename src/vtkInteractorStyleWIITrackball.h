/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkInteractorStyleWIITrackball.h,v $

  Author:	 Luigi Gallo (ICAR-CNR)

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInteractorStyleWIITrackball - interactive manipulation of the camera
// .SECTION Description
// vtkInteractorStyleWII allows the user to interactively manipulate
// (rotate, pan, etc.) the camera, the viewpoint of the scene,
// by using the WII controller.

#ifndef __vtkInteractorStyleWIITrackball_h
#define __vtkInteractorStyleWIITrackball_h

#include "vtkInteractorStyle.h"

class vtkRenderWindowInteractor;

class VTK_RENDERING_EXPORT vtkInteractorStyleWIITrackball : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleWIITrackball *New();
  vtkTypeRevisionMacro(vtkInteractorStyleWIITrackball,vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent);  

  virtual void setupEventObservers(vtkRenderWindowInteractor* i);

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


  // Description:
  // Set the apparent sensitivity of the interactor style to mouse motion.
  vtkSetMacro(MotionFactor,double);
  vtkGetMacro(MotionFactor,double);

  
protected:	
    double MotionFactor;

	vtkInteractorStyleWIITrackball();
	~vtkInteractorStyleWIITrackball();

	
  // Description:
  // Main process event method
  static void ProcessEvents(vtkObject* object, 
                            unsigned long event,
                            void* clientdata, 
                            void* calldata);

	void Dolly(double factor);
  

private:
  vtkInteractorStyleWIITrackball(const vtkInteractorStyleWIITrackball&);  // Not implemented.
  void operator=(const vtkInteractorStyleWIITrackball&);  // Not implemented.
};

#endif
