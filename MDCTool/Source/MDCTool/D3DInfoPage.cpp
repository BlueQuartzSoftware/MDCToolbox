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

#include "D3DInfoPage.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <QtWidgets/QMessageBox>

#include "MDCTool.h"
#include "GatherDataPage.h"
#include "InputDataPage.h"
#include "D3DProcessingPage.h"
#include "PreviewTableModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
D3DInfoPage::D3DInfoPage(QWidget* parent) :
  QWizardPage(parent),
  m_LastOpenDialogFilePath("")
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
D3DInfoPage::~D3DInfoPage()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DInfoPage::setupGui()
{
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DInfoPage::showEvent(QShowEvent* event)
{
  wizard()->setWindowTitle("MDCTool");

  /* This section of code will get hit when the user gets back to this page from the "Back" button
     It just removes any future pages, because we will be reading the configuration file again. */
  QList<int> ids = wizard()->pageIds();
  for (int i = ids.size() - 1; i > 1; i--)
  {
    int id = ids[i];

    QWizardPage* page = wizard()->page(id);
    wizard()->removePage(id);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DInfoPage::on_pipelineRunnerSelectBtn_pressed()
{
  QString filePath = MDCTool::ChooseFile(m_LastOpenDialogFilePath);
  if (filePath.isEmpty()) { return; }

  pipelineRunnerLineEdit->setText(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DInfoPage::on_pipelineSelectBtn_pressed()
{
  QString filePath = MDCTool::ChooseFile(m_LastOpenDialogFilePath);
  if (filePath.isEmpty()) { return; }

  pipelineLineEdit->setText(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DInfoPage::on_configSelectBtn_pressed()
{
  QString filePath = MDCTool::ChooseFile(m_LastOpenDialogFilePath, tr("JSON Files (*.json);;All Files (*.*)"));
  if (filePath.isEmpty()) { return; }

  configLineEdit->setText(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DInfoPage::on_pipelineRunnerLineEdit_textChanged(const QString &text)
{
  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DInfoPage::on_pipelineLineEdit_textChanged(const QString &text)
{
  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool D3DInfoPage::isComplete() const
{
  if (pipelineRunnerLineEdit->text().isEmpty() == false && pipelineLineEdit->text().isEmpty() == false
    && configLineEdit->text().isEmpty() == false)
  {
    QFileInfo prFi(pipelineRunnerLineEdit->text());
    QFileInfo pFi(pipelineLineEdit->text());
    QFileInfo cFi(configLineEdit->text());
    if (prFi.exists() && pFi.exists() && cFi.exists())
    {
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool D3DInfoPage::validatePage()
{
  MDCTool* tool = static_cast<MDCTool*>(wizard());
  if (NULL == tool) { return false; }

  QString configPath = configLineEdit->text();
  QFileInfo configFi(configPath);
  if (configFi.completeSuffix() != "json")
  {
    QString s = "The configuration file that was chosen is not a JSON file.\n\n";
    s.append("Please choose a JSON file.");
    QMessageBox::critical(this, "MDCTool Error", s, QMessageBox::Ok);
    return false;
  }

  QVector<QSharedPointer<MDCConfiguration> > configs = tool->readJsonConfigurationFile(configPath);
  if (configs.size() <= 0)
  {
    return false;
  }

  wizard()->removePage(1);    // Remove the placeholder page

  int pageId = 1;
  for (int i = 0; i < configs.size(); i++)
  {
    MDCConfiguration* config = configs[i].data();

    GatherDataPage* gatherPage = new GatherDataPage(config, wizard());
    wizard()->setPage(pageId, gatherPage);
    pageId++;

    InputDataPage* inputPage = new InputDataPage(config, wizard());
    wizard()->setPage(pageId, inputPage);

    pageId++;
  }

  D3DProcessingPage* d3dProcessingPage = new D3DProcessingPage(wizard());
  wizard()->setPage(pageId, d3dProcessingPage);

  return true;
}
