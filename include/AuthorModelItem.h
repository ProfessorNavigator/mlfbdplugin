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
#ifndef AUTHORMODELITEM_H
#define AUTHORMODELITEM_H

#include <glibmm-2.68/glibmm/object.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <vector>

class AuthorModelItem : public Glib::Object
{
public:
  static Glib::RefPtr<AuthorModelItem>
  create();

  Glib::ustring surname;
  Glib::ustring first_name;
  Glib::ustring middle_name;
  Glib::ustring nickname;
  Glib::ustring home_page;
  Glib::ustring email;
  Glib::ustring id;

  void
  addLabel(Gtk::Label *lab);

  void
  removeLabel(Gtk::Label *lab);

  void
  setActive();

  void
  setInactive();

protected:
  AuthorModelItem();

  std::vector<Gtk::Label *> lab_v;
};

#endif // AUTHORMODELITEM_H
