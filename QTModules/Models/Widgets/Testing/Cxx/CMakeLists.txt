
set(KIT qSlicerModelsWidgets)

set(CMAKE_TESTDRIVER_AFTER_TESTMAIN  "DEBUG_LEAKS_ENABLE_EXIT_ERROR();")
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  qMRMLModelDisplayNodeWidgetTest1.cxx
  qMRMLModelDisplayNodeWidgetTest2.cxx

  EXTRA_INCLUDE TestingMacros.h
  )

set(TestsToRun ${Tests})
list(REMOVE_ITEM TestsToRun ${KIT}CxxTests.cxx)

set(LIBRARY_NAME ${lib_name})

add_executable(${KIT}CxxTests ${Tests})
target_link_libraries(${KIT}CxxTests ${LIBRARY_NAME})

#
# Convenient macros
#

macro(SIMPLE_TEST TESTNAME)
  add_test(NAME ${TESTNAME} COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}CxxTests> ${TESTNAME} ${ARGN})
endmacro()

set(Models_TESTING_DATA_SOURCE_DIR ${MRML_SOURCE_DIR}/Testing)
macro(SCENE_TEST TESTNAME SCENEFILENAME)
  add_test(
    NAME ${TESTNAME}_${SCENEFILENAME}
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}CxxTests> ${TESTNAME}
      ${Models_TESTING_DATA_SOURCE_DIR}/${SCENEFILENAME}
    )
endmacro()

#
# Add Tests
#

SIMPLE_TEST( qMRMLModelDisplayNodeWidgetTest1 )
SCENE_TEST( qMRMLModelDisplayNodeWidgetTest2 cube.mrml )
