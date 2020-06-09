#include <M5Stack.h>

#define LEDC_CHANNEL 3
#define LEDC_TIMER_BIT 10
//#define LEDC_FREQUENCY 50
#define LED_PIN 21 // Grove White
//#define LED_PIN 22 // Grove Yellow

#define HIGHT_MAX 240
#define WIDTH_MAX 320

#define MODE_INIT 0
#define MODE_MENU 1
#define MODE_DUTY 2
#define MODE_FREQ 3
#define MODE_TIMER 4
#define MODE_RUN  5
#define MODE_RUNNING 6
#define MODE_STOP 7

struct MENU {
  char name[16];
  uint8_t cmdcode;
};

MENU menu[] = {
  { " Run LED Test" , 0x00 },
  { " "  , 0x01 },
  { " Set Duty"  , 0x02 },
  { " Set Frequency"  , 0x03 },
  { " Set Timer"  , 0x04 },
};

int cursor = 0;
int mode = MODE_INIT;
bool mode_change = false;

int duty = 20;   // %
int freq = 100;   // Hz
int runtime = 60;   //minutes

uint32_t runtime_sec = 0; // sec for count down
uint32_t target_time = 0;       // for next 1 second timeout
boolean count_down = false;

void setup() {
  M5.begin();
  clear_led();
  mode = MODE_MENU;
  M5.Lcd.setTextSize(3);
  show_menu();
}

void loop() {
  M5.update();
  switch(mode) {
    case MODE_RUNNING:
      func_running();
      break;
    case MODE_RUN:
      func_run();
      break;
    case MODE_STOP:
      func_stop();
      break;
    case MODE_DUTY:
      func_duty();
      break;
    case MODE_FREQ:
      func_freq();
      break;
    case MODE_TIMER:
      func_timer();
      break;
    case MODE_MENU:
      func_menu();
      break;
    default:
      //Do nothing
      func_menu();
      break;
  }
}

void func_menu() {
  if (mode_change) {
    show_menu();
    mode_change = false;
  }
  if (M5.BtnC.wasPressed()) {
    switch(cursor) {
      case 0: mode = MODE_RUN; break;
      case 1: mode = MODE_MENU; break;
      case 2: mode = MODE_DUTY; break;
      case 3: mode = MODE_FREQ; break;
      case 4: mode = MODE_TIMER; break;
    }
    mode_change = true;
  }
  if (M5.BtnA.wasPressed()) {
    cursor--;
    cursor = cursor % (sizeof(menu) / sizeof(MENU));
    M5.Lcd.setCursor(0, 8);
    for ( int i = 0 ; i < (sizeof(menu) / sizeof(MENU)) ; i++ ) {
      M5.Lcd.println((cursor == i) ? ">" : " ");
    }
  }
  if (M5.BtnB.wasPressed()) {
    cursor++;
    cursor = cursor % (sizeof(menu) / sizeof(MENU));
    M5.Lcd.setCursor(0, 8);
    for ( int i = 0 ; i < ( sizeof(menu) / sizeof(MENU) ) ; i++ ) {
      M5.Lcd.println((cursor == i) ? ">" : " ");
    }
  }
}

void show_menu() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 8);
  for (int i = 0 ; i < ( sizeof(menu) / sizeof(MENU)) ; i++ ) {
    M5.Lcd.print((cursor == i) ? ">" : " ");
    M5.Lcd.println(menu[i].name);
  }
  show_bottom("UP", "DOWN", "OK");
}

void func_duty() {
  if (mode_change) {
    show_duty();
    mode_change = false;
  }
  if (M5.BtnC.wasPressed()) {
    mode = MODE_MENU;
    mode_change = true;
  }
  if (M5.BtnA.wasPressed()) {
    if (duty >= 100) duty = 100;
    else if (duty < 10) duty += 2;
    else duty += 5;
    show_duty();
  }
  if (M5.BtnB.wasPressed()) {
    if (duty <= 0) duty = 0;
    else if (duty <= 10) duty -= 2;
    else duty -= 10;
    show_duty();
  }
}

void show_duty() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(40,50);
  M5.Lcd.printf("Duty: %3d %%", duty);
  show_bottom("UP", "DOWN", "OK");
}

void func_freq() {
  if (mode_change) {
    show_freq();
    mode_change = false;
  }
  if (M5.BtnC.wasPressed()) {
    mode = MODE_MENU;
    mode_change = true;
  }
  if (M5.BtnA.wasPressed()) {
    if (freq >= 10000) freq = 10000;
    else if (freq >= 1000) freq += 1000;
    else if (freq >= 100) freq += 50;
    else freq += 10;
    show_freq();
  }
  if (M5.BtnB.wasPressed()) {
    if (freq <= 10) freq = 10;
    else if (freq <= 100) freq -= 10;
    else if (freq <= 1000) freq -= 50;
    else freq -= 1000;
    show_freq();
  }
}

void show_freq() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(40,50);
  M5.Lcd.printf("Freq: %d Hz", freq);
  show_bottom("UP", "DOWN", "OK");
}

void func_timer() {
  if (mode_change) {
    show_timer();
    mode_change = false;
  }
  if (M5.BtnC.wasPressed()) {
    mode = MODE_MENU;
    mode_change = true;
  }
  if (M5.BtnA.wasPressed()) {
    if (runtime >= 24*60) runtime = 24*60;
    else if (runtime >= 120) runtime += 30;
    else runtime += 10;
    show_timer();
  }
  if (M5.BtnB.wasPressed()) {
    if (runtime <= 10) runtime = 10;
    else if (runtime > 120) runtime -= 30;
    else runtime -= 10;
    show_timer();
  }
}

void show_timer() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(40,50);
  M5.Lcd.printf("Timer: %d min", runtime);
  show_bottom("UP", "DOWN", "OK");
}

void func_run() {
  if (mode_change) {
    show_run();
    mode_change = false;
  }
  if (M5.BtnA.wasPressed()) {
    //Start LED test
    mode = MODE_RUNNING;
    mode_change = true;
  }
  if (M5.BtnB.wasPressed()) {
    //Do nothing
  }
  if (M5.BtnC.wasPressed()) {
    // Cancel
    mode = MODE_MENU;
    mode_change = true;
  }
}
void show_run() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(40,35);
  M5.Lcd.print("Run LED Test?");
  M5.Lcd.setCursor(40,75);
  M5.Lcd.printf(" %d min", runtime);
  M5.Lcd.setCursor(40,105);
  M5.Lcd.printf(" %d %%", duty);
  M5.Lcd.setCursor(40,135);
  M5.Lcd.printf(" %d Hz", freq);
  show_bottom("YES", "  ", "NO");
}

void func_running() {
  if (mode_change) {
    //Start LED test
    run_led();
    show_running();
    mode_change = false;
    target_time = millis() + 1000;
    runtime_sec = runtime*60;
    count_down = true;
  }
  if (M5.BtnA.wasPressed()) {
  }
  if (M5.BtnB.wasPressed()) {
  }
  if (M5.BtnC.wasPressed()) {
    // Cancel
    clear_led();
    mode = MODE_STOP;
    mode_change = true;
  }
  func_countdown();
}

void func_countdown() {
  if (target_time < millis() && count_down) {
    // Set next update for 1 second later
    target_time = millis() + 1000;
    runtime_sec -= 1; //count down 1 sec
    if (runtime_sec == 0) {
      clear_led();
      count_down = false;
      mode = MODE_STOP;
      mode_change = true;
    }
    uint8_t rhour = runtime_sec/3600;
    uint8_t rmin = (runtime_sec-(rhour*3600))/60;
    uint8_t rsec = runtime_sec-(rhour*3600)-(rmin*60);
    M5.Lcd.setCursor(40,75);
    M5.Lcd.printf(" %02d:%02d:%02d", rhour, rmin, rsec);
  }
}

void func_stop() {
  if (mode_change) {
    show_stop();
    mode_change = false;
  }
  if (M5.BtnA.wasPressed()) {
    mode = MODE_MENU;
    mode_change = true;
  }
  if (M5.BtnB.wasPressed()) {
  }
  if (M5.BtnC.wasPressed()) {
  }
}
void show_stop() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(40,35);
  M5.Lcd.print("LED Stopped");
  show_bottom("MENU", "", "");
}

void clear_led() {
  ledcSetup(LEDC_CHANNEL, freq, LEDC_TIMER_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL);
  ledcWrite(LEDC_CHANNEL, 0);
}

void run_led() {
  ledcSetup(LEDC_CHANNEL, freq, LEDC_TIMER_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL);
  ledcWrite(LEDC_CHANNEL, (int)duty*1023/100);
}

void show_running() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(40,35);
  M5.Lcd.print("Running LED ...");
  M5.Lcd.setCursor(40,105);
  M5.Lcd.printf(" %d %%", duty);
  M5.Lcd.setCursor(40,135);
  M5.Lcd.printf(" %d Hz", freq);
  show_bottom("", "", "STOP");
}

void show_bottom(String str1, String str2, String str3) {
  M5.Lcd.setCursor(46,200);
  M5.Lcd.print(str1);
  M5.Lcd.setCursor(135,200);
  M5.Lcd.print(str2);
  M5.Lcd.setCursor(235,200);
  M5.Lcd.print(str3);
}
