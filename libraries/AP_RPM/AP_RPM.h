/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <AP_Common/AP_Common.h>
#include <AP_HAL/AP_HAL.h>
#include <AP_Param/AP_Param.h>
#include <AP_Math/AP_Math.h>

 // Maximum number of RPM measurement instances available on this platform
#define RPM_MAX_INSTANCES 4

class AP_RPM_Backend;

class AP_RPM
{
    friend class AP_RPM_Backend;

public:
    AP_RPM();

    /* Do not allow copies */
    AP_RPM(const AP_RPM& other) = delete;
    AP_RPM& operator=(const AP_RPM&) = delete;

    // RPM driver types
    enum RPM_Type {
        RPM_TYPE_NONE = 0,
        RPM_TYPE_PWM = 1,
        RPM_TYPE_PIN = 2,
        RPM_TYPE_EFI = 3,
        RPM_TYPE_HNTCH = 4,
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
        RPM_TYPE_SITL = 10,
#endif
    };

    // The RPM_State structure is filled in by the backend driver
    struct RPM_State {
        uint8_t                instance;        // the instance number of this RPM
        float                  rate_rpm;        // measured rate in revs per minute
        uint32_t               last_reading_ms; // time of last reading
        float                  signal_quality;  // synthetic quality metric 
    };

    // parameters for each instance
    AP_Int8  _type[RPM_MAX_INSTANCES];
    AP_Int8  _pin[RPM_MAX_INSTANCES];
    AP_Float _scaling[RPM_MAX_INSTANCES];
    AP_Float _maximum;
    AP_Float _minimum;
    AP_Float _quality_min;

    static const struct AP_Param::GroupInfo var_info[];

    // Return the number of rpm sensor instances
    uint8_t num_sensors(void) const {
        return num_instances;
    }

    // detect and initialise any available rpm sensors
    void init(void);

    // update state of all rpm sensors. Should be called from main loop
    void update(void);

    /*
      return RPM for a sensor. Return -1 if not healthy
     */
    bool get_rpm(uint8_t instance, float& rpm_value) const;

    /*
      return signal quality for a sensor.
     */
    float get_signal_quality(uint8_t instance) const {
        return state[instance].signal_quality;
    }

    bool healthy(uint8_t instance) const;

    bool enabled(uint8_t instance) const;

    static AP_RPM* get_singleton() { return _singleton; }

private:
    static AP_RPM* _singleton;

    RPM_State state[RPM_MAX_INSTANCES];
    AP_RPM_Backend* drivers[RPM_MAX_INSTANCES];
    uint8_t num_instances : 3;  // This is a bit field not =

    void detect_instance(uint8_t instance);
};

namespace AP {
    AP_RPM* rpm();
};
