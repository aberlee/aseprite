// ASE gui library
// Copyright (C) 2001-2011  David Capello
//
// This source file is ditributed under a BSD-like license, please
// read LICENSE.txt for more information.

#ifndef GUI_CLOSE_EVENT_H_INCLUDED
#define GUI_CLOSE_EVENT_H_INCLUDED

#include "gui/event.h"

class CloseEvent : public Event
{
public:
  enum Trigger {
    ByCode,			// The CloseEvent was generated by code.
    ByUser,			// The CloseEvent was generated by the user.
  };

  CloseEvent(Component* source, Trigger trigger)
    : Event(source)
    , m_trigger(trigger){ }

  Trigger getTrigger() const { return m_trigger; }

private:
  Trigger m_trigger;
};

#endif	// GUI_CLOSE_EVENT_H_INCLUDED
