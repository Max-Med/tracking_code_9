#include <TinyGPS++.h>
TinyGPS gps;



long lat, lon;
unsigned long fix_age, time, date, speed, course;
float flat, flon;

void setup(){
 Serial.begin(9600);
}
void loop()
{ 
  
  while (Serial.available())
  {
    int c = Serial.read();
    if (gps.encode(c))
    {
      // retrieves +/- lat/long in 100000ths of a degree
      gps.get_position(&lat, &lon, &fix_age);
      
      // time in hhmmsscc, date in ddmmyy
      gps.get_datetime(&date, &time, &fix_age);
      
      // returns speed in 100ths of a knot
      speed = gps.speed();
      
      // course in 100ths of a degree
      course = gps.course();
      
      // returns +/- latitude/longitude in degrees
      gps.f_get_position(&flat, &flon, &fix_age);
      float falt = gps.f_altitude(); // +/- altitude in meters
      float fc = gps.f_course(); // course in degrees
      float fmps = gps.f_speed_mps(); // speed in m/sec
      float flat, flon;
      
      unsigned long fix_age; // returns +- latitude/longitude in degrees
      gps.f_get_position(&flat, &flon, &fix_age);
      if (fix_age == TinyGPS::GPS_INVALID_AGE)
        Serial.println("No fix detected");
      else if (fix_age > 5000)
        Serial.println("Warning: possible stale data!");
      else
        Serial.println("Data is current.");

    }
  }
}

