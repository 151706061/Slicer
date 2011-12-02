/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLScene.h,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.18 $

=========================================================================auto=*/
//
//
///  vtkMRMLScene - A set of MRML Nodes that supports serialization and undo/redo
///
/// vtkMRMLScene represents and provides methods to manipulate a list of
/// MRML objects. The list is core and duplicate entries are not prevented.
//
/// .SECTION see also
/// vtkMRMLNode vtkCollection

#ifndef __vtkMRMLScene_h
#define __vtkMRMLScene_h

#include <list>
#include <map>
#include <vector>
#include <string>

#include <vtkCollection.h>

#include "vtkMRML.h"
class vtkCacheManager;
class vtkDataIOManager;
class vtkTagTable;

class vtkCallbackCommand;
class vtkGeneralTransform;
class vtkURIHandler;
class vtkMRMLNode;
class vtkMRMLSceneViewNode;

class VTK_MRML_EXPORT vtkMRMLScene : public vtkCollection
{
public:
  static vtkMRMLScene *New();
  vtkTypeMacro(vtkMRMLScene,vtkCollection);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// Set URL (file name) of the scene
  void SetURL(const char *url) {
    this->URL = std::string(url);
  };

  /// Get URL (file name) of the scene
  const char *GetURL() {
    return this->URL.c_str();
  };

  /// Set Root directory, where URL is pointing
  void SetRootDirectory(const char *dir) {
    this->RootDirectory = std::string(dir);
  };

  /// Get Root directory, where URL is pointing
  const char *GetRootDirectory() {
    return this->RootDirectory.c_str();
  };

  /// Create new scene from URL
  int Connect();

  /// Add the scene from URL
  int Import();

  /// Save scene into URL
  int Commit(const char* url=NULL);

  /// Remove nodes and clear undo/redo stacks
  void Clear(int removeSingletons);

  /// Reset all nodes to their constructor's state
  void ResetNodes();

  /// Create node with a given class
  vtkMRMLNode* CreateNodeByClass(const char* className);

  /// Register node class with the Scene so that it can create it from
  /// a class name
  /// -- this maintains a registered pointer to the node, so users should Delete()
  ///    the node after calling this.  The node is Deleted when the scene is destroyed.
  void RegisterNodeClass(vtkMRMLNode* node);
  /// Register node class with the Scene so that it can create it from
  /// a class name
  /// tagName can be a custom tagName
  /// -- this maintains a registered pointer to the node, so users should Delete()
  ///    the node after calling this.  The node is Deleted when the scene is destroyed.
  void RegisterNodeClass(vtkMRMLNode* node, const char* tagName);

  /// Add a path to the list.
  const char* GetClassNameByTag(const char *tagName);

  /// Add a path to the list.
  const char* GetTagByClassName(const char *className);

  /// return collection of nodes
  vtkCollection* GetNodes()
    {
    return this->Nodes;
    };

  /// Called by another class to request that the node's id be set to the given
  /// string
  /// If the id is not in use, set it, otherwise, useit as a base for a unique
  /// id and then set it
  void RequestNodeID(vtkMRMLNode *node, const char *ID);

  /// Add a node to the scene and send NewNode and SceneModified events.
  vtkMRMLNode* AddNode(vtkMRMLNode *n);

  /// Add a copy of a node to the scene.
  vtkMRMLNode* CopyNode(vtkMRMLNode *n);

  /// Add a node to the scene without invoking a NodeAdded Event
  vtkMRMLNode* AddNodeNoNotify(vtkMRMLNode *n);

  /// Invoke a NodeAddedEvent (used, for instnace, after adding a bunch of nodes with AddNodeNoNotify
  void NodeAdded(vtkMRMLNode *n);
  void NodeAdded() {this->NodeAdded(NULL);};

  /// Remove a path from the list.
  void RemoveNode(vtkMRMLNode *n);

  /// Determine whether a particular node is present. Returns its position
  /// in the list.
  int IsNodePresent(vtkMRMLNode *n) {
    return this->Nodes->vtkCollection::IsItemPresent((vtkObject *)n);};

  /// Initialize a traversal (not reentrant!)
  void InitTraversal() {
    if (this && this->Nodes)
      {
      this->Nodes->InitTraversal();
      }
  };

  /// Get next node in the scene.
  vtkMRMLNode *GetNextNode() {
    return (vtkMRMLNode *)(this->Nodes->GetNextItemAsObject());};

  /// Get next node of the class in the scene.
  vtkMRMLNode *GetNextNodeByClass(const char* className);

  /// Get nodes having the specified name
  vtkCollection *GetNodesByName(const char* name);

  /// Get node given a unique ID
  vtkMRMLNode *GetNodeByID(const char* name);
  vtkMRMLNode *GetNodeByID(std::string name);

  /// Get nodes of a specified class having the specified name
  vtkCollection *GetNodesByClassByName(const char* className, const char* name);

  /// Get number of nodes in the scene
  int GetNumberOfNodes () { return this->Nodes->GetNumberOfItems(); };

  /// Get n-th node in the scene
  vtkMRMLNode* GetNthNode(int n);

  /// Get n-th node of a specified class  in the scene
  vtkMRMLNode* GetNthNodeByClass(int n, const char* className );

  /// Get number of nodes of a specified class in the scene
  int GetNumberOfNodesByClass(const char* className);

  /// Get vector of nodes of a specified class in the scene
  int GetNodesByClass(const char *className, std::vector<vtkMRMLNode *> &nodes);
  vtkCollection* GetNodesByClass(const char *className);

  std::list<std::string> GetNodeClassesList();

  /// returns list of names
  const char* GetNodeClasses();

  /// Get the number of registered node classes (is probably greater than the current number
  /// of nodes instantiated in the scene)
  int GetNumberOfRegisteredNodeClasses();
  /// Get the nth registered node class, returns NULL if n is out of the range 0-GetNumberOfRegisteredNodeClasses
  /// Useful for iterating through nodes to find all the possible storage nodes.
  vtkMRMLNode * GetNthRegisteredNodeClass(int n);

  const char* GetUniqueNameByString(const char* className);
  /// Explore the MRML tree to find the next unique index for use as an ID,
  /// starting from 1
  int GetUniqueIDIndexByClass(const char* className);
  /// Explore the MRML tree to find the next unique index for use as an ID,
  /// starting from hint
  int GetUniqueIDIndexByClassFromIndex(const char* className, int hint);

  /// insert a node in the scene after a specified node
  vtkMRMLNode* InsertAfterNode( vtkMRMLNode *item, vtkMRMLNode *newItem);
  /// insert a node in the scene before a specified node
  vtkMRMLNode* InsertBeforeNode( vtkMRMLNode *item, vtkMRMLNode *newItem);

  /// Ger transformation between two nodes
  int GetTransformBetweenNodes( vtkMRMLNode *node1, vtkMRMLNode *node2,
                                vtkGeneralTransform *xform );

  /// Set undo on/off
  void SetUndoOn() {UndoFlag=true;};
  void SetUndoOff() {UndoFlag=false;};
  bool GetUndoFlag() {return UndoFlag;};
  void SetUndoFlag(bool flag) {UndoFlag = flag;};

  /// undo, set the scene to previous state
  void Undo();

  /// redo, set the scene to previously undone
  void Redo();

  /// clear Undo stack, delete undo history
  void ClearUndoStack();

  /// clear Redo stack, delete redo history
  void ClearRedoStack();

  /// returns number of undo steps in the history buffer
  int GetNumberOfUndoLevels() { return (int)this->UndoStack.size();};

  /// returns number of redo steps in the history buffer
  int GetNumberOfRedoLevels() { return (int)this->RedoStack.size();};

  /// Save current state in the undo buffer
  void SaveStateForUndo();
  /// Save current state of the node in the undo buffer
  void SaveStateForUndo(vtkMRMLNode *node);
  /// Save current state of the nodes in the undo buffer
  void SaveStateForUndo(vtkCollection *nodes);
  void SaveStateForUndo(std::vector<vtkMRMLNode *> nodes);

  void AddReferencedNodeID(const char *id, vtkMRMLNode *refrencingNode);

  void ClearReferencedNodeID()
  {
    this->ReferencedIDs.clear();
    this->ReferencingNodes.clear();
    this->ReferencedIDChanges.clear();
  };

  void RemoveReferencedNodeID(const char *id, vtkMRMLNode *refrencingNode);

  void RemoveNodeReferences(vtkMRMLNode *node);

  void RemoveReferencesToNode(vtkMRMLNode *node);

  void UpdateNodeReferences();

  void UpdateNodeReferences(vtkCollection* chekNodes);

  void CopyNodeReferences(vtkMRMLScene *scene);

  void UpdateNodeChangedIDs();

  void RemoveUnusedNodeReferences();

  void AddReservedID(const char *id);

  void RemoveReservedIDs() {
    this->ReservedIDs.clear();
  };

  /// get the new id of the node that is different from one in the mrml file
  /// or NULL if id has not changed
  const char* GetChangedID(const char* id);

  /// Return collection of all nodes referenced directly or indirectly by a node.
  vtkCollection* GetReferencedNodes(vtkMRMLNode *node);

  /// Get a sub-scene containing all nodes directly or indirectly reference by
  /// the input node
  void GetReferencedSubScene(vtkMRMLNode *node, vtkMRMLScene* newScene);

  /// Get/Set the active Scene
  static void SetActiveScene(vtkMRMLScene *);
  static vtkMRMLScene *GetActiveScene();

  int IsFilePathRelative(const char * filepath);

  vtkSetMacro(ErrorCode,unsigned long);
  vtkGetMacro(ErrorCode,unsigned long);

  vtkSetMacro(LoadFromXMLString,int);
  vtkGetMacro(LoadFromXMLString,int);

  vtkSetMacro(SaveToXMLString,int);
  vtkGetMacro(SaveToXMLString,int);

  vtkSetMacro(ReadDataOnLoad,int);
  vtkGetMacro(ReadDataOnLoad,int);

  void SetErrorMessage(const std::string &error) {
    this->ErrorMessage = error;
  };

  std::string GetErrorMessage() {
    return this->ErrorMessage;
  };

  void SetSceneXMLString(const std::string &xmlString) {
    this->SceneXMLString = xmlString;
  };

  std::string GetSceneXMLString() {
    return this->SceneXMLString;
  };

  void SetErrorMessage(const char * message)
    {
    this->SetErrorMessage(std::string(message));
    }

  const char *GetErrorMessagePointer()
    {
    return (this->GetErrorMessage().c_str());
    }

  unsigned long GetSceneModifiedTime()
    {
    if (this->Nodes && this->Nodes->GetMTime() > this->SceneModifiedTime)
      {
      this->SceneModifiedTime = this->Nodes->GetMTime();
      }
    return this->SceneModifiedTime;
    };

  void IncrementSceneModifiedTime()
    {
    this->SceneModifiedTime ++;
    };

  void Edited()
    {
    this->InvokeEvent( vtkMRMLScene::SceneEditedEvent );
    }


  vtkGetObjectMacro ( CacheManager, vtkCacheManager );
  virtual void SetCacheManager(vtkCacheManager* );
  vtkGetObjectMacro ( DataIOManager, vtkDataIOManager );
  virtual void SetDataIOManager(vtkDataIOManager* );
  vtkGetObjectMacro ( URIHandlerCollection, vtkCollection );
  vtkSetObjectMacro ( URIHandlerCollection, vtkCollection );
  vtkGetObjectMacro ( UserTagTable, vtkTagTable);
  virtual void SetUserTagTable(vtkTagTable* );

  /// find a URI handler in the collection that can work on the passed URI
  /// returns NULL on failure
  vtkURIHandler *FindURIHandler(const char *URI);
  /// Returns a URIhandler of a specific type if its name is known.
  vtkURIHandler *FindURIHandlerByName ( const char *name );
  /// Add a uri handler to the collection.
  void AddURIHandler(vtkURIHandler *handler);

  /// The state of the scene reflects what the scene is doing.
  /// The scene is in BatchProcessState state when succession of node
  /// insertion/removal is happening at once.
  /// Observers can ignore the events NodeAddedEvent or
  /// NodeRemovedEvent when the scene is in BatchProcessState state to
  /// just synchronize with the scene when EndBatchProcessEvent is fired.
  ///
  /// The call <code>scene->Connect("myScene.mrml");</code> that closes and
  /// import a scene will fire the events:
  /// vtkMRMLScene::StartBatchProcessEvent,
  /// vtkMRMLScene::StartCloseEvent,
  ///
  /// vtkMRMLScene::NodeAboutToBeRemovedEvent,
  /// vtkMRMLScene::NodeRemovedEvent,
  /// vtkMRMLScene::ProgressCloseEvent,
  /// vtkMRMLScene::ProgressBatchProcessEvent,
  /// ...
  /// vtkMRMLScene::EndCloseEvent,
  /// vtkMRMLScene::StartImportEvent,
  /// vtkMRMLScene::NodeAboutToBeAddedEvent,
  /// vtkMRMLScene::NodeAddedEvent,
  /// vtkMRMLScene::ProgressImportEvent,
  /// vtkMRMLScene::ProgressBatchProcessEvent,
  /// ...
  /// vtkMRMLScene::EndImportEvent,
  /// vtkMRMLScene::EndBatchProcessEvent
  enum StateType
    {
    BatchProcessState = 0x0001,
    CloseState = 0x0002 | BatchProcessState,
    ImportState = 0x0004 | BatchProcessState,
    RestoreState = 0x0008 | BatchProcessState,
    SaveState = 0x0010
    };

  /// Return the current state of the scene.
  /// It is a combination of all current states.
  /// Return 0 if the scene is in no state.
  int GetStates()const;

  /// Return the current top most state
  StateType GetCurrentState()const;

  /// Return true if the scene is in BatchProcess state, false otherwise
  inline bool GetIsBatchProcessing()const;

  /// Push a new state on the stack of current states.
  /// A matching EndState() must be called later.
  /// StartState() fires the state start event,
  /// e.g. StartImportEvent if state is ImportState.
  /// If the state is BatchProcessState, CloseState, ImportState or
  /// RestoreState and if the scene is not already in a BatchProcessState
  /// state, it also fires the event StartBatchProcessEvent.
  void StartState(const StateType& state, int anticipatedMaxProgress = 0);

  /// Pop the current state from the stack.
  /// EndState() fires the state start event,
  /// e.g. EndImportEvent if state is ImportState.
  void EndState(const StateType& state);

  /// Report progress of the current state.
  void ProgressState(const StateType& state, int progress = 0);

  enum SceneEventType
    {
    NodeAboutToBeAddedEvent = 0x2000,
    NodeAddedEvent,
    NodeAboutToBeRemovedEvent,
    NodeRemovedEvent,

    NewSceneEvent = 66030,
    SceneEditedEvent,
    MetadataAddedEvent,
    ImportProgressFeedbackEvent,
    SaveProgressFeedbackEvent,

    // Internal: not to be used directly
    StateEvent = 0x2000, // 1024 (decimal)
    StartEvent = 0x0100,
    EndEvent = 0x0200,
    ProgressEvent = 0x0400,
    // End internal

    StartBatchProcessEvent = StateEvent | StartEvent | BatchProcessState,
    EndBatchProcessEvent = StateEvent | EndEvent | BatchProcessState,
    ProgressBatchProcessEvent = StateEvent | ProgressEvent | BatchProcessState,

    StartCloseEvent = StateEvent | StartEvent | CloseState,
    EndCloseEvent = StateEvent | EndEvent | CloseState,
    ProgressCloseEvent = StateEvent | ProgressEvent | CloseState,

    StartImportEvent = StateEvent | StartEvent | ImportState,
    EndImportEvent = StateEvent | EndEvent | ImportState,
    ProgressImportEvent = StateEvent | EndEvent | ImportState,

    StartRestoreEvent = StateEvent | StartEvent | RestoreState,
    EndRestoreEvent = StateEvent | EndEvent | RestoreState,
    ProgressRestoreEvent = StateEvent | ProgressEvent | RestoreState,

    StartSaveEvent = StateEvent | StartEvent | SaveState,
    EndSaveEvent = StateEvent | EndEvent | SaveState,
    ProgressSaveEvent = StateEvent | ProgressEvent | SaveState,
    };

  /// the version of the last loaded scene file
  vtkGetStringMacro(LastLoadedVersion);
  vtkSetStringMacro(LastLoadedVersion);

  /// the current software version
  vtkGetStringMacro(Version);
  vtkSetStringMacro(Version);


  void CopyRegisteredNodesToScene(vtkMRMLScene *scene);

protected:

  vtkMRMLScene();
  virtual ~vtkMRMLScene();

  void PushIntoUndoStack();
  void PushIntoRedoStack();

  void CopyNodeInUndoStack(vtkMRMLNode *node);
  void CopyNodeInRedoStack(vtkMRMLNode *node);

  void AddReferencedNodes(vtkMRMLNode *node, vtkCollection *refNodes);

  /// Handle vtkMRMLScene::DeleteEvent: clear the scene
  static void SceneCallback( vtkObject *caller, unsigned long eid,
                             void *clientData, void *callData );

  vtkCollection*  Nodes;
  unsigned long   SceneModifiedTime;

  /// data i/o handling members
  vtkCacheManager *  CacheManager;
  vtkDataIOManager * DataIOManager;
  vtkCollection *    URIHandlerCollection;
  vtkTagTable *      UserTagTable;

  std::vector<StateType> States;

  int  UndoStackSize;
  bool UndoFlag;
  bool InUndo;

  std::list< vtkCollection* >  UndoStack;
  std::list< vtkCollection* >  RedoStack;


  std::string                 URL;
  std::string                 RootDirectory;

  std::map< std::string, int> UniqueIDByClass;
  std::vector< std::string >  UniqueIDs;
  std::vector< vtkMRMLNode* > RegisteredNodeClasses;
  std::vector< std::string >  RegisteredNodeTags;

  std::vector< std::string >          ReferencedIDs;
  std::vector< vtkMRMLNode* >         ReferencingNodes;
  std::map< std::string, std::string> ReferencedIDChanges;
  std::map<std::string, vtkMRMLNode*> NodeIDs;
  std::map<std::string, int> ReservedIDs;

  std::string ErrorMessage;

  std::string SceneXMLString;

  int LoadFromXMLString;

  int SaveToXMLString;

  int ReadDataOnLoad;

  void UpdateNodeIDs();

  unsigned long NodeIDsMTime;

  void RemoveAllNodesExceptSingletons();

  vtkSetStringMacro(ClassNameList);
  vtkGetStringMacro(ClassNameList);

  char * Version;
  char * LastLoadedVersion;

  vtkCallbackCommand *DeleteEventCallback;

private:

  vtkMRMLScene(const vtkMRMLScene&);   // Not implemented
  void operator=(const vtkMRMLScene&); // Not implemented

  /// Hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->Nodes->vtkCollection::AddItem(o); this->Modified();};
  void RemoveItem(vtkObject *o) { this->Nodes->vtkCollection::RemoveItem(o); this->Modified();};
  void RemoveItem(int i) { this->Nodes->vtkCollection::RemoveItem(i); this->Modified();};
  int  IsItemPresent(vtkObject *o) { return this->Nodes->vtkCollection::IsItemPresent(o);};

  int LoadIntoScene(vtkCollection* scene);

  unsigned long ErrorCode;

  char* ClassNameList;

  static vtkMRMLScene *ActiveScene;
};

//------------------------------------------------------------------------------
bool vtkMRMLScene::GetIsBatchProcessing()const
{
  return (this->GetStates() & vtkMRMLScene::BatchProcessState) != 0;
}

#endif
