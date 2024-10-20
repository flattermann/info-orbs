#include "model/onvistaDataModel.h"
#include "utils.h"
#include <config.h>

StockDataModel::StockDataModel() {
}

StockDataModel &StockDataModel::setSymbol(String symbol) {
    Serial.println("Parsing "+symbol);
    int idx1 = symbol.indexOf('@');
    Serial.printf("Idx1: %d\n", idx1);
    if (idx1 != -1) {
        m_symbol = symbol.substring(0, idx1);
        int idx2 = symbol.indexOf('@', idx1+1);
        Serial.printf("Idx2: %d\n", idx2);
        if (idx2 != -1) {
            m_type = symbol.substring(idx1+1, idx2);
            int idx3 = symbol.indexOf('@', idx2+1);
            Serial.printf("Idx3: %d\n", idx3);
            if (idx3 != -1) {
                m_id = symbol.substring(idx2+1, idx3);
                m_exchangeCode = symbol.substring(idx3+1);
            }
        }
    } else {
        m_symbol = symbol;
    }
    Serial.printf("Symbol parsed: %s, %s, %s, %s\n", m_symbol, m_type, m_id, m_exchangeCode);
    return *this;
}

String StockDataModel::getSymbolType() {
    return m_type;
}

String StockDataModel::getSymbolId() {
    return m_id;
}

String StockDataModel::getSymbol() {
    return m_symbol;
}

String StockDataModel::getExchangeCode() {
    return m_exchangeCode;
}

// String StockDataModel::getSymbolShort() {
//     int sepIdx = 0;
//     int sepIdx = m_symbol.indexOf('.');
//     if (sepIdx != -1) {
//         String result = m_symbol.substring(0, sepIdx);
//         return result;
//     } else {
//         return m_symbol;
//     }
// }

StockDataModel &StockDataModel::setCurrentPrice(float currentPrice) {
    if (m_currentPrice != currentPrice) {
        m_currentPrice = currentPrice;
        m_changed = true;
    }
    return *this;
}
float StockDataModel::getCurrentPrice() {
    return m_currentPrice;
}
String StockDataModel::getCurrentPrice(int8_t digits) {
    return Utils::formatFloat(m_currentPrice, digits);
}

StockDataModel &StockDataModel::setVolume(float volume) {
    if (m_volume != volume) {
        m_volume = volume;
        m_changed = true;
    }
    return *this;
}

float StockDataModel::getVolume() {
    return m_volume;
}

String StockDataModel::getVolume(int8_t digits) {
    return Utils::formatFloat(m_volume, digits);
}

StockDataModel &StockDataModel::setPriceChange(float priceChange) {
    if (m_priceChange != priceChange) {
        m_priceChange = priceChange;
        m_changed = true;
    }
    return *this;
}
float StockDataModel::getPriceChange() {
    return m_priceChange;
}
String StockDataModel::getPriceChange(int8_t digits) {
    return Utils::formatFloat(m_priceChange, digits);
}

StockDataModel &StockDataModel::setPercentChange(float percentChange) {
    if (m_percentChange != percentChange) {
        m_percentChange = percentChange;
        m_changed = true;
    }
    return *this;
}
float StockDataModel::getPercentChange() {
    return m_percentChange;
}

String StockDataModel::getPercentChange(int8_t digits) {
    return Utils::formatFloat(m_percentChange*100, digits);
}

bool StockDataModel::isChanged() {
    return m_changed;
}
StockDataModel &StockDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}
