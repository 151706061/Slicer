
#-----------------------------------------------------------------------------
# DiffusionTensorEstimation tests
#-----------------------------------------------------------------------------
set(CLP DiffusionTensorEstimation)
include_directories(
  ${vtkITK_INCLUDE_DIRS}
  ${MRML_INCLUDE_DIRS}
  ${vtkTeem_INCLUDE_DIRS}
  ${SlicerBaseLogic_SOURCE_DIR} ${SlicerBaseLogic_BINARY_DIR})
add_executable(${CLP}Test ${CLP}Test.cxx)
add_dependencies(${CLP}Test ${CLP})
target_link_libraries (${CLP}Test MRML vtkTeem SlicerBaseLogic)
set_target_properties(${CLP}Test PROPERTIES LABELS ${CLP})

set(testname ${CLP}Test)
add_test(NAME ${testname} COMMAND ${Launcher_Command} $<TARGET_FILE:${CLP}Test>
  DiffusionTensorEstimationTest
    -m ${MRML_TEST_DATA}/helix-DWI-otsu-no_islands-05.nrrd
    ${MRML_TEST_DATA}/helix-DWI.nhdr
    ${TEMP}/DiffusionTensorEstimationTest_dti.nhdr
    ${TEMP}/DiffusionTensorEstimationTest_baseline.nhdr
      )
set_property(TEST ${testname} PROPERTY LABELS ${CLP})

