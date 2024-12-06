/**
   传感器数字信号记录按下的时长和松开的时长   获取按下 松开 时间差

   代码用处：记录按下的时长和松开的时长  map映射输出值
    开发板：适用于Arduino系列开发板（如：NANO、UNO、Mega 2560、ESP32、ESP8266等）
   注意事项：输出0 为 低电平，输出1 为 高电平

  流程图链接：
  【金山文档】 PressLoosenDuration_CN_Demo03
   https://kdocs.cn/l/cdsY9VAK7CSG

*/

#define sensorPin        2        // 传感器引脚
#define runIntervalTime  10       // 运行检测间隔时间 检测保持时长
unsigned long runTime    = 0;     // 阶段运行记录时间

bool lastTimeState = LOW;            // 上次状态值  默认为您当前初始化传感器的状态

unsigned long pressTime    = 0;      // 记录按下时间
unsigned long noPressTime  = 0;      // 记录松开时间

#define printIntervalTime  1000      // 输出检测间隔打印时间
unsigned long printRunTime = 0;      // 记录阶段运行打印时间

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
      // 根据状态记录时间
      if (lastTimeState == true) {                  // 判断按下时间
        pressTime = millis();                       // 记录按下时间
      } else {                                      // 判断松开时间
        noPressTime = millis();                     // 记录松开时间
      }
    }
  }

  // 间隔时间处理 运行触发打印时间间隔
  if (millis() - printRunTime >= printIntervalTime) {     // 判断输出打印时间间隔
    printRunTime = millis();                              // 记录阶段时间
    // 根据当前状态 打印时间间隔
    if (lastTimeState == true) {                          // 判断当前状态是否为高电平
      Serial.print("status is: ");                        // 输出打印
      Serial.print(lastTimeState);                        // 输出打印 状态
      Serial.print("  press IntervalTime(ms): ");         // 输出打印
      Serial.println(millis() - pressTime);               // 输出打印 间隔差值
    } else {
      Serial.print("status is: ");                        // 输出打印
      Serial.print(lastTimeState);                        // 输出打印 状态
      Serial.print("  release IntervalTime(ms): ");       // 输出打印
      Serial.println(millis() - noPressTime);             // 输出打印 间隔差值
    }
  }

  // loop回括号
}

// 获取传感器状态
bool sensorState() {
  bool _sensorFlag = digitalRead(sensorPin);
  return _sensorFlag;
}
