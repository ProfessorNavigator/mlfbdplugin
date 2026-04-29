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

#include <FBDCreation.h>
#include <XMLAlgorithms.h>
#include <algorithm>
#include <stdexcept>

FBDCreation::FBDCreation()
{
  createTemplate();
}

void
FBDCreation::addTitleInfo(const TargetElement &target)
{
  XMLElement title_info;
  switch(target)
    {
    case TargetElement::TitleInfo:
      {
        title_info.element_name = "title-info";
        break;
      }
    case TargetElement::SrcTitleInfo:
      {
        title_info.element_name = "src-title-info";
        break;
      }
    case TargetElement::DocumentInfo:
      {
        title_info.element_name = "document-info";
        break;
      }
    case TargetElement::PublishInfo:
      {
        title_info.element_name = "publish-info";
        break;
      }
    default:
      {
        throw std::runtime_error(
            "FBDCreation::addTitleInfo: incorrect target");
      }
    }
  title_info.element_type = XMLElement::OrdinaryElement;

  std::vector<XMLElement *> elements;
  XMLAlgorithms::searchElement(fbd, "description", elements);

  if(elements.size() == 0)
    {
      throw std::runtime_error(
          "FBDCreation::addTitleInfo: cannot find description");
    }

  (*elements.begin())->elements.emplace_back(title_info);
}

void
FBDCreation::addTextElement(const std::string &target,
                            const std::string &element_name,
                            const std::string &value)
{
  std::vector<XMLElement *> elements = std::move(searchTarget(target));

  XMLElement txt_el;
  txt_el.element_name = element_name;
  txt_el.element_type = XMLElement::OrdinaryElement;

  XMLElement el;
  el.element_type = XMLElement::ElementContent;
  el.content = value;
  txt_el.elements.emplace_back(el);

  (*elements.begin())->elements.emplace_back(txt_el);
}

void
FBDCreation::addAuthor(const std::string &target, const UDBElement &author,
                       const AuthorType &type)
{
  std::vector<XMLElement *> elements = std::move(searchTarget(target));

  XMLElement auth;
  switch(type)
    {
    case AuthorType::Author:
      {
        auth.element_name = "author";
        break;
      }
    case AuthorType::Translator:
      {
        auth.element_name = "translator";
        break;
      }
    case AuthorType::Publisher:
      {
        auth.element_name = "publisher";
        break;
      }
    default:
      return void();
    }

  auth.element_type = XMLElement::OrdinaryElement;

  for(auto it = author.subelements.begin(); it != author.subelements.end();
      it++)
    {
      XMLElement el;
      el.element_type = XMLElement::OrdinaryElement;

      switch(bid.getId(*it))
        {
        case BaseID::LastName:
          {
            el.element_name = "last-name";
            break;
          }
        case BaseID::FirstName:
          {
            el.element_name = "first-name";
            break;
          }
        case BaseID::MiddleName:
          {
            el.element_name = "middle-name";
            break;
          }
        case BaseID::Nickname:
          {
            el.element_name = "nickname";
            break;
          }
        case BaseID::HomePage:
          {
            el.element_name = "home-page";
            break;
          }
        case BaseID::EMail:
          {
            el.element_name = "email";
            break;
          }
        case BaseID::AuthorID:
          {
            el.element_name = "id";
            break;
          }
        default:
          continue;
        }
      XMLElement content;
      content.element_type = XMLElement::ElementContent;
      content.content = it->content;
      if(content.content.empty())
        {
          continue;
        }
      el.elements.emplace_back(content);

      auth.elements.emplace_back(el);
    }
  (*elements.begin())->elements.emplace_back(auth);
}

void
FBDCreation::addAnnotation(const std::string &target,
                           const std::string &annotation)
{
  if(annotation.empty())
    {
      return void();
    }
  std::vector<XMLElement *> elements = std::move(searchTarget(target));

  XMLElement annot;
  annot.element_name = "annotation";
  annot.element_type = XMLElement::OrdinaryElement;

  addParagraphs(annot, annotation);

  (*elements.begin())->elements.emplace_back(annot);
}

void
FBDCreation::addDate(const std::string &target, const std::string &date)
{
  if(date.empty())
    {
      return void();
    }

  std::vector<XMLElement *> elements = std::move(searchTarget(target));

  XMLElement d;
  d.element_name = "date";
  d.element_type = XMLElement::OrdinaryElement;

  XMLElementAttribute attr;
  attr.attribute_id = "value";
  attr.attribute_value = date;
  d.element_attributes.emplace_back(attr);

  XMLElement el;
  el.element_type = XMLElement::ElementContent;
  el.content = date;
  d.elements.emplace_back(el);

  (*elements.begin())->elements.emplace_back(d);
}

void
FBDCreation::addCoverPage(const std::string &target,
                          const std::string &base64_image)
{
  if(base64_image.empty())
    {
      return void();
    }

  std::vector<XMLElement *> elements = std::move(searchTarget(target));

  XMLElement cover_page;
  cover_page.element_name = "coverpage";
  cover_page.element_type = XMLElement::OrdinaryElement;

  XMLElement image;
  image.element_name = "image";
  image.element_type = XMLElement::OrdinaryElement;
  image.empty = XMLElement::XML;

  XMLElementAttribute attr;
  attr.attribute_id = "l:href";
  if(target == "title-info")
    {
      attr.attribute_value = "#cover.jpg";
    }
  else
    {
      attr.attribute_value = "#srccover.jpg";
    }
  image.element_attributes.emplace_back(attr);

  cover_page.elements.emplace_back(image);

  (*elements.begin())->elements.emplace_back(cover_page);

  elements = std::move(searchTarget("FictionBook"));

  cover_page = XMLElement();
  cover_page.element_name = "binary";
  cover_page.element_type = XMLElement::OrdinaryElement;

  attr = XMLElementAttribute();
  attr.attribute_id = "content-type";
  attr.attribute_value = "image/jpg";
  cover_page.element_attributes.emplace_back(attr);

  attr = XMLElementAttribute();
  attr.attribute_id = "id";
  if(target == "title-info")
    {
      attr.attribute_value = "cover.jpg";
    }
  else
    {
      attr.attribute_value = "srccover.jpg";
    }
  cover_page.element_attributes.emplace_back(attr);

  image = XMLElement();
  image.element_type = XMLElement::ElementContent;
  image.content = base64_image;
  cover_page.elements.emplace_back(image);

  (*elements.begin())->elements.emplace_back(cover_page);
}

void
FBDCreation::addSequence(const std::string &target, const UDBElement &sequence)
{
  std::vector<XMLElement *> elements = std::move(searchTarget(target));

  XMLElement seq;
  seq.element_name = "sequence";
  seq.element_type = XMLElement::OrdinaryElement;
  seq.empty = XMLElement::XML;

  auto it
      = std::find_if(sequence.subelements.begin(), sequence.subelements.end(),
                     [this](const UDBElement &el)
                       {
                         return bid.getId(el) == BaseID::SequenceName;
                       });
  if(it == sequence.subelements.end())
    {
      return void();
    }
  if(it->content.empty())
    {
      return void();
    }
  XMLElementAttribute attr;
  attr.attribute_id = "name";
  attr.attribute_value = it->content;
  seq.element_attributes.emplace_back(attr);

  it = std::find_if(sequence.subelements.begin(), sequence.subelements.end(),
                    [this](const UDBElement &el)
                      {
                        return bid.getId(el) == BaseID::SequenceNumber;
                      });
  if(it != sequence.subelements.end())
    {
      if(!it->content.empty())
        {
          attr = XMLElementAttribute();
          attr.attribute_id = "number";
          attr.attribute_value = it->content;
          seq.element_attributes.emplace_back(attr);
        }
    }
  (*elements.begin())->elements.emplace_back(seq);
}

void
FBDCreation::addHistory(const std::string &target, const std::string &history)
{
  if(history.empty())
    {
      return void();
    }

  std::vector<XMLElement *> elements = std::move(searchTarget(target));

  XMLElement hist;
  hist.element_name = "history";
  hist.element_type = XMLElement::OrdinaryElement;

  addParagraphs(hist, history);

  (*elements.begin())->elements.emplace_back(hist);
}

void
FBDCreation::addCustomInfo(const std::string &info)
{
  if(info.empty())
    {
      return void();
    }

  std::vector<XMLElement *> elements = std::move(searchTarget("description"));

  XMLElement info_el;
  info_el.element_name = "custom-info";
  info_el.element_type = XMLElement::OrdinaryElement;

  XMLElementAttribute attr;
  attr.attribute_id = "info-type";
  info_el.element_attributes.emplace_back(attr);

  XMLElement el;
  el.element_type = XMLElement::ElementContent;
  el.content = info;
  info_el.elements.emplace_back(el);

  (*elements.begin())->elements.emplace_back(info_el);
}

std::string
FBDCreation::writeToBuffer()
{
  std::string result;

  XMLAlgorithms::writeXML(fbd, result);

  return result;
}

std::vector<XMLElement *>
FBDCreation::searchTarget(const std::string &target)
{
  std::vector<XMLElement *> elements;
  XMLAlgorithms::searchElement(fbd, target, elements);
  if(elements.empty())
    {
      throw std::runtime_error(
          "FBDCreation::searchTarget: cannot find target");
    }
  return elements;
}

void
FBDCreation::createTemplate()
{
  XMLElement el;
  el.element_name = "xml";
  el.element_type = XMLElement::ProgramControlElement;

  XMLElementAttribute attr;
  attr.attribute_id = "version";
  attr.attribute_value = "1.0";
  el.element_attributes.emplace_back(attr);

  attr = XMLElementAttribute();
  attr.attribute_id = "encoding";
  attr.attribute_value = "UTF-8";
  el.element_attributes.emplace_back(attr);

  fbd.emplace_back(el);

  el = XMLElement();
  el.element_name = "FictionBook";
  el.element_type = XMLElement::OrdinaryElement;

  attr = XMLElementAttribute();
  attr.attribute_id = "xmlns";
  attr.attribute_value = "http://www.gribuser.ru/xml/fictionbook/2.0";
  el.element_attributes.emplace_back(attr);

  attr = XMLElementAttribute();
  attr.attribute_id = "xmlns:l";
  attr.attribute_value = "http://www.w3.org/1999/xlink";
  el.element_attributes.emplace_back(attr);

  XMLElement descr;
  descr.element_name = "description";
  descr.element_type = XMLElement::OrdinaryElement;

  el.elements.emplace_back(descr);

  fbd.emplace_back(el);
}

void
FBDCreation::addParagraphs(XMLElement &result, const std::string &text)
{
  std::string find_str("\n");
  std::string::size_type n = 0;
  while(n < text.size())
    {
      XMLElement p;
      p.element_name = "p";
      p.element_type = XMLElement::OrdinaryElement;
      std::string::size_type n2 = text.find(find_str, n);
      if(n2 == std::string::npos)
        {
          XMLElement el;
          el.element_type = XMLElement::ElementContent;
          std::copy(text.begin() + n, text.end(),
                    std::back_inserter(el.content));
          if(!el.content.empty())
            {
              p.elements.emplace_back(el);
              result.elements.emplace_back(p);
            }
          n = n2;
        }
      else
        {
          XMLElement el;
          el.element_type = XMLElement::ElementContent;
          std::copy(text.begin() + n, text.begin() + n2,
                    std::back_inserter(el.content));
          if(!el.content.empty())
            {
              p.elements.emplace_back(el);
              result.elements.emplace_back(p);
            }
          n = n2 + find_str.size();
        }
    }
}
