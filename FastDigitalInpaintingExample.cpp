#include "FastDigitalInpainting.h"

// STL
#include <string>

// Submodules
#include "Mask/Mask.h"
#include "Mask/ITKHelpers/ITKHelpers.h"

#include "itkImage.h"

int main(int argc, char *argv[])
{
  if(argc != 4)
    {
    std::cerr << "Required arguments: image mask output" << std::endl;
    return EXIT_FAILURE;
    }
  std::string imageFilename = argv[1];
  std::string maskFilename = argv[2];
  std::string outputFilename = argv[3];

  Mask::Pointer mask = Mask::New();
  mask->Read(maskFilename);

  typedef itk::Image<unsigned char, 2> ImageType;
  ImageType::Pointer image = ImageType::New();
  ITKHelpers::ReadImage(imageFilename, image.GetPointer());

  FastDigitalInpainting fastDigitalInpainting;
  fastDigitalInpainting.SetImage(image);
  fastDigitalInpainting.SetMask(mask);
  fastDigitalInpainting.Inpaint();

  return EXIT_SUCCESS;
}
