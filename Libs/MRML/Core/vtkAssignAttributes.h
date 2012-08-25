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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAssignAttribute.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAssignAttributes - Add multiple attribute support to
// vtkAssignAttribute

#ifndef __vtkAssignAttributes_h
#define __vtkAssignAttributes_h

// MRML includes
#include "vtkMRML.h"

// VTK includes
#include "vtkDataSetAttributes.h" // Needed for NUM_ATTRIBUTES
#include "vtkPassInputTypeAlgorithm.h"
class vtkFieldData;

// STD includes
#include <vector>

class VTK_MRML_EXPORT vtkAssignAttributes : public vtkPassInputTypeAlgorithm
{
public:
  vtkTypeMacro(vtkAssignAttributes,vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create a new vtkAssignAttributes.
  static vtkAssignAttributes *New();

  // Description:
  // Label an attribute as another attribute.
  // Return the index of assignment or -1 if it fails.
  int Assign(int inputAttributeType, int attributeType, int attributeLoc);

  // Description:
  // Label an array as an attribute.
  // Return the index of assignment or -1 if it fails.
  int Assign(const char* fieldName, int attributeType, int attributeLoc);

  // Description:
  // Helper method used by other language bindings. Allows the caller to
  // specify arguments as strings instead of enums.
  // Return the index of assignment or -1 if it fails.
  int Assign(const char* name, const char* attributeType,
              const char* attributeLoc);

  void RemoveAssignment(int assignment);

  int GetNumberOfAssignments();
  const char* GetFieldName(int assignment);
  int GetFieldType(int assignment);
  int GetAttributeType(int assignment);
  int GetInputAttributeType(int assignment);
  int GetAttributeLocationType(int assignment);

//BTX
  // Always keep NUM_ATTRIBUTE_LOCS as the last entry
  enum AttributeLocation
  {
    POINT_DATA=0,
    CELL_DATA=1,
    VERTEX_DATA=2,
    EDGE_DATA=3,
    NUM_ATTRIBUTE_LOCS
  };
//ETX

protected:

//BTX
  enum FieldType
  {
    NAME,
    ATTRIBUTE
  };
//ETX

  vtkAssignAttributes();
  virtual ~vtkAssignAttributes();

  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int FillInputPortInformation(int, vtkInformation *);

  struct Assignment
  {
    Assignment();
    Assignment(const Assignment&);
    ~Assignment();

    char* FieldName;
    int FieldType;
    int AttributeType;
    int InputAttributeType;
    int AttributeLocationType;
  };
  std::vector<Assignment> Assignments;

  static char AttributeLocationNames[vtkAssignAttributes::NUM_ATTRIBUTE_LOCS][12];
  static char AttributeNames[vtkDataSetAttributes::NUM_ATTRIBUTES][20];
private:
  vtkAssignAttributes(const vtkAssignAttributes&);  // Not implemented.
  void operator=(const vtkAssignAttributes&);  // Not implemented.
};

#endif
