#include "FastDigitalInpainting.h"

#include <sstream>

#include "Mask/ITKHelpers/ITKHelpers.h"

FastDigitalInpainting::FastDigitalInpainting() : NumberOfCompletedIterations(0), NumberOfIterations(100)
{
  this->Kernel = KernelType::New();

  this->CurrentMask = Mask::New();
  this->CurrentImage = ImageType::New();
}

void FastDigitalInpainting::CreateGaussianKernel()
{
  // Create a tiny image whose center is (0,0) and is filled with 1/8.
  // (every pixel of the 9 contributes except the center pixel)

  itk::Size<2> size = {{3,3}};
  //itk::ImageRegion<2> region = ITKHelpers::CornerRegion(size);
  itk::Index<2> corner = {{-1, -1}};
  itk::ImageRegion<2> region(corner, size);

  this->Kernel->SetRegions(region);
  this->Kernel->Allocate();
  this->Kernel->FillBuffer(0.0f);

  itk::ImageRegionIteratorWithIndex<KernelType> kernelIterator(this->Kernel,
                                                               this->Kernel->GetLargestPossibleRegion());
  while(!kernelIterator.IsAtEnd())
    {
    itk::Index<2> currentIndex = kernelIterator.GetIndex();
    if(currentIndex[0] == 0 || currentIndex[1] == 0)
      {
      kernelIterator.Set(.176765); // Prescribed in the paper
      }
    else
      {
      kernelIterator.Set(.073235); // Prescribed in the paper
      }
    ++kernelIterator;
    }

  itk::Index<2> centerPixel = {{0,0}};
  this->Kernel->SetPixel(centerPixel, 0.0f);
}

void FastDigitalInpainting::CreateConstantKernel()
{
  // Create a tiny image whose center is (0,0) and is filled with 1/8.
  // (every pixel of the 9 contributes except the center pixel)

  itk::Size<2> size = {{3,3}};
  //itk::ImageRegion<2> region = ITKHelpers::CornerRegion(size);
  itk::Index<2> corner = {{-1, -1}};
  itk::ImageRegion<2> region(corner, size);
  
  this->Kernel->SetRegions(region);
  this->Kernel->Allocate();
  this->Kernel->FillBuffer(1.0f/8.0f);

  itk::Index<2> centerPixel = {{0,0}};
  this->Kernel->SetPixel(centerPixel, 0.0f);
}

void FastDigitalInpainting::SetNumberOfIterations(const unsigned int numberOfIterations)
{
  this->NumberOfIterations = numberOfIterations;
}

void FastDigitalInpainting::Inpaint()
{
  //CreateConstantKernel();
  CreateGaussianKernel();
  
  ITKHelpers::DeepCopy(this->Image.GetPointer(), this->CurrentImage.GetPointer());

  this->CurrentMask->DeepCopyFrom(this->MaskImage.GetPointer());

  for(unsigned int iteration = 0; iteration < this->NumberOfIterations; ++iteration)
  {
    std::cout << "Iteration " << this->NumberOfCompletedIterations << "..." << std::endl;
    Iterate();
  }

  ITKHelpers::DeepCopy(this->CurrentImage.GetPointer(), this->Output.GetPointer());
}

void FastDigitalInpainting::Iterate()
{
  // Create a temp image to store the values so that the new values are not used during
  // this iteration.
  ImageType::Pointer tempImage = ImageType::New();
  ITKHelpers::DeepCopy(this->CurrentImage.GetPointer(), tempImage.GetPointer());

  Mask::Pointer tempMask = Mask::New();
  tempMask->DeepCopyFrom(this->CurrentMask);

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(this->Image, this->HoleBoundingBox);

  while(!imageIterator.IsAtEnd())
    {
    itk::Index<2> indexToFill = imageIterator.GetIndex();
    // Only modify pixels that are in the original hole
    if(this->MaskImage->IsHole(indexToFill))
    {
      itk::ImageRegionIteratorWithIndex<KernelType> kernelIterator(this->Kernel,
                                                                   this->Kernel->GetLargestPossibleRegion());
      float weightedSum = 0.0f;
      float weightSum = 0.0f;
      unsigned int pixelsUsed = 0;
      while(!kernelIterator.IsAtEnd())
        {
        if(!(kernelIterator.GetIndex()[0] == 0 && kernelIterator.GetIndex()[1] == 0))
          {
          itk::Index<2> currentIndex = imageIterator.GetIndex() +
                                       ITKHelpers::IndexToOffset(kernelIterator.GetIndex());
          // Only use pixels that were originally valid or have been previously filled
          if(this->CurrentMask->IsValid(currentIndex))
            {
            weightSum += kernelIterator.Get();
            weightedSum += tempImage->GetPixel(currentIndex) * kernelIterator.Get();
            pixelsUsed++;
            }
          } // end if we are not in the center of the kernel (this pixel is not counted)
        ++kernelIterator;
        } // end loop over kernel

      if(pixelsUsed > 0)
      {
        tempMask->SetValid(imageIterator.GetIndex());
        if(weightSum > 0)
          {
          weightedSum /= weightSum;
          tempImage->SetPixel(imageIterator.GetIndex(), weightedSum);
          }
      }
    } // end if is valid
    ++imageIterator;
    }

  // Copy the filled values to be used in the next iteration
  ITKHelpers::DeepCopy(tempImage.GetPointer(), this->CurrentImage.GetPointer());
  this->CurrentMask->DeepCopyFrom(tempMask);

  std::stringstream ssMask;
  ssMask << "mask_" << Helpers::ZeroPad(this->NumberOfCompletedIterations, 3) << ".png";
  ITKHelpers::WriteImage(this->CurrentMask.GetPointer(), ssMask.str());

  std::stringstream ssImage;
  ssImage << "image_" << Helpers::ZeroPad(this->NumberOfCompletedIterations, 3) << ".png";
  ITKHelpers::WriteImage(this->CurrentImage.GetPointer(), ssImage.str());

  this->NumberOfCompletedIterations++;
}
