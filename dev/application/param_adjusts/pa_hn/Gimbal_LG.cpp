//
// Created by Youch on 4/25/2023.
//

#include "Gimbal_LG.h"

void Gimbal_LG::start(tprio_t thread_prio) {
    /* set the init angle both to 90 deg */
    for(int i; i < gimbal_motor_count; i ++){
        gimbal_target_angle[i] = 90;
    }
}

void Gimbal_LG::set_yaw_angle(float target_yaw_angle) {
    gimbal_target_angle[Yaw] = target_yaw_angle;
}

void Gimbal_LG::set_pitch_angle(float target_pitch_angle) {
    gimbal_target_angle[pitch] = target_pitch_angle;
}

void Gimbal_LG::set_gimbal_angle(float target_yaw_angle, float target_pitch_angle) {
    set_yaw_angle(target_yaw_angle);
    set_pitch_angle(target_pitch_angle);
}
