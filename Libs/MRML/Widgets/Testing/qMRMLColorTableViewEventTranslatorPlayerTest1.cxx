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
#include "qMRMLColorTableView.h"

// MRML includes
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLFreeSurferProceduralColorNode.h>
#include <vtkMRMLPETProceduralColorNode.h>

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
  qMRMLColorTableView* widget = reinterpret_cast<qMRMLColorTableView*>(data);

  Q_UNUSED(widget);
  }
}

//-----------------------------------------------------------------------------
int qMRMLColorTableViewEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = QString(argv[1]) + "/Libs/MRML/Widgets/Testing/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  ctkQtTestingUtility* testUtility = new ctkQtTestingUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QWidget topLevel;
  qMRMLColorTableView ColorTableView;
  qMRMLColorTableView ColorTableView1;
  qMRMLColorTableView ColorTableView2;

  QHBoxLayout* hboxLayout = new QHBoxLayout;
  hboxLayout->addWidget(&ColorTableView);
  hboxLayout->addWidget(&ColorTableView1);
  hboxLayout->addWidget(&ColorTableView2);
  topLevel.setLayout(hboxLayout);

  vtkSmartPointer<vtkMRMLColorTableNode> colorTableNode =
    vtkSmartPointer<vtkMRMLColorTableNode>::New();
  colorTableNode->SetType(vtkMRMLColorTableNode::Labels);

  ColorTableView.setMRMLColorNode(colorTableNode);
  // for some reasons it generate a warning if the type is changed.
  colorTableNode->NamesInitialisedOff();
  colorTableNode->SetTypeToCool1();

  vtkSmartPointer<vtkMRMLFreeSurferProceduralColorNode> colorFreeSurferNode =
    vtkSmartPointer<vtkMRMLFreeSurferProceduralColorNode>::New();
  colorFreeSurferNode->SetTypeToRedBlue();

  ColorTableView1.setMRMLColorNode(colorFreeSurferNode);
  colorFreeSurferNode->SetTypeToLabels();

  vtkSmartPointer<vtkMRMLPETProceduralColorNode> colorPETNode =
    vtkSmartPointer<vtkMRMLPETProceduralColorNode>::New();
  colorPETNode->SetTypeToRainbow();
  ColorTableView2.setMRMLColorNode(colorPETNode);
  colorPETNode->SetTypeToMIP();

  etpWidget.addTestCase(&topLevel,
                        xmlDirectory + "qMRMLColorTableViewEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

