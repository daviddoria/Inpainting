#ifndef Inpainting_h
#define Inpainting_h

#include "Mask/Mask.h"

class Inpainting
{
public:
  typedef itk::Image<unsigned char> ImageType;

  Inpainting();

  /** Set the image to inpaint. */
  void SetImage(const ImageType* const image);

  /** Set the inpainting mask. */
  void SetMask(const Mask* const mask);
  
private:
  ImageType::Pointer Image;
  Mask::Pointer MaskImage;

};

#endif
