/* SPDX-License-Identifier: BSD-3-Clause */
#include "install_profile.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct cursor { const unsigned char *raw; size_t size, at; };
struct field { const char *name; size_t offset, size; };
#define FIELD(t, f) {#f, offsetof(struct t, f), sizeof(((struct t *)0)->f)}
static const struct field artifacts[] = {FIELD(bwi_artifact,id),
    FIELD(bwi_artifact,kind), FIELD(bwi_artifact,file), FIELD(bwi_artifact,sha256)};
static const struct field principals[] = {FIELD(bwi_principal,name)};
static const struct field files[] = {FIELD(bwi_file,artifact), FIELD(bwi_file,member),
    FIELD(bwi_file,target), FIELD(bwi_file,owner), FIELD(bwi_file,group), FIELD(bwi_file,mode)};
static const struct field inputs[] = {FIELD(bwi_input,name), FIELD(bwi_input,target),
    FIELD(bwi_input,group), FIELD(bwi_input,mode)};
static const struct field services[] = {FIELD(bwi_service,name), FIELD(bwi_service,entry),
    FIELD(bwi_service,template)};
static const struct field policies[] = {FIELD(bwi_policy,management), FIELD(bwi_policy,ssh),
    FIELD(bwi_policy,network)};
static const struct field observability[] = {FIELD(bwi_observability,log), FIELD(bwi_observability,health)};
#undef FIELD
#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

static void
space(struct cursor *c)
{
    while (c->at < c->size && (c->raw[c->at] == ' ' || c->raw[c->at] == '\t' ||
        c->raw[c->at] == '\r' || c->raw[c->at] == '\n')) c->at++;
}
static int
take(struct cursor *c, unsigned char ch)
{
    space(c);
    if (c->at == c->size || c->raw[c->at] != ch) return 0;
    c->at++;
    return 1;
}
static int
text(struct cursor *c, char *out, size_t cap)
{
    size_t n = 0;
    unsigned char ch;
    if (!take(c, '"')) return 0;
    while (c->at < c->size) {
        ch = c->raw[c->at++];
        if (ch == '"') { out[n] = 0; return 1; }
        /* The closed ASCII grammar needs no escapes, including escaped keys. */
        if (ch < 32 || ch > 126 || ch == '\\' || n + 1 >= cap) return 0;
        out[n++] = (char)ch;
    }
    return 0;
}
static int
object(struct cursor *c, void *row, const struct field *fields, size_t count)
{
    char key[24];
    size_t i;
    unsigned seen = 0;
    if (!take(c, '{')) return 0;
    for (;;) {
        if (!text(c, key, sizeof(key)) || !take(c, ':')) return 0;
        for (i = 0; i < count && strcmp(key, fields[i].name) != 0; i++) ;
        if (i == count || (seen & (1U << i)) != 0) return 0;
        seen |= 1U << i;
        if (!text(c, (char *)row + fields[i].offset, fields[i].size)) return 0;
        if (take(c, '}')) return seen == (1U << count) - 1;
        if (!take(c, ',')) return 0;
    }
}
static int
array(struct cursor *c, void *rows, size_t stride, size_t cap, size_t *count,
    const struct field *fields, size_t nfields)
{
    *count = 0;
    if (!take(c, '[')) return 0;
    if (take(c, ']')) return 1;
    for (;;) {
        if (*count == cap || !object(c, (char *)rows + *count * stride, fields, nfields)) return 0;
        (*count)++;
        if (take(c, ']')) return 1;
        if (!take(c, ',')) return 0;
    }
}

int
bwi_profile_parse(const unsigned char *raw, size_t size, struct bwi_profile *out)
{
    static const char *const keys[] = {"version", "project", "release", "architecture",
        "firmware", "handoff", "artifacts", "principals", "files", "inputs", "services",
        "policy", "observability"};
    struct cursor c = {raw, size, 0};
    struct bwi_profile p;
    char key[24];
    size_t i;
    unsigned seen = 0;
    if (!raw || !out || size == 0 || size > BWI_PROFILE_MAX) return 0;
    memset(&p, 0, sizeof(p));
    if (!take(&c, '{')) return 0;
    for (;;) {
        if (!text(&c, key, sizeof(key)) || !take(&c, ':')) return 0;
        for (i = 0; i < COUNT(keys) && strcmp(key, keys[i]) != 0; i++) ;
        if (i == COUNT(keys) || (seen & (1U << i)) != 0) return 0;
        seen |= 1U << i;
        switch (i) {
        case 0: if (!take(&c, '2')) return 0; break;
        case 1: if (!text(&c,p.project,sizeof(p.project))) return 0; break;
        case 2: if (!text(&c,p.release,sizeof(p.release))) return 0; break;
        case 3: if (!text(&c,p.architecture,sizeof(p.architecture))) return 0; break;
        case 4: if (!text(&c,p.firmware,sizeof(p.firmware))) return 0; break;
        case 5: if (!text(&c,p.handoff,sizeof(p.handoff))) return 0; break;
        case 6:
            if (!array(&c,p.artifact,sizeof(p.artifact[0]),BWI_ARTIFACTS,&p.artifacts,artifacts,COUNT(artifacts))) return 0;
            break;
        case 7:
            if (!array(&c,p.principal,sizeof(p.principal[0]),BWI_PRINCIPALS,&p.principals,principals,COUNT(principals))) return 0;
            break;
        case 8:
            if (!array(&c,p.file,sizeof(p.file[0]),BWI_FILES,&p.files,files,COUNT(files))) return 0;
            break;
        case 9:
            if (!array(&c,p.input,sizeof(p.input[0]),BWI_INPUTS,&p.inputs,inputs,COUNT(inputs))) return 0;
            break;
        case 10:
            if (!array(&c,p.service,sizeof(p.service[0]),BWI_SERVICES,&p.services,services,COUNT(services))) return 0;
            break;
        case 11: if (!object(&c,&p.policy,policies,COUNT(policies))) return 0; break;
        case 12: if (!object(&c,&p.observability,observability,COUNT(observability))) return 0; break;
        default: return 0;
        }
        if (take(&c, '}')) break;
        if (!take(&c, ',')) return 0;
    }
    space(&c);
    if (seen != (1U << COUNT(keys)) - 1 || c.at != c.size || !bwi_profile_validate(&p)) return 0;
    *out = p;
    return 1;
}

static int
bounded(const char *s, size_t cap)
{
    return memchr(s, 0, cap) != NULL;
}
static int
name(const char *s, size_t cap, int role)
{
    size_t i = 0;
    if (!bounded(s, cap)) return 0;
    if (role && s[0] == '_') i++;
    if (s[i] < 'a' || s[i] > 'z') return 0;
    for (; s[i]; i++) if (!((s[i] >= 'a' && s[i] <= 'z') ||
        (s[i] >= '0' && s[i] <= '9') || s[i] == '-' || s[i] == '_')) return 0;
    return 1;
}
static int
path(const char *s, size_t cap, int absolute)
{
    size_t i, start;
    if (!bounded(s, cap) || !s[0] || (s[0] == '/') != absolute) return 0;
    start = absolute ? 1 : 0;
    for (i = start;; i++) {
        unsigned char ch = (unsigned char)s[i];
        if (ch == '/' || !ch) {
            if (i == start || (i-start == 1 && s[start] == '.') ||
                (i-start == 2 && s[start] == '.' && s[start+1] == '.')) return 0;
            if (!ch) return 1;
            start = i+1;
        } else if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '_' || ch == '-' || ch == '.')) return 0;
    }
}
static int
prefix(const char *s, const char *p) { return strncmp(s,p,strlen(p)) == 0; }
static int
role(const struct bwi_profile *p, const char *s, size_t cap, int group)
{
    size_t i;
    if (!bounded(s,cap)) return 0;
    if (strcmp(s, group ? "wheel" : "root") == 0) return 1;
    for (i = 0; i < p->principals; i++) if (strcmp(s,p->principal[i].name) == 0) return 1;
    return 0;
}
static int
scope(const struct bwi_profile *p, const char *target, int private)
{
    char root[96];
    if (!path(target,193,1)) return 0;
    if (private) {
        (void)snprintf(root,sizeof(root),"/etc/%s/private/",p->project);
        return prefix(target,root);
    }
    if (prefix(target,"/usr/local/sbin/") || prefix(target,"/usr/local/libexec/")) return 1;
    (void)snprintf(root,sizeof(root),"/usr/local/share/%s/",p->project);
    if (prefix(target,root)) return 1;
    (void)snprintf(root,sizeof(root),"/etc/%s/",p->project);
    if (prefix(target,root)) return 1;
    (void)snprintf(root,sizeof(root),"/var/db/%s/",p->project);
    return prefix(target,root);
}
static int
conflict(const char *a, const char *b)
{
    size_t n = strlen(a), m = strlen(b);
    return strcmp(a,b) == 0 || (n < m && prefix(b,a) && b[n] == '/') ||
        (m < n && prefix(a,b) && a[m] == '/');
}
int
bwi_profile_validate(const struct bwi_profile *p)
{
    size_t i,j,k,media=0,sets=0,payload=0;
    if (!p || !name(p->project,sizeof(p->project),0) ||
        !bounded(p->release,sizeof(p->release)) || strcmp(p->release,"8.0") ||
        !bounded(p->architecture,sizeof(p->architecture)) ||
        (strcmp(p->architecture,"amd64") && strcmp(p->architecture,"arm64")) ||
        !bounded(p->firmware,sizeof(p->firmware)) ||
        (strcmp(p->firmware,"uefi") && strcmp(p->firmware,"bios")) ||
        (!strcmp(p->architecture,"arm64") && strcmp(p->firmware,"uefi")) ||
        !bounded(p->handoff,sizeof(p->handoff)) ||
        (strcmp(p->handoff,"single-stopped") && strcmp(p->handoff,"paired-never-started")) ||
        p->artifacts < 3 || p->artifacts > BWI_ARTIFACTS || p->principals > BWI_PRINCIPALS ||
        !p->files || p->files > BWI_FILES || p->inputs > BWI_INPUTS || !p->services || p->services > BWI_SERVICES) return 0;
    for (i=0;i<p->artifacts;i++) {
        const struct bwi_artifact *a=&p->artifact[i];
        if (!name(a->id,sizeof(a->id),0) || !bounded(a->kind,sizeof(a->kind)) ||
            !path(a->file,sizeof(a->file),0) || strchr(a->file,'/') ||
            !bounded(a->sha256,sizeof(a->sha256)) || strlen(a->sha256)!=64 ||
            strspn(a->sha256,"0123456789abcdef")!=64) return 0;
        if (!strcmp(a->kind,"media")) media++;
        else if (!strcmp(a->kind,"set")) sets++;
        else if (!strcmp(a->kind,"payload")) payload++;
        else if (strcmp(a->kind,"package")) return 0;
        for (j=0;j<i;j++) if (!strcmp(a->id,p->artifact[j].id) || !strcmp(a->file,p->artifact[j].file)) return 0;
    }
    if (media!=1 || !sets || payload!=1) return 0;
    for (i=0;i<p->principals;i++) {
        char stem[36];
        (void)snprintf(stem,sizeof(stem),"_%s",p->project);
        if (!name(p->principal[i].name,sizeof(p->principal[i].name),1) ||
            !prefix(p->principal[i].name,stem) ||
            (p->principal[i].name[strlen(stem)] && p->principal[i].name[strlen(stem)]!='_')) return 0;
        for (j=0;j<i;j++) if (!strcmp(p->principal[i].name,p->principal[j].name)) return 0;
    }
    for (i=0;i<p->files;i++) {
        const struct bwi_file *f=&p->file[i];
        if (!bounded(f->artifact,sizeof(f->artifact)) || !path(f->member,sizeof(f->member),0) ||
            !scope(p,f->target,0) || !role(p,f->owner,sizeof(f->owner),0) || !role(p,f->group,sizeof(f->group),1) ||
            !bounded(f->mode,sizeof(f->mode)) || (strcmp(f->mode,"0440") && strcmp(f->mode,"0444") &&
            strcmp(f->mode,"0500") && strcmp(f->mode,"0555") && strcmp(f->mode,"0600") && strcmp(f->mode,"0644"))) return 0;
        for (j=0;j<p->artifacts;j++) if (!strcmp(f->artifact,p->artifact[j].id) && !strcmp(p->artifact[j].kind,"payload")) break;
        if (j==p->artifacts) return 0;
        for (j=0;j<i;j++) if (conflict(f->target,p->file[j].target) || !strcmp(f->member,p->file[j].member)) return 0;
    }
    for (i=0;i<p->inputs;i++) {
        const struct bwi_input *v=&p->input[i];
        if (!name(v->name,sizeof(v->name),0) || !scope(p,v->target,1) ||
            !role(p,v->group,sizeof(v->group),1) || !bounded(v->mode,sizeof(v->mode)) ||
            (strcmp(v->mode,"0440") && strcmp(v->mode,"0600"))) return 0;
        for (j=0;j<i;j++) if (!strcmp(v->name,p->input[j].name) || conflict(v->target,p->input[j].target)) return 0;
        for (j=0;j<p->files;j++) if (conflict(v->target,p->file[j].target)) return 0;
    }
    for (i=0;i<p->services;i++) {
        const struct bwi_service *s=&p->service[i];
        if (!name(s->name,sizeof(s->name),0) || !prefix(s->name,p->project) ||
            (s->name[strlen(p->project)] && s->name[strlen(p->project)]!='-') ||
            !path(s->entry,sizeof(s->entry),1) || !bounded(s->template,sizeof(s->template)) ||
            strcmp(s->template,"owned-foreground")) return 0;
        for (j=0;j<p->files;j++) if (!strcmp(s->entry,p->file[j].target) &&
            !strcmp(p->file[j].owner,"root") && !strcmp(p->file[j].group,"wheel") &&
            (!strcmp(p->file[j].mode,"0500") || !strcmp(p->file[j].mode,"0555"))) break;
        if (j==p->files) return 0;
        for (k=0;k<i;k++) if (!strcmp(s->name,p->service[k].name) || !strcmp(s->entry,p->service[k].entry)) return 0;
    }
    return bounded(p->policy.management,sizeof(p->policy.management)) && !strcmp(p->policy.management,"site-input") &&
        bounded(p->policy.ssh,sizeof(p->policy.ssh)) && !strcmp(p->policy.ssh,"disabled") &&
        bounded(p->policy.network,sizeof(p->policy.network)) && !strcmp(p->policy.network,"product-input") &&
        bounded(p->observability.log,sizeof(p->observability.log)) && !strcmp(p->observability.log,"local-bounded") &&
        bounded(p->observability.health,sizeof(p->observability.health)) && !strcmp(p->observability.health,"product-owned");
}
