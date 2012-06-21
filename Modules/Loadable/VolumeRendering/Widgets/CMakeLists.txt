project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${vtkSlicerVolumeRenderingModuleMRML_SOURCE_DIR}
  ${vtkSlicerVolumeRenderingModuleMRML_BINARY_DIR}
  )

set(${KIT}_SRCS
  qSlicerCPURayCastVolumeRenderingPropertiesWidget.cxx
  qSlicerCPURayCastVolumeRenderingPropertiesWidget.h
  qSlicerGPURayCastVolumeRenderingPropertiesWidget.cxx
  qSlicerGPURayCastVolumeRenderingPropertiesWidget.h
  qSlicerNCIRayCastVolumeRenderingPropertiesWidget.cxx
  qSlicerNCIRayCastVolumeRenderingPropertiesWidget.h
  qSlicerNCIMultiVolumeRayCastVolumeRenderingPropertiesWidget.cxx
  qSlicerNCIMultiVolumeRayCastVolumeRenderingPropertiesWidget.h
  qSlicerPresetComboBox.cxx
  qSlicerPresetComboBox.h
  qSlicerPresetComboBox_p.h
  qSlicerVolumeRenderingPropertiesWidget.cxx
  qSlicerVolumeRenderingPropertiesWidget.h
  qMRMLVolumePropertyNodeWidget.cxx
  qMRMLVolumePropertyNodeWidget.h
  )

set(${KIT}_MOC_SRCS
  qSlicerCPURayCastVolumeRenderingPropertiesWidget.h
  qSlicerGPURayCastVolumeRenderingPropertiesWidget.h
  qSlicerNCIRayCastVolumeRenderingPropertiesWidget.h
  qSlicerNCIMultiVolumeRayCastVolumeRenderingPropertiesWidget.h
  qSlicerPresetComboBox.h
  qSlicerPresetComboBox_p.h
  qSlicerVolumeRenderingPropertiesWidget.h
  qMRMLVolumePropertyNodeWidget.h
  )

set(${KIT}_UI_SRCS
  ../Resources/UI/qMRMLVolumePropertyNodeWidget.ui
  ../Resources/UI/qSlicerCPURayCastVolumeRenderingPropertiesWidget.ui
  ../Resources/UI/qSlicerGPURayCastVolumeRenderingPropertiesWidget.ui
  ../Resources/UI/qSlicerNCIRayCastVolumeRenderingPropertiesWidget.ui
  ../Resources/UI/qSlicerNCIMultiVolumeRayCastVolumeRenderingPropertiesWidget.ui
  )

set(${KIT}_RESOURCES
  )

set(${KIT}_TARGET_LIBRARIES
  vtkSlicerVolumeRenderingModuleMRML
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleWidgets(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  )

#-----------------------------------------------------------------------------
add_subdirectory(DesignerPlugins)
