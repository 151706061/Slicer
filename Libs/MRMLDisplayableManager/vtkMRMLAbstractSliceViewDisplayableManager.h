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

/// Superclass for displayable manager classes.
/// 
/// A displayable manager class is responsible to represent a
/// MRMLDisplayable node in a renderer.
/// 


#ifndef __vtkMRMLAbstractSliceViewDisplayableManager_h
#define __vtkMRMLAbstractSliceViewDisplayableManager_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractDisplayableManager.h"

#include "vtkMRMLDisplayableManagerWin32Header.h"

class vtkMRMLSliceNode;

class VTK_MRML_DISPLAYABLEMANAGER_EXPORT vtkMRMLAbstractSliceViewDisplayableManager :
    public vtkMRMLAbstractDisplayableManager
{
public:
  
  static vtkMRMLAbstractSliceViewDisplayableManager *New();
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkTypeRevisionMacro(vtkMRMLAbstractSliceViewDisplayableManager,
                       vtkMRMLAbstractDisplayableManager);

  ///
  /// Get MRML SliceNode
  vtkMRMLSliceNode * GetMRMLSliceNode();

protected:

  vtkMRMLAbstractSliceViewDisplayableManager();
  virtual ~vtkMRMLAbstractSliceViewDisplayableManager();

  virtual void OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller);

  /// Could be overloaded if DisplayableManager subclass
  virtual void OnMRMLSliceNodeModifiedEvent(){}
  
private:

  vtkMRMLAbstractSliceViewDisplayableManager(const vtkMRMLAbstractSliceViewDisplayableManager&); // Not implemented
  void operator=(const vtkMRMLAbstractSliceViewDisplayableManager&);                    // Not implemented
};

#endif

