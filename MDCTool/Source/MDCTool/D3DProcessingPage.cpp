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

#include "D3DProcessingPage.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtGui/QMovie>

#include <QtWidgets/QMessageBox>

#include <iostream>

#include "D3DProcessor.h"
#include "PreviewTableModel.h"
#include "Constants.h"
#include "MDCTool.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
D3DProcessingPage::D3DProcessingPage(QWidget* parent) :
  QWizardPage(parent),
  m_WorkerThread(NULL),
  m_FirstRun(true)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
D3DProcessingPage::~D3DProcessingPage()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::setupGui()
{
  PreviewTableModel* model = PreviewTableModel::Instance();
  tableView->setModel(model);
  tableView->horizontalHeader()->setSectionResizeMode(PreviewTableModel::Conditions, QHeaderView::ResizeToContents);
  tableView->horizontalHeader()->setSectionResizeMode(PreviewTableModel::RawImagePath, QHeaderView::ResizeToContents);
  tableView->horizontalHeader()->setSectionResizeMode(PreviewTableModel::D3DOutputPath, QHeaderView::ResizeToContents);

  connect(model->getLoadingGif(), SIGNAL(frameChanged(int)), this, SLOT(modelLoadingGif_frameChanged(int)));

  // Disable start button until Output Directory is filled in
  startBtn->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::showEvent(QShowEvent* event)
{
  wizard()->setWindowTitle("MDCTool - Step " + QString::number(wizard()->currentId() + 1) + " of " + QString::number(wizard()->pageIds().size()));

  double value = static_cast<double>(wizard()->currentId()) / static_cast<double>(wizard()->pageIds().size());
  int progress = static_cast<int>(value * 100);
  progressBar->setValue(progress);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::on_startBtn_pressed()
{
  if (startBtn->text().compare("Stop") == 0)
  {
    emit processStopped();
    return;
  }

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    if (m_WorkerThread->isFinished() == true)
    {
      delete m_WorkerThread;
      m_WorkerThread = NULL;
    }
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  QString pipelineFilePath = field(MDCToolSpace::FieldNames::PipelineFilePath).toString();
  m_Processor = QSharedPointer<D3DProcessor>(new D3DProcessor(pipelineFilePath, outputDir->text(), this));

  // When the thread starts its event loop, start the PipelineBuilder going
  connect(m_WorkerThread, SIGNAL(started()),
    m_Processor.data(), SLOT(run()));

  // When the process ends then tell the QThread to stop its event loop
  connect(m_Processor.data(), SIGNAL(processFinished()),
    m_WorkerThread, SLOT(quit()));

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
    this, SLOT(processDidFinish()));

  // If the user clicks on the "Stop" button send a message to the processor object
  connect(this, SIGNAL(processStopped()),
    m_Processor.data(), SLOT(stopProcess()), Qt::DirectConnection);

  // When the process ends then tell the QThread to stop its event loop
  connect(m_Processor.data(), SIGNAL(processGeneratedMessage(const QString &)),
    this, SLOT(updateStatusMessage(const QString &)));

  // When the process ends then tell the QThread to stop its event loop
  connect(m_Processor.data(), SIGNAL(processGeneratedProgressValue(double)),
    this, SLOT(updateProgress(double)));

  // Move the FilterPipeline object into the thread that we just created.
  m_Processor->moveToThread(m_WorkerThread);

  PreviewTableModel* model = PreviewTableModel::Instance();

  // If this is not the first run, reset all pipeline states back to "Not Running"
  if (m_FirstRun == false)
  {
    for (int i = 0; i < model->rowCount(); i++)
    {
      model->setPipelineState(i, PreviewTableItem::NotRunning);
      model->setData(model->index(i, PreviewTableModel::D3DOutputPath), "", Qt::DisplayRole);
    }
  }

  m_WorkerThread->start();
  emit completeChanged();
  wizard()->button(QWizard::BackButton)->setEnabled(false);
  startBtn->setText("Stop");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::processDidFinish()
{
  m_FirstRun = false;
  completeChanged();
  updateProgress(0);
  m_Processor->setStop(false);
  wizard()->button(QWizard::BackButton)->setEnabled(true);
  startBtn->setText("Start");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::on_outputDirBtn_pressed()
{
  QString outputDirPath = MDCTool::ChooseDirectory(this, "Choose Output Directory");
  if (outputDirPath.isEmpty()) { return; }

  outputDir->setText(outputDirPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::on_outputDir_textChanged(const QString &text)
{
  QDir dir(text);
  if (text.isEmpty() == false && dir.exists() == true)
  {
    startBtn->setEnabled(true);
  }
  else
  {
    startBtn->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::updateStatusMessage(const QString &msg)
{
  executionStatus->setText(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::updateProgress(double value)
{
  executionProgress->setValue(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::modelLoadingGif_frameChanged(int frameNumber)
{
  PreviewTableModel* model = PreviewTableModel::Instance();
  tableView->update(model->getIndexInProgress());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool D3DProcessingPage::isComplete() const
{
  if (NULL != m_WorkerThread && m_WorkerThread->isRunning())
  {
    return false;
  }

  PreviewTableModel* model = PreviewTableModel::Instance();
  for (int i = 0; i < model->rowCount(); i++)
  {
    PreviewTableItem::PipelineState state = model->getPipelineState(i);
    if (state != PreviewTableItem::DoneNoError)
    {
      return false;
    }
  }

  executionStatus->clear();

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessingPage::cleanupPage()
{
  PreviewTableModel* model = PreviewTableModel::Instance();
  for (int i = 0; i < model->rowCount(); i++)
  {
    model->setPipelineState(i, PreviewTableItem::NotRunning);
    model->setData(model->index(i, PreviewTableModel::D3DOutputPath), "", Qt::DisplayRole);
  }

  QWizardPage::cleanupPage();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool D3DProcessingPage::validatePage()
{
  PreviewTableModel* model = PreviewTableModel::Instance();
  for (int i = 0; i < model->rowCount(); i++)
  {
    model->setPipelineState(i, PreviewTableItem::NotRunning);
  }

  return true;
}
