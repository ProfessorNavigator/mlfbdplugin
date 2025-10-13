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
#ifndef GENREVIEW_H
#define GENREVIEW_H

#include <GenreModelItem.h>
#include <giomm-2.68/giomm/liststore.h>
#include <gtkmm-4.0/gtkmm/columnview.h>
#include <gtkmm-4.0/gtkmm/columnviewcolumn.h>
#include <gtkmm-4.0/gtkmm/listitem.h>
#include <gtkmm-4.0/gtkmm/popovermenu.h>

class GenreView : public Gtk::ColumnView
{
public:
  GenreView();

  virtual ~GenreView();

  void
  addGenre(const Glib::ustring &genre_name, const Glib::ustring &genre_code);

  void
  removeAll();

  void
  removeActiveItem();

  Glib::RefPtr<Gio::ListStore<GenreModelItem>>
  getModel();

private:
  void
  createView();

  void
  formView();

  Glib::RefPtr<Gtk::ColumnViewColumn>
  formGenreColumn();

  void
  slotSetupGenre(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotBindGenre(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  formCodeColumn();

  void
  slotSetupCode(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotBindCode(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  setActive(guint row);

  void
  formActionGroup();

  void
  formMenu();

  Glib::RefPtr<Gio::ListStore<GenreModelItem>> model;

  Glib::RefPtr<GenreModelItem> active_item;

  Gtk::PopoverMenu *menu = nullptr;
};

#endif // GENREVIEW_H
