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
#ifndef SOURCEURLSMODEL_H
#define SOURCEURLSMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QString>
#include <vector>

class SourceUrlsModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  SourceUrlsModel(QObject *parent = nullptr);

  QModelIndex
  index(int row, int column,
        const QModelIndex &parent = QModelIndex()) const override;

  QModelIndex
  parent(const QModelIndex &index) const override;

  int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int
  columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant
  data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags
  flags(const QModelIndex &index) const override;

  bool
  setData(const QModelIndex &index, const QVariant &value,
          int role = Qt::EditRole) override;

  QVariant
  headerData(int section, Qt::Orientation orientation,
             int role = Qt::DisplayRole) const override;

  void
  addUrl(const QString &url);

  void
  removeUrl(const QModelIndex &index);

  std::vector<QString>
  getModel();

private:
  std::vector<QString> model;
};

#endif // SOURCEURLSMODEL_H
