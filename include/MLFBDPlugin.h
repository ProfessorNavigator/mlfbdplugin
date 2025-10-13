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
#ifndef MLFBDPLUGIN_H
#define MLFBDPLUGIN_H

#include <AuthorModelItem.h>
#include <AuthorsFrame.h>
#include <GenreModelItem.h>
#include <MLPlugin.h>
#include <SequenceModelItem.h>
#include <giomm-2.68/giomm/liststore.h>
#include <gtkmm-4.0/gtkmm/entry.h>
#include <gtkmm-4.0/gtkmm/frame.h>
#include <gtkmm-4.0/gtkmm/textview.h>

class MLFBDPlugin : public MLPlugin
{
public:
  MLFBDPlugin(void *af_ptr);

  void
  createWindow(Gtk::Window *parent_window) override;

private:
  Gtk::Widget *
  bookWidget();

  Gtk::Widget *
  sourceWidget();

  Gtk::Widget *
  xmlDocInfo();

  Gtk::Widget *
  paperBookInfoWidget();

  Gtk::Widget *
  customInfo();

  void
  applyFunc();

  void
  formWriteBuf(const std::filesystem::path &source_path,
               const std::filesystem::path &archive_path,
               const Glib::ustring &book_title_str,
               const Glib::ustring &book_lang_str);

  void
  saveResult(const std::filesystem::path &source_path,
             const std::filesystem::path &archive_path,
             const std::string &write_buf);

  void
  appendGenres(std::string &write_buf,
               const Glib::RefPtr<Gio::ListStore<GenreModelItem>> &genres);

  void
  appendAuthors(std::string &write_buf,
                const Glib::RefPtr<Gio::ListStore<AuthorModelItem>> &authors,
                const AuthorsFrame::Type &type);

  void
  appendAnnotation(std::string &write_buf,
                   const Glib::RefPtr<Gtk::TextBuffer> &t_buf);

  void
  appendSequence(
      std::string &write_buf,
      const Glib::RefPtr<Gio::ListStore<SequenceModelItem>> &sequences);

  enum ApplyError
  {
    FilesWidgetErr,
    NoSourceFile,
    ArchivePathError,
    BookTitleEmpty,
    BookLanguageEmpty,
    TempFileError,
    LibArchiveError,
    ArchiveWriteError,
    Success
  };

  void
  errorDialog(const ApplyError &er);

  Gtk::Window *main_window;

  Gtk::Widget *files_widget;

  Gtk::Frame *auth_frame;
  Gtk::Entry *book_title;
  Gtk::Frame *genre_frame;
  Gtk::TextView *annotation;
  Gtk::Entry *key_words;
  Gtk::Entry *date;
  Gtk::Frame *cover_frame;
  Gtk::Entry *language;
  Gtk::Entry *source_language;
  Gtk::Frame *translators_frame;
  Gtk::Frame *sequence_frame;

  Gtk::Frame *src_auth_frame;
  Gtk::Entry *src_book_title;
  Gtk::Frame *src_genre_frame;
  Gtk::TextView *src_annotation;
  Gtk::Entry *src_key_words;
  Gtk::Entry *src_date;
  Gtk::Frame *src_cover_frame;
  Gtk::Entry *src_language;
  Gtk::Entry *src_source_language;
  Gtk::Frame *src_translators_frame;
  Gtk::Frame *src_sequence_frame;

  Gtk::Frame *xml_author_frame;
  Gtk::Entry *xml_date;
  Gtk::Entry *xml_src_url;
  Gtk::Entry *xml_orig_author;
  Gtk::Entry *xml_doc_id;
  Gtk::Entry *xml_doc_version;
  Gtk::TextView *xml_doc_history;
  Gtk::Entry *xml_doc_copyright;

  Gtk::Entry *paper_book_title;
  Gtk::Entry *paper_book_publisher;
  Gtk::Entry *paper_book_city;
  Gtk::Entry *paper_book_year;
  Gtk::Entry *paper_book_isbn;
  Gtk::Frame *paper_book_sequence;

  Gtk::TextView *custom_info;
};

extern "C"
{
#if defined(__linux)
  MLPlugin *
  create(void *af_ptr)
  {
    return new MLFBDPlugin(af_ptr);
  }
#elif defined(_WIN32)
  __declspec(dllexport) MLPlugin *
  create(void *af_ptr)
  {
    return new MLFBDPlugin(af_ptr);
  }
#endif
}

#endif // MLFBDPLUGIN_H
