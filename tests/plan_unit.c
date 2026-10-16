/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"

#include <assert.h>
#include <string.h>

int
main(void)
{
    const unsigned char definition[] = "{\"schema_version\":1,"
        "\"project\":{\"id\":\"viewer\",\"name\":\"Viewer\"},"
        "\"system\":{\"release\":\"7.9\",\"architecture\":\"amd64\"},"
        "\"payload\":{\"file\":\"absent.tar\",\"sha256\":\""
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"}}";
    struct bw_plan p, changed;
    char output[BW_OUTPUT_MAX];
    size_t offset, length;
    assert(sizeof(p) <= 2048);
    assert(bw_parse(definition, sizeof(definition) - 1, &p, &offset));
    assert(bw_validate(&p));
    memset(output, 'X', sizeof(output));
    assert(!bw_render(&p, output, 1, &length));
    assert(length == 0 && output[0] == 'X');
    changed = p;
    memset(changed.name, 'A', sizeof(changed.name));
    assert(!bw_render(&changed, output, sizeof(output), &length));
    changed = p;
    changed.name[0] = 27;
    assert(!bw_validate(&changed));
    changed = p;
    changed.id[0] = 'A';
    assert(!bw_validate(&changed));
    changed = p;
    changed.file[0] = '/';
    assert(!bw_validate(&changed));
    changed = p;
    changed.sha256[0] = 'G';
    assert(!bw_validate(&changed));
    changed = p;
    changed.release[3] = 'X';
    assert(!bw_validate(&changed));
    changed = p;
    changed.architecture[5] = 'X';
    assert(!bw_validate(&changed));
    changed = p;
    assert(!bw_parse(definition, 10, &changed, &offset));
    assert(memcmp(&changed, &p, sizeof(p)) == 0);
    return 0;
}
