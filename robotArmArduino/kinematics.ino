//順運動学計算
//返り値 -> なし
void solveForwardkinematics(float *xyz, const float lengths[], int nowDegrees[])
{
  float cos0   = cos(radians(nowDegrees[0]));
  float cos1   = cos(radians(nowDegrees[1]));
  float cos12  = cos(radians(nowDegrees[1] + nowDegrees[2]));
  float sin0   = sin(radians(nowDegrees[0]));
  float sin1   = sin(radians(nowDegrees[1]));
  float sin12  = sin(radians(nowDegrees[1] + nowDegrees[2]));
  
  float XorY = sin1 * lengths[2] + sin12 * (lengths[3] + lengths[4]);
  
  xyz[0] = cos0 * XorY;
  xyz[1] = sin0 * XorY;
  xyz[2] = lengths[0]
         + lengths[1]
         + cos1 * lengths[2]
         + cos12 * (lengths[3] + lengths[4]);
}

//順運動学計算
//返り値 -> 適切な角度かどうか
bool solveInverseKinematics(float xyz[], const float lengths[], int *newDegrees)
{
  newDegrees[0] = degrees(atan2(xyz[1], xyz[0]));

  float cos0   = cos(radians(newDegrees[0]));
  float sin0   = sin(radians(newDegrees[0]));
  
  float sqrtXY = sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1]);
  float Z = xyz[2] - lengths[0] - lengths[1];
  
  float cos2   = (xyz[0] * xyz[0]
                + xyz[1] * xyz[1]
                + Z * Z
                - lengths[2] * lengths[2]
                - (lengths[3] + lengths[4]) * (lengths[3] + lengths[4]))
               / (2.0f * lengths[2] * (lengths[3] + lengths[4]));
  float sin2   = sqrt(1.0f - cos2 * cos2);

  float M = lengths[2] + cos2 * (lengths[3] + lengths[4]);
  float N = sin2 * (lengths[3] + lengths[4]);
  
  newDegrees[1] = degrees(atan2(M * sqrtXY - N * Z, N * sqrtXY + M * Z));
  newDegrees[2] = degrees(atan2(sin2, cos2));
  
  float cos1   = cos(radians(newDegrees[1]));
  float sin1   = sin(radians(newDegrees[1]));
  float cos12 = cos1 * cos2 - sin1 * sin2;
  float sin12 = sin1 * cos2 + cos1 * sin2;

  //ヤコビ行列
  float jacobian3DOF[3][3];
  jacobian3DOF[0][0] = -sin0 * (sin1 * lengths[2] + sin12 * (lengths[3] + lengths[4]));
  jacobian3DOF[0][1] = cos0 * (cos1 * lengths[2] + cos12 * (lengths[3] + lengths[4]));
  jacobian3DOF[0];[2] = cos0 * cos12 * (lengths[3] + lengths[4]);
  jacobian3DOF[1][0] = cos0 * (sin1 * lengths[2] + sin12 * (lengths[3] + lengths[4]));
  jacobian3DOF[1][1] = sin0 * (cos1 * lengths[2] + cos12 * (lengths[3] + lengths[4]));
  jacobian3DOF[1][2] = sin0 * cos12 * (lengths[3] + lengths[4]);
  jacobian3DOF[2][0] = 0;
  jacobian3DOF[2][1] = -sin1 * lengths[2] - sin12 * (lengths[3] + lengths[4]);
  jacobian3DOF[2][2] = -sin12 * (lengths[3] + lengths[4]);

  //可操作度
  float manipulatability = 0;
  for (int i = 0; i < 3; i++)
    manipulatability += jacobian3DOF[0][(0 + i) % 3] * jacobian3DOF[1][(1 + i) % 3] * jacobian3DOF[2][(2 + i) % 3];
  for (int i = 0; i < 3; i++)
    manipulatability -= jacobian3DOF[0][(0 + i) % 3] * jacobian3DOF[1][(2 + i) % 3] * jacobian3DOF[2][(1 + i) % 3];
  manipulatability = abs(manipulatability);

  if (isfinite(manipulatability) == false || manipulatability < 100)
    return false;
  else
    return true;
}
