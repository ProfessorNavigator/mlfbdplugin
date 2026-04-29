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
#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <ColorButton.h>
#include <QLineEdit>
#include <QPaintEvent>
#include <QWidget>
#include <SettingsManager.h>
#include <memory>

class SettingsWindow : public QWidget
{
  Q_OBJECT
public:
  SettingsWindow(QWidget *parent,
                 const std::shared_ptr<SettingsManager> &settings);

  void
  createWindow();

private:
  QWidget *
  tabWidget();

  void
  colorDialog(ColorButton *cb, const QColor &color, const std::string &id,
              const std::string &attribute);

  void
  paintEvent(QPaintEvent *event) override;

  void
  resetToDafaultDialog();

  std::shared_ptr<SettingsManager> settings;

  std::shared_ptr<SettingsManager> settings_copy;
};

#endif // SETTINGSWINDOW_H
