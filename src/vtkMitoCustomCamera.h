/*

*/

#ifndef _vtkMitoCustomCamera_h_
#define _vtkMitoCustomCamera_h_

#include <vtkOpenGLCamera.h>


class vtkMitoCustomCamera : public vtkOpenGLCamera
{
public:
  static vtkMitoCustomCamera *New();
  vtkTypeMacro(vtkMitoCustomCamera, vtkOpenGLCamera);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Luigi
  // Description:
  // This method return the parallax value
  double GetParallax() { return Parallax; }

  // Description:
  // This method set the parallax value
  void SetParallax(double parallax) { Parallax = parallax; }
  // end Luigi

  // Description:
  // Return the projection transform matrix, which converts from camera
  // coordinates to viewport coordinates.  The 'aspect' is the
  // width/height for the viewport, and the nearz and farz are the
  // Z-buffer values that map to the near and far clipping planes.
  // The viewport coordinates of a point located inside the frustum are in the
  // range ([-1,+1],[-1,+1],[nearz,farz]).
   virtual vtkMatrix4x4 *GetProjectionTransformMatrix(double aspect, double nearz, double farz);
  
  // Description:
  // Return the projection transform matrix, which converts from camera
  // coordinates to viewport coordinates.  The 'aspect' is the
  // width/height for the viewport, and the nearz and farz are the
  // Z-buffer values that map to the near and far clipping planes.
  // The viewport coordinates of a point located inside the frustum are in the
  // range ([-1,+1],[-1,+1],[nearz,farz]).
  virtual vtkPerspectiveTransform *GetProjectionTransformObject(double aspect, double nearz, double farz);
  

protected:  
  vtkMitoCustomCamera() {};
  ~vtkMitoCustomCamera() {};

  virtual void CustomComputeProjectionTransform(double aspect, double nearz, double farz);

  double Parallax;

private:

  vtkMitoCustomCamera(const vtkMitoCustomCamera&);  // Not implemented.
  void operator=(const vtkMitoCustomCamera&);  // Not implemented.
};

#endif // _vtkMitoCustomCamera_h_