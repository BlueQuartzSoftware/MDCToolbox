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

#ifndef PreviewTableItem_H
#define PreviewTableItem_H

#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <QtGui/QColor>
#include <QtGui/QFont>

class PreviewTableItem
{
public:
  PreviewTableItem(const QVector<QVariant>& data);
  virtual ~PreviewTableItem();

  enum PipelineState
  {
    NotRunning,
    InProgress,
    DoneError,
    DoneNoError
  };

  bool insertColumns(int position, int columns);
  bool removeColumns(int position, int columns);

  QVariant data(int column) const;
  bool setData(int column, const QVariant& value);

  PipelineState getPipelineState();
  void setPipelineState(PipelineState state);

  int getFileIndex();
  void setFileIndex(int num);

  int getPaddingDigits();
  void setPaddingDigits(int num);

  QString getInputDirectory();
  void setInputDirectory(QString str);

  QString getFilePrefix();
  void setFilePrefix(QString str);

  QString getFileSuffix();
  void setFileSuffix(QString str);

  QString getFileExtension();
  void setFileExtension(QString str);

private:
  QVector<QVariant>                                             m_ItemData;
  PipelineState                                                 m_PipelineState;

  // ImportFilesWidget Information
  int                                                           m_FileIndex;
  int                                                           m_PaddingDigits;
  QString                                                       m_InputDir;
  QString                                                       m_FilePrefix;
  QString                                                       m_FileSuffix;
  QString                                                       m_FileExtension;

  PreviewTableItem(const PreviewTableItem&);    // Copy Constructor Not Implemented
  void operator=(const PreviewTableItem&);  // Operator '=' Not Implemented
};

#endif // PreviewTableItem_H
