
#include <Arduino.h>
#include "RTClib.h"
#include <JC_Button.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;
const byte btn_up(12), btn_down(11), btn_select(10);

Button UP(btn_up);
Button Down(btn_down);
Button Select(btn_select);

int state;                            //saves the current state of the menu
const unsigned long LONG_PRESS(1000); //duration time for long press to trigger
byte arrow[] = {
    B00100,
    B01110,
    B10101,
    B00100,
    B00100,
    B00100,
    B00000,
    B00000};

uint16_t hour_now;
uint16_t minute_now;
uint16_t second_now;
int k;
bool time_edit;


void setup()
{
  UP.begin();
  Down.begin();
  Select.begin();

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running, let's set the time!");
  }
  lcd.createChar(0, arrow);
}

void loop()
{
  Down.read();
  UP.read();
  Select.read();
  DateTime now = rtc.now();

  if (Select.pressedFor(LONG_PRESS)) //----------listens for time edit request---------------
  {
    state = 1; ///-----------time edit request received
    lcd.setCursor(0, 0);
    lcd.print("Edit mode.....");
    lcd.print("               ");
    delay(1000);
    lcd.clear(); //clears the screen
  }
  else
  {
    state = 0; //default state normal procedure
  }

  if (state == 1) //time configuring state
  {
    hour_now = now.hour(); //capture the time and enter a while loop
    minute_now = now.minute();
    second_now = now.second();
    lcd.setCursor(4, 0);
    lcd.print(hour_now); //print the recorded time
    lcd.print('/');
    lcd.print(minute_now);
    lcd.print('/');
    lcd.print(second_now);
    time_edit = true;
    delay(500);               //to give time to microcontroller to not register one key press as two.
    while (time_edit == true) //time edit while loop
    {
      Down.read();
      UP.read();
      Select.read();

      //----------------------------------menu navigation----------------------------

      if (k == 2 && UP.wasPressed()) //limit the k to value of 2
      {
        k = 0;
        lcd.clear();
      }
      else if (UP.wasPressed()) //if up is pressed change the focus to minutes or hours depending on its value
      {                         //increase by one after every press
        if (k != 2)
        {
          k += 1;
          lcd.clear();
        }
      }
      if (k == 0 && Down.wasPressed()) //limit the value so that it wont get below 0
      {
        k = 2;
        lcd.clear();
      }
      else if (Down.wasPressed()) //if down is pressed decrease state value by 1
      {
        if (k != 0) // extra care taken
        {
          k -= 1;
          lcd.clear();
        }
      }
      //------------------------------draws the arrow under lcd --------------------------
      if (k == 0)
      {

        lcd.setCursor(4, 0);
        lcd.print(hour_now); //print the recorded time
        lcd.print('/');
        lcd.print(minute_now);
        lcd.print('/');
        lcd.print(second_now);
        lcd.setCursor(9, 1); //drawing arrow under second
        lcd.write(0);
      }
      else if (k == 1)
      {
        lcd.setCursor(4, 0);
        lcd.print(hour_now); //print the recorded time
        lcd.print('/');
        lcd.print(minute_now);
        lcd.print('/');
        lcd.print(second_now);
        lcd.setCursor(6, 1); //drawing arrow under second
        lcd.write(0);
      }
      else if (k == 2)
      {
        lcd.setCursor(4, 0);
        lcd.print(hour_now); //print the recorded time
        lcd.print('/');
        lcd.print(minute_now);
        lcd.print('/');
        lcd.print(second_now);
        lcd.setCursor(4, 1); //drawing arrow under second
        lcd.write(0);
      }

      //-----------------------seconds edit part--------------------------------

      if (k == 0 && Select.wasPressed()) //if state is zero and
      {
        bool second_flag = 1; //enter into a while loop only editing seconds
        lcd.setCursor(0, 0);
        lcd.print("Edit Seconds...");
        delay(1000);
        while (second_flag == 1)
        {
          Down.read();
          UP.read();
          Select.read();

          lcd.setCursor(4, 0);
          lcd.print(hour_now); //print the recorded time
          lcd.print('/');
          lcd.print(minute_now);
          lcd.print('/');
          lcd.print(second_now);
          if (Select.wasPressed()) //if was select is pressed again that means users set his time and program exits
          {
            rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour_now, minute_now, second_now));
            second_flag = 0; // exit the second connfiguring loop annd rememmbers the values set
            lcd.setCursor(0, 0);
            lcd.print("Saving...          ");
            delay(500);
            lcd.clear();
          }
          if (UP.wasPressed()) //increases the seconds by +5 if up is pressed
          {
            if (second_now != 59)
            {
              second_now += 1;
            }
            if (second_now == 59 && UP.wasPressed()) // limit the second now value
            {
              second_now = 0;
              lcd.clear();
            }
          }
          if (Down.wasPressed()) //same thing as above but only decreases
          {
            if (second_now != 0)
            {
              second_now -= 1;
            }
            if (second_now == 0 && Down.wasPressed())
            {
              lcd.clear();
              second_now = 59;
            }
          }
        }
      }
      //----------------------------------HOUR EDIT PART----------------------------
      else if (k == 2 && Select.wasPressed())
      {
        bool hour_flag = 1;
        delay(10);
        while (hour_flag == 1)
        {

          Down.read();
          UP.read();
          Select.read();

          lcd.setCursor(4, 0);
          lcd.print(hour_now); //print the recorded time
          lcd.print('/');
          lcd.print(minute_now);
          lcd.print('/');
          lcd.print(second_now);
          if (Select.wasPressed())
          {
            rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour_now, minute_now, second_now));
            hour_flag = 0;
            lcd.setCursor(0, 0);
            lcd.print("Saving...          ");
            delay(500);
            lcd.clear();
          }
          if (UP.wasPressed())
          {
            if (hour_now != 24)
            {
              hour_now += 1;
            }
            if (hour_now == 24 && UP.wasPressed()) // limit the second now value
            {
              lcd.clear();
              hour_now = 0;
            }
          }
          if (Down.wasPressed()) //same thing as above but only decreases
          {
            if (hour_now != 0)
            {
              hour_now -= 1;
            }
            if (hour_now == 0 && Down.wasPressed())
            {
              lcd.clear();
              hour_now = 24;
            }
          }
        }
      }
      //---------------------------------------minutes edit part------------------------
      else if (k == 1 && Select.wasPressed()) //if state is zero and
      {
        lcd.setCursor(4, 0);
        lcd.print(hour_now); //print the recorded time
        lcd.print('/');
        lcd.print(minute_now);
        lcd.print('/');
        lcd.print(second_now);
        bool minute_flag = 1; //enter into a while loop only editing seconds
        delay(10);
        while (minute_flag == 1)
        {
          lcd.setCursor(4, 0);
          lcd.print(hour_now); //print the recorded time
          lcd.print('/');
          lcd.print(minute_now);
          lcd.print('/');
          lcd.print(second_now);
          Down.read();
          UP.read();
          Select.read();

          if (Select.wasPressed()) //if was select is pressed again that means users set his time and program exits
          {
            rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour_now, minute_now, second_now));
            minute_flag = 0; // exit the second connfiguring loop annd rememmbers the values set
            lcd.setCursor(0, 0);
            lcd.print("Saving...          ");
            delay(500);
            lcd.clear();
          }
          if (UP.wasPressed()) //increases the seconds by +5 if up is pressed
          {
            if (minute_now != 59)
            {
              minute_now += 1;
            }
            if (minute_now == 59 && UP.wasPressed()) // limit the second now value
            {
              minute_now = 0;
            }
          }
          if (Down.wasPressed()) //same thing as above but only decreases
          {
            if (minute_now != 0)
            {
              minute_now -= 1;
            }
            if (minute_now == 0 && Down.wasPressed())
            {
              minute_now = 59;
            }
          }
        }
      }
      if (Select.pressedFor(3000))
      {
        time_edit = false;
        state = 0;
        lcd.setCursor(0, 0);
        lcd.print("Exiting...       ");
        lcd.setCursor(0, 1);
        lcd.print("                 ");
        delay(1000);
        lcd.clear();
      }
    }
  }
  else
  {
    lcd.setCursor(4, 0);
    lcd.print(now.hour()); //print the recorded time
    lcd.print('/');
    lcd.print(now.minute());
    lcd.print('/');
    lcd.print(now.second());
  }
}