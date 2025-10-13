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
#ifndef SEQUENCEFRAME_H
#define SEQUENCEFRAME_H

#include <SequenceModelItem.h>
#include <giomm-2.68/giomm/liststore.h>
#include <gtkmm-4.0/gtkmm/columnview.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/window.h>

class SequenceFrame : public Gtk::Frame
{
public:
  SequenceFrame(Gtk::Window *main_window);

  Glib::RefPtr<Gio::ListStore<SequenceModelItem>>
  getModel();

private:
  void
  createFrame();

  Gtk::Window *main_window;

  Gtk::ColumnView *sequence_view;
};

#endif // SEQUENCEFRAME_H
