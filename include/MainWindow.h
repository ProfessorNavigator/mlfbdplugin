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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Bases.h>
#include <BookWidget.h>
#include <DocumentInfoWidget.h>
#include <FBDCreation.h>
#include <FilesWidget.h>
#include <PaperBookWidget.h>
#include <QPaintEvent>
#include <QTextEdit>
#include <QTranslator>
#include <QWidget>
#include <SettingsManager.h>

class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow(QWidget *parent, const Bases &bases);

  virtual ~MainWindow();

  void
  createWindow();

private:
  void
  paintEvent(QPaintEvent *event) override;

  void
  checkInput();

  enum InputError
  {
    SrcFileNotexists,
    ResultPathEmpty,
    BookTitleEmpty,
    BookLanguageEmpty,
    FBDCreationError
  };

  void
  errorDialog(const InputError &er, const std::string &txt = std::string());

  void
  confirmationDialog();

  void
  startProcess();

  void
  createFBD();

  void
  createTitleInfo();

  void
  createSrcTitleInfo();

  void
  createDocumentInfo();

  void
  createPublishInfo();

  void
  finalDialog();

  Bases bases;

  QTranslator *translator;

  FilesWidget *files_widget;
  BookWidget *book_widget;
  BookWidget *src_book_widget;
  DocumentInfoWidget *document_info_widget;
  PaperBookWidget *paper_book_widget;
  QTextEdit *custom_info;

  std::shared_ptr<SettingsManager> settings;

  std::filesystem::path src_file_path;
  std::filesystem::path result_path;
  std::string book_title;
  std::string book_language;

  std::shared_ptr<FBDCreation> fbd;

  std::string fbd_buffer;

signals:
  void
  signalFinished();

  void
  signalError(const std::string &er);
};

#endif // MAINWINDOW_H
