//
// Created by 钱晨 on 11/14/21.
//

/**
 * This file contain ... Unit Test.
 */

#include "ch.hpp"
#include "hal.h"

#include "interface/led/led.h"
#include "shell.h"
#include "can_motor_controller.h"
#include "can_motor_interface.h"
#include "remote_interpreter.h"
#include "hardware_conf.h"
// Other headers here

//#define UT_CHASSIS_SHELL_CONTROL

using namespace chibios_rt;
CANInterface can1(&CAND1);
CANInterface can2(&CAND2);

// Thread to ...
class ChassisControl : public BaseStaticThread <512> {
private:
    void main() final {
        setName("Control");
        while (!shouldTerminate()) {
            CANMotorController::set_target_angle(CANMotorCFG::YAW, 10);
            sleep(TIME_MS2I(100));
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

    // Start ChibiOS shell at high priority, so even if a thread stucks, we still have access to shell.
    LED::all_off();
    Shell::start(HIGHPRIO);
    can1.start(NORMALPRIO);
    can2.start(NORMALPRIO+1);
    CANMotorController::start(NORMALPRIO + 2, NORMALPRIO + 3, &can1, &can2);
    LED::led_on(1);
    CANMotorController::set_target_angle(CANMotorCFG::YAW, 10);
    ControlThread.start(NORMALPRIO + 6);
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
