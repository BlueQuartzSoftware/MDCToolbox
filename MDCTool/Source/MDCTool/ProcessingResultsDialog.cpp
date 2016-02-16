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

#include "ProcessingResultsDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessingResultsDialog::ProcessingResultsDialog(QList<D3DProcessorObserver::ProcessorPipelineMessage> messages, QWidget* parent) :
  QDialog(parent)
{
  setupUi(this);

  setupGui(messages);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessingResultsDialog::~ProcessingResultsDialog()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessingResultsDialog::setupGui(QList<D3DProcessorObserver::ProcessorPipelineMessage> messages)
{
  tableWidget->horizontalHeader()->setSectionResizeMode(FileName, QHeaderView::ResizeToContents);
  tableWidget->horizontalHeader()->setSectionResizeMode(Message, QHeaderView::Stretch);
  tableWidget->horizontalHeader()->setSectionResizeMode(Code, QHeaderView::ResizeToContents);
  tableWidget->horizontalHeader()->setSectionResizeMode(Occurrence, QHeaderView::ResizeToContents);

  for (int i = 0; i < messages.size(); i++)
  {
    D3DProcessorObserver::ProcessorPipelineMessage processorMsg = messages[i];
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    QColor bColor;
    if (processorMsg.messageType == D3DProcessorObserver::Error)
    {
      bColor.setRed(255);
      bColor.setGreen(191);
      bColor.setBlue(193);
    }
    else if (processorMsg.messageType == D3DProcessorObserver::Warning)
    {
      bColor.setRed(251);
      bColor.setGreen(254);
      bColor.setBlue(137);
    }
    
    QTableWidgetItem* fileNameItem = new QTableWidgetItem(processorMsg.fileName);
    fileNameItem->setBackgroundColor(bColor);
    tableWidget->setItem(row, FileName, fileNameItem);

    QTableWidgetItem* messageItem = new QTableWidgetItem(processorMsg.message);
    messageItem->setBackgroundColor(bColor);
    tableWidget->setItem(row, Message, messageItem);

    QTableWidgetItem* codeItem = new QTableWidgetItem(QString::number(processorMsg.errorCode));
    codeItem->setBackgroundColor(bColor);
    tableWidget->setItem(row, Code, codeItem);

    QTableWidgetItem* occurrenceItem;
    if (processorMsg.occurrence == D3DProcessorObserver::Preflight)
    {
      occurrenceItem = new QTableWidgetItem("Preflight");
    }
    else if (processorMsg.occurrence == D3DProcessorObserver::Execution)
    {
      occurrenceItem = new QTableWidgetItem("Execution");
    }
    else
    {
      occurrenceItem = new QTableWidgetItem("Unknown");
    }
    occurrenceItem->setBackgroundColor(bColor);
    tableWidget->setItem(row, Occurrence, occurrenceItem);

    tableWidget->verticalHeader()->setSectionResizeMode(row, QHeaderView::ResizeToContents);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessingResultsDialog::on_okBtn_pressed()
{
  close();
}