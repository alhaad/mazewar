#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

// Data structures which are used by the multi-player version of the mazewar
// game as described in prototcol.pdf.

typedef struct {
  uint8_t version;
  uint8_t descriptor_type;
  uint16_t payload_length;
  uint32_t player_id;
  uint32_t sequence_number;
} Header;

typedef struct {
  uint32_t rat_dir:4;
  uint32_t projectile_dir:4;
  uint32_t collision_token:24;
  uint16_t rat_x_pos;
  uint16_t rat_y_pos;
  uint32_t projectile_seq;
  uint16_t projectile_x_pos;
  uint16_t projectile_y_pos;
  uint32_t score;
  char player_name[32];
} StateBody;

typedef struct {
  uint32_t shooter_id;
  uint32_t projectile_seq;
  uint16_t rat_x_pos;
  uint16_t rat_y_pos;
  uint8_t rat_dir:4;
} TagRequestBody;

typedef struct {
  uint32_t projectile_seq;
  uint32_t player_id;
} TagResponseBody;

typedef struct {
  Header header;
  StateBody body;
} StatePacket;

typedef struct {
  Header header;
  TagRequestBody body;
} TagRequestPacket;

typedef struct {
  Header header;
  TagResponseBody body;
} TagResponsePacket;
#endif
