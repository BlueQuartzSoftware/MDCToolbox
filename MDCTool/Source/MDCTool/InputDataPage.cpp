/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "InputDataPage.h"

#include <QtCore/QFile>

#include "MDCTool.h"
#include "MDCConfiguration.h"
#include "PreviewTableModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputDataPage::InputDataPage(MDCConfiguration* config, QWidget* parent) :
  QWizardPage(parent),
  m_Configuration(config),
  m_CondensedConfigText("")
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputDataPage::~InputDataPage()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputDataPage::setupGui()
{
  PreviewTableModel* model = PreviewTableModel::Instance();
  tableView2->setModel(model);
  tableView2->horizontalHeader()->setSectionResizeMode(PreviewTableModel::Conditions, QHeaderView::ResizeToContents);
  tableView2->horizontalHeader()->setSectionResizeMode(PreviewTableModel::RawImagePath, QHeaderView::Stretch);
  tableView2->horizontalHeader()->setSectionResizeMode(PreviewTableModel::D3DOutputPath, QHeaderView::Stretch);

  connect(importFilesWidget, SIGNAL(fileListChanged()), this, SIGNAL(completeChanged()));

  m_CondensedConfigText = MDCTool::TokenizeConfiguration(m_Configuration);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputDataPage::showEvent(QShowEvent* event)
{
  PreviewTableModel* model = PreviewTableModel::Instance();
  QString condensedText = getCondensedConfigText();
  QModelIndexList indexList = model->match(model->index(0, 0), Qt::DisplayRole, condensedText, -1, Qt::MatchExactly);

  QList<QPersistentModelIndex> pList;
  for (int i = 0; i < indexList.size(); i++)
  {
    pList.push_back(QPersistentModelIndex(indexList[i]));
  }

  for (int i = 0; i < pList.size(); i++)
  {
    model->removeRow(pList[i].row());
  }

  wizard()->setWindowTitle("MDCTool - Step " + QString::number(wizard()->currentId() + 1) + " of " + QString::number(wizard()->pageIds().size()));

  double value = static_cast<double>(wizard()->currentId()) / static_cast<double>(wizard()->pageIds().size());
  int progress = static_cast<int>(value * 100);
  progressBar->setValue(progress);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InputDataPage::isComplete() const
{
  if (importFilesWidget->getFileList().size() > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InputDataPage::validatePage()
{
  PreviewTableModel* model = PreviewTableModel::Instance();

  QVector<FilePathGenerator::FilePackage> packageList = importFilesWidget->getFileList();
  for (int i = 0; i < packageList.size(); i++)
  {
    FilePathGenerator::FilePackage package = packageList.at(i);
    QString filePath = package.completeFilePath;

    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, PreviewTableModel::Conditions), m_CondensedConfigText, Qt::DisplayRole);
    model->setData(model->index(row, PreviewTableModel::RawImagePath), filePath, Qt::DisplayRole);


  }

  // Return true, because we are only using this reimplemented function to do tasks between pages.
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InputDataPage::getCondensedConfigText()
{
  return m_CondensedConfigText;
}
