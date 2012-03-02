/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkColorDialog.h"
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkQtTestingUtility.h"

// qMRML includes
#include "qMRMLColorPickerWidget.h"

// MRML includes
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLFreeSurferProceduralColorNode.h>
#include <vtkMRMLPETProceduralColorNode.h>
#include <vtkMRMLScene.h>

// MRMLLogic includes
#include <vtkMRMLColorLogic.h>

// VTK includes
#include <vtkSmartPointer.h>

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  qMRMLColorPickerWidget* widget = reinterpret_cast<qMRMLColorPickerWidget*>(data);

  Q_UNUSED(widget);
  }
}

//-----------------------------------------------------------------------------
int qMRMLColorPickerWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = QString(argv[1]) + "/Libs/MRML/Widgets/Testing/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  ctkQtTestingUtility* testUtility = new ctkQtTestingUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  qMRMLColorPickerWidget* widget = new qMRMLColorPickerWidget();
  widget->setObjectName("ColorPickerWidget1");

  vtkSmartPointer<vtkMRMLScene> scene = vtkSmartPointer<vtkMRMLScene>::New();

  widget->setMRMLScene(scene);
  vtkSmartPointer<vtkMRMLColorLogic> colorLogic =
    vtkSmartPointer<vtkMRMLColorLogic>::New();
  colorLogic->SetMRMLScene(scene);

  etpWidget.addTestCase(widget,
                        xmlDirectory + "qMRMLColorPickerWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  qMRMLColorPickerWidget* widget2 = new qMRMLColorPickerWidget();
  widget2->setObjectName("ColorPickerWidget2");

  vtkSmartPointer<vtkMRMLScene> scene2 = vtkSmartPointer<vtkMRMLScene>::New();

  vtkSmartPointer<vtkMRMLColorTableNode> colorTableNode =
    vtkSmartPointer<vtkMRMLColorTableNode>::New();
  colorTableNode->SetType(vtkMRMLColorTableNode::Labels);
  scene2->AddNode(colorTableNode);

  widget2->setMRMLScene(scene2);

  vtkSmartPointer<vtkMRMLFreeSurferProceduralColorNode> colorFreeSurferNode =
    vtkSmartPointer<vtkMRMLFreeSurferProceduralColorNode>::New();
  colorFreeSurferNode->SetTypeToRedBlue();
  scene2->AddNode(colorFreeSurferNode);


  // for some reasons it generate a warning if the type is changed.
  colorTableNode->NamesInitialisedOff();
  colorTableNode->SetTypeToCool1();

  vtkSmartPointer<vtkMRMLPETProceduralColorNode> colorPETNode =
    vtkSmartPointer<vtkMRMLPETProceduralColorNode>::New();
  colorPETNode->SetTypeToRainbow();
  scene2->AddNode(colorPETNode);

  etpWidget.addTestCase(widget2,
                        xmlDirectory + "qMRMLColorPickerWidgetEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

