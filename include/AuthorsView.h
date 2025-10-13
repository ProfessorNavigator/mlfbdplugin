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
#ifndef AUTHORSVIEW_H
#define AUTHORSVIEW_H

#include <AuthorModelItem.h>
#include <giomm-2.68/giomm/liststore.h>
#include <gtkmm-4.0/gtkmm/columnview.h>
#include <gtkmm-4.0/gtkmm/columnviewcolumn.h>
#include <gtkmm-4.0/gtkmm/listitem.h>
#include <gtkmm-4.0/gtkmm/popovermenu.h>

class AuthorsView : public Gtk::ColumnView
{
public:
  enum Type
  {
    Authors,
    Translators
  };
  AuthorsView(Gtk::Window *main_window, const Type &view_type);

  virtual ~AuthorsView();

  void
  addAuthor(const Glib::ustring &surname, const Glib::ustring &first_name,
            const Glib::ustring &middle_name, const Glib::ustring &nickname,
            const Glib::ustring &home_page, const Glib::ustring &email,
            const Glib::ustring &id);

  void
  activateItem(guint row_num);

  void
  activateItem(const Glib::RefPtr<AuthorModelItem> &item);

  void
  removeActiveItem();

  Glib::RefPtr<AuthorModelItem>
  getActiveItem();

  Glib::RefPtr<Gio::ListStore<AuthorModelItem>>
  getModel();

  void
  updateItem(const Glib::RefPtr<AuthorModelItem> &item);

private:
  void
  formView();

  Glib::RefPtr<Gtk::ColumnViewColumn>
  surnameColumn();

  void
  surnameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  surnameBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  firstNameColumn();

  void
  firstNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  firstNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  middleNameColumn();

  void
  middleNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  middleNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  nickNameColumn();

  void
  nickNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  nickNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  homePageColumn();

  void
  homePageSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  homePageBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  emailColumn();

  void
  emailSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  emailBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  Glib::RefPtr<Gtk::ColumnViewColumn>
  idColumn();

  void
  idSetup(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  idBind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  slotUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  void
  createActionGroup();

  void
  formMenu();

  Gtk::Window *main_window;
  Type view_type;

  Glib::RefPtr<Gio::ListStore<AuthorModelItem>> model;

  Glib::RefPtr<AuthorModelItem> active_item;

  Gtk::PopoverMenu *menu = nullptr;
};

#endif // AUTHORSVIEW_H
