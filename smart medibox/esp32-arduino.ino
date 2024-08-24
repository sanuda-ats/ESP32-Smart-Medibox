//Improting Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

//Defining the variables
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3c

#define LED_1 15
#define LED_2 2
#define LED_3 4
#define BUZZER 5
#define PB_CANCEL 34
#define PB_OK  32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 12

#define NTP_SERVER     "pool.ntp.org"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dhtSensor;

//Global Variables
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

int utc_offset = 0;

bool alarm_enabled = true;
int n_alarms = 3;
int alarm_hours[] = {0, 0, 0};
int alarm_minute[] = {0, 0, 0};
bool alarm_triggered[] = {false, false, false};

int C = 264;
int D = 294;
int E = 330;
int F = 340;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[][3] = {{C, D, E}, {F, G, A}, {A, B, C_H}};
int n_notes = 3;

//Main Menu
int current_mode = 0;
int max_modes = 5;  
String modes[] = {"1 - Set Time Zone", "2 - Set Alarm 1", "3 - Set Alarm 2", "4 - Set Alarm 3", "5 - Disable Alarms", };

void setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(PB_CANCEL, INPUT);
  pinMode(PB_OK, INPUT);
  pinMode(PB_UP, INPUT);
  pinMode(PB_DOWN, INPUT);

  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed."));
    for (;;);
  }

  // Turn on OLED display
  display.display();
  delay(2000);

  // Clear the display
  display.clearDisplay();

  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  // Connecting to WiFi
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WiFi", 0, 0, 2);
  }

  display.clearDisplay();
  print_line("Connected to WiFi", 0, 0, 2); //Successfully connected to WiFi
  delay(1000);

  configTime(utc_offset * 3600, 0, NTP_SERVER);  // Pass the time zone offset in seconds

  // Print the welcome message
  display.clearDisplay();
  print_line("Welcome to Medibox!", 0, 0, 2);
  delay(1000);
  display.clearDisplay();
}

void loop() {
  delay(10); // To speeds up the simulation
  update_time_with_check_alarm();

  if (digitalRead(PB_OK) == LOW) {
    //delay(200);
    go_to_menu();
  }

  check_temp(); //Checking the temperature
  delay(400);
  check_humidity(); //Checking the humidity
  delay(400);
}

void print_line(String text, int column, int row, int text_size) {
  // Display a custom message
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);

  display.display();
}

void print_time_now(void) {
  //Print the current time in the OLED display
  display.clearDisplay();
  print_line(String(hours)+":"+String(minutes)+":"+String(seconds),0,0,2);
}

void update_time() {
  //Updating the time
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char timeHour[3];
  strftime(timeHour,3,"%H",&timeinfo);
  hours = atoi(timeHour);
  
  char timeMinute[3];
  strftime(timeMinute,3,"%M",&timeinfo);
  minutes = atoi(timeMinute);

  char timeSeconds[3];
  strftime(timeSeconds,3,"%S",&timeinfo);
  seconds = atoi(timeSeconds);
}

int wait_for_button_press() {
  //Checking whether any button has pressed and if yes, do the process accordingly
  while (true) {
    if (digitalRead(PB_UP) == LOW) {
      delay(200);
      return PB_UP;
    }
    else if (digitalRead(PB_DOWN) == LOW) {
      delay(200);
      return PB_DOWN;
    }
    else if (digitalRead(PB_OK) == LOW) {
      delay(200);
      return PB_OK;
    }
    else if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      return PB_CANCEL;
    }
    update_time();
  }
}

void go_to_menu() {
  //Display the menu when user press the menu button
  while (digitalRead(PB_CANCEL) == HIGH) {
    display.clearDisplay();
    print_line(modes[current_mode], 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      current_mode += 1;
      current_mode = current_mode % max_modes;
    }
    else if (pressed == PB_DOWN) {
      delay(200);
      current_mode -= 1;
      if (current_mode < 0) {
        current_mode = max_modes - 1;
      }
    }
    else if (pressed == PB_OK) {
      delay(200);
      run_mode(current_mode);
    }
    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }
}

void ring_alarm(int alarm_index) {
  // Ringing the specified alarm
  display.clearDisplay();
  print_line("Medicine Time!", 0, 0, 2); // Message on the OLED display
  digitalWrite(LED_1, HIGH); // Turn on the LED

  bool break_happened = false;

  while(break_happened == false && digitalRead(PB_CANCEL) == HIGH) {
    // Ring the buzzer with a tone specific to the alarm
    for (int i = 0; i < n_notes; i++) {
      if (digitalRead(PB_CANCEL) == LOW) {
        delay(200);
        break_happened = true;
        break;
      }
      tone(BUZZER, notes[alarm_index][i]);
      delay(alarm_index * 500);
      noTone(BUZZER);
      delay(2);
    }
  }

  digitalWrite(LED_1, LOW); // Turn off the LED1
  display.clearDisplay();
}

void update_time_with_check_alarm(void) {
  //Updating the time every second whilw checking whether an alarm has occured.
  update_time();

  //Print the current time
  print_time_now();

  if (alarm_enabled == true) {
    for (int i = 0; i < n_alarms; i++) {
      if (alarm_triggered[i] == false && alarm_hours[i] == hours && alarm_minute[i] == minutes) {
        ring_alarm(i+1);
        alarm_triggered[i] = true;
      }
    }
  }
}

void set_time_zone() {
  //Taking the time zone as an input and set the time according to that
  int temp_offset_whole = utc_offset;
  int temp_offset_fractional = 0;

  while (true) {
    display.clearDisplay();
    print_line("Enter UTC Offset (Whole Part): " + String(temp_offset_whole) , 0, 0, 1);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      temp_offset_whole += 1;
      temp_offset_whole = constrain(temp_offset_whole, -12, 12);  // Constrain whole part to a reasonable range
    } 
    else if (pressed == PB_DOWN) {
      delay(200);
      temp_offset_whole -= 1;
      temp_offset_whole = constrain(temp_offset_whole, -12, 12);
    } 
    else if (pressed == PB_OK) {
      delay(200);
      while (true) {
        display.clearDisplay();
        print_line("Enter UTC Offset (Fractional Part): " + String(temp_offset_fractional * 15), 0, 0, 1);

        int pressed_fractional = wait_for_button_press();

        if (pressed_fractional == PB_UP) {
          delay(200);
          temp_offset_fractional = (temp_offset_fractional + 1) % 4;  // 0, 15, 30, 45
        } 
        else if (pressed_fractional == PB_DOWN) {
          delay(200);
          temp_offset_fractional = (temp_offset_fractional - 1 + 4) % 4;
        } 
        else if (pressed_fractional == PB_OK) {
          delay(200);
          utc_offset = temp_offset_whole * 3600 + temp_offset_fractional * 15 * 60;
          configTime(utc_offset, 0, NTP_SERVER);  // Update time with the new time zone offset
          break;
        } 
        else if (pressed_fractional == PB_CANCEL) {
          delay(200);
          break;
        }
      }
      break;
    } 
    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }

  display.clearDisplay();
  print_line("Time zone is set.", 0, 0, 2);
  delay(1000);
}

void set_alarm(int alarm) {
  //Setting up the alarms
  int temp_hour = alarm_hours[alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter Hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }
    else if (pressed == PB_DOWN) {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0) {
        temp_hour = 23;
      }
    }
    else if (pressed == PB_OK) {
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }
    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }

  int temp_minute = alarm_minute[alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter Minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }
    else if (pressed == PB_DOWN) {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0) {
        temp_minute = 59;
      }
    }
    else if (pressed == PB_OK) {
      delay(200);
      alarm_minute[alarm] = temp_minute;
      break;
    }
    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }
  display.clearDisplay();
  print_line("Alarm is set.", 0, 0, 2);
  delay(1000);
}

void run_mode(int mode) {
  //Difine the scenarios that should be happend when a cirtain option is selected in the menu
  if (mode == 0) {
    set_time_zone();
  }
  else if (mode == 1 || mode == 2 || mode ==3) {
    set_alarm(mode - 1);
  }
  else if (mode == 4) {
    alarm_enabled = false;
    display.clearDisplay();
    print_line("All alarms disabled", 0,0,2);
    delay(1000);
  }
}

void check_temp() {
  //Check the current temperature and infom if it is not in the desired range
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if (data.temperature > 32) {
    display.clearDisplay();
    print_line("TEMP HIGH!", 0, 40, 1);
    blink_temp_LED();
    
  }
  else if (data.temperature < 26) {
    display.clearDisplay();
    print_line("TEMP LOW!", 0, 40, 1);
    blink_temp_LED();
  }
}

void check_humidity(){
  //Check the current temperature and infom if it is not in the desired range
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if (data.humidity > 80) {
    display.clearDisplay();
    print_line("HUMIDITY HIGH!", 0, 50, 1);
    blink_hum_LED();
  }
  else if (data.humidity < 60) {
    display.clearDisplay();
    print_line("HUMIDITY LOW!", 0, 50, 1);
    blink_hum_LED();
  }
}

void blink_temp_LED(){
  //Blink the LED2 when the temperature is not in the desired range
  digitalWrite(LED_2, HIGH);
  delay(50);
  digitalWrite(LED_2,LOW);
}

void blink_hum_LED(){
  //Blink the LED3 when thr humidity is not in the desired range
  digitalWrite(LED_3, HIGH);
  delay(50);
  digitalWrite(LED_3,LOW);
}