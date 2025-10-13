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

#include <FilesWidget.h>
#include <SaveArchive.h>
#include <filesystem>
#include <giomm-2.68/giomm/liststore.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <libintl.h>

#ifndef ML_GTK_OLD
#include <gtkmm-4.0/gtkmm/error.h>
#include <iostream>
#endif

FilesWidget::FilesWidget(Gtk::Window *main_window,
                         const std::shared_ptr<AuxFunc> &af)
{
  this->main_window = main_window;
  this->af = af;
  createWidget();
}

std::filesystem::path
FilesWidget::getSourceFilePath()
{
  std::string str(source_file->get_text());
  return std::filesystem::u8path(str);
}

std::filesystem::path
FilesWidget::getOutputArchivePath()
{
  std::string str(output_archive->get_text());
  return std::filesystem::u8path(str);
}

void
FilesWidget::createWidget()
{
  this->set_halign(Gtk::Align::FILL);
  this->set_valign(Gtk::Align::FILL);
  this->set_hexpand(true);
  this->set_vexpand(false);
  this->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  grid->set_expand(true);
  this->set_child(*grid);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Source file:") + "</b>");
  grid->attach(*lab, 0, 0, 1, 1);

  source_file = Gtk::make_managed<Gtk::Entry>();
  source_file->set_margin(5);
  source_file->set_halign(Gtk::Align::FILL);
  source_file->set_hexpand(true);
  source_file->set_name("windowEntry");
  source_file->set_placeholder_text(gettext("Compulsory"));
  grid->attach(*source_file, 0, 1, 1, 1);

  Gtk::Button *open = Gtk::make_managed<Gtk::Button>();
  open->set_margin(5);
  open->set_halign(Gtk::Align::CENTER);
  open->set_label(gettext("Open"));
  open->set_name("operationBut");
  open->signal_clicked().connect(
      std::bind(&FilesWidget::openFileDialog, this, source_file));
  grid->attach(*open, 1, 1, 1, 1);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Output archive:") + "</b>");
  grid->attach(*lab, 0, 2, 1, 1);

  output_archive = Gtk::make_managed<Gtk::Entry>();
  output_archive->set_margin(5);
  output_archive->set_halign(Gtk::Align::FILL);
  output_archive->set_hexpand(true);
  output_archive->set_name("windowEntry");
  output_archive->set_placeholder_text(gettext("Compulsory"));
  grid->attach(*output_archive, 0, 3, 1, 1);

  Gtk::Button *save = Gtk::make_managed<Gtk::Button>();
  save->set_margin(5);
  save->set_halign(Gtk::Align::CENTER);
  save->set_label(gettext("Save as..."));
  save->set_name("operationBut");
  save->signal_clicked().connect([this] {
    SaveArchive *sa = new SaveArchive(main_window, af);
    sa->signal_archive_type
        = std::bind(&FilesWidget::saveFileDialog, this, std::placeholders::_1);
    sa->signal_close_request().connect(
        [sa] {
          std::unique_ptr<SaveArchive> s(sa);
          s->set_visible(false);
          return true;
        },
        false);
    sa->present();
  });
  grid->attach(*save, 1, 3, 1, 1);
}

void
FilesWidget::openFileDialog(Gtk::Entry *ent)
{
#ifndef ML_GTK_OLD
  Glib::RefPtr<Gtk::FileDialog> fd = Gtk::FileDialog::create();
  fd->set_modal(true);

  Glib::RefPtr<Gio::File> fl
      = Gio::File::create_for_path(af->homePath().u8string());
  fd->set_initial_folder(fl);

  fd->open(*main_window, std::bind(&FilesWidget::openFileDialogSlot, this,
                                   std::placeholders::_1, fd, ent));
#else
  Gtk::FileChooserDialog *fd = new Gtk::FileChooserDialog(
      *main_window, "", Gtk::FileChooserDialog::Action::OPEN, true);
  fd->set_modal(true);

  Gtk::Button *but
      = fd->add_button(gettext("Open"), Gtk::ResponseType::ACCEPT);
  but->set_halign(Gtk::Align::CENTER);
  but->set_margin(5);
  but->set_name("applyBut");

  but = fd->add_button(gettext("Cancel"), Gtk::ResponseType::CANCEL);
  but->set_halign(Gtk::Align::CENTER);
  but->set_margin(5);
  but->set_name("cancelBut");

  fd->set_select_multiple(false);

  Glib::RefPtr<Gio::File> fl
      = Gio::File::create_for_path(af->homePath().u8string());
  fd->set_current_folder(fl);

  fd->signal_response().connect(std::bind(
      &FilesWidget::openFileDialogSlot, this, std::placeholders::_1, fd, ent));

  fd->signal_close_request().connect(
      [fd] {
        std::unique_ptr<Gtk::FileChooserDialog> d(fd);
        d->set_visible(false);
        return true;
      },
      false);

  fd->present();
#endif
}

void
FilesWidget::saveFileDialog(const Glib::ustring &archive_type)
{
#ifndef ML_GTK_OLD
  Glib::RefPtr<Gtk::FileDialog> fd = Gtk::FileDialog::create();
  fd->set_modal(true);

  Glib::RefPtr<Gio::File> fl
      = Gio::File::create_for_path(af->homePath().u8string());
  fd->set_initial_folder(fl);

  Glib::RefPtr<Gio::ListStore<Gtk::FileFilter>> ls
      = Gio::ListStore<Gtk::FileFilter>::create();
  Glib::RefPtr<Gtk::FileFilter> def_f = Gtk::FileFilter::create();
  def_f->set_name(archive_type);
  def_f->add_pattern("*." + archive_type);
  ls->append(def_f);

  fd->set_filters(ls);
  fd->set_default_filter(def_f);

  fd->save(*main_window, std::bind(&FilesWidget::saveFileDialogSlot, this,
                                   std::placeholders::_1, fd));
#else
  Gtk::FileChooserDialog *fd = new Gtk::FileChooserDialog(
      *main_window, "", Gtk::FileChooserDialog::Action::SAVE, true);
  fd->set_modal(true);

  Glib::RefPtr<Gio::File> fl
      = Gio::File::create_for_path(af->homePath().u8string());
  fd->set_current_folder(fl);

  Glib::RefPtr<Gtk::FileFilter> def_f = Gtk::FileFilter::create();
  def_f->set_name(archive_type);
  def_f->add_pattern("*." + archive_type);
  fd->add_filter(def_f);
  fd->set_filter(def_f);

  Gtk::Button *but
      = fd->add_button(gettext("Save"), Gtk::ResponseType::ACCEPT);
  but->set_margin(5);
  but->set_halign(Gtk::Align::CENTER);
  but->set_name("applyBut");

  but = fd->add_button(gettext("Cancel"), Gtk::ResponseType::CANCEL);
  but->set_margin(5);
  but->set_halign(Gtk::Align::CENTER);
  but->set_name("cancelBut");

  fd->signal_response().connect(std::bind(&FilesWidget::saveFileDialogSlot,
                                          this, std::placeholders::_1, fd));

  fd->signal_close_request().connect(
      [fd] {
        std::unique_ptr<Gtk::FileChooserDialog> d(fd);
        d->set_visible(false);
        return true;
      },
      false);

  fd->present();
#endif
}

#ifndef ML_GTK_OLD
void
FilesWidget::openFileDialogSlot(const Glib::RefPtr<Gio::AsyncResult> &result,
                                const Glib::RefPtr<Gtk::FileDialog> &fd,
                                Gtk::Entry *ent)
{
  Glib::RefPtr<Gio::File> fl;
  try
    {
      fl = fd->open_finish(result);
    }
  catch(Gtk::DialogError &er)
    {
      if(er.code() == Gtk::DialogError::FAILED)
        {
          std::cout << "FilesWidget::openFileDialogSlot: " << er.what()
                    << std::endl;
        }
    }
  if(fl)
    {
      ent->set_text(Glib::ustring(fl->get_path()));
    }
}

void
FilesWidget::saveFileDialogSlot(const Glib::RefPtr<Gio::AsyncResult> &result,
                                const Glib::RefPtr<Gtk::FileDialog> &fd)
{
  Glib::RefPtr<Gio::File> fl;
  try
    {
      fl = fd->save_finish(result);
    }
  catch(Gtk::DialogError &er)
    {
      if(er.code() == Gtk::DialogError::FAILED)
        {
          std::cout << "FilesWidget::saveFileDialogSlot: " << er.what()
                    << std::endl;
        }
    }
  if(fl)
    {
      std::filesystem::path p = std::filesystem::u8path(fl->get_path());
      if(!p.has_extension())
        {
          Glib::RefPtr<Gtk::FileFilter> filter = fd->get_default_filter();
          p = p.parent_path()
              / std::filesystem::u8path(p.filename().u8string() + "."
                                        + std::string(filter->get_name()));
        }
      output_archive->set_text(Glib::ustring(p.u8string()));
    }
}
#else
void
FilesWidget::openFileDialogSlot(int respons, Gtk::FileChooserDialog *fd,
                                Gtk::Entry *ent)
{
  if(respons == Gtk::ResponseType::ACCEPT)
    {
      Glib::RefPtr<Gio::File> fl = fd->get_file();
      if(fl)
        {
          ent->set_text(Glib::ustring(fl->get_path()));
        }
    }
  fd->close();
}

void
FilesWidget::saveFileDialogSlot(int respons, Gtk::FileChooserDialog *fd)
{
  if(respons == Gtk::ResponseType::ACCEPT)
    {
      Glib::RefPtr<Gio::File> fl = fd->get_file();
      if(fl)
        {
          std::filesystem::path p = std::filesystem::u8path(fl->get_path());
          if(!p.has_extension())
            {
              Glib::RefPtr<Gtk::FileFilter> filter = fd->get_filter();
              p = p.parent_path()
                  / std::filesystem::u8path(p.filename().u8string() + "."
                                            + std::string(filter->get_name()));
            }
          output_archive->set_text(Glib::ustring(p.u8string()));
        }
    }
  fd->close();
}
#endif
