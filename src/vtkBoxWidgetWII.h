/*

*/


#ifndef __vtkBoxWidgetWII_h
#define __vtkBoxWidgetWII_h

#include <vtkCellPicker.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkBoxWidget.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

class VTK_WIDGETS_EXPORT vtkBoxWidgetWii : public vtkBoxWidget
{
public:
  static vtkBoxWidgetWii *New();
  vtkTypeMacro(vtkBoxWidgetWii,vtkBoxWidget);
  //void PrintSelf(ostream& os, vtkIndent indent);

  virtual void SetEnabled(int);

protected:
  vtkBoxWidgetWii();
  virtual ~vtkBoxWidgetWii() {}
    
  // Handles the events
  static void ProcessEvents(vtkObject* object, 
                            unsigned long event,
                            void* clientdata, 
                            void* calldata);

  // start Wiimote
  // ProcessEvents() dispatches to these Wii methods.
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

  int idFaceSelected;
  //virtual void WiiRotate(int X, int Y, double *p1, double *p2, double *vpn);
  // end Wiimote

};


#endif // __vtkBoxWidgetWII_h