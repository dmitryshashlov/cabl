/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "F1Plus.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include <unmidify.hpp>

#include <cabl/gfx/TextDisplay.h>

namespace
{

const sl::Color kEuklidColor_Red = {60, 0, 0, 80};

}

//--------------------------------------------------------------------------------------------------

namespace sl
{

using namespace midi;
using namespace std::placeholders;

//--------------------------------------------------------------------------------------------------

F1Plus::F1Plus()
  : m_pMidiout(new RtMidiOut)
{
  m_pMidiout->openVirtualPort("F1Plus");
}

//--------------------------------------------------------------------------------------------------

void F1Plus::initDevice()
{
  device()->setKeyLed(0, kEuklidColor_Red);
}

//--------------------------------------------------------------------------------------------------

void F1Plus::render()
{
  updatePads();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::buttonChanged(Device::Button button_, bool buttonState_, bool shiftState_)
{
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::keyChanged(unsigned index_, double value_, bool shiftPressed_)
{
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::controlChanged(unsigned pot_, double value_, bool shiftPressed_)
{

  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::updatePads()
{
   // nothing here now 
}

//--------------------------------------------------------------------------------------------------

} // namespace sl
