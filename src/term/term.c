#include <termios.h>
#include <stdlib.h>

struct termios current_term;
struct termios old_term;

void restoreTermMode() {
    tcsetattr(0, TCSANOW, &old_term);
}

int enableTermRaw() {
    int ret = tcgetattr(0, &current_term);

    if (ret == -1) {
        return ret;
    }

    old_term = current_term;
    atexit(restoreTermMode);

    current_term.c_lflag &= ~( ICANON | ECHO | ISIG );

    ret = tcsetattr(0, TCSANOW, &current_term);
    return ret;
}
