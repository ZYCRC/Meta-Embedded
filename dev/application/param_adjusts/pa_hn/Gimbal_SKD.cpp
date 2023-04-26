//
// Created by HTY是猪 on 2023/4/25.
//

#include "Gimbal_SKD.h"

float Gimbal_SKD::gimbal_target_angle[gimbal_motor_count] = {0};
float Gimbal_SKD::gimbal_feedback_angle[gimbal_motor_count] = {0};
Gimbal_SKD::SKDThread Gimbal_SKD::skd_thread;

void Gimbal_SKD::start(tprio_t thread_prio) {
    /* set the init angle both to 90 deg */
    for(int i; i < gimbal_motor_count; i ++){
        gimbal_target_angle[i] = 90;
    }

    skd_thread.start(thread_prio);
}

void Gimbal_SKD::set_yaw_angle(float target_yaw_angle) {
    gimbal_target_angle[Yaw] = target_yaw_angle;
}

void Gimbal_SKD::set_pitch_angle(float target_pitch_angle) {
    gimbal_target_angle[Pitch] = target_pitch_angle;
}

void Gimbal_SKD::set_gimbal_angle(float target_yaw_angle, float target_pitch_angle) {
    set_yaw_angle(target_yaw_angle);
    set_pitch_angle(target_pitch_angle);
}

void Gimbal_SKD::get_yaw_feedback() {
    gimbal_feedback_angle[Yaw] = CANMotorIF::motor_feedback[CANMotorCFG::YAW].accumulate_angle();
}

void Gimbal_SKD::get_pitch_feedback() {
    gimbal_feedback_angle[Pitch] = CANMotorIF::motor_feedback[CANMotorCFG::YAW].accumulate_angle();
}

void Gimbal_SKD::get_gimbal_feedback() {
    get_yaw_feedback();
    get_pitch_feedback();
}

void Gimbal_SKD::SKDThread::main() {
    setName("Gimbal_SKD");
    while (!shouldTerminate()) {

        chSysLock();  /// S-Lock State
        {
            CANMotorController::set_target_angle(CANMotorCFG::YAW, gimbal_target_angle[Gimbal_SKD::Yaw]);
            CANMotorController::set_target_angle(CANMotorCFG::PITCH, gimbal_target_angle[Gimbal_SKD::Pitch]);
        }
        chSysUnlock();  /// EXIT S-Locked state

        sleep(TIME_MS2I(SKD_THREAD_INTERVAL));
    }
}
