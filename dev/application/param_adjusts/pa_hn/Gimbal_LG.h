//
// Created by Youch on 4/25/2023.
//

#ifndef META_EMBEDDED_GIMBAL_LG_H
#define META_EMBEDDED_GIMBAL_LG_H

#include "ch.hpp"
#include "can_motor_controller.h"
#include "can_motor_interface.h"
#include "Gimbal_SKD.h"

class Gimbal_LG {
public:
    enum motor_id{
        Yaw,
        Pitch,
        gimbal_motor_count
    };

    /**
     * @brief init the gimbal para
     */
    static void start(tprio_t thread_prio);

    static void set_yaw_angle(float target_yaw_angle);

    static void set_pitch_angle(float target_pitch_angle);

    static void set_gimbal_angle(float target_yaw_angle, float target_pitch_angle);

    static void get_yaw_feedback();

    static void get_pitch_feedback();

    static void get_gimbal_feedback();

private:

};


#endif //META_EMBEDDED_GIMBAL_LG_H
