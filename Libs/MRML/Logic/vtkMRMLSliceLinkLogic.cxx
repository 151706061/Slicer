/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLSliceLinkLogic.cxx,v $
  Date:      $Date$
  Version:   $Revision$

=========================================================================auto=*/

// MRMLLogic includes
#include "vtkMRMLSliceLinkLogic.h"
#include "vtkMRMLSliceLogic.h"
#include "vtkMRMLApplicationLogic.h"

// MRML includes
#include <vtkEventBroker.h>
#include <vtkMRMLSliceCompositeNode.h>
#include <vtkMRMLSliceNode.h>

// VTK includes
#include <vtkCollection.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

// STD includes
#include <cassert>


//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkMRMLSliceLinkLogic, "$Revision$");
vtkStandardNewMacro(vtkMRMLSliceLinkLogic);

//----------------------------------------------------------------------------
vtkMRMLSliceLinkLogic::vtkMRMLSliceLinkLogic()
{
  this->BroadcastingEvents = 0;
}

//----------------------------------------------------------------------------
vtkMRMLSliceLinkLogic::~vtkMRMLSliceLinkLogic()
{
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::BroadcastingEventsOn()
{
  this->BroadcastingEvents++;
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::BroadcastingEventsOff()
{
  this->BroadcastingEvents--;
}

//----------------------------------------------------------------------------
int vtkMRMLSliceLinkLogic::GetBroadcastingEvents()
{
  return this->BroadcastingEvents;
}


//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  // List of events the slice logics should listen
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  vtkSmartPointer<vtkFloatArray> priorities =vtkSmartPointer<vtkFloatArray>::New();

  float normalPriority = 0.0;
  float lowPriority = -0.5;
  // float highPriority = 0.5;

  // Events that use the default priority.  Don't care the order they
  // are triggered
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  priorities->InsertNextValue(normalPriority);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  priorities->InsertNextValue(normalPriority);
  events->InsertNextValue(vtkMRMLScene::StartBatchProcessEvent);
  priorities->InsertNextValue(normalPriority);
  events->InsertNextValue(vtkMRMLScene::StartImportEvent);
  priorities->InsertNextValue(normalPriority);
  events->InsertNextValue(vtkMRMLScene::StartRestoreEvent);
  priorities->InsertNextValue(normalPriority);

  // Events that need to a lower priority than normal, in other words,
  // guaranteed to be be called after other triggers
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  priorities->InsertNextValue(lowPriority);
  events->InsertNextValue(vtkMRMLScene::EndImportEvent);
  priorities->InsertNextValue(lowPriority);
  events->InsertNextValue(vtkMRMLScene::EndRestoreEvent);
  priorities->InsertNextValue(lowPriority);

  this->SetAndObserveMRMLSceneEventsInternal(newScene, events, priorities);

  this->ProcessMRMLSceneEvents(newScene, vtkCommand::ModifiedEvent, 0);
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  if (node->IsA("vtkMRMLSliceCompositeNode") 
      || node->IsA("vtkMRMLSliceNode"))
    {
    vtkEventBroker::GetInstance()->AddObservation(
      node, vtkCommand::ModifiedEvent, this, this->GetMRMLNodesCallbackCommand());

    // If sliceNode we insert in our map the current status of the node
    vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
    SliceNodeStatusMap::iterator it = this->SliceNodeInteractionStatus.find(node->GetID());
    if (sliceNode && it == this->SliceNodeInteractionStatus.end())
      {
      this->SliceNodeInteractionStatus.insert(std::pair<std::string, SliceNodeInfos>
        (sliceNode->GetID(), SliceNodeInfos(sliceNode->GetInteracting())));
      }
    }
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  if (node->IsA("vtkMRMLSliceCompositeNode") 
      || node->IsA("vtkMRMLSliceNode"))
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      node, vtkCommand::ModifiedEvent, this, this->GetMRMLNodesCallbackCommand());

    // Update the map
    vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
    if (sliceNode)
      {
      SliceNodeStatusMap::iterator it = this->SliceNodeInteractionStatus.find(node->GetID());
      if(it != this->SliceNodeInteractionStatus.end())
        {
        this->SliceNodeInteractionStatus.erase(it);
        }
      }
  }
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLNodeModified(vtkMRMLNode* node)
{
  // Update from SliceNode
  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
  if (sliceNode && !this->GetMRMLScene()->IsBatchProcessing())
    {

    SliceNodeStatusMap::iterator it = this->SliceNodeInteractionStatus.find(sliceNode->GetID());
    // if this is not the node that we are interacting with, short circuit

    if (!sliceNode->GetInteracting() || !sliceNode->GetInteractionFlags())
      {
      // We end up an interaction on the sliceNode
      if (it != this->SliceNodeInteractionStatus.end() && it->second.Interacting)
        {
        vtkMRMLSliceCompositeNode* compositeNode = this->GetCompositeNode(sliceNode);
        if (!compositeNode->GetHotLinkedControl() &&
            sliceNode->GetInteractionFlags() == vtkMRMLSliceNode::MultiplanarReformatFlag)
          {
          this->BroadcastSliceNodeEvent(sliceNode);
          }
        this->SliceNodeInteractionStatus.find(sliceNode->GetID())->second.Interacting =
          sliceNode->GetInteracting();
        }
      return;
      }

    // SliceNode was modified. Need to find the corresponding
    // SliceCompositeNode to check whether operations are linked
    vtkMRMLSliceCompositeNode* compositeNode = this->GetCompositeNode(sliceNode);

    if (compositeNode && compositeNode->GetLinkedControl())
      {
      // Slice node changed and slices are linked. Broadcast.
      //std::cout << "Slice node changed and slices are linked!" << std::endl;

      if (it != this->SliceNodeInteractionStatus.end() && !it->second.Interacting )
        {
        it->second.Interacting = sliceNode->GetInteracting();
        // Start Interaction event : we update the current sliceNodeNormal
        this->UpdateSliceNodeInteractionStatus(sliceNode);
        }

      if (compositeNode->GetHotLinkedControl() ||
          sliceNode->GetInteractionFlags() != vtkMRMLSliceNode::MultiplanarReformatFlag)
        {
        this->BroadcastSliceNodeEvent(sliceNode);
        }
      }
    else
      {
      // Slice node changed and slices are not linked. Do not broadcast.
      //std::cout << "Slice node changed and slices are NOT linked!" << std::endl;
      return;
      }
    }

  // Update from SliceCompositeNode
  vtkMRMLSliceCompositeNode* compositeNode 
    = vtkMRMLSliceCompositeNode::SafeDownCast(node);
  if (compositeNode && !this->GetMRMLScene()->IsBatchProcessing())
    {

    // if this is not the node that we are interacting with, short circuit
    if (!compositeNode->GetInteracting() 
        || !compositeNode->GetInteractionFlags())
      {
      return;
      }

    if (compositeNode && compositeNode->GetLinkedControl())
      {
      // Slice composite node changed and slices are linked. Broadcast.
      //std::cout << "SliceCompositeNode changed and slices are linked!" << std::endl;
      this->BroadcastSliceCompositeNodeEvent(compositeNode);
      }
    else
      {
      // Slice composite node changed and slices are not linked. Do
      // not broadcast.
      //std::cout << "SliceCompositeNode changed and slices are NOT linked!" << std::endl;
      }
    }
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneStartBatchProcess()
{
  // Note the sense. Turning "on" tells the link logic that we are
  // already broadcasting an event, so don't rebroadcast.
  //std::cerr << "OnMRMLSceneStartBatchProcess" << std::endl;
  this->BroadcastingEventsOn();
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneEndBatchProcess()
{
  // Note the sense. Turning "off" tells the link logic that we are
  // not already broadcasting an event, so future events can be broadcast
  //std::cerr << "OnMRMLSceneEndBatchProcess" << std::endl;
  this->BroadcastingEventsOff();
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneStartImport()
{
  // Note the sense. Turning "on" tells the link logic that we are
  // already broadcasting an event, so don't rebroadcast.
  //std::cerr << "OnMRMLSceneStartImport" << std::endl;
  this->BroadcastingEventsOn();
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneEndImport()
{
  // Note the sense. Turning "off" tells the link logic that we are
  // not already broadcasting an event, so future events can be broadcast
  //std::cerr << "OnMRMLSceneEndImport" << std::endl;
  this->BroadcastingEventsOff();
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneStartRestore()
{
  // Note the sense. Turning "on" tells the link logic that we are
  // already broadcasting an event, so don't rebroadcast.
  //std::cerr << "OnMRMLSceneStartRestore" << std::endl;
  this->BroadcastingEventsOn();
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::OnMRMLSceneEndRestore()
{
  // Note the sense. Turning "off" tells the link logic that we are
  // not already broadcasting an event, so future events can be broadcast
  //std::cerr << "OnMRMLSceneEndRestore" << std::endl;
  this->BroadcastingEventsOff();
}


//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  vtkIndent nextIndent;
  nextIndent = indent.GetNextIndent();
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::BroadcastSliceNodeEvent(vtkMRMLSliceNode *sliceNode)
{
  // only broadcast a slice node event if we are not already actively
  // broadcasting events and we are actively interacting with the node
  // std::cout << "BroadcastingEvents: " << this->GetBroadcastingEvents()
  //           << ", Interacting: " << sliceNode->GetInteracting()
  //           << std::endl;
  if (!this->GetBroadcastingEvents())
    {
    this->BroadcastingEventsOn();

    vtkMRMLSliceNode* sNode;
    vtkCollectionSimpleIterator it;
    vtkSmartPointer<vtkCollection> nodes;
    nodes.TakeReference(this->GetMRMLScene()->GetNodesByClass("vtkMRMLSliceNode"));
    for (nodes->InitTraversal(it);
        (sNode=vtkMRMLSliceNode::SafeDownCast(nodes->GetNextItemAsObject(it)));)
      {
      if (sNode != sliceNode)
        {
        // Link slice parameters whenever the reformation is consistent
        if (!strcmp(sNode->GetOrientationString(),
                    sliceNode->GetOrientationString()))
        // if (sliceNode->Matrix4x4AreEqual(sliceNode->GetSliceToRAS(),
        //                                  sNode->GetSliceToRAS()))
          {
          // std::cout << "Orientation match, flags = " << sliceNode->GetInteractionFlags() << std::endl;
          // std::cout << "Broadcasting SliceToRAS, SliceOrigin, and FieldOfView to "
          //            << sNode->GetName() << std::endl;
          //

          // Copy the slice to RAS information
          if (sliceNode->GetInteractionFlags() & sliceNode->GetInteractionFlagsModifier()
            & vtkMRMLSliceNode::SliceToRASFlag)
            {
            // Need to copy the SliceToRAS. SliceNode::SetSliceToRAS()
            // does a shallow copy. So we have to explictly call DeepCopy()
            sNode->GetSliceToRAS()->DeepCopy( sliceNode->GetSliceToRAS() );
            }

          // Copy the slice origin information
          if (sliceNode->GetInteractionFlags() & sliceNode->GetInteractionFlagsModifier()
            & vtkMRMLSliceNode::XYZOriginFlag)
            {
            // Need to copy the SliceOrigin. 
            double *xyzOrigin = sliceNode->GetXYZOrigin();
            sNode->SetXYZOrigin( xyzOrigin[0], xyzOrigin[1], xyzOrigin[2] );
            }

          // Copy the field of view information. Use the new
          // prescribed x fov, aspect corrected y fov, and keep z fov
          // constant
          if (sliceNode->GetInteractionFlags() & sliceNode->GetInteractionFlagsModifier()
            & vtkMRMLSliceNode::FieldOfViewFlag)
            {
            sNode->SetFieldOfView( sliceNode->GetFieldOfView()[0],
                                   sliceNode->GetFieldOfView()[0]
                                   * sNode->GetFieldOfView()[1]
                                   / sNode->GetFieldOfView()[0],
                                   sNode->GetFieldOfView()[2] );
            }

          // need to manage prescribed spacing here as well?

          // Forces the internal matrices to be updated which results
          // in this being modified so a Render can occur
          sNode->UpdateMatrices();
          }

        //
        // Some parameters and commands do not require the
        // orientations to match. These are handled here.
        //

        // Setting the orientation of the slice plane does not
        // require that the orientations initially match.
        if (sliceNode->GetInteractionFlags() & sliceNode->GetInteractionFlagsModifier()
          & vtkMRMLSliceNode::OrientationFlag)
          {
          // We could copy the orientation strings, but we really
          // want the slice to ras to match, so copy that

          // Need to copy the SliceToRAS. SliceNode::SetSliceToRAS()
          // does a shallow copy. So we have to explictly call DeepCopy()
          sNode->GetSliceToRAS()->DeepCopy( sliceNode->GetSliceToRAS() );

          // Forces the internal matrices to be updated which results
          // in this being modified so a Render can occur
          sNode->UpdateMatrices();
          }

        // Reseting the field of view does not require the
        // orientations to match
        if ((sliceNode->GetInteractionFlags() & sliceNode->GetInteractionFlagsModifier()
            & vtkMRMLSliceNode::ResetFieldOfViewFlag)
            && this->GetMRMLApplicationLogic()->GetSliceLogics())
          {
          // need the logic for this slice (sNode)
          vtkMRMLSliceLogic* logic;
          vtkCollectionSimpleIterator it;
          vtkCollection* logics = this->GetMRMLApplicationLogic()->GetSliceLogics();
          for (logics->InitTraversal(it);
               (logic=vtkMRMLSliceLogic::SafeDownCast(logics->GetNextItemAsObject(it)));)
            {
            if (logic->GetSliceNode() == sNode)
              {
              logic->FitSliceToAll();
              sNode->UpdateMatrices();
              break;
              }
            }
          }

        // Broadcasting the rotation from a ReformatWidget
        if (sliceNode->GetInteractionFlags() & sliceNode->GetInteractionFlagsModifier()
          & vtkMRMLSliceNode::MultiplanarReformatFlag)
          {
          this->BroadcastLastRotation(sliceNode,sNode);
          }

        // Setting the label outline mode
        if (sliceNode->GetInteractionFlags() & sliceNode->GetInteractionFlagsModifier()
          & vtkMRMLSliceNode::LabelOutlineFlag)
          {
          sNode->SetUseLabelOutline( sliceNode->GetUseLabelOutline() );
          }
        

        //
        // End of the block for broadcasting parametes and command
        // that do not require the orientation to match
        //
        }
      }

    // Update SliceNodeInteractionStatus after MultiplanarReformat interaction
    this->UpdateSliceNodeInteractionStatus(sliceNode);
    this->BroadcastingEventsOff();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::BroadcastSliceCompositeNodeEvent(vtkMRMLSliceCompositeNode *sliceCompositeNode)
{
  // only broadcast a slice composite node event if we are not already actively
  // broadcasting events and we actively interacting with the node
  // std::cout << "BroadcastingEvents: " << this->GetBroadcastingEvents()
  //           << ", Interacting: " << sliceCompositeNode->GetInteracting()
  //           << std::endl;
  if (!this->GetBroadcastingEvents() && sliceCompositeNode->GetInteracting())
    {
    this->BroadcastingEventsOn();

    vtkMRMLSliceCompositeNode* cNode;
    vtkCollectionSimpleIterator it;
    vtkSmartPointer<vtkCollection> nodes;
    nodes.TakeReference(this->GetMRMLScene()->GetNodesByClass("vtkMRMLSliceCompositeNode"));

    for (nodes->InitTraversal(it);
        (cNode=vtkMRMLSliceCompositeNode::SafeDownCast(nodes->GetNextItemAsObject(it)));)
      {
      if (cNode != sliceCompositeNode)
        {
        if (sliceCompositeNode->GetInteractionFlags() & sliceCompositeNode->GetInteractionFlagsModifier() 
            & vtkMRMLSliceCompositeNode::ForegroundVolumeFlag)
          {
          //std::cerr << "Broadcasting Foreground Volume " << sliceCompositeNode->GetForegroundVolumeID() << std::endl;
          cNode->SetForegroundVolumeID(sliceCompositeNode->GetForegroundVolumeID());
          }

        if (sliceCompositeNode->GetInteractionFlags() & sliceCompositeNode->GetInteractionFlagsModifier() 
            & vtkMRMLSliceCompositeNode::BackgroundVolumeFlag)
          {
          cNode->SetBackgroundVolumeID(sliceCompositeNode->GetBackgroundVolumeID());
          }

        if (sliceCompositeNode->GetInteractionFlags() & sliceCompositeNode->GetInteractionFlagsModifier() 
            & vtkMRMLSliceCompositeNode::LabelVolumeFlag)
          {
          cNode->SetLabelVolumeID(sliceCompositeNode->GetLabelVolumeID());
          }
        }
      }

    this->BroadcastingEventsOff();
    }
}

//----------------------------------------------------------------------------
vtkMRMLSliceCompositeNode* vtkMRMLSliceLinkLogic::GetCompositeNode(vtkMRMLSliceNode* sliceNode)
{
  vtkMRMLSliceCompositeNode* compositeNode = 0;

  vtkCollectionSimpleIterator it;
  vtkSmartPointer<vtkCollection> nodes;
  nodes.TakeReference(this->GetMRMLScene()->GetNodesByClass("vtkMRMLSliceCompositeNode"));

  for (nodes->InitTraversal(it);
      (compositeNode=vtkMRMLSliceCompositeNode::SafeDownCast(nodes->GetNextItemAsObject(it)));)
    {
    if (compositeNode->GetLayoutName()
        && !strcmp(compositeNode->GetLayoutName(), sliceNode->GetName()))
      {
      break;
      }

    compositeNode = 0;
    }

  return compositeNode;
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::BroadcastLastRotation(vtkMRMLSliceNode* sliceNode,
                                                  vtkMRMLSliceNode* sNode)
{
  SliceNodeStatusMap::iterator it = this->SliceNodeInteractionStatus.find(sliceNode->GetID());
  if (it == this->SliceNodeInteractionStatus.end())
    {
    return;
    }

  // Calculate the rotation applied to the sliceNode
  double cross[3], dot, rotation;
  vtkNew<vtkTransform> transform;
  vtkMatrix4x4* sNodeToRAS = sNode->GetSliceToRAS();
  double sliceNormal[3] = {sliceNode->GetSliceToRAS()->GetElement(0,2),
                           sliceNode->GetSliceToRAS()->GetElement(1,2),
                           sliceNode->GetSliceToRAS()->GetElement(2,2)};

  // Rotate the sliceNode to match the planeWidget normal
  vtkMath::Cross(it->second.LastNormal,sliceNormal, cross);
  dot = vtkMath::Dot(it->second.LastNormal, sliceNormal);
  // Clamp the dot product
  dot = (dot < -1.0) ? -1.0 : (dot > 1.0 ? 1.0 : dot);
  rotation = vtkMath::DegreesFromRadians(acos(dot));

  // Apply the rotation
  transform->PostMultiply();
  transform->SetMatrix(sNodeToRAS);
  transform->RotateWXYZ(rotation,cross);
  transform->GetMatrix(sNodeToRAS); // Update the changes

  sNode->UpdateMatrices();
}

//----------------------------------------------------------------------------
void vtkMRMLSliceLinkLogic::UpdateSliceNodeInteractionStatus(vtkMRMLSliceNode* sliceNode)
{
  SliceNodeStatusMap::iterator it = this->SliceNodeInteractionStatus.find(sliceNode->GetID());

  if (it != SliceNodeInteractionStatus.end())
    {
    it->second.LastNormal[0] = sliceNode->GetSliceToRAS()->GetElement(0,2);
    it->second.LastNormal[1] = sliceNode->GetSliceToRAS()->GetElement(1,2);
    it->second.LastNormal[2] = sliceNode->GetSliceToRAS()->GetElement(2,2);
    }
}

