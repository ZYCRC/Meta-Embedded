//
// Created by 钱晨 on 11/14/21.
//

#include "can_motor_config.h"

CANMotorBase CANMotorCFG::CANMotorProfile[MOTOR_COUNT] = {
        {CANMotorBase::can_channel_1, 0x205, CANMotorBase::GM6020, 6886},
        {CANMotorBase::can_channel_1, 0x206, CANMotorBase::GM6020, 3359},
        {CANMotorBase::can_channel_1, 0x201, CANMotorBase::M3508, 3572}
};

PIDController::pid_params_t CANMotorCFG::a2vParams[MOTOR_COUNT] = {
        {100, 0.0f, 200, 000, 720},
        {60, 0.0f, 50,  000, 720},
        {10, 0.0f, 0.2,  100, 500},
};

PIDController::pid_params_t CANMotorCFG::v2iParams[MOTOR_COUNT] = {
        {25.0f,  0.15f, 0.0f, 5000.0f, 30000.0f},
        { 25.0f, 1.0f, 0.00f, 5000.0f, 30000.0f},
        {26.0f,0.1f,0.02f,2000.0,16000.0}
};

bool CANMotorCFG::enable_a2v[MOTOR_COUNT] {
        true,
        true,
        false
};

bool CANMotorCFG::enable_v2i[MOTOR_COUNT] {
        true,
        true,
        true
};