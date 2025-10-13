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
#ifndef GENREFRAME_H
#define GENREFRAME_H

#include <AuxFunc.h>
#include <GenreView.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/window.h>

class GenreFrame : public Gtk::Frame
{
public:
  GenreFrame(Gtk::Window *main_window, const std::shared_ptr<AuxFunc> &af);

  Glib::RefPtr<Gio::ListStore<GenreModelItem>>
  getModel();

private:
  void
  createFrame();

  void
  addGenresWindow();

  void
  addGenresFunc(
      const std::vector<std::tuple<Glib::ustring, Glib::ustring>> &genres);

  Gtk::Window *main_window;
  std::shared_ptr<AuxFunc> af;

  Gtk::ColumnView *genre_view;
};

#endif // GENREFRAME_H
