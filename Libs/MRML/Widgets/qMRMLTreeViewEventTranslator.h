/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qMRMLTreeViewEventTranslator_h
#define __qMRMLTreeViewEventTranslator_h

// QT includes
#include "QModelIndexList"

// QtTesting inlcudes
#include <pqWidgetEventTranslator.h>

// qMRML includes
#include "qMRMLWidgetsExport.h"

class vtkMRMLNode;

class QMRML_WIDGETS_EXPORT qMRMLTreeViewEventTranslator :
  public pqWidgetEventTranslator
{
  Q_OBJECT

public:
  qMRMLTreeViewEventTranslator(QObject* parent = 0);

  virtual bool translateEvent(QObject *Object, QEvent *Event, bool &Error);

private:
  qMRMLTreeViewEventTranslator(const qMRMLTreeViewEventTranslator&); // NOT implemented
  qMRMLTreeViewEventTranslator& operator=(const qMRMLTreeViewEventTranslator&); // NOT implemented
  
  QObject* CurrentObject;
  
private slots:
  void onDestroyed(QObject*);
  void onCurrentNodeDeleted(const QModelIndex&);
  void onCurrentNodeRenamed(const QString&);
//  void onEditNodeRequested(vtkMRMLNode*);
  void onDecorationClicked(const QModelIndex&);
  void onAboutToReparentByDnD(vtkMRMLNode*, vtkMRMLNode*);

private:
  QString getIndexAsString(const QModelIndex&);
};

#endif
