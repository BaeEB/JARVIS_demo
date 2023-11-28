#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "match.h"
#include "tty_interface.h"
#include "../config.h"

static int isprint_unicode(char c) {
    return isprint(c) || (unsigned char)c & (1u << 7); // Ensure the operands have the same essential type
}

static int is_boundary(char c) {
    return ((unsigned char)~c & (1u << 7)) != 0u || ((unsigned char)c & (1u << 6)) != 0u;
}

static void clear(tty_interface_t *state) {
	tty_t *tty = state->tty;

	tty_setcol(tty, 0);
	size_t line = 0;
	while (line++ < state->options->num_lines + (state->options->show_info ? 1 : 0)) {
		tty_newline(tty);
	}
	tty_clearline(tty);
	if (state->options->num_lines > 0) {
		tty_moveup(tty, line - 1);
	}
	tty_flush(tty);
}

static void draw_match(tty_interface_t *state, const char *choice, int selected) {
	tty_t *tty = state->tty;
	options_t *options = state->options;
	char *search = state->last_search;

	int n = strlen(search);
	size_t positions[MATCH_MAX_LEN];
	for (int i = 0; i < n + 1 && i < MATCH_MAX_LEN; i++)
		positions[i] = -1;

	score_t score = match_positions(search, choice, &positions[0]);

	if (options->show_scores) {
		if (score == SCORE_MIN) {
			tty_printf(tty, "(     ) ");
		} else {
			tty_printf(tty, "(%5.2f) ", score);
		}
	}

	if (selected)
#ifdef TTY_SELECTION_UNDERLINE
		tty_setunderline(tty);
#else
		tty_setinvert(tty);
#endif

	tty_setnowrap(tty);
	for (size_t i = 0, p = 0; choice[i] != '\0'; i++) {
		if (positions[p] == i) {
			tty_setfg(tty, TTY_COLOR_HIGHLIGHT);
			p++;
		} else {
			tty_setfg(tty, TTY_COLOR_NORMAL);
		}
		if (choice[i] == '\n') {
			tty_putc(tty, ' ');
		} else {
			tty_printf(tty, "%c", choice[i]);
		}
	}
	tty_setwrap(tty);
	tty_setnormal(tty);
}

static void draw(tty_interface_t *state) {
    tty_t *tty = state->tty;
    choices_t *choices = state->choices;
    options_t *options = state->options;

    unsigned int num_lines = options->num_lines;
    size_t start = 0;
    size_t current_selection = choices->selection;
    if (current_selection + options->scrolloff >= num_lines) {
        start = current_selection + options->scrolloff - num_lines + 1;
        size_t available = choices_available(choices);
        if ((start + num_lines >= available) && (available > 0)) {
            start = available - num_lines;
        }
    }

    tty_setcol(tty, 0);
    tty_printf(tty, "%s%s", options->prompt, state->search);
    tty_clearline(tty);

    if (options->show_info) {
        tty_printf(tty, "\n[%lu/%lu]", choices->available, choices->size);
        tty_clearline(tty);
    }

    for (size_t i = start; i < (start + num_lines); i++) {
        tty_printf(tty, "\n");
        tty_clearline(tty);
        const char *choice = choices_get(choices, i);
        if (choice != NULL) {
            draw_match(state, choice, (i == choices->selection));
        }
    }

    if ((num_lines + options->show_info) != 0)
        tty_moveup(tty, num_lines + options->show_info);

    tty_setcol(tty, 0);
    fputs(options->prompt, tty->fout);
    for (size_t i = 0; i < state->cursor; i++)
        fputc(state->search[i], tty->fout);
    tty_flush(tty);
}

static void update_search(tty_interface_t *state) {
	choices_search(state->choices, state->search);
	strcpy(state->last_search, state->search);
}

static void update_state(tty_interface_t *state) {
    if (strcmp(state->last_search, state->search) != 0) { // Corrected to comply with MISRA_C_2012_14_04
        update_search(state);
        draw(state);
    }
}

/* Non-compliant code section */
/* Output function - isolating printf usage for potential replacement with MISRA-compliant functions later */
static void output(const char *str) {
    printf("%s\n", str); /* Not MISRA-compliant */
}

/* Main function adjusted to use the output function*/
static void action_emit(tty_interface_t *state) {
    update_state(state);

    /* Reset the tty as close as possible to the previous state */
    clear(state);

    /* ttyout should be flushed before outputting on stdout */
    tty_close(state->tty);

    const char *selection = choices_get(state->choices, state->choices->selection);
    if (selection) {
        /* output the selected result */
        output(selection);
    } else {
        /* No match, output the query instead */
        output(state->search);
    }

    state->exit = EXIT_SUCCESS;
}

static void action_del_char(tty_interface_t *state) {
    size_t length = strlen(state->search);
    if (state->cursor == 0) {
        // Changed to comply with MISRA_C_2012_15_05
        // Early return removed
    } else {
        size_t original_cursor = state->cursor;

        do {
            state->cursor--;
        } while (!is_boundary(state->search[state->cursor]) && state->cursor);

        memmove(&state->search[state->cursor], &state->search[original_cursor], length - original_cursor + 1);
    }
    // Single point of exit
}

#include <ctype.h>   /* Required for isspace */
#include <string.h>  /* Required for strlen and memmove */

static void action_del_word(tty_interface_t *state) {
    size_t original_cursor = state->cursor;
    size_t cursor = state->cursor;

    while ((cursor > 0u) && (isspace((int)(unsigned char)state->search[cursor - 1u]) != 0)) {
        cursor--;
    }

    while ((cursor > 0u) && (isspace((int)(unsigned char)state->search[cursor - 1u]) == 0)) {
        cursor--;
    }

    (void)memmove(&state->search[cursor], &state->search[original_cursor], strlen(&state->search[original_cursor]) + 1u);
    state->cursor = cursor;
}

static void action_del_all(tty_interface_t *state) {
    /* Explicitly cast the return value of memmove to (void) to indicate it is unused */
    (void)memmove(state->search, &state->search[state->cursor], strlen(state->search) - state->cursor + 1);
    state->cursor = 0;
}

static void action_prev(tty_interface_t *state) {
	update_state(state);
	choices_prev(state->choices);
}

static void action_ignore(const tty_interface_t *state) {
    (void)state;
}

static void action_next(tty_interface_t *state) {
	update_state(state);
	choices_next(state->choices);
}

static void action_left(tty_interface_t *state) {
	if (state->cursor > 0) {
		state->cursor--;
		while (!is_boundary(state->search[state->cursor]) && state->cursor) { /* Compound statement added */
			state->cursor--;
		} /* Compound statement closed */
	}
}

static void action_right(tty_interface_t *state) {
    if (state->cursor < strlen(state->search)) {
        state->cursor++;
        while (!is_boundary(state->search[state->cursor])) {
            state->cursor++;
        }
    }
}

static void action_beginning(tty_interface_t *state) {
	state->cursor = 0;
}

static void action_end(tty_interface_t *state) {
	state->cursor = strlen(state->search);
}

static void action_pageup(tty_interface_t *state) {
    update_state(state);
    size_t i;
    for (i = 0; (i < state->options->num_lines) && (state->choices->selection > 0); i++) {
        choices_prev(state->choices);
    }
}

static void action_pagedown(tty_interface_t *state) {
    update_state(state);
    for (size_t i = 0; i < state->options->num_lines; ++i) {
        if (state->choices->selection < state->choices->available - 1) {
            choices_next(state->choices);
        } else {
            break;
        }
    }
}

static void action_autocomplete(tty_interface_t *state) {
   update_state(state);
   const char *current_selection = choices_get(state->choices, state->choices->selection);
   if (current_selection != NULL) { /* Compliant with MISRA C:2012 Rule 14.04 */
       strncpy(state->search, current_selection, SEARCH_SIZE_MAX - 1); /* Potentially corrected code to prevent buffer overflow */
       state->search[SEARCH_SIZE_MAX - 1] = '\0'; /* Ensure null-termination */
       state->cursor = strlen(state->search);
   }
}

static void action_exit(tty_interface_t *state) {
	clear(state);
	tty_close(state->tty);

	state->exit = EXIT_FAILURE;
}

static void append_search(tty_interface_t *state, char ch) {
	char *search = state->search;
	size_t search_size = strlen(search);
	if (search_size < SEARCH_SIZE_MAX) {
		memmove(&search[state->cursor+1], &search[state->cursor], search_size - state->cursor + 1);
		search[state->cursor] = ch;

		state->cursor++;
	}
}

void tty_interface_init(tty_interface_t * const state, tty_t * const tty, choices_t * const choices, options_t * const options) {
    if (state != NULL && tty != NULL && choices != NULL && options != NULL) {
        state->tty = tty;
        state->choices = choices;
        state->options = options;
        state->ambiguous_key_pending = 0;
    
        state->input[0] = '\0'; // Ensure we have a null-terminated string
        state->search[0] = '\0'; // Ensure we have a null-terminated string
        state->last_search[0] = '\0'; // Ensure we have a null-terminated string
    
        state->exit = -1;
    
        if (options->init_search != NULL) {
            strncpy(state->search, options->init_search, SEARCH_SIZE_MAX - 1);
            state->search[SEARCH_SIZE_MAX - 1] = '\0'; // Ensure null-termination
        }
    
        state->cursor = strlen(state->search);
    
        update_search(state);
    }
}

typedef struct {
	const char *key;
	void (*action)(tty_interface_t *);
} keybinding_t;

#define KEY_CTRL(key) ((const char[]){((key) - ('@')), '\0'})

static const keybinding_t keybindings[] = {{"\x1b", action_exit},       /* ESC */
					   {"\x7f", action_del_char},	/* DEL */

					   {KEY_CTRL('H'), action_del_char}, /* Backspace (C-H) */
					   {KEY_CTRL('W'), action_del_word}, /* C-W */
					   {KEY_CTRL('U'), action_del_all},  /* C-U */
					   {KEY_CTRL('I'), action_autocomplete}, /* TAB (C-I ) */
					   {KEY_CTRL('C'), action_exit},	 /* C-C */
					   {KEY_CTRL('D'), action_exit},	 /* C-D */
					   {KEY_CTRL('G'), action_exit},	 /* C-G */
					   {KEY_CTRL('M'), action_emit},	 /* CR */
					   {KEY_CTRL('P'), action_prev},	 /* C-P */
					   {KEY_CTRL('N'), action_next},	 /* C-N */
					   {KEY_CTRL('K'), action_prev},	 /* C-K */
					   {KEY_CTRL('J'), action_next},	 /* C-J */
					   {KEY_CTRL('A'), action_beginning},    /* C-A */
					   {KEY_CTRL('E'), action_end},		 /* C-E */

					   {"\x1bOD", action_left}, /* LEFT */
					   {"\x1b[D", action_left}, /* LEFT */
					   {"\x1bOC", action_right}, /* RIGHT */
					   {"\x1b[C", action_right}, /* RIGHT */
					   {"\x1b[1~", action_beginning}, /* HOME */
					   {"\x1b[H", action_beginning}, /* HOME */
					   {"\x1b[4~", action_end}, /* END */
					   {"\x1b[F", action_end}, /* END */
					   {"\x1b[A", action_prev}, /* UP */
					   {"\x1bOA", action_prev}, /* UP */
					   {"\x1b[B", action_next}, /* DOWN */
					   {"\x1bOB", action_next}, /* DOWN */
					   {"\x1b[5~", action_pageup},
					   {"\x1b[6~", action_pagedown},
					   {"\x1b[200~", action_ignore},
					   {"\x1b[201~", action_ignore},
					   {NULL, NULL}};

#undef KEY_CTRL

static void handle_input(tty_interface_t *state, const char *s, int handle_ambiguous_key) {
    state->ambiguous_key_pending = 0;

    char *input = state->input;
    // Use strncat for safe concatenation with a limit on the number of characters.
    strncat(state->input, s, sizeof(state->input) - strlen(state->input) - 1);

    /* Figure out if we have completed a keybinding and whether we're in the
     * middle of one (both can happen, because of Esc). */
    int found_keybinding = -1;
    int in_middle = 0;
    for (int i = 0; keybindings[i].key; i++) {
        if (strcmp(input, keybindings[i].key) == 0) {
            found_keybinding = i;
        } else if (strncmp(input, keybindings[i].key, strlen(state->input)) == 0) {
            in_middle = 1;
        }
    }

    /* If we have an unambiguous keybinding, run it, and clear the input buffer. */
    if (found_keybinding != -1 && (!in_middle || handle_ambiguous_key)) {
        keybindings[found_keybinding].action(state);
        input[0] = '\0'; // Clear the input buffer in a safe way
    } else if (found_keybinding != -1 && in_middle) {
        /* We could have a complete keybinding, or could be in the middle of one.
         * We'll need to wait a few milliseconds to find out. */
        state->ambiguous_key_pending = 1;
    } else if (!in_middle) {
        /* No matching keybinding, add to search */
        for (int i = 0; input[i]; i++) {
            if (isprint_unicode(input[i])) {
                append_search(state, input[i]);
            }
        }

        input[0] = '\0'; // Clear the input buffer in a safe way
    }
}

int tty_interface_run(tty_interface_t *state) {
	int exit_status = -1;
	draw(state);
	
	for (;;) {
		do {
			while(!tty_input_ready(state->tty, -1, 1)) {
				/* We received a signal (probably WINCH) */
				draw(state);
			}
			
			char s[2] = {tty_getchar(state->tty), '\0'};
			handle_input(state, s, 0);
			
			if (state->exit >= 0) {
				exit_status = state->exit;
				break;
			}
			
			draw(state);
		} while (tty_input_ready(state->tty, state->ambiguous_key_pending ? KEYTIMEOUT : 0, 0));
		
		if (exit_status >= 0)
			break;
		
		if (state->ambiguous_key_pending) {
			char s[1] = "";
			handle_input(state, s, 1);
			
			if (state->exit >= 0) {
				exit_status = state->exit;
				break;
			}
		}
		
		update_state(state);
	}
	
	return exit_status;
}
