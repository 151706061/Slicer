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

#ifndef __qSlicerCLIModule_h
#define __qSlicerCLIModule_h

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerAbstractModule.h"

#include "qSlicerBaseQTCLIExport.h"

class vtkMRMLCommandLineModuleNode;
class vtkSlicerCLIModuleLogic;

class qSlicerCLIModulePrivate;
class Q_SLICER_BASE_QTCLI_EXPORT qSlicerCLIModule : public qSlicerAbstractModule
{
  Q_OBJECT
public:

  typedef qSlicerAbstractModule Superclass;
  qSlicerCLIModule(QWidget *parent=0);
  virtual ~qSlicerCLIModule();

  /// 
  /// Assign the module XML description.
  /// Note: That will also trigger the parsing of the XML structure
  void setXmlModuleDescription(const QString& xmlModuleDescription);

  ///
  /// Optionally set in the module XML description
  virtual int index() const;

  /// 
  /// Return help/acknowledgement text
  virtual QString helpText() const;
  virtual QString acknowledgementText() const;

  ///
  /// Set temporary directory associated with the module
  void setTempDirectory(const QString& tempDirectory);
  QString tempDirectory() const;

  ///
  /// Set module entry point. Typically "slicer:0x012345" for loadable CLI
  /// or "/home/user/work/Slicer-Superbuild/../mycliexec" for executable CLI
  void setEntryPoint(const QString& entryPoint);
  QString entryPoint() const;

  ///
  /// SharedObjectModule for loadable modules or CommandLineModule for
  /// executable modules.
  void setModuleType(const QString& type);
  QString moduleType() const;

  ///
  /// This method allows to get a pointer to the ModuleLogic.
  /// If no moduleLogic already exists, one will be created calling
  /// 'createLogic' method.
  Q_INVOKABLE vtkSlicerCLIModuleLogic* cliModuleLogic();


  virtual QString title() const;
  virtual QString category() const;
  virtual QString contributor() const;

  /// Using the following function to create a commandLineModule node will ensure
  /// ensure the created node is selected within the UI.
  vtkMRMLCommandLineModuleNode* createNode();

  /// Run a command line module given \a parameterNode
  /// If \a waitForCompletion is True, the call will return only upon completion of
  /// the module execution.
  void run(vtkMRMLCommandLineModuleNode* parameterNode, bool waitForCompletion = false);

  /// Abort the execution of the module associated with \a node
  void cancel(vtkMRMLCommandLineModuleNode* node);

protected:
  /// 
  virtual void setup();

  /// 
  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// 
  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerCLIModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerCLIModule);
  Q_DISABLE_COPY(qSlicerCLIModule);
};

#endif
