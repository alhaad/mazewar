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

Header htonHeader(Header);
Header ntohHeader(Header);
void printHeader(Header);

typedef struct {
  /* |complex_field| contains bits from 'rat_dir', 'projectile_dir' and 
  'collision_token'. Do not use this field directly but rather use the helper
  methods provided. */
  uint32_t complex_field;
  uint16_t rat_x_pos;
  uint16_t rat_y_pos;
  uint32_t projectile_seq;
  uint16_t projectile_x_pos;
  uint16_t projectile_y_pos;
  uint32_t score;
  char player_name[32];
} StateBody;

// Only the 4 LSB bits will be used for 'rat_dir'.
uint8_t getRatDir(StateBody);
void setRatDir(StateBody*, uint8_t);

// Only the 4 LSB bits will be used for 'projectile_dir'.
uint8_t getProjectileDir(StateBody);
void setProjectileDir(StateBody*, uint8_t);

// Only the 24 LSB bits will be used for 'collision_token'.
uint32_t getCollisionToken(StateBody);
void setCollisionToken(StateBody*, uint32_t);

StateBody htonStateBody(StateBody);
StateBody ntohStateBody(StateBody);
void printStateBody(StateBody);

typedef struct {
  uint32_t shooter_id;
  uint32_t projectile_seq;
  uint16_t rat_x_pos;
  uint16_t rat_y_pos;
  /* The 4 MSB are to be used. So use the helper function instead of this
  field directly. */
  uint8_t rat_dir;
  char unused[3];
} TagRequestBody;

// Only the 4 LSB bits will be used for 'dir_dir'.
uint8_t getRatDir(TagRequestBody);
void setRatDir(TagRequestBody*, uint8_t);

TagRequestBody htonTagRequestBody(TagRequestBody);
TagRequestBody ntohTagRequestBody(TagRequestBody);
void printTagRequestBody(TagRequestBody);

typedef struct {
  uint32_t projectile_seq;
  uint32_t player_id;
} TagResponseBody;

TagResponseBody htonTagResponseBody(TagResponseBody);
TagResponseBody ntohTagResponseBody(TagResponseBody);
void printTagResponseBody(TagResponseBody);

typedef struct {
  Header header;
  StateBody body;
} StatePacket;

StatePacket htonStatePacket(StatePacket);
StatePacket ntohStatePacket(StatePacket);
void printStatePacket(StatePacket);

typedef struct {
  Header header;
  TagRequestBody body;
} TagRequestPacket;

TagRequestPacket htonTagRequestPacket(TagRequestPacket);
TagRequestPacket ntohTagRequestPacket(TagRequestPacket);
void printTagRequestPacket(TagRequestPacket);

typedef struct {
  Header header;
  TagResponseBody body;
} TagResponsePacket;

TagResponsePacket htonTagResponsePacket(TagResponsePacket);
TagResponsePacket ntohTagResponsePacket(TagResponsePacket);
void printTagResponsePacket(TagResponsePacket);

#endif
