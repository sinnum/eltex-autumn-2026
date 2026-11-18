#include "calc.h"
#include <stdio.h>
#include <math.h> // для isnan

void print(float a, float b, float res, char znak) {
    printf("-----------------\n");
    printf("%g %c %g = %g\n", a, znak, b, res);
    printf("-----------------\n");
}

void menu(void) {
    printf("\n=== CALCULATOR ===\n");
    for (int i = 0; i < calcs_cnt; i++) {
        printf("%d. %s (%c)\n", i + 1, calcs[i].name, calcs[i].op);
    }
    
    printf("%d. sum_n (varargs)\n", calcs_cnt + 1);
    printf("0. exit\n");
    printf("choice: ");
}

int main(void) {
    int choice;
    float a, b, res;

    while (1) {
        menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("err input.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        if (choice == 0) return 0;

        if (choice >= 1 && choice <= calcs_cnt) {
            int idx = choice - 1; 
            
            printf("input two nums: ");
            if (scanf("%f %f", &a, &b) != 2) {
                printf("err input.\n");
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');

            res = calcs[idx].func(a, b);

            if (isnan(res)) {
                printf("err: division by zero\n");
            } else {
                print(a, b, res, calcs[idx].op);
            }
        }
        else if (choice == calcs_cnt + 1) {
            int n;
            printf("input count of numbers (2..16): ");
            if (scanf("%d", &n) != 1) {
                printf("err input.\n");
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');

            if (n < 2 || n > 16) {
                printf("err: n must be 2..16\n");
                continue;
            }

            float add_nums[16];
            printf("enter %d floats: ", n);
            for (int i = 0; i < n; i++) {
                if (scanf("%f", &add_nums[i]) != 1) {
                    printf("err input.\n");
                    break;
                }
            }
            while (getchar() != '\n');

            switch (n) {
                case 2: res = sum_n(n, (double)add_nums[0], (double)add_nums[1]); break;
                case 3: res = sum_n(n, (double)add_nums[0], (double)add_nums[1], (double)add_nums[2]); break;
                case 4: res = sum_n(n, (double)add_nums[0], (double)add_nums[1], (double)add_nums[2], (double)add_nums[3]); break;
                case 5: res = sum_n(n, (double)add_nums[0], (double)add_nums[1], (double)add_nums[2], (double)add_nums[3], (double)add_nums[4]); break;
                case 6: res = sum_n(n, (double)add_nums[0], (double)add_nums[1], (double)add_nums[2], (double)add_nums[3], (double)add_nums[4], (double)add_nums[5]); break;
                default:
                    res = 0;
                    for (int i = 0; i < n; i++) res += add_nums[i];
                    break;
            }
            printf("summ of numbers = %g\n", res);
        }
        else {
            printf("invalid choice\n");
        }
    }
    return 0;
}