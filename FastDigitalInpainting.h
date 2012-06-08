// Based on "Image Inpainting" by Bertalmio, Sapiro, Caselles, and Ballester
#ifndef FastDigitalInpainting_h
#define FastDigitalInpainting_h

#include "Inpainting.h"

// ITK
#include "itkImage.h"

// Submodules
#include "Mask/Mask.h"

class FastDigitalInpainting : public Inpainting
{
public:

  typedef itk::Image<float, 2> KernelType;

  FastDigitalInpainting();

  /** Perform an iteration of the fast digital inpainting algorithm. */
  void Iterate();

  void SetNumberOfIterations(const unsigned int numberOfIterations);

  void Inpaint();

private:
  void CreateConstantKernel();
  void CreateGaussianKernel();
  
  KernelType::Pointer Kernel;
  Mask::Pointer CurrentMask;
  ImageType::Pointer CurrentImage;
  
  unsigned int NumberOfCompletedIterations;
  unsigned int NumberOfIterations;
};

#endif
