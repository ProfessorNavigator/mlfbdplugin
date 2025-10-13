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
#include <AuthorsView.h>
#include <giomm-2.68/giomm/menu.h>
#include <giomm-2.68/giomm/simpleactiongroup.h>
#include <gtkmm-4.0/gtkmm/gestureclick.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <gtkmm-4.0/gtkmm/signallistitemfactory.h>
#include <gtkmm-4.0/gtkmm/singleselection.h>
#include <libintl.h>

AuthorsView::AuthorsView(Gtk::Window *main_window, const Type &view_type)
{
  this->main_window = main_window;
  this->view_type = view_type;
  formView();
}

AuthorsView::~AuthorsView()
{
  delete menu;
}

void
AuthorsView::formView()
{
  this->set_margin(5);
  this->set_halign(Gtk::Align::FILL);
  this->set_hexpand(true);
  this->set_single_click_activate(true);
  this->set_show_column_separators(true);
  this->set_show_row_separators(true);
  this->signal_activate().connect([this](guint pos) {
    activateItem(pos);
  });
  this->set_name("tablesView");

  createActionGroup();
  formMenu();

  model = Gio::ListStore<AuthorModelItem>::create();

  Glib::RefPtr<Gtk::SingleSelection> selection
      = Gtk::SingleSelection::create(model);
  this->set_model(selection);

  this->append_column(surnameColumn());
  this->append_column(firstNameColumn());
  this->append_column(middleNameColumn());
  this->append_column(nickNameColumn());
  this->append_column(homePageColumn());
  this->append_column(emailColumn());
  this->append_column(idColumn());
}

void
AuthorsView::addAuthor(const Glib::ustring &surname,
                       const Glib::ustring &first_name,
                       const Glib::ustring &middle_name,
                       const Glib::ustring &nickname,
                       const Glib::ustring &home_page,
                       const Glib::ustring &email, const Glib::ustring &id)
{
  Glib::RefPtr<AuthorModelItem> item = AuthorModelItem::create();
  item->surname = surname;
  item->first_name = first_name;
  item->middle_name = middle_name;
  item->nickname = nickname;
  item->home_page = home_page;
  item->email = email;
  item->id = id;
  model->append(item);
}

void
AuthorsView::activateItem(guint row_num)
{
  if(row_num != GTK_INVALID_LIST_POSITION)
    {
      if(active_item)
        {
          active_item->setInactive();
        }
      active_item.reset();
      active_item = model->get_item(row_num);
      if(active_item)
        {
          active_item->setActive();
        }
    }
}

void
AuthorsView::activateItem(const Glib::RefPtr<AuthorModelItem> &item)
{
  if(item)
    {
      if(active_item)
        {
          active_item->setInactive();
        }
      active_item = item;
      item->setActive();
    }
}

void
AuthorsView::removeActiveItem()
{
  if(active_item)
    {
      for(guint i = 0; i < model->get_n_items(); i++)
        {
          Glib::RefPtr<AuthorModelItem> item = model->get_item(i);
          if(item == active_item)
            {
              model->remove(i);
              break;
            }
        }
    }
}

Glib::RefPtr<AuthorModelItem>
AuthorsView::getActiveItem()
{
  return active_item;
}

Glib::RefPtr<Gio::ListStore<AuthorModelItem>>
AuthorsView::getModel()
{
  return model;
}

void
AuthorsView::updateItem(const Glib::RefPtr<AuthorModelItem> &item)
{
  for(guint i = 0; i < model->get_n_items(); i++)
    {
      Glib::RefPtr<AuthorModelItem> s_item = model->get_item(i);
      if(item == s_item)
        {
          Glib::RefPtr<AuthorModelItem> new_item = AuthorModelItem::create();
          new_item->surname = item->surname;
          new_item->first_name = item->first_name;
          new_item->middle_name = item->middle_name;
          new_item->nickname = item->nickname;
          new_item->home_page = item->home_page;
          new_item->email = item->email;
          new_item->id = item->id;
          std::vector<Glib::RefPtr<AuthorModelItem>> v;
          v.reserve(1);
          v.push_back(new_item);
          model->splice(i, v.size(), v);
          if(s_item == active_item)
            {
              active_item = new_item;
            }
          break;
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
AuthorsView::surnameColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Surname"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&AuthorsView::surnameSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&AuthorsView::surnameBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&AuthorsView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
AuthorsView::surnameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::FILL);
  lab->set_valign(Gtk::Align::CENTER);
  lab->set_justify(Gtk::Justification::CENTER);
  lab->set_ellipsize(Pango::EllipsizeMode::END);
  list_item->set_child(*lab);
}

void
AuthorsView::surnameBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<AuthorModelItem> item
          = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->surname);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
AuthorsView::firstNameColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("First name"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&AuthorsView::firstNameSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&AuthorsView::firstNameBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&AuthorsView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
AuthorsView::firstNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  surnameSetup(list_item);
}

void
AuthorsView::firstNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<AuthorModelItem> item
          = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->first_name);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
AuthorsView::middleNameColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Middle name"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&AuthorsView::middleNameSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&AuthorsView::middleNameBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&AuthorsView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
AuthorsView::middleNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  surnameSetup(list_item);
}

void
AuthorsView::middleNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<AuthorModelItem> item
          = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->middle_name);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
AuthorsView::nickNameColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Nickname"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&AuthorsView::nickNameSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&AuthorsView::nickNameBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&AuthorsView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
AuthorsView::nickNameSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  surnameSetup(list_item);
}

void
AuthorsView::nickNameBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<AuthorModelItem> item
          = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->nickname);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
AuthorsView::homePageColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create(gettext("Home page"));
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&AuthorsView::homePageSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&AuthorsView::homePageBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&AuthorsView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
AuthorsView::homePageSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  surnameSetup(list_item);
}

void
AuthorsView::homePageBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<AuthorModelItem> item
          = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->home_page);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
AuthorsView::emailColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create("e-mail");
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&AuthorsView::emailSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&AuthorsView::emailBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&AuthorsView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
AuthorsView::emailSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  surnameSetup(list_item);
}

void
AuthorsView::emailBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<AuthorModelItem> item
          = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->email);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

Glib::RefPtr<Gtk::ColumnViewColumn>
AuthorsView::idColumn()
{
  Glib::RefPtr<Gtk::ColumnViewColumn> column
      = Gtk::ColumnViewColumn::create("ID");
  column->set_expand(true);

  Glib::RefPtr<Gtk::SignalListItemFactory> factory
      = Gtk::SignalListItemFactory::create();
  column->set_factory(factory);

  factory->signal_setup().connect(
      std::bind(&AuthorsView::idSetup, this, std::placeholders::_1));

  factory->signal_bind().connect(
      std::bind(&AuthorsView::idBind, this, std::placeholders::_1));

  factory->signal_unbind().connect(
      std::bind(&AuthorsView::slotUnbind, this, std::placeholders::_1));

  return column;
}

void
AuthorsView::idSetup(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  surnameSetup(list_item);
}

void
AuthorsView::idBind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  if(lab)
    {
      Glib::RefPtr<AuthorModelItem> item
          = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
      if(item)
        {
          lab->set_text(item->id);
          lab->set_name("windowLabel");
          item->addLabel(lab);
        }
    }
}

void
AuthorsView::slotUnbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
  Gtk::Label *lab = dynamic_cast<Gtk::Label *>(list_item->get_child());
  Glib::RefPtr<AuthorModelItem> item
      = std::dynamic_pointer_cast<AuthorModelItem>(list_item->get_item());
  if(item)
    {
      item->removeLabel(lab);
    }
}

void
AuthorsView::createActionGroup()
{
  Glib::RefPtr<Gio::SimpleActionGroup> auth_actions
      = Gio::SimpleActionGroup::create();

  auth_actions->add_action("edit_author", [this] {
    Glib::RefPtr<AuthorModelItem> item = getActiveItem();
    if(item)
      {
        AddAuthorWindow *aaw;
        switch(view_type)
          {
          case Type::Authors:
            {
              aaw = new AddAuthorWindow(main_window, this,
                                        AddAuthorWindow::Author);
              break;
            }
          case Type::Translators:
            {
              aaw = new AddAuthorWindow(main_window, this,
                                        AddAuthorWindow::Translator);
              break;
            }
          }

        aaw->createWindow(item);
      }
  });

  auth_actions->add_action("remove_author", [this] {
    removeActiveItem();
  });

  this->insert_action_group("auth_actions", auth_actions);
}

void
AuthorsView::formMenu()
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

  Glib::RefPtr<Gio::MenuItem> item;
  switch(view_type)
    {
    case Type::Authors:
      {
        item = Gio::MenuItem::create(gettext("Edit author"),
                                     "auth_actions.edit_author");
        break;
      }
    case Type::Translators:
      {
        item = Gio::MenuItem::create(gettext("Edit translator"),
                                     "auth_actions.edit_author");
        break;
      }
    }
  menu_model->append_item(item);

  switch(view_type)
    {
    case Type::Authors:
      {
        item = Gio::MenuItem::create(gettext("Remove author"),
                                     "auth_actions.remove_author");
        break;
      }
    case Type::Translators:
      {
        item = Gio::MenuItem::create(gettext("Remove translator"),
                                     "auth_actions.remove_author");
        break;
      }
    }
  menu_model->append_item(item);

  Glib::RefPtr<Gtk::GestureClick> clck = Gtk::GestureClick::create();
  clck->set_button(3);
  clck->signal_pressed().connect([this](int, double x, double y) {
    Glib::RefPtr<Gtk::SingleSelection> selection
        = std::dynamic_pointer_cast<Gtk::SingleSelection>(get_model());
    if(selection)
      {
        guint pos = selection->get_selected();
        activateItem(pos);
        if(getActiveItem())
          {
            Gdk::Rectangle rec(static_cast<int>(x), static_cast<int>(y), 1, 1);
            menu->set_pointing_to(rec);
            menu->popup();
          }
      }
  });
  this->add_controller(clck);
}
