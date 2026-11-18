#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_binary(unsigned int mode) {
    for (int i = 8; i >= 0; i--) {
        printf("%d", (mode >> i) & 1);
        if (i % 3 == 0 && i != 0) printf(" ");
    }
    printf("\n");
}

void print_symbolic(unsigned int mode) {
    char str[10];
    str[0] = (mode & 0400) ? 'r' : '-';
    str[1] = (mode & 0200) ? 'w' : '-';
    str[2] = (mode & 0100) ? 'x' : '-';
    str[3] = (mode & 0040) ? 'r' : '-';
    str[4] = (mode & 0020) ? 'w' : '-';
    str[5] = (mode & 0010) ? 'x' : '-';
    str[6] = (mode & 0004) ? 'r' : '-';
    str[7] = (mode & 0002) ? 'w' : '-';
    str[8] = (mode & 0001) ? 'x' : '-';
    str[9] = '\0';
    printf("%s\n", str);
}

unsigned int parse_input(const char *input) {
    unsigned int mode = 0;
    
    if (input[0] >= '0' && input[0] <= '7') {
        mode = strtol(input, NULL, 8);
        return mode & 0777;
    }
    
    if (strlen(input) == 9) {
        if (input[0] == 'r') mode |= 0400;
        if (input[1] == 'w') mode |= 0200;
        if (input[2] == 'x') mode |= 0100;
        if (input[3] == 'r') mode |= 0040;
        if (input[4] == 'w') mode |= 0020;
        if (input[5] == 'x') mode |= 0010;
        if (input[6] == 'r') mode |= 0004;
        if (input[7] == 'w') mode |= 0002;
        if (input[8] == 'x') mode |= 0001;
        return mode;
    }
    
    return 0;
}

const char* skip_chmod(const char *input) {
    if (strncmp(input, "chmod ", 6) == 0) {
        return input + 6;
    }
    return input; 
}

unsigned int apply_chmod(unsigned int mode, const char *cmd) {
    if (cmd[0] >= '0' && cmd[0] <= '7') {
        return strtol(cmd, NULL, 8) & 0777;
    }

    char *copy = strdup(cmd);
    char *token = strtok(copy, ", "); 
    
    while (token != NULL) {
        int who_mask = 0;
        int op = 0;
        int perms = 0;
        int i = 0;
        
        // Разбор who (u, g, o, a)
        while (token[i] == 'u' || token[i] == 'g' || token[i] == 'o' || token[i] == 'a') {
            if (token[i] == 'u') who_mask |= 0700;
            if (token[i] == 'g') who_mask |= 0070;
            if (token[i] == 'o') who_mask |= 0007;
            if (token[i] == 'a') who_mask |= 0777;
            i++;
        }
        if (who_mask == 0) who_mask = 0777; // по умолчанию все
        
        // Разбор оператора
        if (token[i] == '+') { op = 1; i++; }
        else if (token[i] == '-') { op = -1; i++; }
        else if (token[i] == '=') { op = 2; i++; }
        
        // Разбор прав (r, w, x)
        while (token[i] != '\0') {
            if (token[i] == 'r') perms |= 4;
            if (token[i] == 'w') perms |= 2;
            if (token[i] == 'x') perms |= 1;
            i++;
        }
        
        // Применение
        if (op == 1) { 
            if (who_mask & 0700) mode |= (perms << 6); 
            if (who_mask & 0070) mode |= (perms << 3); 
            if (who_mask & 0007) mode |= perms;        
        } 
        else if (op == -1) { 
            if (who_mask & 0700) mode &= ~(perms << 6); 
            if (who_mask & 0070) mode &= ~(perms << 3); 
            if (who_mask & 0007) mode &= ~perms;        
        } 
        else if (op == 2) { 
            if (who_mask & 0700) { mode &= ~0700; mode |= (perms << 6); }
            if (who_mask & 0070) { mode &= ~0070; mode |= (perms << 3); }
            if (who_mask & 0007) { mode &= ~0007; mode |= perms; }
        }
        
        token = strtok(NULL, ", ");
    }
    free(copy);
    return mode & 0777;
}

void print_all_formats(unsigned int mode) {
    printf("dig: ");
    print_symbolic(mode);
    printf("letter:  %03o\n", mode);
    printf("bit:   ");
    print_binary(mode);
}

int main() {
    int choice;
    unsigned int current_mode = 0;
    char input[256];
    
    while (1) {
        printf("1. input perms\n");
        printf("2. stat\n");
        printf("3. chmod\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                printf("input perms (755 or rwxr-xr-x): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                current_mode = parse_input(input);
                print_all_formats(current_mode);
                break;
                
            case 2: {
                printf("input filename: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                
                struct stat st;
                if (stat(input, &st) == 0) {
                    current_mode = st.st_mode & 0777;
                    printf("perms from stat\n");
                    print_all_formats(current_mode);
                } else {
                    printf("err: file not find\n");
                }
                break;
            }
                
            case 3:
                printf("input chmod: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                // Применяем skip_chmod
                current_mode = apply_chmod(current_mode, skip_chmod(input));
                printf("after change\n");
                print_all_formats(current_mode);
                break;
                
            case 0:
                return 0;
            default:
                printf("invalid choice\n");
        }
    }
    return 0;
}
