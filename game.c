#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#define HEIGHT 25
#define LENGTH 80

int** init_data();
int kbhit(void);
void read_data(int** data);
int nanosleep(const struct timespec* req, struct timespec* rem);
int life_data(int** data);
void copy_data(int** data, int** data_2);
void clear_data(int** data);
void output(int** data, int gen, int sec);
void mssleep(long int ms);
int main() {
    int** data = init_data();
    read_data(data);
    int sec = 150;
    int speed = 3;
    int gen = 1;
    printf("\033[0d\033[2J");
    while (1) {
        output(data, gen, speed);
        life_data(data);
        printf("\033[0d\033[2J");
        mssleep(sec);
        if (kbhit()) {
            char ch = getchar();
            if (ch == 'q' || ch == 'Q')
                break;
            else if (ch == 'w' || ch == 'W') {
                if (sec - 50 != 0) {
                    sec -= 50;
                    speed += 1;
                }
            } else if (ch == 's' || ch == 'S') {
                if (sec + 50 != 350) {
                    sec += 50;
                    speed -= 1;
                }
            }
        }
        gen++;
    }
    printf("\033[0d\033[2J");
    clear_data(data);
    return 0;
}

int** init_data() {
    int** data = (int**)calloc(HEIGHT, sizeof(int*));
    if (data != NULL) {
        for (int i = 0; i < HEIGHT; i++) {
            data[i] = (int*)calloc(LENGTH, sizeof(int));
        }
    }
    return data;
}

void read_data(int** data) {
    FILE* file = NULL;

    int height_index = 0;
    int lenght_index = 0;
    int step = 0;
    step = fgetc(stdin);
    while (step != EOF || height_index < HEIGHT) {
        if (step == '1') {
            data[height_index][lenght_index] = 1;
            lenght_index++;
        } else if (step == '\n') {
            height_index++;
            lenght_index = 0;
        } else if (step == '0') {
            lenght_index++;
        }
        step = fgetc(stdin);
    }
    file = freopen("/dev/tty", "r", stdin);
    if (file == NULL) {
        printf("n/a");
        exit(1);
    }
}

int life_data(int** data) {
    int** data_2 = init_data();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < LENGTH; j++) {
            int count = 0;
            int x, y;
            int cell = data[i][j];
            x = (i - 1 == -1 ? HEIGHT - 1 : i - 1) % HEIGHT;
            y = (j - 1 == -1 ? LENGTH - 1 : j - 1) % LENGTH;
            count += data[x][y];
            count += data[x][j];
            count += data[x][(j + 1) % LENGTH];
            count += data[i][y];
            count += data[i][(j + 1) % LENGTH];
            count += data[(i + 1) % HEIGHT][y];
            count += data[(i + 1) % HEIGHT][j];
            count += data[(i + 1) % HEIGHT][(j + 1) % LENGTH];
            if (cell == 0 && count == 3) {
                data_2[i][j] = 1;
            }
            if (cell == 1 && (count == 2 || count == 3)) {
                data_2[i][j] = 1;
            }
            if (cell == 1 && (count < 2 || count > 3)) {
                data_2[i][j] = 0;
            }
        }
    }
    copy_data(data, data_2);
    clear_data(data_2);
    return 0;
}

void copy_data(int** data, int** data_2) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < LENGTH; j++) {
            data[i][j] = data_2[i][j];
        }
    }
}

void clear_data(int** data) {
    for (int i = 0; i < HEIGHT; i++) {
        free(data[i]);
    }
    free(data);
}
void output(int** data, int gen, int sec) {
    printf("\n");
    printf("Текущее поколение:%d\nТекущая скорость:%d\n", gen, sec);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < LENGTH; j++) {
            if (data[i][j] == 1)
                printf("*");
            else
                printf(" ");
        }
        if (i < HEIGHT - 1) printf("\n");
    }
    printf("\nДля выхода нажмите q\nДля увеличения скорости нажмите w\nДля уменьшения скорости нажмите s\n");
}
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;
    int flag = 0;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        flag = 1;
    }

    return flag;
}
void mssleep(long int ms) {
    struct timespec e;
    struct timespec q = {(int)(ms / 1000), (ms % 1000) * 1000000};
    nanosleep(&q, &e);
}
