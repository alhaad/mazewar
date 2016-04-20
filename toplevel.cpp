/*
 *   FILE: toplevel.c
 * AUTHOR: name (email)
 *   DATE: March 31 23:59:59 PST 2013
 *  DESCR:
 */

/* #define DEBUG */

#include <list>
#include <map>
#include <stdint.h>
#include <string>
#include <time.h>

#include "main.h"
#include "mazewar.h"
#include "packet.h"

using namespace std;

static bool updateView; /* true if update needed */
MazewarInstance::Ptr M;

/* Use this socket address to send packets to the multi-cast group. */
static Sockaddr groupAddr;
#define MAX_OTHER_RATS (MAX_RATS - 1)

/* Unique if of this player */
static uint32_t player_id;

/* Sequence number of the last packet sent out. */
static uint32_t sequence_number = 0;

/* Is my rat cloaked ? */
static bool isCloaked = false;

/* A pool of unused Rat indices. */
static list<RatIndexType> free_rat_index_list;

/* A list of all other rats in the game. */
static map<uint32_t, RatIndexType> player_id_to_rat_index;

int main(int argc, char *argv[]) {
  char *ratName;

  signal(SIGHUP, quit);
  signal(SIGINT, quit);
  signal(SIGTERM, quit);

  srand(time(NULL));
  player_id = rand();

  for (int i = 1; i < MAX_RATS; i++) {
    free_rat_index_list.push_back(RatIndexType(i));
  }

  if (argc >= 2) {
    ratName = strdup(argv[1]);
  } else {
    getName("Welcome to CS244B MazeWar!\n\nYour Name", &ratName);
  }

  ratName[strlen(ratName) - 1] = 0;

  M = MazewarInstance::mazewarInstanceNew(string(ratName));
  strncpy(M->myName_, ratName, NAMESIZE);
  free(ratName);

  MazeInit(argc, argv);

  NewPosition(M);

  /* So you can see what a Rat is supposed to look like, we create
  one rat in the single player mode Mazewar.
  It doesn't move, you can't shoot it, you can just walk around it */

  play();

  return 0;
}

/* ----------------------------------------------------------------------- */

void play(void) {
  MWEvent event;
  MW244BPacket incoming;

  event.eventDetail = &incoming;

  while (TRUE) {
    NextEvent(&event, M->theSocket());
    if (!M->peeking()) switch (event.eventType) {
        case EVENT_A:
          aboutFace();
          break;

        case EVENT_S:
          leftTurn();
          break;

        case EVENT_D:
          forward();
          break;

        case EVENT_F:
          rightTurn();
          break;

        case EVENT_G:
          backward();
          break;

        case EVENT_C:
          cloak();
          break;

        case EVENT_BAR:
          shoot();
          break;

        case EVENT_LEFT_D:
          peekLeft();
          break;

        case EVENT_RIGHT_D:
          peekRight();
          break;

        case EVENT_NETWORK:
          processPacket(&event);
          break;

        case EVENT_TIMEOUT:
          handleTimeout();
          break;

        case EVENT_INT:
          quit(0);
          break;
      }
    else
      switch (event.eventType) {
        case EVENT_RIGHT_U:
        case EVENT_LEFT_U:
          peekStop();
          break;

        case EVENT_NETWORK:
          processPacket(&event);
          break;

        case EVENT_TIMEOUT:
          handleTimeout();
          break;
      }

    ratStates(); /* clean house */

    manageMissiles();

    DoViewUpdate();

    /* Any info to send over network? */
  }
}

/* ----------------------------------------------------------------------- */

StatePacket getStatePacket() {
  Header header;
  header.version = 1;
  header.descriptor_type = 1;
  header.payload_length = sizeof(StateBody);
  header.player_id = player_id;
  header.sequence_number = sequence_number++;

  StateBody body;
  if (isCloaked) {
    body.rat_dir = 0;
  } else {
    switch (MY_DIR) {
      case NORTH:
        body.rat_dir = 1;
        break;
      case SOUTH:
        body.rat_dir = 1 << 1;
        break;
      case EAST:
        body.rat_dir = 1 << 2;
        break;
      case WEST:
        body.rat_dir = 1 << 3;
        break;
    }
  }
  // TODO(alhaad): Add projectile.
  body.projectile_dir = 0;
  srand(time(NULL));
  body.collision_token = rand();
  body.rat_x_pos = MY_X_LOC;
  body.rat_y_pos = MY_Y_LOC;
  body.projectile_seq = ~0;
  body.projectile_x_pos = ~0;
  body.projectile_y_pos = ~0;
  body.score = M->score().value();
  strncpy(body.player_name, M->myName_, NAMESIZE);

  StatePacket packet;
  packet.header = header;
  packet.body = body;
  return packet;
}
/* ----------------------------------------------------------------------- */

void printHeader(Header header) {
  printf("Player Id: %d", header.player_id);
}

/* ----------------------------------------------------------------------- */

void printStatePacket(StatePacket packet) {
  printHeader(packet.header);
  printf("Player name: %s", packet.body.player_name);
}

/* ----------------------------------------------------------------------- */

void handleTimeout() {
  maybeSendHeartBeatPacket();
}

/* ----------------------------------------------------------------------- */

void maybeSendHeartBeatPacket() {
  // TODO(alhaad): Determine when not to send heartbeat packet.
  StatePacket packet = getStatePacket();
  sendto((int)M->theSocket(), &packet, sizeof(StatePacket), 0, (struct sockaddr*) &groupAddr, sizeof(Sockaddr));
}

/* ----------------------------------------------------------------------- */

static Direction _aboutFace[NDIRECTION] = {SOUTH, NORTH, WEST, EAST};
static Direction _leftTurn[NDIRECTION] = {WEST, EAST, NORTH, SOUTH};
static Direction _rightTurn[NDIRECTION] = {EAST, WEST, SOUTH, NORTH};

void aboutFace(void) {
  M->dirIs(_aboutFace[MY_DIR]);
  updateView = TRUE;
}

/* ----------------------------------------------------------------------- */

void leftTurn(void) {
  M->dirIs(_leftTurn[MY_DIR]);
  updateView = TRUE;
}

/* ----------------------------------------------------------------------- */

void rightTurn(void) {
  M->dirIs(_rightTurn[MY_DIR]);
  updateView = TRUE;
}

/* ----------------------------------------------------------------------- */

/* remember ... "North" is to the right ... positive X motion */

void forward(void) {
  register int tx = MY_X_LOC;
  register int ty = MY_Y_LOC;

  switch (MY_DIR) {
    case NORTH:
      if (!M->maze_[tx + 1][ty]) tx++;
      break;
    case SOUTH:
      if (!M->maze_[tx - 1][ty]) tx--;
      break;
    case EAST:
      if (!M->maze_[tx][ty + 1]) ty++;
      break;
    case WEST:
      if (!M->maze_[tx][ty - 1]) ty--;
      break;
    default:
      MWError("bad direction in Forward");
  }
  if ((MY_X_LOC != tx) || (MY_Y_LOC != ty)) {
    M->xlocIs(Loc(tx));
    M->ylocIs(Loc(ty));
    updateView = TRUE;
  }
}

/* ----------------------------------------------------------------------- */

void backward() {
  register int tx = MY_X_LOC;
  register int ty = MY_Y_LOC;

  switch (MY_DIR) {
    case NORTH:
      if (!M->maze_[tx - 1][ty]) tx--;
      break;
    case SOUTH:
      if (!M->maze_[tx + 1][ty]) tx++;
      break;
    case EAST:
      if (!M->maze_[tx][ty - 1]) ty--;
      break;
    case WEST:
      if (!M->maze_[tx][ty + 1]) ty++;
      break;
    default:
      MWError("bad direction in Backward");
  }
  if ((MY_X_LOC != tx) || (MY_Y_LOC != ty)) {
    M->xlocIs(Loc(tx));
    M->ylocIs(Loc(ty));
    updateView = TRUE;
  }
}

/* ----------------------------------------------------------------------- */

void peekLeft() {
  M->xPeekIs(MY_X_LOC);
  M->yPeekIs(MY_Y_LOC);
  M->dirPeekIs(MY_DIR);

  switch (MY_DIR) {
    case NORTH:
      if (!M->maze_[MY_X_LOC + 1][MY_Y_LOC]) {
        M->xPeekIs(MY_X_LOC + 1);
        M->dirPeekIs(WEST);
      }
      break;

    case SOUTH:
      if (!M->maze_[MY_X_LOC - 1][MY_Y_LOC]) {
        M->xPeekIs(MY_X_LOC - 1);
        M->dirPeekIs(EAST);
      }
      break;

    case EAST:
      if (!M->maze_[MY_X_LOC][MY_Y_LOC + 1]) {
        M->yPeekIs(MY_Y_LOC + 1);
        M->dirPeekIs(NORTH);
      }
      break;

    case WEST:
      if (!M->maze_[MY_X_LOC][MY_Y_LOC - 1]) {
        M->yPeekIs(MY_Y_LOC - 1);
        M->dirPeekIs(SOUTH);
      }
      break;

    default:
      MWError("bad direction in PeekLeft");
  }

  /* if any change, display the new view without moving! */

  if ((M->xPeek() != MY_X_LOC) || (M->yPeek() != MY_Y_LOC)) {
    M->peekingIs(TRUE);
    updateView = TRUE;
  }
}

/* ----------------------------------------------------------------------- */

void peekRight() {
  M->xPeekIs(MY_X_LOC);
  M->yPeekIs(MY_Y_LOC);
  M->dirPeekIs(MY_DIR);

  switch (MY_DIR) {
    case NORTH:
      if (!M->maze_[MY_X_LOC + 1][MY_Y_LOC]) {
        M->xPeekIs(MY_X_LOC + 1);
        M->dirPeekIs(EAST);
      }
      break;

    case SOUTH:
      if (!M->maze_[MY_X_LOC - 1][MY_Y_LOC]) {
        M->xPeekIs(MY_X_LOC - 1);
        M->dirPeekIs(WEST);
      }
      break;

    case EAST:
      if (!M->maze_[MY_X_LOC][MY_Y_LOC + 1]) {
        M->yPeekIs(MY_Y_LOC + 1);
        M->dirPeekIs(SOUTH);
      }
      break;

    case WEST:
      if (!M->maze_[MY_X_LOC][MY_Y_LOC - 1]) {
        M->yPeekIs(MY_Y_LOC - 1);
        M->dirPeekIs(NORTH);
      }
      break;

    default:
      MWError("bad direction in PeekRight");
  }

  /* if any change, display the new view without moving! */

  if ((M->xPeek() != MY_X_LOC) || (M->yPeek() != MY_Y_LOC)) {
    M->peekingIs(TRUE);
    updateView = TRUE;
  }
}

/* ----------------------------------------------------------------------- */

void peekStop() {
  M->peekingIs(FALSE);
  updateView = TRUE;
}

/* ----------------------------------------------------------------------- */

void shoot() { printf("Implement shoot()\n"); }

/* ----------------------------------------------------------------------- */

void cloak() { printf("Implement cloak()\n"); }

/* ----------------------------------------------------------------------- */

/*
 * Exit from game, clean up window
 */

void quit(int sig) {
  StopWindow();
  exit(0);
}

/* ----------------------------------------------------------------------- */

void NewPosition(MazewarInstance::Ptr m) {
  Loc newX(0);
  Loc newY(0);
  Direction dir(0); /* start on occupied square */

  while (M->maze_[newX.value()][newY.value()]) {
    /* MAZE[XY]MAX is a power of 2 */
    newX = Loc(random() & (MAZEXMAX - 1));
    newY = Loc(random() & (MAZEYMAX - 1));

    /* In real game, also check that square is
       unoccupied by another rat */
  }

  /* prevent a blank wall at first glimpse */

  if (!m->maze_[(newX.value()) + 1][(newY.value())]) dir = Direction(NORTH);
  if (!m->maze_[(newX.value()) - 1][(newY.value())]) dir = Direction(SOUTH);
  if (!m->maze_[(newX.value())][(newY.value()) + 1]) dir = Direction(EAST);
  if (!m->maze_[(newX.value())][(newY.value()) - 1]) dir = Direction(WEST);

  m->xlocIs(newX);
  m->ylocIs(newY);
  m->dirIs(dir);
}

/* ----------------------------------------------------------------------- */

void MWError(char *s)

{
  StopWindow();
  fprintf(stderr, "CS244BMazeWar: %s\n", s);
  perror("CS244BMazeWar");
  exit(-1);
}

/* ----------------------------------------------------------------------- */

/* This is just for the sample version, rewrite your own */
Score GetRatScore(RatIndexType ratId) {
  if (ratId.value() == M->myRatId().value()) {
    return (M->score());
  } else {
    return (0);
  }
}

/* ----------------------------------------------------------------------- */
/* This is just for the sample version, rewrite your own */
// TODO(alhaad): Fix the need for this hackery.
static char rat_name[NAMESIZE];
char *GetRatName(RatIndexType ratId) {
  if (ratId.value() == M->myRatId().value()) {
    return (M->myName_);
  } else {
    for (int i = 0; i < strlen(M->rat(ratId).rat_name); i++) {
      rat_name[i] = M->rat(ratId).rat_name[i];
    }
    rat_name[strlen(M->rat(ratId).rat_name)] = '\0';
    return (rat_name);
  }
}

/* ----------------------------------------------------------------------- */

/* This is just for the sample version, rewrite your own if necessary */
void ConvertIncoming(MW244BPacket *p) {}

/* ----------------------------------------------------------------------- */

/* This is just for the sample version, rewrite your own if necessary */
void ConvertOutgoing(MW244BPacket *p) {}

/* ----------------------------------------------------------------------- */

/* This is just for the sample version, rewrite your own */
void ratStates() {
  /* In our sample version, we don't know about the state of any rats over
     the net, so this is a no-op */
}

/* ----------------------------------------------------------------------- */

/* This is just for the sample version, rewrite your own */
void manageMissiles() {
  /* Leave this up to you. */
  /*
  //You may find the following lines useful
  //This subtracts one from the current rat's score and updates the display
  M->scoreIs( M->score().value()-1 );
  UpdateScoreCard(M->myRatId().value());
  */
}

/* ----------------------------------------------------------------------- */

void DoViewUpdate() {
  if (updateView) { /* paint the screen */
    ShowPosition(MY_X_LOC, MY_Y_LOC, MY_DIR);
    if (M->peeking())
      ShowView(M->xPeek(), M->yPeek(), M->dirPeek());
    else
      ShowView(MY_X_LOC, MY_Y_LOC, MY_DIR);
    updateView = FALSE;
  }
}

/* ----------------------------------------------------------------------- */

void processRemoteStatePacket(StatePacket packet) {
  RatIndexType rat_index = player_id_to_rat_index.at(packet.header.player_id);
  Rat rat = M->rat(rat_index);
  strncpy(rat.rat_name, packet.body.player_name, NAMESIZE);
  M->ratIs(rat, rat_index);
  SetRatPosition(rat_index, Loc(packet.body.rat_x_pos), Loc(packet.body.rat_y_pos), NORTH);
  WriteScoreString(rat_index);
  updateView = TRUE;
}

/* ----------------------------------------------------------------------- */


void processPacket(MWEvent *eventPacket) {
          MW244BPacket		*pack = eventPacket->eventDetail;
  Header *header = (Header*) &pack->body;
  // Ignore packet if from self.
  if (header->player_id == player_id) {
    return;
  }

  // Are we seeing this player for the first time?
  if (player_id_to_rat_index.count(header->player_id) == 0) {
    RatIndexType rat_index = free_rat_index_list.front();
    free_rat_index_list.pop_front();
    Rat rat = M->rat(rat_index);
    rat.playing = TRUE;
    player_id_to_rat_index.insert(make_pair(header->player_id, rat_index));
    M->ratIs(rat, rat_index);
  }

  switch (header->descriptor_type) {
    case 1:
      processRemoteStatePacket(*(StatePacket*) (&pack->body));
      break;
    default:
      assert(false);
  }
}

/* ----------------------------------------------------------------------- */

void netInit() {
  Sockaddr nullAddr;
  Sockaddr *thisHost;
  char buf[128];
  int reuse;
  u_char ttl;
  struct ip_mreq mreq;

  /* MAZEPORT will be assigned by the TA to each team */
  M->mazePortIs(htons(MAZEPORT));

  gethostname(buf, sizeof(buf));
  if ((thisHost = resolveHost(buf)) == (Sockaddr *)NULL) MWError("who am I?");
  bcopy((caddr_t)thisHost, (caddr_t)(M->myAddr()), sizeof(Sockaddr));

  M->theSocketIs(socket(AF_INET, SOCK_DGRAM, 0));
  if (M->theSocket() < 0) MWError("can't get socket");

  /* SO_REUSEADDR allows more than one binding to the same
     socket - you cannot have more than one player on one
     machine without this */
  reuse = 1;
  if (setsockopt(M->theSocket(), SOL_SOCKET, SO_REUSEADDR, &reuse,
                 sizeof(reuse)) < 0) {
    MWError("setsockopt failed (SO_REUSEADDR)");
  }

  nullAddr.sin_family = AF_INET;
  nullAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  nullAddr.sin_port = M->mazePort();
  if (bind(M->theSocket(), (struct sockaddr *)&nullAddr, sizeof(nullAddr)) < 0)
    MWError("netInit binding");

  /* Multicast TTL:
     0 restricted to the same host
     1 restricted to the same subnet
     32 restricted to the same site
     64 restricted to the same region
     128 restricted to the same continent
     255 unrestricted

     DO NOT use a value > 32. If possible, use a value of 1 when
     testing.
  */

  ttl = 1;
  if (setsockopt(M->theSocket(), IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
                 sizeof(ttl)) < 0) {
    MWError("setsockopt failed (IP_MULTICAST_TTL)");
  }

  /* join the multicast group */
  mreq.imr_multiaddr.s_addr = htonl(MAZEGROUP);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(M->theSocket(), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                 sizeof(mreq)) < 0) {
    MWError("setsockopt failed (IP_ADD_MEMBERSHIP)");
  }

  /*
   * Now we can try to find a game to join; if none, start one.
   */

  printf("\n");

  /* set up some stuff strictly for this local sample */
  M->myRatIdIs(0);
  M->scoreIs(0);
  SetMyRatIndexType(0);

  /* Get the multi-cast address ready to use in SendData()
     calls. */
  memcpy(&groupAddr, &nullAddr, sizeof(Sockaddr));
  groupAddr.sin_addr.s_addr = htonl(MAZEGROUP);
}

/* ----------------------------------------------------------------------- */
