/***************************************************************************
                 y2controlcenterdoc.cpp  -  unused, created by kdevelop
                             -------------------
    begin                : Mit Okt 18 14:21:09 CEST 2000
    copyright            : (C) 2000 by SuSE GmbH
    author               : lnussel@suse.de
 ***************************************************************************/

#include "y2controlcenterdoc.h"

Y2ControlCenterDoc::Y2ControlCenterDoc()
{
  modified = false;
}

Y2ControlCenterDoc::~Y2ControlCenterDoc()
{
}

void Y2ControlCenterDoc::newDoc()
{
}

bool Y2ControlCenterDoc::save()
{
  return true;
}

bool Y2ControlCenterDoc::saveAs(const QString &filename)
{
  return true;
}

bool Y2ControlCenterDoc::load(const QString &filename)
{
  emit documentChanged();
  return true;
}

bool Y2ControlCenterDoc::isModified() const
{
  return modified;
}

#include <y2controlcenterdoc.moc.cpp>
