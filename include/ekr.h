#ifndef ekr_h
#define ekr_h

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <ppp.h>
#include <mat.h>

#define ekr_sda A4
#define ekr_scl A5

#define ekr_delay 333U
unsigned long ekr_millis = 0;

String ekr_menu[] = {"Meter","Clean","Reset","1-->","2-->"};
String ekr_menu_lv1[] = {"Dn1-H","Dn1-L","Dn2-H","Dn2-L","Pr1","Pr2","Pr3","<---"};
String ekr_menu_lv2[] = {"Differential","Density","Pressure","<---"};

#define ekr_menu_cn 4 
uint8_t ekr_menu_item = 0;
uint8_t ekr_menu_level = 0;
uint8_t ekr_menu_curr = -1;
uint8_t ekr_menu_act = -1;
uint8_t ekr_menu_btn = 0;

LiquidCrystal_I2C lcd(0x27,16,2);

extern float preasure_pd_max[];
extern float preasure_pd_lst[];
extern unsigned int preasure_step;
extern unsigned long preasure_millis;
extern bool preasure_on;

// namespace IOPin
namespace Ekr
{
    void setupekr();
    void poolekr();
    void printekr();
    void set_menu_curr(int8_t curr);
    void set_menu_item(int8_t item);
    int8_t get_menu_act();
    int8_t get_menu_curr();
}

void Ekr::setupekr()
{
    // initialize the lcd   
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    ekr_millis = millis();
}

String preasure_rs_p()
{
  String rs = String(Mth::density(preasure_pd_max[0]),2);
  rs +=':';
  rs +=String(Mth::density(preasure_pd_lst[0]),2);
  rs +=':';
  rs +=String(Mth::procent_p(preasure_pd_max[0],preasure_pd_lst[0]),2);
  return rs;
}

String preasure_rs_m()
{
  String rs = String(Mth::density(preasure_pd_max[2]),2);
  rs +=':';
  rs +=String(Mth::density(preasure_pd_lst[2]),2);
  rs +=':';
  rs +=String(Mth::procent_m(preasure_pd_max[2],preasure_pd_lst[2]),2);
  return rs;
}

void Ekr::printekr()
{
    String ss;  

    ss = "Dp:";
    ss += String(IOPin::preasureRead(),2);
    ss += " Vs:";
    if (IOPin::vacumSensorRead()) ss+="ON "; else ss+="OFF";

    while (ss.length()<16) ss+=" ";
    lcd.setCursor(0,0);
    lcd.print(ss);

    if (ekr_menu_item < 3)
    {
        ss = ekr_menu[ekr_menu_item];
        if (ekr_menu_item == ekr_menu_curr) 
        {
            ss += ":ON";
            if (ekr_menu_item == 0 && preasure_on)
            {
                ss += " ";
                ss += String(preasure_step);
                ss += ":";
                ss += String((millis() - preasure_millis) / 1000);
            }
        }

    }

    if (ekr_menu_item == 3)
    {
        ss = "pl=";
        ss += preasure_rs_p();
    }

    if (ekr_menu_item == 4)
    {
        ss = "mg=";
        ss += preasure_rs_m();
    }

    while (ss.length()<16) ss+=" ";
    lcd.setCursor(0,1);
    lcd.print(ss);
}

void Ekr::set_menu_item(int8_t item)
{
    ekr_menu_item = item;
}

void Ekr::set_menu_curr(int8_t curr)
{
    ekr_menu_curr = curr;
    ekr_menu_act = -1;
}

int8_t Ekr::get_menu_act()
{
    return ekr_menu_act;
}

int8_t Ekr::get_menu_curr()
{
    return ekr_menu_curr;
}

void Ekr::poolekr()
{
  if (millis() - ekr_millis > ekr_delay)
  {
    ekr_millis = millis();
    
    uint8_t btn = IOPin::get_button();
    if(ekr_menu_btn != btn)
    {
       switch (btn)
       {
       case 1:
           ekr_menu_item ++;
           break;
       case 2:
           ekr_menu_item --;
           break;
       case 4:
           ekr_menu_act = ekr_menu_item;             
           break;
       }
       if (ekr_menu_item < 0) ekr_menu_item = ekr_menu_cn;
       if (ekr_menu_item > ekr_menu_cn) ekr_menu_item = 0;   

       ekr_menu_btn = btn;
    }

    printekr();
  }
}

#endif