/****
1. Keuw! - App starten
2. App sends continuously 'X__,Y__' (min -400, max 400), e.g. 'X2,Y3' (range for M1 and M2 = [-400,400]
  -> controls movement
  -> direct proportianal to motor output
3. App sends on change 'R__,A__' (min 0, max 100), e.g. 'R100,A360' 
  -> controls head orientation
  -> angle direct proportianal to head orientation

****/

#include "DualVNH5019MotorShield.h"
#include <ServoTimer2.h>

ServoTimer2 Head;
DualVNH5019MotorShield md;

int pinHead = 11;
int headAngle = 90; // 90=mittig 0=links, 180=rechts
String str;
char s;
unsigned long t;
int ledStatus = 13;
//int baudSerial = 19200; // 960d sS0
int baudSerial = 9600;
int interval=100;   // millisekunden 

int theta;
boolean doSetSpeed,doSetHead;
int maxSpeedAllowed=400;
int maxSpeedSet=200;
float maxSpeedFraction=float(maxSpeedSet)/float(maxSpeedAllowed);
int pad_x,pad_y,speed_l,speed_r=0;
int pad_r,pad_a=0;
int pos=0;
int del=5;

void blink()
{
  digitalWrite(ledStatus, HIGH);
  delay(5);
  digitalWrite(ledStatus, LOW);
}

void setup()
{
  Serial.begin(baudSerial);
  pinMode(ledStatus,OUTPUT);

  Head.attach(pinHead);
  md.init();
}

void loop()
{
  t=millis();  // initialize timer
  digitalWrite(ledStatus, LOW);
  doSetSpeed=false;
  doSetHead=false;
    
  if (Serial.available())
  {
    s=Serial.read();
    str=String(s);

    //**** Control Pad on Left -  Sends 'X__,Y___*' continuosly every 150ms
		if(s=='X')
		{
			pad_x=Serial.parseInt();
      str+=String(pad_x);
			while (s!='*')
			{
				if (Serial.available())
				{
					s=Serial.read(); //Get next character from bluetooth
          str+=String(s);
					if(s=='Y')
					{
					  pad_y=Serial.parseInt();
            str+=String(pad_y);
            doSetSpeed=true;blink;
					}
/*
 * 
 *          +-------+-------+
 *          |       |       |
 *          |   VI  |    I  |
 *          |       |       |
 *          +-------+-------+ 
 *          |       |       |
 *          |  III  |   II  |
 *          |       |       |
 *          +-------+-------+ 
 * oben - rechts - I
 *  -> 
 */


          if(pad_x>=0 && pad_y<=0) // oben - rechts - I
          {
            speed_l=int(maxSpeedFraction*(float(sqrt(pow(pad_y,2)+pow(pad_x,2)))));
            speed_r=int(maxSpeedFraction*(abs(pad_y)-abs(pad_x)));
          }
          if(pad_x>0 && pad_y>0) // unten - rechts - II
          {
            speed_l=0;
            speed_r=-int(maxSpeedFraction*(abs(pad_y)-abs(pad_x)));
          }
          if(pad_x<0 && pad_y>0) // unten - links - III
          {
            speed_l=-int(maxSpeedFraction*(abs(pad_y)-abs(pad_x)));
            speed_r=0;
          }
          if(pad_x<0 && pad_y<0) // oben - links - IV
          {
            speed_l=int(maxSpeedFraction*(abs(pad_y)-abs(pad_x)));
            speed_r=int(maxSpeedFraction*(float(sqrt(pow(pad_y,2)+pow(pad_x,2)))));
          }
				}
			}
		}

    //**** Control Pad on Right -  Sends 'R__,A___*' on change every 150ms
		if(s=='R')
		{
			pad_r=Serial.parseInt();
      str+=String(pad_r);
			while (s!='*')
			{
				if (Serial.available())
				{
					s=Serial.read(); //Get next character from bluetooth
          str+=String(s);
					if(s=='A')
					{
					  pad_a=Serial.parseInt();
            str+=String(pad_a);
            doSetHead=true;blink;
					}
          if(pad_a==180){
            headAngle=90; // Ruheposition von pad_a
          }
          else
          {
            headAngle=180-((pad_a/2)+90)%180;
          } // 90=mittig 0=links 180=rechts
				}
			}
		}
  }

  if(Serial.available())
  {
    str="speed_l=";
    str+=String(speed_l);
    str+=",speed_r=";
    str+=String(speed_r);
    str+=",maxSpeedFraction=";
    str+=String(maxSpeedFraction);
    str+=",pad_x=";
    str+=String(pad_x);
    str+=",pad_y=";
    str+=String(pad_y);
    str+=",pad_r=";
    str+=String(pad_r);
    str+=",pad_a=";
    str+=String(pad_a);
    Serial.println(str);
  }

  if(doSetSpeed)
  {
    md.setSpeeds(speed_r,speed_l);
    blink;
    Serial.println("md:speed_r="+String(speed_r)+",speed_l="+String(speed_l)+",maxSpeedFraction="+String(maxSpeedFraction)+",pad_x="+String(pad_x)+",pad_y="+String(pad_y));
  }
  if(doSetHead)
  {
    Head.write(headAngle);
    blink;
    Serial.println("head:pin="+String(pinHead)+",pad_a="+String(pad_a)+",headAngle="+String(headAngle));
  }

	t=millis()-t;if(t<interval){delay(interval-t);}
}


