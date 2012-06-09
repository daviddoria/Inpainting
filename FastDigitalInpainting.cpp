#include "FastDigitalInpainting.h"

#include <sstream>

#include "Mask/ITKHelpers/ITKHelpers.h"
#include "Mask/MaskOperations.h"

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

  /* .073235 .176765 .073235
   * .176765     0   .176765
   * .073235 .176765 .073235
   */

  itk::Size<2> size = {{3,3}};
  //itk::ImageRegion<2> region = ITKHelpers::CornerRegion(size);
  itk::Index<2> corner = {{-1, -1}};
  itk::ImageRegion<2> region(corner, size);
  std::cout << "Kernel region: " << region << std::endl;

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
      kernelIterator.Set(0.176765f); // Prescribed in the paper
      }
    else
      {
      kernelIterator.Set(0.073235f); // Prescribed in the paper
      }
    ++kernelIterator;
    }

  itk::Index<2> centerPixel = {{0,0}};
  this->Kernel->SetPixel(centerPixel, 0.0f);

  // Verify the sum
  float sum = 0.0f;
  kernelIterator.GoToBegin();
  while(!kernelIterator.IsAtEnd())
    {
    sum += kernelIterator.Get();
    ++kernelIterator;
    }
  std::cout << "Kernel sum: " << sum << std::endl;
}

void FastDigitalInpainting::CreateConstantKernel()
{
  // Create a tiny image whose center is (0,0) and is filled with 1/8.
  // (every pixel of the 9 contributes except the center pixel)

  /* 1/8 1/8 1/8
   * 1/8  0  1/8
   * 1/8 1/8 1/8
   */
  itk::Size<2> size = {{3,3}};
  //itk::ImageRegion<2> region = ITKHelpers::CornerRegion(size);
  itk::Index<2> corner = {{-1, -1}};
  itk::ImageRegion<2> region(corner, size);
  
  this->Kernel->SetRegions(region);
  this->Kernel->Allocate();
   // Since there are 8 non-zero pixels in this kernel, 1/8 is giving each of these non-zero pixels equal weight.
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

  MaskOperations::SetHolePixelsToConstant(this->Image.GetPointer(), 0, this->MaskImage);

  ITKHelpers::DeepCopy(this->Image.GetPointer(), this->CurrentImage.GetPointer());

  this->CurrentMask->DeepCopyFrom(this->MaskImage.GetPointer());

  for(unsigned int iteration = 0; iteration < this->NumberOfIterations; ++iteration)
  {
    std::cout << "Iteration " << this->NumberOfCompletedIterations << "..." << std::endl;
    //IterateSmart();
    //IterateNaive();
    IterateFDI();
  }

  ITKHelpers::DeepCopy(this->CurrentImage.GetPointer(), this->Output.GetPointer());
}

void FastDigitalInpainting::IterateSmart()
{
  // Create a temp image to store the values so that the new values are not used during
  // this iteration.
  ImageType::Pointer tempImage = ImageType::New();
  ITKHelpers::DeepCopy(this->CurrentImage.GetPointer(), tempImage.GetPointer());

  Mask::Pointer tempMask = Mask::New();
  tempMask->DeepCopyFrom(this->CurrentMask);

  itk::ImageRegionIteratorWithIndex<Mask> maskIterator(this->MaskImage, this->HoleBoundingBox);

  while(!maskIterator.IsAtEnd())
  {
    itk::Index<2> indexToFill = maskIterator.GetIndex();
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
        itk::Index<2> currentIndex = indexToFill +
                                       ITKHelpers::IndexToOffset(kernelIterator.GetIndex());
        if(this->FullRegion.IsInside(currentIndex)) // region is inside the image
        {
          // Only use pixels that were originally valid or have been previously filled
          if(this->CurrentMask->IsValid(currentIndex))
          {
            weightSum += kernelIterator.Get();
            weightedSum += static_cast<float>(this->CurrentImage->GetPixel(currentIndex)) * kernelIterator.Get();
            pixelsUsed++;
          }
        } // end if we are not in the center of the kernel (this pixel is not counted)
        ++kernelIterator;
      } // end loop over kernel

      if(pixelsUsed > 0)
      {
        if(weightSum > 0.0f)
          {
          if(weightSum < (1.0f - 1e-6))
            {
            std::cout << "Index: " << indexToFill
                      << " Pixels used: " << pixelsUsed
                      << " Weighted sum: " << weightedSum
                      << " weightSum: " << weightSum << std::endl;
            }
          tempMask->SetValid(indexToFill);

          weightedSum /= weightSum;
          tempImage->SetPixel(indexToFill, weightedSum);
          }
      } // end if pixelsUsed > 0
    } // end if we are in the hole
    ++maskIterator;
  } // end main image loop

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

void FastDigitalInpainting::IterateMedium()
{
  // Create a temp image to store the values so that the new values are not used during
  // this iteration.
  ImageType::Pointer tempImage = ImageType::New();
  ITKHelpers::DeepCopy(this->CurrentImage.GetPointer(), tempImage.GetPointer());

  Mask::Pointer tempMask = Mask::New();
  tempMask->DeepCopyFrom(this->CurrentMask);

  itk::ImageRegionIteratorWithIndex<Mask> maskIterator(this->MaskImage, this->HoleBoundingBox);

  while(!maskIterator.IsAtEnd())
  {
    itk::Index<2> indexToFill = maskIterator.GetIndex();
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
        itk::Index<2> currentIndex = indexToFill +
                                       ITKHelpers::IndexToOffset(kernelIterator.GetIndex());
        if(this->FullRegion.IsInside(currentIndex)) // region is inside the image
        {
          weightSum += kernelIterator.Get();
          weightedSum += static_cast<float>(this->CurrentImage->GetPixel(currentIndex)) * kernelIterator.Get();
          pixelsUsed++;
        } // end if we are not in the center of the kernel (this pixel is not counted)
        ++kernelIterator;
      } // end loop over kernel

      if(pixelsUsed > 0)
      {
        if(weightSum > 0.0f)
          {
          tempMask->SetValid(indexToFill);

          weightedSum /= weightSum;
          tempImage->SetPixel(indexToFill, weightedSum);
          }
      } // end if pixelsUsed > 0
    } // end if we are in the hole
    ++maskIterator;
  } // end main image loop

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


void FastDigitalInpainting::IterateNaive()
{
  itk::ImageRegionIteratorWithIndex<Mask> maskIterator(this->MaskImage, this->HoleBoundingBox);

  while(!maskIterator.IsAtEnd())
  {
    itk::Index<2> indexToFill = maskIterator.GetIndex();
    // Only modify pixels that are in the original hole
    if(this->MaskImage->IsHole(indexToFill))
    {
      itk::ImageRegionIteratorWithIndex<KernelType> kernelIterator(this->Kernel,
                                                                   this->Kernel->GetLargestPossibleRegion());
      float weightedSum = 0.0f;
      float weightSum = 0.0f;

      while(!kernelIterator.IsAtEnd())
      {
        // kernelIterator.GetIndex() returns from (-1,-1), to (1,1) (the relative location in the kernel),
        // so currentIndex for indexToFill=(50,50) ends up in the range (49,49) to (51,51)
        itk::Index<2> currentIndex = indexToFill +
                                       ITKHelpers::IndexToOffset(kernelIterator.GetIndex());
        if(this->FullRegion.IsInside(currentIndex)) // pixel is inside the image
        {
          // add the value of the kernel pixel to the total weights used
          weightSum += kernelIterator.Get();

          // Weight the pixel value by the kernel value
          weightedSum += static_cast<float>(this->CurrentImage->GetPixel(currentIndex)) * kernelIterator.Get();
        }
        ++kernelIterator;
      } // end loop over kernel

      // Normalize for cases where the entire kernel was not used (the image boundary)
      weightedSum /= weightSum;
      this->CurrentImage->SetPixel(indexToFill, weightedSum);
    } // end if we are in the hole
    ++maskIterator;
  } // end main image loop

  std::stringstream ssImage;
  ssImage << "image_" << Helpers::ZeroPad(this->NumberOfCompletedIterations, 3) << ".png";
  ITKHelpers::WriteImage(this->CurrentImage.GetPointer(), ssImage.str());

  this->NumberOfCompletedIterations++;
}


void FastDigitalInpainting::IterateFDI()
{
  itk::ImageRegionIteratorWithIndex<Mask> maskIterator(this->MaskImage, this->HoleBoundingBox);

  while(!maskIterator.IsAtEnd())
  {
    itk::Index<2> indexToFill = maskIterator.GetIndex();

    itk::ImageRegionIteratorWithIndex<KernelType> kernelIterator(this->Kernel,
                                                                  this->Kernel->GetLargestPossibleRegion());
    float weightedSum = 0.0f;
    float weightSum = 0.0f;

    while(!kernelIterator.IsAtEnd())
    {
      // kernelIterator.GetIndex() returns from (-1,-1), to (1,1) (the relative location in the kernel),
      // so currentIndex for indexToFill=(50,50) ends up in the range (49,49) to (51,51)
      itk::Index<2> currentIndex = indexToFill +
                                      ITKHelpers::IndexToOffset(kernelIterator.GetIndex());
      if(this->FullRegion.IsInside(currentIndex)) // pixel is inside the image
      {
        // add the value of the kernel pixel to the total weights used
        weightSum += kernelIterator.Get();

        // Weight the pixel value by the kernel value
        weightedSum += static_cast<float>(this->CurrentImage->GetPixel(currentIndex)) * kernelIterator.Get();
      }
      ++kernelIterator;
    } // end loop over kernel

    // Normalize for cases where the entire kernel was not used (the image boundary)
    weightedSum /= weightSum;
    this->CurrentImage->SetPixel(indexToFill, weightedSum);

    ++maskIterator;
  } // end main image loop

  ImageType::Pointer output = ImageType::New();
  ITKHelpers::DeepCopy(this->Image.GetPointer(), output.GetPointer());
  MaskOperations::CopyInHoleRegion(this->CurrentImage.GetPointer(), output.GetPointer(), this->MaskImage);

  ITKHelpers::DeepCopy(output.GetPointer(), this->CurrentImage.GetPointer());

  std::stringstream ssImage;
  ssImage << "image_" << Helpers::ZeroPad(this->NumberOfCompletedIterations, 3) << ".png";
  ITKHelpers::WriteImage(this->CurrentImage.GetPointer(), ssImage.str());

  this->NumberOfCompletedIterations++;
}
