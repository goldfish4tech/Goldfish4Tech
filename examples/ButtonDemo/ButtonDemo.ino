/*
  程序目的：
    检测按钮是否按下，如果按下串口打印按钮编号 +  is pressed。
    一、硬件材料
    1*Arduino uno开发板
    7*按钮/按键
  二、电路接线
    Arduino UNO   的   GND   接   所有按钮  的   引脚1
    Arduino UNO   的    2    接   1号按钮   的   引脚2  
    Arduino UNO   的    3    接   2号按钮   的   引脚2
    Arduino UNO   的    4    接   3号按钮   的   引脚2
    Arduino UNO   的    5    接   4号按钮   的   引脚2
    Arduino UNO   的    6    接   5号按钮   的   引脚2
    Arduino UNO   的    7    接   6号按钮   的   引脚2
    Arduino UNO   的    8    接   7号按钮   的   引脚2
    详见接线图
  三、程序上传
    第1步：打开Arduino软件，依次点击 -> 工具 -> 开发板选择"Arduino Mega"
    第2步：依次点击 -> 工具 -> 端口选择你自己电脑Arduino的COM口，一般是com3开始，如COM3，COM7
    第3步：点击上传（Arduino软件左上角对勾旁边的图标）
  四、程序原理
    采用非阻塞式实现消抖
  五、预期效果
    1、如果按下按钮，串口打印按钮编号 +  is pressed。
    2、支持多个按钮同时按下
  六、可能的问题
    1、接线不对
    仔细核对接线图接线
  七、参考的资料
    Arduino的例程
    File → Examples → 02.Digital → Debounce
    或
    文件 → 示例 → 02.Digital → Debounce
    Arduino教程
    https://space.bilibili.com/103589285/video?tid=0&page=4&keyword=&order=pubdate
    https://www.geek-workshop.com/thread-2719-1-1.html
    https://www.yiboard.com/forum-70-1.html
  八、特别注意
    1、Arduino UNO的D13不支持INPUT_PULLUP。
    2、因为采用INPUT_PULLUP，所以默认是高电平，按下是低电平。如果电路是下拉输入，需要改动相对应的代码。
*/
#define startButton   2                             //0号按钮
#define buttonPin1    3                             //1号按钮
#define buttonPin2    4                             //2号按钮
#define buttonPin3    5                             //3号按钮
#define buttonPin4    6                             //4号按钮
#define buttonPin5    7                             //5号按钮
#define buttonPin6    8                             //6号按钮
                                                    //把所有按钮放入一个数组以便于操作
const int buttonPins[7] = {startButton, buttonPin1, buttonPin2, buttonPin3, buttonPin4, buttonPin5, buttonPin6};
#define buttonON      LOW                           //按钮按下时为低电平
struct Button{
  int buttonState = !buttonON;                      //按钮状态变量，与按钮按下时的状态取反
  int lastButtonState = !buttonON;                  //按钮状态初始化，与按钮按下时的状态取反
  long lastDebounceTime = 0;                        //记录抖动变量
  long debounceDelay = 30;                          //抖动时间变量
  bool flag = false;                                //按钮flag
};
Button button, buttons[7];                          //新建1个按钮，和按钮数组，数组含有七个按钮

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);                               //初始化串口，波特率为9600
  initButtons();                                    //初始化所有按键串口
}

// the loop function runs over and over again forever
void loop() {
  getButton(startButton);                           //适用于单个按钮
  for(int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++){
    getButton(buttonPins[i],i);                     //适用于多个按钮
  }
}
/*********************************************************
 * 函数功能：按钮初始化函数，设置为上拉输入
 * 入口参数：无
 * 返回  值：无
*********************************************************/
void initButtons(){
  pinMode(startButton, INPUT_PULLUP);               //适用于单个按钮
  for(int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++){
    pinMode(buttonPins[i], INPUT_PULLUP);           //适用于多个按钮
  }
}

/*********************************************************
 * 函数功能：按钮输入读取，采用非阻塞式按键消抖，适用于多个按钮
 * 入口参数：int按钮引脚，int按钮数组编号，此时可选0 1 2
 * 返回  值：无
*********************************************************/
void getButton(int _buttonPin,int _buttonIndex){
  int reading = digitalRead(_buttonPin);                //读取状态
  if(reading != buttons[_buttonIndex].lastButtonState){ //如果状态改变
    buttons[_buttonIndex].lastDebounceTime = millis();  //更新时间
  }                                                     //如果等待大于debounceDelay
  if((millis() - buttons[_buttonIndex].lastDebounceTime) > buttons[_buttonIndex].debounceDelay){
    if(reading != buttons[_buttonIndex].buttonState){   //读取状态不等于按钮状态
      buttons[_buttonIndex].buttonState = reading;      //更新状态
      if(buttons[_buttonIndex].buttonState == buttonON){//判断按钮是否真的按下
        Serial.print(_buttonIndex);                     //打印按钮编号
        Serial.println(" is pressed");                  //输出按钮按下的文字
        buttons[_buttonIndex].flag = true;              //按钮flag为真
      }
      else{
        buttons[_buttonIndex].flag = false;             //按钮flag为假
      }
    }
  }
  buttons[_buttonIndex].lastButtonState = reading;      //更新last状态
}

/*********************************************************
 * 函数功能：按钮输入读取，采用非阻塞式按键消抖，适用于单个按钮
 * 入口参数：int按钮引脚
 * 返回  值：无
*********************************************************/
void getButton(int _buttonPin){
  int reading = digitalRead(_buttonPin);              //读取状态
  if(reading != button.lastButtonState){              //如果状态改变
    button.lastDebounceTime = millis();               //更新时间
  }                                                   //如果等待大于debounceDelay
  if((millis() - button.lastDebounceTime) > button.debounceDelay){
    if(reading != button.buttonState){                //读取状态不等于按钮状态
      button.buttonState = reading;                   //更新状态
      if(button.buttonState == buttonON){             //判断按钮是否真的按下
        Serial.println("button is pressed");          //输出按钮按下的文字
        button.flag = true;                           //按钮flag为真
      }
      else{
        button.flag = false;                          //按钮flag为假
      }
    }
  }
  button.lastButtonState = reading;                   //更新last状态
}
