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

#include <Algorithm.h>
#include <BookWidget.h>
#include <ByteOrder.h>
#include <GenreView.h>
#include <QFileDialog>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QImage>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <StyledWindow.h>
#include <TableView.h>
#include <filesystem>
#include <fstream>
#include <iostream>

BookWidget::BookWidget(QWidget *parent, const Bases &bases,
                       const WidgetType &wt)
    : QScrollArea(parent)
{
  this->bases = bases;
  this->wt = wt;

  this->setObjectName("ScrollArea");
  uint32_t val;
  unsigned char *ptr = reinterpret_cast<unsigned char *>(&val);
  for(unsigned char i = 0; i < sizeof(val); i++)
    {
      ptr[i] = i;
    }

  ByteOrder bo;
  bo.setLittle(val);

  uint32_t check = bo;
  if(check != val)
    {
      need_byte_conversion = true;
    }
  createWidget();
}

BookWidget::~BookWidget()
{
  delete book_authors;
  delete book_series;
  delete book_genres;
  delete book_translators;
}

void
BookWidget::createWidget()
{
  this->setWidgetResizable(true);

  QWidget *viewport = new QWidget;
  viewport->setObjectName("ScrollAreaViewport");

  QVBoxLayout *v_box = new QVBoxLayout;
  viewport->setLayout(v_box);

  QGridLayout *grid = new QGridLayout;
  v_box->addLayout(grid);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Book title:"));
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  book_title = new QLineEdit;
  book_title->setObjectName("LineEdit");
  if(wt == WidgetType::Book)
    {
      book_title->setPlaceholderText(tr("Mandatory"));
    }
  grid->addWidget(book_title, 1, 0);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Book language:"));
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  book_language = new QLineEdit;
  book_language->setObjectName("LineEdit");
  if(wt == WidgetType::Book)
    {
      book_language->setPlaceholderText(tr("Mandatory"));
    }
  grid->addWidget(book_language, 1, 1);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Book creation date:"));
  grid->addWidget(lab, 2, 0, Qt::AlignLeft);

  book_date = new QLineEdit;
  book_date->setObjectName("LineEdit");
  grid->addWidget(book_date, 3, 0);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Source language:"));
  grid->addWidget(lab, 2, 1, Qt::AlignLeft);

  book_srclang = new QLineEdit;
  book_srclang->setObjectName("LineEdit");
  grid->addWidget(book_srclang, 3, 1);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Keywords:"));
  grid->addWidget(lab, 4, 0, Qt::AlignLeft);

  book_keywords = new QLineEdit;
  book_keywords->setObjectName("LineEdit");
  grid->addWidget(book_keywords, 5, 0, 1, 2);

  v_box->addWidget(authorsSection(AuthorType::Author));

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  h_box->addStretch();
  h_box->addWidget(seriesSection());
  h_box->addWidget(genresSection());
  h_box->addStretch();

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Annotation:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  book_annotation = new QTextEdit;
  book_annotation->setObjectName("TextEdit");
  book_annotation->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(book_annotation, &QTextEdit::customContextMenuRequested, this,
          [this](const QPoint &pos)
            {
              QMenu *menu = book_annotation->createStandardContextMenu(pos);
              menu->setAttribute(Qt::WA_DeleteOnClose);
              menu->setObjectName("Menu");
              menu->popup(book_annotation->viewport()->mapToGlobal(pos));
            });
  v_box->addWidget(book_annotation);

  v_box->addWidget(coverSection());

  v_box->addWidget(authorsSection(AuthorType::Translator));

  v_box->addStretch();

  this->setWidget(viewport);
}

QWidget *
BookWidget::authorsSection(const AuthorType &type)
{
  QFrame *frame = new QFrame;
  frame->setObjectName("Frame");
  frame->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  frame->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  if(type == AuthorType::Author)
    {
      lab->setText(tr("Book authors:"));
    }
  else
    {
      lab->setText(tr("Book translators:"));
    }
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  if(type == AuthorType::Author)
    {
      book_authors = new AuthorsModel(nullptr);
      book_authors->setEditable(true);
    }
  else
    {
      book_translators = new AuthorsModel(nullptr);
      book_translators->setEditable(true);
    }

  TableView *authors_table = new TableView;
  authors_table->setObjectName("Table");
  authors_table->viewport()->setObjectName("TableViewport");
  QAbstractItemModel *prev = authors_table->model();
  if(type == AuthorType::Author)
    {
      authors_table->setModel(book_authors);
    }
  else
    {
      authors_table->setModel(book_translators);
    }
  QHeaderView *hh = authors_table->horizontalHeader();
  for(int i = 0; i < hh->count(); i++)
    {
      hh->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    }
  delete prev;
  authors_table->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(authors_table, &TableView::customContextMenuRequested,
          [authors_table](const QPoint &pos)
            {
              QModelIndex index = authors_table->indexAt(pos);
              authors_table->setCurrentIndex(index);

              QMenu *menu = new QMenu(authors_table);
              menu->setAttribute(Qt::WA_DeleteOnClose);
              menu->setObjectName("Menu");
              menu->addActions(authors_table->actions());
              menu->popup(authors_table->viewport()->mapToGlobal(pos));
            });
  connect(authors_table, &TableView::signalLeftMouseButton,
          [authors_table](const QPoint &global)
            {
              authors_table->setCurrentIndex(authors_table->indexAt(
                  authors_table->viewport()->mapFromGlobal(global)));
            });
  v_box->addWidget(authors_table);

  QAction *add_author_act = new QAction(tr("Add author"));
  connect(add_author_act, &QAction::triggered, this,
          [type, this]
            {
              if(type == AuthorType::Author)
                {
                  addAuthor(book_authors);
                }
              else
                {
                  addAuthor(book_translators);
                }
            });
  QList<QAction *> actions;
  actions.append(add_author_act);

  QAction *edit_selected_act = new QAction(tr("Edit"));
  connect(edit_selected_act, &QAction::triggered, authors_table,
          [authors_table]
            {
              authors_table->edit(authors_table->currentIndex());
            });
  actions.append(edit_selected_act);

  QAction *remove_selected_act = new QAction(tr("Remove"));
  connect(remove_selected_act, &QAction::triggered, this,
          [this, authors_table, type]
            {
              QModelIndex index = authors_table->currentIndex();
              if(index.isValid())
                {
                  if(type == AuthorType::Author)
                    {
                      book_authors->removeAuthor(index);
                    }
                  else
                    {
                      book_translators->removeAuthor(index);
                    }
                }
            });
  actions.append(remove_selected_act);

  authors_table->addActions(actions);

  connect(authors_table, &TableView::destroyed,
          [actions]
            {
              for(qsizetype i = 0; i < actions.size(); i++)
                {
                  delete actions[i];
                }
            });

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add_author = new QPushButton;
  if(type == AuthorType::Author)
    {
      add_author->setText(tr("Add author"));
    }
  else
    {
      add_author->setText(tr("Add translator"));
    }
  QGraphicsDropShadowEffect *shadow
      = new QGraphicsDropShadowEffect(add_author);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  add_author->setGraphicsEffect(shadow);
  add_author->setObjectName("ApplyButton");
  connect(add_author, &QPushButton::clicked, add_author_act,
          &QAction::trigger);
  h_box->addWidget(add_author, 0, Qt::AlignCenter);

  QPushButton *edit_selected = new QPushButton;
  edit_selected->setText(tr("Edit selected"));
  shadow = new QGraphicsDropShadowEffect(edit_selected);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  edit_selected->setGraphicsEffect(shadow);
  edit_selected->setObjectName("ApplyButton");
  connect(edit_selected, &QPushButton::clicked, edit_selected_act,
          &QAction::trigger);
  h_box->addWidget(edit_selected, 0, Qt::AlignCenter);

  QPushButton *remove_selected = new QPushButton;
  remove_selected->setText(tr("Remove selected"));
  shadow = new QGraphicsDropShadowEffect(remove_selected);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  remove_selected->setGraphicsEffect(shadow);
  remove_selected->setObjectName("ClearButton");
  connect(remove_selected, &QPushButton::clicked, remove_selected_act,
          &QAction::trigger);
  h_box->addWidget(remove_selected, 0, Qt::AlignCenter);

  return frame;
}

QWidget *
BookWidget::seriesSection()
{
  QFrame *frame = new QFrame;
  frame->setObjectName("Frame");
  frame->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  frame->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Book series:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  book_series = new SeriesModel(nullptr);

  TableView *table = new TableView;
  table->setObjectName("Table");
  table->viewport()->setObjectName("TableViewport");
  QAbstractItemModel *prev = table->model();
  table->setModel(book_series);
  delete prev;
  connect(table, &TableView::signalResized,
          [table](const QSize &sz)
            {
              QHeaderView *hh = table->horizontalHeader();
              hh->resizeSection(0, sz.width() * 0.5);
              hh->resizeSection(1, sz.width() - hh->sectionSize(0));
            });
  table->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(table, &TableView::customContextMenuRequested,
          [table](const QPoint &pos)
            {
              table->setCurrentIndex(table->indexAt(pos));
              QMenu *menu = new QMenu(table);
              menu->setAttribute(Qt::WA_DeleteOnClose);
              menu->setObjectName("Menu");
              menu->addActions(table->actions());
              menu->popup(table->viewport()->mapToGlobal(pos));
            });
  connect(table, &TableView::signalLeftMouseButton,
          [table](const QPoint &global)
            {
              table->setCurrentIndex(
                  table->indexAt(table->viewport()->mapFromGlobal(global)));
            });
  v_box->addWidget(table, 0, Qt::AlignCenter);

  QAction *add_series_act = new QAction("Add series");
  connect(add_series_act, &QAction::triggered, this, &BookWidget::addSeries);
  QList<QAction *> actions;
  actions.append(add_series_act);

  QAction *edit_act = new QAction("Edit");
  connect(edit_act, &QAction::triggered, table,
          [table]
            {
              table->edit(table->currentIndex());
            });
  actions.append(edit_act);

  QAction *remove_act = new QAction("Remove");
  connect(remove_act, &QAction::triggered, this,
          [this, table]
            {
              QModelIndex index = table->currentIndex();
              if(index.isValid())
                {
                  book_series->removeSeries(index);
                }
              table->setCurrentIndex(QModelIndex());
            });
  actions.append(remove_act);

  table->addActions(actions);

  connect(table, &TableView::destroyed,
          [actions]
            {
              for(qsizetype i = 0; i < actions.size(); i++)
                {
                  delete actions[i];
                }
            });

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add_series = new QPushButton;
  add_series->setText(tr("Add series"));
  QGraphicsDropShadowEffect *shadow
      = new QGraphicsDropShadowEffect(add_series);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  add_series->setGraphicsEffect(shadow);
  add_series->setObjectName("ApplyButton");
  connect(add_series, &QPushButton::clicked, add_series_act,
          &QAction::trigger);
  h_box->addWidget(add_series, 0, Qt::AlignCenter);

  QPushButton *edit_selected = new QPushButton;
  edit_selected->setText(tr("Edit selected"));
  shadow = new QGraphicsDropShadowEffect(edit_selected);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  edit_selected->setGraphicsEffect(shadow);
  edit_selected->setObjectName("ApplyButton");
  connect(edit_selected, &QPushButton::clicked, edit_act, &QAction::trigger);
  h_box->addWidget(edit_selected, 0, Qt::AlignCenter);

  QPushButton *remove_selected = new QPushButton;
  remove_selected->setText(tr("Remove selected"));
  shadow = new QGraphicsDropShadowEffect(remove_selected);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  remove_selected->setGraphicsEffect(shadow);
  remove_selected->setObjectName("ClearButton");
  connect(remove_selected, &QPushButton::clicked, remove_act,
          &QAction::trigger);
  h_box->addWidget(remove_selected, 0, Qt::AlignCenter);

  return frame;
}

QWidget *
BookWidget::genresSection()
{
  QFrame *frame = new QFrame;
  frame->setObjectName("Frame");
  frame->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  frame->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Book genres:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  book_genres = new GenreEditModel(nullptr, bases.genres_base);

  TableView *table = new TableView;
  table->setObjectName("Table");
  table->viewport()->setObjectName("TableViewport");
  QAbstractItemModel *prev = table->model();
  table->setModel(book_genres);
  delete prev;
  connect(table, &TableView::signalResized,
          [table](const QSize &sz)
            {
              QHeaderView *hh = table->horizontalHeader();
              hh->resizeSection(0, sz.width() * 0.5);
              hh->resizeSection(1, sz.width() - hh->sectionSize(0));
            });
  connect(table, &TableView::signalLeftMouseButton,
          [table](const QPoint &global)
            {
              table->setCurrentIndex(
                  table->indexAt(table->viewport()->mapFromGlobal(global)));
            });
  table->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(table, &TableView::customContextMenuRequested,
          [table](const QPoint &pos)
            {
              table->setCurrentIndex(table->indexAt(pos));
              QMenu *menu = new QMenu(table);
              menu->setAttribute(Qt::WA_DeleteOnClose);
              menu->setObjectName("Menu");
              menu->addActions(table->actions());
              menu->popup(table->viewport()->mapToGlobal(pos));
            });
  v_box->addWidget(table);

  QAction *add_genre_act = new QAction(tr("Add genre"));
  connect(add_genre_act, &QAction::triggered, this, &BookWidget::addGenre);
  QList<QAction *> actions;
  actions.append(add_genre_act);

  QAction *remove_act = new QAction(tr("Remove"));
  connect(remove_act, &QAction::triggered, this,
          [this, table]
            {
              QModelIndex index = table->currentIndex();
              if(index.isValid())
                {
                  book_genres->removeGenre(index);
                }
            });
  actions.append(remove_act);

  table->addActions(actions);
  connect(table, &TableView::destroyed,
          [actions]
            {
              for(qsizetype i = 0; i < actions.size(); i++)
                {
                  delete actions[i];
                }
            });

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add_genre = new QPushButton;
  add_genre->setText(tr("Add genre"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(add_genre);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  add_genre->setGraphicsEffect(shadow);
  add_genre->setObjectName("ApplyButton");
  connect(add_genre, &QPushButton::clicked, add_genre_act, &QAction::trigger);
  h_box->addWidget(add_genre, 0, Qt::AlignCenter);

  QPushButton *remove_selected = new QPushButton;
  remove_selected->setText(tr("Remove selected"));
  shadow = new QGraphicsDropShadowEffect(remove_selected);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  remove_selected->setGraphicsEffect(shadow);
  remove_selected->setObjectName("ClearButton");
  connect(remove_selected, &QPushButton::clicked, remove_act,
          &QAction::trigger);
  h_box->addWidget(remove_selected, 0, Qt::AlignCenter);

  return frame;
}

QWidget *
BookWidget::coverSection()
{
  QFrame *frame = new QFrame;
  frame->setObjectName("Frame");
  frame->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  frame->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Cover:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  cover_widget = new QLabel;
  cover_widget->setAlignment(Qt::AlignCenter);
  v_box->addWidget(cover_widget);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *open = new QPushButton;
  open->setText(tr("Open"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(open);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  open->setGraphicsEffect(shadow);
  open->setObjectName("ApplyButton");
  connect(open, &QPushButton::clicked, this, &BookWidget::openCover);
  h_box->addWidget(open, 0, Qt::AlignCenter);

  QPushButton *clear = new QPushButton;
  clear->setText(tr("Clear"));
  shadow = new QGraphicsDropShadowEffect(clear);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  clear->setGraphicsEffect(shadow);
  clear->setObjectName("ClearButton");
  connect(clear, &QPushButton::clicked, this,
          [this]
            {
              book_cover = Magick::Image();
              cover_widget->clear();
              cover_widget->resize(cover_widget->sizeHint());
            });
  h_box->addWidget(clear, 0, Qt::AlignCenter);

  return frame;
}

void
BookWidget::addAuthor(AuthorsModel *model)
{
  StyledWindow *window = new StyledWindow(this->window());
  window->setObjectName("Window");
  if(model == book_authors)
    {
      window->setWindowTitle(tr("Author editor"));
    }
  else
    {
      window->setWindowTitle(tr("Translator editor"));
    }
  window->setWindowModality(Qt::WindowModality::WindowModal);

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Surname:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *surname = new QLineEdit;
  surname->setObjectName("LineEdit");
  v_box->addWidget(surname);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Name:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *name = new QLineEdit;
  name->setObjectName("LineEdit");
  v_box->addWidget(name);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Second name:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *sec_name = new QLineEdit;
  sec_name->setObjectName("LineEdit");
  v_box->addWidget(sec_name);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Nickname:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *nickname = new QLineEdit;
  nickname->setObjectName("LineEdit");
  v_box->addWidget(nickname);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Home page:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *home_page = new QLineEdit;
  home_page->setObjectName("LineEdit");
  v_box->addWidget(home_page);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("e-mail:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *e_mail = new QLineEdit;
  e_mail->setObjectName("LineEdit");
  v_box->addWidget(e_mail);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("ID:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *id = new QLineEdit;
  id->setObjectName("LineEdit");
  v_box->addWidget(id);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add = new QPushButton;
  add->setText(tr("Add"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(add);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  add->setGraphicsEffect(shadow);
  add->setObjectName("ApplyButton");
  connect(
      add, &QPushButton::clicked, this,
      [model, window, surname, name, sec_name, nickname, home_page, e_mail, id]
        {
          BaseID bid;
          UDBElement author;
          bid.setId(author, BaseID::Author);

          UDBElement el;
          bid.setId(el, BaseID::LastName);
          el.content = surname->text().toStdString();
          author.subelements.emplace_back(el);

          el = UDBElement();
          bid.setId(el, BaseID::FirstName);
          el.content = name->text().toStdString();
          author.subelements.emplace_back(el);

          el = UDBElement();
          bid.setId(el, BaseID::MiddleName);
          el.content = sec_name->text().toStdString();
          author.subelements.emplace_back(el);

          el = UDBElement();
          bid.setId(el, BaseID::Nickname);
          el.content = nickname->text().toStdString();
          author.subelements.emplace_back(el);

          el = UDBElement();
          bid.setId(el, BaseID::HomePage);
          el.content = home_page->text().toStdString();
          author.subelements.emplace_back(el);

          el = UDBElement();
          bid.setId(el, BaseID::EMail);
          el.content = e_mail->text().toStdString();
          author.subelements.emplace_back(el);

          el = UDBElement();
          bid.setId(el, BaseID::AuthorID);
          el.content = id->text().toStdString();
          author.subelements.emplace_back(el);

          model->addAuthor(author);

          window->close();
        });
  h_box->addWidget(add, 0, Qt::AlignCenter);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  shadow = new QGraphicsDropShadowEffect(cancel);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  cancel->setGraphicsEffect(shadow);
  cancel->setObjectName("CancelButton");
  connect(cancel, &QPushButton::clicked, window, &QWidget::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);

  window->show();
}

void
BookWidget::addSeries()
{
  StyledWindow *window = new StyledWindow(this->window());
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Series name:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *series_name = new QLineEdit;
  series_name->setObjectName("LineEdit");
  v_box->addWidget(series_name);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Series book number:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *series_number = new QLineEdit;
  series_number->setObjectName("LineEdit");
  v_box->addWidget(series_number);

  QHBoxLayout *h_box = new QHBoxLayout;
  v_box->addLayout(h_box);

  QPushButton *add = new QPushButton;
  add->setText(tr("Add"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(add);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  add->setGraphicsEffect(shadow);
  add->setObjectName("ApplyButton");
  connect(add, &QPushButton::clicked, this,
          [this, window, series_name, series_number]
            {
              std::string str = series_name->text().toStdString();
              if(str.empty())
                {
                  return void();
                }
              BaseID bid;
              UDBElement sequence;
              bid.setId(sequence, BaseID::Sequence);

              UDBElement el;
              bid.setId(el, BaseID::SequenceName);
              el.content = str;
              sequence.subelements.emplace_back(el);

              el = UDBElement();
              bid.setId(el, BaseID::SequenceNumber);
              el.content = series_number->text().toStdString();
              if(!el.content.empty())
                {
                  sequence.subelements.emplace_back(el);
                }

              book_series->addSeries(sequence);

              window->close();
            });
  h_box->addWidget(add, 0, Qt::AlignCenter);

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Cancel"));
  shadow = new QGraphicsDropShadowEffect(cancel);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  cancel->setGraphicsEffect(shadow);
  cancel->setObjectName("CancelButton");
  connect(cancel, &QPushButton::clicked, window, &QWidget::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);

  window->show();
}

void
BookWidget::addGenre()
{
  StyledWindow *window = new StyledWindow(this->window());
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  GenreView *view = new GenreView(nullptr, bases.genres_base);
  view->setObjectName("Table");
  connect(view, &GenreView::signalGenreSelected, this,
          [this, window](const QModelIndex &index)
            {
              if(!index.parent().isValid())
                {
                  return void();
                }
              const UDBElement *el = reinterpret_cast<const UDBElement *>(
                  index.constInternalPointer());
              if(el == nullptr)
                {
                  return void();
                }
              book_genres->addGenre(el->content);
              window->close();
            });
  v_box->addWidget(view);

  window->show();
}

void
BookWidget::openCover()
{
  QFileDialog *fd = new QFileDialog(this->window());
  fd->setAttribute(Qt::WA_DeleteOnClose);
  fd->setWindowModality(Qt::WindowModal);

  fd->setAcceptMode(QFileDialog::AcceptOpen);
  fd->setFileMode(QFileDialog::ExistingFile);
  fd->setDirectory(QDir::homePath());

  std::vector<Magick::CoderInfo> image_formats;
  try
    {
      Magick::coderInfoList(&image_formats, Magick::CoderInfo::TrueMatch,
                            Magick::CoderInfo::AnyMatch);
    }
  catch(Magick::Exception &er)
    {
      std::cout << "BookWidget::openCover: \"" << er.what() << "\""
                << std::endl;
    }

  Algorithm alg;
  image_formats.erase(alg.parallelRemoveIf(image_formats.begin(),
                                           image_formats.end(),
                                           [](const Magick::CoderInfo &el)
                                             {
                                               return el.mimeType().empty();
                                             }),
                      image_formats.end());

  std::vector<Magick::CoderInfo>::iterator end = image_formats.end();
  for(auto it = image_formats.begin(); it < end; it++)
    {
      std::string str = it->mimeType();
      end = alg.parallelRemoveIf(it + 1, end,
                                 [str](const Magick::CoderInfo &el)
                                   {
                                     return el.mimeType() == str;
                                   });
    }
  image_formats.erase(end, image_formats.end());

  QStringList filters;
  for(auto it = image_formats.begin(); it != image_formats.end(); it++)
    {
      filters.append(it->mimeType().c_str());
    }

  QString def_filter;
  for(qsizetype i = 0; i < filters.size(); i++)
    {
      if(!def_filter.isEmpty())
        {
          def_filter += ", ";
        }
      def_filter += filters[i];
    }
  filters.insert(0, def_filter);
  fd->setMimeTypeFilters(filters);

  connect(fd, &QFileDialog::fileSelected, this, &BookWidget::setCover);

  fd->show();
}

void
BookWidget::setCover(const QString &file)
{
  std::string str = file.toStdString();
  std::filesystem::path p = std::u8string(str.begin(), str.end());
  std::fstream f;
  f.open(p, std::ios_base::in | std::ios_base::binary);
  if(!f.is_open())
    {
      return void();
    }
  str.clear();
  f.seekg(0, std::ios_base::end);
  str.resize(static_cast<size_t>(f.tellg()));
  f.seekg(0, std::ios_base::beg);
  f.read(str.data(), str.size());
  f.close();

  Magick::Blob blob(str.data(), str.size());
  size_t buf_sz;
  unsigned char *buf = nullptr;
  try
    {
      book_cover = Magick::Image(blob);
      buf_sz = book_cover.columns() * book_cover.rows() * 4;
      buf = new unsigned char[buf_sz];
      book_cover.write(0, 0, book_cover.columns(), book_cover.rows(), "RGBA",
                       Magick::CharPixel, buf);
    }
  catch(Magick::Exception &er)
    {
      std::cout << "BookWidget::setCover: \"" << er.what() << "\""
                << std::endl;
      delete[] buf;
      return void();
    }

  if(need_byte_conversion)
    {
#pragma omp parallel for
      for(size_t i = 0; i < buf_sz; i += 4)
        {
          uint32_t val;
          unsigned char *ptr = reinterpret_cast<unsigned char *>(&val);
          for(size_t j = 0; j < 4; j++)
            {
              ptr[j] = buf[i + j];
            }
          ByteOrder bo;
          bo.setLittle(val);
          val = bo;
          for(size_t j = 0; j < 4; j++)
            {
              buf[i + j] = ptr[j];
            }
        }
    }

  QImage image(
      buf, static_cast<int>(book_cover.columns()),
      static_cast<int>(book_cover.rows()), QImage::Format_RGBA8888,
      [](void *info)
        {
          unsigned char *buf = reinterpret_cast<unsigned char *>(info);
          delete[] buf;
        },
      buf);

  QPixmap map;
  map.convertFromImage(image);
  cover_widget->setPixmap(map);
}
