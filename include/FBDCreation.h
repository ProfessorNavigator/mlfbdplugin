/*
 * Copyright (C) 2026 Yury Bobylev <bobilev_yury@mail.ru>
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
#ifndef FBDCREATION_H
#define FBDCREATION_H

#include <BaseID.h>
#include <UDBElement.h>
#include <XMLParserCPP.h>

class FBDCreation
{
public:
  FBDCreation();

  enum TargetElement
  {
    TitleInfo,
    SrcTitleInfo,
    DocumentInfo,
    PublishInfo
  };

  void
  addTitleInfo(const TargetElement &target);

  void
  addTextElement(const std::string &target, const std::string &element_name,
                 const std::string &value);

  enum AuthorType
  {
    Author,
    Translator,
    Publisher
  };

  void
  addAuthor(const std::string &target, const UDBElement &author,
            const AuthorType &type);

  void
  addAnnotation(const std::string &target, const std::string &annotation);

  void
  addDate(const std::string &target, const std::string &date);

  void
  addCoverPage(const std::string &target, const std::string &base64_image);

  void
  addSequence(const std::string &target, const UDBElement &sequence);

  void
  addHistory(const std::string &target, const std::string &history);

  void
  addCustomInfo(const std::string &info);

  std::string
  writeToBuffer();

  std::vector<XMLElement *>
  searchTarget(const std::string &target);

private:
  void
  createTemplate();

  void
  addParagraphs(XMLElement &result, const std::string &text);

  std::vector<XMLElement> fbd;

  BaseID bid;
};

#endif // FBDCREATION_H
