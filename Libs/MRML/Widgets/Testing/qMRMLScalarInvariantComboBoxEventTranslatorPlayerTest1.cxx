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
#include "qMRMLScalarInvariantComboBox.h"

// MRML includes
#include <vtkMRMLDiffusionTensorDisplayPropertiesNode.h>

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
  qMRMLScalarInvariantComboBox* widget = reinterpret_cast<qMRMLScalarInvariantComboBox*>(data);

  CTKCOMPARE(widget->scalarInvariant(), vtkMRMLDiffusionTensorDisplayPropertiesNode::D22);
  }
}

//-----------------------------------------------------------------------------
int qMRMLScalarInvariantComboBoxEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = QString(argv[1]) + "/Libs/MRML/Widgets/Testing/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  ctkQtTestingUtility* testUtility = new ctkQtTestingUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  qMRMLScalarInvariantComboBox* widget = new qMRMLScalarInvariantComboBox();

  vtkSmartPointer<vtkMRMLDiffusionTensorDisplayPropertiesNode> displayPropertiesNode
    = vtkSmartPointer<vtkMRMLDiffusionTensorDisplayPropertiesNode>::New();

  widget->setDisplayPropertiesNode(displayPropertiesNode);

  etpWidget.addTestCase(widget,
                        xmlDirectory + "qMRMLScalarInvariantComboBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

