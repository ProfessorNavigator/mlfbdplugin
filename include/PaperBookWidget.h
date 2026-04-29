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
#ifndef PAPERBOOKWIDGET_H
#define PAPERBOOKWIDGET_H

#include <QLineEdit>
#include <QScrollArea>
#include <QWidget>
#include <SeriesModel.h>

class PaperBookWidget : public QScrollArea
{
  Q_OBJECT
public:
  PaperBookWidget(QWidget *parent);

  virtual ~PaperBookWidget();

  QLineEdit *paper_book_name;
  QLineEdit *paper_book_publisher;
  QLineEdit *paper_book_city;
  QLineEdit *paper_book_year;
  QLineEdit *paper_book_isbn;
  SeriesModel *paper_book_series = nullptr;

private:
  void
  createWidget();

  QWidget *
  seriesSection();

  void
  addSeries();
};

#endif // PAPERBOOKWIDGET_H
