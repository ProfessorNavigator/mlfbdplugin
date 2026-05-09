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

#include <PaperBookWidget.h>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <StyledItemDelegate.h>
#include <StyledWindow.h>
#include <TableView.h>

PaperBookWidget::PaperBookWidget(QWidget *parent) : QScrollArea(parent)
{
  this->setObjectName("ScrollArea");
  this->setWidgetResizable(true);

  createWidget();
}

PaperBookWidget::~PaperBookWidget()
{
  delete paper_book_series;
}

void
PaperBookWidget::createWidget()
{
  QWidget *viewport = new QWidget;
  viewport->setObjectName("ScrollAreaViewport");

  QVBoxLayout *v_box = new QVBoxLayout;
  viewport->setLayout(v_box);

  QGridLayout *grid = new QGridLayout;
  v_box->addLayout(grid);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Paper book name:"));
  grid->addWidget(lab, 0, 0, Qt::AlignLeft);

  paper_book_name = new QLineEdit;
  paper_book_name->setObjectName("LineEdit");
  grid->addWidget(paper_book_name, 1, 0);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Paper book publisher:"));
  grid->addWidget(lab, 0, 1, Qt::AlignLeft);

  paper_book_publisher = new QLineEdit;
  paper_book_publisher->setObjectName("LineEdit");
  grid->addWidget(paper_book_publisher, 1, 1);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("City:"));
  grid->addWidget(lab, 2, 0, Qt::AlignLeft);

  paper_book_city = new QLineEdit;
  paper_book_city->setObjectName("LineEdit");
  grid->addWidget(paper_book_city, 3, 0);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Year:"));
  grid->addWidget(lab, 2, 1, Qt::AlignLeft);

  paper_book_year = new QLineEdit;
  paper_book_year->setObjectName("LineEdit");
  grid->addWidget(paper_book_year, 3, 1);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText("ISBN:");
  grid->addWidget(lab, 4, 0, Qt::AlignLeft);

  paper_book_isbn = new QLineEdit;
  paper_book_isbn->setObjectName("LineEdit");
  grid->addWidget(paper_book_isbn, 5, 0, 1, 2);

  v_box->addWidget(seriesSection());

  v_box->addStretch();

  this->setWidget(viewport);
}

QWidget *
PaperBookWidget::seriesSection()
{
  QFrame *frame = new QFrame;
  frame->setObjectName("Frame");
  frame->setFrameShape(QFrame::Box);

  QVBoxLayout *v_box = new QVBoxLayout;
  frame->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Paper book series:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  paper_book_series = new SeriesModel(nullptr);

  TableView *table = new TableView;
  table->setObjectName("Table");
  table->viewport()->setObjectName("TableViewport");
  QAbstractItemDelegate *delegate = table->itemDelegate();
  StyledItemDelegate *item_delegate = new StyledItemDelegate(table);
  table->setItemDelegate(item_delegate);
  delete delegate;
  QAbstractItemModel *prev = table->model();
  table->setModel(paper_book_series);
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
  connect(add_series_act, &QAction::triggered, this,
          &PaperBookWidget::addSeries);
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
                  paper_book_series->removeSeries(index);
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

void
PaperBookWidget::addSeries()
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

              paper_book_series->addSeries(sequence);

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
