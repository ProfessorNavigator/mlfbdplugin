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

#include <QColorDialog>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QStyleOption>
#include <QVBoxLayout>
#include <SettingsWindow.h>
#include <StyledWindow.h>

SettingsWindow::SettingsWindow(
    QWidget *parent, const std::shared_ptr<SettingsManager> &settings)
    : QWidget(parent)
{
  this->settings = settings;
  settings_copy = std::make_shared<SettingsManager>(*settings);

  this->setWindowFlag(Qt::Window);
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle(tr("Settings"));
  this->setWindowModality(Qt::WindowModal);

  this->setObjectName("Window");
}

void
SettingsWindow::createWindow()
{
  QVBoxLayout *v_box = new QVBoxLayout;
  this->setLayout(v_box);

  QWidget *w = new QWidget;
  w->setObjectName("ScrollAreaViewport");
  QVBoxLayout *w_layout = new QVBoxLayout;
  w->setLayout(w_layout);

  QScrollArea *scrl = new QScrollArea;
  scrl->setObjectName("ScrollArea");
  scrl->setWidgetResizable(true);
  v_box->addWidget(scrl);

  w_layout->addWidget(tabWidget());

  scrl->setWidget(w);

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
  connect(apply, &QPushButton::clicked, this,
          [this]
            {
              *settings = *settings_copy;
              settings->applySettings();
            });
  h_box->addWidget(apply, 0, Qt::AlignCenter);

  QPushButton *apply_close = new QPushButton;
  apply_close->setText(tr("Apply and close"));
  shadow = new QGraphicsDropShadowEffect(apply_close);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  apply_close->setGraphicsEffect(shadow);
  apply_close->setObjectName("ApplyButton");
  connect(apply_close, &QPushButton::clicked, this,
          [this]
            {
              *settings = *settings_copy;
              settings->applySettings();
              this->close();
            });
  h_box->addWidget(apply_close, 0, Qt::AlignCenter);

  QPushButton *reset_to_default = new QPushButton;
  reset_to_default->setText(tr("Reset to default"));
  shadow = new QGraphicsDropShadowEffect(reset_to_default);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  reset_to_default->setGraphicsEffect(shadow);
  reset_to_default->setObjectName("ClearButton");
  connect(reset_to_default, &QPushButton::clicked, this,
          [this]
            {
              resetToDafaultDialog();
            });
  h_box->addWidget(reset_to_default, 0, Qt::AlignCenter);  

  QPushButton *cancel = new QPushButton;
  cancel->setText(tr("Close"));
  shadow = new QGraphicsDropShadowEffect(cancel);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  cancel->setGraphicsEffect(shadow);
  cancel->setObjectName("CancelButton");
  connect(cancel, &QPushButton::clicked, this, &SettingsWindow::close);
  h_box->addWidget(cancel, 0, Qt::AlignCenter);  
}

QWidget *
SettingsWindow::tabWidget()
{
  QFrame *frame = new QFrame;
  frame->setObjectName("Frame");
  frame->setFrameShape(QFrame::Box);

  QGridLayout *grid = new QGridLayout;
  grid->setColumnStretch(1, 1);
  frame->setLayout(grid);

  int row = 0;
  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  QFont font = lab->font();
  font.setBold(true);
  lab->setFont(font);
  lab->setText(tr("Tab widgets style"));
  grid->addWidget(lab, row, 0, 1, 2, Qt::AlignCenter);
  row++;

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Background color:"));
  grid->addWidget(lab, row, 0, Qt::AlignLeft | Qt::AlignVCenter);

  ColorButton *cb = new ColorButton;
  grid->addWidget(cb, row, 1, Qt::AlignVCenter | Qt::AlignLeft);
  row++;

  std::string id = "TabWidget";
  std::string attr = "background-color";
  QString str = settings->getStyleAttributeValue(id, attr);
  cb->setBakcroundColor(str);

  connect(cb, &ColorButton::clicked, this,
          [this, attr, id, cb]
            {
              colorDialog(cb,
                          settings->stringToColor(
                              cb->getBackGroundColor().toStdString()),
                          id, attr);
            });

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Background color of tabs labels:"));
  grid->addWidget(lab, row, 0, Qt::AlignLeft | Qt::AlignVCenter);

  cb = new ColorButton;
  grid->addWidget(cb, row, 1, Qt::AlignVCenter | Qt::AlignLeft);
  row++;

  id = "TabBar::tab";
  attr = "background-color";
  str = settings->getStyleAttributeValue(id, attr);
  cb->setBakcroundColor(str);

  connect(cb, &ColorButton::clicked, this,
          [this, attr, id, cb]
            {
              colorDialog(cb,
                          settings->stringToColor(
                              cb->getBackGroundColor().toStdString()),
                          id, attr);
            });

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Font color of tabs labels:"));
  grid->addWidget(lab, row, 0, Qt::AlignLeft | Qt::AlignVCenter);

  cb = new ColorButton;
  grid->addWidget(cb, row, 1, Qt::AlignVCenter | Qt::AlignLeft);
  row++;

  id = "TabBar::tab";
  attr = "color";
  str = settings->getStyleAttributeValue(id, attr);
  cb->setBakcroundColor(str);

  connect(cb, &ColorButton::clicked, this,
          [this, attr, id, cb]
            {
              colorDialog(cb,
                          settings->stringToColor(
                              cb->getBackGroundColor().toStdString()),
                          id, attr);
            });

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Background color of selected tab label:"));
  grid->addWidget(lab, row, 0, Qt::AlignLeft | Qt::AlignVCenter);

  cb = new ColorButton;
  grid->addWidget(cb, row, 1, Qt::AlignVCenter | Qt::AlignLeft);
  row++;

  id = "TabBar::tab:selected";
  attr = "background-color";
  str = settings->getStyleAttributeValue(id, attr);
  cb->setBakcroundColor(str);

  connect(cb, &ColorButton::clicked, this,
          [this, attr, id, cb]
            {
              colorDialog(cb,
                          settings->stringToColor(
                              cb->getBackGroundColor().toStdString()),
                          id, attr);
            });

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Font color of selected tab label:"));
  grid->addWidget(lab, row, 0, Qt::AlignLeft | Qt::AlignVCenter);

  cb = new ColorButton;
  grid->addWidget(cb, row, 1, Qt::AlignVCenter | Qt::AlignLeft);
  row++;

  id = "TabBar::tab:selected";
  attr = "color";
  str = settings->getStyleAttributeValue(id, attr);
  cb->setBakcroundColor(str);

  connect(cb, &ColorButton::clicked, this,
          [this, attr, id, cb]
            {
              colorDialog(cb,
                          settings->stringToColor(
                              cb->getBackGroundColor().toStdString()),
                          id, attr);
            });

  return frame;
}

void
SettingsWindow::colorDialog(ColorButton *cb, const QColor &color,
                            const std::string &id,
                            const std::string &attribute)
{
  QColorDialog *cd = new QColorDialog(color, this);
  cd->setAttribute(Qt::WA_DeleteOnClose);
  cd->setWindowModality(Qt::WindowModal);
  cd->setOption(QColorDialog::ShowAlphaChannel);

  connect(cd, &QColorDialog::colorSelected, this,
          [cb, this, id, attribute](const QColor &color)
            {
              int a, r, g, b;
              color.getRgb(&r, &g, &b, &a);

              std::string str = "rgba(";
              std::stringstream strm;
              strm.imbue(std::locale("C"));
              strm << r;
              str += strm.str();

              strm.clear();
              strm.str("");
              strm << g;
              str += ", ";
              str += strm.str();

              strm.clear();
              strm.str("");
              strm << b;
              str += ", ";
              str += strm.str();

              strm.clear();
              strm.str("");
              strm << a;
              str += ", ";
              str += strm.str();

              str += ")";

              settings_copy->setStyleAttributeValue(id, attribute, str);              
              cb->setBakcroundColor(str.c_str());
            });

  cd->show();
}

void
SettingsWindow::paintEvent(QPaintEvent *event)
{
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void
SettingsWindow::resetToDafaultDialog()
{
  StyledWindow *window = new StyledWindow(this);
  window->setWindowModality(Qt::WindowModal);
  window->setObjectName("Window");

  QVBoxLayout *v_box = new QVBoxLayout;
  window->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setText(tr("Are you sure?"));
  lab->setObjectName("Label");
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
              settings->resetToDefault();
              SettingsWindow *sw
                  = new SettingsWindow(this->parentWidget(), settings);
              sw->createWindow();

              sw->show();

              window->close();
              this->close();
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
