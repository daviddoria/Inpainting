#include "vtkInpaintingCriminisi.h"
#include "vtkImageIsophotes.h"
#include <vtkBinaryBlobPerimeter2D.h>

#include "vtkObjectFactory.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "vtkImageConvolve.h"
#include "vtkImageData.h"
#include "vtkImageCorrelation.h"
#include "vtkImageShiftScale.h"

vtkStandardNewMacro(vtkInpaintingCriminisi);

void vtkInpaintingCriminisi::Iterate(int iteration)
{
  vtkSmartPointer<vtkImageIsophotes> isophotesFilter =
    vtkSmartPointer<vtkImageIsophotes>::New();

  FindFrontPixels();
  ComputePriorities();
}

void vtkInpaintingCriminisi::FindFrontPixels()
{
  this->Border.clear();
  
  vtkSmartPointer<vtkBinaryBlobPerimeter2D> blobPerimeter =
    vtkSmartPointer<vtkBinaryBlobPerimeter2D>::New();
  blobPerimeter->SetInputData(this->Mask);

  int extent[6];
  this->Mask->GetExtent(extent);

  for(unsigned int i = extent[0]; i < extent[1]; i++)
    {
    for(unsigned int j = extent[2]; j < extent[3]; j++)
      {
      int* pixel = static_cast<int*>(blobPerimeter->GetOutput()->GetScalarPointer(i,j,0));
      if(pixel[0] >= 1)
        {
        Pixel pixel;
      
        pixel.x = i;
        pixel.y = j;
        this->Border.push_back(pixel);
        }
      }
    }

  this->ClearImageInRegion(this->Mask, blobPerimeter->GetOutput());
}

void vtkInpaintingCriminisi::ComputePriorities()
{
  for(unsigned int i = 0; i < this->Border.size(); i++)
    {
    Pixel pixel = this->Border[i];
    double priority = ComputePriority(pixel.x, pixel.y);
    pixel.Priority = priority;
    this->Border[i] = pixel;
    }
}

double vtkInpaintingCriminisi::ComputePriority(int x, int y)
{
  return ComputeConfidenceTerm(x, y) + ComputeDataTerm(x, y);
}

double vtkInpaintingCriminisi::ComputeConfidenceTerm(int x, int y)
{
  // sum of the confidences of patch pixels in the source region / area of the patch
}

double vtkInpaintingCriminisi::ComputeDataTerm(int x, int y)
{
  double alpha = 255; // for grey scale images
  // D(p) = |dot(isophote direction at p, normal of the front at p)|/alpha
}

void vtkInpaintingCriminisi::FindBestPatchMatch(vtkImageData* patch, vtkImageData* image, int bestPatchCenter[2])
{
  unsigned char imageRange[2];
  vtkUnsignedCharArray::SafeDownCast(image->GetPointData()->GetScalars())->GetValueRange(imageRange);

  unsigned char patchRange[2];
  vtkUnsignedCharArray::SafeDownCast(patch->GetPointData()->GetScalars())->GetValueRange(patchRange);

  // Compute the correlation
  vtkSmartPointer<vtkImageShiftScale> imageNormalizeFilter =
    vtkSmartPointer<vtkImageShiftScale>::New();
  imageNormalizeFilter->SetShift(-imageRange[0]);
  imageNormalizeFilter->SetScale(255/(imageRange[1]-imageRange[0]));
  imageNormalizeFilter->SetInputData(image);
  imageNormalizeFilter->Update();

  vtkSmartPointer<vtkImageShiftScale> patchNormalizeFilter =
    vtkSmartPointer<vtkImageShiftScale>::New();
  patchNormalizeFilter->SetShift(-patchRange[0]);
  patchNormalizeFilter->SetScale(255/(patchRange[1]-patchRange[0]));
  patchNormalizeFilter->SetInputData(patch);
  patchNormalizeFilter->Update();

  // Compute the correlation
  vtkSmartPointer<vtkImageCorrelation> correlationFilter =
    vtkSmartPointer<vtkImageCorrelation>::New();
  correlationFilter->SetInput1Data(imageNormalizeFilter->GetOutput());
  correlationFilter->SetInput2Data(patchNormalizeFilter->GetOutput());
  correlationFilter->Update();

  // Find the maxima of this correlation image
  int extent[6];
  float maximum = 0;
  image->GetExtent(extent);
  for(unsigned int i = extent[0]; i < extent[1]; i++)
    {
    for(unsigned int j = extent[2]; j < extent[3]; j++)
      {
      float* pixel = static_cast<float*>(correlationFilter->GetOutput()->GetScalarPointer(i,j,0));
      if(pixel[0] > maximum)
        {
        maximum = pixel[0];
        bestPatchCenter[0] = i;
        bestPatchCenter[1] = j;
        }
      }
    }
}