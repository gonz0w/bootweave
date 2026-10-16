/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"

#include <stdio.h>
#include <string.h>

struct cursor {
    const unsigned char *data;
    size_t length;
    size_t at;
};

static void
space(struct cursor *c)
{
    while (c->at < c->length && (c->data[c->at] == ' ' ||
        c->data[c->at] == '\t' || c->data[c->at] == '\r' ||
        c->data[c->at] == '\n'))
        c->at++;
}

static int
take(struct cursor *c, unsigned char ch)
{
    space(c);
    if (c->at == c->length || c->data[c->at] != ch)
        return 0;
    c->at++;
    return 1;
}

static int
hex(unsigned char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return -1;
}

static int
string(struct cursor *c, char *out, size_t capacity, int key)
{
    size_t used = 0;
    unsigned int ch;
    int digit, i;

    if (!take(c, '"'))
        return 0;
    while (c->at < c->length) {
        ch = c->data[c->at++];
        if (ch == '"') {
            out[used] = '\0';
            return 1;
        }
        if (ch == '\\') {
            if (key || c->at == c->length)
                return 0;
            ch = c->data[c->at++];
            if (ch == 'u') {
                ch = 0;
                for (i = 0; i < 4; i++) {
                    if (c->at == c->length ||
                        (digit = hex(c->data[c->at++])) < 0)
                        return 0;
                    ch = ch * 16 + (unsigned int)digit;
                }
            } else if (ch != '"' && ch != '\\' && ch != '/') {
                return 0;
            }
        }
        if (ch < 32 || ch > 126 || used + 1 >= capacity)
            return 0;
        out[used++] = (char)ch;
    }
    return 0;
}

static int
alpha(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static int
digit(unsigned char ch)
{
    return ch >= '0' && ch <= '9';
}

static size_t
bounded_length(const char *s, size_t capacity)
{
    size_t n;
    for (n = 0; n < capacity && s[n] != '\0'; n++)
        ;
    return n;
}

int
bw_validate(const struct bw_plan *p)
{
    size_t i, n;
    int visible = 0;
    unsigned char ch;

    n = bounded_length(p->id, sizeof(p->id));
    if (n == 0 || n == sizeof(p->id) || p->id[0] < 'a' || p->id[0] > 'z')
        return 0;
    for (i = 0; i < n; i++) {
        ch = (unsigned char)p->id[i];
        if (!((ch >= 'a' && ch <= 'z') || digit(ch) || ch == '-'))
            return 0;
    }
    n = bounded_length(p->name, sizeof(p->name));
    if (n == 0 || n == sizeof(p->name))
        return 0;
    for (i = 0; i < n; i++) {
        ch = (unsigned char)p->name[i];
        if (ch < 32 || ch > 126)
            return 0;
        visible |= ch != ' ';
    }
    if (!visible || memcmp(p->release, "7.9", 4) != 0 ||
        memcmp(p->architecture, "amd64", 6) != 0)
        return 0;
    n = bounded_length(p->file, sizeof(p->file));
    if (n == 0 || n == sizeof(p->file) ||
        !(alpha((unsigned char)p->file[0]) || digit((unsigned char)p->file[0])))
        return 0;
    for (i = 0; i < n; i++) {
        ch = (unsigned char)p->file[i];
        if (!(alpha(ch) || digit(ch) || ch == '.' || ch == '_' || ch == '-'))
            return 0;
    }
    if (bounded_length(p->sha256, sizeof(p->sha256)) != 64)
        return 0;
    for (i = 0; i < 64; i++) {
        ch = (unsigned char)p->sha256[i];
        if (!((ch >= 'a' && ch <= 'f') || digit(ch)))
            return 0;
    }
    return 1;
}

static int
object(struct cursor *c, struct bw_plan *p, int kind)
{
    const char *const names[4][4] = {
        {"schema_version", "project", "system", "payload"},
        {"id", "name", NULL, NULL},
        {"release", "architecture", NULL, NULL},
        {"file", "sha256", NULL, NULL}
    };
    char key[16];
    char *dest;
    size_t capacity;
    unsigned int seen = 0, bit;
    int member, count = kind == 0 ? 4 : 2;

    if (!take(c, '{'))
        return 0;
    for (;;) {
        if (!string(c, key, sizeof(key), 1))
            return 0;
        for (member = 0; member < count; member++)
            if (strcmp(key, names[kind][member]) == 0)
                break;
        if (member == count)
            return 0;
        bit = 1U << member;
        if ((seen & bit) != 0 || !take(c, ':'))
            return 0;
        seen |= bit;
        if (kind == 0) {
            if (member == 0) {
                if (!take(c, '1'))
                    return 0;
            } else if (!object(c, p, member)) {
                return 0;
            }
        } else {
            if (kind == 1 && member == 0) {
                dest = p->id; capacity = sizeof(p->id);
            } else if (kind == 1) {
                dest = p->name; capacity = sizeof(p->name);
            } else if (kind == 2 && member == 0) {
                dest = p->release; capacity = sizeof(p->release);
            } else if (kind == 2) {
                dest = p->architecture; capacity = sizeof(p->architecture);
            } else if (member == 0) {
                dest = p->file; capacity = sizeof(p->file);
            } else {
                dest = p->sha256; capacity = sizeof(p->sha256);
            }
            if (!string(c, dest, capacity, 0))
                return 0;
        }
        if (take(c, '}'))
            return seen == (1U << count) - 1;
        if (!take(c, ','))
            return 0;
    }
}

int
bw_parse(const unsigned char *data, size_t length, struct bw_plan *out,
    size_t *offset)
{
    struct cursor c = {data, length, 0};
    struct bw_plan p = { {0}, {0}, {0}, {0}, {0}, {0} };
    int ok = 0;

    if (length <= BW_INPUT_MAX && object(&c, &p, 0)) {
        space(&c);
        ok = c.at == length && bw_validate(&p);
    }
    *offset = c.at;
    if (ok)
        *out = p;
    return ok;
}

int
bw_render(const struct bw_plan *p, char *out, size_t capacity, size_t *length)
{
    char buffer[BW_OUTPUT_MAX];
    int n;

    *length = 0;
    if (!bw_validate(p))
        return 0;
    n = snprintf(buffer, sizeof(buffer),
        "UNVERIFIED PROJECT PLAN - NO OPERATIONS AUTHORIZED\n"
        "Definition syntax: validated\n"
        "Project ID (supplied): %s\n"
        "Project name (supplied): %s\n"
        "Requested release (supplied): %s\n"
        "Requested architecture (supplied): %s\n"
        "Payload file (unverified): %s\n"
        "Claimed SHA-256 (unverified): %s\n"
        "Payload access: not opened\n"
        "Target: unselected; no device identity collected\n"
        "Operations authorized: none\n"
        "Operations performed: none\n"
        "Operations independently observed: none\n",
        p->id, p->name, p->release, p->architecture, p->file, p->sha256);
    if (n < 0 || (size_t)n >= sizeof(buffer) || (size_t)n >= capacity)
        return 0;
    memcpy(out, buffer, (size_t)n + 1);
    *length = (size_t)n;
    return 1;
}
