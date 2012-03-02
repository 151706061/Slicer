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
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkQtTestingUtility.h"

// qMRML includes
#include "qMRMLNavigationView.h"
#include "qMRMLThreeDView.h"

// MRML includes
#include <vtkMRMLViewNode.h>
#include <vtkMRMLScene.h>

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
  qMRMLNavigationView* widget = reinterpret_cast<qMRMLNavigationView*>(data);

  Q_UNUSED(widget);
  }
}

//-----------------------------------------------------------------------------
int qMRMLNavigationViewEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = QString(argv[1]) + "/Libs/MRML/Widgets/Testing/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  ctkQtTestingUtility* testUtility = new ctkQtTestingUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QWidget topLevel;

  qMRMLNavigationView navigationView;
  navigationView.setWindowTitle("Navigation view");

  qMRMLThreeDView threeDView;
  threeDView.setWindowTitle("ThreeDView");

  QHBoxLayout* hboxLayout = new QHBoxLayout;
  hboxLayout->addWidget(&navigationView);
  hboxLayout->addWidget(&threeDView);
  topLevel.setLayout(hboxLayout);

  navigationView.setRendererToListen(threeDView.renderer());

  vtkSmartPointer<vtkMRMLScene> scene = vtkSmartPointer<vtkMRMLScene>::New();
  navigationView.setMRMLScene(scene);
  threeDView.setMRMLScene(scene);

  vtkMRMLViewNode* viewNode = vtkMRMLViewNode::New();
  viewNode->SetBoxVisible(true);
  scene->AddNode(viewNode);
  viewNode->Delete();

  threeDView.setMRMLViewNode(viewNode);
  navigationView.setMRMLViewNode(viewNode);

  etpWidget.addTestCase(&topLevel,
                        xmlDirectory + "qMRMLNavigationViewEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

