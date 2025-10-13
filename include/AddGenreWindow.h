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
#ifndef ADDGENREWINDOW_H
#define ADDGENREWINDOW_H

#include <AuxFunc.h>
#include <functional>
#include <gtkmm-4.0/gtkmm/checkbutton.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <gtkmm-4.0/gtkmm/window.h>
#include <tuple>
#include <vector>

class AddGenreWindow : public Gtk::Window
{
public:
  AddGenreWindow(Gtk::Window *parent_window,
                 const std::shared_ptr<AuxFunc> &af);

  std::function<void(
      const std::vector<std::tuple<Glib::ustring, Glib::ustring>> &)>
      signal_genres;

private:
  void
  createWindow();

  void
  applyFunc();

  Gtk::Window *parent_window;
  std::shared_ptr<AuxFunc> af;

  std::vector<std::tuple<Gtk::Label *, Gtk::Label *, Gtk::CheckButton *>>
      genres_v;
};

#endif // ADDGENREWINDOW_H
