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
#ifndef AUTHORSFRAME_H
#define AUTHORSFRAME_H

#include <AuthorModelItem.h>
#include <giomm-2.68/giomm/liststore.h>
#include <gtkmm-4.0/gtkmm/columnview.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/popovermenu.h>
#include <gtkmm-4.0/gtkmm/window.h>

class AuthorsFrame : public Gtk::Frame
{
public:
  enum Type
  {
    Authors,
    Translators
  };

  AuthorsFrame(Gtk::Window *main_window, const Type &frame_type);

  Glib::RefPtr<Gio::ListStore<AuthorModelItem>>
  getModel();

  Type
  getType();

private:
  void
  createFrame();

  Gtk::Window *main_window;
  Type frame_type;

  Gtk::ColumnView *authors_view;
};

#endif // AUTHORSFRAME_H
