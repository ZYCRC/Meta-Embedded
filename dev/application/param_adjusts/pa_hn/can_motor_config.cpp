//
// Created by 钱晨 on 11/14/21.
//

#include "can_motor_config.h"

CANMotorBase CANMotorCFG::CANMotorProfile[MOTOR_COUNT] = {
        {CANMotorBase::can_channel_1, 0x205, CANMotorBase::GM6020, 3572},
        {CANMotorBase::can_channel_1, 0x206, CANMotorBase::GM6020, 3572}
};

PIDController::pid_params_t CANMotorCFG::a2vParams[MOTOR_COUNT] = {
        {15, 0.0f, 1, 000, 720},
        {25, 0.0f, 1600, 000, 720}
};

PIDController::pid_params_t CANMotorCFG::v2iParams[MOTOR_COUNT] = {
        {15.0f,  0.15f, 0.0f, 5000.0f, 30000.0f},
        {25.0f,  0.15f, 0.0f, 5000.0f, 30000.0f}
};

bool CANMotorCFG::enable_a2v[MOTOR_COUNT] {
        true,
        true
};

bool CANMotorCFG::enable_v2i[MOTOR_COUNT] {
        true,
        false
};