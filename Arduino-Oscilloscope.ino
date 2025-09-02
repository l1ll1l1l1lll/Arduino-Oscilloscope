#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define ANALOG_PIN A0
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SAFE_WIDTH 126 // 不显示右两列
#define BUFFER_SIZE SAFE_WIDTH

uint8_t buffer[BUFFER_SIZE]; // 存储波形数据

// 记录上一次采样值，用于过零检测
float lastVoltage_10 = 0;

unsigned long fpsLastTime = 0;
int frameCount = 0;
float fps = 0;

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 133; // 10Hz -> 100ms

// 安全画点
void drawPixelSafe(int x, int y) {
  if (x >= 0 && x < SAFE_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
    u8g2.drawPixel(x, y);
  }
}

// 安全画线
void drawLineSafe(int x0,int y0,int x1,int y1){
  int dx=abs(x1-x0), dy=abs(y1-y0);
  int sx=(x0<x1)?1:-1, sy=(y0<y1)?1:-1;
  int err=dx-dy;
  while(true){
    drawPixelSafe(x0,y0);
    if(x0==x1 && y0==y1) break;
    int e2=2*err;
    if(e2> -dy){ err-=dy; x0+=sx;}
    if(e2< dx){ err+=dx; y0+=sy;}
  }
}

// 采样 ADC
float sampleAnalog() {
  for (int i = 0; i < BUFFER_SIZE-1; i++) {
    buffer[i] = buffer[i+1]; // 左移一格
  }

  int val = analogRead(ANALOG_PIN); // 0~1023
  float voltage = (val / 1023.0) * 5.0; // 0~5V

  float voltage_10 = 4 *(voltage-2.5)+0.09;

  // 映射屏幕坐标
  int y = map(voltage * 1000, 0, 5000, SCREEN_HEIGHT-1, 0);
  buffer[BUFFER_SIZE-1] = y;

  return voltage_10;
}

// 绘制波形
void drawOscilloscope() {
  for (int x = 0; x < BUFFER_SIZE-1; x++) {
    int y0 = buffer[x];
    int y1 = buffer[x+1];
    if (y0 < 0) y0 = 0;
    if (y0 >= SCREEN_HEIGHT) y0 = SCREEN_HEIGHT-1;
    if (y1 < 0) y1 = 0;
    if (y1 >= SCREEN_HEIGHT) y1 = SCREEN_HEIGHT-1;
    drawLineSafe(x, y0, x+1, y1);
  }
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  for (int i=0;i<BUFFER_SIZE;i++) buffer[i]=SCREEN_HEIGHT-1; // 初始在底部
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpdateTime >= updateInterval) {
    lastUpdateTime = now;

    // 采样
    float voltage_10 = sampleAnalog();

    // 绘制 OLED
    u8g2.firstPage();
    do {
        // 背景网格
        for(int y=0;y<SCREEN_HEIGHT;y+=8){
            for(int x=0;x<SAFE_WIDTH;x+=8) drawPixelSafe(x,y);
        }
        drawOscilloscope();

        // 显示电压
        u8g2.setFont(u8g2_font_6x10_tf);
        char buf[16];
        dtostrf(voltage_10, 5, 2, buf);
        u8g2.drawStr(25, 10, buf);
        u8g2.drawStr(25 + strlen(buf)*6, 10, "V");

        // 显示刷新帧率
        char fpsBuf[16];
        dtostrf(fps, 5, 2, fpsBuf);
        u8g2.drawStr(25, 22, fpsBuf);
        u8g2.drawStr(25 + strlen(fpsBuf)*6, 22, "FPS");

    } while(u8g2.nextPage());

    // 计算帧率
    frameCount++;
    if (now - fpsLastTime >= 1000) { // 每秒更新一次帧率
        fps = frameCount * 1000.0 / (now - fpsLastTime);
        fpsLastTime = now;
        frameCount = 0;
    }
  }
}
