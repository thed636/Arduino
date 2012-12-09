#ifndef REGULATOR_H_165506122012
#define REGULATOR_H_165506122012
#include <Arduino.h>

class PidRegulator {
public:
    enum { scaleFactor = 10000 };
    PidRegulator(float Kp_, float Kip, float Kdp, float maxu, float minu, float T)
    : e_1(0), e_2(0), Kp(0), Kdi(0), Kdd(0), u_1(0), maxu( maxu * float(scaleFactor)),
      minu(minu * float(scaleFactor)), T(T) {
        configure(Kp_, Kip, Kdp);
    }

    int control(int error) {
        const long e = error;
        const long u = u_1 + Kp * (e - e_1) + Kdi * e + Kdd * (e - 2 * e_1 + e_2);
        u_1 = u;//constrain(u, minu, maxu);
        e_2 = e_1;
        e_1 = e;
        return u_1 / scaleFactor;
    }

    void reset() {
        u_1 = e_1 = e_2 = 0;
    }

    void configure( float Kp_, float Kip, float Kdp ) {
        Kp = Kp_ * float(scaleFactor),
        Kdi = Kp_ * Kip * T * float(scaleFactor);
        Kdd = Kp_ * Kdp / T * float(scaleFactor);
        reset();
    }
private:
    long e_1;
    long e_2;
    long Kp;
    long Kdi;
    long Kdd;
    long u_1;
    long maxu;
    long minu;
    float T;
};

class PRegulator {
public:
    enum { scaleFactor = 1000 };

    PRegulator(float Kp = 1., float maxu = 160., float minu = -160.)
    : Kp(Kp * float(scaleFactor)),
      maxu(maxu * float(scaleFactor)),
      minu( minu * float(scaleFactor)) {
    }

    int control(long error) {
        const long u = error * Kp;
        return int(constrain(u, minu, maxu) / long(scaleFactor));
    }

    void configure( float Kp_ ) {
        Kp = Kp_ * float(scaleFactor);
    }
private:
    long Kp;
    long maxu;
    long minu;
};

#endif
