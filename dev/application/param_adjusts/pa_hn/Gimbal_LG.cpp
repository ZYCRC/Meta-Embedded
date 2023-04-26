//
// Created by Youch on 4/25/2023.
//

#include "Gimbal_LG.h"

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

void Gimbal_LG::get_pitch_feedback() {
    Gimbal_SKD::get_pitch_feedback();
}

void Gimbal_LG::get_gimbal_feedback() {
    get_yaw_feedback();
    get_pitch_feedback();
}
