cmake_minimum_required(VERSION 2.6)

PROJECT(Inpainting)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

include_directories(/media/portable/src/github/vtkBinaryBlobPerimeter/)

ADD_EXECUTABLE(FastDigitalInpainting vtkFastDigitalInpaintingExample.cxx vtkInpainting.cxx vtkBertalmioInpainting.cxx vtkFastDigitalInpainting.cxx)
TARGET_LINK_LIBRARIES(FastDigitalInpainting vtkHybrid)

ADD_EXECUTABLE(vtkInpaintingCriminisiExample vtkInpaintingCriminisiExample.cxx vtkInpainting.cxx vtkInpaintingCriminisi.cxx
vtkImageVectorRotate.cxx
vtkImageIsophotes.cxx
/media/portable/src/github/vtkBinaryBlobPerimeter/vtkBinaryBlobPerimeter2D.cxx)
TARGET_LINK_LIBRARIES(vtkInpaintingCriminisiExample vtkHybrid)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(CriminisiInpainting CriminisiInpaintingExample.cpp Helpers.cpp)
TARGET_LINK_LIBRARIES(CriminisiInpainting vtkHybrid ITKBasicFilters ITKCommon ITKIO)
#
# ADD_EXECUTABLE(TestSelfPatchMatch TestSelfPatchMatch.cpp Helpers.cpp)
# TARGET_LINK_LIBRARIES(TestSelfPatchMatch vtkHybrid ITKBasicFilters ITKCommon ITKIO)
#
# ADD_EXECUTABLE(TestPatchDifference TestPatchDifference.cpp Helpers.cpp)
# TARGET_LINK_LIBRARIES(TestPatchDifference vtkHybrid ITKBasicFilters ITKCommon ITKIO)

# ADD_EXECUTABLE(DemoErroneousGradient DemoErroneousGradient.cpp Helpers.cpp )
# TARGET_LINK_LIBRARIES(DemoErroneousGradient ITKBasicFilters ITKCommon ITKIO)

# ADD_EXECUTABLE(DemoBoundaryNormals DemoBoundaryNormals.cpp Helpers.cpp )
# TARGET_LINK_LIBRARIES(DemoBoundaryNormals ITKBasicFilters ITKCommon ITKIO)

ADD_EXECUTABLE(DemoInnerOuterBoundary DemoInnerOuterBoundary.cpp Helpers.cpp )
TARGET_LINK_LIBRARIES(DemoInnerOuterBoundary ITKBasicFilters ITKCommon ITKIO)

# ADD_EXECUTABLE(PatchImageDifferenceExample PatchImageDifferenceExample.cpp SelfPatchMatch.h Helpers.h  Helpers.cpp)
# TARGET_LINK_LIBRARIES(PatchImageDifferenceExample vtkHybrid ITKBasicFilters ITKCommon ITKIO)
#
# ADD_EXECUTABLE(SelfPatchMatchExample SelfPatchMatchExample.cpp SelfPatchMatch.h Helpers.h  Helpers.cpp)
# TARGET_LINK_LIBRARIES(SelfPatchMatchExample vtkHybrid ITKBasicFilters ITKCommon ITKIO)

ADD_EXECUTABLE(vtkImageIsophotesExample vtkImageIsophotesExample.cxx vtkImageIsophotes.cxx vtkImageVectorRotate.cxx)
TARGET_LINK_LIBRARIES(vtkImageIsophotesExample vtkHybrid)

ADD_EXECUTABLE(PatchMatchExample PatchMatchExample.cxx vtkInpainting.cxx vtkInpaintingCriminisi.cxx
vtkImageIsophotes.cxx
vtkImageVectorRotate.cxx
/media/portable/src/github/vtkBinaryBlobPerimeter/vtkBinaryBlobPerimeter2D.cxx)
TARGET_LINK_LIBRARIES(PatchMatchExample vtkHybrid)
