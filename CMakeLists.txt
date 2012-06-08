cmake_minimum_required(VERSION 2.6)

PROJECT(Inpainting)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(FastDigitalInpainting FastDigitalInpaintingExample.cpp
Inpainting.cpp BertalmioInpainting.cpp FastDigitalInpainting.cpp)
TARGET_LINK_LIBRARIES(FastDigitalInpainting ${ITK_LIBRARIES})

ADD_EXECUTABLE(BertalmioInpainting BertalmioInpainting.cpp
Inpainting.cpp BertalmioInpainting.cpp)
TARGET_LINK_LIBRARIES(BertalmioInpainting ${ITK_LIBRARIES})
