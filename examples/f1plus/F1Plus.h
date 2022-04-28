/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <future>
#include <cstdint>

#include <cabl/cabl.h>

#ifdef __APPLE__
#define __MACOSX_CORE__ 1
#elif defined(_WIN32)
#define __WINDOWS_MM__ 1
#endif
#include "RtMidi.h"

namespace sl
{

using namespace cabl;

class F1Plus : public Client
{
public:

  F1Plus();

  void initDevice() override;
  void render() override;

  void buttonChanged(Device::Button button_, bool buttonState_, bool shiftState_) override;
  void keyChanged(unsigned index_, double value_, bool shiftPressed) override;
  void controlChanged(unsigned pot_, double value_, bool shiftPressed) override;

  void updatePads();

private:
  RtMidiOut* m_pMidiout;

  std::array<bool, 8> trackMutes = { true, true, true, true, true, true, true, true};
  std::array<bool, 8> trackSolos = { false, false, false, false, false, false, false, false};

  void toggleMuteTrack(uint8_t track);
  void muteTrack(uint8_t track, bool mute);

  void toggleSoloTrack(uint8_t track);
  void soloTrack(uint8_t track, bool mute);

  void volumeTrack(uint8_t track, double volume);
  void filterTrack(uint8_t track, double filter);

  void sendMIDIControlChangeMessage(uint8_t channel, uint8_t cc, uint8_t data);
  void sendMIDINoteMessage(uint8_t channel, uint8_t note, bool on);
  void sendMIDIMessage(std::vector<unsigned char>);

};

} // namespace sl
