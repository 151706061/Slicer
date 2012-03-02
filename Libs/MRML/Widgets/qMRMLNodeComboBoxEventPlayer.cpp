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
#include <QDebug>

// qMRML includes
#include "qMRMLNodeComboBox.h"
#include "qMRMLNodeComboBoxEventPlayer.h"

// MRML includes
#include "vtkMRMLNode.h"

// ----------------------------------------------------------------------------
qMRMLNodeComboBoxEventPlayer::qMRMLNodeComboBoxEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{

}

// ----------------------------------------------------------------------------
bool qMRMLNodeComboBoxEventPlayer::playEvent(QObject *Object,
                                    const QString &Command,
                                    const QString &Arguments,
                                    bool &Error)
{
  if (Command != "nodeAddedByUser" && Command != "currentNodeChanged" &&
      Command != "nodeAboutToBeRemoved" && Command != "nodeRenamed")
    {
    return false;
    }

  if (qMRMLNodeComboBox* const comboBox =
      qobject_cast<qMRMLNodeComboBox*>(Object))
    {
    if (Command == "nodeAddedByUser")
      {
      comboBox->addNode();
      return true;
      }
    if (Command == "currentNodeChanged")
      {
      if (Arguments == "None")
        {
        comboBox->setCurrentNodeIndex(0);
        }
      comboBox->setCurrentNode(Arguments);
      return true;
      }
    if (Command == "nodeAboutToBeRemoved")
      {
      comboBox->removeCurrentNode();
      return true;
      }
    if (Command == "nodeRenamed")
      {
      comboBox->currentNode()->SetName(Arguments.toLatin1());
      return true;
      }
    }

  qCritical() << "calling nodeAddedByUser/currentNodeChanged/nodeAboutToBeRemoved/nodeRenamed on unhandled type " << Object;
  Error = true;
  return true;
}

