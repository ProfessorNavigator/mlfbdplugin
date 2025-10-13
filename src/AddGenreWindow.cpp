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
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/scrolledwindow.h>
#include <libintl.h>

AddGenreWindow::AddGenreWindow(Gtk::Window *parent_window,
                               const std::shared_ptr<AuxFunc> &af)
{
  this->parent_window = parent_window;
  this->af = af;
  createWindow();
}

void
AddGenreWindow::createWindow()
{
  this->set_application(parent_window->get_application());
  this->set_title(gettext("Add genre"));
  this->set_name("MLwindow");
  this->set_transient_for(*parent_window);
  this->set_modal(true);
  this->set_default_size(parent_window->get_width() * 0.5,
                         parent_window->get_height());

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  grid->set_expand(true);
  this->set_child(*grid);

  Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
  scroll->set_margin(5);
  scroll->set_halign(Gtk::Align::FILL);
  scroll->set_valign(Gtk::Align::FILL);
  scroll->set_expand(true);
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  grid->attach(*scroll, 0, 0, 2, 1);

  Gtk::Box *scroll_box
      = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  scroll_box->set_halign(Gtk::Align::FILL);
  scroll_box->set_valign(Gtk::Align::FILL);
  scroll_box->set_expand(true);
  scroll->set_child(*scroll_box);

  std::vector<GenreGroup> g_gr = af->get_genre_list();
  for(auto it = g_gr.begin(); it != g_gr.end(); it++)
    {
      Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
      lab->set_margin(5);
      lab->set_halign(Gtk::Align::START);
      lab->set_use_markup(true);
      lab->set_name("windowLabel");
      lab->set_markup(Glib::ustring("<b>") + Glib::ustring(it->group_name)
                      + "</b>");
      scroll_box->append(*lab);

      Gtk::Grid *gen_grid = Gtk::make_managed<Gtk::Grid>();
      gen_grid->set_halign(Gtk::Align::FILL);
      gen_grid->set_hexpand(true);
      gen_grid->set_column_homogeneous(true);
      scroll_box->append(*gen_grid);

      int row = 0;

      genres_v.reserve(it->genres.size());

      for(auto it_gen = it->genres.begin(); it_gen != it->genres.end();
          it_gen++)
        {
          std::tuple<Gtk::Label *, Gtk::Label *, Gtk::CheckButton *> ttup;
          lab = Gtk::make_managed<Gtk::Label>();
          lab->set_margin_start(20);
          lab->set_margin_top(5);
          lab->set_margin_end(5);
          lab->set_margin_bottom(5);
          lab->set_halign(Gtk::Align::START);
          lab->set_max_width_chars(30);
          lab->set_wrap(true);
          lab->set_wrap_mode(Pango::WrapMode::WORD);
          lab->set_name("windowLabel");
          lab->set_text(Glib::ustring(it_gen->genre_name));
          gen_grid->attach(*lab, 0, row, 1, 1);
          std::get<0>(ttup) = lab;

          lab = Gtk::make_managed<Gtk::Label>();
          lab->set_margin(5);
          lab->set_halign(Gtk::Align::START);
          lab->set_valign(Gtk::Align::CENTER);
          lab->set_name("windowLabel");
          lab->set_text(Glib::ustring(it_gen->genre_code));
          gen_grid->attach(*lab, 1, row, 1, 1);
          std::get<1>(ttup) = lab;

          Gtk::CheckButton *check = Gtk::make_managed<Gtk::CheckButton>();
          check->set_margin(5);
          check->set_halign(Gtk::Align::START);
          check->set_valign(Gtk::Align::CENTER);
          check->set_active(false);
          gen_grid->attach(*check, 2, row, 1, 1);
          std::get<2>(ttup) = check;

          genres_v.push_back(ttup);

          row++;
        }
    }

  Gtk::Button *apply = Gtk::make_managed<Gtk::Button>();
  apply->set_margin(5);
  apply->set_halign(Gtk::Align::CENTER);
  apply->set_name("applyBut");
  apply->set_label(gettext("Apply"));
  apply->signal_clicked().connect([this] {
    applyFunc();
    this->close();
  });
  grid->attach(*apply, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelBut");
  cancel->set_label(gettext("Cancel"));
  cancel->signal_clicked().connect(std::bind(&Gtk::Window::close, this));
  grid->attach(*cancel, 1, 1, 1, 1);
}

void
AddGenreWindow::applyFunc()
{
  std::vector<std::tuple<Glib::ustring, Glib::ustring>> res;
  for(auto it = genres_v.begin(); it != genres_v.end(); it++)
    {
      if(std::get<2>(*it)->get_active())
        {
          std::tuple<Glib::ustring, Glib::ustring> ttup;
          std::get<0>(ttup) = std::get<0>(*it)->get_text();
          std::get<1>(ttup) = std::get<1>(*it)->get_text();
          res.emplace_back(ttup);
        }
    }

  if(signal_genres)
    {
      signal_genres(res);
    }
}
