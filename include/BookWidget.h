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
#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <AuthorsModel.h>
#include <Bases.h>
#include <GenreEditModel.h>
#include <Magick++.h>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QTextEdit>
#include <QWidget>
#include <SeriesModel.h>

class BookWidget : public QScrollArea
{
  Q_OBJECT
public:
  enum WidgetType
  {
    Book,
    SourceBook
  };

  BookWidget(QWidget *parent, const Bases &bases, const WidgetType &wt);

  virtual ~BookWidget();

  QLineEdit *book_title;
  QLineEdit *book_language;
  QLineEdit *book_date;
  QLineEdit *book_srclang;
  QLineEdit *book_keywords;
  AuthorsModel *book_authors = nullptr;
  SeriesModel *book_series = nullptr;
  GenreEditModel *book_genres = nullptr;
  QTextEdit *book_annotation;

  Magick::Image book_cover;
  QLabel *cover_widget;

  AuthorsModel *book_translators = nullptr;

private:
  void
  createWidget();

  enum AuthorType
  {
    Author,
    Translator
  };

  QWidget *
  authorsSection(const AuthorType &type);

  QWidget *
  seriesSection();

  QWidget *
  genresSection();

  QWidget *
  coverSection();

  void
  addAuthor(AuthorsModel *model);

  void
  addSeries();

  void
  addGenre();

  void
  openCover();

  void
  setCover(const QString &file);

  Bases bases;
  WidgetType wt;

  bool need_byte_conversion = false;
};

#endif // BOOKWIDGET_H
