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
	tcsetattr(tty->fdin, TCSANOW, &tty->original_termios);
}

void tty_close(tty_t *tty) {
	tty_reset(tty);
	fclose(tty->fout);
	close(tty->fdin);
}

static void handle_sigwinch(int sig){
	(void)sig;
}

void tty_init(tty_t *tty, const char *tty_filename) {
	tty->fdin = open(tty_filename, O_RDONLY);
	if (tty->fdin < 0) {
		perror("Failed to open tty");
	}
	else {
		tty->fout = fopen(tty_filename, "w");
		if (!tty->fout) {
			perror("Failed to open tty");
		}
		else {
			if (setvbuf(tty->fout, NULL, _IOFBF, 4096)) {
				perror("setvbuf");
			}
			else if (tcgetattr(tty->fdin, &tty->original_termios)) {
				perror("tcgetattr");
			}
			else {
				struct termios new_termios = tty->original_termios;

				/* Disable certain input flags */
				new_termios.c_iflag &= ~(ICRNL);
				new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);

				if (tcsetattr(tty->fdin, TCSANOW, &new_termios)) {
					perror("tcsetattr");
				}
				else {
					tty_getwinsz(tty);
					tty_setnormal(tty);
					/* Cannot use signal as it is prohibited by MISRA C 2012 Rule 21_05 */
				}
			}
		}
	}
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
        /* Instead of calling exit(), we can handle the error without terminating the program. */
        /* Handle error appropriately (e.g., set global error flag, return special value, etc.) */
        /* For the sake of this example, assume we return an error code as char, could be -1 if not part of normal data range */
        ch = (char)-1; 
    } else if (size == 0) {
        /* EOF */
        /* Instead of calling exit(), handle EOF condition without terminating */
        /* For the sake of this example, assume we return a special value */
        ch = (char)EOF; /* Typically -1, already represented as an int in stdio.h */
    } else {
        /* Normal operation */
    }
    return ch;
}

int tty_input_ready(tty_t *tty, long int timeout, int return_on_signal) {
    fd_set readfs;
    FD_ZERO(&readfs);
    FD_SET(tty->fdin, &readfs);

    struct timespec ts = {timeout / 1000, (timeout % 1000) * 1000000};

    sigset_t mask;
    sigemptyset(&mask);
    if (!return_on_signal) {
        sigaddset(&mask, SIGWINCH);
    }

    int retval;
    do {
        retval = pselect(
            tty->fdin + 1,
            &readfs,
            NULL,
            NULL,
&ts,
&mask);
    } while ((retval == -1) && (errno == EINTR));

    if (retval < 0) {
        perror("select");
        exit(EXIT_FAILURE);
    } else {
        return FD_ISSET(tty->fdin, &readfs);
    }
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
    (void) fflush(tty->fout); // Cast to void to indicate we're intentionally ignoring the return value
}

size_t tty_getwidth(tty_t *tty) {
	return tty->maxwidth;
}

size_t tty_getheight(tty_t *tty) {
	return tty->maxheight;
}
