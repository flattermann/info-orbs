#include "core/wifiWidget.h"
#include "widgetSet.h"
#include "screenManager.h"
#include "widgets/clockWidget.h"
#include "widgets/weatherWidget.h"
#include "widgets/webDataWidget.h"
#include <Arduino.h>
#include <Button.h>
#include <globalTime.h>
#include <config.h>
#ifdef STOCK_TICKER_LIST
  #ifdef USE_ONVISTA
    #include <widgets/onvistaWidget.h>
  #else
    #include <widgets/stockWidget.h>
  #endif
#endif
#ifdef PARQET_PORTFOLIO_ID
  #include <widgets/parqetWidget.h>
#endif

TFT_eSPI tft = TFT_eSPI();

// Button states
bool lastButtonOKState = HIGH;
bool lastButtonLeftState = HIGH;
bool lastButtonRightState = HIGH;

unsigned long m_widgetDelay = 20*60*1000;  // Automatically cycle widgets every X ms, set to 0 to disable
unsigned long m_widgetDelayPrev = 0;

Button buttonOK(BUTTON_OK);
Button buttonLeft(BUTTON_LEFT);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{ nullptr };

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

ScreenManager* sm;
WidgetSet* widgetSet;

void setup() {

  buttonLeft.begin();
  buttonOK.begin();
  buttonRight.begin();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  sm = new ScreenManager(tft);
  sm->selectAllScreens();
  sm->getDisplay().fillScreen(TFT_WHITE);
  sm->reset();
  widgetSet = new WidgetSet(sm);

  TJpgDec.setSwapBytes(true); // jpeg rendering setup
  TJpgDec.setCallback(tft_output);

#ifdef GC9A01_DRIVER
  Serial.println("GC9A01 Driver");
#endif
#ifdef ILI9341_DRIVER
  Serial.println("ILI9341 Driver");
#endif
#if HARDWARE == WOKWI
  Serial.println("Wokwi Build");
#endif

  pinMode(BUSY_PIN, OUTPUT);
  Serial.println("Connecting to: " + String(WIFI_SSID));

  wifiWidget = new WifiWidget(*sm);
  wifiWidget->setup();

  globalTime = GlobalTime::getInstance();

  widgetSet->add(new ClockWidget(*sm));
#ifdef PARQET_PORTFOLIO_ID
  widgetSet->add(new ParqetWidget(*sm));
#endif
#ifdef STOCK_TICKER_LIST
  #ifdef USE_ONVISTA
    widgetSet->add(new OnvistaWidget(*sm));
  #else
    widgetSet->add(new StockWidget(*sm));
  #endif
#endif
  widgetSet->add(new WeatherWidget(*sm));
#ifdef WEB_DATA_WIDGET_URL
  widgetSet->add(new WebDataWidget(*sm, WEB_DATA_WIDGET_URL));
#endif
#ifdef WEB_DATA_STOCK_WIDGET_URL
  widgetSet->add(new WebDataWidget(*sm, WEB_DATA_STOCK_WIDGET_URL));
#endif

  m_widgetDelayPrev = millis();
}

void checkCycleWidgets() {
  if (m_widgetDelay > 0 && (m_widgetDelayPrev == 0 || (millis() - m_widgetDelayPrev) >= m_widgetDelay)) {
        widgetSet->next();
        m_widgetDelayPrev = millis();
    }
}

void loop() {
  if (wifiWidget->isConnected() == false) {
    wifiWidget->update();
    wifiWidget->draw();
    widgetSet->setClearScreensOnDrawCurrent(); //clear screen after wifiWidget
    delay(100);
  } else {
    if (!widgetSet->initialUpdateDone()) {
      widgetSet->initializeAllWidgetsData();
    }
    globalTime->updateTime();

    if (buttonLeft.pressed()) {
      Serial.println("Left button pressed");
      m_widgetDelayPrev = millis();
      widgetSet->prev();
    }
    if (buttonOK.pressed()) {
      Serial.println("OK button pressed");
      widgetSet->changeMode();
    }
    if (buttonRight.pressed()) {
      Serial.println("Right button pressed");
      m_widgetDelayPrev = millis();
      widgetSet->next();
    }

    widgetSet->updateCurrent();
    widgetSet->drawCurrent();

    checkCycleWidgets();
  }
}
