#include "gametank.h"
#include "drawing_funcs.h"
#include "input.h"
#include "persist.h"
#include "music.h"
#include "banking.h"
#include "dynawave.h"
#include "gen/assets/racing.h"
#include "gen/assets/sfx.h"


#define TRACK_PROGRESSION_END   0x306C

extern unsigned char tmp, tmp2, tmp3, tmp4;
extern unsigned char turn;
extern unsigned char car_x;
extern unsigned char test;
extern unsigned char track_offset;
extern unsigned char track_lengths;
extern unsigned char finish_line_offset;
extern unsigned char finish_line_offset_low;
extern unsigned char finish_line_start;
extern unsigned char finish_line_end;
extern unsigned char speed;
extern unsigned char speed_low;
extern unsigned char speed_high;
extern unsigned char landscape;
extern unsigned char landscape_low;
extern unsigned char end_track;
extern unsigned int track_progression;
extern unsigned char tire;
extern unsigned char drift_turn_offset;
extern unsigned char bump;
extern unsigned char explosion;

extern unsigned char other_car_speed[4];
extern unsigned char other_car_speed_max[4];
extern unsigned char other_car_track_id[4];
extern unsigned char other_car_track_id_long[4];
extern unsigned char other_car_track_offset[4];
extern unsigned char other_car_track_lengths[4];
extern unsigned int other_car_track_progression[4];
extern unsigned char other_car_lap[4];

extern unsigned char pitch_engine;
extern unsigned char pitch_engine_long;
extern unsigned char track_id_long;

extern unsigned char lap_total;
extern unsigned char lap;
extern unsigned char display_lap;

extern char track_id;
extern char *track_x_shift;
extern char *track_x_shift0;

extern char start;

#pragma zpsym ("tmp");
#pragma zpsym ("tmp2");
#pragma zpsym ("tmp3");
#pragma zpsym ("tmp4");
#pragma zpsym ("turn");
#pragma zpsym ("car_x");
#pragma zpsym ("test");
#pragma zpsym ("track_offset");
#pragma zpsym ("track_lengths");
#pragma zpsym ("finish_line_offset");
#pragma zpsym ("finish_line_offset_low");
#pragma zpsym ("finish_line_start");
#pragma zpsym ("finish_line_end");
#pragma zpsym ("speed");
#pragma zpsym ("speed_low");
#pragma zpsym ("speed_high");
#pragma zpsym ("landscape");
#pragma zpsym ("landscape_low");
#pragma zpsym ("end_track");
#pragma zpsym ("track_progression");
#pragma zpsym ("tire");
#pragma zpsym ("drift_turn_offset");
#pragma zpsym ("bump");
#pragma zpsym ("explosion");
#pragma zpsym ("other_car_speed");
#pragma zpsym ("other_car_speed_max");
#pragma zpsym ("other_car_track_id");
#pragma zpsym ("other_car_track_id_long");
#pragma zpsym ("other_car_track_offset");
#pragma zpsym ("other_car_track_lengths");
#pragma zpsym ("other_car_track_progression");
#pragma zpsym ("other_car_lap");
#pragma zpsym ("pitch_engine");
#pragma zpsym ("pitch_engine_long");
#pragma zpsym ("track_id_long");
#pragma zpsym ("track_id");
#pragma zpsym ("track_x_shift");
#pragma zpsym ("track_x_shift0");
#pragma zpsym ("lap_total");
#pragma zpsym ("lap");
#pragma zpsym ("display_lap");
#pragma zpsym ("start");

extern unsigned char track[47];
extern char tracks[20][64];
extern const char finish_offset_start[200];
extern const char finish_offset_end[200];
extern char track_width[64];
extern char track_x_shifts[9][64];
extern unsigned char track_map_x[98];
extern unsigned char track_map_y[98];
extern const char drift[27];

unsigned char start1;
unsigned char start2;
unsigned char start3;

#pragma code-name(push, "PROG0")

#define DRAW_LINE(line) \
    if (line < finish_line_start) tmp2 = 0; \
    else if (line == finish_line_end) tmp2 = 1; \
    rect.x=track_x_shift[line];rect.y=line+32;rect.w=track_width[line];rect.gx=track_x_shift0[line];rect.gy=tracks[track_id][line];rect.b=tmp2;draw_sprite_rect();

void breakpoint();

int main_loop() {
        draw_sprite(landscape, 0, 127-landscape, 32, 0, 0, 2);      // Draw landscape
        draw_sprite(0, 0, landscape, 32, 127-landscape, 0, 2);
        draw_box(0, 32, 127, 64, 253);                               // Draw grass
        draw_box(48, 96, 80, 32, 0);

        tmp2 = 0;
        if (end_track) {
            finish_line_end = finish_offset_end[finish_line_offset] - 1;
            finish_line_start = finish_offset_start[finish_line_offset];

            finish_line_offset += speed_high;
            finish_line_offset_low += speed_low;
            if (finish_line_offset_low & 8) {
                finish_line_offset++;
                finish_line_offset_low -= 8;
            }

            if (finish_line_offset >= 200) {
                end_track = 0;
                finish_line_end = 100;
                finish_line_start = 100;
                track_offset = 0;
                track_progression = 0;
                tmp = track[track_offset];
                track_lengths = tmp & 63;

                if (lap == lap_total) {
                    tmp = 1;
                    if (lap < other_car_lap[0]) { tmp++; }
                    if (lap < other_car_lap[1]) { tmp++; }
                    if (lap < other_car_lap[2]) { tmp++; }
                    if (lap < other_car_lap[3]) { tmp++; }

                    draw_sprite(37, 64, 48, 9, 0, 91, 3);       // Display "Position:"
                    draw_sprite(85, 64, 6, 9, 6*tmp, 64, 3);    // Display the position
                    await_draw_queue();
                    sleep(1);
                    flip_pages();
                    draw_sprite(37, 64, 48, 9, 0, 91, 3);
                    draw_sprite(85, 64, 6, 9, 6*tmp, 64, 3);
                    await_draw_queue();
                    sleep(1);
                    flip_pages();
                    while (1) {
                        stop_music();
                        update_inputs();
                        if (player1_buttons & (INPUT_MASK_A | INPUT_MASK_B | INPUT_MASK_START)) {
                            while (1) {
                                update_inputs();
                                if (!(player1_buttons & (INPUT_MASK_A | INPUT_MASK_B | INPUT_MASK_START))) {
                                    break;
                                }
                            }
                            return 1;
                        }
                    }
                }
                lap++;
                display_lap = 120;
            }
        }

        // Draw track
//        rect.x=X;rect.y=Y;rect.w=W;rect.h=H;rect.gx=GX;rect.gy=GY;rect.b=SPRITESLOT;queue_draw_sprite_rect();
        rect.h=1;
/*        for (tmp=63; tmp!=255; tmp--) {
            if (tmp < finish_line_start) tmp2 = 0;
            else if (tmp == finish_line_end) tmp2 = 1;
            rect.x=track_x_shift[tmp];rect.y=tmp+32;rect.w=track_width[tmp];rect.gx=track_x_shift0[tmp];rect.gy=tracks[track_id][tmp];rect.b=tmp2;queue_draw_sprite_rect();
//            queue_draw_sprite(track_x_shift[tmp], tmp+32, track_width[tmp], 1, track_x_shift0[tmp], tracks[track_id][tmp], tmp2);
        }
*/


        DRAW_LINE(63)
        DRAW_LINE(62)
        DRAW_LINE(61)
        DRAW_LINE(60)
        DRAW_LINE(59)
        DRAW_LINE(58)
        DRAW_LINE(57)
        DRAW_LINE(56)
        DRAW_LINE(55)
        DRAW_LINE(54)
        DRAW_LINE(53)
        DRAW_LINE(52)
        DRAW_LINE(51)
        DRAW_LINE(50)
        DRAW_LINE(49)
        DRAW_LINE(48)
        DRAW_LINE(47)
        DRAW_LINE(46)
        DRAW_LINE(45)
        DRAW_LINE(44)
        DRAW_LINE(43)
        DRAW_LINE(42)
        DRAW_LINE(41)
        DRAW_LINE(40)
        DRAW_LINE(39)
        DRAW_LINE(38)
        DRAW_LINE(37)
        DRAW_LINE(36)
        DRAW_LINE(35)
        DRAW_LINE(34)
        DRAW_LINE(33)
        DRAW_LINE(32)
        DRAW_LINE(31)
        DRAW_LINE(30)
        DRAW_LINE(29)
        DRAW_LINE(28)
        DRAW_LINE(27)
        DRAW_LINE(26)
        DRAW_LINE(25)
        DRAW_LINE(24)
        DRAW_LINE(23)
        DRAW_LINE(22)
        DRAW_LINE(21)
        DRAW_LINE(20)
        DRAW_LINE(19)
        DRAW_LINE(18)
        DRAW_LINE(17)
        DRAW_LINE(16)
        DRAW_LINE(15)
        DRAW_LINE(14)
        DRAW_LINE(13)
        DRAW_LINE(12)
        DRAW_LINE(11)
        DRAW_LINE(10)
        DRAW_LINE(9)
        DRAW_LINE(8)
        DRAW_LINE(7)
        DRAW_LINE(6)
        DRAW_LINE(5)
        DRAW_LINE(4)
        DRAW_LINE(3)
        DRAW_LINE(2)
        DRAW_LINE(1)
        DRAW_LINE(0)

        // Draw other cars
        if (other_car_track_progression[3] >= track_progression && other_car_track_progression[3] < track_progression + 55) {
            tmp = other_car_track_progression[3] - track_progression;
            tmp2 = tmp & 0xF0;
            tmp4 = 63 - tmp;
            tmp3 = track_x_shift[tmp4];
            tmp3 += 32;
            tmp3 -= track_x_shifts[4][tmp4];
            tmp = 80 - tmp;
            draw_sprite(32+tmp3, tmp, 32, 16, 96, 32+tmp2, 2);
            if ((tmp == 79 || tmp == 80) &&
                other_car_speed[3] < speed &&
                car_x > 0x44) {
                explosion = 63;
            }
        }
        if (other_car_track_progression[2] >= track_progression && other_car_track_progression[2] < track_progression + 55) {
            tmp = other_car_track_progression[2] - track_progression;
            tmp2 = tmp & 0xF0;
            tmp4 = 63 - tmp;
            tmp3 = track_x_shift[tmp4];
            tmp3 += 32;
            tmp3 -= track_x_shifts[4][tmp4];
            tmp = 80 - tmp;
            draw_sprite(tmp3, tmp, 32, 16, 64, 32+tmp2, 2);
            if ((tmp == 79 || tmp == 80) &&
                other_car_speed[2] < speed &&
                car_x < 0x74) {
                explosion = 63;
            }
        }
        if (other_car_track_progression[1] >= track_progression && other_car_track_progression[1] < track_progression + 55) {
            tmp = other_car_track_progression[1] - track_progression;
            tmp2 = tmp & 0xF0;
            tmp4 = 63 - tmp;
            tmp3 = track_x_shift[tmp4];
            tmp3 += 32;
            tmp3 -= track_x_shifts[4][tmp4];
            tmp = 80 - tmp;
            draw_sprite(32+tmp3, tmp, 32, 16, 32, 32+tmp2, 2);
            if ((tmp == 79 || tmp == 80) &&
                other_car_speed[1] < speed &&
                car_x > 0x44) {
                explosion = 63;
            }
        }
        if (other_car_track_progression[0] >= track_progression && other_car_track_progression[0] < track_progression + 55) {
            tmp = other_car_track_progression[0] - track_progression;
            tmp2 = tmp & 0xF0;
            tmp4 = 63 - tmp;
            tmp3 = track_x_shift[tmp4];
            tmp3 += 32;
            tmp3 -= track_x_shifts[4][tmp4];
            tmp = 80 - tmp;
            draw_sprite(tmp3, tmp, 32, 16, 0, 32+tmp2, 2);
            if ((tmp == 79 || tmp == 80) &&
                other_car_speed[0] < speed &&
                car_x < 0x74) {
                explosion = 63;
            }
        }

        // Draw player's car
        tmp3 = car_x >> 1;
        if (bump && (track_id & 1)) {
            draw_sprite(tmp3, 79, 32, 16, ((track_id&3) << 5), 96, 2);
        } else {
            draw_sprite(tmp3, 80, 32, 16, ((track_id&3) << 5), 96, 2);
        }

        if (explosion) {
            tmp = ((explosion & 0xF8)) << 2;
            if (tmp >= 128) {
                tmp -= 128;
                tmp2 = 48;
            } else {
                tmp2 = 32;
            }
            draw_sprite(tmp3, 79, 32, 16, tmp, tmp2, 3);
            if (speed != 1) {
                speed--;
            }
            explosion--;
            if (explosion && pitch_engine_long) {
                pitch_engine_long--;
            } else {
                pitch_engine_long = 0;
            }
            pitch_engine = pitch_engine_long >> 2;

        }

        if (display_lap) {
            display_lap--;
            if (lap == 1) {
                if (!start) {
                    draw_sprite(36, 40, 16, 16, 48, 16, 3);
                    draw_sprite(56, 40, 16, 16, 48, 16, 3);
                    draw_sprite(76, 40, 16, 16, 48, 16, 3);
                }
            }
            else if (display_lap & 8) {
                if (lap == lap_total) {
                    draw_sprite(38, 64, 51, 9, 0, 82, 3);
                } else {
                    draw_sprite(50, 64, 18,9, 0, 73, 3);
                    draw_sprite(75, 64, 6, 9, lap*6, 64, 3);
                }
            }
        }

        // Draw speedometer
        draw_sprite(64, 109, speed, 6, 80, 0, 3);
        // Draw track map
        draw_sprite(0, 96, 48, 32, 0, 0, 3);
        tmp = *((unsigned char *)&other_car_track_progression+1) << 1;
        if (*((unsigned char *)&other_car_track_progression) & 0x80) { tmp++; }
        draw_sprite(track_map_x[tmp], track_map_y[tmp], 5, 5, 64, 0, 3);
        tmp = *((unsigned char *)&other_car_track_progression+3) << 1;
        if (*((unsigned char *)&other_car_track_progression+2) & 0x80) { tmp++; }
        draw_sprite(track_map_x[tmp], track_map_y[tmp], 5, 5, 72, 0, 3);
        tmp = *((unsigned char *)&other_car_track_progression+5) << 1;
        if (*((unsigned char *)&other_car_track_progression+4) & 0x80) { tmp++; }
        draw_sprite(track_map_x[tmp], track_map_y[tmp], 5, 5, 64, 8, 3);
        tmp = *((unsigned char *)&other_car_track_progression+7) << 1;
        if (*((unsigned char *)&other_car_track_progression+6) & 0x80) { tmp++; }
        draw_sprite(track_map_x[tmp], track_map_y[tmp], 5, 5, 72, 8, 3);
        tmp = (*((unsigned char *)&track_progression+1)) << 1;
        if (*((unsigned char *)&track_progression) & 0x80) { tmp++; };
        draw_sprite(track_map_x[tmp], track_map_y[tmp], 5, 5, 48, 0, 3);

        // Display starting blocks (no car movement)
        if (start) {
            if (start == 200) {
                start1 = 64;
                start2 = 64;
                start3 = 64;
            } else if (start == 161) {
//                play_sound_effect(&ASSET__sfx__start1_bin , 1);
                start1 = 48;
            } else if (start == 81) {
//                play_sound_effect(&ASSET__sfx__start1_bin , 1);
                start2 = 48;
            } else if (start == 1) {
//                play_sound_effect(&ASSET__sfx__start2_bin , 1);
                start3 = 48;
            }
            draw_sprite(36, 40, 16, 16, start1, 16, 3);
            draw_sprite(56, 40, 16, 16, start2, 16, 3);
            draw_sprite(76, 40, 16, 16, start3, 16, 3);
            start--;
        } else {
            // Steering
            update_inputs();
            if(player1_buttons & INPUT_MASK_LEFT) {
                car_x--;
                car_x--;
            } else if (player1_buttons & INPUT_MASK_RIGHT) {
                car_x++;
                car_x++;
            }

            // Compute the drift
            tmp = speed_high >> 1;
            car_x += drift[drift_turn_offset + tmp];

            // If the car hits the side of the road
            if (car_x <= 30) {
                car_x = 30;
                if (speed > 2) {
                    speed-=2;
                    speed_high = speed >> 3;
                    speed_low = speed & 7;
                    pitch_engine_long-=2;
                    pitch_engine = pitch_engine_long >> 2;
                }
                bump = 1;
            } else if (car_x >= 180) {
                car_x = 180;
                if (speed > 2) {
                    speed-=2;
                    speed_high = speed >> 3;
                    speed_low = speed & 7;
                    pitch_engine_long-=2;
                    pitch_engine = pitch_engine_long >> 2;
                }
                bump = 1;
            } else {
                bump = 0;
            }

            // Player input (acceleration and deceleration)
            if((player1_buttons & INPUT_MASK_UP) && !bump && !explosion) {
                if (speed != 32) {
                    speed++;
                    speed_high = speed >> 3;
                    speed_low = speed & 7;
                    pitch_engine_long++;
                    pitch_engine = pitch_engine_long >> 2;
                }
            } else if(player1_buttons & INPUT_MASK_DOWN) {
                if (speed != 1) {
                    speed--;
                    speed_high = speed >> 3;
                    speed_low = speed & 7;
                    pitch_engine_long--;
                    pitch_engine = pitch_engine_long >> 2;
                }
            }

            // landscape += (turn >> 2) * speed / 32
            if (!(turn & 4)) {
                tmp = (4 - turn) * speed;
                tmp2 = tmp >> 7;
                tmp3 = tmp & 127;
                landscape_low += tmp3;
                if (landscape_low & 0x80) {
                    landscape_low -= 128;
                    landscape++;
                }
                landscape += tmp2;
            } else if (turn == 4) {

            } else {
                tmp = (turn-4) * speed;
                tmp2 = tmp >> 7;
                tmp3 = tmp & 127;
                landscape_low += tmp3;
                if (landscape_low & 0x80) {
                    landscape_low -= 128;
                    landscape--;
                }
                landscape -= tmp2;
            }
            if (landscape & 0x80) landscape -= 128;
            
            // Change the track progression
            track_id_long += speed;
            tmp = track_id;
            track_id = track_id_long >> 3;
            tmp = track_id - tmp;
            track_progression += tmp;
            if (track_id >= 20) {
                track_id -= 20;
                track_id_long -= 160;
                track_lengths--;
                if (!track_lengths) {
                    track_offset++;
                    tmp = track[track_offset];
                    if (tmp == 255) {
                        end_track = 1;
                        finish_line_offset = 0;
                        finish_line_offset_low = 0;
                    } else {
                        track_lengths = tmp & 63;
                        if (tmp & 0x80) {
                            turn--;
                            drift_turn_offset -= 3;
                        }
                        else if (tmp & 0x40) {
                            turn++;
                            drift_turn_offset += 3;
                        }
                        track_x_shift = (char *)track_x_shifts[turn];
                    }
                }
            }

            // Change the track progression for the other cars
            if (other_car_speed[0] < other_car_speed_max[0]) { other_car_speed[0]++; }
            other_car_track_id_long[0] += other_car_speed[0];
            tmp = other_car_track_id[0];
            other_car_track_id[0] = other_car_track_id_long[0] >> 3;
            tmp = other_car_track_id[0] - tmp;
            other_car_track_progression[0] += tmp;
            if (other_car_track_id[0] >= 20) {
                other_car_track_id[0] -= 20;
                other_car_track_id_long[0] -= 160;
            }
            if (other_car_track_progression[0] >= TRACK_PROGRESSION_END) {
                other_car_track_progression[0] = 0;
                other_car_lap[0]++;
            }

            if (other_car_speed[1] < other_car_speed_max[1]) { other_car_speed[1]++; }
            other_car_track_id_long[1] += other_car_speed[1];
            tmp = other_car_track_id[1];
            other_car_track_id[1] = other_car_track_id_long[1] >> 3;
            tmp = other_car_track_id[1] - tmp;
            other_car_track_progression[1] += tmp;
            if (other_car_track_id[1] >= 20) {
                other_car_track_id[1] -= 20;
                other_car_track_id_long[1] -= 160;
            }
            if (other_car_track_progression[1] >= TRACK_PROGRESSION_END) {
                other_car_track_progression[1] = 0;
                other_car_lap[1]++;
            }
            
            if (other_car_speed[2] < other_car_speed_max[2]) { other_car_speed[2]++; }
            other_car_track_id_long[2] += other_car_speed[2];
            tmp = other_car_track_id[2];
            other_car_track_id[2] = other_car_track_id_long[2] >> 3;
            tmp = other_car_track_id[2] - tmp;
            other_car_track_progression[2] += tmp;
            if (other_car_track_id[2] >= 20) {
                other_car_track_id[2] -= 20;
                other_car_track_id_long[2] -= 160;
            }
            if (other_car_track_progression[2] >= TRACK_PROGRESSION_END) {
                other_car_track_progression[2] = 0;
                other_car_lap[2]++;
            }
            
            if (other_car_speed[3] < other_car_speed_max[3]) { other_car_speed[3]++; }
            other_car_track_id_long[3] += other_car_speed[3];
            tmp = other_car_track_id[3];
            other_car_track_id[3] = other_car_track_id_long[3] >> 3;
            tmp = other_car_track_id[3] - tmp;
            other_car_track_progression[3] += tmp;
            if (other_car_track_id[3] >= 20) {
                other_car_track_id[3] -= 20;
                other_car_track_id_long[3] -= 160;
            }
            if (other_car_track_progression[3] >= TRACK_PROGRESSION_END) {
                other_car_track_progression[3] = 0;
                other_car_lap[3]++;
            }
        }

        await_draw_queue();
        PROFILER_END(0);
        sleep(1);
        flip_pages();
        PROFILER_START(0);
        return 0;
}

#pragma code-name (pop)
