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

#include <LibArchive.h>
#include <MainWindow.h>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QStyleOption>
#include <QTabWidget>
#include <QVBoxLayout>
#include <SettingsWindow.h>
#include <StyledWindow.h>
#include <iostream>
#include <thread>

MainWindow::MainWindow(QWidget *parent, const Bases &bases) : QWidget(parent)
{
  this->bases = bases;
  settings = std::make_shared<SettingsManager>();

  translator = new QTranslator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for(const QString &locale : uiLanguages)
    {
      const QString baseName = "MLFBDPlugin_" + QLocale(locale).name();
      if(translator->load(":/i18n/" + baseName))
        {
          qApp->installTranslator(translator);
          break;
        }
    }

  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle("MLFBDPlugin");
  this->setWindowModality(Qt::WindowModal);

  this->setObjectName("Window");
}

MainWindow::~MainWindow()
{
  qApp->setStyleSheet(settings->getMainAppStyles());
  delete translator;
}

void
MainWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QMenuBar *menu_bar = new QMenuBar;
  menu_bar->setObjectName("MenuBar");
  v_box->addWidget(menu_bar, 0, Qt::AlignLeft);

  QAction *act = new QAction(tr("Settings"));
  connect(act, &QAction::triggered, this,
          [this]
            {
              SettingsWindow *sw = new SettingsWindow(this, settings);
              sw->createWindow();
              sw->show();
            });
  menu_bar->addAction(act);

  connect(menu_bar, &QMenuBar::destroyed, act,
          [act]
            {
              delete act;
            });

  QTabWidget *tabs = new QTabWidget;
  tabs->setObjectName("TabWidget");
  tabs->tabBar()->setObjectName("TabBar");
  v_box->addWidget(tabs);

  files_widget = new FilesWidget(nullptr, bases.mlbp);
  tabs->addTab(files_widget, tr("Files"));

  book_widget = new BookWidget(nullptr, bases, BookWidget::Book);
  tabs->addTab(book_widget, tr("Book"));

  src_book_widget = new BookWidget(nullptr, bases, BookWidget::SourceBook);
  tabs->addTab(src_book_widget, tr("Source book"));

  document_info_widget = new DocumentInfoWidget(nullptr, bases);
  tabs->addTab(document_info_widget, tr("Document info"));

  paper_book_widget = new PaperBookWidget(nullptr);
  tabs->addTab(paper_book_widget, tr("Paper book"));

  custom_info = new QTextEdit;
  custom_info->setObjectName("TextEdit");
  tabs->addTab(custom_info, tr("Custom info"));

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *apply = new QPushButton;
  apply->setText(tr("Apply"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(apply);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  apply->setGraphicsEffect(shadow);
  apply->setObjectName("ApplyButton");
  connect(apply, &QPushButton::clicked, this, &MainWindow::checkInput);
  h_box->addWidget(apply, 0, Qt::AlignCenter);

  QPushButton *close = new QPushButton;
  close->setText(tr("Close"));
  shadow = new QGraphicsDropShadowEffect(close);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  close->setGraphicsEffect(shadow);
  close->setObjectName("CancelButton");
  connect(close, &QPushButton::clicked, this, &MainWindow::close);
  h_box->addWidget(close, 0, Qt::AlignCenter);

  QScreen *screen = this->parentWidget()->screen();
  QSize av_sz = screen->availableSize();
  av_sz.setHeight(av_sz.height() * 0.5);
  av_sz.setWidth(av_sz.width() * 0.5);
  this->resize(av_sz);
}

void
MainWindow::paintEvent(QPaintEvent *event)
{
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void
MainWindow::checkInput()
{
  std::string str = files_widget->src_file->text().toStdString();
  src_file_path = std::u8string(str.begin(), str.end());
  if(!std::filesystem::exists(src_file_path))
    {
      errorDialog(InputError::SrcFileNotexists);
      return void();
    }

  str = files_widget->result_file->text().toStdString();
  if(str.empty())
    {
      errorDialog(InputError::ResultPathEmpty);
      return void();
    }
  result_path = std::u8string(str.begin(), str.end());

  book_title = book_widget->book_title->text().toStdString();
  if(book_title.empty())
    {
      errorDialog(InputError::BookTitleEmpty);
      return void();
    }

  book_language = book_widget->book_language->text().toStdString();
  if(book_language.empty())
    {
      errorDialog(InputError::BookLanguageEmpty);
      return void();
    }

  confirmationDialog();
}

void
MainWindow::errorDialog(const InputError &er, const std::string &txt)
{
  StyledWindow *window = new StyledWindow(this);
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  switch(er)
    {
    case InputError::SrcFileNotexists:
      {
        QLabel *lab = new QLabel;
        lab->setObjectName("Label");
        lab->setText(tr("Source file does not exists!"));
        v_box->addWidget(lab, 0, Qt::AlignCenter);
        break;
      }
    case InputError::ResultPathEmpty:
      {
        QLabel *lab = new QLabel;
        lab->setObjectName("Label");
        lab->setText(tr("Result file path is empty!"));
        v_box->addWidget(lab, 0, Qt::AlignCenter);
        break;
      }
    case InputError::BookTitleEmpty:
      {
        QLabel *lab = new QLabel;
        lab->setObjectName("Label");
        lab->setText(tr("Book title is empty!"));
        v_box->addWidget(lab, 0, Qt::AlignCenter);
        break;
      }
    case InputError::BookLanguageEmpty:
      {
        QLabel *lab = new QLabel;
        lab->setObjectName("Label");
        lab->setText(tr("Book language is empty!"));
        v_box->addWidget(lab, 0, Qt::AlignCenter);
        break;
      }
    case InputError::FBDCreationError:
      {
        QLabel *lab = new QLabel;
        lab->setObjectName("Label");
        lab->setText(tr("Error on fbd file creation!"));
        v_box->addWidget(lab, 0, Qt::AlignCenter);

        lab = new QLabel;
        lab->setObjectName("Label");
        lab->setText(txt.c_str());
        v_box->addWidget(lab, 0, Qt::AlignCenter);
        break;
      }
    default:
      break;
    }

  QPushButton *close = new QPushButton;
  close->setText(tr("Close"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(close);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  close->setGraphicsEffect(shadow);
  close->setObjectName("ApplyButton");
  connect(close, &QPushButton::clicked, window, &StyledWindow::close);
  v_box->addWidget(close, 0, Qt::AlignCenter);

  window->show();
}

void
MainWindow::confirmationDialog()
{
  StyledWindow *window = new StyledWindow(this);
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Are you sure?"));
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *yes = new QPushButton;
  yes->setText(tr("Yes"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(yes);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  yes->setGraphicsEffect(shadow);
  yes->setObjectName("ApplyButton");
  connect(yes, &QPushButton::clicked, this,
          [this, window]
            {
              startProcess();
              window->close();
            });
  h_box->addWidget(yes, 0, Qt::AlignCenter);

  QPushButton *no = new QPushButton;
  no->setText(tr("No"));
  shadow = new QGraphicsDropShadowEffect(no);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  no->setGraphicsEffect(shadow);
  no->setObjectName("CancelButton");
  connect(no, &QPushButton::clicked, window, &StyledWindow::close);
  h_box->addWidget(no, 0, Qt::AlignCenter);

  window->show();
}

void
MainWindow::startProcess()
{
  StyledWindow *window = new StyledWindow;
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");
  window->allowClose(false);

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("fbd file creating..."));
  v_box->addWidget(lab);

  connect(this, &MainWindow::signalFinished, window,
          [window, this]
            {
              finalDialog();
              window->allowClose(true);
              window->close();
            });

  connect(this, &MainWindow::signalError, window,
          [this, window](const std::string &er)
            {
              errorDialog(InputError::FBDCreationError, er);
              window->allowClose(true);
              window->close();
            });

  window->show();

  std::thread thr(
      [this]
        {
          try
            {
              createFBD();
            }
          catch(std::exception &er)
            {
              emit signalError(er.what());
              return void();
            }

          emit signalFinished();
        });
  thr.detach();
}

void
MainWindow::createFBD()
{
  fbd = std::make_shared<FBDCreation>();

  createTitleInfo();
  createSrcTitleInfo();
  createDocumentInfo();
  createPublishInfo();

  std::string str = custom_info->toPlainText().toStdString();
  fbd->addCustomInfo(str);

  fbd_buffer = std::move(fbd->writeToBuffer());

  std::u8string u8str = src_file_path.filename().u8string();
  str = std::string(u8str.begin(), u8str.end());
  std::string find_str = bases.mlbp->getExtension(str);
  std::string::size_type n = str.find(find_str);
  if(n != std::string::npos && !find_str.empty())
    {
      str.erase(str.begin() + n, str.end());
    }
  str += ".fbd";

  std::unique_ptr<LibArchive> la(new LibArchive(bases.mlbp));
  la->writeBufferObjectToArchive(fbd_buffer, result_path, str,
                                 std::filesystem::perms::all, true);

  la->writeToArchive(src_file_path, result_path,
                     std::string(u8str.begin(), u8str.end()),
                     std::filesystem::perms::none, false);
}

void
MainWindow::createTitleInfo()
{
  fbd->addTitleInfo(FBDCreation::TitleInfo);

  std::vector<UDBElement> base = book_widget->book_genres->getGenres();
  for(auto it = base.begin(); it != base.end(); it++)
    {
      if(it->content.empty())
        {
          continue;
        }
      fbd->addTextElement("title-info", "genre", it->content);
    }

  base = book_widget->book_authors->getAuthors();
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addAuthor("title-info", *it, FBDCreation::Author);
    }

  fbd->addTextElement("title-info", "book-title", book_title);

  fbd->addAnnotation(
      "title-info", book_widget->book_annotation->toPlainText().toStdString());

  std::string str = book_widget->book_keywords->text().toStdString();
  if(!str.empty())
    {
      fbd->addTextElement("title-info", "keywords", str);
    }

  fbd->addDate("title-info", book_widget->book_date->text().toStdString());

  str.clear();
  try
    {
      Magick::Blob blob;
      book_widget->book_cover.write(&blob, "JPG");
      str = blob.base64();
    }
  catch(Magick::Exception &er)
    {
      std::cout << "MainWindow::createTitleInfo: \"" << er.what() << "\""
                << std::endl;
    }

  fbd->addCoverPage("title-info", str);

  fbd->addTextElement("title-info", "lang", book_language);

  str = book_widget->book_srclang->text().toStdString();
  if(!str.empty())
    {
      fbd->addTextElement("title-info", "src-lang", str);
    }

  base = book_widget->book_translators->getAuthors();
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addAuthor("title-info", *it, FBDCreation::Translator);
    }

  base = book_widget->book_series->getSeries();
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addSequence("title-info", *it);
    }
}

void
MainWindow::createSrcTitleInfo()
{
  bool src_created = false;

  std::vector<UDBElement> base = src_book_widget->book_genres->getGenres();
  if(base.size() > 0)
    {
      fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
      src_created = true;
    }
  for(auto it = base.begin(); it != base.end(); it++)
    {
      if(it->content.empty())
        {
          continue;
        }
      fbd->addTextElement("src-title-info", "genre", it->content);
    }

  base = src_book_widget->book_authors->getAuthors();
  if(base.size() > 0 && !src_created)
    {
      fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
      src_created = true;
    }
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addAuthor("src-title-info", *it, FBDCreation::Author);
    }

  std::string str = src_book_widget->book_title->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
          src_created = true;
        }
      fbd->addTextElement("src-title-info", "book-title", str);
    }

  str = src_book_widget->book_annotation->toPlainText().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
          src_created = true;
        }
      fbd->addAnnotation("src-title-info", str);
    }

  str = src_book_widget->book_keywords->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
          src_created = true;
        }
      fbd->addTextElement("src-title-info", "keywords", str);
    }

  str = src_book_widget->book_date->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
          src_created = true;
        }
      fbd->addDate("src-title-info", str);
    }

  str.clear();
  try
    {
      Magick::Blob blob;
      src_book_widget->book_cover.write(&blob, "JPG");
      str = blob.base64();
    }
  catch(Magick::Exception &er)
    {
      std::cout << "MainWindow::createTitleInfo: \"" << er.what() << "\""
                << std::endl;
    }

  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
          src_created = true;
        }
      fbd->addCoverPage("src-title-info", str);
    }

  str = src_book_widget->book_language->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
          src_created = true;
        }
      fbd->addTextElement("src-title-info", "lang", str);
    }

  str = src_book_widget->book_srclang->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
          src_created = true;
        }
      fbd->addTextElement("src-title-info", "src-lang", str);
    }

  base = book_widget->book_translators->getAuthors();
  if(base.size() > 0 && !src_created)
    {
      fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
      src_created = true;
    }
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addAuthor("src-title-info", *it, FBDCreation::Translator);
    }

  base = book_widget->book_series->getSeries();
  if(base.size() > 0 && !src_created)
    {
      fbd->addTitleInfo(FBDCreation::SrcTitleInfo);
    }
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addSequence("src-title-info", *it);
    }
}

void
MainWindow::createDocumentInfo()
{
  bool src_created = false;

  std::vector<UDBElement> base
      = document_info_widget->document_authors->getAuthors();
  if(base.size() > 0)
    {
      fbd->addTitleInfo(FBDCreation::DocumentInfo);
      src_created = true;
    }
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addAuthor("document-info", *it, FBDCreation::Author);
    }

  std::string str
      = document_info_widget->document_program_used->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::DocumentInfo);
          src_created = true;
        }
      fbd->addTextElement("document-info", "program-used", str);
    }

  str = document_info_widget->document_date->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::DocumentInfo);
          src_created = true;
        }
      fbd->addDate("document-info", str);
    }

  std::vector<QString> model
      = document_info_widget->document_src_urls->getModel();
  if(model.size() > 0)
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::DocumentInfo);
          src_created = true;
        }
    }
  for(auto it = model.begin(); it != model.end(); it++)
    {
      fbd->addTextElement("document-info", "src-url", it->toStdString());
    }

  str = document_info_widget->source_ocr->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::DocumentInfo);
          src_created = true;
        }
      fbd->addTextElement("document-info", "src-ocr", str);
    }

  str = document_info_widget->document_id->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::DocumentInfo);
          src_created = true;
        }
      fbd->addTextElement("document-info", "id", str);
    }

  str = document_info_widget->document_version->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::DocumentInfo);
          src_created = true;
        }
      fbd->addTextElement("document-info", "version", str);
    }

  str = document_info_widget->document_history->toPlainText().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::DocumentInfo);
          src_created = true;
        }
      fbd->addHistory("document-info", str);
    }

  base = document_info_widget->document_publishers->getAuthors();
  if(base.size() > 0 && !src_created)
    {
      fbd->addTitleInfo(FBDCreation::DocumentInfo);
    }
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addAuthor("document-info", *it, FBDCreation::Publisher);
    }
}

void
MainWindow::createPublishInfo()
{
  bool src_created = false;

  std::string str = paper_book_widget->paper_book_name->text().toStdString();
  if(!str.empty())
    {
      fbd->addTitleInfo(FBDCreation::PublishInfo);
      src_created = true;
      fbd->addTextElement("publish-info", "book-name", str);
    }

  str = paper_book_widget->paper_book_publisher->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::PublishInfo);
          src_created = true;
        }
      fbd->addTextElement("publish-info", "publisher", str);
    }

  str = paper_book_widget->paper_book_city->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::PublishInfo);
          src_created = true;
        }
      fbd->addTextElement("publish-info", "city", str);
    }

  str = paper_book_widget->paper_book_year->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::PublishInfo);
          src_created = true;
        }
      fbd->addTextElement("publish-info", "year", str);
    }

  str = paper_book_widget->paper_book_isbn->text().toStdString();
  if(!str.empty())
    {
      if(!src_created)
        {
          fbd->addTitleInfo(FBDCreation::PublishInfo);
          src_created = true;
        }
      fbd->addTextElement("publish-info", "isbn", str);
    }

  std::vector<UDBElement> base
      = paper_book_widget->paper_book_series->getSeries();
  if(base.size() > 0 && !src_created)
    {
      fbd->addTitleInfo(FBDCreation::PublishInfo);
    }
  for(auto it = base.begin(); it != base.end(); it++)
    {
      fbd->addSequence("publish-info", *it);
    }
}

void
MainWindow::finalDialog()
{
  StyledWindow *window = new StyledWindow(this);
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("fbd file has been created!"));
  v_box->addWidget(lab, 0, Qt::AlignCenter);

  QPushButton *close = new QPushButton;
  close->setText(tr("Close"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(close);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  close->setGraphicsEffect(shadow);
  close->setObjectName("ApplyButton");
  connect(close, &QPushButton::clicked, window, &StyledWindow::close);
  v_box->addWidget(close, 0, Qt::AlignCenter);

  window->show();
}