project(BUILDINGBLOCK)



# We define several classes in this example, and we want to be able to use
# their C++ methods as callbacks for our user interface. To do so, we need to
# create a library and wrap it automatically for the Tcl language, which
# is used as a bridge between C++ objects at run-time. Note that an
# initialization function is automatically created in this library to allow
# classes and C++ methods to be used as commands and callbacks inside the Tcl
# interpreter; do *not* forget to call this function right after you
# initialize the Tcl interpreter in your application. The name of this
# function is built from the library name in lower-case (except for the first
# letter) and suffixed with "_Init" (for example:
# KWR21ExampleLib => KwR21examplelib_Init)
# This whole process is required to use C++ methods as callbacks; it is not
# needed if you use VTK's C++ command/observer pattern directly, which is
# demonstrated in KWWidgets's C++ 'Callbacks' example.

# directories for vtkMIMX
#include_directories( ../Common ../Common )
#include_directories( ${IAFEMMESH_SOURCE_DIR}/Code/Filter ${IAFEMMESH_BINARY_DIR}/Code/Filter )
#include_directories( ${IAFEMMESH_SOURCE_DIR}/Code/Widgets ${IAFEMMESH_BINARY_DIR}/Code/Widgets )
#include_directories( ${IAFEMMESH_SOURCE_DIR}/Code/Adapter ${IAFEMMESH_BINARY_DIR}/Code/Adapter )
#include_directories( ${IAFEMMESH_SOURCE_DIR}/Code/IO ${IAFEMMESH_BINARY_DIR}/Code/IO )
#include_directories( ${IAFEMMESH_SOURCE_DIR}/Code/Mesh    ${IAFEMMESH_BINARY_DIR}/Code/Mesh )
include_directories( ${VTK_SOURCE_DIR}/Utilities/verdict/include ${VTK_KITS_DIR}/verdict )

# Include dirs
#
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${SlicerBase_SOURCE_DIR}
  ${SlicerBase_BINARY_DIR}
  ${SlicerBaseLogic_SOURCE_DIR}
  ${SlicerBaseLogic_BINARY_DIR}
  ${SlicerBaseGUI_SOURCE_DIR}
  ${SlicerBaseGUI_BINARY_DIR}
  ${vtkITK_INCLUDE_DIRS}
  ${ITK_INCLUDE_DIR}
  ${VTK_INCLUDE_DIR}
  ${MIMXCommon_SOURCE_DIR} ${MIMXCommon_BINARY_DIR}
  ${mimxCommon_SOURCE_DIR} ${mimxCommon_BINARY_DIR}
  ${MIMXAdapter_SOURCE_DIR} ${MIMXAdapter_BINARY_DIR}
  ${MIMXWidgets_SOURCE_DIR} ${MIMXWidgets_BINARY_DIR}
  ${MIMXFilter_SOURCE_DIR} ${MIMXFilter_BINARY_DIR}
  ${MIMXMeshIO_SOURCE_DIR} ${MIMXMeshIO_BINARY_DIR}
  ${MIMXMesh_SOURCE_DIR} ${MIMXMesh_BINARY_DIR}
  ${MRML_INCLUDE_DIRS}
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkBuildingBlockConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkBuildingBlockConfigure.h
  )

set(LIB_NAME "BuildingBlock")
set(LIB_SRCS
  vtkKWMimxApplication.cxx
  vtkKWMimxApplicationSettingsInterface.cxx
  vtkKWMimxFEMeshMenuGroup.cxx
  # requires VTK 5.1+, so temporarily disabled
  vtkKWMimxImageMenuGroup.cxx
  vtkKWMimxGroupBase.cxx
  vtkKWMimxMainNotebook.cxx
  vtkKWMimxMainMenuGroup.cxx
  #vtkKWMimxMenuWindow.cxx
  vtkKWMimxSurfaceMenuGroup.cxx
  vtkKWMimxBBMenuGroup.cxx
  #vtkKWRenderWidget.cxx
  # needs new mesh quality metrics (VTK 5.1+)
  vtkKWMimxEvaluateMeshQualityGroup.cxx
  vtkKWMimxQualityMenuGroup.cxx
  vtkKWMimxCreateBBFromBoundsGroup.cxx
  vtkKWMimxCreateBBMeshSeedGroup.cxx
  vtkKWMimxEditBBGroup.cxx
  vtkKWMimxCreateFEMeshFromBBGroup.cxx
  vtkKWMimxViewProperties.cxx
  vtkKWMimxSaveSTLSurfaceGroup.cxx
  vtkKWMimxSaveVTKSurfaceGroup.cxx
  vtkKWMimxSaveVTKBBGroup.cxx
  vtkKWMimxEditBBMeshSeedGroup.cxx
  vtkKWMimxSaveVTKFEMeshGroup.cxx
  vtkKWMimxDeleteObjectGroup.cxx
  vtkKWMimxEditFEMeshLaplacianSmoothGroup.cxx
  # VTK5.1+
  vtkKWMimxCreateExtrudeFEMeshGroup.cxx
  vtkKWMimxCreateSurfaceFromContourGroup.cxx
  vtkKWMimxEditNodeElementNumbersGroup.cxx
  vtkKWMimxMergeMeshGroup.cxx
  vtkKWMimxExportAbaqusFEMeshGroup.cxx
  vtkKWMimxMergeBBGroup.cxx
  vtkKWMimxImageViewProperties.cxx
  vtkKWMimxApplyFEMeshMaterialPropertiesFromImageGroup.cxx
  vtkKWMimxCreateFEMeshFromSurfaceExtractionGroup.cxx
  vtkKWMimxCreateElementSetPickFaceGroup.cxx
  vtkKWMimxMirrorBBGroup.cxx
  vtkKWMimxMainWindow.cxx
  #vtkKWMimxViewWindow.cxx
  # VTK5.1
  vtkKWMimxCreateFEMeshNodeSetGroup.cxx
  vtkKWMimxCreateFEMeshElementSetGroup.cxx
  vtkKWMimxMaterialPropertyMenuGroup.cxx
  vtkKWMimxBoundaryConditionsMenuGroup.cxx
  vtkKWMimxDeleteBoundaryConditionsGroup.cxx
  vtkKWMimxAssignBoundaryConditionsGroup.cxx
  vtkKWMimxNodeElementNumbersGroup.cxx
  vtkKWMimxEditElementSetNumbersGroup.cxx
  vtkKWMimxConstMatPropElSetGroup.cxx
  # found on Mac
  vtkKWMimxDefineNodeSetGroup.cxx
  vtkKWMimxDefineElSetGroup.cxx
  #
  vtkKWMimxReBinFEMeshMaterialPropertiesFromImageGroup.cxx
  vtkKWMimxEditNodeSetNumbersGroup.cxx
  #
  vtkKWMimxMainUserInterfacePanel.cxx
  vtkKWMimxViewPropertiesGroup.cxx
   vtkKWMimxViewPropertiesOptionGroup.cxx
  vtkKWMimxDisplayPropertiesGroup.cxx
# Not Needed - This is now defined in mimxFilter
#  vtkFaceHedgeHog.cxx
# Not Needed - These are already defined in Slicer
#  vtkSlicerTheme.cxx
#  vtkSlicerColor.cxx
#  vtkSlicerFont.cxx
#  vtkSlicerCheckRadioButtonIcons.cxx
#  vtkSlicerIcons.cxx
  #
  vtkKWMimxLoadBBGroup.cxx
  vtkKWMimxLoadSurfaceGroup.cxx
  vtkKWMimxLoadMeshGroup.cxx
  vtkKWMimxDisplayMatPropGroup.cxx
  vtkKWMimxImportModelSurfaceGroup.cxx
  vtkKWMimxImportImageGroup.cxx
  )

#set_source_files_properties(
#  vtkKWMimxViewWindow.cxx
#  WRAP_EXCLUDE
#)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
KWWidgets_WRAP_TCL(${LIB_NAME} LIB_TCL_SRCS "${LIB_SRCS}" "")

# Create the library. The library is built in static mode for convenience.
# Check the 'Callbacks' example for more information about building it in
# shared mode, i.e. without the STATIC keyword (Win32 compilers requires
# an additional header file to setup DLL export symbols correctly).

add_library(${LIB_NAME} ${LIB_TCL_SRCS} ${LIB_SRCS})


#target_link_libraries(${LIB_NAME} ${KWWidgets_LIBRARIES})

target_link_libraries(${LIB_NAME}
  SlicerBaseLogic
  SlicerBaseGUI
  MRML
  vtkCommonTCL
  vtkImagingTCL
  vtkFilteringTCL
  vtkIOTCL
  vtkWidgets
  ${KWWidgets_LIBRARIES}
  mimxWidgets
  mimxFilter
  mimxCommon
  mimxMeshIO
  ${ITK_LIBRARIES}
  )

if(CMAKE_SYSTEM MATCHES "SunOS.*")
  set_target_properties(${LIB_NAME} PROPERTIES LINK_FLAGS "-z muldefs")
endif(CMAKE_SYSTEM MATCHES "SunOS.*")

#configure_file(
#  ${BOUNDINGBOX_SOURCE_DIR}/vtkBoundingBlockConfigure.h.in
#  ${BOUNDINGBOX_BINARY_DIR}/vtkBoundingBlockConfigure.h
#)


# Register this example as a test. Our executable supports a --test
# configuration option so that it can be run non-interactively as a test.
# If you are using this CMakeLists.txt file to create your own application
# based on KWWidgets, you should omit this section, unless your application
# supports that feature too and you checked how the macro is working.

#if(BUILD_TESTING)
#  include("${KWWidgets_CMAKE_DIR}/KWWidgetsTestingMacros.cmake")
#  KWWidgets_ADD_TEST_FROM_C_EXAMPLE(KWWidgets-${PROJECT_NAME} ${EXE_NAME})
#endif()

# Apply user-defined properties to the library target.
if(Slicer_LIBRARY_PROPERTIES)
  set_target_properties(${LIB_NAME} PROPERTIES
     ${Slicer_LIBRARY_PROPERTIES}
  )
endif(Slicer_LIBRARY_PROPERTIES)

install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION ${Slicer_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${Slicer_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer_INSTALL_LIB_DIR} COMPONENT Development
  )

