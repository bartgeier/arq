#include "arq_conversion.h"
#include "arq_symbols.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <arq_inttypes.h>
#include <math.h>

bool token_long_option_eq(Arq_Token const *token, char const *cstr) {
        uint32_t i;
        if (strlen(cstr) != token->size - 2) {
                return false;

        }
        for (i = 2; i < token->size; i++) {
                if (cstr[i - 2] != token->at[i]) {
                        return false;
                }
        }
        return true;
}

uint8_to arq_tok_pDec_to_uint8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint8_to result;
        uint32_to num = arq_tok_pDec_to_uint32_t(token, error_msg, cstr);
        if (num.u32 > UINT8_MAX || num.error) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRIu8, UINT8_MAX);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' positive number > UINT8_MAX ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        }
        result.u8 = (uint8_t) num.u32;
        result.error = num.error;
        return result;
}

uint16_to arq_tok_pDec_to_uint16_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint16_to result;
        uint32_to num = arq_tok_pDec_to_uint32_t(token, error_msg, cstr);
        if (num.u32 > UINT16_MAX || num.error) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRIu16, UINT16_MAX);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' positive number > UINT16_MAX ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        }
        result.u16 = (uint16_t) num.u32;
        result.error = num.error;
        return result;
}

uint32_to arq_tok_pDec_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint32_to result = {0};
        uint32_t i;
        assert(token->id == ARQ_P_DEC);
        if (token->at[0] == '+') {
                i = 1;
        } else {
                i = 0;
        }
        for (; i < token->size; i++) {
                uint32_t digit = token->at[i] - '0';
                if (result.u32 > (UINT32_MAX - digit) / 10) {
                        if (error_msg != NULL) {
                                Arq_Token tok = *token;
                                char buffer[12];
                                sprintf(buffer, "%" PRIu32, UINT32_MAX);
                                arq_msg_clear(error_msg);
                                arq_msg_append_cstr(error_msg, cstr);
                                arq_msg_append_cstr(error_msg, "Token '");
                                arq_msg_append_str(error_msg, tok.at, tok.size);
                                arq_msg_append_cstr(error_msg, "' positive number > UINT32_MAX ");
                                arq_msg_append_cstr(error_msg, buffer);
                                arq_msg_append_lf(error_msg);
                        }
                        result.error = true;
                        return result;
                }
                result.u32 = result.u32 * 10 + digit;
        } 
        return result;
}

int32_to arq_tok_sDec_to_int32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        int32_to result = {0};
        int32_t SIGN;
        uint32_t i;
        assert(token->id == ARQ_P_DEC || token->id == ARQ_N_DEC);

        if (token->at[0] == '-') {
                SIGN = -1;
                i = 1;
        } else if (token->at[0] == '+') {
                SIGN = 1;
                i = 1;
        } else {
                SIGN = 1;
                i = 0;
        }
        assert(SIGN != 0);

        for (; i < token->size; i++) {
                char const ch = token->at[i];
                int32_t const digit = ch - '0';

                if (SIGN > 0) {
                        if (result.i32 > (INT32_MAX - digit) / 10) {
                                result.error = true;
                                if (error_msg != NULL) {
                                        Arq_Token tok = *token;
                                        char buffer[12];
                                        sprintf(buffer, "%" PRId32, INT32_MAX);
                                        arq_msg_clear(error_msg);
                                        arq_msg_append_cstr(error_msg, cstr);
                                        arq_msg_append_cstr(error_msg, "Token '");
                                        arq_msg_append_str(error_msg, tok.at, tok.size);
                                        arq_msg_append_cstr(error_msg, "' positive number > INT32_MAX ");
                                        arq_msg_append_cstr(error_msg, buffer);
                                        arq_msg_append_lf(error_msg);
                                }
                                return result;
                        }
                        result.i32 = result.i32 * 10 + digit;
                } else {
                        if (result.i32 < (INT32_MIN + digit) / 10) {
                                result.error = true;
                                if (error_msg != NULL) {
                                        Arq_Token tok = *token;
                                        char buffer[12];
                                        sprintf(buffer, "%" PRId32, INT32_MIN);
                                        arq_msg_clear(error_msg);
                                        arq_msg_append_cstr(error_msg, cstr);
                                        arq_msg_append_cstr(error_msg, "Token '");
                                        arq_msg_append_str(error_msg, tok.at, tok.size);
                                        arq_msg_append_cstr(error_msg, "' negative number < INT32_MIN ");
                                        arq_msg_append_cstr(error_msg, buffer);
                                        arq_msg_append_lf(error_msg);
                                }
                                return result;
                        }
                        result.i32 = result.i32 * 10 - digit;
                }
        }
        return result;
}

int8_to arq_tok_sDec_to_int8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        int8_to result;
        int32_to num = arq_tok_sDec_to_int32_t(token, error_msg, cstr);
        if (num.i32 > INT8_MAX || (num.i32 > 0 && num.error)) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRId8, INT8_MAX);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' positive number > INT8_MAX ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        } else if (num.i32 < INT8_MIN || (num.i32 < 0 && num.error)) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRId8, INT8_MIN);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' negative number < INT8_MIN ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        }
        result.i8 = (int8_t)num.i32;
        result.error = num.error;
        return result;
}

int16_to arq_tok_sDec_to_int16_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        int16_to result;
        int32_to num = arq_tok_sDec_to_int32_t(token, error_msg, cstr);
        if (num.i32 > INT16_MAX || (num.i32 > 0 && num.error)) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRId16, INT16_MAX);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' positive number > INT16_MAX ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        } else if (num.i32 < INT16_MIN || (num.i32 < 0 && num.error)) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        char buffer[12];
                        sprintf(buffer, "%" PRId16, INT16_MIN);
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' negative number < INT16_MIN ");
                        arq_msg_append_cstr(error_msg, buffer);
                        arq_msg_append_lf(error_msg);
                }
        }
        result.i16 = (int16_t)num.i32;
        result.error = num.error;
        return result;
}

uint8_to arq_tok_hex_to_uint8_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint8_to result;
        uint32_to num = arq_tok_hex_to_uint32_t(token, error_msg, cstr);
        if (num.u32 > UINT8_MAX || num.error) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' more than 2 hex digits");
                        arq_msg_append_lf(error_msg);
                }
        }
        result.u8 = (uint8_t) num.u32;
        result.error = num.error;
        return result;
}

uint16_to arq_tok_hex_to_uint16_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint16_to result;
        uint32_to num = arq_tok_hex_to_uint32_t(token, error_msg, cstr);
        if (num.u32 > UINT16_MAX || num.error) {
                num.error = true;
                if (error_msg != NULL) {
                        Arq_Token tok = *token;
                        arq_msg_clear(error_msg);
                        arq_msg_append_cstr(error_msg, cstr);
                        arq_msg_append_cstr(error_msg, "Token '");
                        arq_msg_append_str(error_msg, tok.at, tok.size);
                        arq_msg_append_cstr(error_msg, "' more than 4 hex digits");
                        arq_msg_append_lf(error_msg);
                }
        }
        result.u16 = (uint16_t) num.u32;
        result.error = num.error;
        return result;
}

static int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

uint32_to arq_tok_hex_to_uint32_t(Arq_Token const *token, Arq_msg *error_msg, char const *cstr) {
        uint32_to result = {0};
        uint32_t i;
        assert(token->id == ARQ_HEX);
        for (i = 2; i < token->size; i++) {
                char const ch = token->at[i];
                int const digit = hexval(ch);
                assert(digit >= 0);
                if (result.u32 > (UINT32_MAX - digit) / 10) {
                        result.error = true;
                        if (error_msg != NULL) {
                                Arq_Token tok = *token;
                                arq_msg_clear(error_msg);
                                arq_msg_append_cstr(error_msg, cstr);
                                arq_msg_append_cstr(error_msg, "Token '");
                                arq_msg_append_str(error_msg, tok.at, tok.size);
                                arq_msg_append_cstr(error_msg, "' more than 8 hex digits");
                                arq_msg_append_lf(error_msg);
                        }
                        return result;
                }
                result.u32 = result.u32 * 16 + digit;
        }
        return result;
}

#if 1
float_to arq_tok_hexFloat_to_float(Arq_Token const *token) {
        float_to result;
        double value = 0.0;
        int exp10 = 0;
        int exp_sign = 1;

        uint32_t i = 2;

        assert(token->id == ARQ_HEX_FLOAT);
        assert(token->at[0] == '0' && (token->at[1] == 'x' || token->at[1] == 'X'));

        /* integer part */
        while (i < token->size) {
                int v = hexval(token->at[i]);
                if (v < 0) {
                        break;
                }
                value = value * 16.0 + (double)v;
                i++;
        }

        /* fractional part */
        if (i < token->size && '.' == token->at[i]) {
                double place = 1.0 / 16.0;
                i++;
                while (i < token->size) {
                        int v = hexval(token->at[i]);
                        if (v < 0) {
                                break;
                        }
                        value += (double)v * place;
                        place /= 16.0;
                        i++;
                }
        }

        /* exponent part (binary exponent) */
        if (i < token->size + 1 && ('p' == token->at[i] || 'P' == token->at[i])) {
                i++;
                switch (token->at[i]) {
                case '+': 
                        exp_sign = 1;
                        i++;
                        break;
                case '-': 
                        exp_sign = -1;
                        i++;
                        break;
                }
                while (i < token->size) {
                        if (exp10 < 2000) {
                                exp10 = exp10 * 10 + (token->at[i] - '0');
                        }
                        i++;
                }
                if (exp_sign > 0 && exp10 > 1200) {
                        result.f = HUGE_VAL; /* INFINITY */
                        result.error = false;
                        return result;
                }
                if (exp_sign < 0 && exp10 > 1200) {
                        result.f = 0.0;
                        result.error = false;
                        return result;
                }
        }

        /* scale by 2^final_exp */
        {
                int final_exp = exp_sign * exp10;
                result.f =  ldexp(value, final_exp);
                result.error = false;
                return result;
        }
}
#endif
#if 0
float_to arq_tok_hexFloat_to_float(const Arq_Token *token) {
        float_to result;
        double value;
        int exp10;
        int exp_sign;
        uint32_t i;
        double place;
        /* int v; */

        /* initialize result */
        result.f = 0.0;
        result.error = 0;

        assert(token->id == ARQ_HEX_FLOAT);
        if (!token || token->size < 3) {
                return result; /* must at least contain 0x1 */
        }

        /* initialize parsing variables */
        value = 0.0;
        exp10 = 0;
        exp_sign = 1;
        i = 2; /* skip 0x prefix */

        /* --- Integer part --- */
        while (i < token->size) {
                const int v = hexval(token->at[i]);
                if (v < 0) {
                        break;
                }
                value = value * 16.0 + (double)v;
                i++;
        }

        /* --- Fractional part --- */
        if (i < token->size && token->at[i] == '.') {
                i++;
                place = 1.0 / 16.0;
                while (i < token->size) {
                        const int v = hexval(token->at[i]);
                        if (v < 0) {
                                break;
                        }
                        value += (double)v * place;
                        place /= 16.0;
                        i++;
                }
        }

        /* --- Exponent part --- */
        if (i < token->size + 1 && (token->at[i] == 'p' || token->at[i] == 'P')) {
                i++;
                switch (token->at[i]) {
                case '+': 
                        exp_sign = 1;
                        i++;
                        break;
                case '-': 
                        exp_sign = -1;
                        i++;
                        break;
                }
                while (i < token->size) {
                        if (exp10 < 2000) {
                                exp10 = exp10 * 10 + (token->at[i] - '0');
                        }
                        i++;
                }
                if (exp_sign > 0 && exp10 > 1200) {
                        result.f = HUGE_VAL;  /* infinity */
                        result.error = 1;
                        return result;
                }
                if (exp_sign < 0 && exp10 > 1200) {
                        result.f = 0.0;
                        result.error = 1;
                        return result;
                }
        }

        /* --- Final scaling --- */
        {
                int final_exp = exp_sign * exp10;
                result.f = ldexp(value, final_exp);
                result.error = 0;
                return result;
        }
}
#endif
#if 0
/* Main parser: 64-bit mantissa for perfect rounding */
float_to arq_tok_hexFloat_to_float(const Arq_Token *token) {
    float_to result;
    uint64_t mantissa;
    int mantissa_bits;
    int exp10, exp_sign, final_exp;
    uint32_t i;
    int v;
    char c;
    int fraction_digits;

    /* initialize result */
    result.f = 0.0;
    result.error = 0;

    assert(token->id == ARQ_HEX_FLOAT);
    if (!token || token->size < 3) return result; /* must at least contain 0x1 */

    /* --- Variables --- */
    mantissa = 0ULL;
    mantissa_bits = 0;   /* number of significant bits in mantissa */
    fraction_digits = 0;
    exp10 = 0;
    exp_sign = 1;
    i = 2; /* skip 0x prefix */

    /* --- Integer part --- */
    while (i < token->size) {
        v = hexval(token->at[i]);
        if (v < 0) break;

        if (mantissa_bits < 53) {  /* accumulate only up to 53 bits for double precision */
            mantissa = (mantissa << 4) | (uint64_t)v;
            mantissa_bits += 4;
        } else {
            /* ignore extra digits, they affect rounding minimally */
        }
        i++;
    }

    /* --- Fractional part --- */
    if (i < token->size && token->at[i] == '.') {
        i++;
        while (i < token->size) {
            v = hexval(token->at[i]);
            if (v < 0) break;

            if (mantissa_bits < 53) {
                mantissa = (mantissa << 4) | (uint64_t)v;
                mantissa_bits += 4;
            }
            fraction_digits++;
            i++;
        }
    }

    /* adjust exponent for fractional shift */
    final_exp = -4 * fraction_digits;

    /* --- Exponent part --- */
    if (i < token->size && (token->at[i] == 'p' || token->at[i] == 'P')) {
        i++;
        if (i < token->size) {
            if (token->at[i] == '+') { exp_sign = 1; i++; }
            else if (token->at[i] == '-') { exp_sign = -1; i++; }
        }

        while (i < token->size) {
            c = token->at[i];
            if (c < '0' || c > '9') break;
            /* clamp to prevent integer overflow */
            if (exp10 < 2000) {
                exp10 = exp10 * 10 + (c - '0');
            }
            i++;
        }

        /* combine exponent */
        final_exp += exp_sign * exp10;

        /* handle extreme exponent cases */
        if (final_exp > 1023) {
            result.f = HUGE_VAL;  /* overflow */
            result.error = 1;
            return result;
        }
        if (final_exp < -1074) {
            result.f = 0.0;       /* underflow */
            result.error = 1;
            return result;
        }
    }

    /* --- Construct final double --- */
    result.f = ldexp((double)mantissa, final_exp - (mantissa_bits - 1));
    result.error = 0;
    return result;
}
#endif
