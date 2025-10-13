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
#include <AuthorsFrame.h>
#include <AuthorsView.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <gtkmm-4.0/gtkmm/singleselection.h>
#include <libintl.h>

AuthorsFrame::AuthorsFrame(Gtk::Window *main_window, const Type &frame_type)
{
  this->main_window = main_window;
  this->frame_type = frame_type;
  createFrame();
}

Glib::RefPtr<Gio::ListStore<AuthorModelItem>>
AuthorsFrame::getModel()
{
  Glib::RefPtr<Gio::ListStore<AuthorModelItem>> result;
  AuthorsView *av = dynamic_cast<AuthorsView *>(authors_view);
  if(av)
    {
      result = av->getModel();
    }

  return result;
}

AuthorsFrame::Type
AuthorsFrame::getType()
{
  return frame_type;
}

void
AuthorsFrame::createFrame()
{
  this->set_margin(5);
  this->set_halign(Gtk::Align::FILL);
  this->set_hexpand(true);
  this->set_name("MLframe");

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  switch(frame_type)
    {
    case Type::Authors:
      {
        lab->set_markup(Glib::ustring("<b>") + gettext("Authors") + "</b>");
        break;
      }
    case Type::Translators:
      {
        lab->set_markup(Glib::ustring("<b>") + gettext("Translators")
                        + "</b>");
        break;
      }
    }
  this->set_label_widget(*lab);

  Gtk::Grid *author_grid = Gtk::make_managed<Gtk::Grid>();
  author_grid->set_halign(Gtk::Align::FILL);
  author_grid->set_valign(Gtk::Align::FILL);
  author_grid->set_hexpand(true);
  this->set_child(*author_grid);

  switch(frame_type)
    {
    case Type::Authors:
      {
        authors_view = Gtk::make_managed<AuthorsView>(
            main_window, AuthorsView::Type::Authors);
        break;
      }
    case Type::Translators:
      {
        authors_view = Gtk::make_managed<AuthorsView>(
            main_window, AuthorsView::Type::Translators);
        break;
      }
    }
  author_grid->attach(*authors_view, 0, 0, 3, 1);

  Gtk::Button *add_author = Gtk::make_managed<Gtk::Button>();
  add_author->set_margin(5);
  add_author->set_halign(Gtk::Align::CENTER);
  add_author->set_name("operationBut");
  switch(frame_type)
    {
    case Type::Authors:
      {
        add_author->set_label(gettext("Add author"));
        break;
      }
    case Type::Translators:
      {
        add_author->set_label(gettext("Add translator"));
        break;
      }
    }
  add_author->signal_clicked().connect([this] {
    AuthorsView *av = dynamic_cast<AuthorsView *>(authors_view);
    if(av)
      {
        AddAuthorWindow *aaw;
        switch(frame_type)
          {
          case Type::Authors:
            {
              aaw = new AddAuthorWindow(main_window, av,
                                        AddAuthorWindow::Type::Author);
              break;
            }
          case Type::Translators:
            {
              aaw = new AddAuthorWindow(main_window, av,
                                        AddAuthorWindow::Type::Translator);
              break;
            }
          }
        aaw->createWindow();
      }
  });
  author_grid->attach(*add_author, 0, 1, 1, 1);

  Gtk::Button *edit_author = Gtk::make_managed<Gtk::Button>();
  edit_author->set_margin(5);
  edit_author->set_halign(Gtk::Align::CENTER);
  edit_author->set_name("operationBut");
  switch(frame_type)
    {
    case Type::Authors:
      {
        edit_author->set_label(gettext("Edit selected author"));
        break;
      }
    case Type::Translators:
      {
        edit_author->set_label(gettext("Edit selected translator"));
        break;
      }
    }
  edit_author->signal_clicked().connect([this] {
    AuthorsView *av = dynamic_cast<AuthorsView *>(authors_view);
    if(av)
      {
        Glib::RefPtr<AuthorModelItem> item = av->getActiveItem();
        if(item)
          {
            AddAuthorWindow *aaw;
            switch(frame_type)
              {
              case Type::Authors:
                {
                  aaw = new AddAuthorWindow(main_window, av,
                                            AddAuthorWindow::Type::Author);
                  break;
                }
              case Type::Translators:
                {
                  aaw = new AddAuthorWindow(main_window, av,
                                            AddAuthorWindow::Type::Translator);
                  break;
                }
              }
            aaw->createWindow(item);
          }
      }
  });
  author_grid->attach(*edit_author, 1, 1, 1, 1);

  Gtk::Button *remove_author = Gtk::make_managed<Gtk::Button>();
  remove_author->set_margin(5);
  remove_author->set_halign(Gtk::Align::CENTER);
  remove_author->set_name("removeBut");
  switch(frame_type)
    {
    case Type::Authors:
      {
        remove_author->set_label(gettext("Remove selected author"));
        break;
      }
    case Type::Translators:
      {
        remove_author->set_label(gettext("Remove selected translator"));
        break;
      }
    }
  remove_author->signal_clicked().connect([this] {
    AuthorsView *av = dynamic_cast<AuthorsView *>(authors_view);
    if(av)
      {
        av->removeActiveItem();
      }
  });
  author_grid->attach(*remove_author, 2, 1, 1, 1);
}
