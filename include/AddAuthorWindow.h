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
#ifndef ADDAUTHORWINDOW_H
#define ADDAUTHORWINDOW_H

#include <AuthorModelItem.h>
#include <AuthorsView.h>
#include <gtkmm-4.0/gtkmm/entry.h>
#include <gtkmm-4.0/gtkmm/window.h>

class AddAuthorWindow
{
public:
  enum Type
  {
    Author,
    Translator
  };

  AddAuthorWindow(Gtk::Window *parent_window, AuthorsView *authors_model,
                  const Type &win_type);

  void
  createWindow(const Glib::RefPtr<AuthorModelItem> &item
               = Glib::RefPtr<AuthorModelItem>());

private:
  void
  addAuthor(Gtk::Window *win, const Glib::RefPtr<AuthorModelItem> &item);

  Gtk::Window *parent_window;
  AuthorsView *authors_model;
  Type win_type;

  Gtk::Entry *surname;
  Gtk::Entry *first_name;
  Gtk::Entry *middle_name;
  Gtk::Entry *nickname;
  Gtk::Entry *home_page;
  Gtk::Entry *email;
  Gtk::Entry *id;
};

#endif // ADDAUTHORWINDOW_H
