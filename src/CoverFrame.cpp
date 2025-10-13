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

#include <CoverFrame.h>
#include <cairomm-1.16/cairomm/surface.h>
#include <filesystem>
#include <giomm-2.68/giomm/liststore.h>
#include <glibmm-2.68/glibmm/dispatcher.h>
#include <glibmm-2.68/glibmm/fileutils.h>
#include <glibmm-2.68/glibmm/main.h>
#include <gtkmm-4.0/gdkmm/general.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <iostream>
#include <libintl.h>
#include <thread>

#ifndef ML_GTK_OLD
#include <gtkmm-4.0/gtkmm/error.h>
#endif

CoverFrame::CoverFrame(Gtk::Window *main_window,
                       const std::shared_ptr<AuxFunc> &af)
{
  this->main_window = main_window;
  this->af = af;
  createFrame();
}

std::string
CoverFrame::getBase64Image()
{
  std::string result;

  if(image.columns() > 0 && image.rows() > 0)
    {
      Magick::Blob blob;
      image.write(&blob, "jpeg");
      result = blob.base64();
    }

  return result;
}

void
CoverFrame::createFrame()
{
  this->set_margin(5);
  this->set_halign(Gtk::Align::FILL);
  this->set_hexpand(true);
  this->set_name("MLframe");

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Cover") + "</b>");
  this->set_label_widget(*lab);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  grid->set_expand(true);
  this->set_child(*grid);

  cover_area = Gtk::make_managed<Gtk::DrawingArea>();
  cover_area->set_margin(5);
  cover_area->set_halign(Gtk::Align::FILL);
  cover_area->set_hexpand(true);
  cover_area->set_draw_func(
      std::bind(&CoverFrame::coverDraw, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  grid->attach(*cover_area, 0, 0, 2, 1);

  Gtk::Button *open = Gtk::make_managed<Gtk::Button>();
  open->set_margin(5);
  open->set_halign(Gtk::Align::CENTER);
  open->set_name("operationBut");
  open->set_label(gettext("Open cover"));
  open->signal_clicked().connect(
      std::bind(&CoverFrame::openCoverDialog, this));
  grid->attach(*open, 0, 1, 1, 1);

  Gtk::Button *clear = Gtk::make_managed<Gtk::Button>();
  clear->set_margin(5);
  clear->set_halign(Gtk::Align::CENTER);
  clear->set_name("removeBut");
  clear->set_label(gettext("Clear"));
  clear->signal_clicked().connect([this] {
    image = Magick::Image();
    cover_area->queue_draw();
  });
  grid->attach(*clear, 1, 1, 1, 1);
}

void
CoverFrame::openCoverDialog()
{
#ifndef ML_GTK_OLD
  Glib::RefPtr<Gtk::FileDialog> fd = Gtk::FileDialog::create();
  fd->set_modal(true);

  Glib::RefPtr<Gio::File> fl
      = Gio::File::create_for_path(af->homePath().u8string());
  fd->set_initial_folder(fl);

  Glib::RefPtr<Gio::ListStore<Gtk::FileFilter>> filters_list
      = Gio::ListStore<Gtk::FileFilter>::create();
  std::vector<Magick::CoderInfo> formats;
  try
    {
      Magick::coderInfoList(&formats, Magick::CoderInfo::TrueMatch,
                            Magick::CoderInfo::AnyMatch,
                            Magick::CoderInfo::AnyMatch);
    }
  catch(Magick::Exception &er)
    {
      std::cout << "CoverFrame::openCoverDialog: " << er.what() << std::endl;
    }

  Glib::RefPtr<Gtk::FileFilter> default_filter = Gtk::FileFilter::create();
  default_filter->set_name(gettext("All images"));
  filters_list->append(default_filter);
  std::vector<std::string> mime_types;
  for(auto it = formats.begin(); it != formats.end(); it++)
    {
      std::string type = it->mimeType();
      if(!type.empty())
        {
          mime_types.emplace_back(type);
        }
    }

  for(auto it = mime_types.begin(); it != mime_types.end(); it++)
    {
      std::string type = *it;
      mime_types.erase(std::remove(it + 1, mime_types.end(), type),
                       mime_types.end());
    }

  for(auto it = mime_types.begin(); it != mime_types.end(); it++)
    {
      Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
      filter->add_mime_type(*it);
      filter->set_name(*it);
      default_filter->add_mime_type(*it);
      filters_list->append(filter);
    }
  fd->set_filters(filters_list);
  if(default_filter)
    {
      fd->set_default_filter(default_filter);
    }

  fd->open(*main_window, std::bind(&CoverFrame::openFileDialogSlot, this,
                                   std::placeholders::_1, fd));
#else
  Gtk::FileChooserDialog *fd = new Gtk::FileChooserDialog(
      *main_window, "", Gtk::FileChooserDialog::Action::OPEN, true);
  fd->set_modal(true);

  Glib::RefPtr<Gio::File> fl
      = Gio::File::create_for_path(af->homePath().u8string());
  fd->set_current_folder(fl);

  fd->set_select_multiple(false);

  std::vector<Magick::CoderInfo> formats;
  try
    {
      Magick::coderInfoList(&formats, Magick::CoderInfo::TrueMatch,
                            Magick::CoderInfo::AnyMatch,
                            Magick::CoderInfo::AnyMatch);
    }
  catch(Magick::Exception &er)
    {
      std::cout << "CoverFrame::openCoverDialog: " << er.what() << std::endl;
    }
  Glib::RefPtr<Gtk::FileFilter> default_filter = Gtk::FileFilter::create();
  default_filter->set_name(gettext("All images"));

  std::vector<std::string> mime_types;
  for(auto it = formats.begin(); it != formats.end(); it++)
    {
      std::string type = it->mimeType();
      if(!type.empty())
        {
          mime_types.emplace_back(type);
        }
    }

  for(auto it = mime_types.begin(); it != mime_types.end(); it++)
    {
      std::string type = *it;
      mime_types.erase(std::remove(it + 1, mime_types.end(), type),
                       mime_types.end());
    }

  for(auto it = mime_types.begin(); it != mime_types.end(); it++)
    {
      Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
      filter->set_name(*it);
      filter->add_mime_type(*it);
      default_filter->add_mime_type(*it);
      fd->add_filter(filter);
    }

  fd->add_filter(default_filter);

  if(default_filter)
    {
      fd->set_filter(default_filter);
    }

  Gtk::Button *but
      = fd->add_button(gettext("Open"), Gtk::ResponseType::ACCEPT);
  but->set_margin(5);
  but->set_halign(Gtk::Align::CENTER);
  but->set_name("applyBut");

  but = fd->add_button(gettext("Cancel"), Gtk::ResponseType::CANCEL);
  but->set_margin(5);
  but->set_halign(Gtk::Align::CENTER);
  but->set_name("cancelBut");

  fd->signal_response().connect(std::bind(&CoverFrame::openFileDialogSlot,
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
CoverFrame::openFileDialogSlot(const Glib::RefPtr<Gio::AsyncResult> &result,
                               const Glib::RefPtr<Gtk::FileDialog> &fd)
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
          std::cout << "CoverFrame::openFileDialogSlot: " << er.what()
                    << std::endl;
        }
    }
  if(fl)
    {
      Glib::Dispatcher *disp = new Glib::Dispatcher;
      disp->connect([this, fl, disp] {
        std::unique_ptr<Glib::Dispatcher> d(disp);
        image = Magick::Image();
        try
          {
            std::filesystem::path p = std::filesystem::u8path(fl->get_path());
            image.read(p.string());
          }
        catch(Magick::Exception &er)
          {
            std::cout << "CoverFrame::openFileDialogSlot: " << er.what()
                      << std::endl;
          }

        if(image.rows() > 0 && image.columns() > 0)
          {
            double w_a = static_cast<double>(cover_area->get_width());
            double w_b = static_cast<double>(image.columns());
            if(w_a < w_b)
              {
                double coef = w_a / w_b;
                cover_area->set_content_height(image.rows() * coef);
              }
            else
              {
                cover_area->set_content_height(image.rows());
              }
          }
        cover_area->queue_draw();
      });
      std::thread thr([disp] {
        disp->emit();
      });
      thr.detach();
    }
}
#else
void
CoverFrame::openFileDialogSlot(int response, Gtk::FileChooserDialog *fd)
{
  if(response == Gtk::ResponseType::ACCEPT)
    {
      Glib::RefPtr<Gio::File> fl = fd->get_file();
      if(fl)
        {
          Glib::Dispatcher *disp = new Glib::Dispatcher;
          disp->connect([this, fl, disp] {
            std::unique_ptr<Glib::Dispatcher> d(disp);
            image = Magick::Image();
            try
              {
                std::filesystem::path p
                    = std::filesystem::u8path(fl->get_path());
                image.read(p.string());
              }
            catch(Magick::Exception &er)
              {
                std::cout << "CoverFrame::openFileDialogSlot: " << er.what()
                          << std::endl;
              }

            if(image.rows() > 0 && image.columns() > 0)
              {
                double w_a = static_cast<double>(cover_area->get_width());
                double w_b = static_cast<double>(image.columns());
                if(w_a < w_b)
                  {
                    double coef = w_a / w_b;
                    cover_area->set_content_height(image.rows() * coef);
                  }
                else
                  {
                    cover_area->set_content_height(image.rows());
                  }
              }
            cover_area->queue_draw();
          });

          std::thread thr([disp] {
            disp->emit();
          });
          thr.detach();
        }
    }
  fd->close();
}
#endif

void
CoverFrame::coverDraw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                      int height)
{
  if(image.rows() > 0 && image.columns() > 0)
    {
      Magick::Image img = image;
      int w = static_cast<int>(image.columns());
      double x = 0.0;
      double y = 0.0;
      if(width < w)
        {
          double coef = static_cast<double>(width) / static_cast<double>(w);
          img.resize(
              Magick::Geometry(static_cast<size_t>(width), img.rows() * coef));
        }
      else if(w < width)
        {
          x = static_cast<double>(width - w) / 2.0;
        }
      if(img.rows() > 0 && img.columns() > 0)
        {
          int h = static_cast<int>(img.rows());
          if(height < h)
            {
              double coef
                  = static_cast<double>(height) / static_cast<double>(h);

              img.resize(Magick::Geometry(img.columns() * coef,
                                          static_cast<size_t>(height)));
              x = static_cast<double>(width - static_cast<int>(img.columns()))
                  / 2.0;
            }
          else if(h < height)
            {
              y = static_cast<double>(height - h) / 2.0;
            }
        }
      if(img.rows() > 0 && img.columns() > 0)
        {
          Magick::Blob blob;
          img.write(&blob, "png");
          size_t rb = 0;
          Cairo::RefPtr<Cairo::ImageSurface> surf
              = Cairo::ImageSurface::create_from_png_stream(
                  [&rb, blob](unsigned char *data, int length) {
                    const unsigned char *buf
                        = reinterpret_cast<const unsigned char *>(blob.data());
                    for(int i = 0; i < length && rb < blob.length(); i++, rb++)
                      {
                        data[i] = buf[rb];
                      }
                    return CAIRO_STATUS_SUCCESS;
                  });
          cr->set_source(surf, x, y);
          cr->paint();
        }
    }
}
