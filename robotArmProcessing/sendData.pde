void sendData(Button button, DegreeBar[] bars)
{
  //ダミーデータの送信
  port.write(255);
  
  //Arduino -> Processing なら1，Arduino -> Processing なら0を送信
  if (button.getON_OFF() == false)
    port.write(0);
  else
    port.write(1);
    
  int LEDNum = 10;
  for (int i = 0; i < 5; i++)
  {
    if (bars[i].getIsTouched())
      LEDNum = i;
  }
  port.write(LEDNum);
  
  sendDegreeData(bars);
}



void sendDegreeData(DegreeBar[] bars)
{
  //送信可能なデータは0-255の範囲なので
  //正か負かを表すデータと値の大きさに分けて送信

  for (int i = 0; i < 5; i++)
  {
    int barsDeg = bars[i].getIntValue();
    //値が負なら10，正なら20を送信
    if (barsDeg < 0)
      port.write(10);
    else
      port.write(20);
    //値の大きさを送信
    port.write(abs(barsDeg));
  }
}
