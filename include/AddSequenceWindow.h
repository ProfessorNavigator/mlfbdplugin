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
#ifndef ADDSEQUENCEWINDOW_H
#define ADDSEQUENCEWINDOW_H

#include <SequenceModelItem.h>
#include <SequenceView.h>
#include <gtkmm-4.0/gtkmm/entry.h>
#include <gtkmm-4.0/gtkmm/window.h>

class AddSequenceWindow : public Gtk::Window
{
public:
  AddSequenceWindow(Gtk::Window *parent_window, SequenceView *view);

  void
  createWindow(const Glib::RefPtr<SequenceModelItem> &item
               = Glib::RefPtr<SequenceModelItem>());

private:
  void
  addSequencFunc();

  Gtk::Window *parent_window;
  SequenceView *view;

  Gtk::Entry *name_ent;
  Gtk::Entry *number_ent;

  Glib::RefPtr<SequenceModelItem> item;
};

#endif // ADDSEQUENCEWINDOW_H
