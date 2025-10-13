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

#include <SaveArchive.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <gtkmm-4.0/gtkmm/stringlist.h>
#include <gtkmm-4.0/gtkmm/stringobject.h>
#include <libintl.h>
#include <optional>

SaveArchive::SaveArchive(Gtk::Window *parent_window,
                         const std::shared_ptr<AuxFunc> &af)
{
  this->parent_window = parent_window;
  this->af = af;
  createWindow();
}

void
SaveArchive::createWindow()
{
  this->set_application(parent_window->get_application());
  this->set_title("Archive format selection");
  this->set_name("MLwindow");
  this->set_transient_for(*parent_window);
  this->set_modal(true);
  this->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  grid->set_expand(true);
  grid->set_column_homogeneous(true);
  this->set_child(*grid);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_text(gettext("Archive type:"));
  grid->attach(*lab, 0, 0, 2, 1);

  std::vector<std::string> arch_types
      = af->get_supported_archive_types_packing();

  Glib::RefPtr<Gtk::StringList> list
      = Gtk::StringList::create(std::vector<Glib::ustring>());

  std::optional<size_t> def;
  for(size_t i = 0; i < arch_types.size(); i++)
    {
      list->append(Glib::ustring(arch_types[i]));
      if(arch_types[i] == "zip")
        {
          def = i;
        }
    }

  types = Gtk::make_managed<Gtk::DropDown>();
  types->set_margin(5);
  types->set_halign(Gtk::Align::CENTER);
  types->set_model(list);
  types->set_name("comboBox");
  if(def.has_value())
    {
      types->set_selected(guint(def.value()));
    }
  grid->attach(*types, 0, 1, 2, 1);

  Gtk::Button *apply = Gtk::make_managed<Gtk::Button>();
  apply->set_margin(5);
  apply->set_halign(Gtk::Align::CENTER);
  apply->set_name("applyBut");
  apply->set_label(gettext("Save"));
  apply->signal_clicked().connect(std::bind(&SaveArchive::applyFunc, this));
  grid->attach(*apply, 0, 2, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelBut");
  cancel->set_label(gettext("Cancel"));
  cancel->signal_clicked().connect(std::bind(&SaveArchive::close, this));
  grid->attach(*cancel, 1, 2, 1, 1);
}

void
SaveArchive::applyFunc()
{
  Glib::RefPtr<Gtk::StringObject> obj
      = std::dynamic_pointer_cast<Gtk::StringObject>(
          types->get_selected_item());
  if(obj)
    {
      Glib::ustring sel = obj->get_string();
      if(signal_archive_type)
        {
          signal_archive_type(sel);
        }
    }
  this->close();
}
