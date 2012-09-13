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

/// Qt includes
#include <QFileInfo>

// CTK includes
#include <ctkFlowLayout.h>
#include <ctkUtils.h>

/// Volumes includes
#include "qSlicerIOOptions_p.h"
#include "qSlicerVolumesIOOptionsWidget.h"
#include "ui_qSlicerVolumesIOOptionsWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Volumes
class qSlicerVolumesIOOptionsWidgetPrivate
  : public qSlicerIOOptionsPrivate
  , public Ui_qSlicerVolumesIOOptionsWidget
{
public:
};

//-----------------------------------------------------------------------------
qSlicerVolumesIOOptionsWidget::qSlicerVolumesIOOptionsWidget(QWidget* parentWidget)
  : qSlicerIOOptionsWidget(new qSlicerVolumesIOOptionsWidgetPrivate, parentWidget)
{
  Q_D(qSlicerVolumesIOOptionsWidget);
  d->setupUi(this);

  ctkFlowLayout::replaceLayout(this);
  /*
  // Replace the horizontal layout with a flow layout
  ctkFlowLayout* flowLayout = new ctkFlowLayout;
  flowLayout->setPreferredExpandingDirections(Qt::Horizontal);
  flowLayout->setAlignItems(false);
  QLayout* oldLayout = this->layout();
  int margins[4];
  oldLayout->getContentsMargins(&margins[0],&margins[1],&margins[2],&margins[3]);
  QLayoutItem* item = 0;
  while((item = oldLayout->takeAt(0)))
    {
    if (item->widget())
      {
      flowLayout->addWidget(item->widget());
      }
    }
  // setLayout() will take care or reparenting layouts and widgets
  delete oldLayout;
  flowLayout->setContentsMargins(0,0,0,0);
  this->setLayout(flowLayout);
  */

  connect(d->NameLineEdit, SIGNAL(textChanged(QString)),
          this, SLOT(updateProperties()));
  connect(d->LabelMapCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(updateProperties()));
  connect(d->CenteredCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(updateProperties()));
  connect(d->SingleFileCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(updateProperties()));
  connect(d->OrientationCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(updateProperties()));

  // Single file by default
  d->SingleFileCheckBox->setChecked(true);
}

//-----------------------------------------------------------------------------
qSlicerVolumesIOOptionsWidget::~qSlicerVolumesIOOptionsWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerVolumesIOOptionsWidget::updateProperties()
{
  Q_D(qSlicerVolumesIOOptionsWidget);
  if (!d->NameLineEdit->text().isEmpty())
    {
    QStringList names = d->NameLineEdit->text().split(';');
    for (int i = 0; i < names.count(); ++i)
      {
      names[i] = names[i].trimmed();
      }
    d->Properties["name"] = names;
    }
  else
    {
    d->Properties.remove("name");
    }
  d->Properties["labelmap"] = d->LabelMapCheckBox->isChecked();
  d->Properties["center"] = d->CenteredCheckBox->isChecked();
  d->Properties["singleFile"] = d->SingleFileCheckBox->isChecked();
  d->Properties["discardOrientation"] = d->OrientationCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerVolumesIOOptionsWidget::setFileName(const QString& fileName)
{
  this->setFileNames(QStringList(fileName));
}

//-----------------------------------------------------------------------------
void qSlicerVolumesIOOptionsWidget::setFileNames(const QStringList& fileNames)
{
  Q_D(qSlicerVolumesIOOptionsWidget);
  QStringList names;
  bool onlyNumberInName = false;
  bool onlyNumberInExtension = false;
  bool hasLabelMapName = false;
  foreach(const QString& fileName, fileNames)
    {
    QFileInfo fileInfo(fileName);
    if (fileInfo.isFile())
      {
      names << fileInfo.completeBaseName();
      // Single file
      // If the name (or the extension) is just a number, then it must be a 2D
      // slice from a 3D volume, so uncheck Single File.
      onlyNumberInName = QRegExp("[0-9\\.\\-\\_\\@\\(\\)\\~]+").exactMatch(fileInfo.baseName());
      fileInfo.suffix().toInt(&onlyNumberInExtension);
      }
    // Because '_' is considered as a word character (\w), \b
    // doesn't consider '_' as a word boundary.
    QRegExp labelMapName("(\\b|_)([Ll]abel(s)?)(\\b|_)");
    QRegExp segName("(\\b|_)([Ss]eg)(\\b|_)");
    if (fileInfo.baseName().contains(labelMapName) ||
        fileInfo.baseName().contains(segName))
      {
      hasLabelMapName = true;
      }
    }
  d->NameLineEdit->setText( names.join("; ") );
  d->SingleFileCheckBox->setChecked(!onlyNumberInName && !onlyNumberInExtension);
  d->LabelMapCheckBox->setChecked(hasLabelMapName);
  this->qSlicerIOOptionsWidget::setFileNames(fileNames);
}
