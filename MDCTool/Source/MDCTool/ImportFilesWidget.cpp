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

#include "ImportFilesWidget.h"

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtWidgets/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QButtonGroup>

#include "MDCToolStyles.h"
#include "QFileCompleter.h"
#include "MDCTool.h"

// Initialize private static member variable
QString ImportFilesWidget::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportFilesWidget::ImportFilesWidget(QWidget* parent) :
  QWidget(parent)
{
  if ( getOpenDialogLastDirectory().isEmpty() )
  {
    setOpenDialogLastDirectory( QDir::homePath() );
  }

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportFilesWidget::~ImportFilesWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::setupGui()
{
  QFileCompleter* com = new QFileCompleter(this, true);
  m_InputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString&)),
                    this, SLOT(on_m_InputDir_textChanged(const QString&)));

  m_ErrorMessage->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportFilesWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
  //  std::cout << "outFilePath: " << outFilePath << std::endl;
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists() )
  {
    MDCToolStyles::LineEditErrorStyle(lineEdit);
  }
  else
  {
    MDCToolStyles::LineEditClearStyle(lineEdit);
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::checkIOFiles()
{
  if (true == this->verifyPathExists(m_InputDir->text(), this->m_InputDir))
  {
    findEbsdMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_InputDirBtn_clicked()
{
  QString outputFile = MDCTool::ChooseDirectory(this, "Select Image Directory");
  
  if (!outputFile.isNull())
  {
    m_InputDir->blockSignals(true);
    m_InputDir->setText(QDir::toNativeSeparators(outputFile));
    on_m_InputDir_textChanged(m_InputDir->text());
    getOpenDialogLastDirectory() = outputFile;
    m_InputDir->blockSignals(false);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_InputDir_textChanged(const QString& text)
{
  if (verifyPathExists(m_InputDir->text(), m_InputDir) )
  {
    findEbsdMaxSliceAndPrefix();
    QDir dir(m_InputDir->text());
    QString dirname = dir.dirName();
    dir.cdUp();

    generateExampleEbsdInputFile();
    m_InputDir->blockSignals(true);
    m_InputDir->setText(QDir::toNativeSeparators(m_InputDir->text()));
    m_InputDir->blockSignals(false);
  }
  else
  {
    m_FileListView->clear();
    m_PackageList.clear();
    emit fileListChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_ZEndIndex_valueChanged(int value)
{
  generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_ZStartIndex_valueChanged(int value)
{
  generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_TotalDigits_valueChanged(int value)
{
  generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_FileExt_textChanged(const QString& string)
{
  generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_FileSuffix_textChanged(const QString& string)
{
  generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::on_m_FilePrefix_textChanged(const QString& string)
{
  generateExampleEbsdInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::generateExampleEbsdInputFile()
{
  m_PackageList.clear();

  QString filename = QString("%1%2%3.%4").arg(m_FilePrefix->text())
                     .arg(m_ZStartIndex->text(), m_TotalDigits->value(), '0')
                     .arg(m_FileSuffix->text()).arg(m_FileExt->text());
  m_GeneratedFileNameExample->setText(filename);

  int start = m_ZStartIndex->value();
  int end = m_ZEndIndex->value();
  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  QVector<FilePathGenerator::FilePackage> packageList = FilePathGenerator::GenerateFileList(start, end, hasMissingFiles,
                              m_InputDir->text(),
                              m_FilePrefix->text(),
                              m_FileSuffix->text(),
                              m_FileExt->text(),
                              m_TotalDigits->value());
  m_FileListView->clear();
  QIcon greenDot = QIcon(QString(":/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/bullet_ball_red.png"));

  int validFileCount = 0;
  for (QVector<FilePathGenerator::FilePackage>::size_type i = 0; i < packageList.size(); ++i)
  {
    FilePathGenerator::FilePackage package = packageList.at(i);
    QString filePath(package.completeFilePath);
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem( filePath, m_FileListView);
    if (fi.exists() == true)
    {
      m_PackageList.push_back(package);
      item->setIcon(greenDot);
      validFileCount++;
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }

  if (hasMissingFiles == true)
  {
    m_ErrorMessage->setVisible(true);
    m_ErrorMessage->setText("Alert: " + QString::number(validFileCount) + " of " + QString::number(packageList.size()) + " files exist.");
    MDCToolStyles::LabelRedErrorStyle(m_ErrorMessage);
  }
  else
  {
    m_ErrorMessage->setVisible(true);
    m_ErrorMessage->setText("All files exist.");
    MDCToolStyles::LabelClearStyle(m_ErrorMessage);
  }

  emit fileListChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportFilesWidget::findEbsdMaxSliceAndPrefix()
{
  if (m_InputDir->text().length() == 0) { return; }
  QDir dir(m_InputDir->text());
  m_FileExt->setText("");
  {
    QString ext = ".tiff";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("tiff");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".tif";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("tif");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".gif";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("gif");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".jpeg";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("jpeg");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".jpg";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("jpg");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".jif";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("jif");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".png";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("png");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".pdf";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("pdf");
    }
  }
  if (m_FileExt->text().isEmpty() == true)
  {
    QString ext = ".bmp";
    QStringList filters;
    filters << "*" + ext;
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() != 0)
    {
      m_FileExt->setText("bmp");
    }
  }
  // Add in more file formats to look for here


  // Final check to make sure we have a valid file extension
  if (m_FileExt->text().isEmpty() == true)
  {
    return;
  }

  QString ext = "." + m_FileExt->text();
  QStringList filters;
  filters << "*" + ext;
  dir.setNameFilters(filters);
  QFileInfoList angList = dir.entryInfoList();

  int minSlice = 0;
  int maxSlice = 0;
  int currValue = 0;
  bool flag = false;
  bool ok;
  QString fPrefix;
  QRegExp rx("(\\d+)");
  QStringList list;
  int pos = 0;
  int digitStart = 0;
  int digitEnd = 0;
  int totalOimFilesFound = 0;
  int minTotalDigits = 1000;
  foreach(QFileInfo fi, angList)
  {
    if (fi.suffix().compare(ext) && fi.isFile() == true)
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      QString fns = fn;
      int length =  fn.length();
      digitEnd = length - 1;
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        --digitEnd;
      }
      pos = digitEnd;

      digitStart = pos = rx.indexIn(fn, pos);
      digitEnd = digitStart;
      while ((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        ++digitEnd;
      }

      if ( digitEnd - digitStart < minTotalDigits) { minTotalDigits = digitEnd - digitStart; }
      m_TotalDigits->setValue(minTotalDigits);
      if (list.size() > 0)
      {
        currValue = list.front().toInt(&ok);
        if (false == flag) { minSlice = currValue; flag = true;}
        if (currValue > maxSlice) { maxSlice = currValue; }
        if (currValue < minSlice) { minSlice = currValue; }
      }
      ++totalOimFilesFound;
    }
  }
  this->m_TotalSlices->setText(QString::number(totalOimFilesFound));
  this->m_FilePrefix->setText(fPrefix);
  this->m_ZStartIndex->setValue(minSlice);
  this->m_ZEndIndex->setValue(maxSlice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<FilePathGenerator::FilePackage> ImportFilesWidget::getFileList()
{
  return m_PackageList;
}

