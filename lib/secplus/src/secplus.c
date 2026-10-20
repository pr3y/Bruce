/*
 * Copyright 2022 Clayton Smith (argilo@gmail.com)
 *
 * This file is part of secplus.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "secplus.h"

int8_t encode_v1(const uint32_t rolling, uint32_t fixed, uint8_t *symbols1,
                 uint8_t *symbols2) {
  uint32_t rolling_reversed = 0;
  int8_t i, half;
  uint8_t acc;
  uint8_t *symbols;

  if (fixed >= 3486784401u) {
    return -1;
  }

  for (i = 1; i < 32; i++) {
    rolling_reversed |= ((rolling >> i) & 1) << (32 - i - 1);
  }

  for (half = 1; half >= 0; half--) {
    symbols = (half == 0 ? symbols1 : symbols2);

    for (i = 18; i >= 0; i -= 2) {
      symbols[i] = rolling_reversed % 3;
      rolling_reversed /= 3;
      symbols[i + 1] = fixed % 3;
      fixed /= 3;
    }

    acc = 0;
    for (i = 0; i < 20; i += 2) {
      acc += symbols[i];
      acc += symbols[i + 1];
      symbols[i + 1] = acc % 3;
    }
  }

  return 0;
}

int8_t decode_v1(const uint8_t *symbols1, const uint8_t *symbols2,
                 uint32_t *rolling, uint32_t *fixed) {
  uint32_t rolling_reversed = 0;
  uint8_t acc;
  uint8_t digit;
  int8_t i, half;
  const uint8_t *symbols;

  *rolling = 0;
  *fixed = 0;

  for (half = 0; half < 2; half++) {
    symbols = (half == 0 ? symbols1 : symbols2);
    acc = 0;
    for (i = 0; i < 20; i += 2) {
      digit = symbols[i];
      rolling_reversed = (rolling_reversed * 3) + digit;
      acc += digit;

      digit = (60 + symbols[i + 1] - acc) % 3;
      *fixed = (*fixed * 3) + digit;
      acc += digit;
    }
  }

  for (i = 0; i < 32; i++) {
    *rolling |= ((rolling_reversed >> i) & 1) << (32 - i - 1);
  }

  return 0;
}

static void v2_calc_parity(const uint64_t fixed, uint32_t *data) {
  uint32_t parity = (fixed >> 32) & 0xf;
  int8_t offset;

  *data &= 0xffff0fff;
  for (offset = 0; offset < 32; offset += 4) {
    parity ^= ((*data >> offset) & 0xf);
  }
  *data |= (parity << 12);
}

static int8_t v2_check_parity(const uint64_t fixed, const uint32_t data) {
  uint32_t parity = (fixed >> 32) & 0xf;
  int8_t offset;

  for (offset = 0; offset < 32; offset += 4) {
    parity ^= ((data >> offset) & 0xf);
  }

  if (parity != 0) {
    return -1;
  }

  return 0;
}

static void encode_v2_rolling(const uint32_t rolling,
                              uint32_t *rolling_halves) {
  uint32_t rolling_reversed = 0;
  int8_t i, half;

  for (i = 0; i < 28; i++) {
    rolling_reversed |= ((rolling >> i) & 1) << (28 - i - 1);
  }

  rolling_halves[0] = 0;
  rolling_halves[1] = 0;

  for (half = 0; half < 2; half++) {
    for (i = 0; i < 8; i += 2) {
      rolling_halves[half] |= rolling_reversed % 3 << i;
      rolling_reversed /= 3;
    }
  }

  for (half = 0; half < 2; half++) {
    for (i = 10; i < 18; i += 2) {
      rolling_halves[half] |= rolling_reversed % 3 << i;
      rolling_reversed /= 3;
    }
  }

  rolling_halves[0] |= (rolling_reversed % 3) << 8;
  rolling_reversed /= 3;

  rolling_halves[1] |= (rolling_reversed % 3) << 8;
}

static int8_t decode_v2_rolling(const uint32_t *rolling_halves,
                                uint32_t *rolling) {
  int8_t i, half;
  uint32_t rolling_reversed;

  rolling_reversed = (rolling_halves[1] >> 8) & 3;
  rolling_reversed = (rolling_reversed * 3) + ((rolling_halves[0] >> 8) & 3);

  for (half = 1; half >= 0; half--) {
    for (i = 16; i >= 10; i -= 2) {
      rolling_reversed =
          (rolling_reversed * 3) + ((rolling_halves[half] >> i) & 3);
    }
  }

  for (half = 1; half >= 0; half--) {
    for (i = 6; i >= 0; i -= 2) {
      rolling_reversed =
          (rolling_reversed * 3) + ((rolling_halves[half] >> i) & 3);
    }
  }

  if (rolling_reversed >= 0x10000000) {
    return -1;
  }

  *rolling = 0;
  for (i = 0; i < 28; i++) {
    *rolling |= ((rolling_reversed >> i) & 1) << (28 - i - 1);
  }

  return 0;
}

static int8_t v2_combine_halves(const uint8_t frame_type,
                                const uint32_t *rolling_halves,
                                const uint32_t *fixed_halves,
                                const uint16_t *data_halves, uint32_t *rolling,
                                uint64_t *fixed, uint32_t *data) {
  int8_t err = 0;

  err = decode_v2_rolling(rolling_halves, rolling);
  if (err < 0) {
    return err;
  }

  *fixed = ((uint64_t)fixed_halves[0] << 20) | fixed_halves[1];

  if (frame_type == 1) {
    *data = ((uint32_t)data_halves[0] << 16) | data_halves[1];

    err = v2_check_parity(*fixed, *data);
    if (err < 0) {
      return err;
    }
  }

  return 0;
}

static const int8_t ORDER[16] = {9,  33, 6, -1, 24, 18, 36, -1,
                                 24, 36, 6, -1, -1, -1, -1, -1};
static const int8_t INVERT[16] = {6, 2, 1, -1, 7,  5,  3,  -1,
                                  4, 0, 5, -1, -1, -1, -1, -1};

static void v2_scramble(const uint32_t *parts, const uint8_t frame_type,
                        uint8_t *packet_half) {
  const int8_t order = ORDER[packet_half[0] >> 4];
  const int8_t invert = INVERT[packet_half[0] & 0xf];
  int8_t i;
  uint8_t out_offset = 10;
  int8_t end;
  uint32_t parts_permuted[3];

  end = (frame_type == 0 ? 5 : 8);
  for (i = 1; i < end; i++) {
    packet_half[i] = 0;
  }

  parts_permuted[0] =
      (invert & 4) ? ~parts[(order >> 4) & 3] : parts[(order >> 4) & 3];
  parts_permuted[1] =
      (invert & 2) ? ~parts[(order >> 2) & 3] : parts[(order >> 2) & 3];
  parts_permuted[2] = (invert & 1) ? ~parts[order & 3] : parts[order & 3];

  end = (frame_type == 0 ? 8 : 0);
  for (i = 18 - 1; i >= end; i--) {
    packet_half[out_offset >> 3] |= ((parts_permuted[0] >> i) & 1)
                                    << (7 - (out_offset % 8));
    out_offset++;
    packet_half[out_offset >> 3] |= ((parts_permuted[1] >> i) & 1)
                                    << (7 - (out_offset % 8));
    out_offset++;
    packet_half[out_offset >> 3] |= ((parts_permuted[2] >> i) & 1)
                                    << (7 - (out_offset % 8));
    out_offset++;
  }
}

static int8_t v2_unscramble(const uint8_t frame_type, const uint8_t indicator,
                            const uint8_t *packet_half, uint32_t *parts) {
  const int8_t order = ORDER[indicator >> 4];
  const int8_t invert = INVERT[indicator & 0xf];
  int8_t i;
  uint8_t out_offset = 10;
  const int8_t end = (frame_type == 0 ? 8 : 0);
  uint32_t parts_permuted[3] = {0, 0, 0};

  if ((order == -1) || (invert == -1)) {
    return -1;
  }

  for (i = 18 - 1; i >= end; i--) {
    parts_permuted[0] |=
        (uint32_t)((packet_half[out_offset >> 3] >> (7 - (out_offset % 8))) & 1)
        << i;
    out_offset++;
    parts_permuted[1] |=
        (uint32_t)((packet_half[out_offset >> 3] >> (7 - (out_offset % 8))) & 1)
        << i;
    out_offset++;
    parts_permuted[2] |=
        (uint32_t)((packet_half[out_offset >> 3] >> (7 - (out_offset % 8))) & 1)
        << i;
    out_offset++;
  }

  parts[(order >> 4) & 3] =
      (invert & 4) ? ~parts_permuted[0] : parts_permuted[0];
  parts[(order >> 2) & 3] =
      (invert & 2) ? ~parts_permuted[1] : parts_permuted[1];
  parts[order & 3] = (invert & 1) ? ~parts_permuted[2] : parts_permuted[2];

  return 0;
}

static void encode_v2_half_parts(const uint32_t rolling, const uint32_t fixed,
                                 const uint16_t data, const uint8_t frame_type,
                                 uint8_t *packet_half) {
  uint32_t parts[3];

  parts[0] = ((fixed >> 10) << 8) | (data >> 8);
  parts[1] = ((fixed & 0x3ff) << 8) | (data & 0xff);
  parts[2] = rolling;

  packet_half[0] = (uint8_t)rolling;

  v2_scramble(parts, frame_type, packet_half);
}

static int8_t decode_v2_half_parts(const uint8_t frame_type,
                                   const uint8_t indicator,
                                   const uint8_t *packet_half,
                                   uint32_t *rolling, uint32_t *fixed,
                                   uint16_t *data) {
  int8_t err = 0;
  int8_t i;
  uint32_t parts[3];

  err = v2_unscramble(frame_type, indicator, packet_half, parts);
  if (err < 0) {
    return err;
  }

  if ((frame_type == 1) && ((parts[2] & 0xff) != indicator)) {
    return -1;
  }

  for (i = 8; i < 18; i += 2) {
    if (((parts[2] >> i) & 3) == 3) {
      return -1;
    }
  }

  *rolling = (parts[2] & 0x3ff00) | indicator;
  *fixed = ((parts[0] & 0x3ff00) << 2) | ((parts[1] & 0x3ff00) >> 8);
  *data = ((parts[0] & 0xff) << 8) | (parts[1] & 0xff);

  return 0;
}

static int8_t v2_check_limits(const uint32_t rolling, const uint64_t fixed) {
  if ((rolling >> 28) != 0) {
    return -1;
  }

  if ((fixed >> 40) != 0) {
    return -1;
  }

  return 0;
}

static void encode_v2_half(const uint32_t rolling, const uint32_t fixed,
                           const uint16_t data, const uint8_t frame_type,
                           uint8_t *packet_half) {
  encode_v2_half_parts(rolling, fixed, data, frame_type, packet_half);

  /* shift indicator two bits to the right */
  packet_half[1] |= (packet_half[0] & 0x3) << 6;
  packet_half[0] >>= 2;

  /* set frame type */
  packet_half[0] |= (frame_type << 6);
}

int8_t encode_v2(const uint32_t rolling, const uint64_t fixed, uint32_t data,
                 const uint8_t frame_type, uint8_t *packet1, uint8_t *packet2) {
  int8_t err = 0;
  uint32_t rolling_halves[2];

  err = v2_check_limits(rolling, fixed);
  if (err < 0) {
    return err;
  }

  encode_v2_rolling(rolling, rolling_halves);
  v2_calc_parity(fixed, &data);

  encode_v2_half(rolling_halves[0], fixed >> 20, data >> 16, frame_type,
                 packet1);
  encode_v2_half(rolling_halves[1], fixed & 0xfffff, data & 0xffff, frame_type,
                 packet2);

  return 0;
}

static int8_t decode_v2_half(const uint8_t frame_type,
                             const uint8_t *packet_half, uint32_t *rolling,
                             uint32_t *fixed, uint16_t *data) {
  int8_t err = 0;
  const uint8_t indicator = (packet_half[0] << 2) | (packet_half[1] >> 6);

  if ((packet_half[0] >> 6) != frame_type) {
    return -1;
  }

  err = decode_v2_half_parts(frame_type, indicator, packet_half, rolling, fixed,
                             data);
  if (err < 0) {
    return err;
  }

  return 0;
}

int8_t decode_v2(uint8_t frame_type, const uint8_t *packet1,
                 const uint8_t *packet2, uint32_t *rolling, uint64_t *fixed,
                 uint32_t *data) {
  int8_t err = 0;
  uint32_t rolling_halves[2];
  uint32_t fixed_halves[2];
  uint16_t data_halves[2];

  err = decode_v2_half(frame_type, packet1, &rolling_halves[0],
                       &fixed_halves[0], &data_halves[0]);
  if (err < 0) {
    return err;
  }

  err = decode_v2_half(frame_type, packet2, &rolling_halves[1],
                       &fixed_halves[1], &data_halves[1]);
  if (err < 0) {
    return err;
  }

  err = v2_combine_halves(frame_type, rolling_halves, fixed_halves, data_halves,
                          rolling, fixed, data);
  if (err < 0) {
    return err;
  }

  return 0;
}

static void encode_wireline_half(const uint32_t rolling, const uint32_t fixed,
                                 const uint16_t data, uint8_t *packet_half) {
  encode_v2_half_parts(rolling, fixed, data, 1, packet_half);
}

int8_t encode_wireline(const uint32_t rolling, const uint64_t fixed,
                       uint32_t data, uint8_t *packet) {
  int8_t err = 0;
  uint32_t rolling_halves[2];

  err = v2_check_limits(rolling, fixed);
  if (err < 0) {
    return err;
  }

  encode_v2_rolling(rolling, rolling_halves);
  v2_calc_parity(fixed, &data);

  packet[0] = 0x55;
  packet[1] = 0x01;
  packet[2] = 0x00;

  encode_wireline_half(rolling_halves[0], fixed >> 20, data >> 16, &packet[3]);
  encode_wireline_half(rolling_halves[1], fixed & 0xfffff, data & 0xffff,
                       &packet[11]);

  return 0;
}

int8_t encode_wireline_command(uint32_t rolling, uint64_t device_id,
                               uint16_t command, uint32_t payload,
                               uint8_t *packet) {
  uint64_t fixed;
  uint32_t data;

  if ((device_id >> 40) != 0) {
    return -1;
  }

  if ((command >> 12) != 0) {
    return -1;
  }

  if ((payload >> 20) != 0) {
    return -1;
  }

  fixed = (device_id & 0xf0ffffffff) | ((uint64_t)(command & 0xf00) << 24);
  data = ((payload & 0xff) << 24) | ((payload & 0xff00) << 8) |
         ((payload & 0xf0000) >> 8) | (command & 0xff);

  return encode_wireline(rolling, fixed, data, packet);
}

static int8_t decode_wireline_half(const uint8_t *packet_half,
                                   uint32_t *rolling, uint32_t *fixed,
                                   uint16_t *data) {
  int8_t err = 0;
  const uint8_t indicator = packet_half[0];

  if ((packet_half[1] >> 6) != 0) {
    return -1;
  }

  err = decode_v2_half_parts(1, indicator, packet_half, rolling, fixed, data);
  if (err < 0) {
    return err;
  }

  return 0;
}

int8_t decode_wireline(const uint8_t *packet, uint32_t *rolling,
                       uint64_t *fixed, uint32_t *data) {
  int8_t err = 0;
  uint32_t rolling_halves[2];
  uint32_t fixed_halves[2];
  uint16_t data_halves[2];

  if ((packet[0] != 0x55) || (packet[1] != 0x01) || (packet[2] != 0x00)) {
    return -1;
  }

  err = decode_wireline_half(&packet[3], &rolling_halves[0], &fixed_halves[0],
                             &data_halves[0]);
  if (err < 0) {
    return err;
  }

  err = decode_wireline_half(&packet[11], &rolling_halves[1], &fixed_halves[1],
                             &data_halves[1]);
  if (err < 0) {
    return err;
  }

  err = v2_combine_halves(1, rolling_halves, fixed_halves, data_halves, rolling,
                          fixed, data);
  if (err < 0) {
    return err;
  }

  return 0;
}

int8_t decode_wireline_command(const uint8_t *packet, uint32_t *rolling,
                               uint64_t *device_id, uint16_t *command,
                               uint32_t *payload) {
  int8_t err = 0;
  uint64_t fixed;
  uint32_t data;

  err = decode_wireline(packet, rolling, &fixed, &data);
  if (err < 0) {
    return err;
  }

  *device_id = fixed & 0xf0ffffffff;
  *command = ((fixed >> 24) & 0xf00) | (data & 0xff);
  *payload = ((data << 8) & 0xf0000) | ((data >> 8) & 0xff00) | (data >> 24);

  return 0;
}
