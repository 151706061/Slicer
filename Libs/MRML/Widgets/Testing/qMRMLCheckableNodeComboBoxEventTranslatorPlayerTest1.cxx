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
#include "qMRMLCheckableNodeComboBox.h"
#include "qMRMLCheckableNodeComboBoxEventPlayer.h"
#include "qMRMLSceneFactoryWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  qMRMLCheckableNodeComboBox* widget = reinterpret_cast<qMRMLCheckableNodeComboBox*>(data);

  Q_UNUSED(widget);
  }
}

//-----------------------------------------------------------------------------
int qMRMLCheckableNodeComboBoxEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = QString(argv[1]) + "/Libs/MRML/Widgets/Testing/";
  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  ctkQtTestingUtility* testUtility = new ctkQtTestingUtility(&etpWidget);
  testUtility->addPlayer(new qMRMLCheckableNodeComboBoxEventPlayer());
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  qMRMLCheckableNodeComboBox* widget = new qMRMLCheckableNodeComboBox();
  widget->setNodeTypes(QStringList("vtkMRMLViewNode"));

  qMRMLSceneFactoryWidget sceneFactory;
  sceneFactory.generateScene();
  sceneFactory.generateNode("vtkMRMLViewNode");
  sceneFactory.generateNode("vtkMRMLViewNode");

  widget->setMRMLScene(sceneFactory.mrmlScene());
  sceneFactory.generateNode("vtkMRMLViewNode");
  sceneFactory.generateNode("vtkMRMLViewNode");
  sceneFactory.generateNode("vtkMRMLViewNode");

  etpWidget.addTestCase(widget,
                        xmlDirectory + "qMRMLCheckableNodeComboBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

