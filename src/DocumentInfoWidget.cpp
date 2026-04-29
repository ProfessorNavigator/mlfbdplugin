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

#include <Bases.h>
#include <DocumentInfoWidget.h>
#include <QDateTime>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <StyledWindow.h>
#include <TableView.h>

DocumentInfoWidget::DocumentInfoWidget(QWidget *parent, const Bases &bases)
    : QScrollArea(parent)
{
  this->bases = bases;

  this->setObjectName("ScrollArea");
  this->setWidgetResizable(true);
  createWidget();
}

DocumentInfoWidget::~DocumentInfoWidget()
{
  delete document_authors;
  delete document_src_urls;
  delete document_publishers;
}

void
DocumentInfoWidget::createWidget()
{
  QWidget *viewport = new QWidget;
  viewport->setObjectName("ScrollAreaViewport");

  QVBoxLayout *v_box = new QVBoxLayout;
  viewport->setLayout(v_box);

  v_box->addWidget(authorsSection(AuthorType::Author));

  QGridLayout *grid = new QGridLayout;
  v_box->addLayout(grid);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Program used:"));
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  document_program_used = new QLineEdit;
  document_program_used->setObjectName("LineEdit");
  document_program_used->setText("MLFBDPlugin 2.0");
  grid->addWidget(document_program_used, 1, 0);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Creation date:"));
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  document_date = new QLineEdit;
  document_date->setObjectName("LineEdit");
  QDateTime dtm = QDateTime::currentDateTimeUtc();
  document_date->setText(dtm.toString());
  grid->addWidget(document_date, 1, 1);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Author of the original (online) document:"));
  grid->addWidget(lab, 2, 0, Qt::AlignLeft);

  source_ocr = new QLineEdit;
  source_ocr->setObjectName("LineEdit");
  grid->addWidget(source_ocr, 3, 0);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Document ID:"));
  grid->addWidget(lab, 2, 1, Qt::AlignLeft);

  document_id = new QLineEdit;
  document_id->setObjectName("LineEdit");
  QString str;
  for(int i = 0; i < 4; i++)
    {
      uint32_t val = bases.mlbp->randomNumber<uint32_t>();
      if(!str.isEmpty())
        {
          str += "-";
        }
      str += toHex(reinterpret_cast<unsigned char *>(&val), sizeof(val))
                 .c_str();
    }
  document_id->setText(str);
  grid->addWidget(document_id, 3, 1);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Document version:"));
  grid->addWidget(lab, 4, 0, Qt::AlignLeft);

  document_version = new QLineEdit;
  document_version->setObjectName("LineEdit");
  document_version->setText("1.0");
  grid->addWidget(document_version, 5, 0);

  v_box->addWidget(sourceUrlsSection());

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Document history:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  document_history = new QTextEdit;
  document_history->setObjectName("TextEdit");
  str = "1.0 - ";
  str += tr("Document creation");
  str += "\n";
  document_history->setPlainText(str);
  v_box->addWidget(document_history);

  v_box->addWidget(authorsSection(AuthorType::Publisher));

  v_box->addStretch();

  this->setWidget(viewport);
}

QWidget *
DocumentInfoWidget::authorsSection(const AuthorType &type)
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
      lab->setText(tr("Document authors:"));
    }
  else
    {
      lab->setText(tr("Document publishers:"));
    }
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  if(type == AuthorType::Author)
    {
      document_authors = new AuthorsModel(nullptr);
      document_authors->setEditable(true);
    }
  else
    {
      document_publishers = new AuthorsModel(nullptr);
      document_publishers->setEditable(true);
    }

  TableView *table = new TableView;
  table->setObjectName("Table");
  table->viewport()->setObjectName("TableViewport");
  QAbstractItemModel *prev = table->model();
  if(type == AuthorType::Author)
    {
      table->setModel(document_authors);
    }
  else
    {
      table->setModel(document_publishers);
    }
  QHeaderView *hh = table->horizontalHeader();
  for(int i = 0; i < hh->count(); i++)
    {
      hh->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    }
  delete prev;
  table->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(table, &TableView::customContextMenuRequested,
          [table](const QPoint &pos)
            {
              QModelIndex index = table->indexAt(pos);
              table->setCurrentIndex(index);

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
  v_box->addWidget(table);

  QAction *add_author_act = new QAction(tr("Add author"));
  connect(add_author_act, &QAction::triggered, this,
          [this, table]
            {
              addAuthor(dynamic_cast<AuthorsModel *>(table->model()));
            });
  QList<QAction *> actions;
  actions.append(add_author_act);

  QAction *edit_selected_act = new QAction(tr("Edit"));
  connect(edit_selected_act, &QAction::triggered, table,
          [table]
            {
              table->edit(table->currentIndex());
            });
  actions.append(edit_selected_act);

  QAction *remove_selected_act = new QAction(tr("Remove"));
  connect(remove_selected_act, &QAction::triggered, this,
          [this, table, type]
            {
              QModelIndex index = table->currentIndex();
              if(index.isValid())
                {
                  if(type == AuthorType::Author)
                    {
                      document_authors->removeAuthor(index);
                    }
                  else
                    {
                      document_publishers->removeAuthor(index);
                    }
                }
            });
  actions.append(remove_selected_act);

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

  QPushButton *add_author = new QPushButton;
  if(type == AuthorType::Author)
    {
      add_author->setText(tr("Add author"));
    }
  else
    {
      add_author->setText(tr("Add publisher"));
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
DocumentInfoWidget::sourceUrlsSection()
{
  QFrame *frame = new QFrame;
  frame->setObjectName("Frame");
  frame->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  frame->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Document source URLs:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  document_src_urls = new SourceUrlsModel;

  TableView *table = new TableView;
  table->setObjectName("Table");
  table->viewport()->setObjectName("TableViewport");
  QAbstractItemModel *prev = table->model();
  table->setModel(document_src_urls);
  delete prev;
  connect(table, &TableView::signalResized,
          [table](const QSize &sz)
            {
              QHeaderView *hh = table->horizontalHeader();
              hh->resizeSection(0, sz.width());
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

  QAction *add_act = new QAction(tr("Add URL"));
  connect(add_act, &QAction::triggered, this, &DocumentInfoWidget::addUrl);
  QList<QAction *> actions;
  actions.append(add_act);

  QAction *edit_act = new QAction(tr("Edit"));
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
                  document_src_urls->removeUrl(index);
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

  QPushButton *add_url = new QPushButton;
  add_url->setText(tr("Add URL"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(add_url);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  add_url->setGraphicsEffect(shadow);
  add_url->setObjectName("ApplyButton");
  connect(add_url, &QPushButton::clicked, add_act, &QAction::trigger);
  h_box->addWidget(add_url, 0, Qt::AlignCenter);

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
  remove_selected->setObjectName("CancelButton");
  connect(remove_selected, &QPushButton::clicked, remove_act,
          &QAction::trigger);
  h_box->addWidget(remove_selected, 0, Qt::AlignCenter);

  return frame;
}

void
DocumentInfoWidget::addAuthor(AuthorsModel *model)
{
  StyledWindow *window = new StyledWindow(this->window());
  window->setObjectName("Window");
  if(model == document_authors)
    {
      window->setWindowTitle(tr("Author editor"));
    }
  else
    {
      window->setWindowTitle(tr("Publisher editor"));
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
DocumentInfoWidget::addUrl()
{
  StyledWindow *window = new StyledWindow(this->window());
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("URL:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  QLineEdit *url = new QLineEdit;
  url->setObjectName("LineEdit");
  v_box->addWidget(url);

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
          [this, window, url]
            {
              document_src_urls->addUrl(url->text());
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
  connect(cancel, &QPushButton::clicked, window, &StyledWindow::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);

  window->show();
}

std::string
DocumentInfoWidget::toHex(unsigned char *buf, const size_t &buf_sz)
{
  std::string result;

  for(unsigned char *i = buf; i < buf + buf_sz; i++)
    {
      std::string loc;
      uint8_t val = *i;
      while(val > 0)
        {
          uint8_t rem = val % 16;
          val /= 16;
          if(rem < 10)
            {
              loc.insert(loc.begin(), rem + 48);
            }
          else
            {
              loc.insert(loc.begin(), rem + 87);
            }
        }
      while(loc.size() < 2)
        {
          loc.insert(loc.begin(), '0');
        }
      result += loc;
    }

  result.shrink_to_fit();

  return result;
}
