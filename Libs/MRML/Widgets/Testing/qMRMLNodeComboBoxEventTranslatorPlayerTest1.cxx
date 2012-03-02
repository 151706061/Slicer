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
#include "qMRMLNodeComboBox.h"
#include "qMRMLNodeComboBoxEventPlayer.h"
#include "qMRMLNodeComboBoxEventTranslator.h"
#include "qMRMLSceneFactoryWidget.h"

// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  qMRMLNodeComboBox* widget = reinterpret_cast<qMRMLNodeComboBox*>(data);

  Q_UNUSED(widget);
  }
}

//-----------------------------------------------------------------------------
int qMRMLNodeComboBoxEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = QString(argv[1]) + "/Libs/MRML/Widgets/Testing/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  ctkQtTestingUtility* testUtility = new ctkQtTestingUtility(&etpWidget);
  testUtility->addTranslator(new qMRMLNodeComboBoxEventTranslator());
  testUtility->addPlayer(new qMRMLNodeComboBoxEventPlayer());
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  qMRMLNodeComboBox* widget = new qMRMLNodeComboBox();
//  widget->setEditEnabled(true);
  widget->setRenameEnabled(true);
  qMRMLSceneFactoryWidget sceneFactory;

  sceneFactory.generateScene();
  sceneFactory.generateNode("vtkMRMLViewNode");
  widget->setNodeTypes(QStringList("vtkMRMLViewNode"));
  widget->setMRMLScene(sceneFactory.mrmlScene());

  etpWidget.addTestCase(widget,
                        xmlDirectory + "qMRMLNodeComboBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

//  sceneFactory.deleteScene();

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

