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

#include "PreviewTableModel.h"

#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtGui/QImage>
#include <QtGui/QMovie>

PreviewTableModel* PreviewTableModel::self = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableModel::PreviewTableModel(QObject* parent) :
QAbstractTableModel(parent)
{
  m_LoadingGif = new QMovie(":/loading.gif");
  m_LoadingGif->start();

  // We need to register this metatype for cross-thread signals and slots
  qRegisterMetaType<IntVector>("IntVector");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableModel::~PreviewTableModel()
{
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableModel* PreviewTableModel::Instance()
{
  if (self == NULL)
  {
    self = new PreviewTableModel();
  }

  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PreviewTableModel::columnCount(const QModelIndex& parent) const
{
  return m_HorizontalHeaders.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PreviewTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
  {
    QModelIndex actualIndex = this->index(index.row(), index.column());
    PreviewTableItem* item = getItem(actualIndex);
    return item->data(index.column());
  }
  else if (role == Qt::TextAlignmentRole)
  {
    return Qt::AlignCenter;
  }
  else if (role == Qt::DecorationRole)
  {
    QModelIndex actualIndex = this->index(index.row(), index.column());
    PreviewTableItem* item = getItem(actualIndex);
    if (index.column() == D3DOutputPath && item->getPipelineState() == PreviewTableItem::InProgress)
    {
      return m_LoadingGif->currentImage();
    }
    else if (index.column() == D3DOutputPath && item->getPipelineState() == PreviewTableItem::DoneError)
    {
      return QImage(":/delete.png");
    }
    else if (index.column() == D3DOutputPath && item->getPipelineState() == PreviewTableItem::DoneNoError)
    {
      return QImage(":/check.png");
    }
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags PreviewTableModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
  return (defaultFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableItem* PreviewTableModel::getItem(const QModelIndex& index) const
{
  if (index.isValid())
  {
    QSharedPointer<PreviewTableItem> item = m_TableItems[index.row()];
    if (item)
    {
      return item.data();
    }
  }
  return NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PreviewTableModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if (section < 0)
  {
    return QVariant();
  }

  if (section < m_HorizontalHeaders.size() && orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return m_HorizontalHeaders[section];
  }
  else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    return QString::number(section+1);
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    m_HorizontalHeaders[section] = value.toString();
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableItem::PipelineState PreviewTableModel::getPipelineState(int row)
{
  return m_TableItems[row]->getPipelineState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::setPipelineState(int row, PreviewTableItem::PipelineState state)
{
  m_TableItems[row]->setPipelineState(state);

  if (state == PreviewTableItem::InProgress)
  {
    m_IndexInProgress = index(row, D3DOutputPath);
  }

  emit dataChanged(index(row, PreviewTableModel::D3DOutputPath), index(row, PreviewTableModel::D3DOutputPath));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginInsertRows(QModelIndex(), position, position + rows - 1);
  m_TableItems.insert(position, rows, QSharedPointer<PreviewTableItem>(new PreviewTableItem(QVector<QVariant>(m_HorizontalHeaders.size(), ""))));
  endInsertRows();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginRemoveRows(QModelIndex(), position, position + rows - 1);
  m_TableItems.remove(position, rows);
  endRemoveRows();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginInsertColumns(QModelIndex(), position, position + columns - 1);
  m_HorizontalHeaders.insert(position, columns, "");
  for (int i = 0; i < m_TableItems.size(); i++)
  {
    m_TableItems[i]->insertColumns(position, columns);
  }
  endInsertColumns();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
  Q_UNUSED(parent)
  beginRemoveColumns(QModelIndex(), position, position + columns - 1);
  m_HorizontalHeaders.remove(position, columns);
  for (int i = 0; i < m_TableItems.size(); i++)
  {
    m_TableItems[i]->removeColumns(position, columns);
  }
  endRemoveColumns();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PreviewTableModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)
  return m_TableItems.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  int row = index.row();

  if (index.column() >= m_HorizontalHeaders.size())
  {
    return false;
  }

  bool result = m_TableItems[row]->setData(index.column(), value);

  if (result)
  {
    emit dataChanged(index, index);
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PreviewTableModel::getFileIndex(int row)
{
  return m_TableItems[row]->getFileIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::setFileIndex(int row, int num)
{
  m_TableItems[row]->setFileIndex(num);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PreviewTableModel::getPaddingDigits(int row)
{
  return m_TableItems[row]->getPaddingDigits();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::setPaddingDigits(int row, int num)
{
  m_TableItems[row]->setPaddingDigits(num);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableModel::getInputDirectory(int row)
{
  return m_TableItems[row]->getInputDirectory();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::setInputDirectory(int row, QString str)
{
  m_TableItems[row]->setInputDirectory(str);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableModel::getFilePrefix(int row)
{
  return m_TableItems[row]->getFilePrefix();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::setFilePrefix(int row, QString str)
{
  m_TableItems[row]->setFilePrefix(str);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableModel::getFileSuffix(int row)
{
  return m_TableItems[row]->getFileSuffix();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::setFileSuffix(int row, QString str)
{
  m_TableItems[row]->setFileSuffix(str);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableModel::getFileExtension(int row)
{
  return m_TableItems[row]->getFileExtension();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::setFileExtension(int row, QString str)
{
  m_TableItems[row]->setFileExtension(str);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::clear()
{
  if (rowCount() > 0)
  {
    removeRows(0, rowCount());
  }

  if (columnCount() > 0)
  {
    removeColumns(0, columnCount());
    m_HorizontalHeaders.clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::clearContents()
{
  if (rowCount() > 0 && columnCount() > 0)
  {
    for (int row = 0; row < rowCount(); row++)
    {
      for (int col = 0; col < columnCount(); col++)
      {
        QModelIndex index = this->index(row, col);
        setData(index, "", Qt::DisplayRole);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableModel::clearHeaders(Qt::Orientation orientation)
{
  if (orientation == Qt::Horizontal)
  {
    for (int i = 0; i < m_HorizontalHeaders.size(); i++)
    {
      m_HorizontalHeaders[i] = "";
    }
    emit headerDataChanged(Qt::Horizontal, 0, m_HorizontalHeaders.size() - 1);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMovie* PreviewTableModel::getLoadingGif()
{
  return m_LoadingGif;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex PreviewTableModel::getIndexInProgress()
{
  return m_IndexInProgress;
}




