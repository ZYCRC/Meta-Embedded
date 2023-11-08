//
// Created by 钱晨 on 11/14/21.
//

/**
 * This file contain ... Unit Test.
 */

#include "ch.hpp"
#include "hal.h"

#include "interface/led/led.h"
#include "chthreads.h"
#include "shell.h"
#include "can_motor_controller.h"
#include "Gimbal_LG.h"
#include "Gimbal_SKD.h"
#include "can_motor_interface.h"
#include "remote_interpreter.h"
#include "hardware_conf.h"
#include "thread_priorities.h"
// Other headers here

//#define UT_CHASSIS_SHELL_CONTROL

using namespace chibios_rt;
CANInterface can1(&CAND1);
CANInterface can2(&CAND2);

const char *motor_name[10] = {
        "yaw",
        "pitch",
};

DEF_SHELL_CMD_START(cmd_motor_angle_enable)
    (void) argv;
    if(argc != 1){
        return false;
    }
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    CANMotorCFG::enable_a2v[motor_id] = true;
    CANMotorCFG::enable_v2i[motor_id] = true;
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_motor_angle_disable)
    (void) argv;
    if(argc != 1){
        return false;
    }
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    CANMotorCFG::enable_a2v[motor_id] = false;
    CANMotorCFG::enable_v2i[motor_id] = false;
    CANMotorController::set_target_current((CANMotorCFG::motor_id_t)motor_id, 0);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_motor_velocity_enable)
    (void) argv;
    if(argc != 1){
        return false;
    }
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return true;
    }
    CANMotorCFG::enable_a2v[motor_id] = false;
    CANMotorCFG::enable_v2i[motor_id] = true;
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_motor_velocity_disable)
    (void) argv;
    if(argc != 1){
        return false;
    }
    unsigned motor_id = Shell::atoi(argv[0]);
    CANMotorCFG::enable_a2v[motor_id] = false;
    CANMotorCFG::enable_v2i[motor_id] = false;
    CANMotorController::set_target_current((CANMotorCFG::motor_id_t)motor_id, 0);
    return true; // command executed successfully
DEF_SHELL_CMD_END


DEF_SHELL_CMD_START(cmd_get_sid)
    (void) argv;
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    Shell::printf("motor type: %d, motor sid: %x" SHELL_NEWLINE_STR,
                  CANMotorCFG::CANMotorProfile[motor_id].motor_type,
                  CANMotorCFG::CANMotorProfile[motor_id].CAN_SID);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_enable_feedback)
    if(argc != 1){
        return false;
    }
    unsigned feedback_motor_id = Shell::atoi(argv[0]);
    if (feedback_motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid feedback ID %d" SHELL_NEWLINE_STR, feedback_motor_id);
        return false;
    }
    Shell::printf("%s feedback enabled" SHELL_NEWLINE_STR, motor_name[feedback_motor_id]);
    CANMotorController::shell_display((CANMotorCFG::motor_id_t) feedback_motor_id, true);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_disable_feedback)
    (void) argv;
    if (argc != 1) {
        return false;
    }
    unsigned feedback_motor_id = Shell::atoi(argv[0]);
    if (feedback_motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid feedback ID %d" SHELL_NEWLINE_STR, feedback_motor_id);
        return false;
    }
    Shell::printf("%s feedback disabled" SHELL_NEWLINE_STR, motor_name[feedback_motor_id]);
    CANMotorController::shell_display((CANMotorCFG::motor_id_t) feedback_motor_id, false);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_set_param)
    (void) argv;
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    unsigned pid_id = Shell::atoi(argv[1]);
    if (pid_id > 1) {
        Shell::printf("Invalid pid ID %d" SHELL_NEWLINE_STR, pid_id);
        return false;
    }
    PIDController::pid_params_t pid_param = {Shell::atof(argv[2]),
                                             Shell::atof(argv[3]),
                                             Shell::atof(argv[4]),
                                             Shell::atof(argv[5]),
                                             Shell::atof(argv[6])};

    CANMotorController::load_PID_params((CANMotorCFG::motor_id_t) motor_id,  pid_id == 0, pid_param);

    Shell::printf("ps!" SHELL_NEWLINE_STR);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_echo_param)
    (void) argv;
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }

    unsigned pid_id = Shell::atoi(argv[1]);
    if (pid_id > 1) {
        Shell::printf("Invalid pid ID %d" SHELL_NEWLINE_STR, pid_id);
        return false;
    }

    PIDController::pid_params_t pid_param = {0,0,0,0,0};
    pid_param = CANMotorController::getPIDParams((CANMotorCFG::motor_id_t)motor_id, (pid_id == 0));
    Shell::printf("kp: %.2f, ki: %.2f, kd: %.2f, i_limit: %.2f, out_limit: %.2f" SHELL_NEWLINE_STR,
                  pid_param.kp, pid_param.ki, pid_param.kd, pid_param.i_limit, pid_param.out_limit);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_set_target_angle)
    (void) argv;
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    float angle = Shell::atof(argv[1]);
    CANMotorController::set_target_angle((CANMotorCFG::motor_id_t)motor_id, angle);
    Shell::printf("ps!" SHELL_NEWLINE_STR);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_set_target_velocity)
    (void) argv;
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    float velocity = Shell::atof(argv[1]);
    CANMotorController::set_target_vel((CANMotorCFG::motor_id_t)motor_id, velocity);
    Shell::printf("ps!" SHELL_NEWLINE_STR);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_echo_actual_angle)
    (void) argv;
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    float angle = 0;
    angle = CANMotorIF::motor_feedback[motor_id].accumulate_angle();
    Shell::printf("%.2f" SHELL_NEWLINE_STR, angle);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_echo_raw_angle)
    (void) argv;
    unsigned motor_id = Shell::atoi(argv[0]);
    if (motor_id >= CANMotorCFG::MOTOR_COUNT) {
        Shell::printf("Invalid motor ID %d" SHELL_NEWLINE_STR, motor_id);
        return false;
    }
    Shell::printf("%d" SHELL_NEWLINE_STR, CANMotorIF::motor_feedback[motor_id].rotor_angle_raw);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_get_vision_msg)
    (void) argv;
    float targetX = Shell::atof(argv[0]);
    float targetY = Shell::atof(argv[1]);
    float pull_gun = Shell::atoi(argv[2]);
    float shoot = Shell::atoi(argv[3]);

    Gimbal_LG::rec_gimbal_angle(targetX, targetY);
    Gimbal_LG::set_pull_permit(pull_gun);
    Gimbal_LG::set_shoot(shoot);
    Shell::printf("ps!" SHELL_NEWLINE_STR);
    return true; // command executed successfully
DEF_SHELL_CMD_END

DEF_SHELL_CMD_START(cmd_echo_lift_cycle)
    Shell::printf("pull feedback: %d, shoot feedback: %d\n", Gimbal_LG::pull_feedback, Gimbal_LG::shoot_feedback);
    return true; // command executed successfully
DEF_SHELL_CMD_END

// Command lists for gimbal controller test and adjustments
Shell::Command mainProgramCommands[] = {
        {"set_enable_a",      "motor_id",                                                             cmd_motor_angle_enable,    nullptr},
        {"set_disable_a",     "motor_id",                                                             cmd_motor_angle_disable,   nullptr},
        {"set_enable_v",      "motor_id",                                                             cmd_motor_velocity_enable, nullptr},
        {"set_disable_v",     "motor_id",                                                             cmd_motor_velocity_disable,nullptr},
        {"get_sid",           "motor_id",                                                             cmd_get_sid,               nullptr},
        {"fb_enable",         "motor_id",                                                             cmd_enable_feedback,       nullptr},
        {"fb_disable",        "motor_id",                                                             cmd_disable_feedback,      nullptr},
        {"set_pid",           "motor_id pid_id(0: angle_to_v, 1: v_to_i) kp ki kd i_limit out_limit", cmd_set_param,             nullptr},
        {"echo_pid",          "motor_id pid_id(0: angle_to_v, 1: v_to_i)",                            cmd_echo_param,            nullptr},
        {"set_target_angle",  "motor_id target_angle",                                                cmd_set_target_angle,      nullptr},
        {"set_target_vel",    "motor_id target_vel",                                                  cmd_set_target_velocity,  nullptr},
        {"echo_actual_angle", "motor_id",                                                             cmd_echo_actual_angle,    nullptr},
        {"echo_raw_angle",    "motor_id",                                                             cmd_echo_raw_angle,       nullptr},
        {"get_vision_msg", "targetX targetY pull_gun shoot",                                          cmd_get_vision_msg,      nullptr},
        {"echo_lift_cycle", nullptr,                                                                  cmd_echo_lift_cycle,    nullptr},
        {nullptr, nullptr, nullptr, nullptr}
};

// Thread to ...
/**
 * This file contain ... Unit Test.
 */

using namespace chibios_rt;


// Thread to adjust params of Skywalker-20A for friction wheels
class MG995AdjustThread : public BaseStaticThread <512> {
private:

    static constexpr PWMDriver *MAG_COVER_PWM_DRIVER = &PWMD8;

    enum mag_cover_channel_t {
        HAMMER = 0,
        TRIGGER = 1  // The right friction wheel, PI6, channel 1
    };

    static constexpr PWMConfig MAG_COVER_PWM_CFG = {
            50000,   // frequency
            1000,    // period
            nullptr, // callback
            {
                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH0
                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH1
                    {PWM_OUTPUT_DISABLED, nullptr},    // CH2
                    {PWM_OUTPUT_DISABLED, nullptr}     // CH3
            },
            0,
            0
    };

    void main() final {
        setName("pa_mg995");


        pwmStart(MAG_COVER_PWM_DRIVER, &MAG_COVER_PWM_CFG);

        bool has_set = false;

        while (!shouldTerminate()) {

            if (Gimbal_LG::pull_flag == 0 && Gimbal_LG::pull_flag != 2) {
                pwmEnableChannel(MAG_COVER_PWM_DRIVER, HAMMER,
                                 PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 105));
                chThdSleepMilliseconds(5);
                Gimbal_LG::pull_feedback = 0;
                LED::red_on();
                LED::green_off();
            } else {
                if (Gimbal_LG::pull_feedback == 0 || Gimbal_LG::pull_feedback == 2) {
                    pwmEnableChannel(MAG_COVER_PWM_DRIVER, HAMMER,
                                     PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 440));
                    Gimbal_LG::pull_feedback = 2;
                    chThdSleepMilliseconds(25);
                    pwmDisableChannel(MAG_COVER_PWM_DRIVER, HAMMER);
                    chThdSleepMilliseconds(1000 - 25);
                    Gimbal_LG::pull_feedback = 3;
                }
                else if (Gimbal_LG::pull_feedback == 3) {
                    Gimbal_LG::pull_feedback = 1;
                    pwmEnableChannel(MAG_COVER_PWM_DRIVER, HAMMER,
                                     PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 105));
                    if (Gimbal_LG::shoot_flag == 0) {
                        pwmEnableChannel(MAG_COVER_PWM_DRIVER, TRIGGER,
                                         PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 550));
//                        Shell::printf("trigger preload");
                    }
                    chThdSleepMilliseconds(25);
//                    Gimbal_LG::pull_feedback = 1;
                    pwmDisableChannel(MAG_COVER_PWM_DRIVER, HAMMER);
                    chThdSleepMilliseconds(100);
                }
                LED::green_on();
                LED::red_off();

            }

            if (Gimbal_LG::shoot_flag == 0 && Gimbal_LG::pull_feedback == 0) {
                LED::led_off(6);
                pwmEnableChannel(MAG_COVER_PWM_DRIVER, TRIGGER,
                                 PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 700));
                chThdSleepMilliseconds(5);
                chThdSleepMilliseconds(50);
                pwmDisableChannel(MAG_COVER_PWM_DRIVER, TRIGGER);
                Gimbal_LG::shoot_feedback = 0;
                LED::red_on();
                LED::green_off();
            } else if (Gimbal_LG::shoot_flag == 1 && Gimbal_LG::gun_inpos == 1) {
                LED::led_on(6);
                if (Gimbal_LG::shoot_feedback == 0 && Gimbal_LG::pull_feedback == 1 || Gimbal_LG::shoot_feedback == 2) {
                    LED::led_on(4);
                    Gimbal_LG::shoot_feedback = 2;
                    pwmEnableChannel(MAG_COVER_PWM_DRIVER, TRIGGER,
                                     PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 400));
                    chThdSleepMilliseconds(50);
                    pwmDisableChannel(MAG_COVER_PWM_DRIVER, TRIGGER);
                    chThdSleepMilliseconds(500);
                    Gimbal_LG::shoot_feedback = 1;
                }
                else if (Gimbal_LG::pull_feedback == 1) {
                    LED::led_off(4);
                    pwmEnableChannel(MAG_COVER_PWM_DRIVER, TRIGGER,
                                     PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 700));
                    chThdSleepMilliseconds(5);
                    Gimbal_LG::pull_feedback = 0;
                    Gimbal_LG::shoot_feedback = 0;
                    Gimbal_LG::pull_flag = 0;
                    Gimbal_LG::shoot_flag = 0;
                }
                LED::green_on();
                LED::red_off();

            }

            sleep(TIME_MS2I(50));
        }
    }
} skywalkerAdjustThread;

//// Thread to adjust params of Skywalker-20A for friction wheels
//class HAMMER_REST_Thread : public BaseStaticThread <512> {
//private:
//
//    static constexpr PWMDriver *MAG_COVER_PWM_DRIVER = &PWMD8;
//
//    enum mag_cover_channel_t {
//        HAMMER = 0,
//        TRIGGER = 1  // The right friction wheel, PI6, channel 1
//    };
//
//    static constexpr PWMConfig MAG_COVER_PWM_CFG = {
//            50000,   // frequency
//            1000,    // period
//            nullptr, // callback
//            {
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH0
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH1
//                    {PWM_OUTPUT_DISABLED, nullptr},    // CH2
//                    {PWM_OUTPUT_DISABLED, nullptr}     // CH3
//            },
//            0,
//            0
//    };
//
//    void main() final {
//        setName("hammer_rest");
//
//
//        pwmStart(MAG_COVER_PWM_DRIVER, &MAG_COVER_PWM_CFG);
//
//        bool has_set = false;
//
//        while (!shouldTerminate()) {
//
//            if (Gimbal_LG::pull_flag == 0) {
//                pwmEnableChannel(MAG_COVER_PWM_DRIVER, HAMMER,
//                                 PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 100));
//                chThdSleepMilliseconds(5);
//                pwmDisableChannel(MAG_COVER_PWM_DRIVER, HAMMER);
//                LED::red_on();
//                LED::green_off();
//            }
//        }
//        sleep(TIME_MS2I(50));
//    }
//} hammer_rest_Thread;
//
//// Thread to adjust params of Skywalker-20A for friction wheels
//class HAMMER_PULL_Thread : public BaseStaticThread <512> {
//private:
//
//    static constexpr PWMDriver *MAG_COVER_PWM_DRIVER = &PWMD8;
//
//    enum mag_cover_channel_t {
//        HAMMER = 0,
//        TRIGGER = 1  // The right friction wheel, PI6, channel 1
//    };
//
//    static constexpr PWMConfig MAG_COVER_PWM_CFG = {
//            50000,   // frequency
//            1000,    // period
//            nullptr, // callback
//            {
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH0
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH1
//                    {PWM_OUTPUT_DISABLED, nullptr},    // CH2
//                    {PWM_OUTPUT_DISABLED, nullptr}     // CH3
//            },
//            0,
//            0
//    };
//
//    void main() final {
//        setName("hammer_pull");
//
//
//        pwmStart(MAG_COVER_PWM_DRIVER, &MAG_COVER_PWM_CFG);
//
//        bool has_set = false;
//
//        while (!shouldTerminate()) {
//
//            if (Gimbal_LG::pull_flag == 1) {
//                pwmEnableChannel(MAG_COVER_PWM_DRIVER, HAMMER,
//                                 PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 450));
//                chThdSleepMilliseconds(800);
//                pwmDisableChannel(MAG_COVER_PWM_DRIVER, HAMMER);
//                Gimbal_LG::pull_flag = 1;
//
//            }
//        }
//        sleep(TIME_MS2I(50));
//    }
//} hammer_pull_Thread;
//
//// Thread to adjust params of Skywalker-20A for friction wheels
//class TRIGGER_REST_Thread : public BaseStaticThread <512> {
//private:
//
//    static constexpr PWMDriver *MAG_COVER_PWM_DRIVER = &PWMD8;
//
//    enum mag_cover_channel_t {
//        HAMMER = 0,
//        TRIGGER = 1  // The right friction wheel, PI6, channel 1
//    };
//
//    static constexpr PWMConfig MAG_COVER_PWM_CFG = {
//            50000,   // frequency
//            1000,    // period
//            nullptr, // callback
//            {
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH0
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH1
//                    {PWM_OUTPUT_DISABLED, nullptr},    // CH2
//                    {PWM_OUTPUT_DISABLED, nullptr}     // CH3
//            },
//            0,
//            0
//    };
//
//    void main() final {
//        setName("trigger_rest");
//
//
//        pwmStart(MAG_COVER_PWM_DRIVER, &MAG_COVER_PWM_CFG);
//
//        bool has_set = false;
//
//        while (!shouldTerminate()) {
//
//            if (Gimbal_LG::shoot_flag == 0) {
//                pwmEnableChannel(MAG_COVER_PWM_DRIVER, TRIGGER,
//                                 PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 700));
//                chThdSleepMilliseconds(5);
//                pwmDisableChannel(MAG_COVER_PWM_DRIVER, HAMMER);
//                LED::red_on();
//                LED::green_off();
//
//            }
//        }
//        sleep(TIME_MS2I(50));
//    }
//} trigger_rest_Thread;
//
//// Thread to adjust params of Skywalker-20A for friction wheels
//class TRIGGER_PULL_Thread : public BaseStaticThread <512> {
//private:
//
//    static constexpr PWMDriver *MAG_COVER_PWM_DRIVER = &PWMD8;
//
//    enum mag_cover_channel_t {
//        HAMMER = 0,
//        TRIGGER = 1  // The right friction wheel, PI6, channel 1
//    };
//
//    static constexpr PWMConfig MAG_COVER_PWM_CFG = {
//            50000,   // frequency
//            1000,    // period
//            nullptr, // callback
//            {
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH0
//                    {PWM_OUTPUT_ACTIVE_HIGH, nullptr}, // CH1
//                    {PWM_OUTPUT_DISABLED, nullptr},    // CH2
//                    {PWM_OUTPUT_DISABLED, nullptr}     // CH3
//            },
//            0,
//            0
//    };
//
//    void main() final {
//        setName("trigger_pull");
//
//
//        pwmStart(MAG_COVER_PWM_DRIVER, &MAG_COVER_PWM_CFG);
//
//        bool has_set = false;
//
//        while (!shouldTerminate()) {
//
//            if (Gimbal_LG::shoot_flag == 1) {
//                pwmEnableChannel(MAG_COVER_PWM_DRIVER, TRIGGER,
//                                 PWM_PERCENTAGE_TO_WIDTH(MAG_COVER_PWM_DRIVER, 1 * 500 + 300));
//                chThdSleepMilliseconds(5);
//                pwmDisableChannel(MAG_COVER_PWM_DRIVER, HAMMER);
//                LED::red_on();
//                LED::green_off();
//            }
//        }
//        sleep(TIME_MS2I(50));
//    }
//} trigger_pull_Thread;
PWMConfig constexpr MG995AdjustThread::MAG_COVER_PWM_CFG;
//PWMConfig constexpr TRIGGER_PULL_Thread::MAG_COVER_PWM_CFG;
//PWMConfig constexpr TRIGGER_REST_Thread::MAG_COVER_PWM_CFG;
//PWMConfig constexpr HAMMER_PULL_Thread::MAG_COVER_PWM_CFG;
//PWMConfig constexpr HAMMER_REST_Thread::MAG_COVER_PWM_CFG;

class ChassisControl : public BaseStaticThread <512> {
private:
    void main() final {
        setName("Control");
        while (!shouldTerminate()) {
            if (Gimbal_LG::get_pull_stats()){
                if (Gimbal_LG::get_pull_stats() > Gimbal_LG::pull_last_cmd) {
                    Gimbal_LG::pull_flag = 1;
                    Gimbal_LG::pull_last_cmd = Gimbal_LG::get_pull_stats();
                }
                CANMotorController::set_target_vel(CANMotorCFG::LIFT, -12500);
                if(Gimbal_SKD::lift_cycle < -1200 + Gimbal_SKD::lift_init_angle){
                    CANMotorController::set_target_vel(CANMotorCFG::LIFT, 0);
                    Gimbal_LG::cal_gimbal_angle();
                    Gimbal_LG::gun_inpos = 1;
                }

                if (Gimbal_LG::get_shoot_stats()){
                    if (Gimbal_LG::get_shoot_stats() > Gimbal_LG::shoot_last_cmd) {
                        Gimbal_LG::shoot_flag = 1;
                    }
                }
            }
            else {
                CANMotorController::set_target_vel(CANMotorCFG::LIFT, 1000);
                Gimbal_LG::init_gun_pos();
                Gimbal_LG::gun_inpos = 0;
                if(Gimbal_SKD::lift_cycle > Gimbal_SKD::lift_init_angle - 100){
                    CANMotorController::set_target_vel(CANMotorCFG::LIFT, 0);
                }
                palClearPad(GPIOG, GPIOG_RED_SPOT_LASER);
            }
            Gimbal_LG::pull_last_cmd = Gimbal_LG::get_pull_stats();
            Gimbal_LG::shoot_last_cmd = Gimbal_LG::get_shoot_stats();
            sleep(TIME_MS2I(10));
        }
    }
} ControlThread;

/**
 * @brief set enabled state of yaw and pitch motor
 * @param chp
 * @param argc
 * @param argv
 */

int main() {
    halInit();
    System::init();

    // Enable power of ultraviolet lights
    palClearPad(GPIOG, GPIOG_RED_SPOT_LASER);

    // Start ChibiOS shell at high priority, so even if a thread stucks, we still have access to shell.
    LED::all_off();
    Shell::start(HIGHPRIO);
    Shell::addCommands(mainProgramCommands);
    chThdSleepMilliseconds(50);  // wait for logo to print :)
    can1.start(NORMALPRIO);
    can2.start(NORMALPRIO+1);
    CANMotorController::start(NORMALPRIO + 2, NORMALPRIO + 3, &can1, &can2);
    LED::led_on(1);
    Gimbal_SKD::start(THREAD_GIMBAL_SKD_PRIO);
    ControlThread.start(NORMALPRIO + 6);
    skywalkerAdjustThread.start(NORMALPRIO + 5);
//    trigger_pull_Thread.start(NORMALPRIO + 5);
//    trigger_rest_Thread.start((NORMALPRIO + 9));
//    hammer_pull_Thread.start(NORMALPRIO + 7);
//    hammer_rest_Thread.start((NORMALPRIO + 8));
#ifndef UT_CHASSIS_SHELL_CONTROL

#else
    CANMotorCFG::enable_v2i[CANMotorCFG::FL] = true;
    CANMotorCFG::enable_v2i[CANMotorCFG::FR] = true;
    CANMotorCFG::enable_v2i[CANMotorCFG::BR] = true;
    CANMotorCFG::enable_v2i[CANMotorCFG::BL] = true;
#endif


#if CH_CFG_NO_IDLE_THREAD // see chconf.h for what this #define means
    // ChibiOS idle thread has been disabled, main() should implement infinite loop
    while (true) {}
#else
    // When main() quits, the main thread will somehow enter an infinite loop, so we set the priority to lowest
    // before quitting, to let other threads run normally
    BaseThread::setPriority(1);
#endif
    return 0;
}
