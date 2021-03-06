#include "ctkUtils.h"

// MRML includes
#include "vtkMRMLFiberBundleNode.h"
#include "vtkMRMLDiffusionTensorVolumeNode.h"

// Tractography Logic includes
#include "vtkSlicerTractographyFiducialSeedingLogic.h"
#include "vtkMRMLTractographyFiducialSeedingNode.h"

// Tractography QTModule includes
#include "qSlicerTractographyFiducialSeedingModuleWidget.h"
#include "ui_qSlicerTractographyFiducialSeedingModule.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_TractographyFiducialSeeding
class qSlicerTractographyFiducialSeedingModuleWidgetPrivate:
  public Ui_qSlicerTractographyFiducialSeedingModule
{
};

//-----------------------------------------------------------------------------
qSlicerTractographyFiducialSeedingModuleWidget::qSlicerTractographyFiducialSeedingModuleWidget(QWidget *_parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerTractographyFiducialSeedingModuleWidgetPrivate)
{
  this->TractographyFiducialSeedingNode = 0;
}
//-----------------------------------------------------------------------------
qSlicerTractographyFiducialSeedingModuleWidget::~qSlicerTractographyFiducialSeedingModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  this->Superclass::setMRMLScene(scene);

  // find parameters node or create it if there is no one in the scene
  if (scene && this->TractographyFiducialSeedingNode == 0)
  {
    vtkMRMLTractographyFiducialSeedingNode *tnode = 0;
    vtkMRMLNode *node = scene->GetNthNodeByClass(0, "vtkMRMLTractographyFiducialSeedingNode");
    if (node == 0)
    {
      tnode = vtkMRMLTractographyFiducialSeedingNode::New();
      scene->AddNode(tnode);
      tnode->Delete();
    }
    else {
      tnode = vtkMRMLTractographyFiducialSeedingNode::SafeDownCast(node);
    }
    this->setTractographyFiducialSeedingNode(tnode);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setup()
{
  Q_D(qSlicerTractographyFiducialSeedingModuleWidget);
  d->setupUi(this);


  QObject::connect(d->ParameterNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                             SLOT(setTractographyFiducialSeedingNode(vtkMRMLNode*)));

  QObject::connect(d->DTINodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                       SLOT(setDiffusionTensorVolumeNode(vtkMRMLNode*)));

  QObject::connect(d->FiducialNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                            SLOT(setSeedingNode(vtkMRMLNode*)));

  QObject::connect(d->FiberNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                         SLOT(setFiberBundleNode(vtkMRMLNode*)));

  //QObject::connect(d->StoppingCurvatureSpinBoxLabel,
  //                 SIGNAL(sliderMoved(double)),
  //                 SLOT(onParameterChanged(double)));

  QObject::connect(d->StoppingCurvatureSpinBoxLabel, 
                SIGNAL(valueChanged(double)),
                SLOT(setStoppingCurvature(double)));

  QObject::connect(d->StoppingCriteriaComboBox, 
                SIGNAL(currentIndexChanged(int)),
                SLOT(setStoppingCriteria(int)));

  QObject::connect(d->StoppingValueSpinBoxLabel, 
                SIGNAL(valueChanged(double)),
                SLOT(setStoppingValue(double)));

  QObject::connect(d->IntegrationStepSpinBoxLabel, 
                SIGNAL(valueChanged(double)),
                SLOT(setIntegrationStep(double)));

  QObject::connect(d->MinimumPathSpinBoxLabel, 
                SIGNAL(valueChanged(double)),
                SLOT(setMinimumPath(double)));

  QObject::connect(d->FiducialRegionSpinBoxLabel, 
                SIGNAL(valueChanged(double)),
                SLOT(setFiducialRegion(double)));

  QObject::connect(d->FiducialStepSpinBoxLabel, 
                SIGNAL(valueChanged(double)),
                SLOT(setFiducialRegionStep(double)));

  QObject::connect(d->DisplayTracksComboBox, 
                SIGNAL(currentIndexChanged(int)),
                SLOT(setTrackDisplayMode(int)));

  QObject::connect(d->SeedSelectedCheckBox, 
                SIGNAL(stateChanged(int)),
                SLOT(setSeedSelectedFiducials(int)));

  QObject::connect(d->EnableSeedingCheckBox, 
                SIGNAL(stateChanged(int)),
                SLOT(setEnableSeeding(int)));

  QObject::connect(d->MaxNumberSeedsNumericInput, 
                SIGNAL(valueChanged(int)),
                SLOT(setMaxNumberSeeds(int)));
}

//-----------------------------------------------------------------------------
vtkMRMLNode* qSlicerTractographyFiducialSeedingModuleWidget::seedingNode() 
{
  vtkMRMLNode *node = 0;
  if (this->TractographyFiducialSeedingNode)
  {
    node = this->mrmlScene()->GetNodeByID(
                        this->TractographyFiducialSeedingNode->GetInputFiducialRef());
  }
  return node;
}

//-----------------------------------------------------------------------------
vtkMRMLDiffusionTensorVolumeNode* qSlicerTractographyFiducialSeedingModuleWidget::diffusionTensorVolumeNode() 
{
  vtkMRMLDiffusionTensorVolumeNode *dtiNode = 0;
  if (this->TractographyFiducialSeedingNode)
  {
    dtiNode = vtkMRMLDiffusionTensorVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(
              this->TractographyFiducialSeedingNode->GetInputVolumeRef()));
  }
  return dtiNode;
}

//-----------------------------------------------------------------------------
vtkMRMLFiberBundleNode* qSlicerTractographyFiducialSeedingModuleWidget::fiberBundleNode() 
{
  vtkMRMLFiberBundleNode *fiberNode = 0;
  if (this->TractographyFiducialSeedingNode)
  {
    fiberNode = vtkMRMLFiberBundleNode::SafeDownCast(this->mrmlScene()->GetNodeByID(
                this->TractographyFiducialSeedingNode->GetOutputFiberRef()));
  }
  return fiberNode;
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setTractographyFiducialSeedingNode(vtkMRMLNode *node)
{
  vtkMRMLTractographyFiducialSeedingNode *paramNode = vtkMRMLTractographyFiducialSeedingNode::SafeDownCast(node);

  // each time the node is modified, the logic creates tracks
  vtkSlicerTractographyFiducialSeedingLogic *seedingLogic = 
        vtkSlicerTractographyFiducialSeedingLogic::SafeDownCast(this->logic());
  if (seedingLogic && this->mrmlScene())
  {
    seedingLogic->SetAndObserveTractographyFiducialSeedingNode(paramNode);
  }

  // each time the node is modified, the qt widgets are updated
  this->qvtkReconnect(this->TractographyFiducialSeedingNode, paramNode, 
                       vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()));

  this->TractographyFiducialSeedingNode = paramNode;
  this->updateWidgetFromMRML();
}


//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setSeedingNode(vtkMRMLNode *node)
{

  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetInputFiducialRef(node ?
                                                               node->GetID() : "" );
    vtkSlicerTractographyFiducialSeedingLogic *seedingLogic = 
          vtkSlicerTractographyFiducialSeedingLogic::SafeDownCast(this->logic());
    if (seedingLogic && this->mrmlScene())
    {
      seedingLogic->SetAndObserveTractographyFiducialSeedingNode(this->TractographyFiducialSeedingNode);
    }

  }
}

static double round_num(double num)
{
  double result = num;

  if (num < 1.0)
  {
    std::stringstream ss;
    ss << num;
    std::string s = ss.str();
    std::stringstream res;
    for (unsigned int i=0; i<s.size(); i++)
      {
      if (s.at(i) != '0' && s.at(i) != '.')
        {
        res << s.at(i);
        break;
        }
        res << s.at(i);
      }
    res >> result;
  }
  else
  {
    // drop off everything past the decimal point
    result = floor(result);
  }

  return result;
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setDiffusionTensorVolumeNode(vtkMRMLNode *node)
{
  Q_D(qSlicerTractographyFiducialSeedingModuleWidget);

  vtkMRMLDiffusionTensorVolumeNode *diffusionTensorVolumeNode = vtkMRMLDiffusionTensorVolumeNode::SafeDownCast(node);

  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetInputVolumeRef(diffusionTensorVolumeNode ?
                                                             diffusionTensorVolumeNode->GetID() : "" );
    vtkSlicerTractographyFiducialSeedingLogic *seedingLogic = 
          vtkSlicerTractographyFiducialSeedingLogic::SafeDownCast(this->logic());
    if (seedingLogic && this->mrmlScene())
    {
      seedingLogic->SetAndObserveTractographyFiducialSeedingNode(this->TractographyFiducialSeedingNode);
    }
  }

  if (diffusionTensorVolumeNode && diffusionTensorVolumeNode->GetImageData())
    {
    double spacing[3];
    diffusionTensorVolumeNode->GetSpacing(spacing);
    double minSpacing = spacing[0];
    for (int i=1; i<3; i++)
      {
      if (spacing[i] < minSpacing)
        {
        minSpacing = spacing[i];
        }
      }
    // get 0 decimal places
    minSpacing = round_num(0.5*minSpacing);

    int decimal = ctk::orderOfMagnitude(minSpacing);
    decimal = decimal >= 0 ? 0 : -decimal;

    d->FiducialStepSpinBoxLabel->setDecimals(decimal+1);
    d->FiducialStepSpinBoxLabel->setSingleStep(minSpacing);
    d->FiducialStepSpinBoxLabel->setMinimum(minSpacing);
    d->FiducialStepSpinBoxLabel->setMaximum(10*minSpacing);

    d->FiducialRegionSpinBoxLabel->setDecimals(decimal+1);
    d->FiducialRegionSpinBoxLabel->setSingleStep(minSpacing);
    d->FiducialRegionSpinBoxLabel->setMinimum(minSpacing);
    d->FiducialRegionSpinBoxLabel->setMaximum(100*minSpacing);
    }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setFiberBundleNode(vtkMRMLNode *node)
{
  vtkMRMLFiberBundleNode *fiberBundleNode = vtkMRMLFiberBundleNode::SafeDownCast(node);
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetOutputFiberRef(fiberBundleNode ?
                                                             fiberBundleNode->GetID() : "" );
  }
}


//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerTractographyFiducialSeedingModuleWidget);
  
  vtkMRMLTractographyFiducialSeedingNode *paramNode = this->TractographyFiducialSeedingNode;

  if (paramNode && this->mrmlScene())
    {

    d->IntegrationStepSpinBoxLabel->setValue(paramNode->GetIntegrationStep());
    d->MaxNumberSeedsNumericInput->setValue(paramNode->GetMaxNumberOfSeeds());
    d->MinimumPathSpinBoxLabel->setValue(paramNode->GetMinimumPathLength());
    d->FiducialRegionSpinBoxLabel->setValue(paramNode->GetSeedingRegionSize());
    d->FiducialStepSpinBoxLabel->setValue(paramNode->GetSeedingRegionStep());
    d->SeedSelectedCheckBox->setChecked(paramNode->GetSeedSelectedFiducials()==1);
    d->StoppingCurvatureSpinBoxLabel->setValue(paramNode->GetStoppingCurvature());
    d->StoppingCriteriaComboBox->setCurrentIndex(paramNode->GetStoppingMode());
    d->StoppingValueSpinBoxLabel->setValue(paramNode->GetStoppingValue());
    d->DisplayTracksComboBox->setCurrentIndex(paramNode->GetDisplayMode());

    d->ParameterNodeSelector->setCurrentNode(
      this->mrmlScene()->GetNodeByID(paramNode->GetID()));
    d->FiberNodeSelector->setCurrentNode(
      this->mrmlScene()->GetNodeByID(paramNode->GetOutputFiberRef()));
    d->FiducialNodeSelector->setCurrentNode(
      this->mrmlScene()->GetNodeByID(paramNode->GetInputFiducialRef()));
    d->DTINodeSelector->setCurrentNode(
      this->mrmlScene()->GetNodeByID(paramNode->GetInputVolumeRef()));

    }
}


//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setStoppingCurvature(double value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetStoppingCurvature(value);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setStoppingValue(double value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetStoppingValue(value);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setIntegrationStep(double value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetIntegrationStep(value);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setMinimumPath(double value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetMinimumPathLength(value);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setFiducialRegion(double value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetSeedingRegionSize(value);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setFiducialRegionStep(double value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetSeedingRegionStep(value);
  }
}


//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setStoppingCriteria(int value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetStoppingMode(value);
  }
}
//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setTrackDisplayMode(int value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetDisplayMode(value);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setSeedSelectedFiducials(int value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetSeedSelectedFiducials(value!=0?1:0);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setMaxNumberSeeds(int value)
{
  if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetMaxNumberOfSeeds(value);
  }
}

//-----------------------------------------------------------------------------
void qSlicerTractographyFiducialSeedingModuleWidget::setEnableSeeding(int value)
{
 if (this->TractographyFiducialSeedingNode)
  {
    this->TractographyFiducialSeedingNode->SetEnableSeeding(value);
  }
}

