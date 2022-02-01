void receiveDegreeData(Serial port, DegreeBar[] bars)
{
  int readData;
  int barsDeg;
  for (int i = 0; i < 5; i++)
  {
    readData = port.read();
    barsDeg = port.read();
    if (readData == 10)
      barsDeg *= -1;
    bars[i].setValue(barsDeg);
  }
}
