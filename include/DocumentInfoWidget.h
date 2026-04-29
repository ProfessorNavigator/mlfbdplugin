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
#ifndef DOCUMENTINFOWIDGET_H
#define DOCUMENTINFOWIDGET_H

#include <AuthorsModel.h>
#include <Bases.h>
#include <QLineEdit>
#include <QScrollArea>
#include <QTextEdit>
#include <QWidget>
#include <SourceUrlsModel.h>

class DocumentInfoWidget : public QScrollArea
{
  Q_OBJECT
public:
  DocumentInfoWidget(QWidget *parent, const Bases &bases);

  virtual ~DocumentInfoWidget();

  AuthorsModel *document_authors = nullptr;
  QLineEdit *document_program_used;
  QLineEdit *document_date;
  QLineEdit *source_ocr;
  QLineEdit *document_id;
  QLineEdit *document_version;
  SourceUrlsModel *document_src_urls = nullptr;
  QTextEdit *document_history;
  AuthorsModel *document_publishers = nullptr;

private:
  void
  createWidget();

  enum AuthorType
  {
    Author,
    Publisher
  };

  QWidget *
  authorsSection(const AuthorType &type);

  QWidget *
  sourceUrlsSection();

  void
  addAuthor(AuthorsModel *model);

  void
  addUrl();

  std::string
  toHex(unsigned char *buf, const size_t &buf_sz);

  Bases bases;
};

#endif // DOCUMENTINFOWIDGET_H
