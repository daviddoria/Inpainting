// Based on "Image Inpainting" by Bertalmio, Sapiro, Caselles, and Ballester
#ifndef FastDigitalInpainting_h
#define FastDigitalInpainting_h

class FastDigitalInpainting
{
public:
  vtkFastDigitalInpainting(){}

  // Description:
  // Perform an iteration of the fast digital inpainting algorithm.
  void Iterate(int iteration);

};

#endif
