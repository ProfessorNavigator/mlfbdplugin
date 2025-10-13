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
#include <SequenceView.h>
#include <giomm-2.68/giomm/menu.h>
#include <giomm-2.68/giomm/menuitem.h>
#include <giomm-2.68/giomm/simpleactiongroup.h>
#include <gtkmm-4.0/gtkmm/gestureclick.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <gtkmm-4.0/gtkmm/signallistitemfactory.h>
#include <gtkmm-4.0/gtkmm/singleselection.h>
#include <libintl.h>

SequenceView::SequenceView(Gtk::Window *main_window)
{
  this->main_window = main_window;
  creatView();
}

SequenceView::~SequenceView()
{
  delete menu;
}

void
SequenceView::addSequence(const Glib::ustring &name,
                          const Glib::ustring &number)
{
  Glib::RefPtr<SequenceModelItem> item = SequenceModelItem::create();
  item->name = name;
  item->number = number;
  model->append(item);
}

void
SequenceView::updateItem(const Glib::RefPtr<SequenceModelItem> &item)
{
  for(guint i = 0; i < model->get_n_items(); i++)
    {
      Glib::RefPtr<SequenceModelItem> mitem = model->get_item(i);
      if(mitem == item)
        {
          Glib::RefPtr<SequenceModelItem> new_item
              = SequenceModelItem::create();
          new_item->name = item->name;
          new_item->number = item->number;
          std::vector<Glib::RefPtr<SequenceModelItem>> add_v;
          add_v.reserve(1);
          add_v.push_back(new_item);
          model->splice(i, add_v.size(), add_v);
          if(item == active_item)
            {
              active_item.reset();
            }
          break;
        }
    }
}

void
SequenceView::removeActiveItem()
{
  for(guint i = 0; i < model->get_n_items(); i++)
    {
      Glib::RefPtr<SequenceModelItem> item = model->get_item(i);
      if(item == active_item)
        {
          model->remove(i);
          active_item.reset();
          break;
        }
    }
}

Glib::RefPtr<SequenceModelItem>
SequenceView::getActiveItem()
{
  return active_item;
}

Glib::RefPtr<Gio::ListStore<SequenceModelItem>>
SequenceView::getModel()
{
  return model;
}

void
SequenceView::creatView()
{
  this->set_margin(5);
  this->set_halign(Gtk::Align::CENTER);
  this->set_hexpand(true);
  this->set_single_click_activate(true);
  this->set_show_column_separators(true);
  this->set_show_row_separators(true);
  this->set_name("tablesView");

  this->signal_activate().connect(
      std::bind(&SequenceView::activateItem, this, std::placeholders::_1));

  model = Gio::ListStore<SequenceModelItem>::create();

  Glib::RefPtr<Gtk::SingleSelection> selection
      = Gtk::SingleSelection::create(model);

  this->set_model(selection);

  this->append_column(nameColumn());
  this->append_column(numberColumn());

  createActionGroup();

  createMenu();
}

Glib::RefPtr<Gtk::ColumnViewColumn>
SequenceView::nameColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Name"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&SequenceView::slotNameSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&SequenceView::slotNameBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&SequenceView::slotNameUnbind, this, std::placeholders::_1));

  return column;
}

void
SequenceView::slotNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::FILL);
  lab->set_valign(Gtk::Align::CENTER);
  lab->set_justify(Gtk::Justification::CENTER);
  list_item->set_child(*lab);
}

void
SequenceView::slotNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<SequenceModelItem> item
          = std::dynamic_pointer_cast<SequenceModelItem>(
              list_item->get_item());
      if(item)
        {
          lab->set_text(item->name);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

void
SequenceView::slotNameUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Glib::RefPtr<SequenceModelItem> item
      = std::dynamic_pointer_cast<SequenceModelItem>(list_item->get_item());
  if(item)
    {
      Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
      item->removeLabel(lab);
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
SequenceView::numberColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Number"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&SequenceView::slotNumberSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&SequenceView::slotNumberBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&SequenceView::slotNumberUnbind, this, std::placeholders::_1));

  return column;
}

void
SequenceView::slotNumberSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  slotNameSetup(list_item);
}

void
SequenceView::slotNumberBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<SequenceModelItem> item
          = std::dynamic_pointer_cast<SequenceModelItem>(
              list_item->get_item());
      if(item)
        {
          lab->set_text(item->number);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

void
SequenceView::slotNumberUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  slotNameUnbind(list_item);
}

void
SequenceView::activateItem(guint pos)
{
  if(pos != GTK_INVALID_LIST_POSITION)
    {
      if(active_item)
        {
          active_item->setInactve();
        }
      active_item = model->get_item(pos);
      if(active_item)
        {
          active_item->setActive();
        }
    }
}

void
SequenceView::createActionGroup()
{
  Glib::RefPtr<Gio::SimpleActionGroup> group
      = Gio::SimpleActionGroup::create();

  group->add_action("add-sequence", [this] {
    AddSequenceWindow *asw = new AddSequenceWindow(main_window, this);
    asw->createWindow();

    asw->signal_close_request().connect(
        [asw] {
          std::unique_ptr<AddSequenceWindow> win(asw);
          win->set_visible(false);
          return true;
        },
        false);

    asw->present();
  });

  group->add_action("edit-sequence", [this] {
    if(active_item)
      {
        AddSequenceWindow *asw = new AddSequenceWindow(main_window, this);
        asw->createWindow(active_item);

        asw->signal_close_request().connect(
            [asw] {
              std::unique_ptr<AddSequenceWindow> win(asw);
              win->set_visible(false);
              return true;
            },
            false);

        asw->present();
      }
  });

  group->add_action("remove-sequence", [this] {
    removeActiveItem();
  });

  this->insert_action_group("sequence-act", group);
}

void
SequenceView::createMenu()
{
  menu = new Gtk::PopoverMenu;
  this->signal_realize().connect([this] {
    menu->set_parent(*this);
  });
  this->signal_unrealize().connect([this] {
    menu->unparent();
  });

  Glib::RefPtr<Gio::Menu> menu_model = Gio::Menu::create();
  menu->set_menu_model(menu_model);

  Glib::RefPtr<Gio::MenuItem> item = Gio::MenuItem::create(
      gettext("Add sequence"), "sequence-act.add-sequence");
  menu_model->append_item(item);

  item = Gio::MenuItem::create(gettext("Edit sequence"),
                               "sequence-act.edit-sequence");
  menu_model->append_item(item);

  item = Gio::MenuItem::create(gettext("Remove sequence"),
                               "sequence-act.remove-sequence");
  menu_model->append_item(item);

  Glib::RefPtr<Gtk::GestureClick> clck = Gtk::GestureClick::create();
  clck->set_button(3);
  clck->signal_pressed().connect([this](int, double x, double y) {
    Glib::RefPtr<Gtk::SingleSelection> selection
        = std::dynamic_pointer_cast<Gtk::SingleSelection>(this->get_model());
    if(selection)
      {
        guint pos = selection->get_selected();
        activateItem(pos);
        Gdk::Rectangle rect(static_cast<int>(x), static_cast<int>(y), 1, 1);
        menu->set_pointing_to(rect);
        menu->popup();
      }
  });

  this->add_controller(clck);
}
