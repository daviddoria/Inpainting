#ifndef Inpainting_h
#define Inpainting_h

#include "Mask/Mask.h"

class Inpainting
{
public:
  typedef itk::Image<unsigned char> ImageType;
  
  Inpainting();

  /** Set the image to inpaint. */
  virtual void SetImage(const ImageType* const image);

  /** Set the inpainting mask. */
  virtual void SetMask(const Mask* const mask);

  /** Perform the entire inpainting operation. */
  virtual void Inpaint() = 0;

  /** Get the output. */
  ImageType* GetOutput();
  
protected:
  ImageType::Pointer Image;
  ImageType::Pointer Output;
  Mask::Pointer MaskImage;

  itk::ImageRegion<2> HoleBoundingBox;
  itk::ImageRegion<2> FullRegion;
};

#endif
