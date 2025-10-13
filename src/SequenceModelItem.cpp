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

#include <SequenceModelItem.h>
#include <algorithm>

Glib::RefPtr<SequenceModelItem>
SequenceModelItem::create()
{
  return Glib::make_refptr_for_instance(new SequenceModelItem);
}

void
SequenceModelItem::addLabel(Gtk::Label *lab)
{
  auto it = std::find(lab_v.begin(), lab_v.end(), lab);
  if(it == lab_v.end())
    {
      lab_v.push_back(lab);
    }
}

void
SequenceModelItem::setActive()
{
  for(size_t i = 0; i < lab_v.size(); i++)
    {
      lab_v[i]->set_name("selectedLab");
    }
}

void
SequenceModelItem::setInactve()
{
  for(size_t i = 0; i < lab_v.size(); i++)
    {
      lab_v[i]->set_name("windowLabel");
    }
}

void
SequenceModelItem::removeLabel(Gtk::Label *lab)
{
  lab_v.erase(std::remove(lab_v.begin(), lab_v.end(), lab), lab_v.end());
}

SequenceModelItem::SequenceModelItem()
{
  lab_v.reserve(2);
}
