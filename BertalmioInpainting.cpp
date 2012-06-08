#include "BertalmioInpainting.h"

void BertalmioInpainting::Iterate(int iteration)
{
  vtkSmartPointer<vtkImageGradient> gradientFilter =
    vtkSmartPointer<vtkImageGradient>::New();
  gradientFilter->SetInputData(this->Image);
  gradientFilter->Update();

  // Normalize the gradient
  vtkSmartPointer<vtkImageData> normalizedGradient =
    vtkSmartPointer<vtkImageData>::New();
  normalizedGradient->ShallowCopy(gradientFilter->GetOutput());
  int extents[6];
  normalizedGradient->GetExtent(extents);
  for(vtkIdType i = extents[0]; i <= extents[1]; i++)
    {
    for(vtkIdType j = extents[2]; j <= extents[3]; j++)
      {
      double* pixel = static_cast<double*>(normalizedGradient->GetScalarPointer(i,j,0));
      vtkMath::Normalize2D(pixel);
      }
    }

  // Rotate gradient 90 degrees
  for(vtkIdType i = extents[0]; i <= extents[1]; i++)
    {
    for(vtkIdType j = extents[2]; j <= extents[3]; j++)
      {
      double* pixel = static_cast<double*>(normalizedGradient->GetScalarPointer(i,j,0));
      double temp[2] = {pixel[0], pixel[1]};
      pixel[0] = -temp[1];
      pixel[1] = temp[0];
      }
    }

  vtkSmartPointer<vtkImageLaplacian> laplacianFilter =
    vtkSmartPointer<vtkImageLaplacian>::New();
  laplacianFilter->SetInputData(this->Image);
  laplacianFilter->Update();

  vtkSmartPointer<vtkImageGradient> laplacianGradientFilter =
    vtkSmartPointer<vtkImageGradient>::New();
  laplacianGradientFilter->SetInputConnection(laplacianFilter->GetOutputPort());
  laplacianGradientFilter->Update();

  // This is the image which will be used in the hole filling. 'B' is the authors
  // terminology.
  vtkSmartPointer<vtkImageData> B =
    vtkSmartPointer<vtkImageData>::New();
  B->ShallowCopy(laplacianGradientFilter->GetOutput()); // We want the extent and the scalar type to be correct

  // Project Laplacian gradient onto normalized gradient
  for(vtkIdType i = extents[0]; i < extents[1]; i++)
    {
    for(vtkIdType j = extents[2]; j < extents[3]; j++)
      {
      double* laplacianGradientPixel = static_cast<double*>(laplacianGradientFilter->GetOutput()->GetScalarPointer(i,j,0));
      double* normalizedGradientPixel = static_cast<double*>(normalizedGradient->GetScalarPointer(i,j,0));
      double projection[2];
      vtkMath::ProjectVector2D(laplacianGradientPixel, normalizedGradientPixel, projection);

      double* pixel = static_cast<double*>(B->GetScalarPointer(i,j,0));
      pixel[0] = projection[0]; pixel[1] = projection[1]; pixel[2] = 0;
      }
    }

  vtkSmartPointer<vtkImageData> boundary =
    vtkSmartPointer<vtkImageData>::New();
  FindMaskBoundaryPixels(boundary);

  for(vtkIdType i = extents[0]; i < extents[1]; i++)
    {
    for(vtkIdType j = extents[2]; j < extents[3]; j++)
      {
      unsigned char* pixel = static_cast<unsigned char*>(this->Image->GetScalarPointer(i,j,0));
      if(pixel[0] != 255) // This is a boundary pixel, skip it.
        {
        continue;
        }
      }
    }
}
