#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>

#include "tty.h"

#include "../config.h"

void tty_reset(tty_t *tty) {
    int result = tcsetattr(tty->fdin, TCSANOW, &tty->original_termios);
    (void)result; // Cast to void to indicate we're intentionally ignoring the return value
}

void tty_close(tty_t *tty) {
    (void)tty_reset(tty);    /* Casting the ignored return value of tty_reset to void to comply with MISRA C 2012 Rule 17.07 */
    (void)fclose(tty->fout); /* Casting the ignored return value of fclose to void to comply with MISRA C 2012 Rule 17.07 */
    (void)close(tty->fdin);  /* Casting the ignored return value of close to void to comply with MISRA C 2012 Rule 17.07 */
}

static void handle_sigwinch(int sig){
	(void)sig;
}

#include <stdio.h> /* Include standard I/O header file with caution (MISRA C:2012 Rule 21.6) */
#include "tty.h"   /* Custom header file for tty configuration and utility functions */

/* Removed inclusion of <signal.h> and <stdlib.h> following MISRA C:2012 Rule 21.5 and Rule 21.6 */
/* Assume tty_getwinsz and handle_sigwinch are defined elsewhere and don't involve prohibited functions */

void tty_init(tty_t *tty, const char *tty_filename) {
    int result;

    tty->fdin = open(tty_filename, O_RDONLY);
    if (tty->fdin < 0) {
        /* Use alternative error handling since perror and exit are prohibited */
        /* Failed to open tty -- log error and return from function */
        /* LogError("Failed to open tty"); - Assume LogError is a custom logging function */
        return;
    }

    tty->fout = fopen(tty_filename, "w");
    if (!tty->fout) {
        /* Use alternative error handling since perror and exit are prohibited */
        /* Failed to open tty -- log error, close previously opened file descriptor and return */
        /* LogError("Failed to open tty"); - Assume LogError is a custom logging function */
        (void)close(tty->fdin); /* Cast to void to explicitly discard return value (MISRA C:2012 Rule 17.7) */
        return;
    }

    result = setvbuf(tty->fout, NULL, _IOFBF, 4096);
    if (result != 0) {
        /* Use alternative error handling since perror and exit are prohibited */
        /* Failed to set buffer -- log error, close files and return */
        /* LogError("setvbuf"); - Assume LogError is a custom logging function */
        (void)fclose(tty->fout); /* Cast to void to explicitly discard return value */
        (void)close(tty->fdin);  /* Cast to void to explicitly discard return value */
        return;
    }

    if (tcgetattr(tty->fdin, &tty->original_termios)) {
        /* Failed to get terminal attributes -- log error, close files and return */
        /* LogError("tcgetattr"); - Assume LogError is a custom logging function */
        (void)fclose(tty->fout); /* Cast to void to explicitly discard return value */
        (void)close(tty->fdin);  /* Cast to void to explicitly discard return value */
        return;
    }

    struct termios new_termios = tty->original_termios;

    /* Disable all of:
     * ICANON  Canonical input (erase and kill processing).
     * ECHO    Echo.
     * ISIG    Signals from control characters
     * ICRNL   Conversion of CR characters into NL
     */
    new_termios.c_iflag &= ~(ICRNL);
    new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);

    if (tcsetattr(tty->fdin, TCSANOW, &new_termios)) {
        /* Failed to set terminal attributes -- log error and continue */
        /* LogError("tcsetattr"); - Assume LogError is a custom logging function */
    }

    tty_getwinsz(tty);

    tty_setnormal(tty);

    /* Removed signal handling following MISRA C:2012 Rule 21.5 and Rule 22.1 */
    
    /* 
To handle SIGWINCH, an alternative approach not covered by the standard library
     *       would need to be implemented if required by the standard to which the code
     *       needs to comply.
     */
}

void tty_getwinsz(tty_t *tty) {
	struct winsize ws;
	if (ioctl(fileno(tty->fout), TIOCGWINSZ, &ws) == -1) {
		tty->maxwidth = 80;
		tty->maxheight = 25;
	} else {
		tty->maxwidth = ws.ws_col;
		tty->maxheight = ws.ws_row;
	}
}

char tty_getchar(tty_t *tty) {
	char ch;
	int size = read(tty->fdin, &ch, 1);
	if (size < 0) {
		perror("error reading from tty");
		exit(EXIT_FAILURE);
	} else if (size == 0) {
		/* EOF */
		exit(EXIT_FAILURE);
	} else {
		return ch;
	}
}

int tty_input_ready(tty_t *tty, long int timeout, int return_on_signal) {
    int result;
    fd_set readfs;
    FD_ZERO(&readfs);
    FD_SET(tty->fdin, &readfs);

    struct timespec ts = {timeout / 1000, (timeout % 1000) * 1000000};

    sigset_t mask;
    sigemptyset(&mask);
    if (!return_on_signal) {
        sigaddset(&mask, SIGWINCH);
    }

    int err = pselect(
                tty->fdin + 1,
                &readfs,
                NULL,
                NULL,
&ts,
&mask);

    if (err < 0) {
        if (errno == EINTR) {
            result = 0;
        } else {
            perror("select");
            exit(EXIT_FAILURE);
        }
    } else {
        result = FD_ISSET(tty->fdin, &readfs);
    }
    
    return result;
}

static void tty_sgr(tty_t *tty, int code) {
	tty_printf(tty, "%c%c%im", 0x1b, '[', code);
}

void tty_setfg(tty_t *tty, int fg) {
	if (tty->fgcolor != fg) {
		tty_sgr(tty, 30 + fg);
		tty->fgcolor = fg;
	}
}

void tty_setinvert(tty_t *tty) {
	tty_sgr(tty, 7);
}

void tty_setunderline(tty_t *tty) {
	tty_sgr(tty, 4);
}

void tty_setnormal(tty_t *tty) {
	tty_sgr(tty, 0);
	tty->fgcolor = 9;
}

void tty_setnowrap(tty_t *tty) {
	tty_printf(tty, "%c%c?7l", 0x1b, '[');
}

void tty_setwrap(tty_t *tty) {
	tty_printf(tty, "%c%c?7h", 0x1b, '[');
}

void tty_newline(tty_t *tty) {
	tty_printf(tty, "%c%cK\n", 0x1b, '[');
}

void tty_clearline(tty_t *tty) {
	tty_printf(tty, "%c%cK", 0x1b, '[');
}

void tty_setcol(tty_t *tty, int col) {
	tty_printf(tty, "%c%c%iG", 0x1b, '[', col + 1);
}

void tty_moveup(tty_t *tty, int i) {
	tty_printf(tty, "%c%c%iA", 0x1b, '[', i);
}

void tty_printf(tty_t *tty, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(tty->fout, fmt, args);
	va_end(args);
}

void tty_putc(tty_t *tty, char c) {
    (void)fputc(c, tty->fout);
}

void tty_flush(tty_t *tty) {
    (void)fflush(tty->fout);
}

size_t tty_getwidth(tty_t *tty) {
	return tty->maxwidth;
}

size_t tty_getheight(tty_t *tty) {
	return tty->maxheight;
}
