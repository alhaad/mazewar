RatApp_type Rats2Display;

/*
 * Manage the display. This routine handles the three areas of the
 * screen: the perspective view, the maze overview, and the scores.
 *
 * The coordinate system of the display is a x-reflected 1st quadrant:
 * (0,0) upper left, x increases to the right, y increases down. Just
 * like most CRT coordinate systems. North is to the right, for no
 * particular reason. Cells in the maze are 16x16.
 */

static BitCell normalArrows[NDIRECTION] = {
    {{0, 0200, 0300, 0340, /* right */
      0360, 0370, 0177774, 0177776, 0177777, 0177776, 0177774, 0370, 0360, 0340,
      0300, 0200}},
    {{0400, 01400, 03400, 07400, /* left */
      017400, 037777, 077777, 0177777, 077777, 037777, 017400, 07400, 03400,
      01400, 0400, 0}},
    {{01740, 01740, 01740, 01740, /* down */
      01740, 01740, 01740, 01740, 077777, 037776, 017774, 07770, 03760, 01740,
      0700, 0200}},
    {{0200, 0700, 01740, 03760, /* up */
      07770, 017774, 037776, 077777, 01740, 01740, 01740, 01740, 01740, 01740,
      01740, 01740}}};

static BitCell missile[1] = {
    {{0000, 0000, 0000, 0000, 0000, 0000, 01700, 01700, 01700, 01700, 0000,
      0000, 0000, 0000, 0000, 0000}}};

static XYpair viewTable[] = {
    {{67, 333}, {67, 67}},    {{67, 333}, {333, 333}},
    {{67, 67}, {333, 67}},    {{0, 400}, {67, 333}},
    {{0, 0}, {67, 67}},       {{0, 333}, {67, 333}},
    {{0, 67}, {67, 67}},      {{333, 333}, {333, 67}},
    {{333, 333}, {400, 333}}, {{333, 67}, {400, 67}},
    {{333, 333}, {400, 400}}, {{333, 67}, {400, 0}},
    {{120, 280}, {120, 120}}, {{120, 280}, {280, 280}},
    {{120, 120}, {280, 120}}, {{67, 333}, {120, 280}},
    {{67, 67}, {120, 120}},   {{67, 280}, {120, 280}},
    {{67, 120}, {120, 120}},  {{280, 280}, {280, 120}},
    {{280, 280}, {333, 280}}, {{280, 120}, {333, 120}},
    {{280, 280}, {333, 333}}, {{280, 120}, {333, 67}},
    {{143, 257}, {143, 143}}, {{143, 257}, {257, 257}},
    {{143, 143}, {257, 143}}, {{120, 280}, {143, 257}},
    {{120, 120}, {143, 143}}, {{120, 257}, {143, 257}},
    {{120, 143}, {143, 143}}, {{257, 257}, {257, 143}},
    {{257, 257}, {280, 257}}, {{257, 143}, {280, 143}},
    {{257, 257}, {280, 280}}, {{257, 143}, {280, 120}},
    {{156, 244}, {156, 156}}, {{156, 244}, {244, 244}},
    {{156, 156}, {244, 156}}, {{143, 257}, {156, 244}},
    {{143, 143}, {156, 156}}, {{143, 244}, {156, 244}},
    {{143, 156}, {156, 156}}, {{244, 244}, {244, 156}},
    {{244, 244}, {257, 244}}, {{244, 156}, {257, 156}},
    {{244, 244}, {257, 257}}, {{244, 156}, {257, 143}},
    {{164, 236}, {164, 164}}, {{164, 236}, {236, 236}},
    {{164, 164}, {236, 164}}, {{156, 244}, {164, 236}},
    {{156, 156}, {164, 164}}, {{156, 236}, {164, 236}},
    {{156, 164}, {164, 164}}, {{236, 236}, {236, 164}},
    {{236, 236}, {244, 236}}, {{236, 164}, {244, 164}},
    {{236, 236}, {244, 244}}, {{236, 164}, {244, 156}},
    {{170, 230}, {170, 170}}, {{170, 230}, {230, 230}},
    {{170, 170}, {230, 170}}, {{164, 236}, {170, 230}},
    {{164, 164}, {170, 170}}, {{164, 230}, {170, 230}},
    {{164, 170}, {170, 170}}, {{230, 230}, {230, 170}},
    {{230, 230}, {236, 230}}, {{230, 170}, {236, 170}},
    {{230, 230}, {236, 236}}, {{230, 170}, {236, 164}},
    {{174, 226}, {174, 174}}, {{174, 226}, {226, 226}},
    {{174, 174}, {226, 174}}, {{170, 230}, {174, 226}},
    {{170, 170}, {174, 174}}, {{170, 226}, {174, 226}},
    {{170, 174}, {174, 174}}, {{226, 226}, {226, 174}},
    {{226, 226}, {230, 226}}, {{226, 174}, {230, 174}},
    {{226, 226}, {230, 230}}, {{226, 174}, {230, 170}},
    {{177, 223}, {177, 177}}, {{177, 223}, {223, 223}},
    {{177, 177}, {223, 177}}, {{174, 226}, {177, 223}},
    {{174, 174}, {177, 177}}, {{174, 223}, {177, 223}},
    {{174, 177}, {177, 177}}, {{223, 223}, {223, 177}},
    {{223, 223}, {226, 223}}, {{223, 177}, {226, 177}},
    {{223, 223}, {226, 226}}, {{223, 177}, {226, 174}},
    {{179, 221}, {179, 179}}, {{179, 221}, {221, 221}},
    {{179, 179}, {221, 179}}, {{177, 223}, {179, 221}},
    {{177, 177}, {179, 179}}, {{177, 221}, {179, 221}},
    {{177, 179}, {179, 179}}, {{221, 221}, {221, 179}},
    {{221, 221}, {223, 221}}, {{221, 179}, {223, 179}},
    {{221, 221}, {223, 223}}, {{221, 179}, {223, 177}},
    {{181, 219}, {181, 181}}, {{181, 219}, {219, 219}},
    {{181, 181}, {219, 181}}, {{179, 221}, {181, 219}},
    {{179, 179}, {181, 181}}, {{179, 219}, {181, 219}},
    {{179, 181}, {181, 181}}, {{219, 219}, {219, 181}},
    {{219, 219}, {221, 219}}, {{219, 181}, {221, 181}},
    {{219, 219}, {221, 221}}, {{219, 181}, {221, 179}},
    {{183, 217}, {183, 183}}, {{183, 217}, {217, 217}},
    {{183, 183}, {217, 183}}, {{181, 219}, {183, 217}},
    {{181, 181}, {183, 183}}, {{181, 217}, {183, 217}},
    {{181, 183}, {183, 183}}, {{217, 217}, {217, 183}},
    {{217, 217}, {219, 217}}, {{217, 183}, {219, 183}},
    {{217, 217}, {219, 219}}, {{217, 183}, {219, 181}},
    {{184, 216}, {184, 184}}, {{184, 216}, {216, 216}},
    {{184, 184}, {216, 184}}, {{183, 217}, {184, 216}},
    {{183, 183}, {184, 184}}, {{183, 216}, {184, 216}},
    {{183, 184}, {184, 184}}, {{216, 216}, {216, 184}},
    {{216, 216}, {217, 216}}, {{216, 184}, {217, 184}},
    {{216, 216}, {217, 217}}, {{216, 184}, {217, 183}},
    {{186, 214}, {186, 186}}, {{186, 214}, {214, 214}},
    {{186, 186}, {214, 186}}, {{184, 216}, {186, 214}},
    {{184, 184}, {186, 186}}, {{184, 214}, {186, 214}},
    {{184, 186}, {186, 186}}, {{214, 214}, {214, 186}},
    {{214, 214}, {216, 214}}, {{214, 186}, {216, 186}},
    {{214, 214}, {216, 216}}, {{214, 186}, {216, 184}},
    {{187, 213}, {187, 187}}, {{187, 213}, {213, 213}},
    {{187, 187}, {213, 187}}, {{186, 214}, {187, 213}},
    {{186, 186}, {187, 187}}, {{186, 213}, {187, 213}},
    {{186, 187}, {187, 187}}, {{213, 213}, {213, 187}},
    {{213, 213}, {214, 213}}, {{213, 187}, {214, 187}},
    {{213, 213}, {214, 214}}, {{213, 187}, {214, 186}},
    {{188, 212}, {188, 188}}, {{188, 212}, {212, 212}},
    {{188, 188}, {212, 188}}, {{187, 213}, {188, 212}},
    {{187, 187}, {188, 188}}, {{187, 212}, {188, 212}},
    {{187, 188}, {188, 188}}, {{212, 212}, {212, 188}},
    {{212, 212}, {213, 212}}, {{212, 188}, {213, 188}},
    {{212, 212}, {213, 213}}, {{212, 188}, {213, 187}},
    {{188, 212}, {188, 188}}, {{188, 212}, {212, 212}},
    {{188, 188}, {212, 188}}, {{188, 212}, {188, 212}},
    {{188, 188}, {188, 188}}, {{188, 212}, {188, 212}},
    {{188, 188}, {188, 188}}, {{212, 212}, {212, 188}},
    {{212, 212}, {212, 212}}, {{212, 188}, {212, 188}},
    {{212, 212}, {212, 212}}, {{212, 188}, {212, 188}},
    {{189, 211}, {189, 189}}, {{189, 211}, {211, 211}},
    {{189, 189}, {211, 189}}, {{188, 212}, {189, 211}},
    {{188, 188}, {189, 189}}, {{188, 211}, {189, 211}},
    {{188, 189}, {189, 189}}, {{211, 211}, {211, 189}},
    {{211, 211}, {212, 211}}, {{211, 189}, {212, 189}},
    {{211, 211}, {212, 212}}, {{211, 189}, {212, 188}},
    {{190, 210}, {190, 190}}, {{190, 210}, {210, 210}},
    {{190, 190}, {210, 190}}, {{189, 211}, {190, 210}},
    {{189, 189}, {190, 190}}, {{189, 210}, {190, 210}},
    {{189, 190}, {190, 190}}, {{210, 210}, {210, 190}},
    {{210, 210}, {211, 210}}, {{210, 190}, {211, 190}},
    {{210, 210}, {211, 211}}, {{210, 190}, {211, 189}},
    {{190, 210}, {190, 190}}, {{190, 210}, {210, 210}},
    {{190, 190}, {210, 190}}, {{190, 210}, {190, 210}},
    {{190, 190}, {190, 190}}, {{190, 210}, {190, 210}},
    {{190, 190}, {190, 190}}, {{210, 210}, {210, 190}},
    {{210, 210}, {210, 210}}, {{210, 190}, {210, 190}},
    {{210, 210}, {210, 210}}, {{210, 190}, {210, 190}},
    {{191, 209}, {191, 191}}, {{191, 209}, {209, 209}},
    {{191, 191}, {209, 191}}, {{190, 210}, {191, 209}},
    {{190, 190}, {191, 191}}, {{190, 209}, {191, 209}},
    {{190, 191}, {191, 191}}, {{209, 209}, {209, 191}},
    {{209, 209}, {210, 209}}, {{209, 191}, {210, 191}},
    {{209, 209}, {210, 210}}, {{209, 191}, {210, 190}},
    {{191, 209}, {191, 191}}, {{191, 209}, {209, 209}},
    {{191, 191}, {209, 191}}, {{191, 209}, {191, 209}},
    {{191, 191}, {191, 191}}, {{191, 209}, {191, 209}},
    {{191, 191}, {191, 191}}, {{209, 209}, {209, 191}},
    {{209, 209}, {209, 209}}, {{209, 191}, {209, 191}},
    {{209, 209}, {209, 209}}, {{209, 191}, {209, 191}},
    {{192, 208}, {192, 192}}, {{192, 208}, {208, 208}},
    {{192, 192}, {208, 192}}, {{191, 209}, {192, 208}},
    {{191, 191}, {192, 192}}, {{191, 208}, {192, 208}},
    {{191, 192}, {192, 192}}, {{208, 208}, {208, 192}},
    {{208, 208}, {209, 208}}, {{208, 192}, {209, 192}},
    {{208, 208}, {209, 209}}, {{208, 192}, {209, 191}},
    {{192, 208}, {192, 192}}, {{192, 208}, {208, 208}},
    {{192, 192}, {208, 192}}, {{192, 208}, {192, 208}},
    {{192, 192}, {192, 192}}, {{192, 208}, {192, 208}},
    {{192, 192}, {192, 192}}, {{208, 208}, {208, 192}},
    {{208, 208}, {208, 208}}, {{208, 192}, {208, 192}},
    {{208, 208}, {208, 208}}, {{208, 192}, {208, 192}},
    {{192, 208}, {192, 192}}, {{192, 208}, {208, 208}},
    {{192, 192}, {208, 192}}, {{192, 208}, {192, 208}},
    {{192, 192}, {192, 192}}, {{192, 208}, {192, 208}},
    {{192, 192}, {192, 192}}, {{208, 208}, {208, 192}},
    {{208, 208}, {208, 208}}, {{208, 192}, {208, 192}},
    {{208, 208}, {208, 208}}, {{208, 192}, {208, 192}},
    {{193, 207}, {193, 193}}, {{193, 207}, {207, 207}},
    {{193, 193}, {207, 193}}, {{192, 208}, {193, 207}},
    {{192, 192}, {193, 193}}, {{192, 207}, {193, 207}},
    {{192, 193}, {193, 193}}, {{207, 207}, {207, 193}},
    {{207, 207}, {208, 207}}, {{207, 193}, {208, 193}},
    {{207, 207}, {208, 208}}, {{207, 193}, {208, 192}},
    {{193, 207}, {193, 193}}, {{193, 207}, {207, 207}},
    {{193, 193}, {207, 193}}, {{193, 207}, {193, 207}},
    {{193, 193}, {193, 193}}, {{193, 207}, {193, 207}},
    {{193, 193}, {193, 193}}, {{207, 207}, {207, 193}},
    {{207, 207}, {207, 207}}, {{207, 193}, {207, 193}},
    {{207, 207}, {207, 207}}, {{207, 193}, {207, 193}},
    {{193, 207}, {193, 193}}, {{193, 207}, {207, 207}},
    {{193, 193}, {207, 193}}, {{193, 207}, {193, 207}},
    {{193, 193}, {193, 193}}, {{193, 207}, {193, 207}},
    {{193, 193}, {193, 193}}, {{207, 207}, {207, 193}},
    {{207, 207}, {207, 207}}, {{207, 193}, {207, 193}},
    {{207, 207}, {207, 207}}, {{207, 193}, {207, 193}},
    {{193, 207}, {193, 193}}, {{193, 207}, {207, 207}},
    {{193, 193}, {207, 193}}, {{193, 207}, {193, 207}},
    {{193, 193}, {193, 193}}, {{193, 207}, {193, 207}},
    {{193, 193}, {193, 193}}, {{207, 207}, {207, 193}},
    {{207, 207}, {207, 207}}, {{207, 193}, {207, 193}},
    {{207, 207}, {207, 207}}, {{207, 193}, {207, 193}},
    {{194, 206}, {194, 194}}, {{194, 206}, {206, 206}},
    {{194, 194}, {206, 194}}, {{193, 207}, {194, 206}},
    {{193, 193}, {194, 194}}, {{193, 206}, {194, 206}},
    {{193, 194}, {194, 194}}, {{206, 206}, {206, 194}},
    {{206, 206}, {207, 206}}, {{206, 194}, {207, 194}},
    {{206, 206}, {207, 207}}, {{206, 194}, {207, 193}},
    {{194, 206}, {194, 194}}, {{194, 206}, {206, 206}},
    {{194, 194}, {206, 194}}, {{194, 206}, {194, 206}},
    {{194, 194}, {194, 194}}, {{194, 206}, {194, 206}},
    {{194, 194}, {194, 194}}, {{206, 206}, {206, 194}},
    {{206, 206}, {206, 206}}, {{206, 194}, {206, 194}},
    {{206, 206}, {206, 206}}, {{206, 194}, {206, 194}},
};

#define ratBits_width 384
#define ratBits_height 64
unsigned short ratBits[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0007, 0xe000, 0x0007, 0xe000, 0x003c, 0x0000, 0x3c00, 0x0df0,

    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x001f, 0xf800, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x003f, 0xfc00, 0x003f, 0xfc00, 0x008f, 0x0000, 0x9f00, 0x13f8,

    0x0000, 0x001f, 0xf800, 0x0000, 0x0000, 0x01ff, 0xff80, 0x0000,
    0x0000, 0x001f, 0xf800, 0x0000, 0x0000, 0x001f, 0xf800, 0x0000,
    0x00f3, 0xff00, 0x00f3, 0xff00, 0x037f, 0xc003, 0x7fc0, 0x2ffc,

    0x0000, 0x01ff, 0xff80, 0x0000, 0x0000, 0x0fff, 0xfff0, 0x0000,
    0x0000, 0x01ff, 0xff80, 0x0000, 0x0000, 0x01ff, 0xff80, 0x0000,
    0x01c7, 0xff80, 0x01c7, 0xff80, 0x04ff, 0xe004, 0xffe0, 0x2ffc,

    0x0000, 0x0fff, 0xfff0, 0x0000, 0x0000, 0x3fff, 0xfffc, 0x0000,
    0x0000, 0x0fff, 0xfff0, 0x0000, 0x0000, 0x0fff, 0xfff0, 0x0000,
    0x031f, 0xffc0, 0x031f, 0xffc0, 0x0bff, 0xf00b, 0xfff0, 0x47fe,

    0x0000, 0x3fff, 0xfffc, 0x0000, 0x0000, 0xffe3, 0xffff, 0x0000,
    0x0000, 0x3fff, 0xfffc, 0x0000, 0x0000, 0x3fff, 0xfffc, 0x0000,
    0x067f, 0xffe0, 0x067f, 0xffe0, 0x0bff, 0xf00f, 0xfff0, 0x63fe,

    0x0000, 0xffe3, 0xffff, 0x0000, 0x0001, 0xe79f, 0xffff, 0x8000,
    0x0000, 0xffe3, 0xffff, 0x0000, 0x0000, 0xff8f, 0xffff, 0x0000,
    0x0cff, 0xfff0, 0x0cff, 0xfff0, 0x11ff, 0xf81f, 0xfff8, 0x63fe,

    0x0001, 0xe79f, 0xffff, 0x8000, 0x0003, 0xf03f, 0xffff, 0xc000,
    0x0001, 0xe79f, 0xffff, 0x8000, 0x0001, 0xe60f, 0xffff, 0x8000,
    0x0bff, 0xfff0, 0x0bff, 0xfff0, 0x10ff, 0xf81f, 0xfff8, 0x47fe,

    0x0003, 0xf03f, 0xffff, 0xc000, 0x0007, 0xc07f, 0xffff, 0xe000,
    0x0003, 0xf03f, 0xffff, 0xc000, 0x0003, 0xf83f, 0xffff, 0xc000,
    0x19ff, 0xfff8, 0x1fff, 0xfff8, 0x307f, 0xfc3f, 0xfffc, 0x2ffc,

    0x0007, 0xc07f, 0xffff, 0xe000, 0x000d, 0x01ff, 0xffff, 0xf000,
    0x0007, 0xc07f, 0xffff, 0xe000, 0x0007, 0xc03f, 0xffff, 0xe000,
    0x10ff, 0xfff8, 0x1fff, 0xfff8, 0x383f, 0xfc3f, 0xfffc, 0x3ffc,

    0x000d, 0x01ff, 0xffff, 0xf000, 0x0018, 0x0dff, 0xffff, 0xf800,
    0x000d, 0x01ff, 0xffff, 0xf000, 0x000f, 0x01ff, 0xffff, 0xf000,
    0x087f, 0xfff8, 0x1fff, 0xfff8, 0x383f, 0xfc3f, 0xfffc, 0x1ff8,

    0x0018, 0x0dff, 0xffff, 0xf800, 0x003c, 0x3eff, 0xffff, 0xfc00,
    0x0018, 0x0dff, 0xffff, 0xf800, 0x001c, 0x0dff, 0xffff, 0xf800,
    0x183f, 0xfffc, 0x3fff, 0xfffc, 0x307f, 0xfc3f, 0xfffc, 0x1bdc,

    0x003c, 0x3eff, 0xffff, 0xfc00, 0x0078, 0xfcff, 0xffff, 0xfe00,
    0x003c, 0x3eff, 0xffff, 0xfc00, 0x003c, 0x3eff, 0xffff, 0xfc00,
    0x1c1f, 0xfffc, 0x3fff, 0xfffc, 0x10ff, 0xf81f, 0xfff8, 0x2e3a,

    0x0078, 0xfcff, 0xffff, 0xfe00, 0x00f1, 0x7eff, 0xffff, 0xff00,
    0x0078, 0xfcff, 0xffff, 0xfe00, 0x0078, 0xfcff, 0xffff, 0xfe00,
    0x3c0f, 0xfffc, 0x3fff, 0xfffc, 0x11ff, 0xf81f, 0xfff8, 0x17f4,

    0x00f1, 0x7eff, 0xffff, 0xff00, 0x00e3, 0x9fff, 0xffff, 0xff00,
    0x00f1, 0x7eff, 0xffff, 0xff00, 0x00f1, 0x7eff, 0xffff, 0xff00,
    0x3c0f, 0xfffc, 0x3fff, 0xfffc, 0x0fff, 0xf00f, 0xfff0, 0x0000,

    0x00e3, 0x9fff, 0xffff, 0xff00, 0x01ef, 0xcfff, 0xffff, 0xff80,
    0x00e3, 0x9fff, 0xffff, 0xff00, 0x00e3, 0x9fff, 0xffff, 0xff00,
    0x3c1f, 0xfffc, 0x3fff, 0xfffc, 0x0fff, 0xf00f, 0xfff0, 0x03c0,

    0x01ef, 0xcfff, 0xffff, 0xff80, 0x01df, 0xffff, 0xffff, 0xfc80,
    0x01ef, 0xcfff, 0xffff, 0xff80, 0x01e7, 0xcfff, 0xffff, 0xff80,
    0x383f, 0xfffc, 0x3fff, 0xfffc, 0x07ff, 0xe007, 0xffe0, 0x0df0,

    0x013f, 0xffff, 0xffff, 0xff80, 0x03ff, 0xffff, 0xffff, 0xc0c0,
    0x01df, 0xffff, 0xffff, 0xff80, 0x01df, 0xffff, 0xffff, 0xff80,
    0x187f, 0xfff8, 0x1fff, 0xfff8, 0x03ff, 0xc003, 0xffc0, 0x13f8,

    0x0303, 0xffff, 0xffff, 0xffc0, 0x03ff, 0xffff, 0xfffe, 0x0040,
    0x03ff, 0xffff, 0xffff, 0xffc0, 0x03ff, 0xffe0, 0x07ff, 0xffc0,
    0x10ff, 0xfff8, 0x1fff, 0xfff8, 0x0b7e, 0x680b, 0x7e68, 0x2ffc,

    0x0200, 0x7fff, 0xffff, 0xffc0, 0x03ff, 0xffff, 0xfff8, 0x0040,
    0x03ff, 0xffff, 0xffff, 0xffc0, 0x03ff, 0xff00, 0x00ff, 0xffc0,
    0x19ff, 0xfff8, 0x1fff, 0xfff8, 0x15f9, 0xd415, 0xf9d4, 0x3ff4,

    0x0200, 0x1fff, 0xffff, 0xffc0, 0x07ff, 0xffff, 0xffe0, 0x0060,
    0x03ff, 0xffff, 0xffff, 0xffc0, 0x03ff, 0xf800, 0x001f, 0xffc0,
    0x0fff, 0xfff0, 0x0fff, 0xfff0, 0x0aae, 0xa80a, 0xaea8, 0x7fe2,

    0x0600, 0x07ff, 0xffff, 0xffe0, 0x07ff, 0xffff, 0xffc0, 0x00e0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07ff, 0xc000, 0x0003, 0xffe0,
    0x0fff, 0xfff0, 0x0fff, 0xfff0, 0x0155, 0x4001, 0x5540, 0x7fc6,

    0x0700, 0xcfff, 0xffff, 0xffe0, 0x07ff, 0xffff, 0xff0a, 0x00e0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07ff, 0x0007, 0xe000, 0xffe0,
    0x07ff, 0xff60, 0x07ff, 0xff60, 0x0000, 0x0000, 0x0000, 0x7fc6,

    0x0700, 0x30ff, 0xffff, 0xffe0, 0x07ff, 0xffff, 0xfe0c, 0x00e0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07fc, 0x000d, 0xb800, 0x3fe0,
    0x03ff, 0xfcc0, 0x03ff, 0xfcc0, 0x0000, 0x0000, 0x0000, 0x7fe2,

    0x0700, 0x407f, 0xffff, 0xffe0, 0x0fff, 0xffff, 0xfc38, 0x01f0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07f0, 0x0005, 0xbc00, 0x0fe0,
    0x07ff, 0xff68, 0x07ff, 0xff68, 0x003c, 0x0000, 0x3c00, 0x3ff4,

    0x0f81, 0x003f, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xf875, 0x81b0,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0fc0, 0x0003, 0x7602, 0x03f0,
    0x15ff, 0xf9d4, 0x15ff, 0xf9d4, 0x009f, 0x0000, 0x9f00, 0x3ffc,

    0x0d80, 0x021f, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xf382, 0x41f0,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0fc1, 0x0043, 0xce00, 0x8df0,
    0x2adf, 0xe3aa, 0x2adf, 0xe3aa, 0x037f, 0xc003, 0x7fc0, 0x1ff8,

    0x0f80, 0x01cf, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xee00, 0x0170,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0f2a, 0x00c3, 0xef00, 0x70f0,
    0x157c, 0x3f54, 0x157c, 0x3f54, 0x04ff, 0xe004, 0xffe0, 0x1bdc,

    0x0e80, 0x6677, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xfc00, 0x0170,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0e6d, 0x0091, 0xf900, 0x8070,
    0x0aaf, 0xfaa8, 0x0aaf, 0xfaa8, 0x0bff, 0xf00b, 0xfff0, 0x2e3a,

    0x0e80, 0x183f, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xc000, 0x0170,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0c10, 0x00f8, 0xff00, 0x03b0,
    0x0155, 0x5540, 0x0155, 0x5540, 0x0fff, 0xd00f, 0x81f0, 0x17f4,

    0x0e80, 0x0003, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xe000, 0x01f0,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0c0c, 0x00ff, 0xf700, 0x25f0,
    0x0000, 0x0000, 0x0000, 0x0000, 0x1fff, 0x881e, 0x0078, 0x0000,

    0x0f80, 0x0007, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xf000, 0x01f0,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0e02, 0x0097, 0xf900, 0x1c70,
    0x0000, 0x0000, 0x0000, 0x0000, 0x1fff, 0x0818, 0x1818, 0x03c0,

    0x0f80, 0x000f, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xf800, 0x01f0,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0f01, 0x00fb, 0xe700, 0x00f0,
    0x0007, 0xe000, 0x0007, 0xe000, 0x3ffe, 0x0c30, 0x1c0c, 0x0df0,

    0x0f80, 0x601f, 0xffff, 0xfff0, 0x0fff, 0xffff, 0xfc00, 0x01b0,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0f80, 0x0075, 0xde00, 0x51f0,
    0x003f, 0xfc00, 0x003f, 0xfc00, 0x3ffc, 0x1c20, 0x4e04, 0x13f8,

    0x0d80, 0x91ff, 0xffff, 0xfff0, 0x07ff, 0xffff, 0xfe00, 0x01e0,
    0x0fff, 0xffff, 0xffff, 0xfff0, 0x0fc0, 0x006f, 0x1604, 0x8bf0,
    0x00f3, 0xff00, 0x00f3, 0xff00, 0x3ffc, 0x1c20, 0x6e04, 0x2ffc,

    0x0780, 0x0f7f, 0xffff, 0xffe0, 0x07ff, 0xffff, 0xff00, 0x00e0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07f0, 0x0039, 0x7c03, 0x07e0,
    0x01c7, 0xff80, 0x01c7, 0xff80, 0x3ffe, 0x0c30, 0x7e0c, 0x3ffc,

    0x0700, 0x00ff, 0xffff, 0xffe0, 0x07ff, 0xffff, 0xffc0, 0x00e0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07fc, 0x001d, 0xb810, 0x1fe0,
    0x031f, 0xffc0, 0x031f, 0xffc0, 0x1fff, 0x0818, 0x3c18, 0x7ffe,

    0x0710, 0x03ff, 0xffff, 0xffe0, 0x07ff, 0xffff, 0xffe0, 0x00e0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07ff, 0x0007, 0xe208, 0x7fe0,
    0x067f, 0xffe0, 0x067f, 0xffe0, 0x1fff, 0x881e, 0x1878, 0x7ffe,

    0x0708, 0x07ff, 0xffff, 0xffe0, 0x03ff, 0xffff, 0xfff8, 0x00c0,
    0x07ff, 0xffff, 0xffff, 0xffe0, 0x07ff, 0xc000, 0x0191, 0xffe0,
    0x0cff, 0xfff0, 0x0cff, 0xfff0, 0x0fff, 0xf00f, 0x81f0, 0x7ffe,

    0x0228, 0x1fff, 0xffff, 0xffc0, 0x03ff, 0xffff, 0xfffe, 0x01c0,
    0x03ff, 0xffff, 0xffff, 0xffc0, 0x03ff, 0xf800, 0x0077, 0xffc0,
    0x0bff, 0xfff0, 0x0bff, 0xfff0, 0x0fff, 0xf00f, 0xfff0, 0x7ffe,

    0x0214, 0x7fff, 0xffff, 0xffc0, 0x03ff, 0xffff, 0xffff, 0xc1c0,
    0x03ff, 0xffff, 0xffff, 0xffc0, 0x03ff, 0xff00, 0x001f, 0xffc0,
    0x1fff, 0xff98, 0x1ff0, 0x0ff8, 0x07ff, 0xe007, 0xffe0, 0x3ffc,

    0x0203, 0xffff, 0xffff, 0xffc0, 0x01ff, 0xffff, 0xffff, 0xf980,
    0x03ff, 0xffff, 0xffff, 0xffc0, 0x03ff, 0xff00, 0x00ff, 0xffc0,
    0x1fff, 0xff08, 0x1f80, 0x01f8, 0x03ff, 0xc003, 0xffc0, 0x3ffc,

    0x011f, 0xffff, 0xffff, 0xff80, 0x01ff, 0xffff, 0xffff, 0xff80,
    0x01ff, 0xffff, 0xffff, 0xff80, 0x01ff, 0xffe0, 0x07ff, 0xff80,
    0x1fff, 0xfe18, 0x1e03, 0xc078, 0x0b7e, 0x680b, 0x7e68, 0x1ff8,

    0x01ff, 0xffff, 0xffff, 0xff80, 0x00ff, 0xffff, 0xffff, 0xdf00,
    0x01ff, 0xffff, 0xffff, 0xff80, 0x01ff, 0xffff, 0xffff, 0xff80,
    0x3fff, 0xfc1c, 0x3801, 0xe01c, 0x15f9, 0xd415, 0xf9d4, 0x1bd4,

    0x00fb, 0xffff, 0xffff, 0xff00, 0x00ff, 0xffff, 0xffff, 0xff00,
    0x00ff, 0xffff, 0xffff, 0xdf00, 0x00ff, 0xffff, 0xffff, 0xdf00,
    0x3fff, 0xf83c, 0x3009, 0xf00c, 0x0aae, 0xa80a, 0xaea8, 0x2e2a,

    0x00ff, 0xffff, 0xffff, 0xff00, 0x007f, 0xffff, 0xffff, 0xbe00,
    0x00ff, 0xffff, 0xffff, 0xff00, 0x00ff, 0xffff, 0xffff, 0xff00,
    0x3fff, 0xf03c, 0x200c, 0xf004, 0x0155, 0x4001, 0x5540, 0x17d4,

    0x007f, 0xffff, 0xffff, 0xbe00, 0x003f, 0xffff, 0xffff, 0x7c00,
    0x007f, 0xffff, 0xffff, 0xbe00, 0x007f, 0xffff, 0xffff, 0xbe00,
    0x3fff, 0xf03c, 0x200f, 0xf004, 0x3e1f, 0x0f87, 0xc000, 0x0000,

    0x003f, 0xffff, 0xffff, 0x7c00, 0x001f, 0xffff, 0xfff8, 0xf800,
    0x003f, 0xffff, 0xffff, 0x7c00, 0x003f, 0xffff, 0xffff, 0x7c00,
    0x3fff, 0xf83c, 0x300f, 0xf00c, 0x7f3f, 0x9fcf, 0xe000, 0x03c0,

    0x001f, 0xffff, 0xfff8, 0xf800, 0x000f, 0xffff, 0xffe1, 0xf000,
    0x001f, 0xffff, 0xfff8, 0xf800, 0x001f, 0xffff, 0xfff8, 0xf800,
    0x3fff, 0xfc1c, 0x3807, 0xe01c, 0xffff, 0xfffe, 0xf000, 0x0df0,

    0x000f, 0xffff, 0xffe1, 0xf000, 0x0003, 0xffff, 0xff07, 0xa000,
    0x000f, 0xffff, 0xffe1, 0xf000, 0x000f, 0xffff, 0xffe1, 0xf000,
    0x1fff, 0xfe18, 0x1e03, 0xc078, 0x3fff, 0x3ff8, 0x3000, 0x13f8,

    0x0003, 0xffff, 0xff07, 0xa000, 0x0007, 0xffff, 0xf00e, 0x4000,
    0x0003, 0xffff, 0xff07, 0xa000, 0x0003, 0xffff, 0xff07, 0xa000,
    0x1fff, 0xff08, 0x1f80, 0x01f8, 0x8ffc, 0x7ff1, 0x1ad2, 0x2ffc,

    0x0007, 0xffff, 0xf00e, 0x4000, 0x00ab, 0xffff, 0xfc7d, 0xff00,
    0x0007, 0xffff, 0xf00e, 0x4000, 0x0007, 0xffff, 0xf00e, 0x4000,
    0x1fff, 0xff98, 0x1ff0, 0x0ff8, 0x3fff, 0x3ff8, 0x3000, 0x381c,

    0x00ab, 0xffff, 0xfc7d, 0xff00, 0x0155, 0xffff, 0xffe3, 0xd540,
    0x00ab, 0xffff, 0xfc7d, 0xff00, 0x00ab, 0xffff, 0xfc7d, 0xff00,
    0x0fff, 0xfff0, 0x0fff, 0xfff0, 0xffff, 0xfffe, 0xf000, 0x6186,

    0x0155, 0xffff, 0xffe3, 0xd540, 0x02aa, 0xff7f, 0xffcf, 0xaaa0,
    0x0155, 0xffff, 0xffe3, 0xd540, 0x0155, 0xffff, 0xffe3, 0xd540,
    0x0fff, 0xfff0, 0x0fff, 0xfff0, 0x7f3f, 0x9fcf, 0xe000, 0x42c2,

    0x02aa, 0xff7f, 0xffcf, 0xaaa0, 0x0555, 0x5f3f, 0xfc7f, 0x5550,
    0x02aa, 0xff7f, 0xffcf, 0xaaa0, 0x02aa, 0xff7f, 0xffcf, 0xaaa0,
    0x07ff, 0xff60, 0x07ff, 0xff60, 0x3e1f, 0x0f87, 0xc000, 0x43c2,

    0x0555, 0x5f3f, 0xfc7f, 0x5550, 0x0aaa, 0xaff3, 0xcffe, 0xaaa0,
    0x0555, 0x5f3f, 0xfc7f, 0x5550, 0x0555, 0x5f3f, 0xfc7f, 0x5550,
    0x03ff, 0xfcc0, 0x03ff, 0xfcc0, 0x30c3, 0x0c49, 0x2000, 0x6186,

    0x0aaa, 0xaff3, 0xcffe, 0xaaa0, 0x0555, 0x57f8, 0x1ff5, 0x5540,
    0x0aaa, 0xaff3, 0xcffe, 0xaaa0, 0x0aaa, 0xaff3, 0xcffe, 0xaaa0,
    0x07ff, 0xff68, 0x07ff, 0xff68, 0x79e7, 0x9e33, 0xd000, 0x381c,

    0x0555, 0x57f8, 0x1ff5, 0x5540, 0x02aa, 0xaaff, 0xffaa, 0xaa80,
    0x0555, 0x57f8, 0x1ff5, 0x5540, 0x0555, 0x57f8, 0x1ff5, 0x5540,
    0x15ff, 0xf9d4, 0x15ff, 0xf9d4, 0x3fcf, 0xf349, 0x2000, 0x3ffc,

    0x02aa, 0xaaff, 0xffaa, 0xaa80, 0x0055, 0x555f, 0xf555, 0x5500,
    0x02aa, 0xaaff, 0xffaa, 0xaa80, 0x02aa, 0xaaff, 0xffaa, 0xaa80,
    0x2adf, 0xe3aa, 0x2adf, 0xe3aa, 0x3fcf, 0xf366, 0x6600, 0x1ff8,

    0x0055, 0x555f, 0xf555, 0x5500, 0x000a, 0xaaaa, 0xaaaa, 0xa000,
    0x0055, 0x555f, 0xf555, 0x5500, 0x0055, 0x555f, 0xf555, 0x5500,
    0x157c, 0x3f54, 0x157c, 0x3f54, 0x79e7, 0x9e3c, 0xf900, 0x13d4,

    0x000a, 0xaaaa, 0xaaaa, 0xa000, 0x0000, 0x0555, 0x5550, 0x0000,
    0x000a, 0xaaaa, 0xaaaa, 0xa000, 0x000a, 0xaaaa, 0xaaaa, 0xa000,
    0x0aaf, 0xfaa8, 0x0aaf, 0xfaa8, 0x30c3, 0x0c3c, 0xf900, 0x2a2a,

    0x0000, 0x0555, 0x5550, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0555, 0x5550, 0x0000, 0x0000, 0x0555, 0x5550, 0x0000,
    0x0155, 0x5540, 0x0155, 0x5540, 0x0000, 0x0066, 0x6600, 0x1554};

static TokenId relativeTokens[NDIRECTION][NDIRECTION] = {
    {REAR, FRONT, RIGHT, LEFT}, /* me north, him [n, s, e, w] */
    {FRONT, REAR, LEFT, RIGHT}, /* me south, him [n, s, e, w] */
    {LEFT, RIGHT, REAR, FRONT}, /* me east ... */
    {RIGHT, LEFT, FRONT, REAR}  /* me west ... */
};
