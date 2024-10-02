#include "nrf_common.h"
#include "nrf_spectrum.h"
#include "../../core/display.h"
#include "../../core/mykeyboard.h"

const uint8_t cacheMax = 4;
const uint8_t numChannels = 126; // 0-125 are supported
const uint16_t margin = 1;  // use 1 pixel margin for markers on each side of chart
const uint16_t barWidth = (WIDTH - (margin * 2)) / numChannels;
const uint16_t chartHeight = HEIGHT - 10;
const uint16_t chartWidth = margin * 2 + (numChannels * barWidth);  

struct ChannelHistory {
  /// max peak value is (at most) 2 * CACHE_MAX to allow for half-step decays
  uint8_t maxPeak = 0;

  /// Push a signal's value into cached history while popping
  /// oldest cached value. This also sets the maxPeak value.
  /// @returns The sum of signals found in the cached history
  uint8_t push(bool value) {
    uint8_t sum = value;
    for (uint8_t i = 0; i < cacheMax - 1; ++i) {
      history[i] = history[i + 1];
      sum += history[i];
    }
    history[cacheMax - 1] = value;
    maxPeak = std::max(sum * 2, static_cast<int>(maxPeak));  // sum * 2 to allow half-step decay
    return sum;
  }

private:
  bool history[cacheMax] = { 0 };
};
/// Draw the chart axis and labels
void displayChartAxis() {
// constant chart size attributes
  
  // draw base line
  tft.drawLine(0, chartHeight + 1, chartWidth - margin, chartHeight + 1, FGCOLOR);

  // draw base line border
  tft.drawLine(margin, HEIGHT, margin, chartHeight - 2, FGCOLOR);
  tft.drawLine(chartWidth - margin, HEIGHT, chartWidth - margin, chartHeight - 2, FGCOLOR);

  // draw scalar marks
  for (uint8_t i = 0; i < cacheMax; ++i) {
    uint8_t scalarHeight = chartHeight * i / cacheMax;
    tft.drawLine(0, scalarHeight, chartWidth, scalarHeight, FGCOLOR);
  }

  // draw channel range labels
  tft.setTextSize(1);
  tft.setTextColor(FGCOLOR);
  uint8_t maxChannelDigits = 0;
  uint8_t tmp = numChannels;
  while (tmp) {
    maxChannelDigits += 1;
    tmp /= 10;
  }
  tft.setCursor(chartWidth - (7 * maxChannelDigits), chartHeight + 3);
  tft.print(numChannels - 1);
  tft.setCursor(margin + 2, chartHeight + 3);
  tft.print(0);


}


/// Scan a specified channel and return the resulting flag
bool scanChannel(uint8_t channel) {
  NRFradio.setChannel(channel);

  // Listen for a little
  NRFradio.startListening();
  delayMicroseconds(130);
  bool foundSignal = NRFradio.testRPD();
  NRFradio.stopListening();

  // Did we get a signal?
  if (foundSignal || NRFradio.testRPD() || NRFradio.available()) {
    NRFradio.flush_rx();  // discard packets of noise
    return true;
  }
  return false;
}


void nrf_spectrum() {
#if defined(HAS_SCREEN)
    if(nrf_start()) {
        const uint8_t noiseAddress[][2] = { { 0x55, 0x55 }, { 0xAA, 0xAA }, { 0xA0, 0xAA }, { 0xAB, 0xAA }, { 0xAC, 0xAA }, { 0xAD, 0xAA } };
        NRFradio.setAutoAck(false);   // Don't acknowledge arbitrary signals
        NRFradio.disableCRC();        // accept any signal we find
        NRFradio.setAddressWidth(2);  // a reverse engineering tactic (not typically recommended)
        for (uint8_t i = 0; i < 6; ++i) {
            NRFradio.openReadingPipe(i, noiseAddress[i]);
        }
    char dataRate = '1'; // use 2 or 3 for debug
    if (dataRate == '2') {
        Serial.println(F("Using 2 Mbps."));
        NRFradio.setDataRate(RF24_2MBPS);
    } else if (dataRate == '3') {
        Serial.println(F("Using 250 kbps."));
        NRFradio.setDataRate(RF24_250KBPS);
    } else {  // dataRate == '1' or invalid values
        Serial.println(F("Using 1 Mbps."));
        NRFradio.setDataRate(RF24_1MBPS);
    }

    // Get into standby mode
    NRFradio.startListening();
    NRFradio.stopListening();
    NRFradio.flush_rx();
    ChannelHistory stored[numChannels]; // need to put it into RAM???? 
    while(checkSelPress()); // debounce

    while(!checkSelPress()) {
                // Print out channel measurements, clamped to a single hex digit
        for (uint8_t channel = 0; channel < numChannels; ++channel) {
            bool foundSignal = scanChannel(channel);
            uint8_t cacheSum = stored[channel].push(foundSignal);
            uint8_t x = (barWidth * channel) + 1 + margin - (barWidth * (bool)channel);
            // reset bar for current channel to 0
            tft.fillRect(x, 0, barWidth, chartHeight, BGCOLOR);
            if (stored[channel].maxPeak > cacheSum * 2) {
            // draw a peak line only if it is greater than current sum of cached signal counts
            uint16_t y = chartHeight - (chartHeight * stored[channel].maxPeak / (cacheMax * 2));
            tft.drawLine(x, y, x + barWidth, y, FGCOLOR);
        #ifndef HOLD_PEAKS
            stored[channel].maxPeak -= 1;  // decrement max peak
        #endif
            }
            if (cacheSum) {  // draw the cached signal count
            uint8_t barHeight = chartHeight * cacheSum / cacheMax;
            tft.fillRect(x, chartHeight - barHeight, barWidth, barHeight, FGCOLOR);
            }
        }
    }

    }
    else {
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
    }
#endif
}