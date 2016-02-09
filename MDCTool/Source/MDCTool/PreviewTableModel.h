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

#ifndef PreviewTableModel_H
#define PreviewTableModel_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QMetaType>

#include <QtGui/QMovie>

#include "PreviewTableItem.h"

class PreviewTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  ~PreviewTableModel();

  static PreviewTableModel* Instance();

  enum Columns
  {
    Conditions,
    RawImagePath,
    D3DOutputPath
  };

  typedef QVector<int> IntVector;

  void clear();
  void clearContents();
  void clearHeaders(Qt::Orientation orientation);

  QVariant data(const QModelIndex& index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::DisplayRole);

  PreviewTableItem::PipelineState getPipelineState(int row);
  void setPipelineState(int row, PreviewTableItem::PipelineState state);

  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  int columnCount(const QModelIndex& parent = QModelIndex()) const;

  bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex());
  bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex());

  bool insertColumns(int position, int columns, const QModelIndex& parent = QModelIndex());
  bool removeColumns(int position, int columns, const QModelIndex& parent = QModelIndex());

  Qt::ItemFlags flags(const QModelIndex& index) const;

  bool setData(const QModelIndex& index, const QVariant& value, int role);

  int getFileIndex(int row);
  void setFileIndex(int row, int num);

  int getPaddingDigits(int row);
  void setPaddingDigits(int row, int num);

  QString getInputDirectory(int row);
  void setInputDirectory(int row, QString str);

  QString getFilePrefix(int row);
  void setFilePrefix(int row, QString str);

  QString getFileSuffix(int row);
  void setFileSuffix(int row, QString str);

  QString getFileExtension(int row);
  void setFileExtension(int row, QString str);

  QMovie* getLoadingGif();

  QModelIndex getIndexInProgress();

protected:
  PreviewTableModel(QObject* parent = 0);

private:
  static PreviewTableModel*                             self;

  QVector<QSharedPointer<PreviewTableItem> >            m_TableItems;

  QVector<QString>                                      m_HorizontalHeaders;

  QMovie*                                               m_LoadingGif;

  QModelIndex                                           m_IndexInProgress;

  PreviewTableItem* getItem(const QModelIndex& index) const;

  PreviewTableModel(const PreviewTableModel&);    // Copy Constructor Not Implemented
  void operator=(const PreviewTableModel&);  // Operator '=' Not Implemented
};

#endif // PreviewTableModel_H
