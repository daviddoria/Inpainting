#include "FastDigitalInpainting.h"

#include <string>

int main(int argc, char *argv[])
{
  if(argc != 4)
    {
    std::cerr << "Required arguments: image mask output" << std::endl;
    return EXIT_FAILURE;
    }
  std::string imageFilename = argv[1];
  std::string imageMaskFilename = argv[2];
  std::string outputFilename = argv[3];

  return EXIT_SUCCESS;
}