#ifndef ONVISTA_DATA_MODEL_H
#define ONVISTA_DATA_MODEL_H

#include <Arduino.h>

#include <iomanip>

class OnvistaDataModel {
   public:
    OnvistaDataModel();
    OnvistaDataModel &setSymbol(String symbol);
    String getSymbol();
    String getSymbolType();
    String getSymbolId();
    String getExchangeCode();
    OnvistaDataModel &setCurrentPrice(float currentPrice);
    float getCurrentPrice();
    String getCurrentPrice(int8_t digits);
    OnvistaDataModel &setVolume(float volume);
    float getVolume();
    String getVolume(int8_t digits);
    OnvistaDataModel &setPriceChange(float change);
    float getPriceChange();
    String getPriceChange(int8_t digits);
    OnvistaDataModel &setPercentChange(float percentChange);
    float getPercentChange();
    String getPercentChange(int8_t digits);

    bool isChanged();
    OnvistaDataModel &setChangedStatus(bool changed);

   private:
    String m_symbol = "";
    String m_type = "";
    String m_id = "";
    String m_exchangeCode = "";
    float m_currentPrice = 0.0;
    float m_volume = 0.0;
    float m_priceChange = 0.0;
    float m_percentChange = 0.0;
    bool m_changed = false;
};

#endif  // STOCK_DATA_MODEL_H
