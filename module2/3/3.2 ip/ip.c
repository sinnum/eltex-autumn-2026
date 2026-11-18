#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

uint32_t parse_ip(const char *ip_str) {
    unsigned int a, b, c, d;
    if (sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) {
        fprintf(stderr, "Ошибка: неверный формат IP-адреса %s\n", ip_str);
        exit(1);
    }
    if (a > 255 || b > 255 || c > 255 || d > 255) {
        fprintf(stderr, "Ошибка: октет IP-адреса больше 255\n");
        exit(1);
    }
    return (a << 24) | (b << 16) | (c << 8) | d;
}

void print_ip(uint32_t ip) {
    unsigned int a = (ip >> 24) & 0xFF;
    unsigned int b = (ip >> 16) & 0xFF;
    unsigned int c = (ip >> 8) & 0xFF;
    unsigned int d = ip & 0xFF;
    printf("%u.%u.%u.%u", a, b, c, d);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <Gateway_IP> <Subnet_Mask> <N_packets>\n", argv[0]);
        return 1;
    }

    // разбираем аргументы командной строки
    uint32_t gw_ip = parse_ip(argv[1]);
    uint32_t mask = parse_ip(argv[2]);
    int N = atoi(argv[3]);

    if (N <= 0) {
        fprintf(stderr, "Ошибка: N должно быть больше нуля\n");
        return 1;
    }

    //вычисляем адрес локальной сети
    uint32_t local_network = gw_ip & mask;

    printf("Шлюз:          "); print_ip(gw_ip); printf("\n");
    printf("Маска:         "); print_ip(mask); printf("\n");
    printf("Локальная сеть:"); print_ip(local_network); printf("\n\n");

    // генератор случайных чисел
    srand(time(NULL));
    int local_count = 0;
    int remote_count = 0;

    for (int i = 0; i < N; i++) {
        uint32_t dest_ip = ((uint32_t)(rand() & 0xFF) << 24) |
                           ((uint32_t)(rand() & 0xFF) << 16) |
                           ((uint32_t)(rand() & 0xFF) << 8)  |
                           ((uint32_t)(rand() & 0xFF));

        // определяем сеть назначения
        uint32_t dest_network = dest_ip & mask;

        // проверяем, совпадает ли сеть назначения с нашей
        if (dest_network == local_network) {
            local_count++;
        } else {
            remote_count++;
        }
    }

    printf("Обработано пакетов: %d\n", N);
    printf("В своей подсети:    %d (%.4f%%)\n", local_count, (double)local_count / N * 100.0);
    printf("В других сетях:     %d (%.4f%%)\n", remote_count, (double)remote_count / N * 100.0);

    return 0;
}
