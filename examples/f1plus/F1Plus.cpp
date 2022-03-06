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

enum MidiMessageType : unsigned char{
    ControlChange = 0xB0,
    NoteOff = 0x80,
    NoteOn = 0x90,
};

enum MidiNote {
    C3 = 48,
    Cs3,
    D3,
    Ds3,
    E3,
    F3,
    Fs3,
    G3,
    Gs3,
    A3,
    As3,
    B3
};

}

namespace OT
{

enum MappingTarget {
    REC_AB,
    REC_CD,
    REC_INT,
    TRACK_UP,
    TRACK_DOWN
};

std::map<MappingTarget, std::vector<unsigned char>> MidiMapping {
    { REC_AB,       { MidiMessageType::NoteOn, MidiNote::Cs3,   1 } },
    { REC_CD,       { MidiMessageType::NoteOn, MidiNote::D3,    1 } },
    { REC_INT,      { MidiMessageType::NoteOn, MidiNote::Ds3,   1 } },
    { TRACK_UP,     { MidiMessageType::NoteOn, MidiNote::Gs3,   1 } },
    { TRACK_DOWN,   { MidiMessageType::NoteOn, MidiNote::A3,    1 } }
};

std::map<sl::Device::Button, MappingTarget> KeyMapping {
    { sl::Device::Button::Sync,     REC_AB},
    { sl::Device::Button::Quant,    REC_CD },
    { sl::Device::Button::Capture,  REC_INT},
    { sl::Device::Button::Reverse,  TRACK_UP },
    { sl::Device::Button::Type,     TRACK_DOWN }
};

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
#if defined(__APPLE__)
  for (uint8_t i = 0 ; i < m_pMidiout->getPortCount() ; i++ ) {
#ifdef DEBUG
    M_LOG("MIDI port: " << m_pMidiout->getPortName(i));
#endif
    if (m_pMidiout->getPortName(i).compare("Midihub MH-0MRSSEG Port 1") == 0) {
        m_pMidiout->openPort(i, "F1PlusMidiOut");
    }
  }
#else
  m_pMidiout->openVirtualPort("F1PlusMidiOut");
#endif
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
#ifdef DEBUG
  M_LOG("[Client] buttonChanged " << static_cast<int>(button_) << " ("
                                   << (buttonState_ ? "clicked " : "released")
                                   << ") "
                                   << (shiftState_ ? " SHIFT" : ""));
#endif

  sendMIDIMessage(OT::MidiMapping[OT::KeyMapping[button_]]);
  requestDeviceUpdate();
}

//--------------------------------------------------------------------------------------------------

void F1Plus::keyChanged(unsigned index_, double value_, bool shiftPressed_)
{
#ifdef DEBUG
  M_LOG("[Client] keyChanged " << static_cast<int>(index_) << " (" << value_ << ") "
                               << (shiftPressed_ ? " SHIFT" : ""));
#endif

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
#ifdef DEBUG
  M_LOG("[Client] controlChanged " << static_cast<int>(pot_) << " (" << value_ << ") "
                                   << (shiftPressed_ ? " SHIFT" : ""));
#endif

  if (0 <= pot_ && pot_ < 4) {          // track filters
      filterTrack(pot_, value_ / 4);
  }
  else if (4 <= pot_ && pot_ < 8) {     // track volumes
      volumeTrack(pot_-4, value_ / 4);
  }
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
#ifdef DEBUG
    M_LOG("Mute track " << static_cast<int>(track + 1) << " (" << mute << ") ");
#endif
    trackMutes[track] = mute;

    sendMIDIControlChangeMessage(5 + track, 94, mute ? 2 : 0);
}

//--------------------------------------------------------------------------------------------------

void F1Plus::toggleSoloTrack(uint8_t track)
{
    soloTrack(track, !trackSolos[track]);
}

void F1Plus::soloTrack(uint8_t track, bool solo)
{
#ifdef DEBUG
    M_LOG("Solo track " << static_cast<int>(track + 1) << " (" << solo << ") ");
#endif
    trackSolos[track] = solo;

    // send mutes to all tracks but this
    for (int i = 0 ; i < 4 ; i++) {
        if ( i == track) {
            muteTrack(i, false);
            continue;
        }
        muteTrack(i, true);
        trackSolos[track] = false;
    }
}

//--------------------------------------------------------------------------------------------------

void F1Plus::volumeTrack(uint8_t track, double volume)
{
#ifdef DEBUG
    M_LOG("Volume track " << static_cast<int>(track + 1) << " (" << volume << ") ");
#endif
    sendMIDIControlChangeMessage(5 + track, 95, 128 * volume);
}

void F1Plus::filterTrack(uint8_t track, double filter)
{
#ifdef DEBUG
    M_LOG("Filter track " << static_cast<int>(track + 1) << " (" << filter << ") ");
#endif
    sendMIDIControlChangeMessage(5 + track, 23, 128 * filter);
}

//--------------------------------------------------------------------------------------------------

void F1Plus::sendMIDIControlChangeMessage(uint8_t channel, uint8_t cc, uint8_t data)
{
#ifdef DEBUG
    M_LOG("Send MIDI CC message: ch " << static_cast<int>(channel) 
                                      << " cc " << static_cast<int>(cc) 
                                      << " val " << static_cast<int>(data));
#endif
    std::vector<unsigned char> message 
        = { static_cast<unsigned char>(MidiMessageType::ControlChange + channel - 1), cc, data };
    m_pMidiout->sendMessage(&message);
}

//--------------------------------------------------------------------------------------------------

void F1Plus::sendMIDINoteMessage(uint8_t channel, uint8_t note, bool on)
{
#ifdef DEBUG
    M_LOG("Send MIDI note message: ch " << static_cast<int>(channel) 
                                        << " note " << static_cast<int>(note) 
                                        << " " << on);
#endif
    unsigned char velocity = 100;
    unsigned char messageType = on ? MidiMessageType::NoteOn : MidiMessageType::NoteOff;
    std::vector<unsigned char> message 
        = { static_cast<unsigned char>(messageType + channel - 1), note, velocity };
    m_pMidiout->sendMessage(&message);
}

//--------------------------------------------------------------------------------------------------

void F1Plus::sendMIDIMessage(std::vector<unsigned char> message)
{
#ifdef DEBUG
    M_LOG("Send MIDI message: " << static_cast<int>(message[0]) 
                                << " " << static_cast<int>(message[1]) 
                                << " " << static_cast<int>(message[2]));
#endif
    m_pMidiout->sendMessage(&message);
}

} // namespace sl
