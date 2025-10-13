/*
 * Copyright (C) 2025 Yury Bobylev <bobilev_yury@mail.ru>
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
#ifndef SAVEARCHIVE_H
#define SAVEARCHIVE_H

#include <AuxFunc.h>
#include <functional>
#include <gtkmm-4.0/gtkmm/dropdown.h>
#include <gtkmm-4.0/gtkmm/window.h>
#include <memory>

class SaveArchive : public Gtk::Window
{
public:
  SaveArchive(Gtk::Window *parent_window, const std::shared_ptr<AuxFunc> &af);

  std::function<void(const Glib::ustring &archive_type)> signal_archive_type;

private:
  void
  createWindow();

  void
  applyFunc();

  Gtk::Window *parent_window;
  std::shared_ptr<AuxFunc> af;

  Gtk::DropDown *types;
};

#endif // SAVEARCHIVE_H
