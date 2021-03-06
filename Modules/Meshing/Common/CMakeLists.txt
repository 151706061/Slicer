project(MIMXCommon)


# Include dirs
#
#include_directories(
#  ${Slicer_INCLUDE_DIRS}
#  ${SlicerBase_SOURCE_DIR}
#  ${SlicerBase_BINARY_DIR}
#  ${SlicerBaseLogic_SOURCE_DIR}
#  ${SlicerBaseLogic_BINARY_DIR}
#  ${SlicerBaseGUI_SOURCE_DIR}
#  ${SlicerBaseGUI_BINARY_DIR}
#  ${vtkITK_INCLUDE_DIRS}
#  ${ITK_INCLUDE_DIR}
#  ${VTK_INCLUDE_DIR}
#  ${mimxCommon_SOURCE_DIR} ${mimxCommon_BINARY_DIR}
#  ${MIMXCommon_SOURCE_DIR}../Adapter ${MIMXCommon_BINARY_DIR}../Adapter
#  ${MIMXAdapter_SOURCE_DIR} ${MIMXAdapter_BINARY_DIR}
#)

# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer_Libs_INCLUDE_DIRS}
  ${Slicer_Base_INCLUDE_DIRS}
  ${MIMXAdapter_SOURCE_DIR} ${MIMXAdapter_BINARY_DIR}
  ${MIMXFilter_SOURCE_DIR} ${MIMXFilter_BINARY_DIR}
  ${mimxCommon_SOURCE_DIR} ${mimxCommon_BINARY_DIR}
  ${ITK_INCLUDE_DIR}
  )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMimxCommonConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMimxCommonConfigure.h
  )

set(LIB_NAME "mimxCommon")


set( LIB_SRCS
    #mimxRegisterVersorRigidFilter.cxx
    #mimxRegisterBSplineFilter.cxx
    #mimxTransformIO.cxx
    mimxCheckTypeOfFile.cxx
    # container types
    vtkContainer.cxx
    vtkLocalLinkedListWrapper.cxx
    vtkLinkedListWrapper.cxx
    vtkLinkedListWrapperTree.cxx
    # added images to be handled like surfaces, & meshes
    vtkMRMLFiniteElementImageNode.cxx
    vtkFiniteElementImageList.cxx
    vtkMRMLFESurfaceNode.cxx
    vtkFESurfaceList.cxx
    # removed because it can't wrap
    #vtkMimxEntryValueChangedCallback.cxx
    vtkMRMLFiniteElementBuildingBlockNode.cxx
    vtkMRMLFiniteElementBuildingBlockDisplayNode.cxx
    vtkMRMLFiniteElementMeshDisplayNode.cxx
    vtkMRMLNullDisplayNode.cxx
    vtkFiniteElementBuildingBlockList.cxx
    vtkMRMLFiniteElementMeshNode.cxx
    vtkFiniteElementMeshList.cxx
    # end container types
    vtkMimxCurve.cxx
    vtkMimxSTLSource.cxx
    vtkMimxUGrid.cxx
    vtkMimxBoundingBoxSource.cxx
    vtkLocalAxesActor.cxx
    vtkPVAxesActor.cxx
    vtkMimxSolidStructuredGridSource.cxx
    vtkMimxPlanarStructuredGridSource.cxx
    #vtkMeshQualityClass.cxx
    vtkMimxApplyNodeElementNumbers.cxx
    vtkMimxCreateIntervertebralDiscElementSets.cxx
    vtkMimxEntryValueChangedCallback.cxx
    vtkMimxMergeNodesPointAndCellData.cxx
    vtkMimxMeshSeed.cxx
    vtkMimxActorBase.cxx
    vtkMimxMeshActor.cxx
    vtkMimxSurfacePolyDataActor.cxx
    vtkMimxUnstructuredGridActor.cxx
    vtkMimxErrorCallback.cxx
    vtkMimxImageActor.cxx
#    vtkMimxInteractorStyleUser.cxx
    vtkMimxColorCodeMeshSeedActor.cxx
    #
    vtkMimxBoundaryConditionActor.cxx
    vtkMimxTestErrorCallback.cxx
)

set_source_files_properties(
  mimxCheckTypeOfFile.cxx
  vtkMimxActorBase.cxx
  vtkMimxMeshActor.cxx
  vtkMimxSTLSource.cxx
  vtkMimxUGrid.cxx
  vtkMimxBoundingBoxSource.cxx
  vtkMimxEntryValueChangedCallback.cxx
  vtkMimxErrorCallback.cxx
  vtkMimxImageActor.cxx
  vtkMimxMeshSeed.cxx
  vtkMimxMergeNodesPointAndCellData.cxx
  vtkMimxCreateIntervertebralDiscElementSets.cxx
  vtkMimxApplyNodeElementNumbers.cxx
  vtkContainer.cxx
  vtkLocalLinkedListWrapper.cxx
  vtkLinkedListWrapper.cxx
  vtkLinkedListWrapperTree.cxx
  vtkFESurfaceList.cxx
  vtkMRMLFiniteElementBuildingBlockNode.cxx
  vtkMRMLFiniteElementBuildingBlockDisplayNode.cxx
  vtkFiniteElementBuildingBlockList.cxx
  vtkMimxSurfacePolyDataActor.cxx
  vtkMimxUnstructuredGridActor.cxx
  vtkMimxCurve.cxx
  vtkMimxBoundaryConditionActor.cxx
  vtkMimxTestErrorCallback.cxx
  WRAP_EXCLUDE
  )

set_source_files_properties(
  vtkMimxActorBase.cxx
  ABSTRACT
  )


#
# Wrapping
#

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
#KWWidgets_WRAP_TCL(${LIB_NAME} LIB_TCL_SRCS "${LIB_SRCS}" "")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
VTK_WRAP_TCL3(${LIB_NAME} LIB_TCL_SRCS "${LIB_SRCS}" "")


add_library( ${LIB_NAME} ${LIB_TCL_SRCS} ${LIB_SRCS} )
#add_library( ${LIB_NAME}  ${LIB_SRCS} )

target_link_libraries(${LIB_NAME}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  #mimxAdapter
  MRML
  )

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





