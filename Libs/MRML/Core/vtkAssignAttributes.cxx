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
  Module:    vtkAssignAttributes.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkAssignAttributes.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkGraph.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"

// STD includes
#include <cassert>
#include <ctype.h>

vtkStandardNewMacro(vtkAssignAttributes);

char vtkAssignAttributes::AttributeLocationNames[vtkAssignAttributes::NUM_ATTRIBUTE_LOCS][12] 
= { "POINT_DATA",
    "CELL_DATA",
    "VERTEX_DATA",
    "EDGE_DATA"};

char vtkAssignAttributes::AttributeNames[vtkDataSetAttributes::NUM_ATTRIBUTES][20]  = { {0} };

//---------------------------------------------------------------------------
vtkAssignAttributes::Assignment::Assignment()
{
  this->FieldName = 0;
  this->AttributeLocationType = -1;
  this->AttributeType = -1;
  this->InputAttributeType = -1;
  this->FieldType = -1;
}

//---------------------------------------------------------------------------
vtkAssignAttributes::Assignment::Assignment(const vtkAssignAttributes::Assignment& assignment)
{
  if (assignment.FieldName)
    {
    this->FieldName = new char[strlen(assignment.FieldName)+1];
    strcpy(this->FieldName, assignment.FieldName);
    }
  else
    {
    this->FieldName = 0;
    }
  this->AttributeLocationType = assignment.AttributeLocationType;
  this->AttributeType = assignment.AttributeType;
  this->InputAttributeType = assignment.InputAttributeType;
  this->FieldType = assignment.FieldType;
}

//---------------------------------------------------------------------------
vtkAssignAttributes::Assignment::~Assignment()
{
  delete [] this->FieldName;
}

//---------------------------------------------------------------------------
vtkAssignAttributes::vtkAssignAttributes()
{
  //convert the attribute names to uppercase for local use
  if (vtkAssignAttributes::AttributeNames[0][0] == 0)
    {
    for (int i = 0; i < vtkDataSetAttributes::NUM_ATTRIBUTES; i++)
      {
      int l = static_cast<int>(strlen(vtkDataSetAttributes::GetAttributeTypeAsString(i)));
      for (int c = 0; c < l && c < 19; c++)
        {
        vtkAssignAttributes::AttributeNames[i][c] =
          toupper(vtkDataSetAttributes::GetAttributeTypeAsString(i)[c]);
        }
      }
    }
}

//---------------------------------------------------------------------------
vtkAssignAttributes::~vtkAssignAttributes()
{
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::Assign(const char* fieldName, int attributeType,
                                int attributeLoc)
{
  if (!fieldName)
    {
    return -1;
    }

  if ( (attributeType < 0) ||
       (attributeType > vtkDataSetAttributes::NUM_ATTRIBUTES) )
    {
    vtkErrorMacro("Wrong attribute type.");
    return -1;
    }

  if ( (attributeLoc < 0) ||
       (attributeLoc > vtkAssignAttributes::NUM_ATTRIBUTE_LOCS) )
    {
    vtkErrorMacro("The source for the field is wrong.");
    return -1;
    }

  vtkAssignAttributes::Assignment assignment;
  assignment.FieldName = new char[strlen(fieldName)+1];
  strcpy(assignment.FieldName, fieldName);

  assignment.AttributeType = attributeType;
  assignment.AttributeLocationType = attributeLoc;
  assignment.FieldType = vtkAssignAttributes::NAME;
  this->Assignments.push_back(vtkAssignAttributes::Assignment());
  int index = this->Assignments.size() -1;

  this->Modified();
  return index;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::Assign(int inputAttributeType, int attributeType,
                                int attributeLoc)
{
  if ( (attributeType < 0) ||
       (attributeType > vtkDataSetAttributes::NUM_ATTRIBUTES) ||
       (inputAttributeType < 0) ||
       (inputAttributeType > vtkDataSetAttributes::NUM_ATTRIBUTES))
    {
    vtkErrorMacro("Wrong attribute type.");
    return -1;
    }

  if ( (attributeLoc < 0) ||
       (attributeLoc > vtkAssignAttributes::NUM_ATTRIBUTE_LOCS) )
    {
    vtkErrorMacro("The source for the field is wrong.");
    return -1;
    }

  Assignment assignment;
  assignment.AttributeType = attributeType;
  assignment.InputAttributeType = inputAttributeType;
  assignment.AttributeLocationType = attributeLoc;
  assignment.FieldType = vtkAssignAttributes::ATTRIBUTE;
  this->Assignments.push_back(assignment);
  int index = this->Assignments.size() -1;

  this->Modified();
  return index;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::Assign(const char* name,
                                const char* attributeType,
                                const char* attributeLoc)
{
  if (!name || !attributeType || !attributeLoc)
    {
    return -1;
    }

  int numAttr = vtkDataSetAttributes::NUM_ATTRIBUTES;
  int numAttributeLocs = vtkAssignAttributes::NUM_ATTRIBUTE_LOCS;
  int i;

  // Convert strings to ints and call the appropriate Assign()
  int inputAttributeType=-1;
  for(i=0; i<numAttr; i++)
    {
    if (!strcmp(name, AttributeNames[i]))
      {
      inputAttributeType = i;
      break;
      }
    }

  int attrType=-1;
  for(i=0; i<numAttr; i++)
    {
    if (!strcmp(attributeType, AttributeNames[i]))
      {
      attrType = i;
      break;
      }
    }
  if ( attrType == -1 )
    {
    vtkErrorMacro("Target attribute type is invalid.");
    return -1;
    }

  int loc=-1;
  for(i=0; i<numAttributeLocs; i++)
    {
    if (!strcmp(attributeLoc, AttributeLocationNames[i]))
      {
      loc = i;
      break;
      }
    }
  if (loc == -1)
    {
    vtkErrorMacro("Target location for the attribute is invalid.");
    return -1;
    }

  int index = -1;
  if ( inputAttributeType == -1 )
    {
    index = this->Assign(name, attrType, loc);
    }
  else
    {
    index = this->Assign(inputAttributeType, attrType, loc);
    }
  return index;
}

//---------------------------------------------------------------------------
void vtkAssignAttributes::RemoveAssignment(int assignment)
{
  assert(assignment >= 0);
  assert(assignment < this->GetNumberOfAssignments());
  this->Assignments.erase(this->Assignments.begin() + assignment);
  this->Modified();
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::GetNumberOfAssignments()
{
  return this->Assignments.size();
}

//---------------------------------------------------------------------------
const char* vtkAssignAttributes::GetFieldName(int assignment)
{
  if (assignment < 0 || assignment >= this->GetNumberOfAssignments())
    {
    return 0;
    }
  return this->Assignments[assignment].FieldName;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::GetFieldType(int assignment)
{
  if (assignment < 0 || assignment >= this->GetNumberOfAssignments())
    {
    return -1;
    }
  return this->Assignments[assignment].FieldType;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::GetAttributeType(int assignment)
{
  if (assignment < 0 || assignment >= this->GetNumberOfAssignments())
    {
    return -1;
    }
  return this->Assignments[assignment].AttributeType;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::GetInputAttributeType(int assignment)
{
  if (assignment < 0 || assignment >= this->GetNumberOfAssignments())
    {
    return -1;
    }
  return this->Assignments[assignment].InputAttributeType;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::GetAttributeLocationType(int assignment)
{
  if (assignment < 0 || assignment >= this->GetNumberOfAssignments())
    {
    return -1;
    }
  return this->Assignments[assignment].AttributeLocationType;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::RequestInformation(vtkInformation *vtkNotUsed(request),
                                           vtkInformationVector **inputVector,
                                           vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  for (int assignmentIndex = 0; this->GetNumberOfAssignments(); ++assignmentIndex)
    {
    Assignment& assignment = this->Assignments[assignmentIndex];
    if ((assignment.AttributeType != -1) &&
        (assignment.AttributeLocationType != -1) &&
        (assignment.FieldType != -1))
      {
      int fieldAssociation = vtkDataObject::FIELD_ASSOCIATION_POINTS;
      switch (assignment.AttributeLocationType)
        {
        case POINT_DATA:
          fieldAssociation = vtkDataObject::FIELD_ASSOCIATION_POINTS;
          break;
        case CELL_DATA:
          fieldAssociation = vtkDataObject::FIELD_ASSOCIATION_CELLS;
          break;
        case VERTEX_DATA:
          fieldAssociation = vtkDataObject::FIELD_ASSOCIATION_VERTICES;
          break;
        default:
          fieldAssociation = vtkDataObject::FIELD_ASSOCIATION_EDGES;
          break;
        }
      if (assignment.FieldType == vtkAssignAttributes::NAME &&
          assignment.FieldName)
        {
        vtkDataObject::SetActiveAttribute(outInfo, fieldAssociation,
                                          assignment.FieldName, assignment.AttributeType);
        }
      else if (assignment.FieldType == vtkAssignAttributes::ATTRIBUTE  &&
               assignment.InputAttributeType != -1)
        {
        vtkInformation *inputAttributeInfo =
          vtkDataObject::GetActiveFieldInformation(
            inInfo, fieldAssociation, assignment.InputAttributeType);
        if (inputAttributeInfo) // do we have an active field of requested type
          {
          vtkDataObject::SetActiveAttribute(
            outInfo, fieldAssociation,
            inputAttributeInfo->Get( vtkDataObject::FIELD_NAME() ),
            assignment.AttributeType);
          }
        }
      }
    }
  return 1;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkDataObject *input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkDataObject *output = outInfo->Get(vtkDataObject::DATA_OBJECT());

  if (vtkDataSet::SafeDownCast(input))
    {
    vtkDataSet *dsInput = vtkDataSet::SafeDownCast(input);
    vtkDataSet *dsOutput = vtkDataSet::SafeDownCast(output);
    // This has to be here because it initialized all field datas.
    dsOutput->CopyStructure( dsInput );

    if ( dsOutput->GetFieldData() && dsInput->GetFieldData() )
      {
      dsOutput->GetFieldData()->PassData( dsInput->GetFieldData() );
      }
    dsOutput->GetPointData()->PassData( dsInput->GetPointData() );
    dsOutput->GetCellData()->PassData( dsInput->GetCellData() );
    }
  else
    {
    vtkGraph *graphInput = vtkGraph::SafeDownCast(input);
    vtkGraph *graphOutput = vtkGraph::SafeDownCast(output);
    graphOutput->ShallowCopy( graphInput );
    }

  for (int assignmentIndex = 0; this->GetNumberOfAssignments(); ++assignmentIndex)
    {
    Assignment& assignment = this->Assignments[assignmentIndex];

    vtkDataSetAttributes* ods=0;
    if (vtkDataSet::SafeDownCast(input))
      {
      vtkDataSet *dsOutput = vtkDataSet::SafeDownCast(output);
      switch (assignment.AttributeLocationType)
        {
        case vtkAssignAttributes::POINT_DATA:
          ods = dsOutput->GetPointData();
          break;
        case vtkAssignAttributes::CELL_DATA:
          ods = dsOutput->GetCellData();
          break;
        default:
          vtkErrorMacro(<<"Data must be point or cell for vtkDataSet");
          return 0;
        }
      }
    else
      {
      vtkGraph *graphOutput = vtkGraph::SafeDownCast(output);
      switch (assignment.AttributeLocationType)
        {
        case vtkAssignAttributes::VERTEX_DATA:
          ods = graphOutput->GetVertexData();
          break;
        case vtkAssignAttributes::EDGE_DATA:
          ods = graphOutput->GetEdgeData();
          break;
        default:
          vtkErrorMacro(<<"Data must be vertex or edge for vtkGraph");
          return 0;
        }
      }

    if ((assignment.AttributeType != -1) &&
        (assignment.AttributeLocationType != -1) && (assignment.FieldType != -1))
      {
      // Get the appropriate output DataSetAttributes
      if (assignment.FieldType == vtkAssignAttributes::NAME && assignment.FieldName)
        {
        ods->SetActiveAttribute(assignment.FieldName, assignment.AttributeType);
        }
      else if (assignment.FieldType == vtkAssignAttributes::ATTRIBUTE  &&
               (assignment.InputAttributeType != -1))
        {
        // If labeling an attribute as another attribute, we
        // need to get it's index and call SetActiveAttribute()
        // with that index
        //int attributeIndices[vtkDataSetAttributes::NUM_ATTRIBUTES];
        //ods->GetAttributeIndices(attributeIndices);
        // if (attributeIndices[assignment.InputAttributeType] != -1)
        vtkAbstractArray *oaa = ods->GetAbstractAttribute(assignment.InputAttributeType);
        if (oaa)
          {
          ods->SetActiveAttribute(oaa->GetName(),assignment.AttributeType);
          }
        }
      }
    }
  return 1;
}

//---------------------------------------------------------------------------
int vtkAssignAttributes::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  // This algorithm may accept a vtkPointSet or vtkGraph.
  info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkGraph");
  return 1;
}

//---------------------------------------------------------------------------
void vtkAssignAttributes::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  for (int assignmentIndex = 0; this->GetNumberOfAssignments(); ++assignmentIndex)
    {
    Assignment& assignment = this->Assignments[assignmentIndex];

    os << indent << "Field name: ";
    if (assignment.FieldName)
      {
      os << assignment.FieldName << endl;
      }
    else
      {
      os << "(none)" << endl;
      }
    os << indent << "Field type: " << assignment.FieldType << endl;
    os << indent << "Attribute type: " << assignment.AttributeType << endl;
    os << indent << "Input attribute type: " << assignment.InputAttributeType
       << endl;
    os << indent << "Attribute location: " << assignment.AttributeLocationType << endl;
    }
}
