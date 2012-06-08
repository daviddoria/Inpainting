#ifndef __vtkInpaintingCriminisi_h
#define __vtkInpaintingCriminisi_h

#include "vtkInpainting.h"

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

class vtkInpaintingCriminisi : public vtkInpainting
{
public:
  vtkTypeMacro(vtkInpaintingCriminisi,vtkInpainting);
  static vtkInpaintingCriminisi *New();

  // Description:
  // Given a nxn (n odd) patch, find the center of the best matching patch in the image
  // Public for testing purposes.
  void FindBestPatchMatch(vtkImageData* patch, vtkImageData* image, int bestPatchCenter[2]);

protected:
  vtkInpaintingCriminisi(){}
  ~vtkInpaintingCriminisi(){}

  // Description:
  // Perform an iteration of the fast digital inpainting algorithm.
  void Iterate(int iteration);

  // Description:
  // Find the pixels on the border of the target region and source region.
  void FindFrontPixels();

  // Description:
  // Compute the priority of every pixel on the front
  void ComputePriorities();

  // Description:
  // Compute the 'priority' of a pixel. This is the sum of the ConfidenceTerm
  // and DataTerm.
  double ComputePriority(int x, int y);

  // Description:
  // Perform an iteration of the fast digital inpainting algorithm.
  double ComputeConfidenceTerm(int x, int y);

  // Description:
  // Perform an iteration of the fast digital inpainting algorithm.
  double ComputeDataTerm(int x, int y);

  // Description:
  // Maintain a list of the current front/border pixels
  std::vector<Pixel> Border;

private:
  vtkInpaintingCriminisi(const vtkInpaintingCriminisi&);  // Not implemented.
  void operator=(const vtkInpaintingCriminisi&);  // Not implemented.

};

#endif
