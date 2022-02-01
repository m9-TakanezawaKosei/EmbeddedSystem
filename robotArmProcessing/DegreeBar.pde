class DegreeBar
{
  //円がにマウスが触れていればtrue
  boolean isTouched = false;
  //円がマウスでクリックされていればtrue
  boolean isPressed = false;
  
  float circleX;//バーに描画する円の中心のx座標
  float x;
  float y;
  float value = 0;//パラメータの値
  float minValue;
  float range;
  int size;//バーの長さ
  String titleName;
  
  DegreeBar(float tempX, float tempY, int tempSize, float tempRange, float tempMinValue, String tempTitleName)
  {
    x     = tempX;
    y     = tempY;
    size  = tempSize;
    range = tempRange;
    minValue = tempMinValue;
    titleName = tempTitleName;
    //円の初期位置はバーの中心
    circleX = tempX;
  }
  void display()
  {
    pushMatrix();
    translate(x, y);
    
    //テキスト（titleName、value）の表示
    fill(#FFFFFF);
    textSize(48);
    textAlign(CENTER);
    text(titleName + "\n" + round(value) + " deg", 0, -100);
    
    //バーと円の描画
    stroke(255);
    line(-size / 2, 0 , size / 2, 0);
    if(circleX - 50 < mouseX && mouseX < circleX + 50 && y - 50 < mouseY && mouseY < y + 50)
    {
      //マウスポイントが円に触れていたら色を変える
      if(isPressed == false)
      {
        fill(#C8C8C8);
        isTouched = true;
      }
      
      if (mousePressed == true)
        isPressed = true;
      else
        isPressed = false;
      
    }
    else
    {
      fill(#FFFFFF);
      isPressed = false;
      isTouched = false;
    }
    popMatrix();
    pushMatrix();
    noStroke();
    //ボタンが押されていたらボタンを拡大して色を変える
    if(isPressed == true)
    {
      //円をドラッグで移動できるようにcircleXをmouseXに更新
      circleX = mouseX;
      if(circleX - x > size / 2)
        circleX = x + size / 2;
      if(circleX - x < -size / 2) 
        circleX = x - size / 2;
      translate(circleX, y);
      fill(#FFFF00);
      scale(1.1);
    }else
    {
      translate(circleX, y);
    }
    circle(0, 0, 30);
    popMatrix();
    
    //円のバー上での位置に応じてvalueを minValue ～ (minValue+range) の値に変更
    value = round(((circleX - x + (size / 2)) / size) * range);
    value += minValue;
  }
  void setValue(float newValue)
  {
    circleX = x + size * (newValue - minValue) / range - size / 2;
  }

  //直接アクセスした場合に誤って上書きされる等のバグ防止
  float getValue(){ return value; }
  int getIntValue(){ return (int)value; }
  boolean getIsPressed() { return isPressed; }
  boolean getIsTouched() { return isTouched; }
}