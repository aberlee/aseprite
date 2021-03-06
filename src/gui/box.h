// ASE gui library
// Copyright (C) 2001-2011  David Capello
//
// This source file is ditributed under a BSD-like license, please
// read LICENSE.txt for more information.

#ifndef GUI_BOX_H_INCLUDED
#define GUI_BOX_H_INCLUDED

#include "base/compiler_specific.h"
#include "gui/widget.h"

class Box : public Widget
{
public:
  Box(int align);

protected:
  // Events
  bool onProcessMessage(Message* msg) OVERRIDE;
  void onPreferredSize(PreferredSizeEvent& ev) OVERRIDE;
  void onPaint(PaintEvent& ev) OVERRIDE;

private:
  void box_set_position(JRect rect);
};

class VBox : public Box
{
public:
  VBox() : Box(JI_VERTICAL) { }
};

class HBox : public Box
{
public:
  HBox() : Box(JI_HORIZONTAL) { }
};

class BoxFiller : public Box
{
public:
  BoxFiller() : Box(JI_HORIZONTAL) {
    jwidget_expansive(this, true);
  }
};

#endif
