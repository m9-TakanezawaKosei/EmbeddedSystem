/**
組み込みシステム最終課題
4軸ロボットアーム Processing
**/

import processing.serial.*;
Serial port;

//各クラスの宣言
DegreeBar[] degreeBars = new DegreeBar[5];
Button button;

//Arduinoからデータを受け取ったかどうか
boolean dataReceived = false;

void setup()
{
  port = new Serial(this, "COM7", 9600);

  surface.setResizable( true );
  PSurface psf = getSurface();
  psf.setResizable( true );
  size(1000, 1000);
  background(0);

  //角度バーのインスタンス
  for (int i = 0; i < 4; i++)
  {
    String jointName = "JOINT ";
    jointName += str(i);
    degreeBars[i] = new DegreeBar(width / 2, 180 + (i * 180), 500, 270, -135, jointName);
    degreeBars[i].setValue(0);
  }
  degreeBars[4] = new DegreeBar(width / 2, 900, 500, 90, 0, "HAND");
  //ボタンのインスタンス
  button = new Button(1300, 500, 100, "Arduino <- Processing", "Arduino -> Processing");
}

void draw()
{
  //画面の初期化
  background(0);
  
  //バーとボタンの表示
  for (int i = 0; i < 5; i++)
    degreeBars[i].display();
  button.display();
  
  //Arduinoからデータを受け取っていなかった場合ここで関数を終了
  if (dataReceived == false)
    return;
    
  sendData(button, degreeBars);
}

//シリアル通信でデータを受け取った時に実行
void serialEvent(Serial port)
{
  if (button.getON_OFF())
    return;
  //ダミーデータ255 1つ
  //角度データ((+か-か)+(角度の絶対値))×5つ=10つ
  //計11データ
  if (port.available() >= 11)
  {
    dataReceived = true;
    if (port.read() == 255)
      receiveDegreeData(port, degreeBars);
  }
}
