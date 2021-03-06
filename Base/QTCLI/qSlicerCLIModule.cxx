/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#include "qSlicerCLIModule.h"

// Qt includes
#include <QDebug>

// SlicerQT includes
#include "qMRMLNodeComboBox.h"
#include "qSlicerCLIModuleWidget.h"
#include "vtkSlicerCLIModuleLogic.h"

// Slicer vtk includes
//#include <vtkCommandLineModuleLogic.h>

// ModuleDescriptionParser includes
#include <ModuleDescriptionParser.h>

//-----------------------------------------------------------------------------
class qSlicerCLIModulePrivate
{
public:
  typedef qSlicerCLIModulePrivate Self;
  qSlicerCLIModulePrivate();

  QString           Title;
  QString           Acknowledgement;
  QString           Help;
  QString           Category;
  QString           Contributor;
  int               Index;

  QString           EntryPoint;
  QString           ModuleType;
  QString           TempDirectory;

  ModuleDescription                 Desc;
  ModuleProcessInformation*         ProcessInformation;
};

//-----------------------------------------------------------------------------
// qSlicerCLIModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCLIModulePrivate::qSlicerCLIModulePrivate()
{
  this->ProcessInformation = 0;
  this->Index = -1;
}

//-----------------------------------------------------------------------------
// qSlicerCLIModule methods

//-----------------------------------------------------------------------------
qSlicerCLIModule::qSlicerCLIModule(QWidget* _parent):Superclass(_parent)
  , d_ptr(new qSlicerCLIModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerCLIModule::~qSlicerCLIModule()
{
}

//-----------------------------------------------------------------------------
void qSlicerCLIModule::setup()
{
  Q_D(qSlicerCLIModule);
  
  // Temporary directory should be set before the module is initialized
  Q_ASSERT(!d->TempDirectory.isEmpty());

  // Set temp directory 
  vtkSlicerCLIModuleLogic* myLogic = vtkSlicerCLIModuleLogic::SafeDownCast(this->logic());
  myLogic->SetTemporaryDirectory(d->TempDirectory.toLatin1());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerCLIModule::createWidgetRepresentation()
{
  Q_D(qSlicerCLIModule);
  return new qSlicerCLIModuleWidget(&d->Desc);
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerCLIModule::createLogic()
{
  return vtkSlicerCLIModuleLogic::New();
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(qSlicerCLIModule, QString, title, Title);
CTK_GET_CPP(qSlicerCLIModule, QString, category, Category);
CTK_GET_CPP(qSlicerCLIModule, QString, contributor, Contributor);
CTK_GET_CPP(qSlicerCLIModule, int, index, Index);
CTK_GET_CPP(qSlicerCLIModule, QString, acknowledgementText, Acknowledgement);
CTK_GET_CPP(qSlicerCLIModule, QString, helpText, Help);
CTK_SET_CPP(qSlicerCLIModule, const QString&, setTempDirectory, TempDirectory);
CTK_GET_CPP(qSlicerCLIModule, QString, tempDirectory, TempDirectory);
CTK_SET_CPP(qSlicerCLIModule, const QString&, setEntryPoint, EntryPoint);
CTK_GET_CPP(qSlicerCLIModule, QString, entryPoint, EntryPoint);
CTK_SET_CPP(qSlicerCLIModule, const QString&, setModuleType, ModuleType);
CTK_GET_CPP(qSlicerCLIModule, QString, moduleType, ModuleType);

//-----------------------------------------------------------------------------
void qSlicerCLIModule::setXmlModuleDescription(const QString& xmlModuleDescription)
{
  Q_D(qSlicerCLIModule);
  //qDebug() << "xmlModuleDescription:" << xmlModuleDescription;
  Q_ASSERT(!this->entryPoint().isEmpty());

  // Parse module description
  ModuleDescription desc;
  ModuleDescriptionParser parser;
  if (parser.Parse(xmlModuleDescription.toStdString(), desc) != 0)
    {
    qWarning() << "Failed to parse xml module description";
    return;
    }

  // Set properties
  d->Title = QString::fromStdString(desc.GetTitle());
  d->Acknowledgement = QString::fromStdString(desc.GetAcknowledgements());
  d->Contributor = QString::fromStdString(desc.GetContributor());
  bool ok = false;
  d->Index = QString::fromStdString(desc.GetIndex()).toInt(&ok);
  if (!ok)
    {
    d->Index = -1;
    }
  d->Category = QString::fromStdString(desc.GetCategory());

  d->ProcessInformation = desc.GetProcessInformation();

  QString help =
    "%1<br>"
    "For more detailed documentation see:<br>"
    "%2";

  d->Help = help.arg(
    QString::fromStdString(desc.GetDescription())).arg(
    QString::fromStdString(desc.GetDocumentationURL()));

  // Set module type
  desc.SetType(this->moduleType().toStdString());
  
  // Set module entry point
  desc.SetTarget(this->entryPoint().toStdString());

  d->Desc = desc; 
}

//-----------------------------------------------------------------------------
vtkSlicerCLIModuleLogic* qSlicerCLIModule::cliModuleLogic()
{
  vtkSlicerCLIModuleLogic* myLogic = vtkSlicerCLIModuleLogic::SafeDownCast(this->logic());
  return myLogic;
}

//-----------------------------------------------------------------------------
vtkMRMLCommandLineModuleNode* qSlicerCLIModule::createNode()
{
  qSlicerCLIModuleWidget* widget = dynamic_cast<qSlicerCLIModuleWidget*>(this->widgetRepresentation());
  Q_ASSERT(widget);
  qMRMLNodeComboBox * selector = widget->findChild<qMRMLNodeComboBox*>(QLatin1String("MRMLCommandLineModuleNodeSelector"));
  Q_ASSERT(selector);
  vtkMRMLCommandLineModuleNode* node = vtkMRMLCommandLineModuleNode::SafeDownCast(selector->addNode());
  Q_ASSERT(node);
  return node;
}

//-----------------------------------------------------------------------------
void qSlicerCLIModule::run(vtkMRMLCommandLineModuleNode* parameterNode, bool waitForCompletion)
{
  if (!parameterNode)
    {
    return;
    }

  Q_ASSERT(this->cliModuleLogic());

  if (waitForCompletion)
    {
    this->cliModuleLogic()->ApplyAndWait(parameterNode);
    }
  else
    {
    this->cliModuleLogic()->Apply(parameterNode);
    }
}

//-----------------------------------------------------------------------------
void qSlicerCLIModule::cancel(vtkMRMLCommandLineModuleNode* node)
{
  if (!node)
    {
    return;
    }
  qDebug() << "Cancel module processing...";
  node->SetStatus(vtkMRMLCommandLineModuleNode::Cancelled);
}

