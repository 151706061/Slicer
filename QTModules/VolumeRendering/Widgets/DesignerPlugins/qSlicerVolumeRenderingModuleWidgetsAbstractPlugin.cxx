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

#include "qSlicerVolumeRenderingModuleWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
qSlicerVolumeRenderingModuleWidgetsAbstractPlugin::qSlicerVolumeRenderingModuleWidgetsAbstractPlugin()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVolumeRenderingModuleWidgetsAbstractPlugin::group() const
{
  return "Slicer [Volume Rendering Widgets]";
}

//-----------------------------------------------------------------------------
QIcon qSlicerVolumeRenderingModuleWidgetsAbstractPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString qSlicerVolumeRenderingModuleWidgetsAbstractPlugin::toolTip() const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerVolumeRenderingModuleWidgetsAbstractPlugin::whatsThis() const
{
  return QString();
}
