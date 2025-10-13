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

#include <AddGenreWindow.h>
#include <GenreFrame.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <libintl.h>

GenreFrame::GenreFrame(Gtk::Window *main_window,
                       const std::shared_ptr<AuxFunc> &af)
{
  this->main_window = main_window;
  this->af = af;
  createFrame();
}

Glib::RefPtr<Gio::ListStore<GenreModelItem>>
GenreFrame::getModel()
{
  Glib::RefPtr<Gio::ListStore<GenreModelItem>> result;
  GenreView *gv = dynamic_cast<GenreView *>(genre_view);
  if(gv)
    {
      result = gv->getModel();
    }

  return result;
}

void
GenreFrame::createFrame()
{
  this->set_margin(5);
  this->set_halign(Gtk::Align::FILL);
  this->set_hexpand(true);
  this->set_name("MLframe");

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Genre") + "</b>");
  this->set_label_widget(*lab);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::FILL);
  grid->set_expand(true);
  this->set_child(*grid);

  genre_view = Gtk::make_managed<GenreView>();
  grid->attach(*genre_view, 0, 0, 3, 1);

  Gtk::Button *add_genres = Gtk::make_managed<Gtk::Button>();
  add_genres->set_margin(5);
  add_genres->set_halign(Gtk::Align::CENTER);
  add_genres->set_name("operationBut");
  add_genres->set_label(gettext("Add genres"));
  add_genres->signal_clicked().connect(
      std::bind(&GenreFrame::addGenresWindow, this));
  grid->attach(*add_genres, 0, 1, 1, 1);

  Gtk::Button *remove_selected_genre = Gtk::make_managed<Gtk::Button>();
  remove_selected_genre->set_margin(5);
  remove_selected_genre->set_halign(Gtk::Align::CENTER);
  remove_selected_genre->set_name("removeBut");
  remove_selected_genre->set_label(gettext("Remove selected genre"));
  remove_selected_genre->signal_clicked().connect(std::bind(
      &GenreView::removeActiveItem, dynamic_cast<GenreView *>(genre_view)));
  grid->attach(*remove_selected_genre, 1, 1, 1, 1);

  Gtk::Button *remove_all_genres = Gtk::make_managed<Gtk::Button>();
  remove_all_genres->set_margin(5);
  remove_all_genres->set_halign(Gtk::Align::CENTER);
  remove_all_genres->set_name("removeBut");
  remove_all_genres->set_label(gettext("Remove all genres"));
  remove_all_genres->signal_clicked().connect(
      std::bind(&GenreView::removeAll, dynamic_cast<GenreView *>(genre_view)));
  grid->attach(*remove_all_genres, 2, 1, 1, 1);
}

void
GenreFrame::addGenresWindow()
{
  AddGenreWindow *window = new AddGenreWindow(main_window, af);

  window->signal_genres
      = std::bind(&GenreFrame::addGenresFunc, this, std::placeholders::_1);

  window->signal_close_request().connect(
      [window] {
        std::unique_ptr<AddGenreWindow> win(window);
        win->set_visible(false);
        return true;
      },
      false);

  window->present();
}

void
GenreFrame::addGenresFunc(
    const std::vector<std::tuple<Glib::ustring, Glib::ustring>> &genres)
{
  GenreView *gv = dynamic_cast<GenreView *>(genre_view);
  if(gv)
    {
      for(auto it = genres.begin(); it != genres.end(); it++)
        {
          gv->addGenre(std::get<0>(*it), std::get<1>(*it));
        }
    }
}
