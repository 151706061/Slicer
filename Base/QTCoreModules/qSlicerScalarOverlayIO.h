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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerScalarOverlayIO
#define __qSlicerScalarOverlayIO

// SlicerQt includes
#include "qSlicerIO.h"
#include "qSlicerBaseQTCoreModulesExport.h"

//-----------------------------------------------------------------------------
class Q_SLICER_BASE_QTCOREMODULES_EXPORT qSlicerScalarOverlayIO: public qSlicerIO
{
  Q_OBJECT
public:
  qSlicerScalarOverlayIO(QObject* parent = 0);
  virtual QString description()const;
  virtual IOFileType fileType()const;
  virtual QStringList extensions()const;
  virtual qSlicerIOOptions* options()const;

  virtual bool load(const IOProperties& properties);
};

#endif
