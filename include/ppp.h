#ifndef ppp_h
#define ppp_h

#include <Arduino.h>

//--- preasure sensor define ---
#define preasure_pin A0
#define preasure_pin_lovl 204.0  // 1V
#define preasure_pin_hivl 1023.0 // 5V
#define preasure_pin_lodp 0.0    // 0 kPa
#define preasure_pin_hidp 8.0    // 8 kPa
#define preasure_pin_tm 100U
#define preasure_pin_cn 5U
float preasure_sensor_curr = 0;
float preasure_sensor_sum = 0;
unsigned long preasure_sensor_millis = 0;

//--- vacum sensor define ---
#define vacum_sensor_pin 3
#define vacum_relay_on 4
#define vacum_relay_off 5
#define vacum_sensor_delay 100U
bool vacum_sensor_curr = 0;
bool vacum_sensor_new = 0;
unsigned long vacum_sensor_millis = 0;

//--- vacum error sensor define ---
#define vacum_err_sensor_pin A3
#define vacum_err_sensor_delay 10U
bool vacum_err_sensor_curr = 0;
bool vacum_err_sensor_new = 0;
unsigned long vacum_err_sensor_millis = 0;

//--- vacum balon sensor ---
#define vacumbl_pin A2
#define vacumbl_pin_lovl 204.0  // 1V
#define vacumbl_pin_hivl 1023.0 // 5V
#define vacumbl_pin_lodp 0.0    // 0 kPa
#define vacumbl_pin_hidp 250.0  // 250 kPa
#define vacumbl_pin_tm 100U
#define vacumbl_pin_cn 5U
#define vacumbl_predel 30.0 // 30 kPa Predel vacum
float vacumbl_sensor_curr = 0;
float vacumbl_sensor_sum = 0;
unsigned long vacumbl_sensor_millis = 0;

//--- vacum balon pump ---
#define vacumbl_pump_pin 11  

// --- pump define ---
#define pump_pin 2

//--- magnit define ---
#define magnit_relay_on 6

//--- water define ---
#define water_relay_on 7
const unsigned long water_time[] = {10000, 1000, 20000, 1000, 10000, 1000};
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
  bool vacumErrSensorRead();
  void vacumrelay_on(bool von);
  void vacumrelay_off(bool vof);
  void magnitrelay(bool mon);
  void set_water(bool won);
  void waterrelay(bool won);
  void poolwater();
  uint8_t get_button();
  void poolbtn(); 
  void set_ind_err(bool err);
  float vacumblRead();
  void vacumblPump(bool pon);
  void poolsensors();
} 

void IOPin::setupio()
{
  //--- preasure initialize ---
  pinMode(preasure_pin, INPUT);

  //--- vacum initialize ---
  pinMode(vacum_sensor_pin, INPUT);
  pinMode(vacum_relay_on, OUTPUT);
  pinMode(vacum_relay_off, OUTPUT);
  pinMode(vacum_err_sensor_pin, INPUT);

  //--- vacum balon initialize ---
  pinMode(vacumbl_pin, INPUT);
  pinMode(vacumbl_pump_pin, OUTPUT);

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

  vacumrelay(false,true);
  waterrelay(false);
  magnitrelay(false);
  pumprelay(false);
  vacumblPump(false);
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

//--- vacum balon sensor ---
float IOPin::vacumblRead()
{
  if (millis() - vacumbl_sensor_millis > vacumbl_pin_tm)
  {
    float sn = float(analogRead(vacumbl_pin)) - vacumbl_pin_lovl;
    sn = sn * (vacumbl_pin_hidp - vacumbl_pin_lodp) / (vacumbl_pin_hivl - vacumbl_pin_lovl);
    
    vacumbl_sensor_sum = vacumbl_sensor_sum - vacumbl_sensor_curr + sn;
    vacumbl_sensor_curr = vacumbl_sensor_sum / vacumbl_pin_cn;
    
    vacumbl_sensor_millis = millis();
  }

  return vacumbl_sensor_curr;
}

void IOPin::poolsensors()
{
  vacumErrSensorRead();
  vacumSensorRead();  
  vacumblRead();
  preasureRead();
}

//--- Vacum balon pump ---
void IOPin::vacumblPump(bool pon)
{
  digitalWrite(vacumbl_pump_pin, !pon);
}

//--- Vacum ---
void IOPin::vacumrelay(bool von, bool voff)
{
  vacumrelay_on(von);
  vacumrelay_off(voff);
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

bool IOPin::vacumErrSensorRead()
{
  bool vsens = !digitalRead(vacum_err_sensor_pin);

  if (vsens != vacum_err_sensor_new)
  {
    vacum_err_sensor_new = vsens;
    vacum_err_sensor_millis = millis();
  }

  if (millis() - vacum_err_sensor_millis > vacum_err_sensor_delay)
  {
    if (vacum_err_sensor_new == vsens)
      vacum_err_sensor_curr = vsens;
    vacum_err_sensor_millis = millis();
  }

  return vacum_err_sensor_curr;
}

//--- vacum relay on ---
void IOPin::vacumrelay_on(bool von)
{
  digitalWrite(vacum_relay_on, !von);
}

//--- vacum relay off ---
void IOPin::vacumrelay_off(bool vof)
{
  digitalWrite(vacum_relay_off, vof);
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
    vacumrelay(false,false);
    waterrelay(true);
    pumprelay(false);
    vacumblPump(false);
  }
  else
  {
    Serial.println("Water=Off");
    vacumrelay(false,true);
    pumprelay(false);
    waterrelay(false);
    vacumblPump(false);
  }
  Serial.flush();
}

void IOPin::poolwater()
{
  if (!water_on) return;

  if (vacumErrSensorRead())
  {
    set_water(false);
  }
  
  switch (water_step)
  {
    case 0 :
        vacumrelay(false,true);
        pumprelay(true);
        waterrelay(false);
        break;

    case 1 :
        vacumrelay(false,false);
        pumprelay(false);
        break;

    case 2 :
        waterrelay(true);
        break;

    case 3 :
        waterrelay(false);
        break;

    case 4 :
        vacumrelay(false,true);
        pumprelay(true);
        break;

    case 5 :
        vacumrelay(false,true);
        pumprelay(false);
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
