#ifndef ONVISTA_DATA_MODEL_H
#define ONVISTA_DATA_MODEL_H

#include <Arduino.h>

#include <iomanip>

class StockDataModel {
   public:
    StockDataModel();
    StockDataModel &setSymbol(String symbol);
    String getSymbol();
    String getSymbolType();
    String getSymbolId();
    String getExchangeCode();
    StockDataModel &setCurrentPrice(float currentPrice);
    float getCurrentPrice();
    String getCurrentPrice(int8_t digits);
    StockDataModel &setVolume(float volume);
    float getVolume();
    String getVolume(int8_t digits);
    StockDataModel &setPriceChange(float change);
    float getPriceChange();
    String getPriceChange(int8_t digits);
    StockDataModel &setPercentChange(float percentChange);
    float getPercentChange();
    String getPercentChange(int8_t digits);

    bool isChanged();
    StockDataModel &setChangedStatus(bool changed);

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
