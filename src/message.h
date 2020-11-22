#ifndef MESSAGE_H
#define MESSAGE_H

/* Manually calculated number of "settable" things */
#define PRESETS_NUM 110

/* Offsets from the beginning of the SYSEX message */
#define OFF_SYSEX_START            0 /* Exclusive message start: 0xf0 */
#define OFF_MF_ID                  1 /* Manufacturer ID: 0x47 (Akai)*/
#define OFF_ADDR                   2 /* Address/direction, 0x7f for sending, 0x00 from device */
#define OFF_PROD_ID                3 /* Product ID: 0x49 (MPKmini MK3)*/
#define OFF_COMMAND                4 /* Command, 0x64 send, 0x66 query, 0x67 data receive */
#define OFF_MSG_LEN                5 /* Two 7-bit values determinig the length of message */
#define OFF_PGM_NUM                7 /* Program number 0x0 is RAM */
#define OFF_PGM_NAME               8 /* Program name: 16 bytes of ASCII data */
#define OFF_PAD_MIDI_CH           24 /* Pad MIDI Channel, starts from 0 */
#define OFF_AFTERTOUCH            25 /* Aftertouch setting, 0x0 off, 0x1, channel, 0x2 polyphonic */
#define OFF_KEYBED_CH             26 /* Keybed channel (from 0) */
#define OFF_KEYBED_OCTAVE         27 /* Keybed octave, default 0x04 */
#define OFF_ARP_SWITCH            28 /* Arpeggiator, 0x0 off, 0x7f on */
#define OFF_ARP_MODE              29 /* Arpeggiatior mode, 0x0 - 0x05 */
#define OFF_ARP_DIVISION          30 /* Arpeggiator time division */
#define OFF_CLK_SOURCE            31 /* Clock source 0x0 internal 0x1 external */
#define OFF_LATCH                 32 /* Arpeggiator latch on/off */
#define OFF_ARP_SWING             33 /* Arpeggiator swing setting 0x0 is 50 %, 0x19 is 75 % (max) */
#define OFF_TEMPO_TAPS            34 /* Tempo taps (2, 3, 4) */
#define OFF_TEMPO_BPM             35 /* Two 7 bit values for BPM (60 - 240) */
#define OFF_ARP_OCTAVE            37 /* Arpeggiatior octave setting */
#define OFF_JOY_HORIZ_MODE        38 /* Joystick horizontal mode: 0x0 pitchbend, 0x1 single, 0x2 dual */
#define OFF_JOY_HORIZ_POSITIVE_CH 39 /* Joystick horizontal positive channel */
#define OFF_JOY_HORIZ_NEGATIVE_CH 40 /* Joystick horizontal positive channel */
#define OFF_JOY_VERT_MODE         41 /* Joystick vertical mode: 0x0 pitchbend, 0x1 single, 0x2 dual */
#define OFF_JOY_VERT_POSITIVE_CH  42 /* Joystick vertical positive channel */
#define OFF_JOY_VERT_NEGATIVE_CH  43 /* Joystick vertical positive channel */
#define OFF_PAD_1_NOTE            44 /* Pad 1 note */
#define OFF_PAD_1_PC              45 /* Pad 1 PC */
#define OFF_PAD_1_CC              46 /* Pad 1 CC */
#define OFF_PAD_2_NOTE            47 /* Pad 2 note */
#define OFF_PAD_2_PC              48 /* Pad 2 PC */
#define OFF_PAD_2_CC              49 /* Pad 2 CC */
#define OFF_PAD_3_NOTE            50 /* Pad 3 note */
#define OFF_PAD_3_PC              51 /* Pad 3 PC */
#define OFF_PAD_3_CC              52 /* Pad 3 CC */
#define OFF_PAD_4_NOTE            53 /* Pad 4 note */
#define OFF_PAD_4_PC              54 /* Pad 4 PC */
#define OFF_PAD_4_CC              55 /* Pad 4 CC */
#define OFF_PAD_5_NOTE            56 /* Pad 5 note */
#define OFF_PAD_5_PC              57 /* Pad 5 PC */
#define OFF_PAD_5_CC              58 /* Pad 5 CC */
#define OFF_PAD_6_NOTE            59 /* Pad 6 note */
#define OFF_PAD_6_PC              60 /* Pad 6 PC */
#define OFF_PAD_6_CC              61 /* Pad 6 CC */
#define OFF_PAD_7_NOTE            62 /* Pad 7 note */
#define OFF_PAD_7_PC              63 /* Pad 7 PC */
#define OFF_PAD_7_CC              64 /* Pad 7 CC */
#define OFF_PAD_8_NOTE            65 /* Pad 8 note */
#define OFF_PAD_8_PC              66 /* Pad 8 PC */
#define OFF_PAD_8_CC              67 /* Pad 8 CC */
#define OFF_PAD_9_NOTE            68 /* Pad 9 note */
#define OFF_PAD_9_PC              69 /* Pad 9 PC */
#define OFF_PAD_9_CC              70 /* Pad 9 CC */
#define OFF_PAD_10_NOTE           71 /* Pad 10 note */
#define OFF_PAD_10_PC             72 /* Pad 10 PC */
#define OFF_PAD_10_CC             73 /* Pad 10 CC */
#define OFF_PAD_11_NOTE           74 /* Pad 11 note */
#define OFF_PAD_11_PC             75 /* Pad 11 PC */
#define OFF_PAD_11_CC             76 /* Pad 11 CC */
#define OFF_PAD_12_NOTE           77 /* Pad 12 note */
#define OFF_PAD_12_PC             78 /* Pad 12 PC */
#define OFF_PAD_12_CC             79 /* Pad 12 CC */
#define OFF_PAD_13_NOTE           80 /* Pad 13 note */
#define OFF_PAD_13_PC             81 /* Pad 13 PC */
#define OFF_PAD_13_CC             82 /* Pad 13 CC */
#define OFF_PAD_14_NOTE           83 /* Pad 14 note */
#define OFF_PAD_14_PC             84 /* Pad 14 PC */
#define OFF_PAD_14_CC             85 /* Pad 14 CC */
#define OFF_PAD_15_NOTE           86 /* Pad 15 note */
#define OFF_PAD_15_PC             87 /* Pad 15 PC */
#define OFF_PAD_15_CC             88 /* Pad 15 CC */
#define OFF_PAD_16_NOTE           89 /* Pad 16 note */
#define OFF_PAD_16_PC             90 /* Pad 16 PC */
#define OFF_PAD_16_CC             91 /* Pad 16 CC */
#define OFF_KNOB_1_MODE           92 /* Knob 1 abs/rel */
#define OFF_KNOB_1_CC             93 /* Knob 1 CC */
#define OFF_KNOB_1_MIN            94 /* Knob 1 min value */
#define OFF_KNOB_1_MAX            95 /* Knob 1 max value */
#define OFF_KNOB_1_NAME           96 /* Knob 1 name (16 chars) */
#define OFF_KNOB_2_MODE          112 /* Knob 2 abs/rel */
#define OFF_KNOB_2_CC            113 /* Knob 2 CC */
#define OFF_KNOB_2_MIN           114 /* Knob 2 min value */
#define OFF_KNOB_2_MAX           115 /* Knob 2 max value */
#define OFF_KNOB_2_NAME          116 /* Knob 2 name (16 chars) */
#define OFF_KNOB_3_MODE          132 /* Knob 3 abs/rel */
#define OFF_KNOB_3_CC            133 /* Knob 3 CC */
#define OFF_KNOB_3_MIN           134 /* Knob 3 min value */
#define OFF_KNOB_3_MAX           135 /* Knob 3 max value */
#define OFF_KNOB_3_NAME          136 /* Knob 3 name (16 chars) */
#define OFF_KNOB_4_MODE          152 /* Knob 4 abs/rel */
#define OFF_KNOB_4_CC            153 /* Knob 4 CC */
#define OFF_KNOB_4_MIN           154 /* Knob 4 min value */
#define OFF_KNOB_4_MAX           155 /* Knob 4 max value */
#define OFF_KNOB_4_NAME          156 /* Knob 4 name (16 chars) */
#define OFF_KNOB_5_MODE          172 /* Knob 5 abs/rel */
#define OFF_KNOB_5_CC            173 /* Knob 5 CC */
#define OFF_KNOB_5_MIN           174 /* Knob 5 min value */
#define OFF_KNOB_5_MAX           175 /* Knob 5 max value */
#define OFF_KNOB_5_NAME          176 /* Knob 5 name (16 chars) */
#define OFF_KNOB_6_MODE          192 /* Knob 6 abs/rel */
#define OFF_KNOB_6_CC            193 /* Knob 6 CC */
#define OFF_KNOB_6_MIN           194 /* Knob 6 min value */
#define OFF_KNOB_6_MAX           195 /* Knob 6 max value */
#define OFF_KNOB_6_NAME          196 /* Knob 6 name (16 chars) */
#define OFF_KNOB_7_MODE          212 /* Knob 7 abs/rel */
#define OFF_KNOB_7_CC            213 /* Knob 7 CC */
#define OFF_KNOB_7_MIN           214 /* Knob 7 min value */
#define OFF_KNOB_7_MAX           215 /* Knob 7 max value */
#define OFF_KNOB_7_NAME          216 /* Knob 7 name (16 chars) */
#define OFF_KNOB_8_MODE          232 /* Knob 8 abs/rel */
#define OFF_KNOB_8_CC            233 /* Knob 8 CC */
#define OFF_KNOB_8_MIN           234 /* Knob 8 min value */
#define OFF_KNOB_8_MAX           235 /* Knob 8 max value */
#define OFF_KNOB_8_NAME          236 /* Knob 8 name (16 chars) */
#define OFF_TRANSPOSE            252 /* Transposition 0x0c is C4, +/-12 semitones */
#define OFF_SYSEX_END            253 /* SYSEX end, 0xf7 */

/* Shortcuts to the repetitve sections */
#define OFF_PAD_SETTINGS_START  OFF_PAD_1_NOTE
#define OFF_KNOB_SETTINGS_START OFF_KNOB_1_MODE
#define PADS_NUM        16
#define PADS_PER_BANK    8
#define PAD_SETTINGS_LEN 3
#define KNOBS_NUM        8
#define KNOB_SETTINGS_LEN (OFF_KNOB_2_MODE - OFF_KNOB_1_MODE)

/* Message constants */
#define SYSEX_START     0xf0
#define SYSEX_END       0xf7
#define MANUFACTURER_ID 0x47
#define PRODUCT_ID      0x49
#define DATA_MSG_LEN     254
#define MSG_PAYLOAD_LEN  246
#define QUERY_MSG_LEN      9
#define CHANNEL_MIN        0
#define CHANNEL_MAX      127

/* Number of programs */
#define PGM_NUM_RAM 0 /* Number of the temporary "RAM" program slot */
#define PGM_NUM_MAX 8

/* Message direction */
#define MSG_DIRECTION_OUT 0x7f
#define MSG_DIRECTION_IN  0x00

/* Command values */
#define CMD_WRITE_DATA    0x64
#define CMD_QUERY_DATA    0x66
#define CMD_INCOMING_DATA 0x67

/* Name (program, knob) string length */
#define NAME_STR_LEN 16

/* Aftertouch settings */
#define AFTERTOUCH_OFF        0x00
#define AFTERTOUCH_CHANNEL    0x01
#define AFTERTOUCH_POLYPHONIC 0x02

/* Keybed octave */
#define KEY_OCTAVE_MIN 0x00
#define KEY_OCTAVE_MAX 0x07

/* Arpeggiator settings */
#define ARP_ON  0x7f
#define ARP_OFF 0x00

#define ARP_OCTAVE_MIN 0x00
#define ARP_OCTAVE_MAX 0x03

#define ARP_MODE_UP 0x00
#define ARP_MODE_DOWN 0x01
#define ARP_MODE_EXCL 0x02
#define ARP_MODE_INCL 0x03
#define ARP_MODE_ORDER 0x04
#define ARP_MODE_RAND 0x05

#define ARP_DIV_1_4   0x00
#define ARP_DIV_1_4T  0x01
#define ARP_DIV_1_8   0x02
#define ARP_DIV_1_8T  0x03
#define ARP_DIV_1_16  0x04
#define ARP_DIV_1_16T 0x05
#define ARP_DIV_1_32  0x06
#define ARP_DIV_1_32T 0x07

#define ARP_LATCH_OFF 0x00
#define ARP_LATCH_ON  0x01

#define ARP_SWING_MIN 0x00
#define ARP_SWING_MAX 0x19
#define ARP_SWING_BASE  50

/* Clock settings */
#define CLK_INTERNAL 0x00
#define CLK_EXTERNAL 0x01

#define TEMPO_TAPS_MIN 2
#define TEMPO_TAPS_MAX 4

#define BPM_MIN 60
#define BPM_MAX 240

/* Joystick */
#define JOY_MODE_PITCHBEND 0x00
#define JOY_MODE_SINGLE    0x01
#define JOY_MODE_DUAL      0x02

#endif
