#include "calc.h"
#include <stdarg.h>
#include <math.h>

float sum_n(int n, ...){
    float result = 0;
    va_list nums;
    va_start(nums, n);
    for (int i = 0; i<n; i++) {
        result += va_arg(nums, double);
    }
    va_end(nums);
    return result;
}
float sum(float a, float b){
    return a+b;
}
float diff(float a, float b){
    return a - b;
}
float mult(float a, float b){
    return a*b;
}
float divv(float a, float b){
    if (b==0) return NAN;
    return a/b;
}

const calculates calcs[] = {
    { "sum",  '+', sum  },
    { "diff", '-', diff },
    { "mult", '*', mult },
    { "div",  '/', divv }
};

const int calcs_cnt = sizeof(calcs) / sizeof(calcs[0]);
