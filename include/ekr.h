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

uint8_t ekr_menu_item = 0;
uint8_t ekr_menu_level = 0;
uint8_t ekr_menu_curr = -1;
uint8_t ekr_menu_act = -1;
uint8_t ekr_menu_btn = 0;
uint8_t ekr_value_curr = 0;

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
    void print_menu_lv0();
    void print_menu_lv1();
    void print_menu_lv2();

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

void Ekr::printekr()
{
    String ss;  

    switch (ekr_value_curr)
    {
        case 0:
            ss = "Dp:";
            ss += String(IOPin::preasureRead(),2);
        break;

        case 1:
            ss = "Dn:";
            ss += String(Mth::density(IOPin::preasureRead()),2);
        break;

        case 2:
            ss = "P:";
            ss += String(IOPin::vacumblRead(),2);
        break;
    }

    ss += " Vs:";
    if (IOPin::vacumErrSensorRead()) ss+="Err";
    else if (IOPin::vacumSensorRead()) ss+="ON "; else ss+="OFF";

    while (ss.length()<16) ss+=" ";
    lcd.setCursor(0,0);
    lcd.print(ss);

    switch (ekr_menu_level)
    {
        case 0:
            print_menu_lv0();
        break;
    
        case 1:
            print_menu_lv1();
        break;

        case 2:
            print_menu_lv2();
        break;
    }
}

void Ekr::print_menu_lv0()
{
    String ss;
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

    while (ss.length()<16) ss+=" ";
    lcd.setCursor(0,1);
    lcd.print(ss);
}

//String ekr_menu_lv1[] = {"Dn1-H","Dn1-L","Dn2-H","Dn2-L","Pr1","Pr2","Pr3","<---"};
void Ekr::print_menu_lv1()
{
    String ss;
    ss = ekr_menu_lv1[ekr_menu_item];
    ss += ":";
    switch (ekr_menu_item)
    {
        case 0: ss += String(Mth::density(preasure_pd_max[0]),2);
        break;    
        case 1: ss += String(Mth::density(preasure_pd_lst[0]),2);
        break;    
        case 2: ss += String(Mth::density(preasure_pd_max[2]),2);
        break;    
        case 3: ss += String(Mth::density(preasure_pd_lst[2]),2);
        break;    
        case 4:
        {
            float pr = Mth::procent_p(preasure_pd_max[0],preasure_pd_lst[0]);
            ss += String(pr,2);
        } 
        break;    
        case 5:
        {
            float pr = Mth::procent_m(preasure_pd_max[2],preasure_pd_lst[2]);
            ss += String(pr,2);
        } 
        break;    
        case 6:
        {
            float pr1 = Mth::procent_p(preasure_pd_max[0],preasure_pd_lst[0]);
            float pr2 = Mth::procent_m(preasure_pd_max[2],preasure_pd_lst[2]);
            float pr = Mth::procent_pm(pr1,pr2);
            ss += String(pr,2);
        } 
        break;    
    }

    while (ss.length()<16) ss+=" ";
    lcd.setCursor(0,1);
    lcd.print(ss);
}

void Ekr::print_menu_lv2()
{
    String ss;
    ss = ekr_menu_lv2[ekr_menu_item];
    if(ekr_menu_item == ekr_value_curr) ss +=":ON";

    while (ss.length()<16) ss+=" ";
    lcd.setCursor(0,1);
    lcd.print(ss);
}

void Ekr::set_menu_item(int8_t item)
{
    ekr_menu_level = 0;
    ekr_menu_item = item;
}

void Ekr::set_menu_curr(int8_t curr)
{
    ekr_menu_level = 0;
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

        uint8_t ekr_menu_cn = 0;
        switch (ekr_menu_level)
        {
            case 0:
                ekr_menu_cn = 4;
            break;
            case 1:
                ekr_menu_cn = 7;
            break;
            case 2:
                ekr_menu_cn = 3;
            break;
        }

        switch (btn)
        {
            case 1:
                if (ekr_menu_item < ekr_menu_cn)
                    ekr_menu_item ++;
                else    
                    ekr_menu_item = 0;   
            break;
            case 2:
                if (ekr_menu_item > 0)
                    ekr_menu_item --;
                else
                    ekr_menu_item = ekr_menu_cn;
            break;
            case 4:
                switch (ekr_menu_level)
                {
                    case 0:
                        if (ekr_menu_item < 3)
                          ekr_menu_act = ekr_menu_item;
                        if (ekr_menu_item == 3) 
                        {
                            ekr_menu_level = 1;
                            ekr_menu_item = 0;
                        }
                        if (ekr_menu_item == 4) 
                        {
                            ekr_menu_level = 2;
                            ekr_menu_item = 0;
                        }
                    break;

                    case 1:
                        if (ekr_menu_item == 7) 
                        {
                            ekr_menu_level = 0;
                            ekr_menu_item = 0;
                        }
                    break;

                    case 2:
                        if (ekr_menu_item < 3) 
                          ekr_value_curr = ekr_menu_item;
                        if (ekr_menu_item == 3) 
                        {
                            ekr_menu_level = 0;
                            ekr_menu_item = 0;
                        }
                    break;
                }                            
            break;
        }
        ekr_menu_btn = btn;
    }

    printekr();
  }
}

#endif