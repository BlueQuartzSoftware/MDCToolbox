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

#include "PreviewTableItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableItem::PreviewTableItem(const QVector<QVariant>& data) :
  m_ItemData(data),
  m_PipelineState(NotRunning),
  m_FileIndex(-1),
  m_PaddingDigits(-1),
  m_InputDir(""),
  m_FilePrefix(""),
  m_FileSuffix(""),
  m_FileExtension("")
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableItem::~PreviewTableItem()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableItem::insertColumns(int position, int columns)
{
  if (position < 0 || position > m_ItemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
  {
    m_ItemData.insert(position, QVariant());
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableItem::removeColumns(int position, int columns)
{
  if (position < 0 || position + columns > m_ItemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
  {
    m_ItemData.remove(position);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PreviewTableItem::data(int column) const
{
  return m_ItemData.value(column);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PreviewTableItem::setData(int column, const QVariant& value)
{
  if (column < 0)
  {
    return false;
  }

  m_ItemData[column] = value;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreviewTableItem::PipelineState PreviewTableItem::getPipelineState()
{
  return m_PipelineState;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableItem::setPipelineState(PreviewTableItem::PipelineState state)
{
  m_PipelineState = state;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PreviewTableItem::getFileIndex()
{
  return m_FileIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableItem::setFileIndex(int num)
{
  m_FileIndex = num;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PreviewTableItem::getPaddingDigits()
{
  return m_PaddingDigits;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableItem::setPaddingDigits(int num)
{
  m_PaddingDigits = num;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableItem::getInputDirectory()
{
  return m_InputDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableItem::setInputDirectory(QString str)
{
  m_InputDir = str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableItem::getFilePrefix()
{
  return m_FilePrefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableItem::setFilePrefix(QString str)
{
  m_FilePrefix = str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableItem::getFileSuffix()
{
  return m_FileSuffix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableItem::setFileSuffix(QString str)
{
  m_FileSuffix = str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PreviewTableItem::getFileExtension()
{
  return m_FileExtension;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PreviewTableItem::setFileExtension(QString str)
{
  m_FileExtension = str;
}
