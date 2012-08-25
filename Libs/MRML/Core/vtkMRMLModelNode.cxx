/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLModelNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/

// MRML includes
#include "vtkAssignAttributes.h"
#include "vtkEventBroker.h"
#include "vtkMRMLFreeSurferProceduralColorNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLModelStorageNode.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkFloatArray.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkTransformPolyDataFilter.h>

// STD includes
#include <sstream>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLModelNode);

//----------------------------------------------------------------------------
vtkMRMLModelNode::vtkMRMLModelNode()
{
  this->AssignAttributes = vtkAssignAttributes::New();
}

//----------------------------------------------------------------------------
vtkMRMLModelNode::~vtkMRMLModelNode()
{
  this->SetInputPolyData(NULL);
  this->AssignAttributes->Delete();
  this->AssignAttributes = NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLModelNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  this->Superclass::Copy(anode);
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(anode);
  if (modelNode)
    {
    this->SetInputPolyData(modelNode->GetInputPolyData());
    }
  this->EndModify(disabledModify);
}

//---------------------------------------------------------------------------
void vtkMRMLModelNode::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event,
                                           void *callData )
{
  if (this->GetInputPolyData() == vtkPolyData::SafeDownCast(caller) &&
      this->GetInputPolyData() != 0 &&
      event ==  vtkCommand::ModifiedEvent)
    {
    this->InvokeEvent(vtkMRMLModelNode::PolyDataModifiedEvent, NULL);
    }
  this->Superclass::ProcessMRMLEvents(caller, event, callData);
}

//----------------------------------------------------------------------------
vtkMRMLModelDisplayNode* vtkMRMLModelNode::GetModelDisplayNode()
{
  return vtkMRMLModelDisplayNode::SafeDownCast(this->GetDisplayNode());
}

//----------------------------------------------------------------------------
void vtkMRMLModelNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "\nPoly Data:";
  if (this->GetInputPolyData())
    {
    os << "\n";
    this->GetInputPolyData()->PrintSelf(os, indent.GetNextIndent());
    }
  else
    {
    os << " none\n";
    }
}

//----------------------------------------------------------------------------
void vtkMRMLModelNode::SetInputPolyData(vtkPolyData *polyData)
{
  if (polyData == this->GetInputPolyData())
    {
    return;
    }

  // Disconnect future old input polydata
  if (this->GetInputPolyData() != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations (
      this->GetInputPolyData(), vtkCommand::ModifiedEvent, this,
      this->MRMLCallbackCommand );
    }

  this->SetInputToPolyDataPipeline(polyData);

  // Connect polydata to display nodes
  // \todo Remove because not really necessary:
  // the model display node input polydata doesn't
  // depend on the model input polydata but output polydata.
  this->SetOutputPolyDataToDisplayNodes();

  // Observe new polydata
  if (this->GetInputPolyData() != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      this->GetInputPolyData(), vtkCommand::ModifiedEvent,
      this, this->MRMLCallbackCommand );
    }

  this->Modified();
  this->InvokeEvent( vtkMRMLModelNode::PolyDataModifiedEvent , this);
}

//----------------------------------------------------------------------------
void vtkMRMLModelNode::SetInputToPolyDataPipeline(vtkPolyData* polyData)
{
  std::cout << "====before====> "
            << (polyData ? polyData->GetReferenceCount() : 0)
            << std::endl;
  this->AssignAttributes->SetInput(polyData);
  std::cout << "====after=====> "
            << (this->GetInputPolyData() ? this->GetInputPolyData()->GetReferenceCount() : 0)
            << std::endl;
}

//----------------------------------------------------------------------------
vtkPolyData* vtkMRMLModelNode::GetInputPolyData()
{
  return vtkPolyData::SafeDownCast(this->AssignAttributes->GetInput());
}

//----------------------------------------------------------------------------
vtkPolyData* vtkMRMLModelNode::GetOutputPolyData()
{
  return this->AssignAttributes->GetPolyDataOutput();
}

//----------------------------------------------------------------------------
void vtkMRMLModelNode::OutputPolyDataModified()
{
  this->SetOutputPolyDataToDisplayNodes();
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLModelNode::SetOutputPolyDataToDisplayNodes()
{
  int ndisp = this->GetNumberOfDisplayNodes();
  for (int n=0; n<ndisp; ++n)
    {
    std::cout << "SetOutputPolyDataToDisplayNodes" << std::endl;
    vtkMRMLModelDisplayNode *dnode = vtkMRMLModelDisplayNode::SafeDownCast(
      this->GetNthDisplayNode(n));
    if (dnode)
      {
      dnode->SetInputPolyData(this->GetOutputPolyData());
      }
    }
}

//---------------------------------------------------------------------------
void vtkMRMLModelNode::AddPointScalars(vtkDataArray *array)
{
  if (array == NULL)
    {
    return;
    }
  if (this->GetInputPolyData() == NULL)
    {
    vtkErrorMacro("AddPointScalars: No poly data on model "
                  << (this->GetName() ? this->GetName() : "no_name"));
    return;
    }

  int numScalars = this->GetInputPolyData()->GetPointData()->GetNumberOfArrays();
  vtkDebugMacro("Model node has " << numScalars << " point scalars now, adding " << array->GetName());
  if (numScalars > 0)
    {
    // add array
    this->GetInputPolyData()->GetPointData()->AddArray(array);
    }
  else
    {
    // set the scalars
    this->GetInputPolyData()->GetPointData()->SetScalars(array);
    }
}

//---------------------------------------------------------------------------
void vtkMRMLModelNode::AddCellScalars(vtkDataArray *array)
{
  if (array == NULL)
    {
    return;
    }
  if (this->GetInputPolyData() == NULL)
    {
    vtkErrorMacro("AddCellScalars: No poly data on model "
                  << (this->GetName() ? this->GetName() : "no_name"));
    return;
    }

  int numScalars = this->GetInputPolyData()->GetCellData()->GetNumberOfArrays();
  vtkDebugMacro("Model node has " << numScalars << " cell scalars now, adding " << array->GetName());
  if (numScalars > 0)
    {
    // add array
    this->GetInputPolyData()->GetCellData()->AddArray(array);
    }
  else
    {
    // set the scalars
    this->GetInputPolyData()->GetCellData()->SetScalars(array);
    }
}

//---------------------------------------------------------------------------
void vtkMRMLModelNode::RemoveScalars(const char *scalarName)
{
  if (scalarName == NULL)
    {
    vtkErrorMacro("Scalar name is null");
    return;
    }
  if (this->GetInputPolyData() == NULL)
    {
    vtkErrorMacro("RemoveScalars: No poly data on model "
                  << (this->GetName() ? this->GetName() : "no_name"));
    return;
    }
  // try removing the array from the points first
  if (this->GetInputPolyData()->GetPointData())
    {
    this->GetInputPolyData()->GetPointData()->RemoveArray(scalarName);
    // it's a void method, how to check if it succeeded?
    }
  // try the cells
  if (this->GetInputPolyData()->GetCellData())
    {
    this->GetInputPolyData()->GetCellData()->RemoveArray(scalarName);
    }
}


//---------------------------------------------------------------------------
const char * vtkMRMLModelNode::GetActivePointScalarName(int type)
{
  if (this->GetInputPolyData() == NULL ||
      this->GetOutputPolyData() == NULL ||
      this->GetOutputPolyData()->GetPointData() == NULL)
    {
    return NULL;
    }
  vtkAbstractArray* attributeArray =
    this->GetOutputPolyData()->GetPointData()->GetAbstractAttribute(type);
  return attributeArray ? attributeArray->GetName() : NULL;
}

//---------------------------------------------------------------------------
const char * vtkMRMLModelNode::GetActiveCellScalarName(int type)
{
  if (this->GetInputPolyData() == NULL ||
      this->GetOutputPolyData() == NULL ||
      this->GetOutputPolyData()->GetCellData() == NULL)
    {
    return NULL;
    }
  vtkAbstractArray* attributeArray =
    this->GetOutputPolyData()->GetCellData()->GetAbstractAttribute(type);
  return attributeArray ? attributeArray->GetName() : NULL;
}

//---------------------------------------------------------------------------
bool vtkMRMLModelNode::HasPointScalarName(const char* scalarName)
{
if (this->GetOutputPolyData() == NULL ||
      this->GetOutputPolyData()->GetPointData() == NULL)
    {
    return false;
    }
  return static_cast<bool>(
    this->GetOutputPolyData()->GetPointData()->HasArray(scalarName));
}

//---------------------------------------------------------------------------
bool vtkMRMLModelNode::HasCellScalarName(const char* scalarName)
{
  if (this->GetOutputPolyData() == NULL ||
      this->GetOutputPolyData()->GetCellData() == NULL)
    {
    return false;
    }
  return static_cast<bool>(
    this->GetOutputPolyData()->GetCellData()->HasArray(scalarName));
}

//---------------------------------------------------------------------------
int vtkMRMLModelNode::GetAttributeTypeFromString(const char* typeName)
{
  if (typeName != NULL && (strcmp(typeName, "") != 0))
    {
    for (int a = 0; a < vtkDataSetAttributes::NUM_ATTRIBUTES; a++)
      {
      if (strcmp(typeName, vtkDataSetAttributes::GetAttributeTypeAsString(a)) == 0)
        {
        return a;
        }
      }
    }
  return -1;
}

//---------------------------------------------------------------------------
int vtkMRMLModelNode::SetActiveScalars(const char *scalarName, int attribute)
{
  int retval = -1;
  // is it a point scalar?
  retval = this->SetActivePointScalars(scalarName, attribute);
  if (retval != -1)
    {
    return retval;
    }
  // is it a cell scalar?
  retval =  this->SetActiveCellScalars(scalarName, attribute);
  if (retval != -1)
    {
    return retval;
    }
  vtkDebugMacro("Unable to find scalar attribute " << attribute << " "
                << scalarName << " on model " << this->GetName());
  return retval;
}

//---------------------------------------------------------------------------
int vtkMRMLModelNode::SetActivePointScalars(const char *scalarName, int attributeType)
{
  int assignment = 0;
  for(; assignment  < this->AssignAttributes->GetNumberOfAssignments();)
    {
    if (this->AssignAttributes->GetAttributeType(assignment) == attributeType &&
        this->AssignAttributes->GetAttributeLocationType(assignment) ==
        vtkAssignAttributes::POINT_DATA)
      {
      std::cout << "before: " << this->AssignAttributes->GetNumberOfAssignments();
      this->AssignAttributes->RemoveAssignment(assignment);
      std::cout << "after: " << this->AssignAttributes->GetNumberOfAssignments();
      }
    else
      {
      ++assignment;
      }
    }
  int res = this->AssignAttributes->Assign(
    scalarName, attributeType, vtkAssignAttributes::POINT_DATA);
  return res;
}

//---------------------------------------------------------------------------
int vtkMRMLModelNode::SetActiveCellScalars(const char *scalarName, int attributeType)
{
  int assignment = 0;
  for(; assignment  < this->AssignAttributes->GetNumberOfAssignments();)
    {
    if (this->AssignAttributes->GetAttributeType(assignment) == attributeType &&
        this->AssignAttributes->GetAttributeLocationType(assignment) ==
        vtkAssignAttributes::CELL_DATA)
      {
      this->AssignAttributes->RemoveAssignment(assignment);
      }
    else
      {
      ++assignment;
      }
    }
  int res = this->AssignAttributes->Assign(
    scalarName, attributeType, vtkAssignAttributes::CELL_DATA);
  return res;
}

//---------------------------------------------------------------------------
int vtkMRMLModelNode::CompositeScalars(const char* backgroundName, const char* overlayName,
                                       float overlayMin, float overlayMax,
                                       int showOverlayPositive, int showOverlayNegative,
                                       int reverseOverlay)
{

    if (backgroundName == NULL || overlayName == NULL)
      {
      vtkErrorMacro("CompositeScalars: one of the input array names is null");
      return 0;
      }

    bool haveCurvScalars = false;
    // is there a curv scalar in the composite?
    if (strstr(backgroundName, "curv") != NULL ||
        strstr(overlayName, "curv") != NULL)
      {
      haveCurvScalars = true;
      }
    
    // get the scalars to composite, putting any curv file in scalars 1
    vtkDataArray *scalars1, *scalars2;
    if (!haveCurvScalars ||
        strstr(backgroundName, "curv") != NULL)
      {
      scalars1 = this->GetInputPolyData()->GetPointData()->GetScalars(backgroundName);
      scalars2 = this->GetInputPolyData()->GetPointData()->GetScalars(overlayName);
      }
    else
      {
      scalars1 = this->GetInputPolyData()->GetPointData()->GetScalars(overlayName);
      scalars2 = this->GetInputPolyData()->GetPointData()->GetScalars(backgroundName);
      }
    if (scalars1 == NULL || scalars2 == NULL)
      {
      vtkErrorMacro("CompositeScalars: unable to find the named scalar arrays " << backgroundName << " and/or " << overlayName);
      return 0;
      }
    if (scalars1->GetNumberOfTuples() != scalars1->GetNumberOfTuples())
      {
      vtkErrorMacro("CompositeScalars: sizes of scalar arrays don't match");
      return 0;
      }
    // Get the number of elements and initialize the composed scalar
    // array.
    int cValues = 0;
    cValues = scalars1->GetNumberOfTuples();

    vtkFloatArray* composedScalars = vtkFloatArray::New();

    std::stringstream ss;
    ss << backgroundName;
    ss << "+";
    ss << overlayName;
    std::string composedName = std::string(ss.str());
    composedScalars->SetName(composedName.c_str());
    composedScalars->Allocate( cValues );
    composedScalars->SetNumberOfComponents( 1 );
   
    // For each value, check the overlay value. If it's < min, use
    // the background value. If we're reversing, reverse the overlay
    // value. If we're not showing one side, use the background
    // value. If we are showing curvature (and have it), the
    // background value is our curvature value.
    float overlayMid = 0.5 * (overlayMax - overlayMin) + overlayMin; // 2.0;
    vtkDebugMacro("CompositeScalars: using overlay mid = " << overlayMid);
    float overlay = 0.0;
    float background = 0.0;
    for( int nValue = 0; nValue < cValues; nValue++ )
      {
      background = scalars1->GetTuple1(nValue);
      overlay = scalars2->GetTuple1(nValue);
      
      if( reverseOverlay )
        {
        overlay = -overlay;
        }
      if( overlay > 0 && !showOverlayPositive )
        {
        overlay = 0;
        }
      
      if( overlay < 0 && !showOverlayNegative )
        {
        overlay = 0;
        }
      
      // Insert the appropriate color into the composed array.
      if( overlay < overlayMin &&
          overlay > -overlayMin )
        {
        composedScalars->InsertNextValue( background );
        }
      else
        {
        composedScalars->InsertNextValue( overlay );
        }
      }
    
    // set up a colour node
    vtkMRMLProceduralColorNode *colorNode = vtkMRMLProceduralColorNode::New();
    colorNode->SetName(composedName.c_str());
    // set the type to avoid error messages when copy it, as the default is -1
    colorNode->SetType(vtkMRMLFreeSurferProceduralColorNode::Custom);
    vtkColorTransferFunction *func = colorNode->GetColorTransferFunction();

    // adapted from FS code that assumed that one scalar was curvature, the
    // other heat overlay
    const double EPS = 0.00001; // epsilon
    double curvatureMin = 0;
    
    if (haveCurvScalars)
      {
      curvatureMin = 0.5;
      }
    bool bUseGray = true;
    if( overlayMin <= curvatureMin )
      {
      curvatureMin = overlayMin - EPS;
      bUseGray = false;
      }
    func->AddRGBPoint( -overlayMax, 0, 1, 1 );
    func->AddRGBPoint( -overlayMid, 0, 0, 1 );
    func->AddRGBPoint( -overlayMin, 0, 0, 1 );
    
    if( bUseGray && overlayMin != 0 )
      {
      func->AddRGBPoint( -overlayMin + EPS, 0.5, 0.5, 0.5 );
      if( haveCurvScalars)
        {
        func->AddRGBPoint( -curvatureMin - EPS, 0.5, 0.5, 0.5 );
        }
      }
    if( haveCurvScalars && overlayMin != 0 )
      {
      func->AddRGBPoint( -curvatureMin, 0.6, 0.6, 0.6 );
      func->AddRGBPoint(  0,            0.6, 0.6, 0.6 );
      func->AddRGBPoint(  EPS,          0.4, 0.4, 0.4 );
      func->AddRGBPoint(  curvatureMin, 0.4, 0.4, 0.4 );
      }
    
    if ( bUseGray && overlayMin != 0 )
      {
      if( haveCurvScalars )
        {
        func->AddRGBPoint( curvatureMin + EPS, 0.5, 0.5, 0.5 );
        }
      func->AddRGBPoint( overlayMin - EPS, 0.5, 0.5, 0.5 );
      }

    func->AddRGBPoint( overlayMin, 1, 0, 0 );
    func->AddRGBPoint( overlayMid, 1, 0, 0 );
    func->AddRGBPoint( overlayMax, 1, 1, 0 );
    
    func->Build();
    
    // use the new colornode
    this->Scene->AddNode(colorNode);
    vtkDebugMacro("CompositeScalars: created color transfer function, and added proc color node to scene, id = " << colorNode->GetID());
    if (colorNode->GetID() != NULL)
      {
      this->GetModelDisplayNode()->SetAndObserveColorNodeID(colorNode->GetID());
      this->GetModelDisplayNode()->SetScalarRange(-overlayMax, overlayMax);
      }
    
    // add the new scalars
    this->AddPointScalars(composedScalars);

    // make them active
    this->GetModelDisplayNode()->SetActiveScalarName(composedName.c_str());

    // clean up
    colorNode->Delete();
    colorNode = NULL;
    composedScalars->Delete();
    composedScalars = NULL;
    
    return 1;
}

//---------------------------------------------------------------------------
bool vtkMRMLModelNode::CanApplyNonLinearTransforms()const
{
  return true;
}

//---------------------------------------------------------------------------
void vtkMRMLModelNode::ApplyTransform(vtkAbstractTransform* transform)
{
  vtkTransformPolyDataFilter* transformFilter = vtkTransformPolyDataFilter::New();
  transformFilter->SetInput(this->GetInputPolyData());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

//  this->SetAndObservePolyData(transformFilter->GetOutput());
  this->GetInputPolyData()->DeepCopy(transformFilter->GetOutput());

  transformFilter->Delete();
}

//---------------------------------------------------------------------------
void vtkMRMLModelNode::GetRASBounds(double bounds[6])
{
  this->Superclass::GetRASBounds( bounds);

  if (this->GetOutputPolyData() == NULL)
  {
    return;
  }

  this->GetOutputPolyData()->ComputeBounds();

  double boundsLocal[6];
  this->GetOutputPolyData()->GetBounds(boundsLocal);

  vtkMatrix4x4 *localToRas = vtkMatrix4x4::New();
  localToRas->Identity();
  vtkMRMLTransformNode *transformNode = this->GetParentTransformNode();
  if ( transformNode )
    {
    vtkMatrix4x4 *rasToRAS = vtkMatrix4x4::New();;
    transformNode->GetMatrixTransformToWorld(rasToRAS);
    vtkMatrix4x4::Multiply4x4(rasToRAS, localToRas, localToRas);
    rasToRAS->Delete();
    }

  double minBounds[3], maxBounds[3];
  int i;
  for ( i=0; i<3; i++)
    {
    minBounds[i] = 1.0e10;
    maxBounds[i] = -1.0e10;
    }

  double ras[4];
  double pnts[8][4] = {
    {boundsLocal[0], boundsLocal[2], boundsLocal[4], 1},
    {boundsLocal[0], boundsLocal[3], boundsLocal[4], 1},
    {boundsLocal[0], boundsLocal[2], boundsLocal[5], 1},
    {boundsLocal[0], boundsLocal[3], boundsLocal[5], 1},
    {boundsLocal[1], boundsLocal[2], boundsLocal[4], 1},
    {boundsLocal[1], boundsLocal[3], boundsLocal[4], 1},
    {boundsLocal[1], boundsLocal[2], boundsLocal[5], 1},
    {boundsLocal[1], boundsLocal[3], boundsLocal[5], 1}
  };

  for ( i=0; i<8; i++)
    {
    localToRas->MultiplyPoint( pnts[i], ras );
    for (int n=0; n<3; n++) {
      if (ras[n] < minBounds[n])
        {
        minBounds[n] = ras[n];
        }
      if (ras[n] > maxBounds[n])
        {
        maxBounds[n] = ras[n];
        }
      }
     }
   localToRas->Delete();
   for ( i=0; i<3; i++)
    {
    bounds[2*i]   = minBounds[i];
    bounds[2*i+1] = maxBounds[i];
    }
}

//---------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLModelNode::CreateDefaultStorageNode()
{
  return vtkMRMLStorageNode::SafeDownCast(vtkMRMLModelStorageNode::New());
}

//---------------------------------------------------------------------------
void vtkMRMLModelNode::OnDisplayNodeAdded(vtkMRMLDisplayNode *dnode)
{
  vtkMRMLModelDisplayNode* modelDisplayNode =
    vtkMRMLModelDisplayNode::SafeDownCast(dnode);
  if (modelDisplayNode)
    {
    modelDisplayNode->SetInputPolyData(this->GetOutputPolyData());
    }
  this->Superclass::OnDisplayNodeAdded(dnode);
}

//---------------------------------------------------------------------------
bool vtkMRMLModelNode::GetModifiedSinceRead()
{
  return this->Superclass::GetModifiedSinceRead() ||
    (this->GetInputPolyData() && this->GetInputPolyData()->GetMTime() > this->GetStoredTime());
}
