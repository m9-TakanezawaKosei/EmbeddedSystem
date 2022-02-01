/**
組み込みシステム最終課題
4軸ロボットアーム Arduino IDE
**/

#include <Servo.h>
#include "kinematics.h"
#include "sendData.h"

//各ピンの宣言
const int JOINT_PINS[5] = {5, 6, 9, 10, 11};
const int JOYSTICK_ANALOG_PINS1[3] = {A0, A1};
const int JOYSTICK_ANALOG_PINS2[3] = {A2, A3};
const int JOYSTICK_SW_PIN1 = 2;
const int JOYSTICK_SW_PIN2 = 3;
const int LED_PIN[5] = {4, 7, 8, 12, 13};

//Servo宣言
Servo armJoints[5];

//armLengths[5] = {l_0, l_1, l_2, l_3, l_4} ロボットアームの各寸法[mm]
//armDegrees[5] = {theta_0, theta_1, theta_2, theta_3, handdeg} ロボットアームの各角度[deg]
const float armLengths[5] = {39.0f, 48.0f, 81.0f, 98.0f, 35.0f};
int armDegrees[5] = {0, 45, 45, 0, 0};

//Processingからの角度データを読み込むかどうか
bool shouldReadDegreeData = false;

//ジョイスティックのアナログ入力とデジタル入力
long joystick1AnalogValue[2];
long joystick2AnalogValue[2];
int joystick1DigitalValue;
int joystick2DigitalValue;

//角度が適切であるかどうか
bool isProperDegree = true;

//手先座標
float xyz[3];

void setup()
{
  //シリアル通信開始
  Serial.begin(9600);
  //ピン設定
  pinMode(JOYSTICK_SW_PIN1, INPUT_PULLUP);
  pinMode(JOYSTICK_SW_PIN2, INPUT_PULLUP);
  for (int i = 0; i < 5; i++)
  {
    pinMode(LED_PIN[i], OUTPUT);
    armJoints[i].attach(JOINT_PINS[i]);
    if (i == 4)
      armJoints[i].write(armDegrees[i]);
    else
      armJoints[i].write(map(armDegrees[i],-135,135,0,180));
  }
  //順運動学計算を行いxyzを初期化
  solveForwardkinematics(xyz, armLengths, armDegrees);
}

void loop()
{
  //ダミーデータ255 1つ
  //shouldReadDegreeData true false 1つ
  //LED点滅データ（どのLEDを点滅させるか）1つ
  //角度データ((+か-か)+(角度の絶対値))×5つ=10つ
  //計13データ
  if (Serial.available() >= 13)
  {
    //255を受け取ったらデータ読み込み開始，違うデータならreturn
    if (Serial.read() == 255)
    {
      int readData;
      readData = Serial.read();
      if (readData == 0)
        shouldReadDegreeData = false;
      else
        shouldReadDegreeData = true;
        
      readData = Serial.read();
      for (int i = 0; i < 5; i++)
      {
        if (i == readData)
          digitalWrite(LED_PIN[i], HIGH);
        else
          digitalWrite(LED_PIN[i], LOW);
      }

      //shouldReadDegreeData = trueなら送られてきた角度データを読み込む
      if (shouldReadDegreeData)
      {
        for (int i = 0; i < 5; i++)
        {
          readData = Serial.read();
          armDegrees[i] = Serial.read();
          if (readData == 10)
            armDegrees[i] *= -1;
          if (i == 4)
            armJoints[i].write(armDegrees[i]);
          else
            armJoints[i].write(map(armDegrees[i],-135,135,0,180));
        }
      }
    }
    else
      return;
  }
  //shouldReadDegreeData = trueなら Arduino側からの制御は行わない
  if (shouldReadDegreeData)
  {
    sendData(armDegrees);
    return;
  }

  //現時点での（前回の）xyzと角度データを保存
  float oldXYZ[3];
  int oldDegrees[3];
  for (int i = 0; i < 3; i++)
    oldXYZ[i] = xyz[i];
  for (int i = 0; i < 3; i++)
    oldDegrees[i] = armDegrees[i];

  //xyzと角度（JOINT3）の変化量
  float deltaXYZ[3] = {0.0f, 0.0f, 0.0f};
  long deltaDegree3 = 0;

  //ジョイスティックのアナログ入力を読み込む
  for (int i = 0; i < 2; i++)
    joystick1AnalogValue[i] = analogRead(JOYSTICK_ANALOG_PINS1[(i + 1) % 2]);
  for (int i = 0; i < 2; i++)
    joystick2AnalogValue[i] = analogRead(JOYSTICK_ANALOG_PINS2[i]);

  //ジョイスティックのデジタル入力を読み込む
  joystick1DigitalValue = digitalRead(JOYSTICK_SW_PIN1);
  joystick2DigitalValue = digitalRead(JOYSTICK_SW_PIN2);

  //XYZ座標の変化量（-5~5）を計算
  for (int i = 0; i < 2; i++)
  {
    deltaXYZ[i] = (float)joystick1AnalogValue[i] * 10.0f / 1023.0f;
    deltaXYZ[i] -= 5.0f;
  }
  deltaXYZ[2] = (float)joystick2AnalogValue[1] * 10.0f / 1023.0f;
  deltaXYZ[2] -= 5.0f;

  //deltaXYZの大きさが一定以上ならXYZ座標を変化させる
  if (abs(deltaXYZ[0]) + abs(deltaXYZ[1]) + abs(deltaXYZ[2]) >= 1)
  {
    for (int i = 0; i < 3; i++)
    {
      if (abs(deltaXYZ[i]) >= 2)
        xyz[i] -= deltaXYZ[i];
    }
    //xyzが更新されたので逆運動学計算でJOINT0-2の角度を算出
    //計算結果の角度が適切ならisProperDegree = true;
    isProperDegree = solveInverseKinematics(xyz, armLengths, armDegrees);
  }

  //JOINT0-2モータの可動範囲外の角度や角度変化が大きい場合は isProperDegree = false
    for (int i = 0; i < 3; i++)
  {
    if (armDegrees[i] < -135 || 135 < armDegrees[i])
      isProperDegree = false;
    if (abs(armDegrees[i] - oldDegrees[i]) > 20)
      isProperDegree = false;
  }

  //手先の回転角（JOINT3）の変化量（-5~5 deg）を計算
  deltaDegree3 = map(joystick2AnalogValue[0], 0, 1023, -5, 5);

  //deltaDegree3の大きさが一定以上ならarmDegrees[3]を変化させる
  if (abs(deltaDegree3) > 2)
    armDegrees[3] += deltaDegree3;

  //JOINT3は独立に決まるので可動範囲制限のみ
  if (armDegrees[3] < -135)
    armDegrees[3] = -135;
  if (135 < armDegrees[3])
    armDegrees[3] = 135;

  //ジョイスティックのデジタル入力から手先の開閉角度を変更
  if (joystick1DigitalValue == LOW)
    armDegrees[4] -= 10;
  if (joystick2DigitalValue == LOW)
    armDegrees[4] += 10;

  //armDegrees[4]（HAND）も独立に決まるので可動範囲制限（0-90degに制限）のみ
  if (armDegrees[4] < 0)
    armDegrees[4] = 0;
  if (90 < armDegrees[4])
    armDegrees[4] = 90;
  
  //逆運動学で求めた角度が適切ならJOINT0-2のモータに角度指令を送る
  if (isProperDegree)
  {
    //JOINT0-2モータに角度指令を送る
    for (int i = 0; i < 3; i++)
      armJoints[i].write(map(armDegrees[i],-135,135,0,180));
  }
  else
  {
    //更新前のXYZデータ，角度データ（JOINT0-2）で上書き
    for (int i = 0; i < 3; i++)
      xyz[i] = oldXYZ[i];
    for (int i = 0; i < 3; i++)
      armDegrees[i] = oldDegrees[i];
  }
  //JOINT3，HANDモータに角度指令を送る
  armJoints[3].write(map(armDegrees[3],-135,135,0,180));
  armJoints[4].write(armDegrees[4]);

  //PCにデータを送信
  sendData(armDegrees);
}
