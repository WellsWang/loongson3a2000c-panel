/**************************************************************************
 Loongson 3A2000C Greatwall Mainborad Control Panel

 Author   : Wells Wang
 Version  : 0.1beta

 Release under GPL v3 License
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneButton.h>

#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64 // OLED display height, in pixels
#define OLED_RESET      -1 
#define SCREEN_ADDRESS  0x3C
Adafruit_SSD1306        display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define B_PREV          D5
#define B_NEXT          D4
#define B_OK            D3
#define B_PWR           D6
#define POWER           D7
OneButton btn_OK        = OneButton(B_OK, true, false);
OneButton btn_PREV      = OneButton(B_PREV, true, false);
OneButton btn_NEXT      = OneButton(B_NEXT, true, false);
OneButton btn_PWR       = OneButton(B_PWR, true, false);

//  Following is Chinese Character Matrix for Chinese UI.
//  测(0) 试(1) 中(2) 通(3) 过(4) 失(5) 败(6)
//#define TXT_HEIGHT   12
//#define TXT_WIDTH    12
//static const unsigned char PROGMEM txt[] =
//{0x00,0x20,0xBE,0x20,0x62,0xA0,0x2A,0xA0,0x2A,0xA0,0xAA,0xA0,0x6A,0xA0,0x2A,0xA0,0x2A,0xA0,0x48,0x20,0x94,0x20,0x22,0x60,/*"测",0*/
// 0x80,0xA0,0x40,0x80,0x0F,0xE0,0x00,0x80,0xC0,0x80,0x4E,0x80,0x44,0x80,0x44,0x80,0x44,0x80,0x46,0xA0,0x6C,0x60,0x40,0x20,/*"试",1*/
// 0x04,0x00,0x04,0x00,0x04,0x00,0x7F,0xC0,0x44,0x40,0x44,0x40,0x44,0x40,0x7F,0xC0,0x44,0x40,0x04,0x00,0x04,0x00,0x04,0x00,/*"中",2*/
// 0x9F,0x80,0x44,0x80,0x03,0x00,0x1F,0xC0,0xD2,0x40,0x5F,0xC0,0x52,0x40,0x5F,0xC0,0x52,0x40,0x52,0xC0,0x40,0x00,0xBF,0xE0,/*"通",3*/
// 0x40,0x40,0x20,0x40,0x00,0x40,0x0F,0xE0,0xE0,0x40,0x24,0x40,0x22,0x40,0x22,0x40,0x20,0x40,0x21,0xC0,0x50,0x00,0x8F,0xE0,/*"过",4*/
// 0xFB,0xFF,0xDB,0xFF,0xDB,0xFF,0xC0,0x7F,0xBB,0xFF,0x7B,0xFF,0xFB,0xFF,0x00,0x1F,0xF5,0xFF,0xEE,0xFF,0xDF,0x7F,0x3F,0x9F,/*"-失",5*/
// 0xFE,0xFF,0x06,0xFF,0x75,0xFF,0x54,0x1F,0x53,0xBF,0x55,0xBF,0x55,0xBF,0x55,0xBF,0x55,0x7F,0xDE,0xFF,0xAD,0x7F,0x73,0x9F,/*"-败",6*/
// 0x00,0x00,0x1F,0x80,0x3F,0xC0,0x76,0xE0,0x36,0xC0,0x7F,0xE0,0x3F,0xC0,0x79,0xE0,0x36,0xC0,0x7F,0xE0,0x3F,0xC0,0x1F,0x80,/* FAILED ICON 故障内存图标,7*/
// 0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x80,0x36,0xC0,0x10,0x80,0x30,0xC0,0x10,0x80,0x30,0xC0,0x10,0x80,0x30,0xC0,0x1F,0x80, /* MEMCHIP ICON 内存图标,8*/
// 0x00,0x00,0x1F,0x80,0x3F,0xC0,0x76,0xE0,0x36,0xC0,0x7F,0xE0,0x3F,0xC0,0x76,0xE0,0x39,0xC0,0x7F,0xE0,0x3F,0xC0,0x1F,0x80};/* PASSED ICON 内存笑脸图标,9*/

#define USAGE_WIDTH     76    //screen_width - width_of_text_"CPU:100%"
#define MODE_STATUS     1
#define MODE_TEMP       2
#define MODE_LOG        3


int     incomingByte      = 0; // for incoming serial data
String  inputString       = "";         // a String to hold incoming data
bool    stringComplete    = false;  // whether the string is complete
bool    update            = false;
byte    mode[3]           = {MODE_STATUS, MODE_TEMP, MODE_LOG};
byte    currentMode       = 0;
byte    cpuUsage[USAGE_WIDTH];
byte    memUsage[USAGE_WIDTH];
byte    temperature[USAGE_WIDTH];
String  hostname          = "MYHOSTNAME";
String  kernel            = "0.00.0";
String  ip                = "255.255.255.255";
String  displaytime       = "00:00";
String  uptime            = "000d00h";
byte    cu                = 0;
byte    mu                = 0;
byte    page              = 0;
String  logItem;



// 
// UI 相关
// =======================================================

/*
void drawTxt(int x, int y, byte id) {
  unsigned char ch[24];
  for (int i=0; i<24; i++){
    ch[i] = pgm_read_word_near(txt + id*24 +i);
   // Serial.println(txt[i],BIN);
  }
  display.drawBitmap(x,y,ch, TXT_WIDTH, TXT_HEIGHT, 1);
}
*/

void drawUsageGraph(byte graphData[], int x, int y){
  int i;

  display.fillRect(x, y, USAGE_WIDTH, 10, SSD1306_BLACK);
  for (i=0; i < USAGE_WIDTH; i++){
    display.drawLine(x + i, y+10-graphData[i], x+i, y+10, SSD1306_WHITE);
  }
  display.display();
}

void drawTempGraph(byte graphData[], int x, int y){
  int i;
  byte max_v = 0;
  byte min_v = 255;
  int last_y = 0;
  int pos_y;

  for (i=0; i < USAGE_WIDTH; i++){
    if (graphData[i] > 0) {
      max_v = max(graphData[i], max_v);
      min_v = min(graphData[i], min_v);
    }
  }

  display.fillRect(x, y, USAGE_WIDTH, 20, SSD1306_BLACK);
  for (i=0; i < USAGE_WIDTH; i++){
    if (graphData[i]>0){
      pos_y = y + 20 - int((graphData[i] - min_v + 5) *20 / (max_v-min_v+10));  // y + 20（图像宽度） - 当前温度在最大最小值上下5度之间的相对比例位置  
      if (last_y > 0) //前面有效数值，和前一点用线连接
        display.drawLine(x + i - 1, last_y, x+i, pos_y, SSD1306_WHITE);
      else            //前面无有效值，画第一个点
        display.drawPixel(x + i, pos_y , SSD1306_WHITE);
      last_y = pos_y;
    }
  }
  display.display();
}

void updateUI(){
  display.fillRect(0,0, display.width(), 16, SSD1306_BLACK);
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setCursor(0,0);   
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.print(hostname);
  display.fillRect(0,16, display.width(), 10, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);        // Draw white text
  display.setTextSize(1);
  display.setCursor(0,17);   
  display.print(ip);
  display.setCursor(display.width()-30,17);   
  display.print(displaytime);
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  switch (mode[currentMode]) {
    case MODE_STATUS:
      display.fillRect(104,33, 24, 20, SSD1306_BLACK);
      display.setCursor(104,33);
      display.print(cu);display.print("%");   
      display.setCursor(104,45);   
      display.print(mu);display.print("%");   
      display.fillRect(0,56, display.width(), 8, SSD1306_BLACK);
      display.setCursor(0,56);   
      display.print(kernel);
      display.setCursor(display.width()-uptime.length()*6, 56);   
      display.print(uptime);
      drawUsageGraph(cpuUsage, 0, 30);
      drawUsageGraph(memUsage, 0, 42);
      break;
    case MODE_TEMP:
      display.fillRect(86,45, 24, 16, SSD1306_BLACK);
      display.setTextSize(2);
      display.setCursor(86,45);  
      display.print(temperature[USAGE_WIDTH-1]);
      drawTempGraph(temperature, 0, 42);
      break;
    case MODE_LOG:
      display.fillRect(0,30, display.width(), display.height()-30, SSD1306_BLACK);
      if (logItem.length()>0){
        display.setCursor(0,30);
        display.print(logItem);
      }
      break;  
  }

  display.display();  
  update = false;
}

void drawUI(){
  display.clearDisplay();
  //display.drawLine(0, 25, display.width()-1, 25, SSD1306_WHITE);
  //drawTxt(80,20,8);

  switch(mode[currentMode]){
    case MODE_STATUS:
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setTextSize(1);
      display.setCursor(80,33);   
      display.print(F("CPU:"));
      display.setCursor(80,45);   
      display.print(F("MEM:"));
      break;
    case MODE_TEMP:
      display.setTextColor(SSD1306_WHITE); 
      display.setTextSize(1);
      display.setCursor(0,33);
      display.print(F("CPU Temperature"));
      display.setTextSize(2);
      display.setCursor(116,45);  
      display.print(F("C"));
      display.setTextSize(1);
      display.setCursor(112,39); 
      display.print(F("o"));
      break;
  }
}

// 
// 数据处理
// =======================================================

void updateUsageData(byte usageData[], byte newVal){
  int i;
  for (i=0; i< USAGE_WIDTH - 1; i++){
    usageData[i] = usageData[i+1];
  }
  usageData[i] = newVal;
}

byte getByteValue(String data, String cmd){
  byte temp_b[2] = {0,0};
  String temp;

  if (data.indexOf(cmd)>0){
    temp = data.substring(data.indexOf(cmd)+4, data.indexOf(cmd)+5);
    temp.getBytes(temp_b, 2);
  }
  return temp_b[0];
}

boolean updateValue(String data, String &key, String cmd){
  String temp;
  boolean res = false;
  if (data.indexOf(cmd)>0){
    temp = data.substring(data.indexOf(cmd)+4);
    temp = temp.substring(0,temp.indexOf("\n"));
    if (key != temp){
      key = temp;
      res = true;
    }
  }
  return res;
}

void parseData(String data){

  if (data){
    data = " " + data;
    updateValue(data, hostname, "!HN!");
    updateValue(data, ip, "!IP!");
    updateValue(data, kernel, "!KR!");
    updateValue(data, uptime, "!UT!");
    updateValue(data, displaytime, "!TI!");
    if (data.indexOf("!CG!")>0) updateUsageData(cpuUsage, getByteValue(data, "!CG!"));
    if (data.indexOf("!MG!")>0) updateUsageData(memUsage, getByteValue(data, "!MG!"));
    if (data.indexOf("!CT!")>0) updateUsageData(temperature, getByteValue(data, "!CT!"));
    if (data.indexOf("!CU!")>0) cu = getByteValue(data, "!CU!");
    if (data.indexOf("!MU!")>0) mu = getByteValue(data, "!MU!");
    if (data.indexOf("!LP!")>0) page = getByteValue(data, "!LP!");
    updateValue(data, logItem, "!LG!");

    update = true;
  }

}

// 
// 事件处理
// =======================================================


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    digitalWrite(LED_BUILTIN, LOW);
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      parseData(inputString);
      inputString = "";
    }
    digitalWrite(LED_BUILTIN, HIGH);
  }
}


static void clickOK() {
  currentMode = (currentMode + 1) % (sizeof(mode) / sizeof(mode[0]));
  if (mode[currentMode] == MODE_LOG){
    Serial.write("#GL#");
    Serial.write(page);
    Serial.write("\n");
  }
  drawUI();
  updateUI();
}

static void clickPREV() {
  if (mode[currentMode] == MODE_LOG){
    if (page > 0) page = page - 1;
    Serial.write("#GL#");
    Serial.write(page);
    Serial.write("\n");
    delay(100);
    updateUI();
  }
}

static void clickNEXT() {
  if (mode[currentMode] == MODE_LOG){
    if (page <255) page = page + 1;
    Serial.write("#GL#");
    Serial.write(page);
    Serial.write("\n");
    delay(100);
    updateUI();
  }
}

static void clickPWR() {
  digitalWrite(POWER, LOW);
  digitalWrite(POWER, HIGH);
  delay(5000);
  digitalWrite(POWER, LOW);
}



// 
// 初始化
// =======================================================

void setup() {
  
  int i;

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(POWER, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  
  digitalWrite(POWER, LOW);

  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(250);
  logItem.reserve(250);
  logItem = "";

  btn_OK.attachClick(clickOK);
  btn_PREV.attachClick(clickPREV);
  btn_NEXT.attachClick(clickNEXT);
  btn_PWR.attachClick(clickPWR);
  

  for (i=0; i < USAGE_WIDTH; i++){
    updateUsageData(cpuUsage, 0);
    updateUsageData(memUsage, 0);
    updateUsageData(temperature, 0);
  }
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  drawUI();
  updateUI();

}


// 
// 循环体
// =======================================================

void loop() {
  int i;
  // print the string when a newline arrives:
  if (update) {
    updateUI();
  }
  btn_OK.tick();
  btn_PREV.tick();
  btn_NEXT.tick();
  btn_PWR.tick();
}