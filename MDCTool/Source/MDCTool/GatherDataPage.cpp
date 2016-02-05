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

#include "GatherDataPage.h"

#include <QtCore/QFile>

#include <iostream>

#include "MDCTool.h"
#include "InputDataPage.h"
#include "D3DInfoPage.h"
#include "MDCConfiguration.h"
#include "PreviewTableModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GatherDataPage::GatherDataPage(MDCConfiguration* config, QWidget* parent) :
  QWizardPage(parent),
  m_Configuration(config)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GatherDataPage::~GatherDataPage()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GatherDataPage::setupGui()
{
  PreviewTableModel* model = PreviewTableModel::Instance();
  tableView1->setModel(model);
  tableView1->horizontalHeader()->setSectionResizeMode(PreviewTableModel::Conditions, QHeaderView::ResizeToContents);
  tableView1->horizontalHeader()->setSectionResizeMode(PreviewTableModel::RawImagePath, QHeaderView::Stretch);
  tableView1->horizontalHeader()->setSectionResizeMode(PreviewTableModel::D3DOutputPath, QHeaderView::Stretch);

  QString nextText = nextLabel->text();
#if defined (Q_OS_MAC)
  nextText.replace("@Next_Word@", "Continue");
#else
  nextText.replace("@Next_Word@", "Next");
#endif
  nextLabel->setText(nextText);

  QString conditionText = "<b>Please set the microscope to these conditions:</b><br><br>";

  QVector<QSharedPointer<MDCCondition> > conditions = m_Configuration->getConditions();
  for (int i = 0; i < conditions.size(); i++)
  {
    MDCCondition* condition = conditions[i].data();
    conditionText.append(condition->conditionName + " - " + QString::number(condition->conditionValue));

    if (i < conditions.size() - 1)
    {
      conditionText.append("<br>");
    }
  }

  conditionLabel->setText(conditionText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GatherDataPage::showEvent(QShowEvent* event)
{
  wizard()->setWindowTitle("MDCTool - Step " + QString::number(wizard()->currentId() + 1) + " of " + QString::number(wizard()->pageIds().size()));

  double value = static_cast<double>(wizard()->currentId()) / static_cast<double>(wizard()->pageIds().size());
  int progress = static_cast<int>(value * 100);
  progressBar->setValue(progress);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GatherDataPage::cleanupPage()
{
  QWizardPage::cleanupPage();
}
