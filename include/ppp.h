#ifndef ppp_h
#define ppp_h

#include <Arduino.h>

//--- preasure define ---
#define preasure_pin A0
#define preasure_pin_lovl 204.0  // 1V
#define preasure_pin_hivl 1023.0 // 5V
#define preasure_pin_lodp 0.0    // 0 kPa
#define preasure_pin_hidp 8.0    // 8 kPa
#define preasure_pin_tm 50U
#define preasure_pin_cn 5U
float preasure_sensor_curr = 0;
float preasure_sensor_sum = 0;
unsigned long preasure_sensor_millis = 0;

//--- vacum define ---
#define vacum_sensor_pin 3
#define vacum_relay_on 4
#define vacum_relay_off 5
#define vacum_sensor_delay 200U
bool vacum_sensor_curr = 0;
bool vacum_sensor_new = 0;
unsigned long vacum_sensor_millis = 0;

// --- pump define ---
#define pump_pin 2

//--- magnit define ---
#define magnit_relay_on 6

//--- water define ---
#define water_relay_on 7
const unsigned long water_time[] = {10000, 20000, 10000};
bool water_on = false;
unsigned long water_millis = 0;
unsigned long water_millis_view = 0;
unsigned int water_step = 0;

//--- button define ---
#define btn_0 8
#define btn_1 9
#define btn_2 10
#define btn_delay 100U
unsigned long btn_millis = 0;
uint8_t btn_new = 0;
uint8_t btn_curr = 0;

//--- inderr define ---
#define inderr_pin A1


// namespace IOPin
namespace IOPin
{
  void setupio();
  void pumprelay(bool pon);
  float preasureRead();
  void vacumrelay(bool von, bool voff);
  bool vacumSensorRead();
  void vacumrelay_on(bool von);
  void vacumrelay_off(bool vof);
  void magnitrelay(bool mon);
  void set_water(bool won);
  void waterrelay(bool won);
  void poolwater();
  uint8_t get_button();
  void poolbtn(); 
  void set_ind_err(bool err);
} 

void IOPin::setupio()
{
  //--- preasure initialize ---
  pinMode(preasure_pin, INPUT);

  //--- vacum initialize ---
  pinMode(vacum_sensor_pin, INPUT);
  pinMode(vacum_relay_on, OUTPUT);
  pinMode(vacum_relay_off, OUTPUT);

  //--- pump initialize ---
  pinMode(pump_pin, OUTPUT);

  //--- magnit initialize ---
  pinMode(magnit_relay_on, OUTPUT);

  //--- water initialize ---
  pinMode(water_relay_on, OUTPUT);

  //--- button initialize ---
  pinMode(btn_0, INPUT);
  pinMode(btn_1, INPUT);
  pinMode(btn_2, INPUT);

  //--- inderr initialize ---
  pinMode(inderr_pin,INPUT);

  vacumrelay(false,false);
  waterrelay(false);
  magnitrelay(false);
  pumprelay(false);
  set_ind_err(false);
}

//--- Preasure ---
float IOPin::preasureRead()
{
  if (millis() - preasure_sensor_millis > preasure_pin_tm)
  {
    float sn = float(analogRead(preasure_pin)) - preasure_pin_lovl;
    sn = sn * (preasure_pin_hidp - preasure_pin_lodp) / (preasure_pin_hivl - preasure_pin_lovl);
    
    preasure_sensor_sum = preasure_sensor_sum - preasure_sensor_curr + sn;
    preasure_sensor_curr = preasure_sensor_sum / preasure_pin_cn;
    
    preasure_sensor_millis = millis();
  }

  return preasure_sensor_curr;
}

//--- Vacum ---
void IOPin::vacumrelay(bool von, bool voff)
{
  digitalWrite(vacum_relay_on, !von);
  digitalWrite(vacum_relay_off, !voff);
}

bool IOPin::vacumSensorRead()
{
  bool vsens = !digitalRead(vacum_sensor_pin);

  if (vsens != vacum_sensor_new)
  {
    vacum_sensor_new = vsens;
    vacum_sensor_millis = millis();
  }

  if (millis() - vacum_sensor_millis > vacum_sensor_delay)
  {
    if (vacum_sensor_new == vsens)
      vacum_sensor_curr = vsens;
    vacum_sensor_millis = millis();
  }

  return vacum_sensor_curr;
}

void IOPin::vacumrelay_on(bool von)
{
  digitalWrite(vacum_relay_on, !von);
}

void IOPin::vacumrelay_off(bool vof)
{
  digitalWrite(vacum_relay_off, !vof);
}

//--- Pump ---
void IOPin::pumprelay(bool pon)
{
  digitalWrite(pump_pin, !pon);
}


//--- Magnit ---
void IOPin::magnitrelay(bool mon)
{
  digitalWrite(magnit_relay_on, !mon);
}

//--- Water ---
void IOPin::waterrelay(bool won)
{
  digitalWrite(water_relay_on, !won);
}

void IOPin::set_water(bool won)
{
  water_on = won;
  if (water_on)
  {
    Serial.println("Water=On");
    water_millis = millis();
    water_millis_view = millis();
    water_step = 0;
  }
  else
  {
    Serial.println("Water=Off");
  }
  Serial.flush();

  vacumrelay(false,false);
  waterrelay(water_on);
}

void IOPin::poolwater()
{
  if (!water_on) return;
  
  switch (water_step)
  {
    case 0 :
        vacumrelay(false,true);
        waterrelay(false);
        break;
    case 1 :
        vacumrelay(false,false);
        waterrelay(true);
        break;
    case 2 :
        vacumrelay(false,true);
        waterrelay(false);
        break;
  }

  if (millis() - water_millis > water_time[water_step]) 
  {
    water_millis = millis();
    water_step++;
  }
    
  unsigned int cnn = sizeof(water_time) / sizeof(water_time[0]);
  if (water_step >= cnn)
  {
    set_water(false);
  }     
}

uint8_t IOPin::get_button()
{
   return btn_curr; 
}

void IOPin::poolbtn()
{
  uint8_t btn = !digitalRead(btn_2);
  btn = (btn << 1) | !digitalRead(btn_1);
  btn = (btn << 1) | !digitalRead(btn_0);

  if(btn != btn_new)
  {
    btn_new = btn;
    btn_millis = millis();
  } 

  if(millis() - btn_millis > btn_delay)
  {
    btn_curr = btn;
    btn_millis = millis();
  }
} 

void IOPin::set_ind_err(bool err)
{
  digitalWrite(inderr_pin,err);
}

#endif
