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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerAbstractModuleFactoryManager_h
#define __qSlicerAbstractModuleFactoryManager_h

// Qt includes
#include <QObject>
#include <QString>

// CTK includes
#include <ctkAbstractFileBasedFactory.h>

#include "qSlicerBaseQTCoreExport.h"

class qSlicerAbstractCoreModule;

class qSlicerAbstractModuleFactoryManagerPrivate;

class Q_SLICER_BASE_QTCORE_EXPORT qSlicerAbstractModuleFactoryManager : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QStringList searchPaths READ searchPaths WRITE setSearchPaths)
  Q_PROPERTY(QStringList modulesToSkip READ modulesToSkip WRITE setModulesToSkip NOTIFY modulesToSkipChanged)
public:
  typedef ctkAbstractFileBasedFactory<qSlicerAbstractCoreModule> qSlicerFileBasedModuleFactory;
  typedef ctkAbstractFactory<qSlicerAbstractCoreModule> qSlicerModuleFactory;

  typedef QObject Superclass;
  qSlicerAbstractModuleFactoryManager(QObject * newParent = 0);

  /// Destructor, Deallocates resources
  /// Unregister (and delete) all registered factories.
  virtual ~qSlicerAbstractModuleFactoryManager();

  /// Print internal state using qDebug()
  virtual void printAdditionalInfo();

  /// \brief Register a \a factory
  /// The factory will be deleted when unregistered
  /// (e.g. in ~qSlicerAbstractModuleFactoryManager())
  /// Example:
  /// qSlicerAbstractModuleFactoryManager factoryManager;
  /// factoryManager.registerFactory(new qSlicerCoreModuleFactory);
  /// factoryManager.registerFactory(new qSlicerLoadableModuleFactory);
  /// The order in which factories are registered is important. When scanning
  /// directories, registered factories are browse and the first factory that
  /// can read a file is used.
  void registerFactory(qSlicerModuleFactory* factory);
  void unregisterFactory(qSlicerModuleFactory* factory);
  void unregisterFactories();

  void setSearchPaths(const QStringList& searchPaths);
  QStringList searchPaths()const;

  inline void addSearchPaths(const QStringList& paths);
  inline void addSearchPath(const QString& path);

  inline void removeSearchPaths(const QStringList& paths);
  inline void removeSearchPath(const QString& path);

  void setModulesToSkip(const QStringList& modulesNames);
  QStringList modulesToSkip()const;

  inline void addModuleToSkip(const QString& moduleName);
  inline void removeModuleToSkip(const QString& moduleName);

  /// After the modules are registered, skippedModules contains the list
  /// of all the modules that were ignored.
  QStringList skippedModuleNames()const;

  /// Register all modules
  void registerModules();

  /// Convenient method returning the list of all registered module names
  Q_INVOKABLE QStringList registeredModuleNames() const;

  /// Indicate if a module has been registered
  Q_INVOKABLE bool isRegistered(const QString& name)const;

  /// Instanciate all modules
  void instantiateModules();

  /// List of registered and instantiated modules
  Q_INVOKABLE QStringList instantiatedModuleNames() const;

  /// Indicate if a module has been instantiated
  Q_INVOKABLE bool isInstantiated(const QString& name)const;

  /// Return the instance of a module if already instantiated, 0 otherwise
  Q_INVOKABLE qSlicerAbstractCoreModule* moduleInstance(const QString& moduleName)const;

  /// Uninstantiate all instantiated modules
  void uninstantiateModules();

  /// Enable/Disable verbose output during module discovery process
  void setVerboseModuleDiscovery(bool value);

signals:
  /// \brief This signal is emitted when all the modules associated with the
  /// registered factories have been loaded
  void modulesRegistered(const QStringList& moduleNames);
  void moduleRegistered(const QString& moduleName);

  void modulesToSkipChanged(const QStringList& moduleNames);
  void moduleSkipped(const QString& moduleName);

  void modulesInstantiated(const QStringList& moduleNames);
  void moduleInstantiated(const QString& moduleName);

  void modulesAboutToBeUninstantiated(const QStringList& moduleNames);
  void moduleAboutToBeUninstantiated(const QString& moduleName);

  void modulesUninstantiated(const QStringList& moduleNames);
  void moduleUninstantiated(const QString& moduleName);

protected:
  QScopedPointer<qSlicerAbstractModuleFactoryManagerPrivate> d_ptr;

  void registerModules(const QString& directoryPath);
  void registerModule(const QFileInfo& file);

  /// Instantiate a module given its \a name
  qSlicerAbstractCoreModule* instantiateModule(const QString& name);

  /// Uninstantiate a module given its \a moduleName
  virtual void uninstantiateModule(const QString& moduleName);

private:
  Q_DECLARE_PRIVATE(qSlicerAbstractModuleFactoryManager);
  Q_DISABLE_COPY(qSlicerAbstractModuleFactoryManager);
};

//-----------------------------------------------------------------------------
void qSlicerAbstractModuleFactoryManager::addSearchPaths(const QStringList& paths)
{
  this->setSearchPaths(this->searchPaths() << paths);
}

//-----------------------------------------------------------------------------
void qSlicerAbstractModuleFactoryManager::addSearchPath(const QString& path)
{
  this->setSearchPaths(this->searchPaths() << path);
}

//-----------------------------------------------------------------------------
void qSlicerAbstractModuleFactoryManager::removeSearchPaths(const QStringList& paths)
{
  foreach(const QString& path, paths)
    {
    this->removeSearchPath(path);
    }
}

//-----------------------------------------------------------------------------
void qSlicerAbstractModuleFactoryManager::removeSearchPath(const QString& path)
{
  QStringList newSearchPaths = this->searchPaths();
  newSearchPaths.removeAll(path);
  this->setSearchPaths(newSearchPaths);
}

//-----------------------------------------------------------------------------
void qSlicerAbstractModuleFactoryManager::addModuleToSkip(const QString& moduleName)
{
  QStringList modules = this->modulesToSkip();
  if (modules.contains(moduleName))
    {
    return;
    }
  modules << moduleName;
  this->setModulesToSkip(modules);
}

//-----------------------------------------------------------------------------
void qSlicerAbstractModuleFactoryManager::removeModuleToSkip(const QString& moduleName)
{
  QStringList modules = this->modulesToSkip();
  modules.removeAll(moduleName);
  this->setModulesToSkip(modules);
}

#endif
