/*
 * main.cpp
 *
 * Created: 22.01.2022 20:59:53
 * Author: Nils Bruder
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <Timer.h>
#include <QuadEncoder.h>
#include <Basics.h>
#include "Classes.h"

EEMEM uint8_t eeprom_temp_alarm = 25;			//reservieren und inizaliesieren von Platz im EEPROM 
EEMEM uint8_t eeprom_hours_alarm = 0;
EEMEM uint8_t eeprom_minutes_alarm = 1;

///////////////Funktionsdeklarationen///////////////
void ticker_clockISR(void);
void ticker_sensorISR(void);
inline uint8_t quadEndcoderRead (LCD *display, uint8_t line, uint8_t pos, uint8_t len, uint8_t min, uint8_t max, int8_t start);

///////////////Initialisierungen///////////////
static DigiPortIRPT myKeys (PK, SET_IN_PORT);
static QuadEncoder myQuadEncoder (PJ);
static LCD myDisplay_1 (PC, LCD_Type_40x4, WRAPPING_OFF | BLINK_OFF | CURSOR_OFF | DISPLAY_ON);
//static LCD myDisplay_2 (PL, LCD_Type_16x2, WRAPPING_OFF | BLINK_OFF | CURSOR_OFF | DISPLAY_ON);
static Timer16 ticker_Clock(TC1, ticker_clockISR);
static Timer16 ticker_Sensor(TC3, ticker_sensorISR);
static DigiPortRaw myLeds (PA, SET_OUT_PORT);
static ADConverter temp_adc (AD_CHAN_3);
static ADConverter humidity_adc (AD_CHAN_5);

static Clock myClock(&myDisplay_1);
static Alarm myAlarm(&myDisplay_1, &myLeds, &myClock, &eeprom_hours_alarm, &eeprom_minutes_alarm);
static Temp_Sensor myTemp_Sensor(&myDisplay_1, &temp_adc, &eeprom_temp_alarm);
static Humidity_Sensor myHumidity_Sensor(&myDisplay_1, &humidity_adc);

///////////////Variablen///////////////
volatile bool do_update_display1 = false;
volatile bool do_update_display2 = false;
const char *setminutes = "Set minutes";
const char *sethours = "Set hours";
const char *setseconds = "Set seconds";
const char *setalarmminutes = "Set Alarm minutes";
const char *setalarmhours = "Set Alarm hours";
const char *settempalarm = "Alarm set";

///////////////MAIN///////////////
int main(void){
	sei ();
	ticker_Clock.start_ms(1000);				// Startet Timer der Clock (Alle Sekunde)
	ticker_Sensor.start_ms(5000);			// Startet Timer für Sensoren (Alle 5 Sekunden)
	myAlarm.clear_alarm();					// LED's deaktivieren und Alarm deaktivieren
	
	while (1){
		if (do_update_display1){						//Updatet Display 1
			myClock.time_update();
			do_update_display1 = false;
		}
		if (do_update_display2){						//Updatet Display 2
			myTemp_Sensor.value_update();
			myHumidity_Sensor.value_update();
			do_update_display2 = false;
		}
		
		switch(myKeys.falling_edge()) {			//liest Tasten ein
			case 0b00000001:
			myClock.set_h(quadEndcoderRead(&myDisplay_1, 0, 7, 2, 0, 23, myClock.get_h()));
			break;
			case 0b00000010:
			myClock.set_m(quadEndcoderRead(&myDisplay_1, 0, 10, 2, 0, 59, myClock.get_m()));
			break;
			case 0b00000100:
			myClock.set_s(quadEndcoderRead(&myDisplay_1, 0, 13, 2, 0, 59, myClock.get_s()));
			break;
			case 0b00001000:
			myAlarm.set_ah(quadEndcoderRead(&myDisplay_1, 1, 7, 2, 0, 23, myAlarm.get_ah()));
			break;
			case 0b00010000:
			myAlarm.set_am(quadEndcoderRead(&myDisplay_1, 1, 10, 2, 0, 59, myAlarm.get_am()));
			break;
			case 0b00100000:
			myTemp_Sensor.set_alarm_value(quadEndcoderRead(&myDisplay_1, 0, 9, 2, 0, 120, myTemp_Sensor.get_temp_alarm()));
			break;
			case 0b01000000:
			myTemp_Sensor.convert_unit();
			break;
			case 0b10000000:
			myTemp_Sensor.clear_temp_alarm();
			myAlarm.clear_alarm();
			break;
		}
	}
}

///////////////ClockISR///////////////
void ticker_clockISR() {				//ISR für Clock Timer (1000ms)
	myClock.callback();
	myAlarm.callback();
	do_update_display1 = true;
}

///////////////SensorISR///////////////
void ticker_sensorISR() {				//ISR für Sensor Timer (5000ms)
	myTemp_Sensor.get_measurement();
	myHumidity_Sensor.get_measurement();
	do_update_display2 = true;
}


///////////////QuadEndcoderInput///////////////
inline uint8_t quadEndcoderRead (LCD *display, uint8_t line, uint8_t pos, uint8_t len, uint8_t min, uint8_t max, int8_t start) {
	uint8_t val = 0;
	
	display->set_pos(line, pos);
	display->blink_on();
	myQuadEncoder.start(min, max, start);
	while(!myQuadEncoder.new_locked_value_available()) {
		if(myQuadEncoder.new_value_available()) {
			val = (uint8_t) myQuadEncoder.get_unlocked_value();
			display->write_number(val, len, '0');
			display->set_pos(line, pos);
		}
	}
	myQuadEncoder.stop();
	display->blink_off();
	return val;
}



