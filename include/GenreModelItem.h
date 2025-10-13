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
#ifndef GENREMODELITEM_H
#define GENREMODELITEM_H

#include <glibmm-2.68/glibmm/object.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <vector>

class GenreModelItem : public Glib::Object
{
public:
  static Glib::RefPtr<GenreModelItem>
  create();

  Glib::ustring genre_code;
  Glib::ustring genre_name;

  void
  addLabel(Gtk::Label *lab);

  void
  removeLabel(Gtk::Label *lab);

  void
  setActive();

  void
  setInactive();

protected:
  GenreModelItem();

  std::vector<Gtk::Label *> lab_v;
};

#endif // GENREMODELITEM_H
