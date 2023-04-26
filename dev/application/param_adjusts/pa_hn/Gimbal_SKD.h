//
// Created by HTY是猪 on 2023/4/25.
//

#ifndef META_EMBEDDED_GIMBAL_SKD_H
#define META_EMBEDDED_GIMBAL_SKD_H

#include "ch.hpp"

class Gimbal_SKD {
public:

    enum motor_id{
        Yaw,
        Pitch,
        gimbal_motor_count
    };

    static void start();

    static void set_yaw_angle();

    static void set_pitch_angle();

    static void set_gimbal_angle();
private:
    static float gimbal_feedback_angle[gimbal_motor_count];

    static float gimbal_target_angle[gimbal_motor_count];

    class SKDThread : public chibios_rt::BaseStaticThread<512> {
        void main() final;
    };
};


#endif //META_EMBEDDED_GIMBAL_SKD_H
