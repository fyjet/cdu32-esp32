#include "Arduino.h"
#include "Display.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include "const.h"
#include <list>
#include <iostream>
#include <map>

Display::Display(TFT_eSPI *tft)
{
  _tft = tft;
};

void Display::init()
{
  _tft->init();
  // 1=landscape
  // 0=portrait
  _tft->setRotation(1);
  _tft->fillScreen(TFT_BLACK);

  uint16_t calData[5] = { 407, 3489, 276, 3485, 7 };
  _tft->setTouch(calData);

}

void Display::newPage(const String& title)
{
  _tft->fillScreen(TFT_BLACK);
  m_TouchKeyList.clear();
  _tft->setTextColor(TFT_WHITE,TFT_BLACK);
  setTitle(title);

}
void Display::setTitle(const String& title)
{
  _tft->setTextColor(TFT_WHITE,TFT_BLACK);
  _tft->setTextSize(2);
  _tft->setTextDatum(TC_DATUM);
  _tft->drawString(title,160,0);
}

void Display::addTouchKey(int id, int color, int boxed,int size, const String& txt)
{
  // check is already exists then change it
  list<TouchKey*>::const_iterator end = m_TouchKeyList.end();
  for (list<TouchKey*>::const_iterator it = m_TouchKeyList.begin(); it != end; ++it)
  {
    if ((*it)->id()==id)
    {
      (*it)->set(color, boxed, size, txt);
      return;
    }
  }
  // else create it
  m_TouchKeyList.push_front(new TouchKey(_tft, id, color, boxed, size, txt));
}

void Display::writeLED(const String& txt)
{
  if (txt=="1") {
    digitalWrite(17,1);
  } else {
    digitalWrite(17,0);
  }
  return;
}

int Display::TouchKeyPressed(int x, int y)
{
  list<TouchKey*>::const_iterator end = m_TouchKeyList.end();
  for (list<TouchKey*>::const_iterator it = m_TouchKeyList.begin(); it != end; ++it)
  {
    if ((*it)->isIn(x, y))
    {
      //delay(100);
      return (*it)->id();
    }
  }
  return -1;
}

int Display::poll()
{
  uint16_t x = 0, y = 0;
  if (_tft->getTouch(&x,&y))
  {
    int tp=TouchKeyPressed(x,y);
    if (tp!=-1) {return tp;}
  }
  return -1;
}
