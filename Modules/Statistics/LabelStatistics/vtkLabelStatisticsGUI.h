/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkLabelStatisticsGUI.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkLabelStatisticsGUI_h
#define __vtkLabelStatisticsGUI_h

#include <string>
#include <iostream>
#include <sstream>

#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkLabelStatisticsLogic.h"


class vtkSlicerSliceWidget;
class vtkKWFrame;
class vtkKWScaleWithEntry;
class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWMultiColumnList;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWLoadSaveButton;

class VTK_LABELSTATISTICS_EXPORT vtkLabelStatisticsGUI : public vtkSlicerModuleGUI
{
  public:
  static vtkLabelStatisticsGUI *New();
  vtkTypeMacro(vtkLabelStatisticsGUI,vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // Get the categorization of the module.
  const char *GetCategory() const
  { return "Quantification"; }
  
   // Description: Get/Set MRML node
  vtkGetObjectMacro (Logic, vtkLabelStatisticsLogic);
  vtkSetObjectMacro (Logic, vtkLabelStatisticsLogic);

  // Description: implement setter method for logic class's parent
  virtual void SetModuleLogic(vtkSlicerLogic*);

  // Description: Get/Set MRML node
  vtkGetObjectMacro (LabelStatisticsNode, vtkMRMLLabelStatisticsNode);

  // Description:
  // Create widgets
  virtual void BuildGUI ( );
  //BTX
  using vtkSlicerComponentGUI::BuildGUI; 
  //ETX

  // Description:
  // Add obsereves to GUI widgets
  virtual void AddGUIObservers ( );
  
  // Description:
  // Remove obsereves to GUI widgets
  virtual void RemoveGUIObservers ( );
  
  // Description:
  // Process events generated by Logic
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event,
                                    void *callData );

  // Description:
  // Process events generated by GUI widgets
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event,
                                  void *callData );

  // Description:
  // Pprocess events generated by MRML
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, 
                                  void *callData);
  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter ( ){};
  //BTX
  using vtkSlicerComponentGUI::Enter; 
  //ETX
  virtual void Exit ( ){};

  // Description:
  // Set up primary selection tcl procedures 
  virtual void SetPrimarySelectionTclProcedures ( );

  //BTX
  // Description:
  // Sets text to be primary selection, for copy to clipboard functionality 
  virtual void SetPrimarySelection ( std::string text );
  //ETX
  
protected:
  vtkLabelStatisticsGUI();
  virtual ~vtkLabelStatisticsGUI();
  vtkLabelStatisticsGUI(const vtkLabelStatisticsGUI&);
  void operator=(const vtkLabelStatisticsGUI&);

  // Description:
  // Updates GUI widgets based on parameters values in MRML node
  void UpdateGUI();

  // Description:
  // Updates parameters values in MRML node based on GUI widgets 
  void UpdateMRML();
  
  vtkSlicerNodeSelectorWidget* GrayscaleSelector;
  vtkSlicerNodeSelectorWidget* LabelmapSelector; 
  vtkKWPushButton* ApplyButton;
 
  vtkLabelStatisticsLogic *Logic;
  vtkMRMLLabelStatisticsNode* LabelStatisticsNode;
  vtkKWMultiColumnList* ResultList;
  vtkKWMultiColumnListWithScrollbars* ResultListWithScrollbars;
  vtkKWLoadSaveButton* SaveToFile;
};

#endif

