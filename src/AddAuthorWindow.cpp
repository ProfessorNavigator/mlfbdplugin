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

#include <AddAuthorWindow.h>
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <libintl.h>

AddAuthorWindow::AddAuthorWindow(Gtk::Window *parent_window,
                                 AuthorsView *authors_model,
                                 const Type &win_type)
{
  this->parent_window = parent_window;
  this->authors_model = authors_model;
  this->win_type = win_type;
}

void
AddAuthorWindow::createWindow(const Glib::RefPtr<AuthorModelItem> &item)
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(parent_window->get_application());
  if(item)
    {
      switch(win_type)
        {
        case Type::Author:
          {
            window->set_title(gettext("Edit author"));
            break;
          }
        case Type::Translator:
          {
            window->set_title(gettext("Edit translator"));
            break;
          }
        }
    }
  else
    {
      switch(win_type)
        {
        case Type::Author:
          {
            window->set_title(gettext("Add author"));
            break;
          }
        case Type::Translator:
          {
            window->set_title(gettext("Add translator"));
            break;
          }
        }
    }
  window->set_transient_for(*parent_window);
  window->set_modal(true);
  window->set_default_size(parent_window->get_width() * 0.35, -1);
  window->set_name("MLwindow");

  Gtk::Box *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  box->set_halign(Gtk::Align::FILL);
  box->set_valign(Gtk::Align::FILL);
  box->set_expand(true);
  window->set_child(*box);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("Surname:"));
  box->append(*lab);

  surname = Gtk::make_managed<Gtk::Entry>();
  surname->set_margin(5);
  surname->set_halign(Gtk::Align::FILL);
  surname->set_hexpand(true);
  surname->set_name("windowEntry");
  if(item)
    {
      surname->set_text(item->surname);
    }
  box->append(*surname);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("First name:"));
  box->append(*lab);

  first_name = Gtk::make_managed<Gtk::Entry>();
  first_name->set_margin(5);
  first_name->set_halign(Gtk::Align::FILL);
  first_name->set_hexpand(true);
  first_name->set_name("windowEntry");
  if(item)
    {
      first_name->set_text(item->first_name);
    }
  box->append(*first_name);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("Middle name:"));
  box->append(*lab);

  middle_name = Gtk::make_managed<Gtk::Entry>();
  middle_name->set_margin(5);
  middle_name->set_halign(Gtk::Align::FILL);
  middle_name->set_hexpand(true);
  middle_name->set_name("windowEntry");
  if(item)
    {
      middle_name->set_text(item->middle_name);
    }
  box->append(*middle_name);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("Nickname:"));
  box->append(*lab);

  nickname = Gtk::make_managed<Gtk::Entry>();
  nickname->set_margin(5);
  nickname->set_halign(Gtk::Align::FILL);
  nickname->set_hexpand(true);
  nickname->set_name("windowEntry");
  if(item)
    {
      nickname->set_text(item->nickname);
    }
  box->append(*nickname);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("Home page:"));
  box->append(*lab);

  home_page = Gtk::make_managed<Gtk::Entry>();
  home_page->set_margin(5);
  home_page->set_halign(Gtk::Align::FILL);
  home_page->set_hexpand(true);
  home_page->set_name("windowEntry");
  if(item)
    {
      home_page->set_text(item->home_page);
    }
  box->append(*home_page);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text("e-mail:");
  box->append(*lab);

  email = Gtk::make_managed<Gtk::Entry>();
  email->set_margin(5);
  email->set_halign(Gtk::Align::FILL);
  email->set_hexpand(true);
  email->set_name("windowEntry");
  if(item)
    {
      email->set_text(item->email);
    }
  box->append(*email);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text("ID:");
  box->append(*lab);

  id = Gtk::make_managed<Gtk::Entry>();
  id->set_margin(5);
  id->set_halign(Gtk::Align::FILL);
  id->set_hexpand(true);
  id->set_name("windowEntry");
  if(item)
    {
      id->set_text(item->id);
    }
  box->append(*id);

  Gtk::Box *button_box
      = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
  button_box->set_halign(Gtk::Align::FILL);
  button_box->set_valign(Gtk::Align::FILL);
  button_box->set_expand(true);
  button_box->set_homogeneous(true);
  box->append(*button_box);

  Gtk::Button *add = Gtk::make_managed<Gtk::Button>();
  add->set_margin(5);
  add->set_halign(Gtk::Align::CENTER);
  add->set_name("applyBut");
  if(item)
    {
      add->set_label(gettext("Edit"));
    }
  else
    {
      add->set_label(gettext("Add"));
    }
  add->signal_clicked().connect(
      std::bind(&AddAuthorWindow::addAuthor, this, window, item));
  button_box->append(*add);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelBut");
  cancel->set_label(gettext("Cancel"));
  cancel->signal_clicked().connect(std::bind(&Gtk::Window::close, window));
  button_box->append(*cancel);

  window->signal_close_request().connect(
      [window, this] {
        std::unique_ptr<Gtk::Window> win(window);
        win->set_visible(false);
        delete this;
        return true;
      },
      false);

  window->present();
}

void
AddAuthorWindow::addAuthor(Gtk::Window *win,
                           const Glib::RefPtr<AuthorModelItem> &item)
{
  Glib::ustring surname_str = surname->get_text();
  if(surname_str.empty())
    {
      Gtk::Window *window = new Gtk::Window;
      window->set_application(win->get_application());
      window->set_title(gettext("Error"));
      window->set_transient_for(*win);
      window->set_modal(true);
      window->set_default_size(1, 1);
      window->set_name("MLwindow");

      Gtk::Box *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
      box->set_halign(Gtk::Align::FILL);
      box->set_valign(Gtk::Align::FILL);
      box->set_expand(true);
      window->set_child(*box);

      Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
      lab->set_margin(5);
      lab->set_halign(Gtk::Align::CENTER);
      lab->set_name("windowLabel");
      lab->set_text(gettext("Surname cannot be empty!"));
      box->append(*lab);

      Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
      close->set_margin(5);
      close->set_halign(Gtk::Align::CENTER);
      close->set_name("cancelBut");
      close->set_label(gettext("Close"));
      close->signal_clicked().connect(std::bind(&Gtk::Window::close, window));
      box->append(*close);

      window->signal_close_request().connect(
          [window] {
            std::unique_ptr<Gtk::Window> win(window);
            win->set_visible(false);
            return true;
          },
          false);

      window->present();

      return void();
    }

  Glib::ustring first_name_str = first_name->get_text();
  Glib::ustring middle_name_str = middle_name->get_text();
  Glib::ustring nickname_str = nickname->get_text();
  Glib::ustring home_page_str = home_page->get_text();
  Glib::ustring email_str = email->get_text();
  Glib::ustring id_str = id->get_text();

  if(item)
    {
      item->surname = surname_str;
      item->first_name = first_name_str;
      item->middle_name = middle_name_str;
      item->nickname = nickname_str;
      item->home_page = home_page_str;
      item->email = email_str;
      item->id = id_str;
      authors_model->updateItem(item);
    }
  else
    {
      authors_model->addAuthor(surname_str, first_name_str, middle_name_str,
                               nickname_str, home_page_str, email_str, id_str);
    }

  win->close();
}
