
/*
 * Classes.h
 *
 * Created: 22.01.2022 21:02:14
 * Author: Nils Bruder
 */ 


#ifndef CLASSES_H_
#define CLASSES_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include <LCD.h>
#include <ADConverter.h>
#include <DigiPort.h>

///////////////Clock Klasse///////////////
class Clock {
	private:
	uint8_t hours, minutes, seconds;
	
	LCD *display;
	
	public:
	Clock(LCD *);
	
	uint8_t get_h();
	uint8_t get_m();
	uint8_t get_s();
	
	void set_h(uint8_t value);
	void set_m(uint8_t value);
	void set_s(uint8_t value);
	
	void time_update();
	void callback();
};



inline uint8_t Clock :: get_h() {return hours;}
inline uint8_t Clock :: get_m() {return minutes;}
inline uint8_t Clock :: get_s() {return seconds;}

inline void Clock :: set_h(uint8_t value) {hours = value;}
inline void Clock :: set_m(uint8_t value) {minutes = value;}
inline void Clock :: set_s(uint8_t value) {seconds = value;}
	
///////////////Alarm Klasse///////////////
class Alarm {
	private:
	uint8_t hours_alarm, minutes_alarm;
	bool alarm;
	
	LCD *display;
	DigiPortRaw *led;
	Clock *relatedClock;
	
	uint8_t *eeprom_hours_alarm;				//Zeiger auf reservierten Platz im EEPROM
	uint8_t *eeprom_minutes_alarm;			
	
	public:
	Alarm(LCD *, DigiPortRaw *, Clock *, uint8_t *, uint8_t *);
	
	uint8_t get_ah();
	uint8_t get_am();
	
	void set_ah(uint8_t value);
	void set_am(uint8_t value);
	
	void clear_alarm();
	void callback();
};

inline uint8_t Alarm :: get_ah() {return hours_alarm;}
inline uint8_t Alarm :: get_am() {return minutes_alarm;}

inline void Alarm :: set_ah(uint8_t val) {
	hours_alarm = val;
	eeprom_update_byte(eeprom_hours_alarm, val);				//updatet EEPROM
}
inline void Alarm :: set_am(uint8_t val) {
	minutes_alarm = val;
	eeprom_update_byte(eeprom_minutes_alarm, val);			//updatet EEPROM
}

///////////////Abstrakte Sensor Klasse///////////////
class Sensor {
	protected:
		uint8_t value;
		
		LCD *display;
		ADConverter *adc_Sensor;
	public:
		virtual void get_measurement()=0;
		virtual void value_update();
};

///////////////Temp-Sensor Klasse///////////////
class Temp_Sensor: Sensor{
	private:
		uint8_t temp_alarm;
		
		bool alarm;
		bool fahrenheit;
		
		uint8_t *eeprom_temp_alarm;				//EEPROM für Alarm temp
	public:
		Temp_Sensor(LCD *, ADConverter *, uint8_t *);
		
		
		void get_measurement();
		uint8_t get_temp_alarm();
		void set_alarm_value(uint8_t temp_alarm);
		void clear_temp_alarm();
		void convert_unit();
		void value_update();
};
inline void Temp_Sensor :: convert_unit() {fahrenheit = !fahrenheit;}
	
	
///////////////Humidity Sensor Klasse///////////////
class Humidity_Sensor : Sensor{
	public:
		Humidity_Sensor(LCD *, ADConverter *);
	
		void get_measurement();
		void value_update();
};



#endif /* CLASSES_H_ */