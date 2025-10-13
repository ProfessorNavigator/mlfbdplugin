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
#ifndef FILESFRAME_H
#define FILESFRAME_H

#include <AuxFunc.h>
#include <filesystem>
#include <gtkmm-4.0/gtkmm/entry.h>
#include <gtkmm-4.0/gtkmm/scrolledwindow.h>
#include <gtkmm-4.0/gtkmm/window.h>

#ifndef ML_GTK_OLD
#include <gtkmm-4.0/gtkmm/filedialog.h>
#else
#include <gtkmm-4.0/gtkmm/filechooserdialog.h>
#endif

class FilesWidget : public Gtk::ScrolledWindow
{
public:
  FilesWidget(Gtk::Window *main_window, const std::shared_ptr<AuxFunc> &af);

  std::filesystem::path
  getSourceFilePath();

  std::filesystem::path
  getOutputArchivePath();

private:
  void
  createWidget();

  void
  openFileDialog(Gtk::Entry *ent);

  void
  saveFileDialog(const Glib::ustring &archive_type);

#ifndef ML_GTK_OLD
  void
  openFileDialogSlot(const Glib::RefPtr<Gio::AsyncResult> &result,
                     const Glib::RefPtr<Gtk::FileDialog> &fd, Gtk::Entry *ent);

  void
  saveFileDialogSlot(const Glib::RefPtr<Gio::AsyncResult> &result,
                     const Glib::RefPtr<Gtk::FileDialog> &fd);
#else
  void
  openFileDialogSlot(int respons, Gtk::FileChooserDialog *fd, Gtk::Entry *ent);

  void
  saveFileDialogSlot(int respons, Gtk::FileChooserDialog *fd);
#endif

  Gtk::Window *main_window;
  std::shared_ptr<AuxFunc> af;

  Gtk::Entry *source_file;
  Gtk::Entry *output_archive;
};

#endif // FILESFRAME_H
