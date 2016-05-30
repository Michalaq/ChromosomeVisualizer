#ifndef TIMETRANSFORMATION_H
#define TIMETRANSFORMATION_H

#include "common.h"

struct TimeTransformation {
    frameNumber_t displacement;
    frameNumber_t nominator, denominator;

    void simplify() {
        frameNumber_t a = nominator, b = denominator;
        while (b != 0) {
            frameNumber_t tmp = b;
            b = a % b;
            a = tmp;
        }

        nominator /= a;
        denominator /= a;
    }

    void setTempo(frameNumber_t nom, frameNumber_t denom) {
        nominator = nom;
        denominator = denom;
        simplify();
    }

    inline frameNumber_t transform(frameNumber_t time) const {
        return
            displacement +
            (frameNumber_t)((__int128)time * (__int128)nominator / (__int128)denominator);
    }

    inline frameNumber_t transformBack(frameNumber_t time) const {
        return
            (frameNumber_t)((__int128)(time - displacement)
                            * (__int128)denominator / (__int128)nominator);
    }
};

#endif // TIMETRANSFORMATION_H
