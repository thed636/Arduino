#ifndef REGULATOR_H_165506122012
#define REGULATOR_H_165506122012
#include <Arduino.h>

class PidRegulator {
public:
    enum { scaleFactor = 10000 };
    PidRegulator(float Kp, float Kip, float Kdp, float maxu, float minu, float T)
    : e_1(0),
      e_2(0),
      Kp(Kp * float(scaleFactor)),
      Kdi( Kp * Kip * T * float(scaleFactor)),
      Kdd( Kp * Kdp / T * float(scaleFactor)),
      u_1(0), maxu( maxu * float(scaleFactor)),
      minu(minu * float(scaleFactor)) {
    }

    int control(int error) {
        const long e = error;
        const long u = u_1 + Kp * (e - e_1) + Kdi * e + Kdd * (e - 2 * e_1 + e_2);
        u_1 = constrain(u, minu, maxu);
        e_2 = e_1;
        e_1 = e;
        return u_1 / scaleFactor;
    }

    void reset() {
        u_1 = e_1 = e_2 = 0;
    }
private:
    int e_1;
    int e_2;
    int Kp;
    int Kdi;
    int Kdd;
    long u_1;
    long maxu;
    long minu;
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
private:
    long Kp;
    long maxu;
    long minu;
};

#endif
