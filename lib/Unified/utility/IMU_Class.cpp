#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "../M5Unified.hpp"
#include "IMU_Class.hpp"

#include "m5unified_common.h"

#if defined(ESP_PLATFORM)

#include <sdkconfig.h>
#include <nvs.h>

#include "imu/MPU6886_Class.hpp"
#include "imu/SH200Q_Class.hpp"
#include "imu/BMI270_Class.hpp"
#include "imu/BMM150_Class.hpp"
#include "imu/AK8963_Class.hpp"

#endif

static constexpr char LIBRARY_NAME[] = "M5Unified";

namespace m5
{
  static constexpr const char* nvs_key_names[9] = { "ax", "ay", "az", "gx", "gy", "gz", "mx", "my", "mz" };

  bool IMU_Class::begin(I2C_Class* i2c, m5::board_t board)
  {
#if defined(M5UNIFIED_PC_BUILD)
    (void)i2c;
    (void)board;
#else
    if (i2c)
    {
      i2c->begin();
    }

    _imu = imu_t::imu_none;
    _has_sensor_mask = sensor_mask_none;

    {
      auto mpu6886 = new MPU6886_Class();
      auto res = mpu6886->begin(i2c);
      if (!res) { delete mpu6886; }
      else
      {
        _imu_instance[0].reset(mpu6886);
        switch (mpu6886->whoAmI())
        {
        case MPU6886_Class::DEV_ID_MPU6050:
          _imu = imu_t::imu_mpu6050;
          break;
        case MPU6886_Class::DEV_ID_MPU6886:
          _imu = imu_t::imu_mpu6886;
          break;
        case MPU6886_Class::DEV_ID_MPU9250:
          _imu = imu_t::imu_mpu9250;
          break;
        default:
          _imu = imu_t::imu_unknown;
          break;
        }

        if (board == m5::board_t::board_M5Atom)
        { // ATOM Matrix's IMU is oriented differently, so change the setting.
          _internal_axisorder_fixed[sensor_index_accel] = (internal_axisorder_t)(axis_invert_x | axis_invert_z); // X軸,Z軸反転
          _internal_axisorder_fixed[sensor_index_gyro ] = (internal_axisorder_t)(axis_invert_x | axis_invert_z); // X軸,Z軸反転
        }
      }
    }

    if (_imu == imu_t::imu_none)
    {
      auto bmi2 = new BMI270_Class();
      if (!bmi2->begin(i2c)) { delete bmi2; }
      else
      {
        _imu_instance[0].reset(bmi2);
        _imu = imu_t::imu_bmi270;
        // CoreS3 BMI270 + BMM150 構成では、地磁気のY軸Z軸をそれぞれ反転する
        _internal_axisorder_fixed[sensor_index_mag] = (internal_axisorder_t)(axis_invert_y | axis_invert_z); // Y軸,Z軸反転
      }
    }

    if (_imu == imu_t::imu_none)
    {
      auto sh200q = new SH200Q_Class();
      if (!sh200q->begin(i2c)) { delete sh200q; }
      else
      {
        _imu_instance[0].reset(sh200q);
        _imu = imu_t::imu_sh200q;
      }
    }

    {
      auto bmm150 = new BMM150_Class();
      if (!bmm150->begin(i2c)) {
        delete bmm150;
      }
      else
      {
        _imu_instance[1].reset(bmm150);
        if (board == m5::board_t::board_M5Stack)
        { // M5Stack MPU6886 + BMM150構成では、地磁気のX軸とZ軸をそれぞれ反転する
          // M5Stack SH200Q + BMM150構成での動作は未確認。(過去に一時期製造されている)
          _internal_axisorder_fixed[sensor_index_mag] = (internal_axisorder_t)(axis_invert_x | axis_invert_z); // X軸,Z軸反転
        }
      }
    }

    {
      auto ak8963 = new AK8963_Class();
      if (!ak8963->begin(i2c)) {
        delete ak8963;
      }
      else
      {
        _imu_instance[1].reset(ak8963);
        if (_imu == imu_t::imu_mpu9250)
        { // MPU9250内蔵AK8963は地磁気のX軸とY軸を取り換え、Z軸の向きを反転する
          _internal_axisorder_fixed[sensor_index_mag ] = (internal_axisorder_t)(axis_order_yxz | axis_invert_z); // Y軸X軸を入替, Z軸反転
        }
      }
    }
#endif
    if (_imu == imu_t::imu_none)
    {
      return false;
    }

    memset(&_offset_data, 0, sizeof(imu_offset_data_t));
    _update_convert_param();
    _update_axis_order();

    if (!loadOffsetFromNVS())
    {
      setCalibration(255, 255, 255);
      update();
      for (int i = 0; i < 3; ++i)
      {
        for (int j = 0; j < 3; ++j)
        {
          int32_t d = _raw_data.sensor[i].value[j] << 16;
          _offset_data.sensor[i].prev_value[j] = d;
          _offset_data.sensor[i].avg_value[j] = d;
        }
        _offset_data.sensor[i].stillness = 255;
        _offset_data.sensor[i].calibration();
      }
    }
    setCalibration(0, 0, 0);

// debug
// for(int i=0;i<3;++i){for(int j=0;j<3;++j){_offset_data.sensor[i].value[j] = 65536*512;}}
// for(int i=0;i<3;++i){for(int j=0;j<3;++j){_offset_data.sensor[i].value[j] = 0;}}

    return true;
  }

  bool IMU_Class::sleep(void)
  {
    bool res = false;
    for (size_t i = 0; i < 2; ++i)
    {
      if (_imu_instance[i].get())
      {
        res = _imu_instance[i]->sleep() || res;
      }
    }
    return res;
  }

  void IMU_Class::setClock(std::uint32_t freq)
  {
    for (size_t i = 0; i < 2; ++i)
    {
      if (_imu_instance[i].get())
      {
        _imu_instance[i]->setClock(freq);
      }
    }
  }

  void IMU_Class::_update_convert_param(void)
  {
    if (_imu_instance[0]) { _imu_instance[0]->getConvertParam(&_convert_param); }
    if (_imu_instance[1]) { _imu_instance[1]->getConvertParam(&_convert_param); }

    // 加速度は 1.0G ± 0.000488f の範囲に収まるよう調整
    _offset_data.accel.radius = 1.0f / _convert_param.accel_res;
    _offset_data.accel.tolerance = (1.0f / 2048.0f) / _convert_param.accel_res;
    _offset_data.accel.noise_level = 0.0625f / _convert_param.accel_res;
    _offset_data.accel.average_shifter = 1;

    // ジャイロは 誤差 ±2.0度/sec の範囲に収まるよう調整
    _offset_data.gyro.radius = 0;
    _offset_data.gyro.tolerance = 0.0f / _convert_param.gyro_res;
    _offset_data.gyro.noise_level = 2.0f / _convert_param.gyro_res;
    _offset_data.gyro.average_shifter = 6;

    // 地磁気は…パラメータ模索中…
    _offset_data.mag.radius = 384.0f / _convert_param.mag_res;
    _offset_data.mag.tolerance = 64.0f / _convert_param.mag_res;
    _offset_data.mag.noise_level = 96.0f / _convert_param.mag_res;
    _offset_data.mag.average_shifter = 1;
  }

  void IMU_Class::setCalibration(uint8_t accel, uint8_t gyro, uint8_t mag)
  {
    _offset_data.accel.strength = accel;
    _offset_data.gyro.strength = gyro;
    _offset_data.mag.strength = mag;
    _calibration_flg = (sensor_mask_t)((accel ? sensor_mask_accel : 0)
                                     | (gyro  ? sensor_mask_gyro  : 0)
                                     | (mag   ? sensor_mask_mag   : 0));
  }

  bool IMU_Class::setAxisOrder(axis_t axis0, axis_t axis1, axis_t axis2)
  { // データの取り出し順序を指定する
    uint_fast8_t result;
    switch ((axis0 >> 1) + ((axis1 >> 1) << 2) + ((axis2 >> 1) << 4))
    {
    case 0 << 0 | 1 << 2 | 2 << 4: result = axis_order_xyz; break;
    case 0 << 0 | 2 << 2 | 1 << 4: result = axis_order_xzy; break;
    case 1 << 0 | 0 << 2 | 2 << 4: result = axis_order_yxz; break;
    case 1 << 0 | 2 << 2 | 0 << 4: result = axis_order_yzx; break;
    case 2 << 0 | 0 << 2 | 1 << 4: result = axis_order_zxy; break;
    case 2 << 0 | 1 << 2 | 0 << 4: result = axis_order_zyx; break;
    default: return false;
    }
    result += (axis0 & 1) + ((axis1 & 1) << 1) + ((axis2 & 1) << 2);
    _internal_axisorder_user = (internal_axisorder_t)result;
    _update_axis_order();
    return true;
  }

  void IMU_Class::_update_axis_order(void)
  { // システムの軸設定とユーザの軸設定を反映したデータの取り出し順序を求める
    static constexpr const uint8_t internal_axisorder_table[6] = 
    {//X idx |  Y idx |  Z idx
      0 << 0 | 1 << 2 | 2 << 4,  // axis_order_xyz
      0 << 0 | 2 << 2 | 1 << 4,  // axis_order_xzy
      1 << 0 | 0 << 2 | 2 << 4,  // axis_order_yxz
      1 << 0 | 2 << 2 | 0 << 4,  // axis_order_yzx
      2 << 0 | 0 << 2 | 1 << 4,  // axis_order_zxy
      2 << 0 | 1 << 2 | 0 << 4,  // axis_order_zyx
    };

    std::uint32_t result = 0;
    std::uint32_t bitshift = 0;
    auto axis_order_user = _internal_axisorder_user;
    auto order_user_tbl = internal_axisorder_table[axis_order_user >> axis_order_shift];
    for (std::size_t sensor = 0; sensor < 3; ++sensor)
    {
      auto axis_order_fixed = _internal_axisorder_fixed[sensor];
      auto order_fixed_tbl = internal_axisorder_table[axis_order_fixed >> axis_order_shift];
      for (std::size_t j = 0; j < 3; ++j, bitshift += 3)
      { // ユーザが要求している軸順と反転指定を求める
        std::uint_fast8_t axis_user_index = (order_user_tbl >> (j << 1)) & 3;
        bool invert_user = axis_order_user & (1 << j);

        // システムで設定された軸順テーブルを、ユーザーが要求している軸順で取得する
        std::uint_fast8_t axis_fixed_index = (order_fixed_tbl >> (axis_user_index << 1)) & 3;
        bool invert_fixed = axis_order_fixed & (1 << axis_user_index);

        // 3ビットでx=0b000,y=b010,z=0b100 | 反転フラグ0b001 を表現する。
        // これを3軸×3センサの9個分を並べて保持する。合計3bit×9の27ビットが使用される。
        result |= ((axis_fixed_index << 1) + (invert_user != invert_fixed ? 1 : 0)) << bitshift;
// printf("user_index:%d invert_user:%d fixed_index:%d invert_fixed:%d res:%08x\n", axis_user_index, invert_user, axis_fixed_index, invert_fixed, result);
      }
    }
    _axis_order_3bit_x9 = result;
  }

  // 指定された2軸を元に、残りの1軸を求める (右手系)
  static IMU_Class::axis_t getAxis2(IMU_Class::axis_t axis0, IMU_Class::axis_t axis1)
  {
    std::uint_fast8_t axis2 = (1 << (axis0 >> 1) | 1 << (axis1 >> 1)) ^ 0b111;
    axis2 &= 0b110;
    std::uint_fast8_t ax0 = axis2 ? (axis2 - 2) : 4;
    axis2 += (bool)((axis0 & 0b110) == ax0) == (bool)((axis0 & 1) == (axis1 & 1));
    return (IMU_Class::axis_t)axis2;
  }

  bool IMU_Class::setAxisOrderRightHanded(axis_t axis0, axis_t axis1)
  {
    return setAxisOrder(axis0, axis1, getAxis2(axis0, axis1));
  }

  bool IMU_Class::setAxisOrderLeftHanded(axis_t axis0, axis_t axis1)
  {
    // 右手系で求めた軸に反転フラグを設定し、左手系とする。
    auto axis2 = getAxis2(axis0, axis1) ^ 1;
    return setAxisOrder(axis0, axis1, (axis_t)axis2);
  }

  bool IMU_Class::saveOffsetToNVS(void)
  { // NVSへオフセット値を保存する
#if !defined(M5UNIFIED_PC_BUILD)
    std::uint32_t nvs_handle = 0;
    if (ESP_OK != nvs_open(LIBRARY_NAME, NVS_READWRITE, &nvs_handle))
    {
      return false;
    }
    size_t index = 0;
    for (size_t i = 0; i < 3; ++i)
    {
      for (size_t j = 0; j < 3; ++j, ++index)
      {
        int32_t val = _offset_data.sensor[i].value[j];
        nvs_set_i32(nvs_handle, nvs_key_names[index], val);
        M5_LOGV("%s:%d", nvs_key_names[index], val);
      }
    }
    nvs_close(nvs_handle);
#endif
    return true;
  }

  bool IMU_Class::loadOffsetFromNVS(void)
  { // NVSからオフセット値を読み込む
#if !defined(M5UNIFIED_PC_BUILD)
    std::uint32_t nvs_handle = 0;
    if (ESP_OK != nvs_open(LIBRARY_NAME, NVS_READONLY, &nvs_handle))
    {
      return false;
    }
    size_t index = 0;
    for (size_t i = 0; i < 3; ++i)
    {
      for (size_t j = 0; j < 3; ++j, ++index)
      {
        int32_t val = 0;
        nvs_get_i32(nvs_handle, nvs_key_names[index], &val);
        _offset_data.sensor[i].value[j] = val;
        M5_LOGD("%s:%d", nvs_key_names[index], val);
      }
    }
    nvs_close(nvs_handle);
#endif
    return true;
  }

  void IMU_Class::clearOffsetData(void)
  {
    size_t index = 0;
    for (size_t i = 0; i < 3; ++i)
    {
      for (size_t j = 0; j < 3; ++j, ++index)
      {
        _offset_data.sensor[i].value[j] = 0;
      }
    }
  }

  void IMU_Class::setOffsetData(size_t index, int32_t value)
  {
    if (index < 9) {
      _offset_data.sensor[index / 3].value[index % 3] = value;
    }
  }

  int32_t IMU_Class::getOffsetData(size_t index)
  {
    return index < 9 ? _offset_data.sensor[index / 3].value[index % 3] : 0;
  }

  int16_t IMU_Class::getRawData(size_t index)
  {
    return index < 9 ? _raw_data.value[index] : 0;
  }

  IMU_Class::sensor_mask_t IMU_Class::update(void)
  {
    sensor_mask_t res = (sensor_mask_t)0;
    for (size_t i = 0; i < 2; ++i)
    {
      if (_imu_instance[i].get())
      {
        uint_fast8_t t = _imu_instance[i]->getImuRawData(&_raw_data);
        if (t)
        {
          res = (sensor_mask_t)(res | t);
        }
        if (i == 0)
        {
          _latest_micros = m5gfx::micros();
        }
      }
    }
    if (res)
    {
      std::uint_fast8_t mask_flg = _calibration_flg & res;
      // キャリブレーション処理
      for (size_t i = 0; mask_flg && i < 3; ++i, mask_flg >>= 1)
      {
        // if ((mask_flg & 1) && (16 < _offset_data.sensor[i].updateStillness(_raw_data.sensor[i])))
        if (mask_flg & 1)
        {
          auto st = _offset_data.sensor[i].updateStillness(_raw_data.sensor[i]);
          if (16 < st)
          {
            _offset_data.sensor[i].calibration();
// if (i == 1) { printf("Ok  %d\n" ,st); }
          }
//           else
//           {
// if (i == 1) { printf(" NG %d\n", st); }
//           }
        }
      }
    }
    return res;
  }

  void IMU_Class::getImuData(imu_data_t* data)
  {
    data->usec = _latest_micros;
    auto &raw = _raw_data;
    // data->temp = raw.temp * _convert_param.temp_res + _convert_param.temp_offset;
    auto &offset = _offset_data;
    auto order = _axis_order_3bit_x9;
    for (int i = 0; i < 3; ++i)
    {
      float resolution = _convert_param.value[i] * (1.0f / 65536.0f);
      for (int j = 0; j < 3; ++j)
      {
        auto axis_index = (order >> 1) & 3;
        int32_t value = (raw.sensor[i].value[axis_index] << 16) - offset.sensor[i].value[axis_index];
        if (order & 1) { value = -value; }
        data->sensor[i].value[j] = resolution * value;
        order >>= 3;
      }
    }
  }

  bool IMU_Class::getAccel(float *x, float *y, float *z)
  {
    bool res = true;
    uint32_t us = m5gfx::micros();
    if (us - _latest_micros > 256)
    {
      res = update();
    }
    imu_data_t data;
    getImuData(&data);
    *x = data.accel.x;
    *y = data.accel.y;
    *z = data.accel.z;
    return res;
  }

  bool IMU_Class::getGyro(float *x, float *y, float *z)
  {
    bool res = true;
    uint32_t us = m5gfx::micros();
    if (us - _latest_micros > 256)
    {
      res = update();
    }
    imu_data_t data;
    getImuData(&data);
    *x = data.gyro.x;
    *y = data.gyro.y;
    *z = data.gyro.z;
    return res;
  }

  bool IMU_Class::getMag(float* x, float* y, float* z)
  {
    bool res = true;
    uint32_t us = m5gfx::micros();
    if (us - _latest_micros > 256)
    {
      res = update();
    }
    imu_data_t data;
    getImuData(&data);
    *x = data.mag.x;
    *y = data.mag.y;
    *z = data.mag.z;
    return res;
  }

  bool IMU_Class::getTemp(float *t)
  {
    int16_t temp;
    if (_imu_instance[0].get() && _imu_instance[0]->getTempAdc(&temp))
    {
      _raw_data.temp = temp;
      *t = temp * _convert_param.temp_res + _convert_param.temp_offset;
      return true;
    }
    return false;
  }

  bool IMU_Class::setINTPinActiveLogic(bool level)
  {
    return _imu_instance[0].get() && _imu_instance[0]->setINTPinActiveLogic(level);
  }


  std::uint_fast8_t IMU_Class::offset_point_t::updateStillness(const IMU_Base::point3d_i16_t& dst)
  { // 前回の座標情報との差が許容範囲以内か調べる
    int32_t res = stillness + 1;

    int32_t maxdiff = 0;
    for (int i = 0; i < 3; ++i)
    {
      int32_t d = dst.value[i] << 16;
      int32_t pv = prev_value[i];
      int32_t diff = d - pv;
      prev_value[i] = d;
      maxdiff = std::max(maxdiff, abs(diff));

      int32_t av = avg_value[i];
      diff = d - av;
      maxdiff = std::max(maxdiff, abs(diff));

      diff = (diff + (1 << (average_shifter - 1))) >> average_shifter;
      avg_value[i] = av + diff;
      // maxdiff = std::max(maxdiff, abs(diff) << (average_shifter));
    }

    int32_t rt = noise_level << 8;
    maxdiff >>= 8;
    if (rt > maxdiff)
    {
      maxdiff = rt - (maxdiff + 1);
      maxdiff = (maxdiff << 8) / rt;
    }
    else
    {
      maxdiff = 0;
    }
    res = std::min(res, maxdiff);

    stillness = res;
    return res;
  }

  void IMU_Class::offset_point_t::calibration(void)
  {
    if (stillness * strength == 0) return;
    float distance = 0;
    float diffs[3];
    for (int i = 0; i < 3; ++i)
    {
      // int p = ((average_shifter) ? avg_value : prev_value)[i];  
      auto p = prev_value[i];  
      // auto p = avg_value[i];  
      // auto p = (prev_value[i] + avg_value[i]) >> 1;
      float f = p - value[i];
      diffs[i] = f;
      distance += f * f;
    }
    distance = sqrtf(distance * (1.0f / (65536.0f * 65536.0f)));
    if (distance < 1.0f) { return; }

    // 誤差を求める (目的の半径と現在の距離の差)
    float measure_error = distance - radius;
    // if (signbit(measure_error)) { return; }
    // float tole_half = tolerance;
    float force = fabsf(measure_error);
    // if (force <= 0.0f) { return; }
    if (force <= tolerance) { return; }
    // if (force > tolerance) { force = tolerance; }
    if (measure_error < 0.0f) { force = -force; }
    float fk = force * (stillness * strength) / (distance * (255.0f * 255.0f));
    for (int i = 0; i < 3; ++i)
    {
      value[i] += roundf(diffs[i] * fk);
    }
    return;
  }
}
#endif