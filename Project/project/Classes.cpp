/*
 * Classes.cpp
 *
 * Created: 22.01.2022 21:04:31
 * Author: Nils Bruder
 */ 

#include "Classes.h"

///////////////Konstruktor Clock///////////////
Clock :: Clock (LCD *lcd) : hours(0), minutes(0), seconds(0), display(lcd) {
		display->set_pos(0,0);
		display->write_SRAM_text("Zeit:  00:00:00");
}

///////////////Unterfunktion für Clock-ISR///////////////
void Clock :: callback() {
	seconds++;
	if(seconds >= 60) {
		seconds = 0;
		minutes++;
		if(minutes >= 60) {
			minutes = 0;
			hours++;
			if(hours >= 24) {
				hours = 0;
			}
		}
	}
}

///////////////Time Update Clock///////////////
void Clock :: time_update() {
	display -> set_pos(0,7);
	display -> write_number(hours, 2, '0');
	
	display -> set_pos(0,10);
	display -> write_number(minutes, 2, '0');
	
	display -> set_pos(0,13);
	display -> write_number(seconds, 2, '0');
}

///////////////Konstruktor Alarm///////////////
Alarm :: Alarm (LCD *lcd, DigiPortRaw *ledpointer, Clock *clock_1, uint8_t *eeprom_hours, uint8_t *eeprom_min) : hours_alarm(0), minutes_alarm(0), alarm(false), display(lcd), led(ledpointer), relatedClock(clock_1), eeprom_hours_alarm(eeprom_hours), eeprom_minutes_alarm(eeprom_min) {
	hours_alarm = eeprom_read_byte(eeprom_hours_alarm);				//liest aus dem EEPROM
	minutes_alarm = eeprom_read_byte(eeprom_minutes_alarm);			//liest aus dem EEPROM
	display->set_pos(1,0);
	display->write_SRAM_text("Alarm: ");									//Bereitet Display vor
	display -> write_number(hours_alarm, 2, '0');
	display->write_SRAM_text(":");
	display -> write_number(minutes_alarm, 2, '0');
}


///////////////Clear Alarm///////////////
void Alarm :: clear_alarm() {
	alarm = false;
	led->off();
}

///////////////Unterfunktion für Alarm-ISR///////////////
void Alarm :: callback() {
	if(hours_alarm == (relatedClock->get_h()) && minutes_alarm == relatedClock->get_m() && !(relatedClock->get_s())) {		//Setzt Alarm
		alarm = true;
	}
	
	if(alarm) {									//Toggelt LED's wenn Alarm an
		led->toggle(0b11111111);
	}
}

///////////////Konstruktor Temp-Sensor///////////////
Temp_Sensor :: Temp_Sensor (LCD *lcd, ADConverter *adc_s, uint8_t *eeprom_temp) : alarm(0), fahrenheit(0), eeprom_temp_alarm(eeprom_temp){
	temp_alarm = eeprom_read_byte(eeprom_temp_alarm);				//liest aus dem EEPROM
	display = lcd;
	adc_Sensor = adc_s; 
	display->set_pos(2,0);
	display->write_SRAM_text("T:");
}

///////////////Unterfunktion für Temp-Sensor-ISR///////////////
void Temp_Sensor :: get_measurement() {
	value = (uint8_t)(adc_Sensor->get_value() * 0.4736) - 273.15;	// Berechnete Umformung der Kennlinie
}

///////////////Temp Value Update///////////////
void Temp_Sensor ::	value_update() {
	if (!fahrenheit) {
		display -> set_pos(2,3);
		display -> write_number(value, 2, '0');
		display -> write_SRAM_text("\xDF""C (");
		display -> write_number(temp_alarm, 2, '0');
	} else {
		display -> set_pos(2,3);
		display -> write_number(((value * (9/5)) + 32), 2, '0');
		display -> write_SRAM_text("\xDF""F (");
		display -> write_number(((temp_alarm * 9/5) + 32), 2, '0');
	}
	display -> write_SRAM_text(")");
	
	if(value >= temp_alarm) {					//Setzt Alarm und schreibt Alarm auf LCD
		if (alarm == false){
			display -> set_pos(2,14);
			display -> write_SRAM_text("A!");	
			alarm = true;
		}
	} else{
		alarm = false;
	}
}


///////////////Clear Temp Alarm///////////////
void  Temp_Sensor :: clear_temp_alarm() {
	display -> set_pos(2,14);
	display -> write_SRAM_text("   ");
}

///////////////Set Temp Alarm///////////////
void Temp_Sensor :: set_alarm_value(uint8_t val) {		//Nehme Wert in C oder in F an
	if (fahrenheit){											
		val = (uint8_t) ((val - 32) / (1.8));					//von F in C
	}
	eeprom_update_byte(eeprom_temp_alarm, val);
	temp_alarm =  val;
}

///////////////Get Temp Alarm///////////////
uint8_t Temp_Sensor :: get_temp_alarm() {						//Gebe Wert in C oder in F aus
	if (fahrenheit){
		temp_alarm = (uint8_t) ((temp_alarm * (1.8)) + 32);		//von C in F
	}
	return temp_alarm;
}


///////////////Konstruktor Humidity-Sensor///////////////
Humidity_Sensor :: Humidity_Sensor (LCD *lcd, ADConverter *adc_s) {
	value = 0;
	display = lcd; 
	adc_Sensor = adc_s; 
	display->set_pos(3,0);
	display->write_SRAM_text("F:   %");
}

///////////////Unterfunktion für Humidity-Sensor-ISR///////////////
void Humidity_Sensor :: get_measurement() {
	value = (uint8_t)(adc_Sensor->get_value() * 0.1575 - 25.806);	// Umrechnung Celsius in Fahrenheit
}

///////////////Humidity Value Update///////////////
void Humidity_Sensor ::	value_update() {
	display -> set_pos(3,2);
	display -> write_number(value, 3, ' ');
}

