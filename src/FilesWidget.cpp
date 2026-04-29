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

#include <FilesWidget.h>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

FilesWidget::FilesWidget(QWidget *parent,
                         const std::shared_ptr<MLBookProc> &mlbp)
    : QScrollArea(parent)
{
  this->mlbp = mlbp;

  this->setWidgetResizable(true);
  this->setObjectName("ScrollArea");
  createWidget();
}

void
FilesWidget::createWidget()
{
  QWidget *viewport = new QWidget;
  viewport->setObjectName("ScrollAreaViewport");
  QVBoxLayout *v_box = new QVBoxLayout;
  viewport->setLayout(v_box);

  QLabel *lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Source file:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  src_file = new QLineEdit;
  src_file->setObjectName("LineEdit");
  src_file->setPlaceholderText(tr("Mandatory"));
  v_box->addWidget(src_file);

  QPushButton *open = new QPushButton;
  open->setText(tr("Open"));
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(open);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  open->setGraphicsEffect(shadow);
  open->setObjectName("ApplyButton");
  connect(open, &QPushButton::clicked, this,
          [this]
            {
              openFileDialog(src_file, QFileDialog::AcceptOpen);
            });
  v_box->addWidget(open, 0, Qt::AlignRight);

  lab = new QLabel;
  lab->setObjectName("Label");
  lab->setText(tr("Result file:"));
  v_box->addWidget(lab, 0, Qt::AlignLeft);

  result_file = new QLineEdit;
  result_file->setObjectName("LineEdit");
  result_file->setPlaceholderText(tr("Mandatory"));
  v_box->addWidget(result_file);

  open = new QPushButton;
  open->setText(tr("Save as..."));
  shadow = new QGraphicsDropShadowEffect(open);
  shadow->setBlurRadius(12);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 120));
  open->setGraphicsEffect(shadow);
  open->setObjectName("ApplyButton");
  connect(open, &QPushButton::clicked, this,
          [this]
            {
              openFileDialog(result_file, QFileDialog::AcceptSave);
            });
  v_box->addWidget(open, 0, Qt::AlignRight);

  v_box->addStretch();

  this->setWidget(viewport);
}

void
FilesWidget::openFileDialog(QLineEdit *edit,
                            const QFileDialog::AcceptMode &mode)
{
  QFileDialog *fd = new QFileDialog(this->window());
  fd->setAttribute(Qt::WA_DeleteOnClose);
  fd->setWindowModality(Qt::WindowModal);

  fd->setDirectory(QDir::homePath());
  fd->setAcceptMode(mode);
  if(mode == QFileDialog::AcceptOpen)
    {
      fd->setFileMode(QFileDialog::ExistingFile);
    }
  else
    {
      std::vector<std::string> sup = mlbp->getSupportedArchivesTypesPacking();
      QStringList filters;
      for(auto it = sup.begin(); it != sup.end(); it++)
        {
          QString str = "*.";
          str += it->c_str();
          filters.append(str);
        }
      fd->setNameFilters(filters);
    }

  connect(fd, &QFileDialog::fileSelected, edit, &QLineEdit::setText);

  fd->show();
}
