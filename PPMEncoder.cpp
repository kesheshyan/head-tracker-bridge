#include "PPMEncoder.h"
#include "BBTimer.h"

PPMEncoder ppmEncoder;
BBTimer ppmTimer1(BB_TIMER4);


void ppmTimerCallback() {
  ppmEncoder.interrupt();
}

void PPMEncoder::begin(uint8_t pin) {
  begin(pin, PPM_DEFAULT_CHANNELS, false);
}

void PPMEncoder::begin(uint8_t pin, uint8_t ch) {
  begin(pin, ch, false);
}

void PPMEncoder::begin(uint8_t pin, uint8_t ch, boolean inverted) {
  // Store on/off-State in variable to avoid another if in timing-critical interrupt
  onState = (inverted) ? HIGH : LOW;
  offState = (inverted) ? LOW : HIGH;
  
  pinMode(pin, OUTPUT);
  digitalWrite(pin, offState);

  state = true;
  elapsedUs = 0;
  currentChannel = 0;

  numChannels = ch;
  outputPin = pin;

  for (uint8_t ch = 0; ch < numChannels; ch++) {
    setChannelPercent(ch, 0);
  }

  // Setting the timers
  ppmTimer1.setupTimer(100, ppmTimerCallback);
  ppmTimer1.timerStart();

}

void PPMEncoder::setChannel(uint8_t channel, uint16_t value) {
  channels[channel] = constrain(value, PPMEncoder::MIN, PPMEncoder::MAX);
}

void PPMEncoder::setChannelPercent(uint8_t channel, uint8_t percent) {
  percent = constrain(percent, 0, 100);
  setChannel(channel, map(percent, 0, 100, PPMEncoder::MIN, PPMEncoder::MAX));
}

void PPMEncoder::interrupt() {
  if (state) {
    digitalWrite(outputPin, onState);
    ppmTimer1.updatePeriod(PPM_PULSE_LENGTH_uS * 2);

  } else {
    digitalWrite(outputPin, offState);

    if (currentChannel >= numChannels) {
      currentChannel = 0;
      elapsedUs = elapsedUs + PPM_PULSE_LENGTH_uS;
      ppmTimer1.updatePeriod((PPM_FRAME_LENGTH_uS - elapsedUs) * 2);
      elapsedUs = 0;
    } else {
      ppmTimer1.updatePeriod((channels[currentChannel] - PPM_PULSE_LENGTH_uS) * 2);
      elapsedUs = elapsedUs + channels[currentChannel];

      currentChannel++;
    }
  }

  state = !state;
}
