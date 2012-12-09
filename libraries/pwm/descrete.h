#ifndef DESCRETE_H_165506122012
#define DESCRETE_H_165506122012
#include <Arduino.h>

class DescreteSample {
public:
    enum { dtDefault = 1 };
    DescreteSample(unsigned long dt = dtDefault)
    : dt_(dt), time_(millis()) {
    }

    bool tick() {
        unsigned long t = millis();
        if (t - time_ < dt_) {
            return false;
        }
        time_ = t;
        return true;
    }

    unsigned long dt() const {
        return dt_;
    }

    float T() const {
        return float(dt()) / 1000.;
    }

    unsigned long time() const {
        return time_;
    }
private:
    unsigned long time_;
    unsigned long dt_;
};

#endif
