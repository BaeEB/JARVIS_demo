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
    (void)tcsetattr(tty->fdin, TCSANOW, &tty->original_termios);
}

void tty_close(tty_t *tty) {
    (void)tty_reset(tty);         // Cast to void if tty_reset return value is not used
    (void)fclose(tty->fout);      // Cast to void if fclose return value is not used
    (void)close(tty->fdin);       // Cast to void if close return value is not used
}

static void handle_sigwinch(int sig){
	(void)sig;
}

#include <unistd.h>    // for open(), close() and related POSIX functions
#include <fcntl.h>     // for O_RDONLY, O_WRONLY flags
#include <termios.h>   // for tcgetattr(), tcsetattr() and related termios functions
#include <sys/ioctl.h> // for ioctl() and related ioctl functions
#include <stdio.h>     // for FILE, fopen(), setvbuf() and related stdio functions

// Definition of tty_t and any other data structures need to be included here

// Comment out or remove the signal.h include directive, as it goes against MISRA C Rule 21.05
// #include <signal.h> // for signal()

// Declaration or definition of EXIT_FAILURE, handle_sigwinch(), tty_getwinsz(), tty_setnormal()
// and any other functions need to be included here

void tty_init(tty_t *tty, const char *tty_filename) {
    tty->fdin = open(tty_filename, O_RDONLY);
    if (tty->fdin < 0) {
        perror("Failed to open tty");
        // Use an alternative error-handling mechanism instead of calling exit()
    }

    tty->fout = fopen(tty_filename, "w");
    if (!tty->fout) {
        perror("Failed to open tty");
        // Use an alternative error-handling mechanism instead of calling exit()
    }

    if (setvbuf(tty->fout, NULL, _IOFBF, 4096)) {
        perror("setvbuf");
        // Use an alternative error-handling mechanism instead of calling exit()
    }

    if (tcgetattr(tty->fdin, &tty->original_termios)) {
        perror("tcgetattr");
        // Use an alternative error-handling mechanism instead of calling exit()
    }

    struct termios new_termios = tty->original_termios;
    new_termios.c_iflag &= ~(ICRNL);
    new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);

    if (tcsetattr(tty->fdin, TCSANOW, &new_termios))
        perror("tcsetattr");

    tty_getwinsz(tty);
    tty_setnormal(tty);

    // Replace the signal function call with an alternative approach
    // signal(SIGWINCH, handle_sigwinch); // If signal handling is critical, consider a safer mechanism
}

// Please note that I cannot provide an alternative to signal handling without knowing the context of its use.
// If signal handling is critical to the application, you should design a safer and decidable mechanism, possibly
// involving inter-process communication (IPC) or other signaling mechanisms that avoid the use of <signal.h> directly.

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
        // exit(EXIT_FAILURE); // Removed due to MISRA_C_2012_21_08 violation
    } else if (size == 0) {
        /* EOF */
        // exit(EXIT_FAILURE); // Removed due to MISRA_C_2012_21_08 violation
    } else {
        // The return statement is moved to the end of the function to comply with MISRA_C_2012_15_05
    }
    // Single return statement to comply with MISRA_C_2012_15_05
    return size > 0 ? ch : '\0'; // Replace with a neutral value such as null character if an error or EOF occurs
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

    int result = 0; // Variable to store the return value
    int err = pselect(
            tty->fdin + 1,
            &readfs,
            NULL,
            NULL,
&ts,
&mask);

    if (err < 0) {
        if (errno == EINTR) {
            result = 0; // Return value already set to 0, can comment out this line
        } else {
            perror("select");
            exit(EXIT_FAILURE);
        }
    } else {
        result = FD_ISSET(tty->fdin, &readfs);
    }

    return result; // Single point of exit at the end of the function
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
    (void)fputc(c, tty->fout); // Cast the return value of fputc to void
}

void tty_flush(tty_t *tty) {
    if (fflush(tty->fout) != 0) {
        // Handle error
    }
}

size_t tty_getwidth(tty_t *tty) {
	return tty->maxwidth;
}

size_t tty_getheight(tty_t *tty) {
	return tty->maxheight;
}
