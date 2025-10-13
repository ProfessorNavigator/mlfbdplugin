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
#ifndef SEQUENCEVIEW_H
#define SEQUENCEVIEW_H

#include <SequenceModelItem.h>
#include <giomm-2.68/giomm/liststore.h>
#include <gtkmm-4.0/gtkmm/columnview.h>
#include <gtkmm-4.0/gtkmm/listitem.h>
#include <gtkmm-4.0/gtkmm/popovermenu.h>
#include <gtkmm-4.0/gtkmm/window.h>

class SequenceView : public Gtk::ColumnView
{
public:
  SequenceView(Gtk::Window *main_window);

  virtual ~SequenceView();

  void
  addSequence(const Glib::ustring &name, const Glib::ustring &number);

  void
  updateItem(const Glib::RefPtr<SequenceModelItem> &item);

  void
  removeActiveItem();

  Glib::RefPtr<SequenceModelItem>
  getActiveItem();

  Glib::RefPtr<Gio::ListStore<SequenceModelItem>>
  getModel();

private:
  void
  creatView();

  Glib::RefPtr<Gtk::ColumnViewColumn>
  nameColumn();

  void
  slotNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotNameUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  numberColumn();

  void
  slotNumberSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotNumberBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotNumberUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  activateItem(guint pos);

  void
  createActionGroup();

  void
  createMenu();

  Gtk::Window *main_window;

  Glib::RefPtr<Gio::ListStore<SequenceModelItem>> model;

  Glib::RefPtr<SequenceModelItem> active_item;

  Gtk::PopoverMenu *menu = nullptr;
};

#endif // SEQUENCEVIEW_H
