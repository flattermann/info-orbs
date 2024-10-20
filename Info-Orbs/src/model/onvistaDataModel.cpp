#include "model/onvistaDataModel.h"
#include "utils.h"
#include <config.h>

OnvistaDataModel::OnvistaDataModel() {
}

OnvistaDataModel &OnvistaDataModel::setSymbol(String symbol) {
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

String OnvistaDataModel::getSymbolType() {
    return m_type;
}

String OnvistaDataModel::getSymbolId() {
    return m_id;
}

String OnvistaDataModel::getSymbol() {
    return m_symbol;
}

String OnvistaDataModel::getExchangeCode() {
    return m_exchangeCode;
}

// String OnvistaDataModel::getSymbolShort() {
//     int sepIdx = 0;
//     int sepIdx = m_symbol.indexOf('.');
//     if (sepIdx != -1) {
//         String result = m_symbol.substring(0, sepIdx);
//         return result;
//     } else {
//         return m_symbol;
//     }
// }

OnvistaDataModel &OnvistaDataModel::setCurrentPrice(float currentPrice) {
    if (m_currentPrice != currentPrice) {
        m_currentPrice = currentPrice;
        m_changed = true;
    }
    return *this;
}
float OnvistaDataModel::getCurrentPrice() {
    return m_currentPrice;
}
String OnvistaDataModel::getCurrentPrice(int8_t digits) {
    return Utils::formatFloat(m_currentPrice, digits);
}

OnvistaDataModel &OnvistaDataModel::setVolume(float volume) {
    if (m_volume != volume) {
        m_volume = volume;
        m_changed = true;
    }
    return *this;
}

float OnvistaDataModel::getVolume() {
    return m_volume;
}

String OnvistaDataModel::getVolume(int8_t digits) {
    return Utils::formatFloat(m_volume, digits);
}

OnvistaDataModel &OnvistaDataModel::setPriceChange(float priceChange) {
    if (m_priceChange != priceChange) {
        m_priceChange = priceChange;
        m_changed = true;
    }
    return *this;
}
float OnvistaDataModel::getPriceChange() {
    return m_priceChange;
}
String OnvistaDataModel::getPriceChange(int8_t digits) {
    return Utils::formatFloat(m_priceChange, digits);
}

OnvistaDataModel &OnvistaDataModel::setPercentChange(float percentChange) {
    if (m_percentChange != percentChange) {
        m_percentChange = percentChange;
        m_changed = true;
    }
    return *this;
}
float OnvistaDataModel::getPercentChange() {
    return m_percentChange;
}

String OnvistaDataModel::getPercentChange(int8_t digits) {
    return Utils::formatFloat(m_percentChange*100, digits);
}

bool OnvistaDataModel::isChanged() {
    return m_changed;
}
OnvistaDataModel &OnvistaDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}
