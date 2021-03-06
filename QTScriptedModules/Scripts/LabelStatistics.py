from __main__ import vtk, qt, ctk, slicer

#
# LabelStatistics
#

class LabelStatistics:
  def __init__(self, parent):
    parent.title = "Label Statistics"
    parent.category = "Quantification"
    parent.contributor = "Steve Pieper"
    parent.helpText = """
Use this module to calculate counts and volumes for different labels of a label map plus statistics on the grayscale background volume.  Note: volumes must have same dimensions.
    """
    parent.acknowledgementText = """
    Supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See http://www.slicer.org for details.  Module implemented by Steve Pieper.
    """
    self.parent = parent

#
# qSlicerPythonModuleExampleWidget
#

class LabelStatisticsWidget:
  def __init__(self, parent=None):
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
    self.logic = None
    self.grayscaleNode = None
    self.labelNode = None
    self.fileName = None
    self.fileDialog = None
    if not parent:
      self.setup()
      self.grayscaleSelector.setMRMLScene(slicer.mrmlScene)
      self.labelSelector.setMRMLScene(slicer.mrmlScene)
      self.parent.show()
    
  def setup(self):
    #
    # the grayscale volume selector
    #
    self.grayscaleSelectorFrame = qt.QFrame(self.parent)
    self.grayscaleSelectorFrame.setLayout(qt.QHBoxLayout())
    self.parent.layout().addWidget(self.grayscaleSelectorFrame)

    self.grayscaleSelectorLabel = qt.QLabel("Grayscale Volume: ", self.grayscaleSelectorFrame)
    self.grayscaleSelectorLabel.setToolTip( "Select the grayscale volume (background grayscale scalar volume node) for statistics calculations")
    self.grayscaleSelectorFrame.layout().addWidget(self.grayscaleSelectorLabel)

    self.grayscaleSelector = slicer.qMRMLNodeComboBox(self.grayscaleSelectorFrame)
    self.grayscaleSelector.nodeTypes = ( ("vtkMRMLScalarVolumeNode"), "" )
    self.grayscaleSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", 0 )
    self.grayscaleSelector.selectNodeUponCreation = False
    self.grayscaleSelector.addEnabled = False
    self.grayscaleSelector.removeEnabled = False
    self.grayscaleSelector.noneEnabled = True
    self.grayscaleSelector.showHidden = False
    self.grayscaleSelector.showChildNodeTypes = False
    self.grayscaleSelector.setMRMLScene( slicer.mrmlScene )
    # TODO: need to add a QLabel
    # self.grayscaleSelector.SetLabelText( "Master Volume:" )
    self.grayscaleSelectorFrame.layout().addWidget(self.grayscaleSelector)
    
    #
    # the label volume selector
    #
    self.labelSelectorFrame = qt.QFrame()
    self.labelSelectorFrame.setLayout( qt.QHBoxLayout() )
    self.parent.layout().addWidget( self.labelSelectorFrame )

    self.labelSelectorLabel = qt.QLabel()
    self.labelSelectorLabel.setText( "Label Map: " )
    self.labelSelectorFrame.layout().addWidget( self.labelSelectorLabel )

    self.labelSelector = slicer.qMRMLNodeComboBox()
    self.labelSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.labelSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", "1" )
    # todo addAttribute
    self.labelSelector.selectNodeUponCreation = False
    self.labelSelector.addEnabled = False
    self.labelSelector.noneEnabled = True
    self.labelSelector.removeEnabled = False
    self.labelSelector.showHidden = False
    self.labelSelector.showChildNodeTypes = False
    self.labelSelector.setMRMLScene( slicer.mrmlScene )
    self.labelSelector.setToolTip( "Pick the label map to edit" )
    self.labelSelectorFrame.layout().addWidget( self.labelSelector )
    
    # Apply button
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Calculate Statistics."
    self.applyButton.enabled = False
    self.parent.layout().addWidget(self.applyButton)

    # model and view for stats table
    self.view = qt.QTableView()
    self.view.sortingEnabled = True
    self.parent.layout().addWidget(self.view)

    # Save button
    self.saveButton = qt.QPushButton("Save")
    self.saveButton.toolTip = "Calculate Statistics."
    self.saveButton.enabled = False
    self.parent.layout().addWidget(self.saveButton)

    # Add vertical spacer
    self.parent.layout().addStretch(1)

    # connections
    self.applyButton.connect('clicked()', self.onApply)
    self.saveButton.connect('clicked()', self.onSave)
    self.grayscaleSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onGrayscaleSelect)
    self.labelSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onLabelSelect)
    
  def onGrayscaleSelect(self, node):
    self.grayscaleNode = node
    self.applyButton.enabled = bool(self.grayscaleNode) and bool(self.labelNode)

  def onLabelSelect(self, node):
    self.labelNode = node
    self.applyButton.enabled = bool(self.grayscaleNode) and bool(self.labelNode)

  def onApply(self):
    """Calculate the label statistics
    """
    self.applyButton.text = "Working..."
    # TODO: why doesn't processEvents make the label text change?
    slicer.app.processEvents()
    self.logic = LabelStatisticsLogic(self.grayscaleNode, self.labelNode)
    self.populateStats()
    self.saveButton.enabled = True
    self.applyButton.text = "Apply"

  def onSave(self):
    """Calculate the label statistics
    """
    if not self.fileDialog:
      self.fileDialog = qt.QFileDialog(self.parent)
      self.fileDialog.acceptMode = 1 # save dialog
      self.fileDialog.defaultSuffix = "csv"
      self.fileDialog.setNameFilter("Comma Separated Values (*.csv)")
      self.fileDialog.connect("fileSelected(QString)", self.onFileSelected)
    self.fileDialog.show()

  def onFileSelected(self,fileName):
    self.logic.saveStats(fileName)

  def populateStats(self):
    if not self.logic:
      return
    self.items = []
    self.model = qt.QStandardItemModel()
    self.view.setModel(self.model)
    row = 0
    for i in self.logic.labelStats["Labels"]:
      col = 0
      for k in self.logic.keys:
        item = qt.QStandardItem()
        item.setText(str(self.logic.labelStats[i,k]))
        self.model.setItem(row,col,item)
        self.items.append(item)
        col += 1
      row += 1

    col = 0
    for k in self.logic.keys:
      self.view.setColumnWidth(col,15*len(k))
      self.model.setHeaderData(col,1,k)
      col += 1

class LabelStatisticsLogic:
  """Implement the logic to calculate label statistics.
  Nodes are passed in as arguments.
  Results are stored as 'statistics' instance variable.
  """
  
  def __init__(self, grayscaleNode, labelNode, fileName=None):
    import numpy

    self.keys = ("Index", "Count", "Volume", "Min", "Max", "Mean", "StdDev")
    cubicMMPerVoxel = reduce(lambda x,y: x*y, labelNode.GetSpacing())
    
    # TODO: progress and status updates
    # this->InvokeEvent(vtkLabelStatisticsLogic::StartLabelStats, (void*)"start label stats")
    
    self.labelStats = {}
    self.labelStats['Labels'] = []
   
    stataccum = vtk.vtkImageAccumulate()
    stataccum.SetInput(labelNode.GetImageData())
    stataccum.Update()
    lo = int(stataccum.GetMin()[0])
    hi = int(stataccum.GetMax()[0])

    for i in xrange(lo,hi+1):

      # this->SetProgress((float)i/hi);
      # std::string event_message = "Label "; std::stringstream s; s << i; event_message.append(s.str());
      # this->InvokeEvent(vtkLabelStatisticsLogic::LabelStatsOuterLoop, (void*)event_message.c_str());

      # logic copied from slicer3 LabelStatistics
      # to create the binary volume of the label
      # //logic copied from slicer2 LabelStatistics MaskStat
      # // create the binary volume of the label
      thresholder = vtk.vtkImageThreshold()
      thresholder.SetInput(labelNode.GetImageData())
      thresholder.SetInValue(1)
      thresholder.SetOutValue(0)
      thresholder.ReplaceOutOn()
      thresholder.ThresholdBetween(i,i)
      thresholder.SetOutputScalarType(grayscaleNode.GetImageData().GetScalarType())
      thresholder.Update()
      
      # this.InvokeEvent(vtkLabelStatisticsLogic::LabelStatsInnerLoop, (void*)"0.25");
      
      #  use vtk's statistics class with the binary labelmap as a stencil
      stencil = vtk.vtkImageToImageStencil()
      stencil.SetInput(thresholder.GetOutput())
      stencil.ThresholdBetween(1, 1)
      
      # this.InvokeEvent(vtkLabelStatisticsLogic::LabelStatsInnerLoop, (void*)"0.5")
      
      stat1 = vtk.vtkImageAccumulate()
      stat1.SetInput(grayscaleNode.GetImageData())
      stat1.SetStencil(stencil.GetOutput())
      stat1.Update()

      # this.InvokeEvent(vtkLabelStatisticsLogic::LabelStatsInnerLoop, (void*)"0.75")

      if stat1.GetVoxelCount() > 0:
        # add an entry to the LabelStats list
        self.labelStats["Labels"].append(i)
        self.labelStats[i,"Index"] = i
        self.labelStats[i,"Count"] = stat1.GetVoxelCount()
        self.labelStats[i,"Volume"] = self.labelStats[i,"Count"] * cubicMMPerVoxel
        self.labelStats[i,"Min"] = stat1.GetMin()[0]
        self.labelStats[i,"Max"] = stat1.GetMax()[0]
        self.labelStats[i,"Mean"] = stat1.GetMean()[0]
        self.labelStats[i,"StdDev"] = stat1.GetStandardDeviation()[0]
        
        # this.InvokeEvent(vtkLabelStatisticsLogic::LabelStatsInnerLoop, (void*)"1")

    # this.InvokeEvent(vtkLabelStatisticsLogic::EndLabelStats, (void*)"end label stats")

  def statsAsCSV(self):
    csv = ""
    header = ""
    for k in self.keys[:-1]:
      header += k + ","
    header += self.keys[-1] + "\n"
    csv = header
    for i in self.labelStats["Labels"]:
      line = ""
      for k in self.keys[:-1]:
        line += str(self.labelStats[i,k]) + ","
      line += str(self.labelStats[i,self.keys[-1]]) + "\n"
      csv += line
    return csv

  def saveStats(self,fileName):
    fp = open(fileName, "w")
    fp.write(self.statsAsCSV())
    fp.close()

      

class Slicelet(object):
  """A slicer slicelet is a module widget that comes up in stand alone mode
  implemented as a python class.
  This class provides common wrapper functionality used by all slicer modlets.
  """
  # TODO: put this in a SliceletLib 
  # TODO: parse command line arge


  def __init__(self, widgetClass=None):
    self.parent = qt.QFrame()
    self.parent.setLayout( qt.QVBoxLayout() )

    # TODO: should have way to pop up python interactor
    self.buttons = qt.QFrame()
    self.buttons.setLayout( qt.QHBoxLayout() )
    self.parent.layout().addWidget(self.buttons)
    self.addDataButton = qt.QPushButton("Add Data")
    self.buttons.layout().addWidget(self.addDataButton)
    self.addDataButton.connect("clicked()",slicer.app.ioManager().openAddDataDialog)
    self.loadSceneButton = qt.QPushButton("Load Scene")
    self.buttons.layout().addWidget(self.loadSceneButton)
    self.loadSceneButton.connect("clicked()",slicer.app.ioManager().openLoadSceneDialog)

    if widgetClass:
      self.widget = widgetClass(self.parent)
      self.widget.setup()
    self.parent.show()

class LabelStatisticsSlicelet(Slicelet):
  """ Creates the interface when module is run as a stand alone gui app.
  """

  def __init__(self):
    super(LabelStatisticsSlicelet,self).__init__(LabelStatisticsWidget)


if __name__ == "__main__":
  # TODO: need a way to access and parse command line arguments
  # TODO: ideally command line args should handle --xml

  import sys
  print( sys.argv )

  slicelet = LabelStatisticsSlicelet()
