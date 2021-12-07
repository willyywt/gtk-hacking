#include "hw_utils.h"
size_t
HW_ScanInt(const char* text, int* pval) {
    const char* textstart = text;
    int value = 0;
    bool negative = false;

    if (*text == '-') {
        negative = true;
        ++text;
    }
    for (;;) {
        int v;
        if (isdigit((unsigned char)*text)) {
            v = *text - '0';
        }
        else {
            break;
        }
        value *= 10;
        value += v;
        ++text;
    }
    if (pval && text > textstart) {
        if (negative && value) {
            *pval = -value;
        }
        else {
            *pval = value;
        }
    }
    return (text - textstart);
}
size_t
HW_ScanDouble(const char* text, double* pval)
{
    const char* textstart = text;
    int lvalue = 0;
    double value = 0.0;
    bool negative = false;

    if (*text == '-') {
        negative = true;
        ++text;
    }
    text += HW_ScanInt(text, &lvalue);
    value += lvalue;
    if (*text == '.') {
        int mult = 10;
        ++text;
        while (isdigit((unsigned char)*text)) {
            lvalue = *text - '0';
            value += (double)lvalue / mult;
            mult *= 10;
            ++text;
        }
    }
    if (pval && text > textstart) {
        if (negative && value) {
            *pval = -value;
        }
        else {
            *pval = value;
        }
    }
    return (text - textstart);
}
size_t
HW_JumpSpace(char* buf) {
    char c;
    char* ptr = buf;
    while (1) {
        c = *ptr;
        if (c == ' ') {
            ptr++;
            continue;
        }
        break;
    }
    return ptr - buf;
}

int
issep(int C) {
    return isspace(C) || (char)C == ',';
}

int
double_equal(double x, double y) {
    return x - y < 1e-8 && x - y > -1e-8;
}
