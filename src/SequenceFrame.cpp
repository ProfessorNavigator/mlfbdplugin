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
#include <SequenceFrame.h>
#include <SequenceView.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/label.h>
#include <libintl.h>

SequenceFrame::SequenceFrame(Gtk::Window *main_window)
{
  this->main_window = main_window;
  createFrame();
}

Glib::RefPtr<Gio::ListStore<SequenceModelItem>>
SequenceFrame::getModel()
{
  Glib::RefPtr<Gio::ListStore<SequenceModelItem>> result;

  SequenceView *sw = dynamic_cast<SequenceView *>(sequence_view);
  if(sw)
    {
      result = sw->getModel();
    }

  return result;
}

void
SequenceFrame::createFrame()
{
  this->set_margin(5);
  this->set_halign(Gtk::Align::FILL);
  this->set_hexpand(true);
  this->set_name("MLframe");

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Sequence") + "</b>");
  this->set_label_widget(*lab);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  grid->set_expand(true);
  this->set_child(*grid);

  sequence_view = Gtk::make_managed<SequenceView>(main_window);
  grid->attach(*sequence_view, 0, 0, 3, 1);

  Gtk::Button *add_sequence = Gtk::make_managed<Gtk::Button>();
  add_sequence->set_margin(5);
  add_sequence->set_halign(Gtk::Align::CENTER);
  add_sequence->set_name("operationBut");
  add_sequence->set_label(gettext("Add sequence"));
  add_sequence->signal_clicked().connect([this] {
    SequenceView *sw = dynamic_cast<SequenceView *>(sequence_view);
    if(sw)
      {
        AddSequenceWindow *asw = new AddSequenceWindow(main_window, sw);
        asw->createWindow();

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
  grid->attach(*add_sequence, 0, 1, 1, 1);

  Gtk::Button *edit_sequence = Gtk::make_managed<Gtk::Button>();
  edit_sequence->set_margin(5);
  edit_sequence->set_halign(Gtk::Align::CENTER);
  edit_sequence->set_name("operationBut");
  edit_sequence->set_label(gettext("Edit selected sequence"));
  edit_sequence->signal_clicked().connect([this] {
    SequenceView *sw = dynamic_cast<SequenceView *>(sequence_view);
    if(sw)
      {
        Glib::RefPtr<SequenceModelItem> item = sw->getActiveItem();
        if(item)
          {
            AddSequenceWindow *asw = new AddSequenceWindow(main_window, sw);
            asw->createWindow(item);

            asw->signal_close_request().connect(
                [asw] {
                  std::unique_ptr<AddSequenceWindow> win(asw);
                  win->set_visible(false);
                  return true;
                },
                false);

            asw->present();
          }
      }
  });
  grid->attach(*edit_sequence, 1, 1, 1, 1);

  Gtk::Button *remove_sequence = Gtk::make_managed<Gtk::Button>();
  remove_sequence->set_margin(5);
  remove_sequence->set_halign(Gtk::Align::CENTER);
  remove_sequence->set_name("removeBut");
  remove_sequence->set_label(gettext("Remove selected sequence"));
  remove_sequence->signal_clicked().connect([this] {
    SequenceView *sw = dynamic_cast<SequenceView *>(sequence_view);
    if(sw)
      {
        sw->removeActiveItem();
      }
  });
  grid->attach(*remove_sequence, 2, 1, 1, 1);
}
