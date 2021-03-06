project(VolumeRenderingReplacements)

#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#-----------------------------------------------------------------------------
if(POLICY CMP0016)
  cmake_policy(SET CMP0016 NEW)
endif()
if(POLICY CMP0017)
  cmake_policy(SET CMP0017 OLD)
endif()

# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer_Libs_INCLUDE_DIRS}
  ${Slicer_Base_INCLUDE_DIRS}
  )

configure_file(
  ${VolumeRenderingReplacements_SOURCE_DIR}/vtkVolumeRenderingReplacementsConfigure.h.in
  ${VolumeRenderingReplacements_BINARY_DIR}/vtkVolumeRenderingReplacementsConfigure.h
  )

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES
  ${headers}
  "${CMAKE_CURRENT_BINARY_DIR}/vtkVolumeRenderingReplacementsConfigure.h"
  DESTINATION ${Slicer_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(VolumeRenderingReplacements_SRCS
  vtkSlicerVolumeTextureMapper3D.cxx
  vtkSlicerOpenGLVolumeTextureMapper3D.cxx
  vtkSlicerVolumeRenderingFactory.cxx
  #GPU Ray Cast stuff
  vtkSlicerGPURayCastVolumeMapper.cxx
  vtkSlicerGPUVolumeMapper.cxx
  vtkSlicerGPURayCastMultiVolumeMapper.cxx
  vtkSlicerGPUMultiVolumeMapper.cxx
  #Ray Cast stuff
  vtkSlicerFixedPointRayCastImage.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeGOHelper.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeGOShadeHelper.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeHelper.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper.cxx
  vtkSlicerFixedPointVolumeRayCastHelper.cxx
  vtkSlicerFixedPointVolumeRayCastMapper.cxx
  vtkSlicerFixedPointVolumeRayCastMIPHelper.cxx
  vtkSlicerRayCastImageDisplayHelper.cxx
  vtkSlicerOpenGLRayCastImageDisplayHelper.cxx
  #GUI
  #vtkSlicerBoxWidget.cxx
  vtkSlicerVolumePropertyWidget.cxx
  #vtkSlicerBoxWidget2.cxx
  #vtkSlicerBoxRepresentation.cxx
  )

# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(VolumeRenderingReplacements
  VolumeRenderingReplacements_TCL_SRCS
  "${VolumeRenderingReplacements_SRCS}" "")

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name VolumeRenderingReplacements)
add_library(${lib_name}
  ${VolumeRenderingReplacements_SRCS}
  ${VolumeRenderingReplacements_TCL_SRCS}
  )
slicer3_set_modules_output_path(${lib_name})

# Apply user-defined properties to the library target.
if(Slicer_LIBRARY_PROPERTIES)
  set_target_properties(${lib_name} PROPERTIES
     ${Slicer_LIBRARY_PROPERTIES}
  )
endif(Slicer_LIBRARY_PROPERTIES)

target_link_libraries(${lib_name}
  ${Slicer_Libs_LIBRARIES}
  ${Slicer_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  )

slicer3_install_modules(${lib_name})
