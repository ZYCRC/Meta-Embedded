//
// Created by Youch on 4/25/2023.
//

#include "Gimbal_LG.h"


float Gimbal_LG::gimbal_feedback_angle[gimbal_motor_count] = {0};

float Gimbal_LG::gimbal_target_angle[gimbal_motor_count] = {0};

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
    gimbal_target_angle[Pitch] = target_pitch_angle;
}

void Gimbal_LG::set_gimbal_angle(float target_yaw_angle, float target_pitch_angle) {
    set_yaw_angle(target_yaw_angle);
    set_pitch_angle(target_pitch_angle);
}

void Gimbal_LG::get_yaw_feedback() {
    gimbal_feedback_angle[Yaw] = CANMotorIF::motor_feedback[CANMotorCFG::YAW].accumulate_angle();
}

void Gimbal_LG::get_pitch_feedback() {
    gimbal_feedback_angle[Pitch] = CANMotorIF::motor_feedback[CANMotorCFG::YAW].accumulate_angle();
}

void Gimbal_LG::get_gimbal_feedback() {
    get_yaw_feedback();
    get_pitch_feedback();
}
