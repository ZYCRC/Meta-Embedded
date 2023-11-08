//
// Created by Youch on 4/25/2023.
//

#include "Gimbal_LG.h"

#define REGULAR_DIST 3
#define FOV 66

float Gimbal_LG::vision_pos[2] = {0};
int Gimbal_LG::pull_permit = 0;
int Gimbal_LG::shoot_permit = 0;
int Gimbal_LG::pull_request = 0;
int Gimbal_LG::pull_flag = 0;
int Gimbal_LG::shoot_flag = 0;
int Gimbal_LG::pull_feedback = 0;
int Gimbal_LG::shoot_feedback = 0;
int Gimbal_LG::finish_shoot = 0;
int Gimbal_LG::shoot_last_cmd = 0;
int Gimbal_LG::pull_last_cmd = 0;
int Gimbal_LG::gun_inpos = 0;

void Gimbal_LG::start(tprio_t thread_prio) {
    /* set the init angle both to 90 deg */

}

void Gimbal_LG::set_yaw_angle(float target_yaw_angle) {
    Gimbal_SKD::set_yaw_angle(target_yaw_angle);
}

void Gimbal_LG::set_pitch_angle(float target_pitch_angle) {
    Gimbal_SKD::set_pitch_angle(target_pitch_angle);
}

void Gimbal_LG::set_gimbal_angle(float target_yaw_angle, float target_pitch_angle) {
    set_yaw_angle(target_yaw_angle);
    set_pitch_angle(target_pitch_angle);
}

void Gimbal_LG::get_yaw_feedback() {
    Gimbal_SKD::get_yaw_feedback();
}

float Gimbal_LG::get_pitch_feedback() {
    return Gimbal_SKD::get_pitch_feedback();
}

void Gimbal_LG::get_gimbal_feedback() {
    get_yaw_feedback();
    get_pitch_feedback();
}

void Gimbal_LG::cal_gimbal_angle() {
    float target_yaw_angle = 0;
    float target_pitch_angle = 0;
    double offsetX = 0;
    double offsetY = 0;

    offsetX = (vision_pos[0] - 0.5) * 2.85;
    offsetY = (vision_pos[1] - 0.5) * 2.14;

    target_yaw_angle = - atan2(offsetX, REGULAR_DIST) * 180 / 3.1415926;
    target_pitch_angle = - atan2(offsetY, REGULAR_DIST) * 180 / 3.1415926;
//    Shell::printf("TargetX: %.2f, TargetY: %.2f\n", target_yaw_angle, target_pitch_angle);
    set_yaw_angle(target_yaw_angle);
    set_pitch_angle(target_pitch_angle);
}

void Gimbal_LG::rec_gimbal_angle(float targetX, float targetY) {
    vision_pos[0] = targetX;
    vision_pos[1] = targetY;
}

void Gimbal_LG::init_gun_pos() {
    set_pitch_angle(-45);
    set_yaw_angle(0);
}

void Gimbal_LG::set_lift_vel(float target_vel) {
    Gimbal_SKD::set_lift_vel(target_vel);
}

void Gimbal_LG::pull_gun() {
    Gimbal_LG::set_lift_vel(-1000);

}

void Gimbal_LG::set_pull_permit(int pull) {
//    if (pull == 1) {
//        pull_request ++;
//    }
//    else {
//        pull_permit = 0;
//        pull_request = 0;
//    }
//
//    if (pull_request > 3) {
//        pull_permit = 1;
//    }
    pull_permit = pull;
    if (pull_permit == 0 && pull_flag == 2) {
        pull_flag = 0;
        pull_permit = 0;
//        pull_feedback = 0;
    }
}

void Gimbal_LG::set_shoot(int shoot) {
    shoot_permit = shoot;
    if (shoot_permit == 0 && shoot_flag == 2) {
        shoot_flag = 0;
        shoot_permit = 0;
//        shoot_feedback = 0;
    }
}

int Gimbal_LG::get_shoot_stats() {
    return shoot_permit;
}

int Gimbal_LG::get_pull_stats() {
    return pull_permit;
}

void Gimbal_LG::shoot() {
    set_yaw_angle(45);
    set_pitch_angle(0);
}