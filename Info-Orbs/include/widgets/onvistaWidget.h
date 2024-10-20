#ifndef ONVISTA_WIDGET_H
#define ONVISTA_WIDGET_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <globalTime.h>

#include "model/onvistaDataModel.h"
#include "widget.h"

#define MAX_STOCKS 5

class OnvistaWidget : public Widget {
   public:
    OnvistaWidget(ScreenManager &manager);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void changeMode() override;

   private:
    void getStockData(OnvistaDataModel &stock);
    void displayStock(int8_t displayIndex, OnvistaDataModel &stock, uint32_t backgroundColor, uint32_t textColor);
    void displayClock(int displayIndex, uint32_t background, uint32_t color);

    GlobalTime* m_time;

    unsigned long m_stockDelay = 900000;  //default to 15m between updates
    unsigned long m_stockDelayPrev = 0;

    unsigned long m_clockDelay = 60000;  // update the clock every minute
    unsigned long m_clockDelayPrev = 0;

    OnvistaDataModel m_stocks[MAX_STOCKS];
    int8_t m_stockCount;
};
#endif  // STOCK_WIDGET_H
