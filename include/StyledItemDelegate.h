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
#ifndef STYLEDITEMDELEGATE_H
#define STYLEDITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class StyledItemDelegate : public QStyledItemDelegate
{
public:
  StyledItemDelegate(QObject *parent = nullptr);

  virtual ~StyledItemDelegate();

  void
  paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

protected:
  std::string
  getRGBA(const std::string &object_name, const std::string &css_id) const;

  QColor
  getColor(const std::string &rgba) const;
};

#endif // STYLEDITEMDELEGATE_H
