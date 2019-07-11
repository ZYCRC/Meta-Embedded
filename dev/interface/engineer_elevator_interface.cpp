//
// Created by Kerui Zhu on 7/9/2019.
//

#include "engineer_elevator_interface.h"

EngineerElevatorIF::elevator_motor_t EngineerElevatorIF::elevatorMotor[MOTOR_COUNT];
EngineerElevatorIF::aided_motor_t EngineerElevatorIF::aidedMotor[MOTOR_COUNT];
CANInterface* EngineerElevatorIF::can = nullptr;


void EngineerElevatorIF::init(CANInterface *can_interface) {
    can = can_interface;
    can->register_callback(0x201, 0x204, process_feedback);
    for (elevator_motor_t motor : elevatorMotor) {
        motor.last_update_time = SYSTIME;
        motor.clear_accmulate_angle();
        motor.target_current = 0;
    }
    for (aided_motor_t motor : aidedMotor){
        motor.last_update_time = SYSTIME;
        motor.target_current = 0;
    }
}

bool EngineerElevatorIF::send_currents() {

    if (!can) return false;

    CANTxFrame txmsg;

    // Fill the header
    txmsg.IDE = CAN_IDE_STD;
    txmsg.SID = 0x200;
    txmsg.RTR = CAN_RTR_DATA;
    txmsg.DLC = 0x08;

    // Fill target currents
    txmsg.data8[0] = (uint8_t) (elevatorMotor[R].target_current >> 8);
    txmsg.data8[1] = (uint8_t) elevatorMotor[R].target_current;
    txmsg.data8[2] = (uint8_t) (elevatorMotor[L].target_current >> 8);
    txmsg.data8[3] = (uint8_t) elevatorMotor[L].target_current;

    txmsg.data8[4] = (uint8_t) (aidedMotor[R].target_current >> 8);
    txmsg.data8[5] = (uint8_t) aidedMotor[R].target_current;
    txmsg.data8[6] = (uint8_t) ((-aidedMotor[L].target_current) >> 8);
    txmsg.data8[7] = (uint8_t) (-aidedMotor[R].target_current);


    can->send_msg(&txmsg);
    return true;
}

void EngineerElevatorIF::process_feedback(CANRxFrame const *rxmsg) {

    if (rxmsg->SID > 0x204 || rxmsg->SID < 0x201) return;

    int motor_id = (int) (rxmsg->SID - 0x201);

    if (motor_id < 2){
        auto new_angle_raw = (uint16_t) (rxmsg->data8[0] << 8 | rxmsg->data8[1]);
        elevatorMotor[motor_id].actual_rpm_raw = (int16_t) (rxmsg->data8[2] << 8 | rxmsg->data8[3]);
        elevatorMotor[motor_id].actual_current = (int16_t) (rxmsg->data8[4] << 8 | rxmsg->data8[5]);

        int angle_movement = (int) new_angle_raw - (int) elevatorMotor[motor_id].actual_angle_raw;

        // If angle_movement is too extreme between two samples,
        // we grant that it's caused by moving over the 0(8192) point.
        if (angle_movement < -4096) {
            angle_movement += 8192;
        } else if (angle_movement > 4096) {
            angle_movement -= 8192;
        }

        elevatorMotor[motor_id].present_angle += angle_movement * 360.0f / 8192.0f / chassis_motor_decelerate_ratio;

        elevatorMotor[motor_id].actual_angle_raw = new_angle_raw;

        // See the meaning of the motor decelerate ratio
        elevatorMotor[motor_id].actual_velocity =
                elevatorMotor[motor_id].actual_rpm_raw / chassis_motor_decelerate_ratio * 360.0f / 60.0f;

        elevatorMotor[motor_id].last_update_time = SYSTIME;
    } else{
        motor_id = motor_id % 2;
        aidedMotor[motor_id].actual_velocity = (rxmsg->data8[2] << 8 | rxmsg->data8[3]) * 360.0f / 60.0f;
        aidedMotor[motor_id].last_update_time = SYSTIME;
    }

}

void EngineerElevatorIF::elevator_motor_t::clear_accmulate_angle() {
    present_angle = 0;
}