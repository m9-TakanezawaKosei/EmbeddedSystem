void sendData(int degreeData[])
{
  Serial.write(255);
  sendDegreeData(degreeData);
}

void sendDegreeData(int degreeData[])
{
  //データは0-255の範囲なので
  //正か負かを表すデータと値の大きさに分けて送信
  for (int i = 0; i < 5; i++)
  {
    //値が負なら10，正なら20を送信
    if (degreeData[i] < 0)
      Serial.write(10);
    else
      Serial.write(20);
    //値の大きさを送信
    Serial.write(abs(degreeData[i]));
  }
}
