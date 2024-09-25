#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "IMU_Base.hpp"

namespace m5
{
  IMU_Base::~IMU_Base() {}
  IMU_Base::IMU_Base(std::uint8_t i2c_addr, std::uint32_t freq, I2C_Class* i2c)
  : I2C_Device ( i2c_addr, freq, i2c )
  {}
/*
  int32_t IMU_Base::point3d_i32_t::calibration(const point3d_i16_t& dst)
  {
    { // 前回の座標情報との差が許容範囲よりも大きく移動しているか調べる
      int32_t moving_distance = 0;
      for (int i = 0; i < 3; ++i)
      {
        int32_t diff = dst.value[i] - prev_point.value[i];
        moving_distance += diff * diff;
      }
      prev_point = dst;
      uint32_t wait = wait_counter;
      if (moving_distance > (tolerance * tolerance))
      { // 移動変化量が大きい場合は補正を暫く停止する
        wait = 16;
      }
      if (wait)
      { // 座標移動が安定するまで待機
        wait_counter = wait - 1;
        return 0;
      }
    }

    float distance = 0;
    float diffs[3];
    for (int i = 0; i < 3; ++i)
    {
      float diff = (dst.value[i] << 16) - value[i];
      diffs[i] = diff;
      distance += diff * diff;
    }
    distance = sqrtf(distance * (1.0f / (65536.0f * 65536.0f)));

    // 誤差を求める (目的の半径と現在の距離の差)
    float measure_error = distance - radius;

    int32_t tol = tolerance;
    int32_t force = abs((int32_t)measure_error) - tol;
    if (force <= 0) { return 1; }

    if (force > tol) { force = tol; }
    if (signbit(measure_error)) { force = -force; }

    float fk = (force * strength) / (distance * 256.0f);
    for (int i = 0; i < 3; ++i)
    {
      value[i] += diffs[i] * fk;
    }
    return 0;
  }
//*/
}
#endif