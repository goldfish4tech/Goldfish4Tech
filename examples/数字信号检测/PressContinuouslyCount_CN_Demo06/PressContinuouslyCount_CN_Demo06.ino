/**
   传感器数字信号记录  连续有10次按下，才打印一次按下
  连续20次松开，才打印松开

   代码用处：连续有10次按下，才打印一次按下
  连续20次松开，才打印松开
    开发板：适用于Arduino系列开发板（如：NANO、UNO、Mega 2560、ESP32、ESP8266等）
   注意事项：输出0 为 低电平，输出1 为 高电平
*/

#define sensorPin        2        // 传感器引脚
#define runIntervalTime  10       // 运行检测间隔时间 检测保持时长
unsigned long runTime    = 0;     // 阶段运行记录时间

bool lastTimeState = LOW;            // 上次状态值  默认为您当前初始化传感器的状态


unsigned long pressTime    = 0;      // 记录按下时间
unsigned long noPressTime  = 0;      // 记录松开时间

int pressRunCount = 0;               // 记录按下运行次数
int noPressRunCount = 0;             // 记录松开运行次数

#define PressInterval     3000       // 认定为按压保持时间
#define noPressInterval   5000       // 认定为松开保持时间
#define PressCount     10            // 认定为按压保持 次数
#define noPressCount   20            // 认定为松开保持 次数

void setup() {
  Serial.begin(9600);                // 设置波特率
  pinMode(sensorPin, INPUT);         // 设置引脚输入
  Serial.println(F("Start successfully"));          // 启动打印提示
}

void loop() {
  // 运行 间隔 检测一次
  if (millis() - runTime >= runIntervalTime) {      // 当前时间如果减去阶段记录时间大于并等于间隔时间
    runTime = millis();                             // 记录阶段时间
    int tempValue = sensorState();                  // 原始状态
    Serial.print(tempValue);                        // 打印原始状态
    Serial.print(",");
    
    if(tempValue == true){            // 按下
      
    } else {                          // 松开
      
    }

/*    
    if (sensorState() != lastTimeState) {           // 判断当前状态是否不等于上次的值
      lastTimeState = !lastTimeState;               // 状态反转 
      // Serial.print("The current status is: ");      // 输出打印
      // Serial.println(lastTimeState);                // 输出打印 状态
      // 根据状态记录时间
      if (lastTimeState == true) {                  // 判断按下
        runCount = 0;                               // 次数初始化
      } else {                                      // 判断松开
        runCount = 0;                               // 次数初始化
      }
    }
*/
    
  }

  // 次数间隔判断状态
  if (lastTimeState == true) {                            // 如果状态为高电平
    if (runCount >= PressCount) {          // 次数间隔判断
      runCount = 0;                                       // 赋值为0 降低打印速度
      Serial.print("status is: ");                        // 输出打印
      Serial.println(lastTimeState);                      // 输出打印 状态
    }
  } else {                                                // 如果状态为低电平
    if (runCount >= noPressCount) {        // 次数间隔判断
      runCount = 0;                                       // 赋值为0 降低打印速度
      Serial.print("status is: ");                        // 输出打印
      Serial.println(lastTimeState);                      // 输出打印 状态
    }
  }

  // loop回括号
}

// 获取传感器状态
bool sensorState() {
  bool _sensorFlag = digitalRead(sensorPin);
  // 原始数据打印
  //Serial.print("original status is: ");                        // 输出打印
  //Serial.println(_sensorFlag);                                 // 输出打印 状态
  return _sensorFlag;
}
