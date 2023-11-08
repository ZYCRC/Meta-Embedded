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

    static float get_pitch_feedback();

    static void get_gimbal_feedback();

    static void cal_gimbal_angle();

    static void rec_gimbal_angle(float targetX, float targetY);

    static void init_gun_pos();

    static void set_pull_permit(int pull);

    static void set_shoot(int shoot);

    static int get_shoot_stats();

    static int get_pull_stats();

    static void pull_gun();

    static void set_lift_vel(float target_vel);

    static void shoot();

    static int pull_flag;

    static int shoot_flag;

    static int pull_feedback;

    static int shoot_feedback;

    static int finish_shoot;

    static int pull_last_cmd;

    static int shoot_last_cmd;

    static int gun_inpos;

private:

    static float vision_pos[2];

    static int shoot_permit;

    static int pull_permit;

    static int pull_request;

};


#endif //META_EMBEDDED_GIMBAL_LG_H
