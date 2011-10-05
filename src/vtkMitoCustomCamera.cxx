/*

*/

#include "vtkMitoCustomCamera.h"

#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPerspectiveTransform.h>
#include <vtkMath.h>

vtkStandardNewMacro(vtkMitoCustomCamera);

//----------------------------------------------------------------------------
// Compute the projection transform matrix. This is used in converting
// between view and world coordinates.
void vtkMitoCustomCamera::CustomComputeProjectionTransform(double aspect, double nearz, double farz)
{
  this->ProjectionTransform->Identity();

  // apply user defined transform last if there is one
  if ( this->UserTransform )
    {
    this->ProjectionTransform->Concatenate( this->UserTransform->GetMatrix() );
    }

  // adjust Z-buffer range
  this->ProjectionTransform->AdjustZBuffer( -1, +1, nearz, farz );

  if ( this->ParallelProjection)
    {
    // set up a rectangular parallelipiped

    double width = this->ParallelScale * aspect;
    double height = this->ParallelScale;

    double xmin = ( this->WindowCenter[0] - 1.0 ) * width;
    double xmax = ( this->WindowCenter[0] + 1.0 ) * width;
    double ymin = ( this->WindowCenter[1] - 1.0 ) * height;
    double ymax = ( this->WindowCenter[1] + 1.0 ) * height;

    this->ProjectionTransform->Ortho( xmin, xmax, ymin, ymax,
                                      this->ClippingRange[0],
                                      this->ClippingRange[1] );
    }
  else
    {
    // set up a perspective frustum

    double tmp = tan( vtkMath::RadiansFromDegrees( this->ViewAngle ) / 2. );
    double width;
    double height;
    if ( this->UseHorizontalViewAngle )
      {
      width = this->ClippingRange[0] * tmp;
      height = this->ClippingRange[0] * tmp / aspect;
      }
    else
      {
      width = this->ClippingRange[0] * tmp * aspect;
      height = this->ClippingRange[0] * tmp;
      }

    double xmin = ( this->WindowCenter[0] - 1.0 ) * width;
    double xmax = ( this->WindowCenter[0] + 1.0 ) * width;
    double ymin = ( this->WindowCenter[1] - 1.0 ) * height;
    double ymax = ( this->WindowCenter[1] + 1.0 ) * height;

    this->ProjectionTransform->Frustum( xmin, xmax, ymin, ymax,
                                        this->ClippingRange[0],
                                        this->ClippingRange[1] );
    }
  
  // modified by Luigi
  if ( this->Stereo )
    {
    // set up a shear for stereo views
    if ( this->LeftEye )
      {
      this->ProjectionTransform->Stereo( -this->EyeAngle/2,
                                          this->Distance-this->Parallax );
      }
    else
      {
      this->ProjectionTransform->Stereo( +this->EyeAngle/2,
                                          this->Distance-this->Parallax );
      }
    }

  if ( this->ViewShear[0] != 0.0 || this->ViewShear[1] != 0.0 )
    {
    this->ProjectionTransform->Shear( this->ViewShear[0],
                                      this->ViewShear[1],
                                      this->ViewShear[2] * (this->Distance-this->Parallax) );
    }

}


//----------------------------------------------------------------------------
// Return the projection transform matrix. See ComputeProjectionTransform.
vtkMatrix4x4 *vtkMitoCustomCamera::GetProjectionTransformMatrix(double aspect,
                                                      double nearz,
                                                      double farz)
{
  this->CustomComputeProjectionTransform(aspect, nearz, farz);

  // return the transform
  return this->ProjectionTransform->GetMatrix();
}

//----------------------------------------------------------------------------
// Return the projection transform object. See ComputeProjectionTransform.
vtkPerspectiveTransform *vtkMitoCustomCamera::GetProjectionTransformObject(double aspect,
                                                                 double nearz,
                                                                 double farz)
{
  this->CustomComputeProjectionTransform(aspect, nearz, farz);

  // return the transform
  return this->ProjectionTransform;
}

void vtkMitoCustomCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}