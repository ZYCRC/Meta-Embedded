//
// Created by liuzikai on 2019-06-25.
//

#ifndef META_INFANTRY_USER_H
#define META_INFANTRY_USER_H

#include "ch.hpp"

#include "remote_interpreter.h"
#include "referee_interface.h"

#include "gimbal_logic.h"
#include "infantry_shoot_logic.h"
#include "chassis_logic.h"

#include "inspector.h"

class User {

public:

    static void start(tprio_t user_thd_prio, tprio_t user_action_thd_prio, tprio_t bullet_increment_thd_prio,
                      tprio_t client_data_sending_thd_prio);

private:

    /// Gimbal Config
    static float gimbal_rc_yaw_max_speed;  // [degree/s]
    static float gimbal_pc_yaw_sensitivity[];  // [Ctrl, Normal, Shift] [degree/s]

    static float gimbal_pc_pitch_sensitivity[];   // rotation speed when mouse moves fastest [degree/s]
    static float gimbal_pitch_min_angle; // down range for pitch [degree]
    static float gimbal_pitch_max_angle; //  up range for pitch [degree]

    /// Chassis Config
    static float chassis_v_left_right;  // [mm/s]
    static float chassis_v_forward;     // [mm/s]
    static float chassis_v_backward;    // [mm/s]

    static float chassis_pc_shift_ratio;  // 150% when Shift is pressed
    static float chassis_pc_ctrl_ratio;    // 50% when Ctrl is pressed

    static Remote::key_t chassis_dodge_switch;
    static unsigned chassis_dodge_light_index;

    /// Shoot Config
    static float shoot_launch_left_count;
    static float shoot_launch_right_count;

    static float shoot_lanuch_speed;

    static float shoot_common_duty_cycle;

    static unsigned shoot_remain_bullet_data_index;

    static Remote::key_t shoot_fw_switch;
    static unsigned shoot_fw_status_light_index;

    /// Other Config
    static unsigned high_speed_light_index;
    static unsigned low_speed_light_index;


    /// User Thread
    static constexpr unsigned USER_THREAD_INTERVAL = 7;  // [ms]
    class UserThread : public chibios_rt::BaseStaticThread<512> {

        /// Runtime variables

        float gimbal_yaw_target_angle_ = 0;
        float gimbal_pc_pitch_target_angle_ = 0;

        void main() final;
    };

    static UserThread userThread;


    /// User Action Thread
    class UserActionThread : public chibios_rt::BaseStaticThread<512> {

        /// Runtime variables
        event_listener_t s_change_listener;
        static constexpr eventmask_t S_CHANGE_EVENTMASK = (1U << 0U);

        event_listener_t mouse_press_listener;
        static constexpr eventmask_t MOUSE_PRESS_EVENTMASK = (1U << 1U);

        event_listener_t mouse_release_listener;
        static constexpr eventmask_t MOUSE_RELEASE_EVENTMASK = (1U << 2U);

        event_listener_t key_press_listener;
        static constexpr eventmask_t KEY_PRESS_EVENTMASK = (1U << 3U);

        event_listener_t key_release_listener;
        static constexpr eventmask_t KEY_RELEASE_EVENTMASK = (1U << 4U);

        void main() final;
    };

    static UserActionThread userActionThread;

    /// Thread to react to bullet increment event
    class BulletIncrementThread : public chibios_rt::BaseStaticThread<256> {

        event_listener_t data_received_listener;
        static constexpr eventmask_t DATA_RECEIVED_EVENTMASK = (1U << 0U);

        void main() final;
    };

    static BulletIncrementThread bulletIncrementThread;

    /// Referee Client Data Sending Thread
    static constexpr unsigned CLIENT_DATA_SENDING_THREAD_INTERVAL = 10;  // [ms]

    class ClientDataSendingThread : public chibios_rt::BaseStaticThread<256> {
        void main() final;
    };

    static ClientDataSendingThread clientDataSendingThread;

};


#endif //META_INFANTRY_USER_H
