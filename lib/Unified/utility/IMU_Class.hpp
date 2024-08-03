#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_IMU_CLASS_H__
#define __M5_IMU_CLASS_H__

#include "I2C_Class.hpp"
#include "imu/IMU_Base.hpp"
#include <memory>

namespace m5
{
  enum imu_t
  { imu_none,
    imu_unknown,
    imu_sh200q,
    imu_mpu6050,
    imu_mpu6886,
    imu_mpu9250,
    imu_bmi270,
  };

  class IMU_Class
  {
  public:

    struct imu_3d_t
    {
      union
      {
        float value[3];
        struct
        {
          float x;
          float y;
          float z;
        };
      };
    };

    struct imu_data_t
    {
      uint32_t usec;
      union
      {
        float value[9];
        imu_3d_t sensor[3];
        struct
        {
          imu_3d_t accel;
          imu_3d_t gyro;
          imu_3d_t mag;
        };
      };
    };

    enum axis_t
    {
      axis_x_pos = 0,
      axis_x_neg = 1,
      axis_y_pos = 2,
      axis_y_neg = 3,
      axis_z_pos = 4,
      axis_z_neg = 5,
    };

    enum sensor_index_t
    {
      sensor_index_accel = 0,
      sensor_index_gyro  = 1,
      sensor_index_mag   = 2,
    };

    enum sensor_mask_t
    {
      sensor_mask_none = 0,
      sensor_mask_accel = 1 << sensor_index_accel,
      sensor_mask_gyro  = 1 << sensor_index_gyro,
      sensor_mask_mag   = 1 << sensor_index_mag,
    };

    bool begin(I2C_Class* i2c = nullptr, board_t board = board_t::board_unknown);
    bool init(I2C_Class* i2c = nullptr) { return begin(i2c); }
    bool sleep(void);

    void setClock(std::uint32_t freq);

    sensor_mask_t update(void);

    void getImuData(imu_data_t* imu_data);

    const imu_data_t& getImuData(void) { getImuData(&_last_data); return _last_data; }

    // 軸の順序を指定する。デフォルトはX+,Y+,Z+
    bool setAxisOrder(axis_t axis0, axis_t axis1, axis_t axis2);

    // 軸の順序を右手系で指定する。最初の２軸のみ指定し、３軸目は省略
    bool setAxisOrderRightHanded(axis_t axis0, axis_t axis1);

    // 軸の順序を左手系で指定する。最初の２軸のみ指定し、３軸目は省略
    bool setAxisOrderLeftHanded(axis_t axis0, axis_t axis1);

    bool getAccel(float* ax, float* ay, float* az);
    bool getGyro(float* gx, float* gy, float* gz);
    bool getMag(float* mx, float* my, float* mz);
    bool getAccelData(float* ax, float* ay, float* az) { return getAccel(ax, ay, az); }
    bool getGyroData(float* gx, float* gy, float* gz) { return getGyro(gx, gy, gz); }
    bool getGyroMag(float* mx, float* my, float* mz) { return getMag(mx, my, mz); }
    bool getTemp(float *t);

    bool isEnabled(void) const { return _imu != imu_none; }

    imu_t getType(void) const { return _imu; }

    // 実装予定
    // void getAhrsData(float *pitch, float *roll, float *yaw);

    // 廃止
    // void setRotation(uint_fast8_t rotation) { _rotation = rotation & 3; };

    bool setINTPinActiveLogic(bool level);

    // 各センサの自動オフセット調整機能の強さを指定する。 0=自動調整なし 1~255=自動調整あり
    void setCalibration(uint8_t accel_strength, uint8_t gyro_strength, uint8_t mag_strength);

    // 現在のオフセット調整値をNVSに保存する
    bool saveOffsetToNVS(void);

    // NVSからオフセット調整値を読み込む
    bool loadOffsetFromNVS(void);

    // オフセットデータをクリアする
    void clearOffsetData(void);

    // OffsetData は RawData<<16 スケール(16bit固定小数扱い)
    void setOffsetData(size_t index, int32_t value);

    int32_t getOffsetData(size_t index);

    int16_t getRawData(size_t index);

    IMU_Base* getImuInstancePtr(int idx) const { return _imu_instance[idx].get(); }

  private:

    struct offset_point_t
    {
      union
      {
        int32_t value[3];
        struct
        {
          int32_t x;
          int32_t y;
          int32_t z;
        };
      };
      union
      {
        int32_t prev_value[3];
        struct
        {
          int32_t prev_x;
          int32_t prev_y;
          int32_t prev_z;
        };
      };
      union
      {
        int32_t avg_value[3];
        struct
        {
          int32_t avg_x;
          int32_t avg_y;
          int32_t avg_z;
        };
      };
      float radius;
      float tolerance;
      uint16_t noise_level;
      uint8_t average_shifter;   // 前回値への移動平均設定
      uint8_t stillness; // 移動量の少なさ(変動が大きい時0になり、静止時に255に近付く)
      uint8_t strength;  // キャリブレーションの強さ(ユーザー指定)

      std::uint_fast8_t updateStillness(const IMU_Base::point3d_i16_t& dst);
      void calibration(void);
      inline void setValue16(size_t index, int16_t val) { value[index] = val << 16; }
      inline int32_t getValue16(size_t index) const { return value[index] >> 16; }
    } __attribute__((__packed__));

    struct imu_offset_data_t
    {
      union
      {
        offset_point_t sensor[3];
        struct
        {
          offset_point_t accel;
          offset_point_t gyro;
          offset_point_t mag;
        };
      };
    };

    void _update_convert_param(void);
    void _update_axis_order(void);
    void _proc_calibration(void);

    // update成功時のマイクロ秒情報
    uint32_t _latest_micros;

    // センサのインスタンス保持用 ([0]=加速度+ジャイロ / [1]=地磁気)
    std::unique_ptr<IMU_Base> _imu_instance[2];

    // 生の値を測定値に変換するための補正値
    IMU_Base::imu_convert_param_t _convert_param;

    // updateで得た最新の生値
    IMU_Base::imu_raw_data_t _raw_data;

    // オフセット補正用の値
    imu_offset_data_t _offset_data;

    // 最後に得たセンサ値
    imu_data_t _last_data;

    // 最後に動きがあった時のusec
    uint32_t _moving_micros;

    // ユーザ側で任意の軸割当を行うための設定項目
    uint8_t _assign_axis_x;
    uint8_t _assign_axis_y;
    uint8_t _assign_axis_z;

    sensor_mask_t _calibration_flg;

    sensor_mask_t _has_sensor_mask;

    imu_t _imu = imu_t::imu_none;

    // 機種別設定とユーザー設定の両方を反映した後のデータ取得順の値
    uint32_t _axis_order_3bit_x9;

    enum internal_axisorder_t : uint8_t
    {
      axis_invert_x  =  1,
      axis_invert_y  =  2,
      axis_invert_z  =  4,
      axis_order_shift = 3,
      axis_order_xyz =  0 << axis_order_shift,
      axis_order_xzy =  1 << axis_order_shift,
      axis_order_yxz =  2 << axis_order_shift,
      axis_order_yzx =  3 << axis_order_shift,
      axis_order_zxy =  4 << axis_order_shift,
      axis_order_zyx =  5 << axis_order_shift,
    };

    // 機種別の軸の不一致を統一するための設定項目(システム自動設定)
    internal_axisorder_t _internal_axisorder_fixed[3] = { (internal_axisorder_t)0, (internal_axisorder_t)0, (internal_axisorder_t)0 };

    // ユーザーによる設定値
    internal_axisorder_t _internal_axisorder_user = (internal_axisorder_t)0;
  };

  typedef IMU_Class::imu_3d_t imu_3d_t;
  typedef IMU_Class::imu_data_t imu_data_t;
}
#endif
#endif