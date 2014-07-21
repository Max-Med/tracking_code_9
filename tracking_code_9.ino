#include <TinyGPS.h>
TinyGPS gps;
#include <SoftwareSerial.h>
#define RXPIN 3
#define TXPIN 2
SoftwareSerial mySerial(RXPIN, TXPIN);

long lat, lon;
unsigned long fix_age, time, date, speed, course;
float flat, flon;
#define RADIOPIN 9

#include <string.h>
#include <util/crc16.h>
#include <SD.h> 
char datastring[100];
const int chipSelect = 4;
 

void setup(){
 Serial.begin(9600);
 mySerial.begin(9600);
 mySerial.println("Initializing SD card...");
 pinMode(10, OUTPUT);
 pinMode(RADIOPIN, OUTPUT);
 setPwmFrequency(RADIOPIN, 1);
 if (!SD.begin(chipSelect)) {
     Serial.println("Card failed, or not present");
     // don't do anything more:
     return;
   }
   Serial.println("card initialized.");

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
        mySerial.println("No fix detected");
      else if (fix_age > 5000)
        mySerial.println("Warning: possible stale data!");
      else
        mySerial.println("Data is current.");
        snprintf(datastring,sizeof(datastring),"$$MAX,%f,%f,%d,%f,%f",flat,flon,gps.satellites(),gps.f_altitude(),gps.f_speed_mps());
        mySerial.println(datastring); 
        
        File dataFile = SD.open("datalog.txt", FILE_WRITE);
        // if the file is available, write to it:
        if (dataFile) {
         dataFile.println(datastring);
         dataFile.close();
         }  
         // if the file isn't open, pop up an error:
        else {
          Serial.println("error opening datalog.txt");
        }
      unsigned int CHECKSUM = gps_CRC16_checksum(datastring); // Calculates the checksum for this datastring
      char checksum_str[7];
      snprintf(checksum_str,sizeof(checksum_str), "*%04X\r\n", CHECKSUM);
      strcat(datastring,checksum_str);
      rtty_txstring (datastring);
     }
  }
}
 
 void rtty_txstring (char * string)
 {
 
  /* Simple function to sent a char at a time to
  ** rtty_txbyte function.
  ** NB Each char is one byte (8 Bits)
  */
 
  char c;
 
  c = *string++;
 
  while ( c != '\0')
   {
   rtty_txbyte (c);
   c = *string++;
   }
 }
 void rtty_txbyte (char c)
  {
  /* Simple function to sent each bit of a char to
  ** rtty_txbit function.
  ** NB The bits are sent Least Significant Bit first
  **
  ** All chars should be preceded with a 0 and
  ** proceed with a 1. 0 = Start bit; 1 = Stop bit
  **
  */
 
  int i;
 
  rtty_txbit (0); // Start bit
  
  // Send bits for for char LSB first
 
  for (i=0;i<7;i++) // Change this here 7 or 8 for ASCII-7 / ASCII-8
   {
   if (c & 1) rtty_txbit(1);
 
   else rtty_txbit(0);
 
   c = c >> 1;
 
   }
 rtty_txbit (1); // Stop bit
 rtty_txbit (1); // Stop bit
 }
 
 void rtty_txbit (int bit)
 {
  if (bit)
  {
 // high
  analogWrite(RADIOPIN,110);
  }
  else
  {
 // low
  analogWrite(RADIOPIN,100);
 
 }
 
// delayMicroseconds(3370); // 300 baud
  delayMicroseconds(10000); // For 50 Baud uncomment this and the line below.
  delayMicroseconds(10150); // You can't do 20150 it just doesn't work as the
 // largest value that will produce an accurate delay is 16383
 // See : http://arduino.cc/en/Reference/DelayMicroseconds
 
 }
 
 uint16_t gps_CRC16_checksum (char *string)
 {
  size_t i;
  uint16_t crc;
  uint8_t c;
  
 crc = 0xFFFF;
  
 // Calculate checksum ignoring the first two $s
  for (i = 2; i < strlen(string); i++)
  {
  c = string[i];
  crc = _crc_xmodem_update (crc, c);
  }
 
 return crc;
 }
 
 void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
  switch(divisor) {
  case 1:
  mode = 0x01;
  break;
  case 8:
  mode = 0x02;
  break;
  case 64:
  mode = 0x03;
  break;
  case 256:
  mode = 0x04;
  break;
  case 1024:
  mode = 0x05;
  break;
  default:
  return;
  }
  if(pin == 5 || pin == 6) {
  TCCR0B = TCCR0B & 0b11111000 | mode;
  }
  else {
  TCCR1B = TCCR1B & 0b11111000 | mode;
  }
  }
  else if(pin == 3 || pin == 11) {
  switch(divisor) {
  case 1:
  mode = 0x01;
  break;
  case 8:
  mode = 0x02;
  break;
  case 32:
  mode = 0x03;
  break;
  case 64:
  mode = 0x04;
  break;
  case 128:
  mode = 0x05;
  break;
  case 256:
  mode = 0x06;
  break;
  case 1024:
  mode = 0x7;
  break;
  default:
  return;
  }
  TCCR2B = TCCR2B & 0b11111000 | mode;
  }

     }



