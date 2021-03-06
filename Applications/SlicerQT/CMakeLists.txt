project(SlicerQT)

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------

set(SlicerQT_SRCS
  qSlicerAboutDialog.cxx
  qSlicerAboutDialog.h

  qSlicerMainWindow.cxx
  qSlicerMainWindow.h
  qSlicerMainWindowCore.cxx
  qSlicerMainWindowCore.h
  qSlicerMainWindowCore_p.h
  )

set(SlicerQT_MOC_SRCS
  qSlicerAboutDialog.h

  qSlicerMainWindow.h
  qSlicerMainWindowCore.h
  qSlicerMainWindowCore_p.h
  )

set(SlicerQT_UI_SRCS
  Resources/UI/qSlicerAboutDialog.ui
  Resources/UI/qSlicerMainWindow.ui
  )

# Resources
set(qt_module_resources
  Resources/SlicerQT.qrc
  ${qSlicerBaseQTGUI_SOURCE_DIR}/Resources/qSlicerBaseQTGUI.qrc
  ${qSlicerBaseQTCoreModules_SOURCE_DIR}/Resources/qSlicerBaseQTCoreModules.qrc
)
#set(SlicerQT_RC_FILE Resources/SlicerQT.rc)


QT4_WRAP_CPP(SlicerQT_SRCS ${SlicerQT_MOC_SRCS})
QT4_WRAP_UI(SlicerQT_UI_CXX ${SlicerQT_UI_SRCS})
QT4_ADD_RESOURCES(SlicerQT_QRC_SRCS ${qt_module_resources})

# --------------------------------------------------------------------------
# Include dirs
# --------------------------------------------------------------------------

set(include_dirs
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Slicer_Base_INCLUDE_DIRS}
  ${MRML_INCLUDE_DIRS}
  ${MRMLLogic_INCLUDE_DIRS}
  ${qMRMLWidgets_INCLUDE_DIRS}
  )

include_directories(${include_dirs})


# --------------------------------------------------------------------------
# Build the library
# --------------------------------------------------------------------------

set(lib_name "qSlicerQT")

add_library(${lib_name}
  ${SlicerQT_SRCS}
  ${SlicerQT_UI_CXX}
  ${SlicerQT_QRC_SRCS}
  )
set_target_properties(${lib_name} PROPERTIES LABELS SlicerQT)

set(SlicerQT_LIBRARIES
  qSlicerBaseQTCLI
  qSlicerBaseQTCoreModules
  )

if(Slicer_USE_PYTHONQT)
  set(SlicerQT_LIBRARIES
    ${SlicerQT_LIBRARIES}
    qSlicerBaseQTCorePythonQt
    qSlicerBaseQTGUIPythonQt
    )
endif()

target_link_libraries(${lib_name}
  ${SlicerQT_LIBRARIES}
  )


# --------------------------------------------------------------------------
# Configure Application Bundle Resources (Mac Only)
# --------------------------------------------------------------------------

if(Q_WS_MAC)
  set(apple_icon_file Slicer.icns)
  set(apple_bundle_sources "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${apple_icon_file}")
  set_source_files_properties(
    "${apple_bundle_sources}"
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
    )
  set(MACOSX_BUNDLE_ICON_FILE ${apple_icon_file})
endif(Q_WS_MAC)

if(QT_MAC_USE_COCOA)
  get_filename_component(qt_menu_nib
    "@QT_QTGUI_LIBRARY_RELEASE@/Resources/qt_menu.nib"
    REALPATH)

  set(qt_menu_nib_sources
    "${qt_menu_nib}/classes.nib"
    "${qt_menu_nib}/info.nib"
    "${qt_menu_nib}/keyedobjects.nib"
    )
  set_source_files_properties(
    ${qt_menu_nib_sources}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib
    )
  else(QT_MAC_USE_COCOA)
    set(qt_menu_nib_sources)
endif(QT_MAC_USE_COCOA)

# --------------------------------------------------------------------------
# Apply user-defined properties to the library target.
# --------------------------------------------------------------------------

if(Slicer_LIBRARY_PROPERTIES)
  set_target_properties(${lib_name} PROPERTIES ${Slicer_LIBRARY_PROPERTIES})
endif(Slicer_LIBRARY_PROPERTIES)

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${Slicer_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${Slicer_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer_INSTALL_LIB_DIR} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Build the executable
# --------------------------------------------------------------------------

if(NOT APPLE)
  set(SlicerQT_EXE_SUFFIX -real)
endif()

add_executable(SlicerQT${SlicerQT_EXE_SUFFIX} MACOSX_BUNDLE
  Main.cxx
  ${apple_bundle_sources}
  ${qt_menu_nib_sources}
  )
set_target_properties(SlicerQT${SlicerQT_EXE_SUFFIX} PROPERTIES LABELS SlicerQT)

if(APPLE)
  set_target_properties(SlicerQT${SlicerQT_EXE_SUFFIX} PROPERTIES OUTPUT_NAME Slicer)
endif()

target_link_libraries(SlicerQT${SlicerQT_EXE_SUFFIX}
  ${lib_name}
  )

#-----------------------------------------------------------------------------
# Configure
# --------------------------------------------------------------------------

set(MY_LIBRARY_EXPORT_DIRECTIVE "Q_SLICERQT_EXPORT")
set(MY_EXPORT_HEADER_PREFIX ${lib_name})
set(MY_LIBNAME ${lib_name})

configure_file(
  ${Slicer_SOURCE_DIR}/qSlicerExport.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
  )
set(dynamicHeaders
  "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

# --------------------------------------------------------------------------
# Install
# --------------------------------------------------------------------------

if(NOT APPLE)
  set(SlicerQT_INSTALL_DESTINATION_ARGS RUNTIME DESTINATION ${Slicer_INSTALL_BIN_DIR})
else()
  set(SlicerQT_INSTALL_DESTINATION_ARGS BUNDLE DESTINATION ".")
endif()

install(TARGETS SlicerQT${SlicerQT_EXE_SUFFIX}
  ${SlicerQT_INSTALL_DESTINATION_ARGS}
  COMPONENT Runtime)

# --------------------------------------------------------------------------
# Configure Slicer Launcher
# --------------------------------------------------------------------------
if(Slicer_USE_CTKAPPLAUNCHER)
  include(${CTKAPPLAUNCHER_DIR}/CMake/ctkAppLauncher.cmake)

  # Define list of extra 'application to launch' to associate with the launched
  set(extraApplicationToLaunchListForBuildTree)
  if(EXISTS "${QT_DESIGNER_EXECUTABLE}")
    ctkAppLauncherAppendExtraAppToLaunchToList(
      LONG_ARG designer
      HELP "Start Qt designer using Slicer plugins"
      PATH ${QT_DESIGNER_EXECUTABLE}
      OUTPUTVAR extraApplicationToLaunchListForBuildTree
      )
  endif()
  set(executables)
  if(UNIX)
    list(APPEND executables gnome-terminal xterm ddd gdb)
  elseif(WIN32)
    list(APPEND executables VisualStudio)
    set(VisualStudio_EXECUTABLE ${CMAKE_BUILD_TOOL})
  endif()
  foreach(executable ${executables})
    find_program(${executable}_EXECUTABLE ${executable})
    if(${executable}_EXECUTABLE)
      message(STATUS "Enabling Slicer launcher option: --${executable}")
      ctkAppLauncherAppendExtraAppToLaunchToList(
        LONG_ARG ${executable}
        HELP "Start ${executable}"
        PATH ${${executable}_EXECUTABLE}
        OUTPUTVAR extraApplicationToLaunchListForBuildTree
        )
    endif()
  endforeach()

  include(SlicerBlockCTKAppLauncherSettings)
  ctkAppLauncherConfigure(
    TARGET SlicerQT${SlicerQT_EXE_SUFFIX}
    APPLICATION_INSTALL_SUBDIR ${Slicer_INSTALL_BIN_DIR}
    APPLICATION_NAME Slicer
    SPLASH_IMAGE_PATH ${Slicer_SOURCE_DIR}/Applications/SlicerQT/Resources/Images/SlicerSplashScreen.png
    SPLASH_IMAGE_INSTALL_SUBDIR ${Slicer_INSTALL_BIN_DIR}
    SPLASHSCREEN_HIDE_DELAY_MS 3000
    HELP_SHORT_ARG "-h"
    HELP_LONG_ARG "--help"
    NOSPLASH_ARGS "--no-splash,--help,--version,--home,--program-path,--no-main-window"
    EXTRA_APPLICATION_TO_LAUNCH_BUILD ${extraApplicationToLaunchListForBuildTree}
    DESTINATION_DIR ${Slicer_BINARY_DIR}
    LIBRARY_PATHS_BUILD "${SLICER_LIBRARY_PATHS_BUILD}"
    PATHS_BUILD "${SLICER_PATHS_BUILD}"
    ENVVARS_BUILD "${SLICER_ENVVARS_BUILD}"
    LIBRARY_PATHS_INSTALLED "${SLICER_LIBRARY_PATHS_INSTALLED}"
    PATHS_INSTALLED "${SLICER_PATHS_INSTALLED}"
    ENVVARS_INSTALLED "${SLICER_ENVVARS_INSTALLED}"
    )
  if(NOT APPLE)
    install(PROGRAMS "${Slicer_BINARY_DIR}/Slicer${CMAKE_EXECUTABLE_SUFFIX}" DESTINATION ".")

    install(
      FILES "${Slicer_SOURCE_DIR}/Applications/SlicerQT/Resources/Images/SlicerSplashScreen.png"
      DESTINATION ${Slicer_INSTALL_BIN_DIR}
      )
    install(
      FILES "${Slicer_BINARY_DIR}/SlicerLauncherSettingsToInstall.ini"
      DESTINATION ${Slicer_INSTALL_BIN_DIR}
      RENAME SlicerLauncherSettings.ini
      )
  endif()
endif()

# --------------------------------------------------------------------------
# Command line parsing tests
#
# add_test(SlicerCLTest1 ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --help)
# add_test(SlicerCLTest2 ${Slicer_LAUNCHER_EXECUTABLE} --xml --test-mode)
# add_test(SlicerCLTest3 ${Slicer_LAUNCHER_EXECUTABLE} --test-mode -f ${Slicer_SOURCE_DIR}/Applications/GUI/Testing/TestScript.tcl)
# add_test(SlicerCLTest4 ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --eval "puts testing ,. exit 0")
# add_test(SlicerCLTest5 ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --exec "puts testing ,. exit 0")
# add_test(SlicerCLTest6 ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --script ${Slicer_SOURCE_DIR}/Applications/GUI/Testing/TestScript.tcl)
# add_test(SlicerCLTest7 ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --script ${Slicer_SOURCE_DIR}/Applications/GUI/Testing/LoadSceneTest.tcl)
# add_test(SlicerScrollTest ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --script ${Slicer_SOURCE_DIR}/Applications/GUI/Testing/ScrollTesting.tcl)

# --------------------------------------------------------------------------
# MRML Tests
#
# add_test(SlicerMRMLUndo ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --no-splash -f ${Slicer_HOME}/share/MRML/Testing/testUndo.tcl)
# add_test(SlicerMRMLVolume ${Slicer_LAUNCHER_EXECUTABLE} --test-mode --no-splash -f ${Slicer_HOME}/share/MRML/Testing/testVolume.tcl)

# --------------------------------------------------------------------------
# Testing
#
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
