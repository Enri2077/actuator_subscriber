#ifndef _PID_HPP_
#define _PID_HPP_

#include <float.h>

class PID
{
private:
    float _i;
    float _d;
    float _setpoint;
    float _output;
    float _k;
    float _ki;
    float _kd;
    float _min;
    float _max;

public:
    PID(
        void
    );
    void
    config(
        float k,
        float ti,
        float td,
        float ts,
        float min,
        float max
    );

    void
    setLimits(
        float min,
        float max
    );

    void
    set(
        float setpoint
    );

    void
    setI(
        float setpoint
    );

    float
    getError(
        float measure
    );

    float
    update(
        float measure
    );
};


PID::PID(
    void
)
{
    _i = 0;
    _d = 0;
    _setpoint = 0;
    _output   = 0;
    _k   = 0;
    _ki  = 0;
    _kd  = 0;
    _min = -FLT_MAX;
    _max = FLT_MAX;
}

void
PID::config(
    float k,
    float ti,
    float td,
    float ts,
    float min = -FLT_MAX,
    float max = FLT_MAX
)
{
    chSysLock();
    _k   = k;
    _ki  = (ti == 0) ? 0 : k * (ts / ti);
    _kd  = k * (td / ts);
    _min = min;
    _max = max;
    _i   = 0;
    _d   = 0;
    chSysUnlock();
}

void
PID::setLimits(
    float min,
    float max
)
{
    chSysLock();
    _min = min;
    _max = max;
    chSysUnlock();
}

void
PID::set(
    float setpoint
)
{
    chSysLock();

    setI(setpoint);
    chSysUnlock();
}

void
PID::setI(
    float setpoint
)
{
    // Reset integral and derivative components if sign has changed or setpoint is 0
    if ((setpoint > 0) != (_setpoint > 0)) {
        _i = 0;
        _d = 0;
    }

    _setpoint = setpoint;
}

float
PID::getError(
    float measure
)
{
    return _setpoint - measure;
}

float
PID::update(
    float measure
)
{
    float error;
    float output;

    /* calculate error */
    error = _setpoint - measure;

    /* proportional term */
    output = (_k * error);

    /* integral term */
    if (error * _i < 0) {
        _i = 0;
    }

    _i     += _ki * error;
    output += _i;

    /* derivative term */
    output += _kd * (error - _d);
    _d      = error;

    /* saturation filter */
    if (output > _max) {
        output = _max;
        /* anti windup: cancel error integration */
        _i -= _ki * error;
    } else if (output < _min) {
        output = _min;
        /* anti windup: cancel error integration */
        _i -= _ki * error;
    }

    _output += output;

    return _output;
} // PID::update

#endif /* _PID_HPP_ */
