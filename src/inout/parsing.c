
#include <ustd/parsing.h>

#include <stdio.h>

/**
 * @brief Advances the parser state one character.
 *
 * @param[inout] state
 */
void parser_state_advance(struct parser_state *state)
{
    if (state->buffer_idx >= array_length(state->buffer_array)) {
        return;
    }

    if (state->buffer_array[state->buffer_idx] == '\n') {
        state->column = 0;
        state->line  += 1;
    } else {
        state->column += 1;
    }

    state->buffer_idx += 1;
}

/**
 * @brief Advances the parser state until a character other than a whitespace
 * (space or tab) is found.
 *
 * @param[inout] state
 */
void parser_skip_whitespace(struct parser_state *state)
{
    while (parser_accept(state, (char[]) { ' ', '\t' }, 2, NULL));
}

/**
 * @brief Parser utility to read a character among a list of possibilities.
 * If a match is found, 1 is returned and the character is read. Otherwise,
 * nothing happens and the function returns 0.
 *
 * @param[inout] state Current parser state.
 * @param[in] alternatives List of characters (there must be at least nb
 * characters in the list).
 * @param[in] nb Number of characters.
 * @param[out] c_out Optional pointer willed with the read character.
 * @return bool
 */
bool parser_accept(struct parser_state *state, char *alternatives, size_t nb,
        char *c_out)
{
    if (parser_lookup(state, alternatives, nb, c_out)) {
        parser_state_advance(state);
        return true;
    }

    return false;
}

/**
 * @brief Parser utility to read a character among a list of possibilities.
 * If a match is found, 1 is returned and the character is read. Otherwise,
 * an error is porduced.
 *
 * @param[inout] state Current parser state.
 * @param[in] alternatives List of characters (there must be at least nb
 * characters in the list).
 * @param[in] nb Number of characters.
 * @param[out] c_out Optional pointer willed with the read character.
 * @return i32
 */
bool parser_expect(struct parser_state *state, char *alternatives, size_t nb,
        char *c_out)
{
    if (parser_accept(state, alternatives, nb, c_out)) {
        return true;
    }

    fprintf(stderr, "at line %d:%d ; ", state->line+1, state->column+1);

    fprintf(stderr, "expected ");
    if (nb == 0) {
        fprintf(stderr, "end of stream");
    } else {
        fprintf(stderr, "one of : ");
        for (size_t i = 0 ; i < nb ; i++) {
            fprintf(stderr, "%c ", alternatives[i]);
        }
    }

    fprintf(stderr, "\nbut found : ");
    if (state->buffer_idx >= array_length(state->buffer_array)) {
        fprintf(stderr, "end of stream.\n");
    } else {
        fprintf(stderr, "%c\n", state->buffer_array[state->buffer_idx]);
    }

    return false;
}

/**
 * @brief Parser utility to read a character among a list of possibilities.
 * If a match is found, 1 is returned, and nothing else happens. Otherwise,
 * the function returns 0.
 *
 * @param[inout] state Current parser state.
 * @param[in] alternatives List of characters (there must be at least nb
 * characters in the list).
 * @param[in] nb Number of characters.
 * @param[out] c_out Optional pointer willed with the read character.
 * @return i32
 */
bool parser_lookup(struct parser_state *state, char *alternatives, size_t nb,
        char *c_out)
{
    bool found = 0;
    size_t alt_idx = 0;

    if (state->buffer_idx >= array_length(state->buffer_array)) {
        return false;
    }

    while (!found && (alt_idx < nb)) {
        found = state->buffer_array[state->buffer_idx] == alternatives[alt_idx];
        alt_idx += 1;
    }

    if (c_out) {
        *c_out = (char) state->buffer_array[state->buffer_idx];
    }

    return found;
}

/**
 * @brief
 *
 * @param state
 * @return true
 * @return false
 */
bool parser_parse_end_line(struct parser_state *state)
{
    return parser_accept(state, (char []) { '\n' }, 1, NULL);
}

/**
 * @brief
 *
 * @param state
 * @param out_obj
 * @return true
 * @return false
 */
bool parser_parse_end_of_file(struct parser_state *state)
{
    return ((state->buffer_idx+1) >= array_length(state->buffer_array));
}

/**
 * @brief
 *
 * @param state
 * @param out_value
 * @return true
 * @return false
 */
bool parser_parse_value_float(struct parser_state *state, f32 *out_value)
{
    u32 parsed_value = 0;
    f32 int_part = 0.f;
    f32 frac_part = 0.f;
    f32 sign = 1.f;
    size_t frac_part_start = 0;
    size_t frac_part_length = 0;

    parser_skip_whitespace(state);

    if (parser_accept(state, (char[]) { '-' }, 1, NULL)) {
        sign = -1.f;
    }

    if (!parser_parse_value_uint(state, &parsed_value)) {
        return false;
    }
    int_part = (f32) parsed_value;

    if (parser_accept(state, (char[]) { '.' }, 1, NULL)) {
        frac_part_start = state->buffer_idx;
        parser_parse_value_uint(state, &parsed_value);
        frac_part_length = state->buffer_idx - frac_part_start;
        frac_part = (f32) parsed_value;
    }

    for (size_t i = 0 ; i < frac_part_length ; i++) {
        frac_part /= 10.f;
    }

    *out_value = sign * (int_part + frac_part);

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool parser_parse_value_int(struct parser_state *state, i32 *out_value)
{
    u32 value = 0;
    i32 sign = 1;
    bool valid = false;
    char read_c = '+';

    parser_skip_whitespace(state);

    if (parser_lookup(state, (char[]) { '+', '-' }, 2, &read_c)) {
        if (read_c == '-') sign = -1;
        parser_state_advance(state);
    }

    valid = parser_parse_value_uint(state, &value);
    if (valid) {
        *out_value = sign * (i32) value;
    }

    return valid;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool parser_parse_value_uint(struct parser_state *state, u32 *out_value)
{
    u32 value = 0;
    char read_c = '\0';

    if (!parser_expect(state, (char[])
            { '0','1','2','3','4','5','6','7','8','9', }, 10, &read_c)) {
        return false;
    }

    do {
        value *= 10;
        value += (u32) (read_c - '0');
    } while (parser_accept(state, (char[])
            { '0','1','2','3','4','5','6','7','8','9', }, 10, &read_c));

    *out_value = value;

    return true;
}
