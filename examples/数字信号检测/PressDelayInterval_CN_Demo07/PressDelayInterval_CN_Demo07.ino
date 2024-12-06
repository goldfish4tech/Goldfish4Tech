/**
   传感器数字信号记录按下的时长和松开的时长   按下的状态，5s内，只打印一次。5s后，如果状态没变，会再次打印
  松开的状态，10s内，只打印一次。10s后，如果状态没变，会再次打印

  场景：如果在规定时间内没有做出动作响应，会二次提醒。例如手机闹钟

   代码用处：按下的状态，5s内，只打印一次。5s后，如果状态没变，会再次打印
  松开的状态，10s内，只打印一次。10s后，如果状态没变，会再次打印

  场景：如果在规定时间内没有做出动作响应，会二次提醒。例如手机闹钟
  火车司机30s踩一次脚踏板，没踩就会刹车
  每隔1个小时，需要按一下按钮，否则就怎么怎么样
  开发板：适用于Arduino系列开发板（如：NANO、UNO、Mega 2560、ESP32、ESP8266等）
   注意事项：输出0 为 低电平，输出1 为 高电平
  流程图链接：
  【金山文档】 PressDelayInterval_CN_Demo07
  https://kdocs.cn/l/ckQe2kM9gR4G
 
*/

#define sensorPin        2        // 传感器引脚
#define runIntervalTime  10       // 运行检测间隔时间 检测保持时长
unsigned long runTime    = 0;     // 阶段运行记录时间

bool lastTimeState = LOW;            // 上次状态值  默认为您当前初始化传感器的状态

unsigned long runningRecordTime    = 0;      // 记录按下时间

int pressCount = 0;
int noPressCount = 0;

#define pressCountTime    5000       // 输出检测间隔打印时间
#define noPressCountTime  10000      // 输出检测间隔打印时间

void setup() {
  Serial.begin(9600);                // 设置波特率
  pinMode(sensorPin, INPUT);         // 设置引脚输入
  Serial.println(F("Start successfully"));          // 打印提示
}

void loop() {
  // 运行
  if (millis() - runTime >= runIntervalTime) {      // 当前时间如果减去阶段记录时间大于并等于间隔时间
    runTime = millis();                             // 记录阶段时间
    if (sensorState() != lastTimeState) {           // 判断当前状态是否不等于上次的值
      lastTimeState = !lastTimeState;               // 状态反转
      Serial.print("The current status is: ");      // 输出打印
      Serial.println(lastTimeState);                // 输出打印 状态
      
      // 次数清除
      pressCount = 0;                               // 状态重置
      noPressCount = 0;                             // 状态重置
      
      // 记录时间
      runningRecordTime = millis();                 // 记录当前时间
    }
  }

  if (lastTimeState == true) {
    if (millis() - runningRecordTime >= pressCountTime) {
      runningRecordTime = millis();                       // 记录时间
      Serial.print("status is: ");                        // 输出打印
      Serial.print(lastTimeState);                        // 输出打印 状态
      Serial.print("  Count: ");                          // 输出打印
      Serial.println(++pressCount);                       // 自增次数打印
    }
  } else {
    if (millis() - runningRecordTime >= noPressCountTime) {
      runningRecordTime = millis();                       // 记录时间
      Serial.print("status is: ");                        // 输出打印
      Serial.print(lastTimeState);                        // 输出打印 状态
      Serial.print("  Count: ");                          // 输出打印
      Serial.println(++noPressCount);                     // 自增次数打印
    }
  }

  // loop回括号
}

// 获取传感器状态
bool sensorState() {
  bool _sensorFlag = digitalRead(sensorPin);
  return _sensorFlag;
}
