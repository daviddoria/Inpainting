// Based on "Image Inpainting" by Bertalmio, Sapiro, Caselles, and Ballester
#ifndef FastDigitalInpainting_h
#define FastDigitalInpainting_h

#include "Inpainting.h"

// ITK
#include "itkImage.h"

// Submodules
#include "Mask/Mask.h"

class FastDigitalInpainting : Inpainting
{
public:

  FastDigitalInpainting();

  /** Perform an iteration of the fast digital inpainting algorithm. */
  void Iterate();

  
private:

  unsigned int NumberOfCompletedIterations;
};

#endif
