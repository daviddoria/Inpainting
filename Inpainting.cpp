#include "Inpainting.h"

#include "Mask/ITKHelpers/ITKHelpers.h"
Inpainting::Inpainting()
{

}

void Inpainting::SetImage(const ImageType* const image)
{
  ITKHelpers::DeepCopy(image, this->Image.GetPointer());
}

void Inpainting::SetMask(const Mask* const mask)
{
  this->MaskImage->DeepCopyFrom(mask);
}
