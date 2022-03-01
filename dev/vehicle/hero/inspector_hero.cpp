//
// Created by liuzikai on 2019-06-25.
// Edited by Qian Chen & Mo Kanya on 2019-07-05
//

#include "inspector_hero.h"

AbstractAHRS *InspectorH::ahrs = nullptr;
CANInterface *InspectorH::can1 = nullptr;
CANInterface *InspectorH::can2 = nullptr;

bool InspectorH::gimbal_failure_ = false;
bool InspectorH::chassis_failure_ = false;
bool InspectorH::remote_failure_ = false;

InspectorH::InspectorThread InspectorH::inspectorThread;

void InspectorH::init(CANInterface *can1_, CANInterface *can2_, AbstractAHRS *ahrs_) {
    can1 = can1_;
    can2 = can2_;
    ahrs = ahrs_;
}

void InspectorH::start_inspection(tprio_t thread_prio) {
    inspectorThread.start(thread_prio);
}

void InspectorH::startup_check_can() {
    time_msecs_t t = SYSTIME;
    while (WITHIN_RECENT_TIME(t, 100)) {
        if (WITHIN_RECENT_TIME(can1->last_error_time, 5)) {  // can error occurs
            t = SYSTIME;  // reset the counter
        }
        if (WITHIN_RECENT_TIME(can2->last_error_time, 5)) {  // can error occurs
            t = SYSTIME;  // reset the counter
        }
        chThdSleepMilliseconds(5);
    }
}

void InspectorH::startup_check_mpu() {
    time_msecs_t t = SYSTIME;
    while (WITHIN_RECENT_TIME(t, 20)) {
        if (not WITHIN_RECENT_TIME(ahrs->get_mpu_update_time(), 5)) {
            // No signal in last 5 ms (normal interval 1 ms for on-board MPU)
            t = SYSTIME;  // reset the counter
        }
        chThdSleepMilliseconds(5);
    }
}

void InspectorH::startup_check_ist() {
    time_msecs_t t = SYSTIME;
    while (WITHIN_RECENT_TIME(t, 20)) {
        if (not WITHIN_RECENT_TIME(ahrs->get_ist_update_time(), 5)) {
            // No signal in last 5 ms (normal interval 1 ms for on-board MPU)
            t = SYSTIME;  // reset the counter
        }
        chThdSleepMilliseconds(5);
    }
}

void InspectorH::startup_check_remote() {
    time_msecs_t t = SYSTIME;
    while (WITHIN_RECENT_TIME(t, 50)) {
        if (not WITHIN_RECENT_TIME(Remote::last_update_time, 25)) {  // No signal in last 25 ms (normal interval 7 ms)
            t = SYSTIME;  // reset the counter
        }
        chThdSleepMilliseconds(15);
    }
}

void InspectorH::startup_check_chassis_feedback() {
    time_msecs_t t = SYSTIME;
    while (WITHIN_RECENT_TIME(t, 20)) {
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::FR].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Chassis FR offline.");
            t = SYSTIME;  // reset the counter
        }
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::FL].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Chassis FL offline.");
            t = SYSTIME;  // reset the counter
        }
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::BL].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Chassis BL offline.");
            t = SYSTIME;  // reset the counter
        }
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::BR].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Chassis BR offline.");
            t = SYSTIME;  // reset the counter
        }
        chThdSleepMilliseconds(5);
    }
}

void InspectorH::startup_check_gimbal_feedback() {
    time_msecs_t t = SYSTIME;
    while (WITHIN_RECENT_TIME(t, 20)) {
//        if(Referee::bullet_remaining.bullet_remaining_num_17mm > 0) {
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::FW_DOWN].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Gimbal FW_LEFT offline.");
            t = SYSTIME;  // reset the counter
        }
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::FW_UP].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Gimbal FW_RIGHT offline.");
            t = SYSTIME;  // reset the counter
        }
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::BULLET_LOADER].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Gimbal Bullet offline.");
            t = SYSTIME;  // reset the counter
        }
//        }

        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::YAW].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Gimbal Yaw offline.");
            t = SYSTIME;  // reset the counter
        }
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::PITCH].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Gimbal Pitch offline.");
            t = SYSTIME;  // reset the counter
        }
        if (not WITHIN_RECENT_TIME(
                CANMotorIF::motor_feedback[CANMotorIF::SUB_PITCH].last_update_time, 5)) {
            // No feedback in last 5 ms (normal 1 ms)
            LOG_ERR("Startup - Gimbal SUB_Pitch offline.");
            t = SYSTIME;  // reset the counter
        }

        chThdSleepMilliseconds(5);
    }
}

bool InspectorH::gimbal_failure() {
    return gimbal_failure_;
}

bool InspectorH::chassis_failure() {
    return chassis_failure_;
}

bool InspectorH::remote_failure() {
    return remote_failure_;
}

bool InspectorH::check_gimbal_failure() {
    bool ret = false;
    /// TODO: Modified with referee.
//    if(Referee::bullet_remaining.bullet_remaining_num_17mm > 0) {
        for (unsigned i = CANMotorIF::YAW; i < CANMotorIF::FW_DOWN + 1; i++) {
            if (not WITHIN_RECENT_TIME(CANMotorIF::motor_feedback[i].last_update_time, 250)) {
                if (!gimbal_failure_) {  // avoid repeating printing
                    LOG_ERR("Gimbal motor %u offline (at %u)", i, CANMotorIF::motor_feedback[i].last_update_time);
                }
                ret = true;
            }
        }
//    } else {
//        for (unsigned i = 0; i < 3; i++) {
//            if (not WITHIN_RECENT_TIME(GimbalIF::feedback[i]->last_update_time, 250) &&
//                GimbalIF::feedback[i]->type != CANInterface::NONE_MOTOR) {
//                if (!gimbal_failure_) {  // avoid repeating printing
//                    LOG_ERR("Gimbal motor %u offline (since time %u)", i, GimbalIF::feedback[i]->last_update_time);
//                }
//                ret = true;
//            }
//        }
//    }
    return ret;
}

bool InspectorH::check_chassis_failure() {
    bool ret = false;
    for (unsigned i = CANMotorIF::FL; i < CANMotorIF::BL + 1; i++) {
        if (not WITHIN_RECENT_TIME(CANMotorIF::motor_feedback[i].last_update_time, 250)) {
            if (!chassis_failure_) {  // avoid repeating printing
                LOG_ERR("Gimbal motor %u offline (at %u)", i, CANMotorIF::motor_feedback[i].last_update_time);
            }
            ret = true;
        }
    }
    return ret;
}

bool InspectorH::check_remote_data_error() {
    bool ret = (!ABS_IN_RANGE(Remote::rc.ch0, 1.1) || !ABS_IN_RANGE(Remote::rc.ch1, 1.1) ||
                !ABS_IN_RANGE(Remote::rc.ch2, 1.1) || !ABS_IN_RANGE(Remote::rc.ch3, 1.1) ||
                !(Remote::rc.s1 >= 1 && Remote::rc.s1 <= 3) || !(Remote::rc.s2 >= 1 && Remote::rc.s2 <= 3) ||
                !ABS_IN_RANGE(Remote::mouse.x, 1.1) || !ABS_IN_RANGE(Remote::mouse.y, 1.1) ||
                !ABS_IN_RANGE(Remote::mouse.z, 1.1) ||
                Remote::rx_buf_[12] > 1 || Remote::rx_buf_[13] > 1);
    return ret;

}

void InspectorH::InspectorThread::main() {
    setName("InspectorH");
    while (!shouldTerminate()) {

        if (check_remote_data_error()) {
            remote_failure_ = true;  // Set it to true to avoid problem when thread switches to User in the middle
            while (check_remote_data_error()) {
                Remote::uart_synchronize();
                sleep(TIME_MS2I(10));  // wait for another normal frame
            }
            remote_failure_ = false;
        }

        remote_failure_ = (not WITHIN_RECENT_TIME(Remote::last_update_time, 30));
        if (remote_failure_) LED::led_off(DEV_BOARD_LED_REMOTE);
        else LED::led_on(DEV_BOARD_LED_REMOTE);
        gimbal_failure_ = check_gimbal_failure();
        if (gimbal_failure_) LED::led_off(DEV_BOARD_LED_GIMBAL);
        else LED::led_on(DEV_BOARD_LED_GIMBAL);

        chassis_failure_ = check_chassis_failure();
        if (chassis_failure_) LED::led_off(DEV_BOARD_LED_CHASSIS);
        else LED::led_on(DEV_BOARD_LED_CHASSIS);

        if (remote_failure_ || gimbal_failure_ || chassis_failure_) {
            if (!BuzzerSKD::alerting()) BuzzerSKD::alert_on();
        } else {
            if (BuzzerSKD::alerting()) BuzzerSKD::alert_off();
        }

        sleep(TIME_MS2I(INSPECTOR_THREAD_INTERVAL));
    }
}