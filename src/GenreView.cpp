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

#include <GenreView.h>
#include <giomm-2.68/giomm/menu.h>
#include <giomm-2.68/giomm/simpleactiongroup.h>
#include <gtkmm-4.0/gtkmm/gestureclick.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <gtkmm-4.0/gtkmm/signallistitemfactory.h>
#include <gtkmm-4.0/gtkmm/singleselection.h>
#include <libintl.h>

GenreView::GenreView()
{
  createView();
}

GenreView::~GenreView()
{
  delete menu;
}

void
GenreView::addGenre(const Glib::ustring &genre_name,
                    const Glib::ustring &genre_code)
{
  Glib::RefPtr<GenreModelItem> item = GenreModelItem::create();
  item->genre_code = genre_code;
  item->genre_name = genre_name;
  model->append(item);
}

void
GenreView::removeAll()
{
  model->remove_all();
  active_item.reset();
}

void
GenreView::removeActiveItem()
{
  for(guint i = 0; i < model->get_n_items(); i++)
    {
      if(active_item == model->get_item(i))
        {
          model->remove(i);
          active_item.reset();
          break;
        }
    }
}

Glib::RefPtr<Gio::ListStore<GenreModelItem>>
GenreView::getModel()
{
  return model;
}

void
GenreView::setActive(guint row)
{
  if(row != GTK_INVALID_LIST_POSITION)
    {
      if(active_item)
        {
          active_item->setInactive();
        }
      active_item = model->get_item(row);
      if(active_item)
        {
          active_item->setActive();
        }
    }
}

void
GenreView::formActionGroup()
{
  Glib::RefPtr<Gio::SimpleActionGroup> action_group
      = Gio::SimpleActionGroup::create();

  action_group->add_action("remove_genre", [this] {
    removeActiveItem();
  });

  action_group->add_action("remove_all",
                           std::bind(&GenreView::removeAll, this));

  this->insert_action_group("genre_action_group", action_group);
}

void
GenreView::formMenu()
{
  menu = new Gtk::PopoverMenu;
  this->signal_realize().connect([this] {
    menu->set_parent(*this);
  });
  this->signal_unrealize().connect([this] {
    menu->unparent();
  });

  Glib::RefPtr<Gio::Menu> menu_model = Gio::Menu::create();

  Glib::RefPtr<Gio::MenuItem> item = Gio::MenuItem::create(
      gettext("Remove genre"), "genre_action_group.remove_genre");
  menu_model->append_item(item);

  item = Gio::MenuItem::create(gettext("Remove all"),
                               "genre_action_group.remove_all");
  menu_model->append_item(item);

  menu->set_menu_model(menu_model);

  Glib::RefPtr<Gtk::GestureClick> clck = Gtk::GestureClick::create();
  clck->set_button(3);
  clck->signal_pressed().connect([this](int, double x, double y) {
    Glib::RefPtr<Gtk::SingleSelection> selection
        = std::dynamic_pointer_cast<Gtk::SingleSelection>(this->get_model());
    if(selection)
      {
        active_item.reset();
        guint row = selection->get_selected();
        setActive(row);
        if(active_item)
          {
            Gdk::Rectangle rect(static_cast<int>(x), static_cast<int>(y), 1,
                                1);
            menu->set_pointing_to(rect);
            menu->popup();
          }
      }
  });

  this->add_controller(clck);
}

void
GenreView::createView()
{
  this->set_margin(5);
  this->set_halign(Gtk::Align::CENTER);
  this->set_hexpand(true);
  this->set_single_click_activate(true);
  this->set_show_column_separators(true);
  this->set_show_row_separators(true);
  this->set_name("tablesView");

  formView();

  formActionGroup();

  formMenu();

  this->signal_activate().connect(
      std::bind(&GenreView::setActive, this, std::placeholders::_1));
}

void
GenreView::formView()
{
  model = Gio::ListStore<GenreModelItem>::create();

  Glib::RefPtr<Gtk::SingleSelection> selection
      = Gtk::SingleSelection::create(model);

  this->set_model(selection);

  this->append_column(formGenreColumn());
  this->append_column(formCodeColumn());
}

Glib::RefPtr<Gtk::ColumnViewColumn>
GenreView::formGenreColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Genre"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&GenreView::slotSetupGenre, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&GenreView::slotBindGenre, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&GenreView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
GenreView::slotSetupGenre(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::FILL);
  lab->set_valign(Gtk::Align::CENTER);
  lab->set_justify(Gtk::Justification::CENTER);
  list_item->set_child(*lab);
}

void
GenreView::slotBindGenre(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<GenreModelItem> item
          = std::dynamic_pointer_cast<GenreModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->genre_name);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
GenreView::formCodeColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Code"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&GenreView::slotSetupCode, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&GenreView::slotBindCode, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&GenreView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
GenreView::slotSetupCode(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  slotSetupGenre(list_item);
}

void
GenreView::slotBindCode(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<GenreModelItem> item
          = std::dynamic_pointer_cast<GenreModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->genre_code);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

void
GenreView::slotUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  Glib::RefPtr<GenreModelItem> item
      = std::dynamic_pointer_cast<GenreModelItem>(list_item->get_item());
  if(item)
    {
      item->removeLabel(lab);
    }
}
