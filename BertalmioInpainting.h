// Based on "Image Inpainting" by Bertalmio, Sapiro, Caselles, and Ballester
#ifndef BertalmioInpainting_h
#define BertalmioInpainting_h

class BertalmioInpainting
{
public:
  vtkBertalmioInpainting(){}

  void Iterate(int iteration);

};

#endif
