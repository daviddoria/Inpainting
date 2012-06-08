#include "Inpainting.h"

#include "Mask/ITKHelpers/ITKHelpers.h"
#include "Mask/MaskOperations.h"

Inpainting::Inpainting()
{
  this->Image = ImageType::New();
  this->Output = ImageType::New();
  this->MaskImage = Mask::New();
}

void Inpainting::SetImage(const ImageType* const image)
{
  // Initialize the output the input
  ITKHelpers::DeepCopy(image, this->Output.GetPointer());

  // Copy the input
  ITKHelpers::DeepCopy(image, this->Image.GetPointer());
}

void Inpainting::SetMask(const Mask* const mask)
{
  this->MaskImage->DeepCopyFrom(mask);

  this->HoleBoundingBox = MaskOperations::ComputeHoleBoundingBox(this->MaskImage);
}
