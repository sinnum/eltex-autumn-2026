#ifndef CALC_H
#define CALC_H

typedef float (*calc_f)(float, float);

typedef struct {
    const char* name;
    char op;
    calc_f func;
}calculates;

float sum_n(int n, ...);
float sum(float a, float b);
float diff(float a, float b);
float mult(float a, float b);
float divv(float a, float b);
extern const calculates calcs[];
extern const int calcs_cnt;

#endif
