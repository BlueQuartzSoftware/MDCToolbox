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

#include "D3DProcessor.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QDir>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Utilities/TestObserver.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "PreviewTableModel.h"
#include "MDCTool.h"
#include "Constants.h"
#include "MDCToolProjTempFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void delay(int seconds)
{
  QTime dieTime = QTime::currentTime().addSecs(seconds);
  while (QTime::currentTime() < dieTime)
  {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
D3DProcessor::D3DProcessor(const QString &pipelineFilePath, const QString &outputDir, QObject* parent) :
  QObject(parent),
  m_PipelineFilePath(pipelineFilePath),
  m_OutputDir(outputDir),
  m_Stop(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
D3DProcessor::~D3DProcessor()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessor::run()
{
  PreviewTableModel* model = PreviewTableModel::Instance();

  // Read in the pipeline file and store as a QJsonObject.  We will treat this as a template.
  int errorCode = 0;
  QJsonObject templateObj = MDCTool::ReadJsonFile(m_PipelineFilePath, errorCode);

  // Now run the chosen pipeline on each image file in the table
  bool hasErrors = false;
  for (int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex index = model->index(i, PreviewTableModel::RawImagePath);
    QString imagePath = model->data(index, Qt::DisplayRole).toString();
    QFileInfo imageFi(imagePath);
    if (imagePath.isEmpty() == true)
    {
      QString s = tr("The image file path at row %1 is empty.").arg(QString::number(i + 1));
      QMessageBox::warning(NULL, "MDCTool Warning", s, QMessageBox::Ok);
      continue;
    }
    else if (imageFi.exists() == false)
    {
      QString s = tr("The image file path at row %1 does not exist.").arg(QString::number(i + 1));
      QMessageBox::warning(NULL, "MDCTool Warning", s, QMessageBox::Ok);
      continue;
    }

    model->setPipelineState(index.row(), PreviewTableItem::InProgress);
    emit processGeneratedMessage("Executing pipeline on \"" + imageFi.baseName() + "\"");

    {
      QJsonObject obj = templateObj;
      QJsonObject firstFilterObj = obj.find("0").value().toObject();
      QJsonObject inputFileListObj = firstFilterObj.find("InputFileListInfo").value().toObject();
      inputFileListObj["EndIndex"] = model->getFileIndex(i);
      inputFileListObj["StartIndex"] = model->getFileIndex(i);
      inputFileListObj["FileExtension"] = model->getFileExtension(i);
      inputFileListObj["FilePrefix"] = model->getFilePrefix(i);
      inputFileListObj["FileSuffix"] = model->getFileSuffix(i);
      inputFileListObj["InputPath"] = model->getInputDirectory(i);
      inputFileListObj["PaddingDigits"] = model->getPaddingDigits(i);

      QJsonObject::iterator iter = obj.end();
      iter--;
      if (iter.key() == "PipelineBuilder")
      {
        iter--;
      }

      QJsonObject lastFilterObj = iter.value().toObject();
      QString outputFilePath = m_OutputDir + "/" + imageFi.baseName() + ".dream3d";
      outputFilePath = QDir::toNativeSeparators(outputFilePath);
      lastFilterObj["OutputFile"] = outputFilePath;

      // We have to copy these back into the original object, because there is no support for QJsonObject references yet.
      firstFilterObj["InputFileListInfo"] = inputFileListObj;
      obj["0"] = firstFilterObj;
      obj[iter.key()] = lastFilterObj;

      QJsonDocument doc(obj);

      QFile tempPipelineFile(MDCToolPipelines::MDCToolTempPipeline);
      if (tempPipelineFile.open(QIODevice::WriteOnly))
      {
        tempPipelineFile.write(doc.toJson());
        tempPipelineFile.close();
      }

      delay(1);   // Artificial delay to see progress better

      // Run the pipeline
      ErrorPackage errPackage = executePipeline(MDCToolPipelines::MDCToolTempPipeline);
      if (errPackage.errorCode < 0)
      {
        model->setPipelineState(index.row(), PreviewTableItem::DoneError);
        model->setData(model->index(i, PreviewTableModel::D3DOutputPath), errPackage.errorMessage, Qt::DisplayRole);
        hasErrors = true;
      }
      else
      {
        model->setPipelineState(index.row(), PreviewTableItem::DoneNoError);
        model->setData(model->index(i, PreviewTableModel::D3DOutputPath), outputFilePath, Qt::DisplayRole);
      }

      tempPipelineFile.remove();
    }

    double value = (static_cast<double>(i + 1) / static_cast<double>(model->rowCount())) * 100;
    emit processGeneratedProgressValue(value);

    if (m_Stop == true)
    {
      break;
    }
  }

  if (m_Stop == true)
  {
    emit processGeneratedMessage("The process has been stopped.");
  }
  else
  {
    emit processGeneratedMessage("The process has finished.");
  }

  if (hasErrors == true)
  {
    QString s = tr("Errors occurred while running the pipeline on each image file.\n\nCheck the preview table to view all errors.");
    QMessageBox::critical(NULL, "MDCTool Error", s, QMessageBox::Ok);
  }

  emit processFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
D3DProcessor::ErrorPackage D3DProcessor::executePipeline(const QString &pipelineFilePath)
{
  QFileInfo fi(pipelineFilePath);
  QString ext = fi.completeSuffix();

  // Use the static method to read the Pipeline file and return a Filter Pipeline
  FilterPipeline::Pointer pipeline = JsonFilterParametersReader::ReadPipelineFromFile(pipelineFilePath);

  if (NULL == pipeline.get())
  {
    ErrorPackage package;
    package.errorCode = -1;
    package.errorMessage = "An error occurred while trying to read the pipeline file.";
    return package;
  }

  // Preflight the pipeline
  int err = pipeline->preflightPipeline();
  if (err < 0)
  {
    ErrorPackage package;
    package.errorCode = -1;
    package.errorMessage = "An error (" + QString::number(err) + ") occurred while trying to preflight the pipeline file.";
    return package;
  }

  // Now actually execute the pipeline
  pipeline->execute();
  err = pipeline->getErrorCondition();
  if (err < 0)
  {
    ErrorPackage package;
    package.errorCode = -1;
    package.errorMessage = "An error (" + QString::number(err) + ") occurred during the execution of the pipeline file.";
    return package;
  }

  ErrorPackage package;
  package.errorCode = 0;
  return package;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessor::stopProcess()
{
  setStop(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void D3DProcessor::setStop(bool val)
{
  m_Stop = val;
}
