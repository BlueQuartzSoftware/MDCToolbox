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
#include <QtCore/QJsonDocument>

#include <QtWidgets/QMessageBox>

#include "MDCTool.h"
#include "GatherDataPage.h"
#include "InputDataPage.h"
#include "D3DProcessingPage.h"
#include "PreviewTableModel.h"
#include "Constants.h"

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
  registerField(MDCToolSpace::FieldNames::PipelineFilePath, pipelineLineEdit);
  registerField(MDCToolSpace::FieldNames::PipelineRunnerFilePath, pipelineRunnerLineEdit);
  registerField(MDCToolSpace::FieldNames::ConfigFilePath, configLineEdit);
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

  QString standardsError = "The pipeline file that was chosen does not conform to MDCTool standards.\n\n";
  standardsError.append("Please choose a new pipeline that conforms to the MDCTool standard.");

  // Check that the configuration file is a JSON file
  QString configPath = configLineEdit->text();
  QFileInfo configFi(configPath);
  if (configFi.completeSuffix() != "json")
  {
    QString s = "The configuration file that was chosen is not a JSON file.\n\n";
    s.append("Please choose a JSON file.");
    QMessageBox::critical(this, "MDCTool Error", s, QMessageBox::Ok);
    return false;
  }

  // Check that the chosen pipeline file is a JSON file
  QString pipelinePath = pipelineLineEdit->text();
  QFileInfo pipelineFi(pipelinePath);
  if (pipelineFi.completeSuffix() != "json")
  {
    QString s = "The pipeline file that was chosen is not a JSON file.\n\n";
    s.append("Please choose a JSON file.");
    QMessageBox::critical(this, "MDCTool Error", s, QMessageBox::Ok);
    return false;
  }

  // Validate the chosen pipeline file
  int errorCode = 0;
  QJsonObject obj = MDCTool::ReadJsonFile(pipelinePath, errorCode);
  if (errorCode < 0)
  {
    QString s = "MDCTool was unable to read the pipeline file that was chosen.\n\n";
    s.append("Please choose a new pipeline file.");
    QMessageBox::critical(NULL, "MDCTool Error", s, QMessageBox::Ok);
    return false;
  }

  QJsonObject firstFilterObj = obj["0"].toObject();
  if (firstFilterObj.isEmpty() == true)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString filterName = firstFilterObj["Filter_Name"].toString();
  if (filterName.isEmpty() == true)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  if (filterName != "ImportImageStack")
  {
    QString s = "The pipeline file that was chosen does not begin with \"Import Images (3D Stack)\" filter.\n\n";
    s.append("Please choose a new pipeline file that begins with this filter.");
    QMessageBox::critical(this, "MDCTool Error", s, QMessageBox::Ok);
    return false;
  }

  QJsonObject fileListInfoObj = firstFilterObj["InputFileListInfo"].toObject();
  if (fileListInfoObj.isEmpty() == true)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString endIndex = fileListInfoObj["EndIndex"].toString();
  if (endIndex != MDCToolSpace::ReplacementMonikers::EndIndex)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString startIndex = fileListInfoObj["StartIndex"].toString();
  if (startIndex != MDCToolSpace::ReplacementMonikers::StartIndex)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString fileExt = fileListInfoObj["FileExtension"].toString();
  if (fileExt != MDCToolSpace::ReplacementMonikers::FileExtension)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString filePrefix = fileListInfoObj["FilePrefix"].toString();
  if (filePrefix != MDCToolSpace::ReplacementMonikers::FilePrefix)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString fileSuffix = fileListInfoObj["FileSuffix"].toString();
  if (fileSuffix != MDCToolSpace::ReplacementMonikers::FileSuffix)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString paddingDigits = fileListInfoObj["PaddingDigits"].toString();
  if (paddingDigits != MDCToolSpace::ReplacementMonikers::PaddingDigits)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  QString inputPath = fileListInfoObj["InputPath"].toString();
  if (inputPath != MDCToolSpace::ReplacementMonikers::InputPath)
  {
    QMessageBox::critical(NULL, "MDCTool Error", standardsError, QMessageBox::Ok);
    return false;
  }

  // Read in the configurations
  QVector<QSharedPointer<MDCConfiguration> > configs = tool->readJsonConfigurationFile(configPath);
  if (configs.size() <= 0)
  {
    return false;
  }

  /* Remove the placeholder page that we only used to make sure that the wizard's
     Next/Finish button read "Next" and not "Finish" */
  wizard()->removePage(1);

  // Add in the gather and input pages for each configuration
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

  // Add the processing page (this is where we run the chosen pipeline with each image file)
  D3DProcessingPage* d3dProcessingPage = new D3DProcessingPage(wizard());
  wizard()->setPage(pageId, d3dProcessingPage);

  return true;
}
