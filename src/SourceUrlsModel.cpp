/*
 * Copyright (C) 2026 Yury Bobylev <bobilev_yury@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <SourceUrlsModel.h>

SourceUrlsModel::SourceUrlsModel(QObject *parent) : QAbstractItemModel(parent)
{
}

QModelIndex
SourceUrlsModel::index(int row, int column, const QModelIndex &parent) const
{
  QModelIndex result;

  if(parent.isValid())
    {
      return result;
    }
  if(static_cast<size_t>(row) >= model.size())
    {
      return result;
    }

  result = createIndex(row, column, model.data() + row);

  return result;
}

QModelIndex
SourceUrlsModel::parent(const QModelIndex &index) const
{
  return QModelIndex();
}

int
SourceUrlsModel::rowCount(const QModelIndex &parent) const
{
  if(parent.isValid())
    {
      return 0;
    }
  return static_cast<int>(model.size());
}

int
SourceUrlsModel::columnCount(const QModelIndex &parent) const
{
  if(parent.isValid())
    {
      return 0;
    }
  return 1;
}

QVariant
SourceUrlsModel::data(const QModelIndex &index, int role) const
{
  QVariant result;

  const QString *el
      = reinterpret_cast<const QString *>(index.constInternalPointer());
  if(el == nullptr)
    {
      return result;
    }

  switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
      {
        QString str = *el;
        result = QVariant(str);
        break;
      }
    case Qt::TextAlignmentRole:
      {
        result = QVariant(Qt::AlignCenter);
        break;
      }
    default:
      break;
    }

  return result;
}

Qt::ItemFlags
SourceUrlsModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags result
      = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;

  return result;
}

bool
SourceUrlsModel::setData(const QModelIndex &index, const QVariant &value,
                         int role)
{
  bool result = false;

  QString *el = const_cast<QString *>(
      reinterpret_cast<const QString *>(index.constInternalPointer()));
  if(el == nullptr || role != Qt::EditRole)
    {
      return result;
    }

  *el = value.toString();

  result = true;

  emit dataChanged(index, index);

  return result;
}

QVariant
SourceUrlsModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
  QVariant result;

  switch(orientation)
    {
    case Qt::Orientation::Horizontal:
      {
        switch(role)
          {
          case Qt::DisplayRole:
            {
              QString str(tr("URL"));
              result = QVariant(str);
              break;
            }
          case Qt::TextAlignmentRole:
            {
              result = QVariant(Qt::AlignCenter);
              break;
            }
          default:
            break;
          }
        break;
      }
    case Qt::Orientation::Vertical:
      {
        switch(role)
          {
          case Qt::DisplayRole:
            {
              QString str;
              str.setNum(section + 1);
              result = QVariant(str);
              break;
            }
          case Qt::TextAlignmentRole:
            {
              result = QVariant(Qt::AlignCenter);
              break;
            }
          default:
            break;
          }
        break;
      }
    default:
      break;
    }

  return result;
}

void
SourceUrlsModel::addUrl(const QString &url)
{
  if(url.isEmpty())
    {
      return void();
    }

  beginInsertRows(QModelIndex(), static_cast<int>(model.size()),
                  static_cast<int>(model.size()));
  model.push_back(url);
  endInsertRows();
}

void
SourceUrlsModel::removeUrl(const QModelIndex &index)
{
  if(!index.isValid())
    {
      return void();
    }
  if(static_cast<size_t>(index.row()) >= model.size())
    {
      return void();
    }
  beginRemoveRows(QModelIndex(), index.row(), index.row());
  model.erase(model.begin() + index.row());
  endRemoveRows();
}

std::vector<QString>
SourceUrlsModel::getModel()
{
  return model;
}
