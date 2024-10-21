#include "widgets/onvistaWidget.h"

#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <HTTPClient.h>
#include <config.h>

#include <iomanip>

// To use this widget, define STOCK_TICKER_LIST in config.h like this:
// Stocks for onvista are in the format: NAME@TYPE@ID@EXCHANGE
// TYPE IS stocks, funds or crypto
// ID is ISIN:xxx for stocks and etfs, or BTCEUR for crypto
// EXCHANGE is GAT for Tradegate, CCD for CCData (Crypto)
// To find the exchange code, open a stock in OnVista and check the call to "snapshot" in developer tools
// Use "-"" for a blank tft panel and "clock" for a clock like the weather widget
// e.g. for BASF, Vanguard FTSE All World, McDonalds and BTC/EUR:
// #define STOCK_TICKER_LIST "BASF@stocks@ISIN:DE000BASF111@GAT,FTSE@funds@ISIN:IE00BK5BQT80@GAT,MCD@stocks@ISIN:US5801351017@GAT,clock,BTC@crypto@BTCEUR@CCD" // Choose your 5 stokcs to display on the stock tracker
// and set
// #define USE_ONVISTA

OnvistaWidget::OnvistaWidget(ScreenManager &manager) : Widget(manager) {
    #ifdef STOCK_TICKER_LIST
    char stockList[strlen(STOCK_TICKER_LIST) + 1];
    strcpy(stockList, STOCK_TICKER_LIST);

    char *symbol = strtok(stockList, ",");
    m_stockCount = 0;
    do {
        OnvistaDataModel stockModel = OnvistaDataModel();
        stockModel.setSymbol(String(symbol));
        m_stocks[m_stockCount] = stockModel;
        m_stockCount++;
        if (m_stockCount > MAX_STOCKS) {
            Serial.println("MAX STOCKS UNABLE TO ADD MORE");
            break;
        }
    } while (symbol = strtok(nullptr, ","));
    #endif
}

void OnvistaWidget::setup() {
    if (m_stockCount == 0) {
        Serial.println("No stock tickers available");
        return;
    }
    m_time = GlobalTime::getInstance();
}

void OnvistaWidget::draw(bool force) {
    for (int8_t i = 0; i < m_stockCount; i++) {
        if (m_stocks[i].isChanged() || force) {
            displayStock(i, m_stocks[i], TFT_WHITE, TFT_BLACK);
            m_stocks[i].setChangedStatus(false);
        }
    }
}

void OnvistaWidget::update(bool force) {
    if (force || m_stockDelayPrev == 0 || (millis() - m_stockDelayPrev) >= m_stockDelay) {
        setBusy(true);
        for (int8_t i = 0; i < m_stockCount; i++) {
            getStockData(m_stocks[i]);
        }
        setBusy(false);
        m_stockDelayPrev = millis();
    }
    if (force || m_clockDelayPrev == 0 || (millis() - m_clockDelayPrev) >= m_clockDelay) {
        setBusy(true);
        for (int8_t i = 0; i < m_stockCount; i++) {
            if (m_stocks[i].getSymbol() == "clock") {
                m_stocks[i].setChangedStatus(true);
            }
        }
        setBusy(false);
        m_clockDelayPrev = millis();
    }
}

void OnvistaWidget::changeMode() {
    update(true);
}

void OnvistaWidget::getStockData(OnvistaDataModel &stock) {
    if (stock.getSymbolId() == "") {
        return;
    }

    // Default v1
    String apiVersion = "v1";

    // Crypto needs v2
    if (stock.getSymbolType() == "crypto") {
        apiVersion = "v2";
    }

    String httpRequestAddress = "https://api.onvista.de/api/" + apiVersion + "/" + stock.getSymbolType() + "/" + stock.getSymbolId() + "/snapshot";

    Serial.println();
    Serial.println(httpRequestAddress);

    HTTPClient http;
    const char* keys[] = {"Transfer-Encoding"};
    http.collectHeaders(keys, 1);
    http.begin(httpRequestAddress);
    int httpCode = http.GET();
    Serial.printf("HTTP %d, Size %d\n", httpCode, http.getSize());

    if (httpCode == 200) {  // Check for the returning code
        Stream& rawStream = http.getStream();
        ChunkDecodingStream decodedStream(http.getStream());

        // Choose the right stream depending on the Transfer-Encoding header
        // Onvista might send chunked responses
        Stream& response =
            http.header("Transfer-Encoding") == "chunked" ? decodedStream : rawStream;

        // Parse response
        JsonDocument doc;
        JsonDocument filter;
        filter["quoteList"] = true;

        // ReadLoggingStream loggingStream(response, Serial);
        // DeserializationError error = deserializeJson(doc, loggingStream, DeserializationOption::Filter(filter));
        DeserializationError error = deserializeJson(doc, response, DeserializationOption::Filter(filter));

        Serial.println("Type: " + doc["type"].as<String>());

        if (!error) {
            JsonArray quoteArray = doc["quoteList"]["list"];
            JsonObject data;
            Serial.println("Searching for exchange " + stock.getExchangeCode());
            for (JsonVariant quote : quoteArray) {
                if (quote["market"]["codeExchange"] == stock.getExchangeCode()) {
                    data = quote;
                    Serial.println("Found it!");
                    break;
                }
            }

            float currentPrice = data["last"].as<float>();
            if (currentPrice > 0.0) {
                Serial.printf("Price found: %f\n", currentPrice);
                stock.setCurrentPrice(currentPrice);
                float change = data["performance"].as<float>();
                float previous = data["previousLast"].as<float>();
                stock.setPriceChange(change);
                stock.setPercentChange(change/previous);
                stock.setVolume(data["volume"].as<float>());
            } else {
                Serial.println("skipping invalid data for: " + stock.getSymbol());
            }
        } else {
            // Handle JSON deserialization error
            Serial.println("deserializeJson() failed");
            Serial.println(error.c_str());
        }
    } else {
        // Handle HTTP request error
        Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

void OnvistaWidget::displayClock(int displayIndex, uint32_t background, uint32_t color) {
    Serial.printf("displayClock at screen %d\n", displayIndex);
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    int clky = 95;
    int centre = 120;

    display.setTextColor(color);
    display.setTextSize(1);
    display.setTextDatum(MC_DATUM);

    display.fillScreen(background);
    display.setTextColor(color);
    display.setTextSize(2);
    display.setTextDatum(MC_DATUM);
#ifdef WEATHER_UNITS_METRIC
    display.drawString(String(m_time->getDay()) + " " + m_time->getMonthName(), centre, 151, 2);
#else
    display.drawString(m_time->getMonthName() + " " + String(m_time->getDay()), centre, 151, 2);
#endif
    display.setTextSize(3);
    display.drawString(m_time->getWeekday(), centre, 178, 2);
    display.setTextColor(color);
    display.setTextDatum(MR_DATUM);
    display.setTextSize(1);

    display.drawString(m_time->getHourPadded(), centre - 5, clky, 8);

    display.setTextColor(color);
    display.setTextDatum(ML_DATUM);
    display.setTextSize(1);
    display.drawString(m_time->getMinutePadded(), centre + 5, clky, 8);
    display.setTextDatum(MC_DATUM);
    display.setTextColor(color);
    display.drawString(":", centre, clky, 8);
}

void OnvistaWidget::displayStock(int8_t displayIndex, OnvistaDataModel &stock, uint32_t backgroundColor, uint32_t textColor) {
    Serial.println("displayStock - " + stock.getSymbol() + " ~ " + stock.getCurrentPrice());
    if (stock.getSymbol() == "clock") {
        displayClock(displayIndex, TFT_BLACK, TFT_WHITE);
        return;
    }

    if (stock.getCurrentPrice() == 0.0) {
        // there isn't any data to display yet
        return;
    }
    m_manager.selectScreen(displayIndex);

    TFT_eSPI &display = m_manager.getDisplay();

    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(4);  // Set text size

    // Calculate center positions
    int screenWidth = display.width();
    int centre = 120;

    // Draw stock data
    display.fillRect(0, 0, screenWidth, 50, 0x0256);  // rgb565 colors
    display.drawString(stock.getSymbol(), centre, 27, 1);
    display.drawString(stock.getCurrentPrice(2), centre, 51 + display.fontHeight(1), 1);

    if (stock.getPercentChange() < 0.0) {
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.fillTriangle(120, 220, 140, 185, 100, 185, TFT_RED);
    } else {
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.fillTriangle(120, 185, 140, 220, 100, 220, TFT_GREEN);
    }

    display.drawString(stock.getPercentChange(2) + "%", centre, 147, 1);
}
