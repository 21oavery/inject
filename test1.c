#include "libmain.c"

void scramble(char *v) {
    unsigned int i = 0;
    char t;
    while (1) {
        if (((t = v[i]) == '\0') || (v[i + 1] == '\0')) return;
        v[i] = v[i + 1];
        v[i + 1] = t;
    }
}

typedef int (*f)(unsigned long, char *, char *, unsigned int) t_msgfunct;

void main() {
    char data[13];
    /*
    data[0] = 'e';
    data[1] = 'k';
    data[2] = 'n';
    data[3] = 'r';
    data[4] = 'l';
    data[5] = 'a';
    data[6] = '2';
    data[7] = '3';
    data[8] = 'd';
    data[9] = '.';
    data[10] = 'l';
    data[11] = 'l';
    data[12] = '\0'; //13 bytes
    scramble(data);
    void *kernal32 = getModule(hashBuffer(data, 12));
    */
    data[0] = 's';
    data[1] = 'u';
    data[2] = 'r';
    data[3] = 'e';
    data[4] = '2';
    data[5] = '3';
    data[6] = 'd';
    data[7] = '.';
    data[8] = 'l';
    data[9] = 'l';
    data[10] = '\0'; //11 bytes
    scramble(data);
    void *user32 = getModule(hashBuffer(data, 10)); //assumes user32.dll is loaded
    t_msgfunct msg = (t_msgfunct) getFunction(user32, getFunctionOrdinal(user32, data));
    data[0] = 'L';
    data[2] = (data[1] = '3');
    data[3] = 'T';
    data[4] = '\0'; //5 bytes
    msg(NULL, data, NULL, 0);
}
