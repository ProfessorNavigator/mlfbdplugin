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

#include <AuthorsFrame.h>
#include <CoverFrame.h>
#include <FilesWidget.h>
#include <GenreFrame.h>
#include <LibArchive.h>
#include <MLFBDPlugin.h>
#include <SelfRemovingPath.h>
#include <SequenceFrame.h>
#include <chrono>
#include <fstream>
#include <glibmm-2.68/glibmm/dispatcher.h>
#include <glibmm-2.68/glibmm/main.h>
#include <gtkmm-4.0/gtkmm/box.h>
#include <gtkmm-4.0/gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/grid.h>
#include <gtkmm-4.0/gtkmm/notebook.h>
#include <libintl.h>
#include <thread>

MLFBDPlugin::MLFBDPlugin(void *af_ptr) : MLPlugin(af_ptr)
{
  plugin_name = "MLFBDPlugin";
  std::filesystem::path txt_domain = af->share_path();
  txt_domain /= std::filesystem::u8path("locale");
  bindtextdomain(plugin_name.c_str(), txt_domain.u8string().c_str());
  bind_textdomain_codeset(plugin_name.c_str(), "UTF-8");
  textdomain(plugin_name.c_str());
  plugin_description = gettext("Plugin to create fbd files");
}

void
MLFBDPlugin::createWindow(Gtk::Window *parent_window)
{
  textdomain(plugin_name.c_str());

  main_window = new Gtk::Window;
  main_window->set_application(parent_window->get_application());
  main_window->set_title("MLFBDPlugin");
  main_window->set_name("MLwindow");
  main_window->set_transient_for(*parent_window);
  main_window->set_modal(true);
  main_window->set_default_size(parent_window->get_width(),
                                parent_window->get_height());

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  main_window->set_child(*grid);

  Gtk::Notebook *tabs = Gtk::make_managed<Gtk::Notebook>();
  tabs->set_margin(5);
  tabs->set_valign(Gtk::Align::FILL);
  tabs->set_halign(Gtk::Align::FILL);
  tabs->set_expand(true);
  tabs->set_name("MLnotebook");
  grid->attach(*tabs, 0, 0, 2, 1);

  files_widget = Gtk::make_managed<FilesWidget>(main_window, af);
  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Files") + "</b>");
  tabs->append_page(*files_widget, *lab);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Book") + "</b>");
  tabs->append_page(*bookWidget(), *lab);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Book's source") + "</b>");
  tabs->append_page(*sourceWidget(), *lab);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("XML document info")
                  + "</b>");
  tabs->append_page(*xmlDocInfo(), *lab);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Papaer book info") + "</b>");
  tabs->append_page(*paperBookInfoWidget(), *lab);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_halign(Gtk::Align::CENTER);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Custom info") + "</b>");
  tabs->append_page(*customInfo(), *lab);

  Gtk::Button *apply = Gtk::make_managed<Gtk::Button>();
  apply->set_margin(5);
  apply->set_halign(Gtk::Align::CENTER);
  apply->set_name("applyBut");
  apply->set_label(gettext("Apply"));
  apply->signal_clicked().connect(std::bind(&MLFBDPlugin::applyFunc, this));
  grid->attach(*apply, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("cancelBut");
  cancel->set_label(gettext("Close"));
  cancel->signal_clicked().connect(
      std::bind(&Gtk::Window::close, main_window));
  grid->attach(*cancel, 1, 1, 1, 1);

  main_window->signal_close_request().connect(
      [this] {
        std::unique_ptr<Gtk::Window> win(main_window);
        win->set_visible(false);
        return true;
      },
      false);

  main_window->present();
}

Gtk::Widget *
MLFBDPlugin::bookWidget()
{
  Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
  scroll->set_halign(Gtk::Align::FILL);
  scroll->set_valign(Gtk::Align::FILL);
  scroll->set_expand(true);
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Box *scroll_box
      = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  scroll_box->set_halign(Gtk::Align::FILL);
  scroll_box->set_valign(Gtk::Align::FILL);
  scroll_box->set_expand(true);
  scroll->set_child(*scroll_box);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Book title:") + "</b>");
  scroll_box->append(*lab);

  book_title = Gtk::make_managed<Gtk::Entry>();
  book_title->set_margin(5);
  book_title->set_halign(Gtk::Align::FILL);
  book_title->set_hexpand(true);
  book_title->set_name("windowEntry");
  book_title->set_placeholder_text(gettext("Compulsory"));
  scroll_box->append(*book_title);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Language:") + "</b>");
  scroll_box->append(*lab);

  language = Gtk::make_managed<Gtk::Entry>();
  language->set_margin(5);
  language->set_halign(Gtk::Align::FILL);
  language->set_hexpand(true);
  language->set_name("windowEntry");
  language->set_placeholder_text(gettext("Compulsory"));
  scroll_box->append(*language);

  auth_frame = Gtk::make_managed<AuthorsFrame>(main_window,
                                               AuthorsFrame::Type::Authors);
  scroll_box->append(*auth_frame);

  genre_frame = Gtk::make_managed<GenreFrame>(main_window, af);
  scroll_box->append(*genre_frame);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Annotation:") + "</b>");
  scroll_box->append(*lab);

  annotation = Gtk::make_managed<Gtk::TextView>();
  annotation->set_margin(5);
  annotation->set_halign(Gtk::Align::FILL);
  annotation->set_hexpand(true);
  annotation->set_vexpand(true);
  annotation->set_name("textField");
  annotation->set_left_margin(5);
  annotation->set_right_margin(5);
  annotation->set_bottom_margin(5);
  annotation->set_top_margin(5);
  scroll_box->append(*annotation);

  std::shared_ptr<Glib::Dispatcher> disp(new Glib::Dispatcher);
  disp->connect([scroll, this] {
    int height = scroll->get_height();
    if(height > 0)
      {
        annotation->set_size_request(-1, scroll->get_height() * 0.5);
      }
  });

  annotation->signal_realize().connect([scroll, disp] {
    std::thread thr([scroll, disp] {
      for(int i = 0; i <= 3; i++)
        {
          if(scroll->get_height() > 0)
            {
              break;
            }
          std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
      disp->emit();
    });
    thr.detach();
  });

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Key words:") + "</b>");
  scroll_box->append(*lab);

  key_words = Gtk::make_managed<Gtk::Entry>();
  key_words->set_margin(5);
  key_words->set_halign(Gtk::Align::FILL);
  key_words->set_hexpand(true);
  key_words->set_name("windowEntry");
  scroll_box->append(*key_words);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Date book was written:")
                  + "</b>");
  scroll_box->append(*lab);

  date = Gtk::make_managed<Gtk::Entry>();
  date->set_margin(5);
  date->set_halign(Gtk::Align::FILL);
  date->set_hexpand(true);
  date->set_name("windowEntry");
  scroll_box->append(*date);

  cover_frame = Gtk::make_managed<CoverFrame>(main_window, af);
  scroll_box->append(*cover_frame);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Source language:") + "</b>");
  scroll_box->append(*lab);

  source_language = Gtk::make_managed<Gtk::Entry>();
  source_language->set_margin(5);
  source_language->set_halign(Gtk::Align::FILL);
  source_language->set_hexpand(true);
  source_language->set_name("windowEntry");
  scroll_box->append(*source_language);

  translators_frame = Gtk::make_managed<AuthorsFrame>(
      main_window, AuthorsFrame::Type::Translators);
  scroll_box->append(*translators_frame);

  sequence_frame = Gtk::make_managed<SequenceFrame>(main_window);
  scroll_box->append(*sequence_frame);

  return scroll;
}

Gtk::Widget *
MLFBDPlugin::sourceWidget()
{
  Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
  scroll->set_halign(Gtk::Align::FILL);
  scroll->set_valign(Gtk::Align::FILL);
  scroll->set_expand(true);
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Box *scroll_box
      = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  scroll_box->set_halign(Gtk::Align::FILL);
  scroll_box->set_valign(Gtk::Align::FILL);
  scroll_box->set_expand(true);
  scroll->set_child(*scroll_box);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Book title:") + "</b>");
  scroll_box->append(*lab);

  src_book_title = Gtk::make_managed<Gtk::Entry>();
  src_book_title->set_margin(5);
  src_book_title->set_halign(Gtk::Align::FILL);
  src_book_title->set_hexpand(true);
  src_book_title->set_name("windowEntry");
  scroll_box->append(*src_book_title);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Language:") + "</b>");
  scroll_box->append(*lab);

  src_language = Gtk::make_managed<Gtk::Entry>();
  src_language->set_margin(5);
  src_language->set_halign(Gtk::Align::FILL);
  src_language->set_hexpand(true);
  src_language->set_name("windowEntry");
  scroll_box->append(*src_language);

  src_auth_frame = Gtk::make_managed<AuthorsFrame>(
      main_window, AuthorsFrame::Type::Authors);
  scroll_box->append(*src_auth_frame);

  src_genre_frame = Gtk::make_managed<GenreFrame>(main_window, af);
  scroll_box->append(*src_genre_frame);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Annotation:") + "</b>");
  scroll_box->append(*lab);

  src_annotation = Gtk::make_managed<Gtk::TextView>();
  src_annotation->set_margin(5);
  src_annotation->set_halign(Gtk::Align::FILL);
  src_annotation->set_hexpand(true);
  src_annotation->set_vexpand(true);
  src_annotation->set_name("textField");
  src_annotation->set_left_margin(5);
  src_annotation->set_right_margin(5);
  src_annotation->set_bottom_margin(5);
  src_annotation->set_top_margin(5);
  scroll_box->append(*src_annotation);

  std::shared_ptr<Glib::Dispatcher> disp(new Glib::Dispatcher);
  disp->connect([this, scroll] {
    int height = scroll->get_height();
    if(height > 0)
      {
        src_annotation->set_size_request(-1, scroll->get_height() * 0.5);
      }
  });
  src_annotation->signal_realize().connect([scroll, disp] {
    std::thread thr([scroll, disp] {
      for(int i = 0; i <= 3; i++)
        {
          if(scroll->get_height() > 0)
            {
              break;
            }
          std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
      disp->emit();
    });
    thr.detach();
  });

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Key words:") + "</b>");
  scroll_box->append(*lab);

  src_key_words = Gtk::make_managed<Gtk::Entry>();
  src_key_words->set_margin(5);
  src_key_words->set_halign(Gtk::Align::FILL);
  src_key_words->set_hexpand(true);
  src_key_words->set_name("windowEntry");
  scroll_box->append(*src_key_words);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Date book was written:")
                  + "</b>");
  scroll_box->append(*lab);

  src_date = Gtk::make_managed<Gtk::Entry>();
  src_date->set_margin(5);
  src_date->set_halign(Gtk::Align::FILL);
  src_date->set_hexpand(true);
  src_date->set_name("windowEntry");
  scroll_box->append(*src_date);

  src_cover_frame = Gtk::make_managed<CoverFrame>(main_window, af);
  scroll_box->append(*src_cover_frame);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Source language:") + "</b>");
  scroll_box->append(*lab);

  src_source_language = Gtk::make_managed<Gtk::Entry>();
  src_source_language->set_margin(5);
  src_source_language->set_halign(Gtk::Align::FILL);
  src_source_language->set_hexpand(true);
  src_source_language->set_name("windowEntry");
  scroll_box->append(*src_source_language);

  src_translators_frame = Gtk::make_managed<AuthorsFrame>(
      main_window, AuthorsFrame::Type::Translators);
  scroll_box->append(*src_translators_frame);

  src_sequence_frame = Gtk::make_managed<SequenceFrame>(main_window);
  scroll_box->append(*src_sequence_frame);

  return scroll;
}

Gtk::Widget *
MLFBDPlugin::xmlDocInfo()
{
  Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
  scroll->set_halign(Gtk::Align::FILL);
  scroll->set_valign(Gtk::Align::FILL);
  scroll->set_expand(true);
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Box *scroll_box
      = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  scroll_box->set_halign(Gtk::Align::FILL);
  scroll_box->set_valign(Gtk::Align::FILL);
  scroll_box->set_expand(true);
  scroll->set_child(*scroll_box);

  xml_author_frame
      = Gtk::make_managed<AuthorsFrame>(main_window, AuthorsFrame::Authors);
  scroll_box->append(*xml_author_frame);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Date:") + "</b>");
  scroll_box->append(*lab);

  xml_date = Gtk::make_managed<Gtk::Entry>();
  xml_date->set_margin(5);
  xml_date->set_halign(Gtk::Align::FILL);
  xml_date->set_hexpand(true);
  xml_date->set_name("windowEntry");
  std::chrono::time_point<std::chrono::system_clock> curtm
      = std::chrono::system_clock::now();
  std::time_t curtm_t = std::chrono::system_clock::to_time_t(curtm);
  std::tm *utc = std::gmtime(&curtm_t);
  if(utc)
    {
      std::string str;
      str.resize(1000);
      size_t sz
          = std::strftime(str.data(), str.size(), "%d.%m.%Y %H:%M:%S %Z", utc);
      if(sz > 0)
        {
          str.resize(sz);
          str = af->to_utf_8(str, nullptr);
          xml_date->set_text(Glib::ustring(str));
        }
    }
  scroll_box->append(*xml_date);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Source URL:") + "</b>");
  scroll_box->append(*lab);

  xml_src_url = Gtk::make_managed<Gtk::Entry>();
  xml_src_url->set_margin(5);
  xml_src_url->set_halign(Gtk::Align::FILL);
  xml_src_url->set_hexpand(true);
  xml_src_url->set_name("windowEntry");
  scroll_box->append(*xml_src_url);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(
      Glib::ustring("<b>")
      + gettext("Author of the original document (if it is conversion):")
      + "</b>");
  scroll_box->append(*lab);

  xml_orig_author = Gtk::make_managed<Gtk::Entry>();
  xml_orig_author->set_margin(5);
  xml_orig_author->set_halign(Gtk::Align::FILL);
  xml_orig_author->set_hexpand(true);
  xml_orig_author->set_name("windowEntry");
  scroll_box->append(*xml_orig_author);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Document ID:") + "</b>");
  scroll_box->append(*lab);

  xml_doc_id = Gtk::make_managed<Gtk::Entry>();
  xml_doc_id->set_margin(5);
  xml_doc_id->set_halign(Gtk::Align::FILL);
  xml_doc_id->set_hexpand(true);
  xml_doc_id->set_name("windowEntry");
  std::string id;
  for(int i = 0; i < 4; i++)
    {
      std::string rnd = af->randomFileName();
      rnd.resize(8);
      if(id.empty())
        {
          id = rnd;
        }
      else
        {
          id += "-" + rnd;
        }
    }
  xml_doc_id->set_text(Glib::ustring(id));
  scroll_box->append(*xml_doc_id);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Document version:")
                  + "</b>");
  scroll_box->append(*lab);

  xml_doc_version = Gtk::make_managed<Gtk::Entry>();
  xml_doc_version->set_margin(5);
  xml_doc_version->set_halign(Gtk::Align::FILL);
  xml_doc_version->set_hexpand(true);
  xml_doc_version->set_name("windowEntry");
  xml_doc_version->set_text("1.0");
  scroll_box->append(*xml_doc_version);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>") + gettext("Document history:")
                  + "</b>");
  scroll_box->append(*lab);

  xml_doc_history = Gtk::make_managed<Gtk::TextView>();
  xml_doc_history->set_margin(5);
  xml_doc_history->set_halign(Gtk::Align::FILL);
  xml_doc_history->set_hexpand(true);
  xml_doc_history->set_left_margin(5);
  xml_doc_history->set_top_margin(5);
  xml_doc_history->set_right_margin(5);
  xml_doc_history->set_bottom_margin(5);
  Glib::RefPtr<Gtk::TextBuffer> buf = Gtk::TextBuffer::create();
  buf->set_text(Glib::ustring("1.0 - ") + gettext("document creation"));
  xml_doc_history->set_buffer(buf);
  scroll_box->append(*xml_doc_history);

  std::shared_ptr<Glib::Dispatcher> disp(new Glib::Dispatcher);
  disp->connect([this, scroll] {
    int height = scroll->get_height() * 0.5;
    if(height > 0)
      {
        xml_doc_history->set_size_request(-1, height);
      }
  });

  scroll->signal_realize().connect([scroll, disp] {
    std::thread thr([scroll, disp] {
      for(int i = 0; i < 4; i++)
        {
          if(scroll->get_height() > 0)
            {
              break;
            }
          std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
      disp->emit();
    });
    thr.detach();
  });

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_name("windowLabel");
  lab->set_use_markup(true);
  lab->set_markup(Glib::ustring("<b>")
                  + gettext("Owner of the fb2 document copyrights:") + "</b>");
  scroll_box->append(*lab);

  xml_doc_copyright = Gtk::make_managed<Gtk::Entry>();
  xml_doc_copyright->set_margin(5);
  xml_doc_copyright->set_halign(Gtk::Align::FILL);
  xml_doc_copyright->set_hexpand(true);
  xml_doc_copyright->set_name("windowEntry");
  scroll_box->append(*xml_doc_copyright);

  return scroll;
}

Gtk::Widget *
MLFBDPlugin::paperBookInfoWidget()
{
  Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
  scroll->set_halign(Gtk::Align::FILL);
  scroll->set_valign(Gtk::Align::FILL);
  scroll->set_expand(true);
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Box *scroll_box
      = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  scroll_box->set_halign(Gtk::Align::FILL);
  scroll_box->set_valign(Gtk::Align::FILL);
  scroll_box->set_expand(true);
  scroll->set_child(*scroll_box);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Book title:") + "</b>");
  scroll_box->append(*lab);

  paper_book_title = Gtk::make_managed<Gtk::Entry>();
  paper_book_title->set_margin(5);
  paper_book_title->set_halign(Gtk::Align::FILL);
  paper_book_title->set_hexpand(true);
  paper_book_title->set_name("windowEntry");
  scroll_box->append(*paper_book_title);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Publisher:") + "</b>");
  scroll_box->append(*lab);

  paper_book_publisher = Gtk::make_managed<Gtk::Entry>();
  paper_book_publisher->set_margin(5);
  paper_book_publisher->set_halign(Gtk::Align::FILL);
  paper_book_publisher->set_hexpand(true);
  paper_book_publisher->set_name("windowEntry");
  scroll_box->append(*paper_book_publisher);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("City:") + "</b>");
  scroll_box->append(*lab);

  paper_book_city = Gtk::make_managed<Gtk::Entry>();
  paper_book_city->set_margin(5);
  paper_book_city->set_halign(Gtk::Align::FILL);
  paper_book_city->set_hexpand(true);
  paper_book_city->set_name("windowEntry");
  scroll_box->append(*paper_book_city);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Year:") + "</b>");
  scroll_box->append(*lab);

  paper_book_year = Gtk::make_managed<Gtk::Entry>();
  paper_book_year->set_margin(5);
  paper_book_year->set_halign(Gtk::Align::FILL);
  paper_book_year->set_hexpand(true);
  paper_book_year->set_name("windowEntry");
  scroll_box->append(*paper_book_year);

  lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + "ISBN:" + "</b>");
  scroll_box->append(*lab);

  paper_book_isbn = Gtk::make_managed<Gtk::Entry>();
  paper_book_isbn->set_margin(5);
  paper_book_isbn->set_halign(Gtk::Align::FILL);
  paper_book_isbn->set_hexpand(true);
  paper_book_isbn->set_name("windowEntry");
  scroll_box->append(*paper_book_isbn);

  paper_book_sequence = Gtk::make_managed<SequenceFrame>(main_window);
  scroll_box->append(*paper_book_sequence);

  return scroll;
}

Gtk::Widget *
MLFBDPlugin::customInfo()
{
  Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
  scroll->set_halign(Gtk::Align::FILL);
  scroll->set_valign(Gtk::Align::FILL);
  scroll->set_expand(true);
  scroll->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

  Gtk::Box *scroll_box
      = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
  scroll_box->set_halign(Gtk::Align::FILL);
  scroll_box->set_valign(Gtk::Align::FILL);
  scroll_box->set_expand(true);
  scroll->set_child(*scroll_box);

  Gtk::Label *lab = Gtk::make_managed<Gtk::Label>();
  lab->set_margin(5);
  lab->set_halign(Gtk::Align::START);
  lab->set_use_markup(true);
  lab->set_name("windowLabel");
  lab->set_markup(Glib::ustring("<b>") + gettext("Custom info") + ":</b>");
  scroll_box->append(*lab);

  custom_info = Gtk::make_managed<Gtk::TextView>();
  custom_info->set_margin(5);
  custom_info->set_halign(Gtk::Align::FILL);
  custom_info->set_valign(Gtk::Align::FILL);
  custom_info->set_expand(true);
  custom_info->set_left_margin(5);
  custom_info->set_top_margin(5);
  custom_info->set_right_margin(5);
  custom_info->set_bottom_margin(5);
  custom_info->set_name("textField");
  scroll_box->append(*custom_info);

  return scroll;
}

void
MLFBDPlugin::applyFunc()
{
  FilesWidget *fw = dynamic_cast<FilesWidget *>(files_widget);
  std::filesystem::path source_path;
  std::filesystem::path archive_path;
  if(fw)
    {
      source_path = fw->getSourceFilePath();
      if(!std::filesystem::exists(source_path))
        {
          errorDialog(ApplyError::NoSourceFile);
          return void();
        }
      archive_path = fw->getOutputArchivePath();
      if(archive_path.empty())
        {
          errorDialog(ApplyError::ArchivePathError);
          return void();
        }
    }
  else
    {
      errorDialog(ApplyError::FilesWidgetErr);
      return void();
    }

  Glib::ustring book_title_str = book_title->get_text();
  if(book_title_str.empty())
    {
      errorDialog(ApplyError::BookTitleEmpty);
      return void();
    }

  Glib::ustring book_lang_str = language->get_text();
  if(book_lang_str.empty())
    {
      errorDialog(ApplyError::BookLanguageEmpty);
      return void();
    }
  formWriteBuf(source_path, archive_path, book_title_str, book_lang_str);
}

void
MLFBDPlugin::formWriteBuf(const std::filesystem::path &source_path,
                          const std::filesystem::path &archive_path,
                          const Glib::ustring &book_title_str,
                          const Glib::ustring &book_lang_str)
{
  std::string write_buf = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

  write_buf
      += "<FictionBook xmlns=\"http://www.gribuser.ru/xml/fictionbook/2.0\" "
         "xmlns:l=\"http://www.w3.org/1999/xlink\">";
  {
    std::string cover_img;
    std::string src_cover_img;
    write_buf += "<description>";
    {
      // title-info section
      write_buf += "<title-info>";
      {
        GenreFrame *gfrm = dynamic_cast<GenreFrame *>(genre_frame);
        if(gfrm)
          {
            Glib::RefPtr<Gio::ListStore<GenreModelItem>> genres
                = gfrm->getModel();
            if(genres)
              {
                appendGenres(write_buf, genres);
              }
          }

        AuthorsFrame *afrm = dynamic_cast<AuthorsFrame *>(auth_frame);
        if(afrm)
          {
            Glib::RefPtr<Gio::ListStore<AuthorModelItem>> authors
                = afrm->getModel();
            if(authors)
              {
                appendAuthors(write_buf, authors, afrm->getType());
              }
          }

        write_buf += "<book-title>";
        write_buf += std::string(book_title_str);
        write_buf += "</book-title>";

        Glib::RefPtr<Gtk::TextBuffer> t_buf = annotation->get_buffer();
        if(t_buf)
          {
            appendAnnotation(write_buf, t_buf);
          }

        Glib::ustring str = key_words->get_text();
        if(!str.empty())
          {
            write_buf += "<keywords>";
            write_buf += std::string(str);
            write_buf += "</keywords>";
          }

        str = date->get_text();
        if(!str.empty())
          {
            write_buf += "<date>";
            write_buf += std::string(str);
            write_buf += "</date>";
          }

        CoverFrame *cf = dynamic_cast<CoverFrame *>(cover_frame);
        if(cf)
          {
            cover_img = cf->getBase64Image();
            if(!cover_img.empty())
              {
                write_buf += "<coverpage>";
                write_buf += "<image l:href=\"#cover.jpg\"/>";
                write_buf += "</coverpage>";
              }
          }

        write_buf += "<lang>";
        write_buf += std::string(book_lang_str);
        write_buf += "</lang>";

        str = source_language->get_text();
        if(!str.empty())
          {
            write_buf += "<src-lang>";
            write_buf += std::string(str);
            write_buf += "</src-lang>";
          }

        AuthorsFrame *tfrm = dynamic_cast<AuthorsFrame *>(translators_frame);
        if(tfrm)
          {
            Glib::RefPtr<Gio::ListStore<AuthorModelItem>> translators
                = tfrm->getModel();
            if(translators)
              {
                appendAuthors(write_buf, translators, tfrm->getType());
              }
          }

        SequenceFrame *sf = dynamic_cast<SequenceFrame *>(sequence_frame);
        if(sf)
          {
            Glib::RefPtr<Gio::ListStore<SequenceModelItem>> sequences
                = sf->getModel();
            if(sequences)
              {
                appendSequence(write_buf, sequences);
              }
          }
      }
      write_buf += "</title-info>";

      // src-title-info section
      std::string src_write_buf;
      {
        GenreFrame *gfrm = dynamic_cast<GenreFrame *>(src_genre_frame);
        if(gfrm)
          {
            Glib::RefPtr<Gio::ListStore<GenreModelItem>> genres
                = gfrm->getModel();
            if(genres)
              {
                appendGenres(src_write_buf, genres);
              }
          }

        AuthorsFrame *afrm = dynamic_cast<AuthorsFrame *>(src_auth_frame);
        if(afrm)
          {
            Glib::RefPtr<Gio::ListStore<AuthorModelItem>> authors
                = afrm->getModel();
            if(authors)
              {
                appendAuthors(src_write_buf, authors, afrm->getType());
              }
          }

        Glib::ustring str = src_book_title->get_text();
        if(!str.empty())
          {
            src_write_buf += "<book-title>";
            src_write_buf += std::string(str);
            src_write_buf += "</book-title>";
          }

        Glib::RefPtr<Gtk::TextBuffer> t_buf = src_annotation->get_buffer();
        if(t_buf)
          {
            appendAnnotation(src_write_buf, t_buf);
          }

        str = src_key_words->get_text();
        if(!str.empty())
          {
            src_write_buf += "<keywords>";
            src_write_buf += std::string(str);
            src_write_buf += "</keywords>";
          }

        str = src_date->get_text();
        if(!str.empty())
          {
            src_write_buf += "<date>";
            src_write_buf += std::string(str);
            src_write_buf += "</date>";
          }

        CoverFrame *cf = dynamic_cast<CoverFrame *>(src_cover_frame);
        if(cf)
          {
            src_cover_img = cf->getBase64Image();
            if(!src_cover_img.empty())
              {
                src_write_buf += "<coverpage>";
                src_write_buf += "<image l:href=\"#src_cover.jpg\"/>";
                src_write_buf += "</coverpage>";
              }
          }

        str = src_language->get_text();
        if(!str.empty())
          {
            src_write_buf += "<lang>";
            src_write_buf += std::string(str);
            src_write_buf += "</lang>";
          }

        str = src_source_language->get_text();
        if(!str.empty())
          {
            src_write_buf += "<src-lang>";
            src_write_buf += std::string(str);
            src_write_buf += "</src-lang>";
          }

        AuthorsFrame *tfrm
            = dynamic_cast<AuthorsFrame *>(src_translators_frame);
        if(tfrm)
          {
            Glib::RefPtr<Gio::ListStore<AuthorModelItem>> translators
                = tfrm->getModel();
            if(translators)
              {
                appendAuthors(src_write_buf, translators, tfrm->getType());
              }
          }

        SequenceFrame *sf = dynamic_cast<SequenceFrame *>(src_sequence_frame);
        if(sf)
          {
            Glib::RefPtr<Gio::ListStore<SequenceModelItem>> sequences
                = sf->getModel();
            if(sequences)
              {
                appendSequence(src_write_buf, sequences);
              }
          }
      }

      if(src_write_buf.size() > 0)
        {
          write_buf += "<src-title-info>";
          write_buf += src_write_buf;
          write_buf += "</src-title-info>";
        }

      // document-info section
      std::string doc_info_buf;
      {
        AuthorsFrame *afrm = dynamic_cast<AuthorsFrame *>(xml_author_frame);
        if(afrm)
          {
            Glib::RefPtr<Gio::ListStore<AuthorModelItem>> authors
                = afrm->getModel();
            if(authors)
              {
                appendAuthors(doc_info_buf, authors, AuthorsFrame::Authors);
              }
          }

        doc_info_buf += "<program-used>";
        doc_info_buf += "MLFBDPlugin";
        doc_info_buf += "</program-used>";

        Glib::ustring str = xml_date->get_text();
        if(!str.empty())
          {
            doc_info_buf += "<date>";
            doc_info_buf += std::string(str);
            doc_info_buf += "</date>";
          }

        str = xml_src_url->get_text();
        if(!str.empty())
          {
            doc_info_buf += "<src-url>";
            doc_info_buf += std::string(str);
            doc_info_buf += "</src-url>";
          }

        str = xml_orig_author->get_text();
        if(!str.empty())
          {
            doc_info_buf += "<src-ocr>";
            doc_info_buf += std::string(str);
            doc_info_buf += "</src-ocr>";
          }

        str = xml_doc_id->get_text();
        if(!str.empty())
          {
            doc_info_buf += "<id>";
            doc_info_buf += std::string(str);
            doc_info_buf += "</id>";
          }

        str = xml_doc_version->get_text();
        if(!str.empty())
          {
            doc_info_buf += "<version>";
            doc_info_buf += std::string(str);
            doc_info_buf += "</version>";
          }

        Glib::RefPtr<Gtk::TextBuffer> t_buf = xml_doc_history->get_buffer();
        if(t_buf)
          {
            str = t_buf->get_text();
            if(!str.empty())
              {
                doc_info_buf += "<history>";
                doc_info_buf += std::string(str);
                doc_info_buf += "</history>";
              }
          }

        str = xml_doc_copyright->get_text();
        if(!str.empty())
          {
            doc_info_buf += "<publisher>";
            doc_info_buf += std::string(str);
            doc_info_buf += "</publisher>";
          }
      }

      if(doc_info_buf.size() > 0)
        {
          write_buf += "<document-info>";
          write_buf += doc_info_buf;
          write_buf += "</document-info>";
        }

      // publish-info section
      std::string publish_info_buf;
      {
        Glib::ustring str = paper_book_title->get_text();
        if(!str.empty())
          {
            publish_info_buf += "<book-name>";
            publish_info_buf += std::string(str);
            publish_info_buf += "</book-name>";
          }

        str = paper_book_publisher->get_text();
        if(!str.empty())
          {
            publish_info_buf += "<publisher>";
            publish_info_buf += std::string(str);
            publish_info_buf += "</publisher>";
          }

        str = paper_book_city->get_text();
        if(!str.empty())
          {
            publish_info_buf += "<city>";
            publish_info_buf += std::string(str);
            publish_info_buf += "</city>";
          }

        str = paper_book_city->get_text();
        if(!str.empty())
          {
            std::string buf(str);
            buf.erase(std::remove_if(buf.begin(), buf.end(),
                                     [](const char &el) {
                                       if(el >= '0' && el <= '9')
                                         {
                                           return false;
                                         }
                                       return true;
                                     }),
                      buf.end());
            if(!buf.empty())
              {
                publish_info_buf += "<year>";
                publish_info_buf += buf;
                publish_info_buf += "</year>";
              }
          }

        str = paper_book_isbn->get_text();
        if(!str.empty())
          {
            publish_info_buf += "<isbn>";
            publish_info_buf += std::string(str);
            publish_info_buf += "</isbn>";
          }

        SequenceFrame *sf = dynamic_cast<SequenceFrame *>(paper_book_sequence);
        if(sf)
          {
            Glib::RefPtr<Gio::ListStore<SequenceModelItem>> sequences
                = sf->getModel();
            if(sequences)
              {
                appendSequence(publish_info_buf, sequences);
              }
          }
      }
      if(publish_info_buf.size() > 0)
        {
          write_buf += "<publish-info>";
          write_buf += publish_info_buf;
          write_buf += "</publish-info>";
        }

      // custom-info section
      Glib::RefPtr<Gtk::TextBuffer> t_buf = custom_info->get_buffer();
      if(t_buf)
        {
          Glib::ustring str = t_buf->get_text();
          if(!str.empty())
            {
              write_buf += "<custom-info>";
              write_buf += std::string(str);
              write_buf += "</custom-info>";
            }
        }
    }
    write_buf += "</description>";

    if(!cover_img.empty())
      {
        write_buf += "<binary id=\"cover.jpg\" content-type=\"image/jpeg\">";
        write_buf += cover_img;
        write_buf += "</binary>";
      }
    if(!src_cover_img.empty())
      {
        write_buf
            += "<binary id=\"src_cover.jpg\" content-type=\"image/jpeg\">";
        write_buf += src_cover_img;
        write_buf += "</binary>";
      }
  }
  write_buf += "</FictionBook>";

  saveResult(source_path, archive_path, write_buf);
}

void
MLFBDPlugin::saveResult(const std::filesystem::path &source_path,
                        const std::filesystem::path &archive_path,
                        const std::string &write_buf)
{
  SelfRemovingPath srp(af->temp_path());
  srp.path /= std::filesystem::u8path(af->randomFileName());
  std::fstream f;
  f.open(srp.path, std::ios_base::out | std::ios_base::binary);
  if(f.is_open())
    {
      f.write(write_buf.c_str(), write_buf.size());
      f.close();
    }
  else
    {
      errorDialog(ApplyError::TempFileError);
      return void();
    }

  std::unique_ptr<LibArchive> la(new LibArchive(af));
  std::shared_ptr<archive> a = la->libarchive_write_init(archive_path);
  if(a)
    {
      std::filesystem::path fbd_arch_path
          = std::filesystem::u8path(source_path.stem().u8string() + ".fbd");

      if(la->writeToArchive(a, srp.path, fbd_arch_path) != ARCHIVE_OK)
        {
          errorDialog(ApplyError::ArchiveWriteError);
          return void();
        }
      if(la->writeToArchive(a, source_path, source_path.filename())
         != ARCHIVE_OK)
        {
          errorDialog(ApplyError::ArchiveWriteError);
          return void();
        }
      errorDialog(ApplyError::Success);
    }
  else
    {
      errorDialog(ApplyError::LibArchiveError);
    }
}

void
MLFBDPlugin::appendGenres(
    std::string &write_buf,
    const Glib::RefPtr<Gio::ListStore<GenreModelItem>> &genres)
{
  guint n_itms = genres->get_n_items();
  if(n_itms > 0)
    {
      for(guint i = 0; i < n_itms; i++)
        {
          Glib::RefPtr<GenreModelItem> item = genres->get_item(i);
          write_buf += "<genre>";
          write_buf += std::string(item->genre_code);
          write_buf += "</genre>";
        }
    }
}

void
MLFBDPlugin::appendAuthors(
    std::string &write_buf,
    const Glib::RefPtr<Gio::ListStore<AuthorModelItem>> &authors,
    const AuthorsFrame::Type &type)
{
  guint n_itms = authors->get_n_items();
  if(n_itms > 0)
    {
      for(guint i = 0; i < n_itms; i++)
        {
          Glib::RefPtr<AuthorModelItem> item = authors->get_item(i);
          switch(type)
            {
            case AuthorsFrame::Authors:
              {
                write_buf += "<author>";
                break;
              }
            case AuthorsFrame::Translators:
              {
                write_buf += "<translator>";
                break;
              }
            }

          if(!item->first_name.empty())
            {
              write_buf += "<first-name>";
              write_buf += std::string(item->first_name);
              write_buf += "</first-name>";
            }
          if(!item->middle_name.empty())
            {
              write_buf += "<middle-name>";
              write_buf += std::string(item->middle_name);
              write_buf += "</middle-name>";
            }
          if(!item->surname.empty())
            {
              write_buf += "<last-name>";
              write_buf += std::string(item->surname);
              write_buf += "</last-name>";
            }
          if(!item->nickname.empty())
            {
              write_buf += "<nickname>";
              write_buf += std::string(item->nickname);
              write_buf += "</nickname>";
            }
          if(!item->home_page.empty())
            {
              write_buf += "<home-page>";
              write_buf += std::string(item->home_page);
              write_buf += "</home-page>";
            }
          if(!item->email.empty())
            {
              write_buf += "<email>";
              write_buf += std::string(item->email);
              write_buf += "</email>";
            }
          if(!item->id.empty())
            {
              write_buf += "<id>";
              write_buf += std::string(item->id);
              write_buf += "</id>";
            }
          switch(type)
            {
            case AuthorsFrame::Authors:
              {
                write_buf += "</author>";
                break;
              }
            case AuthorsFrame::Translators:
              {
                write_buf += "</translator>";
                break;
              }
            }
        }
    }
}

void
MLFBDPlugin::appendAnnotation(std::string &write_buf,
                              const Glib::RefPtr<Gtk::TextBuffer> &t_buf)
{
  std::string annot(t_buf->get_text());
  if(!annot.empty())
    {
      std::string find_str = "\n";
      std::string p_beg = "<p>";
      std::string p_end = "</p>";
      std::string::size_type n = 0;
      bool found = false;
      for(;;)
        {
          n = annot.find(find_str, n);
          if(n == std::string::npos)
            {
              if(found)
                {
                  if(*annot.rbegin() != '>')
                    {
                      std::copy(p_end.begin(), p_end.end(),
                                std::back_inserter(annot));
                    }
                }
              break;
            }
          found = true;
          annot.erase(n, find_str.size());
          annot.insert(annot.begin() + n, p_end.begin(), p_end.end());
        }

      find_str = p_end;
      n = annot.size() - 1;
      found = false;
      for(;;)
        {
          n = annot.rfind(find_str, n);
          if(n == std::string::npos)
            {
              if(found)
                {
                  if(*annot.begin() != '<')
                    {
                      annot.insert(annot.begin(), p_beg.begin(), p_beg.end());
                    }
                }
              break;
            }
          found = true;
          if(n != annot.size() - p_end.size())
            {
              annot.insert(annot.begin() + n + p_end.size(), p_beg.begin(),
                           p_beg.end());
            }
          n -= p_beg.size();
        }
      write_buf += "<annotation>";
      write_buf += annot;
      write_buf += "</annotation>";
    }
}

void
MLFBDPlugin::appendSequence(
    std::string &write_buf,
    const Glib::RefPtr<Gio::ListStore<SequenceModelItem>> &sequences)
{
  guint n = sequences->get_n_items();
  if(n > 0)
    {
      for(guint i = 0; i < n; i++)
        {
          Glib::RefPtr<SequenceModelItem> item = sequences->get_item(i);
          write_buf += "<sequence ";
          write_buf += "name=\"" + std::string(item->name) + "\"";
          if(!item->number.empty())
            {
              write_buf += " number=\"" + std::string(item->number) + "\"";
            }
          write_buf += "/>";
        }
    }
}

void
MLFBDPlugin::errorDialog(const ApplyError &er)
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(main_window->get_application());
  window->set_title(gettext("Message"));
  window->set_transient_for(*main_window);
  window->set_modal(true);
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
  switch(er)
    {
    case ApplyError::FilesWidgetErr:
      {
        lab->set_text(gettext("Generic error (1)!"));
        break;
      }
    case ApplyError::NoSourceFile:
      {
        lab->set_text(gettext("Source file has not been found!"));
        break;
      }
    case ApplyError::ArchivePathError:
      {
        lab->set_text(gettext("Output archive path is empty!"));
        break;
      }
    case ApplyError::BookTitleEmpty:
      {
        lab->set_text(gettext("Book title is empty!"));
        break;
      }
    case ApplyError::BookLanguageEmpty:
      {
        lab->set_text(gettext("Book language is empty!"));
        break;
      }
    case ApplyError::TempFileError:
      {
        lab->set_text(gettext("Temporary file error!"));
        break;
      }
    case ApplyError::LibArchiveError:
      {
        lab->set_text(gettext("LibArchive error!"));
        break;
      }
    case ApplyError::ArchiveWriteError:
      {
        lab->set_text(gettext("Archive write error!"));
        break;
      }
    case ApplyError::Success:
      {
        lab->set_text(
            gettext("All operations has been successfully completed!"));
        break;
      }
    }
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
}
