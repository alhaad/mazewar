#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "packet.h"

Header htonHeader(Header input) {
  Header output;
  output.version = input.version;
  output.descriptor_type = input.descriptor_type;
  output.payload_length = htons(input.payload_length);
  output.player_id = htonl(input.player_id);
  output.sequence_number = htonl(input.sequence_number);
  return output;
}

Header ntohHeader(Header input) {
  Header output;
  output.version = input.version;
  output.descriptor_type = input.descriptor_type;
  output.payload_length = ntohs(input.payload_length);
  output.player_id = ntohl(input.player_id);
  output.sequence_number = ntohl(input.sequence_number);
  return output;
}

void printHeader(Header header) {
  printf("**************HEADER*************\n");
  printf("Player id: %d\n", header.player_id);
  printf("Seq Number: %d\n", header.sequence_number);
}

uint32_t getBitMask(int start_pos, int end_pos) {
  uint32_t mask = 0;
  for (int i = start_pos; i <= end_pos; i++) {
    mask |= 1 << i;
  }
  return mask;
}

uint8_t getRatDir(StateBody body) {
  return (body.complex_field & getBitMask(28, 31)) >> 28;
}

void setRatDir(StateBody* body, uint8_t rat_dir) {
  body->complex_field &= ~getBitMask(28, 31);
  body->complex_field |= (rat_dir & getBitMask(0, 3)) << 28;
}

uint8_t getProjectileDir(StateBody body) {
  return (body.complex_field & getBitMask(24, 27)) >> 24;
}

void setProjectileDir(StateBody* body, uint8_t projectile_dir) {
  body->complex_field &= ~getBitMask(24, 27);
  body->complex_field |= (projectile_dir & getBitMask(0, 3)) << 24;
}

uint32_t getCollisionToken(StateBody body) {
  return (body.complex_field & getBitMask(0, 23));
}

void setCollisionToken(StateBody* body, uint32_t collision_token) {
  body->complex_field &= ~getBitMask(0, 23);
  body->complex_field |= (collision_token & getBitMask(0, 23));
}

uint8_t getRatDir(TagRequestBody body) {
  return (body.rat_dir & getBitMask(4, 7)) >> 4;
}

void setRatDir(TagRequestBody* body, uint8_t rat_dir) {
  body->rat_dir &= ~getBitMask(4, 7);
  body->rat_dir |= (rat_dir & getBitMask(0, 3)) << 4;
}

StateBody htonStateBody(StateBody input) {
  StateBody output;
  output.complex_field = htonl(input.complex_field);
  output.rat_x_pos = htons(input.rat_x_pos);
  output.rat_y_pos = htons(input.rat_y_pos);
  output.projectile_seq = htonl(input.projectile_seq);
  output.projectile_x_pos = htons(input.projectile_x_pos);
  output.projectile_y_pos = htons(input.projectile_y_pos);
  output.score = htonl(input.score);
  strcpy(output.player_name, input.player_name);
  return output;
}

StateBody ntohStateBody(StateBody input) {
  StateBody output;
  output.complex_field = ntohl(input.complex_field);
  output.rat_x_pos = ntohs(input.rat_x_pos);
  output.rat_y_pos = ntohs(input.rat_y_pos);
  output.projectile_seq = ntohl(input.projectile_seq);
  output.projectile_x_pos = ntohs(input.projectile_x_pos);
  output.projectile_y_pos = ntohs(input.projectile_y_pos);
  output.score = ntohl(input.score); 
  strcpy(output.player_name, input.player_name);
  return output;
}

void printStateBody(StateBody body) {
  printf("*************STATE BODY**********\n");
  printf("X POS: %d\n", body.rat_x_pos);
  printf("y POS: %d\n", body.rat_y_pos);
  printf("DIR: %d\n", getRatDir(body));
  printf("Score: %d\n", body.score);
  printf("Name: %s\n", body.player_name);
}

TagRequestBody htonTagRequestBody(TagRequestBody input) {
  TagRequestBody output;
  output.shooter_id = htonl(input.shooter_id);
  output.projectile_seq = htonl(input.projectile_seq);
  output.rat_x_pos = htonl(input.rat_x_pos);
  output.rat_dir = input.rat_dir;
  return output;
}

TagRequestBody ntohTagRequestBody(TagRequestBody input) {
  TagRequestBody output;
  output.shooter_id = ntohl(input.shooter_id);
  output.projectile_seq = ntohl(input.projectile_seq);
  output.rat_x_pos = ntohl(input.rat_x_pos);
  output.rat_dir = input.rat_dir;
  return output;
}

void printTagRequestBody(TagRequestBody body) {
  printf("***************TAG REQUEST***********\n");
  printf("Shooter id: %d\n", body.shooter_id);
  printf("Rat dir: %d\n", getRatDir(body));
}

TagResponseBody htonTagResponseBody(TagResponseBody input) {
  TagResponseBody output;
  output.projectile_seq = htonl(input.projectile_seq);
  output.player_id = htonl(input.player_id);
  return output;
}

TagResponseBody ntohTagResponseBody(TagResponseBody input) {
  TagResponseBody output;
  output.projectile_seq = ntohl(input.projectile_seq);
  output.player_id = ntohl(input.player_id);
  return output;
}

StatePacket htonStatePacket(StatePacket input) {
  StatePacket output;
  output.header = htonHeader(input.header);
  output.body = htonStateBody(input.body);
  return output;
}

StatePacket ntohStatePacket(StatePacket input) {
  StatePacket output;
  output.header = ntohHeader(input.header);
  output.body = ntohStateBody(input.body);
  return output;
}

void printStatePacket(StatePacket input) {
  printHeader(input.header);
  printStateBody(input.body);
}

TagRequestPacket htonTagRequestPacket(TagRequestPacket input) {
  TagRequestPacket output;
  output.header = htonHeader(input.header);
  output.body = htonTagRequestBody(input.body);
  return output;
}

TagRequestPacket ntohTagRequestPacket(TagRequestPacket input) {
  TagRequestPacket output;
  output.header = ntohHeader(input.header);
  output.body = ntohTagRequestBody(input.body);
  return output;
}

void printTagRequestPacket(TagRequestPacket packet) {
  printHeader(packet.header);
  printTagRequestBody(packet.body);
}

TagResponsePacket htonTagResponsePacket(TagResponsePacket input) {
  TagResponsePacket output;
  output.header = htonHeader(input.header);
  output.body = htonTagResponseBody(input.body);
  return output;
}

TagResponsePacket ntohTagResponsePacket(TagResponsePacket input) {
  TagResponsePacket output;
  output.header = ntohHeader(input.header);
  output.body = ntohTagResponseBody(input.body);
  return output;
}
