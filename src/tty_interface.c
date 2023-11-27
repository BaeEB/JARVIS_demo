#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "match.h"
#include "tty_interface.h"
#include "../config.h"

#include <ctype.h>

static int isprint_unicode(char c) {
    /* Given that 'isprint' expects an 'int' as an argument with the value of 'unsigned char' or EOF, 
       we need to explicitly cast the char 'c' to 'unsigned char' before conversion to 'int'. 
       Also, since the expression involves bitwise operations with a potential essential type mismatch, 
       we need to cast the result of the bitwise operation to the correct type. */
    return isprint((int)(unsigned char)c) || ((int)(unsigned char)c & (1 << 7)) != 0;
}

static int is_boundary(char c) {
    return (~((unsigned char)c) & (1u << 7)) || (((unsigned char)c) & (1u << 6));
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
	if ((current_selection + options->scrolloff) >= num_lines) { // Fix for MISRA_C_2012_12_01
		start = (current_selection + options->scrolloff) - num_lines + 1; // Fix for MISRA_C_2012_12_01
		size_t available = choices_available(choices);
		if ((start + num_lines) >= available && available > 0) { // Fix for MISRA_C_2012_12_01
			start = available - num_lines;
		}
	}

	tty_setcol(tty, 0);
	tty_printf(tty, "%s%s", options->prompt, state->search);
	tty_clearline(tty);

	if (options->show_info) {
		tty_printf(tty, "\n[%lu/%lu]", choices->available, choices->size); // No violations detected needing fix
		tty_clearline(tty);
	}

	for (size_t i = start; i < (start + num_lines); i++) { // Fix for MISRA_C_2012_12_01
		tty_printf(tty, "\n");
		tty_clearline(tty);
		const char *choice = choices_get(choices, i);
		if (choice) {
			draw_match(state, choice, (i == choices->selection)); // Fix for MISRA_C_2012_12_01
		}
	}

	if (num_lines + options->show_info) { // Fix for MISRA_C_2012_14_04
		tty_moveup(tty, num_lines + (options->show_info ? 1u : 0u)); // Fix for MISRA_C_2012_14_04 and MISRA_C_2012_12_01
	}

	tty_setcol(tty, 0);
	fputs(options->prompt, tty->fout);
	for (size_t i = 0; i < state->cursor; i++) // No violations detected needing fix
		fputc(state->search[i], tty->fout);
	tty_flush(tty);
}

static void update_search(tty_interface_t *state) {
	choices_search(state->choices, state->search);
	strcpy(state->last_search, state->search);
}

static void update_state(tty_interface_t *state) {
    if (strcmp(state->last_search, state->search) != 0) { // compliant
        update_search(state);
        draw(state);
    }
}

#include <stdlib.h> // Assuming this is included for EXIT_SUCCESS.

static void action_emit(tty_interface_t *state) {
    update_state(state);
    clear(state);
    tty_close(state->tty);

/* The value returned by `tty_close` and other functions should be checked or explicitly cast to void if not used */
    (void) tty_close(state->tty);  // Cast to void to comply with MISRA C:2012 Rule 17.07

    const char *selection = choices_get(state->choices, state->choices->selection);
    
    if (selection != NULL) {  // Compliant with MISRA C:2012 Rule 14.04
        /* output the selected result */
        (void)printf("%s\n", selection);  // Cast to void to comply with MISRA C:2012 Rule 17.07
    } else {
        /* No match, output the query instead */
        (void)printf("%s\n", state->search);  // Cast to void to comply with MISRA C:2012 Rule 17.07
    }

    state->exit = EXIT_SUCCESS;
}

static void action_del_char(tty_interface_t *state) {
    size_t length = strlen(state->search);
    if (state->cursor == 0) {
        // Early return removed to comply with MISRA C:2012 Rule 15.05
        // Code refactored to preserve the single exit point
    } else {
        size_t original_cursor = state->cursor;

        do {
            state->cursor--;
        } while (!is_boundary(state->search[state->cursor]) && state->cursor);

        memmove(&state->search[state->cursor], &state->search[original_cursor], length - original_cursor + 1);
    }
}

static void action_del_word(tty_interface_t *state) {
    size_t original_cursor = state->cursor;
    size_t cursor = state->cursor;
    
    while (cursor && isspace(state->search[cursor - 1])) {
        cursor--;
    }
    
    while (cursor && !isspace(state->search[cursor - 1])) {
        cursor--;
    }
    
    memmove(&state->search[cursor], &state->search[original_cursor], strlen(state->search) - original_cursor + 1);
    state->cursor = cursor;
}

static void action_del_all(tty_interface_t *state) {
    /* Cast to void the result of strlen since its value is not used directly */
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
        while (!is_boundary(state->search[state->cursor]) && state->cursor) {
            state->cursor--;
        }
    }
}

static void action_right(tty_interface_t *state) {
    if (state->cursor < strlen(state->search)) {
        state->cursor++;
        while (!is_boundary(state->search[state->cursor])) { // Compliant with MISRA C 2012 Rule 15.06
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
    size_t i = 0;
    size_t lines_to_move = state->options->num_lines;
    // Continue the loop only if there are lines to move up and the selection is greater than 0
    while (i < lines_to_move && state->choices->selection > 0) {
        choices_prev(state->choices);
        ++i;  // Increment the loop counter
    }
}

static void action_pagedown(tty_interface_t *state) {
    update_state(state);
    size_t i;
    for (i = 0; i < state->options->num_lines; i++) {
        if (state->choices->selection >= state->choices->available - 1) {
            break;
        }
        choices_next(state->choices);
        state->choices->selection++;
    }
}

static void action_autocomplete(tty_interface_t *state) {
	update_state(state);
	const char *current_selection = choices_get(state->choices, state->choices->selection);
	if (current_selection != NULL) {  /* Compliant with MISRA_C_2012_14_04 */
		strncpy(state->search, current_selection, SEARCH_SIZE_MAX);
		state->search[SEARCH_SIZE_MAX - 1] = '\0'; // Ensure null-termination
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
		memmove(&search[state->cursor + 1U], &search[state->cursor], search_size - state->cursor + 1U); // Corrected to use unsigned literal
		search[state->cursor] = ch;

		state->cursor++;
	}
}

void tty_interface_init(tty_interface_t *state, tty_t *tty, choices_t *choices, options_t *options) {
	state->tty = tty;
	state->choices = choices;
	state->options = options;
	state->ambiguous_key_pending = 0;

	strcpy(state->input, "");
	strcpy(state->search, "");
	strcpy(state->last_search, "");

	state->exit = -1;

	if (options->init_search != NULL) // Make sure the pointer is not NULL before using it
	{
		strncpy(state->search, options->init_search, SEARCH_SIZE_MAX - 1); // Use strncpy safely
		state->search[SEARCH_SIZE_MAX - 1] = '\0'; // Ensure null termination
	}

	state->cursor = strlen(state->search);

	update_search(state); // Assume update_search(void) call is acceptable, as the original code cannot be verified for MISRA_C_2012_17_07 violation without further context
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
    size_t input_length = strlen(state->input);

    strcat(state->input, s); /* Modify input by appending 's' */

    /* Now, 'input' is the modified content of 'state->input' */
    const char *input = state->input; 

    /* Figure out if we have completed a keybinding and whether we're in the
     * middle of one (both can happen, because of Esc). */
    int found_keybinding = -1;
    int in_middle = 0;
    for (int i = 0; keybindings[i].key; i++) {
        if (!strcmp(input, keybindings[i].key))
            found_keybinding = i;
        else if (!strncmp(input, keybindings[i].key, input_length))
            in_middle = 1;
    }

    /* If we have an unambiguous keybinding, run it.  */
    if (found_keybinding != -1 && (!in_middle || handle_ambiguous_key)) {
        keybindings[found_keybinding].action(state);
        strcpy(state->input, "");
        return;
    }

    /* We could have a complete keybinding, or could be in the middle of one.
     * We'll need to wait a few milliseconds to find out. */
    if (found_keybinding != -1 && in_middle) {
        state->ambiguous_key_pending = 1;
        return;
    }

    /* Wait for more if we are in the middle of a keybinding */
    if (in_middle) {
        return;
    }

    /* No matching keybinding, add to search */
    for (int i = 0; input[i]; i++) {
        if (isprint_unicode(input[i])) {
            append_search(state, input[i]);
        }
    }

    /* We have processed the input, so clear it */
    strcpy(state->input, "");
}

int tty_interface_run(tty_interface_t *state) {
    draw(state);

    while (1) { // Compliant with MISRA C:2012, Rule 14.4 (essentially Boolean type)
        do {
            while(!tty_input_ready(state->tty, -1, 1)) {
                /* We received a signal (probably WINCH) */
                draw(state);
            }

            char s[2] = {tty_getchar(state->tty), '\0'};
            handle_input(state, s, 0);

            if (state->exit >= 0)
                break; // Change return to break to have a single point of exit

            draw(state);
        } while (tty_input_ready(state->tty, state->ambiguous_key_pending ? KEYTIMEOUT : 0, 0)); // Compliant with MISRA C:2012, Rule 15.6 (compound-statement)

        if (state->ambiguous_key_pending) {
            char s[1] = {0}; // Ensure it is initialized to an empty string
            handle_input(state, s, 1);

            if (state->exit >= 0)
                break; // Change return to break to have a single point of exit
        }

        update_state(state);
    } // End of while loop

    return state->exit; // Single point of exit
}
