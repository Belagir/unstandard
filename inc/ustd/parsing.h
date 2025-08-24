
#ifndef UNSTANDARD_PARSING_H__
#define UNSTANDARD_PARSING_H__

#include <ustd/array.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Private state of the parser.
 */
struct parser_state {
    const ARRAY(byte) buffer_array;
    size_t buffer_idx;
    u32 line, column;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void parser_state_advance(struct parser_state *state);
void parser_skip_whitespace(struct parser_state *state);

// -----------------------------------------------------------------------------

bool parser_accept(struct parser_state *state, char *alternatives, size_t nb, char *c_out);
bool parser_expect(struct parser_state *state, char *alternatives, size_t nb, char *c_out);
bool parser_lookup(struct parser_state *state, char *alternatives, size_t nb, char *c_out);

// -----------------------------------------------------------------------------

bool parser_parse_end_line(struct parser_state *state);
bool parser_parse_end_of_file(struct parser_state *state);

// -----------------------------------------------------------------------------

bool parser_parse_value_float(struct parser_state *state, f32 *out_value);
bool parser_parse_value_int(struct parser_state *state, i32 *out_value);
bool parser_parse_value_uint(struct parser_state *state, u32 *out_value);

#endif
