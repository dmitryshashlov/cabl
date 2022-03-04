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

const sl::Color kColor_Red = {60, 0, 0, 80};
const sl::Color kColor_Green = {0, 60, 0, 80};
const sl::Color kColor_Blue = {0, 0, 60, 80};
const sl::Color kColor_Purple = {35, 0, 60, 80};
const sl::Color kColor_White = {60, 60, 60, 80};

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
  updatePads();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::render()
{
  updatePads();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::buttonChanged(Device::Button button_, bool buttonState_, bool shiftState_)
{
  M_LOG("[Client] encoderChanged " << static_cast<int>(button_) << " ("
                                   << (buttonState_ ? "clicked " : "released")
                                   << ") "
                                   << (shiftState_ ? " SHIFT" : ""));

  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::keyChanged(unsigned index_, double value_, bool shiftPressed_)
{
  M_LOG("[Client] keyChanged " << static_cast<int>(index_) << " (" << value_ << ") "
                               << (shiftPressed_ ? " SHIFT" : ""));

  if (value_ != 1) return;

  uint8_t btn = static_cast<uint8_t>(index_);
  if (4 <= btn && btn < 8) {            // track mutes
      toggleMuteTrack(7-btn);
  } else if (0 <= btn && btn < 4) {     // track solos
      toggleSoloTrack(3-btn);
  }
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::controlChanged(unsigned pot_, double value_, bool shiftPressed_)
{
  M_LOG("[Client] controlChanged " << static_cast<int>(pot_) << " (" << value_ << ") "
                                   << (shiftPressed_ ? " SHIFT" : ""));
  volumeTrack(pot_-4, value_ / 4);
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::updatePads()
{
  // Digitone track mutes
  for (uint8_t i = 0; i < 4; i++) {
    device()->setKeyLed(i, trackMutes[i] ? kColor_Purple : kColor_Green);
  }

  // Digitone track solos
  for (uint8_t i = 4; i < 8; i++) {
    device()->setKeyLed(i, trackSolos[i%4] ? kColor_Green : kColor_White);
  }
}

//--------------------------------------------------------------------------------------------------

void F1Plus::toggleMuteTrack(uint8_t track)
{
    muteTrack(track, !trackMutes[track]);
}

void F1Plus::muteTrack(uint8_t track, bool mute)
{
    trackMutes[track] = mute;
    // send MIDI
}

//--------------------------------------------------------------------------------------------------

void F1Plus::toggleSoloTrack(uint8_t track)
{
    soloTrack(track, !trackSolos[track]);
}

void F1Plus::soloTrack(uint8_t track, bool solo)
{
    trackSolos[track] = solo;
    // send MIDI
}

//--------------------------------------------------------------------------------------------------

void F1Plus::volumeTrack(uint8_t track, double volume)
{
    M_LOG("Volume track " << static_cast<int>(track + 1) << " (" << volume << ") ");

    // send MIDI
}


} // namespace sl
