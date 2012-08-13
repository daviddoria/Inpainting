cmake_minimum_required(VERSION 2.6)

PROJECT(Inpainting)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Enable C++0x
if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
endif(UNIX)

if(NOT ITK_FOUND)
  FIND_PACKAGE(ITK REQUIRED ITKCommon ITKIOImageBase ITKIOPNG ITKIOMeta
                            ITKImageIntensity ITKImageFeature ITKMathematicalMorphology
                            ITKBinaryMathematicalMorphology ITKDistanceMap)
  INCLUDE(${ITK_USE_FILE})
endif()

if(NOT TARGET Mask)
  add_subdirectory(Mask)
  include_directories(${Mask_includes})
  set(Inpainting_includes ${Inpainting_includes} ${Mask_includes})
  set(Inpainting_libraries ${Inpainting_libraries} ${Mask_libraries})
endif()

if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set(Mask_includes ${Inpainting_includes} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
  #set(Mask_libraries ${Mask_libraries} Mask PARENT_SCOPE)
  message("Inpainting submodule used from ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

ADD_EXECUTABLE(FastDigitalInpainting FastDigitalInpaintingExample.cpp
Inpainting.cpp FastDigitalInpainting.cpp)
TARGET_LINK_LIBRARIES(FastDigitalInpainting ${ITK_LIBRARIES} ${Inpainting_libraries})

# ADD_EXECUTABLE(BertalmioInpainting BertalmioInpainting.cpp
# Inpainting.cpp BertalmioInpainting.cpp)
# TARGET_LINK_LIBRARIES(BertalmioInpainting ${ITK_LIBRARIES} Mask)

GET_DIRECTORY_PROPERTY(AllInpaintingIncludes INCLUDE_DIRECTORIES)
message("AllInpaintingIncludes include directories: ${AllInpaintingIncludes}")
