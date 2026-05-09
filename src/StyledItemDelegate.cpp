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

#include <QApplication>
#include <QPainter>
#include <StyledItemDelegate.h>

StyledItemDelegate::StyledItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

StyledItemDelegate::~StyledItemDelegate()
{
}

void
StyledItemDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);
  if(opt.state & QStyle::State_Selected)
    {
      QColor color = getColor(getRGBA("Table", "selection-background-color"));
      painter->fillRect(opt.rect, color);
      color = getColor(getRGBA("Table", "selection-color"));
      opt.palette.setColor(QPalette::Text, color);
    }
  QStyledItemDelegate::paint(painter, opt, index);
}

std::string
StyledItemDelegate::getRGBA(const std::string &object_name,
                            const std::string &css_id) const
{
  std::string result;

  std::string styles = qApp->styleSheet().toStdString();

  std::string find_str = "#" + object_name;
  std::string::size_type n = styles.find(find_str);
  if(n == std::string::npos)
    {
      return result;
    }
  n += find_str.size();

  find_str = "{";
  n = styles.find(find_str, n);
  if(n == std::string::npos)
    {
      return result;
    }
  n += find_str.size();

  find_str = "}";
  std::string::size_type n2 = styles.find(find_str, n);
  if(n2 == std::string::npos)
    {
      return result;
    }

  std::string val(styles.begin() + n, styles.begin() + n2);

  find_str = css_id + ":";
  n = val.find(find_str);
  if(n == std::string::npos)
    {
      return result;
    }
  val.erase(val.begin(), val.begin() + n + find_str.size());

  find_str = ";";
  n = val.find(find_str);
  if(n == std::string::npos)
    {
      return result;
    }
  std::copy(val.begin(), val.begin() + n, std::back_inserter(result));

  while(result.size() > 0)
    {
      char ch = *result.begin();
      if(ch >= 0 && ch <= ' ')
        {
          result.erase(result.begin());
        }
      else
        {
          break;
        }
    }

  return result;
}

QColor
StyledItemDelegate::getColor(const std::string &rgba) const
{
  QColor result;

  std::string find_str("(");
  std::string::size_type n = rgba.find(find_str);
  if(n == std::string::npos)
    {
      return result;
    }
  n += find_str.size();

  find_str = ")";
  std::string::size_type n2 = rgba.find(find_str, n);
  if(n2 == std::string::npos)
    {
      return result;
    }

  std::string val(rgba.begin() + n, rgba.begin() + n2);
  find_str = ",";
  QString color_str;
  for(;;)
    {
      n = val.find(find_str);
      if(n != std::string::npos)
        {
          std::string l_str(val.begin(), val.begin() + n);
          val.erase(0, n + find_str.size());
          while(l_str.size() > 0)
            {
              char ch = *l_str.begin();
              if(ch >= 0 && ch <= ' ')
                {
                  l_str.erase(l_str.begin());
                }
              else
                {
                  break;
                }
            }
          QString str = l_str.c_str();
          bool ok;
          int v = str.toInt(&ok);
          if(!ok)
            {
              return result;
            }
          str.setNum(v, 16);
          if(str.size() == 1)
            {
              str = "0" + str;
            }
          color_str += str.toUpper();
        }
      else
        {
          while(val.size() > 0)
            {
              char ch = *val.begin();
              if(ch >= 0 && ch <= ' ')
                {
                  val.erase(val.begin());
                }
              else
                {
                  break;
                }
            }
          QString str = val.c_str();
          bool ok;
          int v = str.toInt(&ok);
          if(!ok)
            {
              return result;
            }
          str.setNum(v, 16);
          if(str.size() == 1)
            {
              str = "0" + str;
            }
          color_str = str.toUpper() + color_str;
          break;
        }
    }

  color_str = "#" + color_str;
  result = QColor(color_str);

  return result;
}
