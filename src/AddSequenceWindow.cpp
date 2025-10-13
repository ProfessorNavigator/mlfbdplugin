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

#include <AddSequenceWindow.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <libintl.h>

AddSequenceWindow::AddSequenceWindow(Gtk::Window *parent_window,
                                     SequenceView *view)
{
  this->parent_window = parent_window;
  this->view = view;
}

void
AddSequenceWindow::createWindow(const Glib::RefPtr<SequenceModelItem> &item)
{
  this->item = item;
  this->set_application(parent_window->get_application());
  if(item)
    {
      this->set_title(gettext("Edit sequence"));
    }
  else
    {
      this->set_title(gettext("Add sequence"));
    }
  this->set_transient_for(*parent_window);
  this->set_modal(true);
  this->set_name("MLwindow");

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  grid->set_expand(true);
  this->set_child(*grid);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("Sequence:"));
  grid->attach(*lab, 0, 0, 1, 1);

  name_ent = Gtk::make_managed<Gtk::Entry>();
  name_ent->set_margin(5);
  name_ent->set_halign(Gtk::Align::FILL);
  name_ent->set_hexpand(true);
  name_ent->set_name("windowEntry");
  if(item)
    {
      name_ent->set_text(item->name);
    }
  grid->attach(*name_ent, 0, 1, 2, 1);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("Number:"));
  grid->attach(*lab, 0, 2, 1, 1);

  number_ent = Gtk::make_managed<Gtk::Entry>();
  number_ent->set_margin(5);
  number_ent->set_halign(Gtk::Align::FILL);
  number_ent->set_hexpand(true);
  number_ent->set_name("windowEntry");
  if(item)
    {
      number_ent->set_text(item->number);
    }
  else
    {
      number_ent->set_text("1");
    }
  grid->attach(*number_ent, 0, 3, 2, 1);

  Gtk::Button *add = Gtk::make_managed<Gtk::Button>();
  add->set_margin(5);
  add->set_halign(Gtk::Align::CENTER);
  add->set_name("applyBut");
  if(item)
    {
      add->set_label(gettext("Edit sequence"));
    }
  else
    {
      add->set_label(gettext("Add sequence"));
    }
  add->signal_clicked().connect(
      std::bind(&AddSequenceWindow::addSequencFunc, this));
  grid->attach(*add, 0, 4, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelBut");
  cancel->set_label(gettext("Cancel"));
  cancel->signal_clicked().connect(std::bind(&Gtk::Window::close, this));
  grid->attach(*cancel, 1, 4, 1, 1);
}

void
AddSequenceWindow::addSequencFunc()
{
  Glib::ustring name = name_ent->get_text();
  if(!name.empty())
    {
      Glib::ustring number = number_ent->get_text();
      for(auto it = number.begin(); it != number.end();)
        {
          if(*it >= 48 && *it <= 57)
            {
              it++;
            }
          else
            {
              number.erase(it);
            }
        }
      if(item)
        {
          item->name = name;
          item->number = number;
          view->updateItem(item);
        }
      else
        {
          view->addSequence(name, number);
        }
      this->close();
    }
}
