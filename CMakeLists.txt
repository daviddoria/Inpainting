cmake_minimum_required(VERSION 2.6)

PROJECT(Inpainting)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(FastDigitalInpainting FastDigitalInpaintingExample.cxx
Inpainting.cxx BertalmioInpainting.cxx FastDigitalInpainting.cxx)
TARGET_LINK_LIBRARIES(FastDigitalInpainting ${ITK_LIBRARIES})

ADD_EXECUTABLE(BertalmioInpainting BertalmioInpainting.cxx
Inpainting.cxx BertalmioInpainting.cxx)
TARGET_LINK_LIBRARIES(BertalmioInpainting ${ITK_LIBRARIES})
