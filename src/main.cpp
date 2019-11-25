#include <Arduino.h>
#include <MsTimer2.h>

#include <ppp.h>

float preasure_pd_max[] = {0, 0, 0, 0};
float preasure_pd_lst[] = {0, 0, 0, 0};

#include <mat.h>
#include <ekr.h>

#define preasure_view_period 500U
bool preasure_view_on = false;
bool preasure_on_view = false;
unsigned long preasure_view_millis = 0;

const unsigned long preasure_time[] = {30000, 30000, 180000, 30000};
bool preasure_on = false;
unsigned int preasure_step = 0;
unsigned long preasure_millis = 0;

void set_preasureview(bool von)
{
  preasure_view_on = von;
  if (preasure_view_on)
  {
    preasure_view_millis = millis();
    Serial.println("View=On");
  }
  else
    Serial.println("View=Off");
  Serial.flush();
}

void preasureview()
{
  if (preasure_view_on)
  {
    String rs = "";
    if (preasure_on)
    {
      rs += "tm=";
      rs += String(millis() - preasure_view_millis);
      rs += ",";
    }
    if (water_on)
    {
      rs += "tw=";
      rs += String(millis() - water_millis_view);
      rs += ",";
    }
    rs += "dp=";
    rs += String(IOPin::preasureRead());
    rs += ",vs=";
    rs += String(IOPin::vacumSensorRead());
    Serial.println(rs);
    Serial.flush();
  }
}

void set_preasure(bool pon)
{
  preasure_on = pon;
  if (preasure_on)
  {
    int cnn = sizeof(preasure_time) / sizeof(preasure_time[0]);
    for (int ii = 0; ii < cnn; ii++)
    {
      preasure_pd_lst[ii] = 0;
      preasure_pd_max[ii] = 0;
    }
    preasure_millis = millis();
    preasure_step = 0;
    Serial.println("Metr=On");
  }
  else
  {
    if (preasure_view_on) set_preasureview(false);
    IOPin::vacumrelay(false, true);
    IOPin::pumprelay(false);
    IOPin::magnitrelay(false);
    IOPin::set_water(false);
    Serial.println("Metr=Off");
  }
  Serial.flush();
}

void preasurePrintErr(int err)
{
  IOPin::set_ind_err(true);
  Serial.print("Err=");
  Serial.println(err);
  Serial.flush();
}

void preasurePrintAll()
{
  String rs = "mv=1";

  rs += ",dh1=";
  rs +=  String(Mth::density(preasure_pd_max[1]),2);
  rs += ",dl1=";
  rs +=  String(Mth::density(preasure_pd_lst[1]),2);
  rs += ",pr1=";
  rs +=  String(Mth::procent_p(preasure_pd_max[1],preasure_pd_lst[1]),0);

  rs += ",dh2=";
  rs +=  String(Mth::density(preasure_pd_max[3]),2);
  rs += ",dl2=";
  rs +=  String(Mth::density(preasure_pd_lst[3]),2);
  rs += ",pr2=";
  rs +=  String(Mth::procent_m(preasure_pd_max[3],preasure_pd_lst[3]),0);

  Serial.println(rs);
  Serial.flush();
}

void poolpreasure()
{
  bool vsens = IOPin::vacumSensorRead();
  if (!preasure_on) return;

  //--- Check Sensor ---
  if (preasure_step == 0)
  {
    if (millis() - preasure_millis < preasure_time[preasure_step] / 3)
    {
      IOPin::vacumrelay(false, true);
      IOPin::pumprelay(true);
    }
    else
    {
      IOPin::vacumrelay(true, false);
      IOPin::pumprelay(false);
    }
    
    if (millis() - preasure_millis > preasure_time[preasure_step])
    {
      set_preasure(false);
      preasurePrintErr(1);
    }

    if (vsens)
    {
      IOPin::vacumrelay(false, false);
      preasure_pd_lst[preasure_step] = IOPin::preasureRead();
      preasure_pd_max[preasure_step] = preasure_pd_lst[preasure_step];
      preasure_millis = millis();
      IOPin::magnitrelay(true);
      preasure_step++;
    }
  }

  //--- maxPD  and lastPD---
  if (preasure_step > 0)
  {
    if (!vsens)
    {
      set_preasure(false);
      preasurePrintErr(2);
    }

    float pd = IOPin::preasureRead();
    if (pd > preasure_pd_max[preasure_step])
      preasure_pd_max[preasure_step] = pd;

    if (millis() - preasure_millis > preasure_time[preasure_step])
    {
      preasure_pd_lst[preasure_step] = pd;
      preasure_millis = millis();
      preasure_step++;
      preasurePrintAll();
    }
  }

  //--- view on step 1 ---
  if (preasure_step == 1 && preasure_on_view && !preasure_view_on)
    set_preasureview(true);

  unsigned int cnn = sizeof(preasure_time) / sizeof(preasure_time[0]);

  //--- magnit off ---
  if (preasure_step >= cnn - 1)
    IOPin::magnitrelay(false);

  //--- stop ---
  if (preasure_step >= cnn)
  {
    preasurePrintAll();
    set_preasure(false);
    IOPin::set_water(true);
  }
}

void prb_init()
{
  if (water_on) IOPin::set_water(false);
  if (preasure_on) set_preasure(false);
  if (preasure_view_on) set_preasureview(false);
  IOPin::set_ind_err(false);
}

//---- command interpretar ----
String cmd = "";
void poolcmd()
{
  if (!Serial.available()) return;

  char ch = Serial.read();
  if (ch == char(13))
  {
    if (cmd == "v0") set_preasureview(false);
    if (cmd == "v1") set_preasureview(true);

    if (cmd == "ab") 
    {
      prb_init();
      Serial.println("Abort");
      Serial.flush();
    }

    if (cmd == "mt")
    {
      prb_init();
      preasure_on_view = true;
      set_preasure(true);
    }

    if (cmd == "mv") preasurePrintAll();

    if (cmd == "cl")
    {
      prb_init();
      preasure_on_view = true;
      IOPin::set_water(true);
    }

    if (cmd == "rm0") 
    {      
      IOPin::magnitrelay(false);
      Serial.println("rm=Off");
      Serial.flush();
    }
    if (cmd == "rm1") 
    {
      IOPin::magnitrelay(true);
      Serial.println("rm=On");
      Serial.flush();
    }

    if (cmd == "rv0") 
    {
      IOPin::vacumrelay_on(false);
      Serial.println("rv=Off");
      Serial.flush();
    }
    if (cmd == "rv1") 
    {
      IOPin::vacumrelay_on(true);
      Serial.println("rv=On");
      Serial.flush();
    }

    if (cmd == "ra0") 
    {
      IOPin::vacumrelay_off(false);
      Serial.println("ra=Off");
      Serial.flush();
    }

    if (cmd == "ra1")
    {
      IOPin::vacumrelay_off(true);
      Serial.println("ra=On");
      Serial.flush();
    } 

    if (cmd == "rw0") 
    {
      IOPin::waterrelay(false);
      Serial.println("rw=Off");
      Serial.flush();
    }
    if (cmd == "rw1")
    {
       IOPin::waterrelay(true);
      Serial.println("rw=On");
      Serial.flush();
    }

    if (cmd == "rp0") 
    {
      IOPin::pumprelay(false);
      Serial.println("rp=Off");
      Serial.flush();
    }
    if (cmd == "rp1") 
    {
      IOPin::pumprelay(true);
      Serial.println("rp=On");
      Serial.flush();
    }

    cmd = "";
    return;
  }
  if (ch >= char(32))
    cmd += ch;
  if (cmd.length() > 8)
    cmd = "";
}
//-------------------------------

//-------- menu select --------
void pollmenu()
{
  if (Ekr::get_menu_curr() == 0 && !preasure_on) Ekr::set_menu_curr(-1);
  if (Ekr::get_menu_curr() != 0 && preasure_on) Ekr::set_menu_curr(0);

  if (Ekr::get_menu_curr() == 1 && !water_on) Ekr::set_menu_curr(-1);
  if (Ekr::get_menu_curr() != 1 && water_on) Ekr::set_menu_curr(1);

  int8_t act = Ekr::get_menu_act();
  if (act < 0) return;

  switch (act)
  {
  case 0:
    if(!preasure_on)
    {
      prb_init();
      preasure_on_view = true;
      set_preasure(true);
    }
    Ekr::set_menu_curr(0);
    break;

  case 1:
    if(!water_on)
    {
      prb_init();
      preasure_on_view = true;
      IOPin::set_water(true);
    }
    Ekr::set_menu_curr(1);
    break;

  case 2:
    prb_init();
    Ekr::set_menu_curr(-1);
    break;

  default:
    Ekr::set_menu_curr(-1);
    break;
  }

}
//-----------------------------

void setup()
{
  // Open serial communications:
  Serial.begin(115200);
  Serial.println("Cntrl=On");

  IOPin::setupio();
  Ekr::setupekr();

  MsTimer2::set(preasure_view_period, preasureview);
  MsTimer2::start();

  prb_init();
}

void loop()
{
  poolcmd();
  poolpreasure();
  IOPin::poolwater();
  IOPin::poolbtn();
  Ekr::poolekr();
  pollmenu();
  delay(1);
}
