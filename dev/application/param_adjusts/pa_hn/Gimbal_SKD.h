//
// Created by HTY是猪 on 2023/4/25.
//

#ifndef META_EMBEDDED_GIMBAL_SKD_H
#define META_EMBEDDED_GIMBAL_SKD_H

#include "ch.hpp"
#include "can_motor_config.h"
#include "can_motor_interface.h"

#define SKD_THREAD_INTERVAL 1

class Gimbal_SKD {
public:

    enum motor_id{
        Yaw,
        Pitch,
        gimbal_motor_count
    };

    static void start(tprio_t thread_prio);

    static void set_yaw_angle(float target_yaw_angle);

    static void set_pitch_angle(float target_pitch_angle);

    static void set_gimbal_angle(float target_yaw_angle, float target_pitch_angle);

    static void get_yaw_feedback();

    static void get_pitch_feedback();

    static void get_gimbal_feedback();
private:
    static float gimbal_feedback_angle[gimbal_motor_count];

    static float gimbal_target_angle[gimbal_motor_count];

    class SKDThread : public chibios_rt::BaseStaticThread<512> {
        void main() final;
    };

    static SKDThread skd_thread;
};


#endif //META_EMBEDDED_GIMBAL_SKD_H
