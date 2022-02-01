class Button
{
  //円がにマウスが触れていればtrue
  boolean isTouched = false;
  //円がマウスでクリックされていればtrue
  boolean isPressed = false;
  //Arduino <- Processingでtrue
  boolean ON_OFF = false;
  
  float x;
  float y;
  int size;
  String titleName1;
  String titleName2;
  
  Button(float tempX, float tempY, int tempSize, String tempTitleName1, String tempTitleName2)
  {
    x     = tempX;
    y     = tempY;
    size  = tempSize;
    titleName1 = tempTitleName1;
    titleName2 = tempTitleName2;
  }
  
  void display()
  {
    pushMatrix();
    translate(x, y);

    fill(#FFFFFF);
    textSize(80);
    textAlign(CENTER);
    if (ON_OFF)
      text(titleName1, 0, -size);
    else
      text(titleName2, 0, -size);
    
    if(x - size < mouseX && mouseX < x + size && y - size < mouseY && mouseY < y + size)
    {
      if (isPressed && mousePressed == false)
      {
        isPressed = false;
        if (ON_OFF) 
          ON_OFF = false;
        else
          ON_OFF = true;
      }
      if(isPressed == false)
        fill(#C8C8C8);
      if (mousePressed == true)
        isPressed = true;
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
      translate(x, y);
      fill(#FFFF00);
      scale(1.1);
    }else
    {
      translate(x, y);
    }
    circle(0, 0, size);
    popMatrix();
  }
  //直接アクセスした場合に誤って上書きされる等のバグ防止
  boolean getON_OFF() { return ON_OFF; }
}