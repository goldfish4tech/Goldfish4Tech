#define buttonPin 2  // the number of the pushbutton pin

#define Press LOW
#define noPress !Press
#define PressInterval     4000
#define noPressInterval   2000



bool pressFlag = false;
unsigned long pressTime = 0;

bool noPressFlag = false;
unsigned long noPressTime = 0;

bool currentState = false;
bool lastState = false;
bool realState = false;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  currentState = digitalRead(buttonPin);
  if (currentState != lastState) {        //当前按键状态不等于上一次记录的状态
    lastState = currentState;             //重置上一次记录的状态
    if (currentState == Press) {          //当按键按下
      pressFlag = true;                   //按键按下标志位置1
      noPressFlag = false;                //按键抬起标志位置0
      pressTime = millis();               //记录按键按下时间
    }
    else {                                //当按键抬起，记录按键抬起状态，与按键按下状态相反
      pressFlag = false;
      noPressFlag = true;
      noPressTime = millis();
    }
  }
  if (currentState == Press) {                    //如果按键按下
    if (millis() - pressTime > PressInterval) {   //按下持续一段时间
      realState = true;                           //realState置1
    }
  }
  else {
    if (millis() - noPressTime > noPressInterval) {//当按键持续松开一段时间后
      realState = false;                           //realState置0
    }
  }
  Serial.print(currentState + 5);
  Serial.print(",");
  Serial.println(realState);
  delay(50);
}
