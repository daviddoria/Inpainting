#ifndef __vtkInpaintingCriminisi_h
#define __vtkInpaintingCriminisi_h

#include "vtkImageAlgorithm.h"
#include "vtkSmartPointer.h"

class vtkImageData;

#include <vector>

struct Pixel
{
public:
  Pixel() : x(-1), y(-1), Priority(0) {}

  int x;
  int y;
  double Priority;
};

#endif
