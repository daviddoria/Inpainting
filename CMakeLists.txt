cmake_minimum_required(VERSION 2.6)

PROJECT(Inpainting)

# This is needed for c++0x features used in Helpers.
if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
endif(UNIX)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

add_subdirectory(Mask)

ADD_EXECUTABLE(FastDigitalInpainting FastDigitalInpaintingExample.cpp
Inpainting.cpp FastDigitalInpainting.cpp)
TARGET_LINK_LIBRARIES(FastDigitalInpainting ${ITK_LIBRARIES} Mask)

# ADD_EXECUTABLE(BertalmioInpainting BertalmioInpainting.cpp
# Inpainting.cpp BertalmioInpainting.cpp)
# TARGET_LINK_LIBRARIES(BertalmioInpainting ${ITK_LIBRARIES} Mask)
