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
#ifndef COVERFRAME_H
#define COVERFRAME_H

#include <AuxFunc.h>
#include <Magick++.h>
#include <gtkmm-4.0/gtkmm/drawingarea.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/window.h>

#ifndef ML_GTK_OLD
#include <gtkmm-4.0/gtkmm/filedialog.h>
#else
#include <gtkmm-4.0/gtkmm/filechooserdialog.h>
#endif

class CoverFrame : public Gtk::Frame
{
public:
  CoverFrame(Gtk::Window *main_window, const std::shared_ptr<AuxFunc> &af);

  std::string
  getBase64Image();

private:
  void
  createFrame();

  void
  openCoverDialog();

#ifndef ML_GTK_OLD
  void
  openFileDialogSlot(const Glib::RefPtr<Gio::AsyncResult> &result,
                     const Glib::RefPtr<Gtk::FileDialog> &fd);
#else
  void
  openFileDialogSlot(int response, Gtk::FileChooserDialog *fd);
#endif

  void
  coverDraw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

  Gtk::Window *main_window;
  std::shared_ptr<AuxFunc> af;

  Gtk::DrawingArea *cover_area;

  Magick::Image image;
};

#endif // COVERFRAME_H
