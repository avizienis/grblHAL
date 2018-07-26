/*
  sleep.c - determines and executes sleep procedures
  Part of Grbl

  Copyright (c) 2018 Terje Io
  Copyright (c) 2016 Sungeun K. Jeon

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"

volatile bool slumber;

static void fall_asleep()
{
    slumber = false;
}

// Starts sleep timer if running conditions are satified. When elapsed, sleep mode is executed.
static void sleep_execute()
{
    // Enable sleep timeout
    slumber = true;
    hal.delay_ms((uint32_t)(SLEEP_DURATION * 1000.0f), fall_asleep);

    // Fetch current number of buffered characters in serial RX buffer.
    uint16_t rx_initial = hal.serial_get_rx_buffer_available();

    do {
        // Monitor for any new RX serial data or external events (queries, buttons, alarms) to exit.
        if ((hal.serial_get_rx_buffer_available() != rx_initial) || sys_rt_exec_state || sys_rt_exec_alarm ) {
            // Disable sleep timeout and return to normal operation.
            hal.delay_ms(0, 0);
            return;
        }
    } while(slumber);

    // If reached, sleep counter has expired. Execute sleep procedures.
    // Notify user that Grbl has timed out and will be parking.
    // To exit sleep, resume or reset. Either way, the job will not be recoverable.
    report_feedback_message(Message_SleepMode);
    system_set_exec_state_flag(EXEC_SLEEP);
}


// Checks running conditions for sleep. If satisfied, enables sleep timeout and executes
// sleep mode upon elapse.
// NOTE: Sleep procedures can be blocking, since Grbl isn't receiving any commands, nor moving.
// Hence, make sure any valid running state that executes the sleep timer is not one that is moving.
void sleep_check()
{
    // The sleep execution feature will continue only if the machine is in an IDLE or HOLD state and
    // has any powered components enabled.
    // NOTE: With overrides or in laser mode, modal spindle and coolant state are not guaranteed. Need
    // to directly monitor and record running state during parking to ensure proper function.
    if ((gc_state.modal.spindle.value || gc_state.modal.coolant.value) && !sys.steppers_deenergize) {
        if (sys.state == STATE_IDLE) {
            sleep_execute();
        } else if ((sys.state & STATE_HOLD) && (sys.holding_state == Hold_Complete)) {
            sleep_execute();
        } else if (sys.state == STATE_SAFETY_DOOR && (sys.parking_state == Parking_DoorAjar)) {
            sleep_execute();
        }
    }
}
