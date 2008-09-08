#include "Analysis.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static bool analyis_initialized = false;    /* has the module been initialized? */

/* Models a single group, enclosed in the rectangle (0,0)-(height,width)
   Every field in this rectangle is set to 1 if it is part of the group; fields
   outside the rectangle may have random values. Fields must be connected. */
typedef struct Group
{
    int height, width;
    Board board;
} Group;

uint64_t hash_data[4][100] = {
    {   0x0a25244du, 0x917ade9fu, 0xbc875b53u, 0xc70cfaa5u,
        0x96b3f82fu, 0xad84fbb0u, 0x25ec883fu, 0x249a03fcu,
        0x68f76ac1u, 0x9b011c6fu, 0x8de11b5du, 0xca7efa1eu,
        0x1e03c9d1u, 0xea560014u, 0xf92e471fu, 0xee36d423u,
        0x134d1c08u, 0x906672f4u, 0x4713d58cu, 0x8be73a9au,
        0x1a1f1534u, 0xc9c5b48bu, 0xec6b776du, 0xa64eea6eu,
        0x717e7691u, 0x69e650f4u, 0xf3b810eau, 0xd28a0a65u,
        0xb5a4ec98u, 0xb6832c1au, 0x034fe66fu, 0x86d973bau,
        0x464b2cb7u, 0x1c447eeeu, 0x584e4ddbu, 0x77ef6ba7u,
        0x8d67ecc8u, 0xb19488e7u, 0x382ee4b6u, 0xeed7f1b8u,
        0x67cc4c7bu, 0xf58c8afau, 0x5de2488bu, 0xced8fd85u,
        0x82402cc9u, 0xea7fe2fau, 0xbf902e61u, 0x6202af45u,
        0xec940315u, 0x6e777347u, 0xc2f49e01u, 0x18ceb4beu,
        0xd0a33658u, 0x15cafc90u, 0x13a62fafu, 0xb55369afu,
        0xbeea8280u, 0x60af37deu, 0x3181d2b6u, 0x0eed1f50u,
        0x1eea5af9u, 0x341d1e2cu, 0xfdd8500fu, 0x72bd9b0bu,
        0xdcfe478fu, 0x1d96551cu, 0x04fb4188u, 0xb56d23eeu,
        0x543412d9u, 0x5290f1b1u, 0x39fc7da7u, 0xb6ab3aafu,
        0x959317b7u, 0x2580b87bu, 0x60128d16u, 0xe83dbda3u,
        0xd8137fccu, 0xe26d4113u, 0xdceb1eb3u, 0x59ad7c5au,
        0x03f7deacu, 0x97ca5f23u, 0xe1fc7a4eu, 0xda4a37c4u,
        0x7b50bcadu, 0x70f2c0efu, 0x1e619af5u, 0xb7bb009eu,
        0xd843b6c9u, 0xdb7be755u, 0x1e470ef3u, 0x09163af0u,
        0x2a75a8ebu, 0x26d7f8cfu, 0x0a569494u, 0x0bfabef1u,
        0x29c151aau, 0x1b2f51e7u, 0x17bc272cu, 0x64615934u
    },
    {   0x895f2715u, 0x36e24859u, 0x9a1a1633u, 0xc5b8926cu,
        0x6adf8a1bu, 0xc08aab21u, 0xd42a63fcu, 0xae55d3eeu,
        0xe38dc404u, 0x848364c2u, 0xe224d86au, 0x59c5406cu,
        0xd6f0e906u, 0xdf7d15dau, 0x854333f9u, 0x46c3833au,
        0xb2b95680u, 0x9d7d2935u, 0xc5830927u, 0x22556287u,
        0xa5760ff9u, 0x62aa0982u, 0x55e3a264u, 0xc06f140fu,
        0x0d0a2296u, 0x8901da8du, 0xa07fd1d2u, 0xa05b6fbcu,
        0x90c2ccffu, 0xd29e5270u, 0x40d3212bu, 0x14563f5au,
        0xf39d9668u, 0x15d05a2du, 0x21fa6b1eu, 0x26a02966u,
        0x92a419c8u, 0xfda5a99du, 0x829b790au, 0xf28cec82u,
        0x92607270u, 0x4a697fc9u, 0x7bfff723u, 0x1a02a37au,
        0x7f3492deu, 0xe5c8222bu, 0x3541105cu, 0x75813b80u,
        0xdd7920ebu, 0x4423721au, 0x9ba82481u, 0x62eaf763u,
        0x9a87f626u, 0xf9bfcb83u, 0x25e87f09u, 0x62a9d3dbu,
        0x9c7fd7abu, 0x8b401c21u, 0x520c512fu, 0xc51b7d29u,
        0x608c19c8u, 0x0d659938u, 0x521c8c4bu, 0x514737e7u,
        0xad75ac80u, 0x8a056b6au, 0x81443d28u, 0xe20a1792u,
        0x8b71247bu, 0xe4bf42f4u, 0x6a727755u, 0x2f1f57f9u,
        0xc1705a47u, 0x6b8a331cu, 0x5192aa85u, 0x1cfde229u,
        0x9c5e5500u, 0x2777919au, 0x8e6bdd7fu, 0xc91ee766u,
        0x169345cau, 0xaa602b04u, 0x0ad55db9u, 0x1b06eb71u,
        0x0767a141u, 0x474bf88du, 0x980de408u, 0x60cac7e1u,
        0xc46ab382u, 0x7fb4e6e8u, 0x709ac749u, 0x457f2cbau,
        0xd47ac3f9u, 0xbe86556eu, 0xf27b8238u, 0x0bcf4ce5u,
        0xbc1d5661u, 0x010afb7eu, 0x990d6df1u, 0xae22f536u
    },
    {   0xd4e758b1u, 0xe6265cf1u, 0xb6481922u, 0x6ddb98eeu,
        0xf7bbb7bbu, 0xa13be522u, 0x035f2969u, 0xcd14634bu,
        0x5dacd601u, 0xcca25ecfu, 0x9dbd1691u, 0x41aceb60u,
        0x5ff8b901u, 0xc54ad686u, 0x3ede564bu, 0x9ff3356fu,
        0x2a17cad0u, 0x8f43f0e6u, 0x30f528cbu, 0xd1c4326au,
        0x16becadfu, 0x715c900bu, 0x172fba80u, 0xe453c6dbu,
        0xf7ee6c68u, 0xc72a739cu, 0x4291a45eu, 0x6d886bfeu,
        0x18df3c9au, 0x3dd8031au, 0x5b2f4f7au, 0x78fe1dfcu,
        0x491bc407u, 0xa3c8df35u, 0x388f4ef4u, 0xacd11627u,
        0x33422aecu, 0x845006a6u, 0x9628cf6bu, 0x1669377fu,
        0xf7557728u, 0x07877813u, 0xa74cafaau, 0x38ef5034u,
        0xe3a45256u, 0x310bb0ceu, 0xee178c0bu, 0x13af70dbu,
        0x976c1e92u, 0xfe7770a9u, 0x74563455u, 0x6fec540du,
        0xb62395d3u, 0xc6f7922eu, 0xf4fefde8u, 0xe3e9a940u,
        0x746acd64u, 0x42f97805u, 0xbf1e5645u, 0x8fe0f669u,
        0xd15de8a5u, 0x973937feu, 0xa1cd0d34u, 0x65a845eau,
        0xe1d78260u, 0xd867acc0u, 0x34be7b50u, 0xefa465e4u,
        0x77982ad4u, 0xec81f720u, 0x94e7e36bu, 0x1b3d8e23u,
        0xf5762901u, 0x2ded2d0eu, 0x2ccd102fu, 0x5dc75b90u,
        0x67a34919u, 0x95da4bfcu, 0x9e04be3bu, 0xb9d79c24u,
        0x4b64f3feu, 0xe48e6b63u, 0x0bb5950bu, 0x31a59754u,
        0xf802e56bu, 0x778df383u, 0xf76a3513u, 0x20a80f9cu,
        0x414cc4eeu, 0x17da8898u, 0xdfa581cfu, 0x9993da66u,
        0xf9a3fcafu, 0x07c24aadu, 0x04036c99u, 0xe4de56f8u,
        0x2b265c83u, 0xedb0ba7eu, 0xf0f30168u, 0x5176e5f7u
    },
    {   0xe69caf41u, 0x79bc4b7bu, 0x10f7ec40u, 0x2d04f669u,
        0x5148d8cdu, 0x97810848u, 0x4e01c8ebu, 0xe26fbc8cu,
        0x409427cau, 0x56194fb7u, 0x54156151u, 0x3c2566b2u,
        0xecaf639du, 0x49688247u, 0xabbe58c5u, 0xb8688fcfu,
        0x374c360cu, 0xe0dddf10u, 0x4cf02f10u, 0xe1a9e4f6u,
        0xb6a5adecu, 0xa341d46au, 0x1c9fb00cu, 0x3b2af0d4u,
        0xa2a260c7u, 0x4bb4ef93u, 0x55dfc1d8u, 0xed69eb38u,
        0xf27f1a3cu, 0x341358a4u, 0x0cb1bb4eu, 0x65c0f4c6u,
        0x0cd4ebd8u, 0x12d241d7u, 0xf30caa86u, 0x54c49fb3u,
        0x261b0239u, 0x9c18b53bu, 0x508b9e41u, 0xebf9d7a6u,
        0x94186606u, 0xa60ed3b1u, 0x01800da9u, 0x7c333d1fu,
        0x4fc9a7b3u, 0xa3e82602u, 0x452dea8bu, 0x77be18a7u,
        0x9a61b484u, 0x3aaeb579u, 0xf7771edbu, 0x1ee19105u,
        0xaf8e081fu, 0x2d2f24f3u, 0xcec1053fu, 0xcbf8b100u,
        0x62f6dcb6u, 0xfb7c08a8u, 0x36390e90u, 0x0cfbf9edu,
        0xd3371cd2u, 0x3c036733u, 0xe1dea7e8u, 0x86c77b2fu,
        0xb0e91784u, 0x19d4b7feu, 0xe36c3132u, 0xaab5e29bu,
        0x81f19db8u, 0x4fb45d00u, 0x1c1a0eadu, 0x5c93c49bu,
        0xd24c57d3u, 0x0b37ce4fu, 0xe5f15b02u, 0x24c36e0au,
        0x3a839f83u, 0x758c3977u, 0xcb46ba58u, 0x02bf08c6u,
        0x7cd26bd8u, 0xf0b10afcu, 0xb7f95ed2u, 0x3773de5bu,
        0xed43d639u, 0xb250b4efu, 0x3407a15eu, 0x74ad66d4u,
        0xa22dcf66u, 0xecd17de2u, 0x6594574fu, 0x0b3f6fc8u,
        0xf863d83fu, 0x7fb879e3u, 0x810557a7u, 0x7534d416u,
        0x8c8600bbu, 0xcf4e6a58u, 0xb247ce6au, 0x4228be94u
    } };

#define HASH_TABLE_SIZE (1000001)

typedef struct GroupId
{
    uint32_t hash[4];
} GroupId;

typedef struct GroupCacheEntry
{
    struct GroupCacheEntry *next;

    GroupId     id;
    NV          nvalue;

} GroupCacheEntry;

GroupCacheEntry *group_cache[HASH_TABLE_SIZE];

void debug_cache_info()
{
    int n;
    int total = 0, cnt[21] = { };

    GroupCacheEntry *gce;
    for (n = 0; n < HASH_TABLE_SIZE; ++n)
    {
        int c = 0;
        for (gce = group_cache[n]; gce != NULL; gce = gce->next) ++c;
        cnt[c > 20 ? 20 :c] += 1;
        total += c;
    }
    printf( "Total cache population: %d (size: %d; %2.3f%% full)\n",
            total, HASH_TABLE_SIZE, 100.0*total/HASH_TABLE_SIZE );
    for (n = 0; n <= 20; ++n) printf("%4d: %12d\n", n, cnt[n]);
}

static void mirror_horizontal(Group *src, Group *dst)
{
    int r, c1, c2;

    dst->height = src->height;
    dst->width  = src->width;
    for (r = 0; r < src->height; ++r)
    {
        c1 = 0;
        c2 = src->width;
        while (c2 > 0) dst->board[r][c1++] = src->board[r][--c2];
    }
}

static void mirror_vertical(Group *src, Group *dst)
{
    int r1, r2, c;

    dst->height = src->height;
    dst->width  = src->width;

    r1 = 0;
    r2 = src->height;
    while (r2 > 0)
    {
        --r2;
        for (c = 0; c < src->width; ++c) dst->board[r1][c] = src->board[r2][c];
        ++r1;
    }
}

static void mirror_diagonal(Group *src, Group *dst)
{
    int r, c;

    dst->height = src->width;
    dst->width  = src->height;

    for (r = 0; r < dst->height; ++r)
    {
        for (c = 0; c < dst->width; ++c)
        {
            dst->board[r][c] = src->board[c][r];
        }
    }
}

static int group_cmp(Group *gr1, Group *gr2)
{
    int r, c;
    assert(gr1->height == gr2->height && gr1->width == gr2->width);
    for (r = 0; r < gr1->height; ++r)
    {
        for (c = 0; c < gr1->width; ++c)
        {
            if (gr1->board[r][c] != gr2->board[r][c])
            {
                return gr1->board[r][c] - gr2->board[r][c];
            }
        }
    }
    return 0;
}

/* Normalizes a group with respect to horizontal/vertical reflections
   (but not rotations!) */
static void group_normalize_half(Group *gr)
{
    Group alt1, alt2;

    mirror_horizontal(gr, &alt1);
    if (group_cmp(gr, &alt1) > 0) *gr = alt1;

    mirror_vertical(&alt1, &alt2);
    if (group_cmp(gr, &alt2) > 0) *gr = alt2;

    mirror_horizontal(&alt2, &alt1);
    if (group_cmp(gr, &alt1) > 0) *gr = alt1;
}

/* Normalizes a group with respect to rotations/reflections.
   Note that this does not translate the group; the group is already assumed
   to be in its bounding rectangle! */
static void group_normalize(Group *gr)
{
    Group alt;

    if (gr->width == gr->height)
    {
        group_normalize_half(gr);
        mirror_diagonal(gr, &alt);
        group_normalize_half(&alt);
    }
    else
    {
        if (gr->width < gr->height)
        {
            mirror_diagonal(gr, &alt);
            *gr = alt;
        }
        group_normalize_half(gr);
    }
}

static void group_print(Group *gr)
{
    int r, c;
    printf("[%dx%d]\n", gr->height, gr->width);
    for (r = 0; r < gr->height; ++r)
    {
        for (c = 0; c < gr->width; ++c)
        {
            fputc( gr->board[r][c] >= 0 && gr->board[r][c] < 10
                    ? '0' + gr->board[r][c] : '?', stdout);
        }
        fputc('\n', stdout);
    }
}

/* Isolates a connected subgroup of the given group (which is not in standard
   form; i.e. it may contain multiple connected components). The subgroup is
   moved form src to dst (so both arguments are modified!) */
static void group_isolate(Group *src, int r, int c, Group *dst)
{
    int q[100][2], qlen, qpos;     /* flood-fill queue */
    Rect b;                        /* bounding rectangle */

    memset(&dst->board, 0, sizeof(Board));
    b.p.r = b.p.c = 9;
    b.q.r = b.q.c = 0;

    src->board[r][c] = 0;
    q[0][0] = r;
    q[0][1] = c;
    qlen = 1;
    for (qpos = 0; qpos < qlen; ++qpos)
    {
        r = q[qpos][0];
        c = q[qpos][1];
        dst->board[r][c] = 1;

        if (r < b.p.r) b.p.r = r;
        if (r > b.q.r) b.q.r = r;
        if (c < b.p.c) b.p.c = c;
        if (c > b.q.c) b.q.c = c;

        if (r - 1 >= 0 && src->board[r - 1][c])
        {
            src->board[r - 1][c] = 0;
            q[qlen][0] = r - 1;
            q[qlen][1] = c;
            ++qlen;
        }

        if (r + 1 < src->height && src->board[r + 1][c])
        {
            src->board[r + 1][c] = 0;
            q[qlen][0] = r + 1;
            q[qlen][1] = c;
            ++qlen;
        }

        if (c - 1 >= 0 && src->board[r][c - 1])
        {
            src->board[r][c - 1] = 0;
            q[qlen][0] = r;
            q[qlen][1] = c - 1;
            ++qlen;
        }

        if (c + 1 < src->width && src->board[r][c + 1])
        {
            src->board[r][c + 1] = 0;
            q[qlen][0] = r;
            q[qlen][1] = c + 1;
            ++qlen;
        }
    }

    /* Translate isolated group to upper-left corner */
    dst->height = b.q.r - b.p.r + 1;
    dst->width  = b.q.c - b.p.c + 1;
    assert(sizeof(dst->board[0]) == 10);
    memmove(&dst->board[0][0], &dst->board[b.p.r][b.p.c],
        sizeof(dst->board[0])*(b.q.r - b.p.r) + (b.q.c - b.p.c + 1));
}

static void group_hash(Group *gr, GroupId *id)
{
    int r, c;

    id->hash[0] = 0;
    id->hash[1] = 0;
    id->hash[2] = 0;
    id->hash[3] = 0;
    for (r = 0; r < gr->height; ++r)
    {
        for (c = 0; c < gr->width; ++c)
        {
            if (gr->board[r][c])
            {
                id->hash[0] ^= hash_data[0][10*r + c];
                id->hash[1] ^= hash_data[1][10*r + c];
                id->hash[2] ^= hash_data[2][10*r + c];
                id->hash[3] ^= hash_data[3][10*r + c];
            }
        }
    }
}

static GroupCacheEntry **group_cache_lookup(GroupId *id)
{
    GroupCacheEntry **gce;

    gce = &group_cache[id->hash[0]%HASH_TABLE_SIZE];
    while (*gce && (
        id->hash[0] != (*gce)->id.hash[0] ||
        id->hash[1] != (*gce)->id.hash[1] ||
        id->hash[2] != (*gce)->id.hash[2] ||
        id->hash[3] != (*gce)->id.hash[3]))
    {
        gce = &(*gce)->next;
    }
    return gce;
}

static void group_cache_store(GroupCacheEntry **gce, GroupId *id, NV nval)
{
    *gce = malloc(sizeof(GroupCacheEntry));
    assert(gce != NULL);
    (*gce)->next = NULL;
    memcpy(&(*gce)->id, id, sizeof(GroupId));
    (*gce)->nvalue = nval;
}

/* Stores an nvalue for the given group, if it is not yet stored */
static void insert_new_nval(Group *gr, NV nval)
{
    GroupCacheEntry **gce;
    GroupId id;

    group_hash(gr, &id);
    gce = group_cache_lookup(&id);
    if (*gce != NULL) return;
    group_cache_store(gce, &id, nval);
}

/* Stores alternate (but equivalent) configurations of the given group
   in the group cache.
   NB: Assumes the primary configuration has already been stored! */
static void group_cache_store_alternatives(Group *gr, NV nval)
{
    Group a, b;

    mirror_horizontal(gr, &a);
    insert_new_nval(&a, nval);

    mirror_vertical(&a, &b);
    insert_new_nval(&b, nval);

    mirror_vertical(&b, &a);
    insert_new_nval(&a, nval);

    mirror_diagonal(&a, &b);
    insert_new_nval(&b, nval);

    mirror_horizontal(&b, &a);
    insert_new_nval(&a, nval);

    mirror_vertical(&a, &b);
    insert_new_nval(&b, nval);

    mirror_vertical(&b, &a);
    insert_new_nval(&a, nval);
}

/* Determine the nim value for a group.
   NB: modifies the argument to normalize it! */
static NV group_nvalue(Group *gr)
{
    GroupCacheEntry **gce;      /* group cache entry */
    GroupId id;                 /* identification of the group */
    unsigned nvals;             /* nim-values of reachable states */
    NV nval;                    /* group nim-value (result) */
    Rect m;                     /* move */

    /* Look up in cache */
    group_hash(gr, &id);
    gce = group_cache_lookup(&id);
    if (*gce != NULL) return (*gce)->nvalue;

    /* Consider all possible moves */
    nvals = 0;
    for (m.p.r = 0; m.p.r < gr->height; ++m.p.r)
    {
        for (m.p.c = 0; m.p.c < gr->width; ++m.p.c)
        {
            if (!gr->board[m.p.r][m.p.c]) continue;

            for (m.q.r = m.p.r; m.q.r < gr->height; ++m.q.r)
            {
                if (!gr->board[m.q.r][m.p.c]) break;
                for (m.q.c = m.p.c; m.q.c < gr->width; ++m.q.c)
                {
                    Group ngr, nngr;
                    int r, c;
                    NV nnval;       /* nim-value of new groups */

                    /* Check if rectangle is covered completely by fields */
                    for (r = m.p.r; r <= m.q.r; ++r)
                    {
                        for (c = m.p.c; c <= m.q.c; ++c)
                        {
                            if (!gr->board[r][c]) goto invalid;
                        }
                    }

                    /* Construct new group, with selected rectangle removed */
                    ngr = *gr;
                    for (r = m.p.r; r <= m.q.r; ++r)
                    {
                        for (c = m.p.c; c <= m.q.c; ++c)
                        {
                            ngr.board[r][c] = 0;
                        }
                    }

                    /* Split up into groups */
                    nnval = 0;
                    for (r = 0; r < gr->height; ++r)
                    {
                        for (c = 0; c < gr->width; ++c)
                        {
                            if (ngr.board[r][c] != 0)
                            {
                                group_isolate(&ngr, r, c, &nngr);
                                nnval ^= group_nvalue(&nngr);
                            }
                        }
                    }

                    nvals |= (1u << nnval);

                    continue;
                invalid: break;
                }
            }
        }
    }

    /* Compute nvalue */
    nval = 0;
    while (nvals & (1u << nval)) ++nval;

    /* Store in cache */
    group_cache_store(gce, &id, nval);
    group_cache_store_alternatives(gr, nval);

    return nval;
}

/* Determine the nim value for the given group. */
static NV nvalue(Board *brd, GroupInfo *gi, int g)
{
    Group gr;
    int r, c;

    if (gi->size[g] > ANALYSIS_MAX_SIZE) return -1;

    gr.height = gi->bounds[g].q.r - gi->bounds[g].p.r;
    gr.width  = gi->bounds[g].q.c - gi->bounds[g].p.c;
    for (r = 0; r < gr.height; ++r)
    {
        for (c = 0; c < gr.width; ++c)
        {
            gr.board[r][c] = (*brd)[r + gi->bounds[g].p.r][c + gi->bounds[g].p.c] == g + 1;
        }
    }

    return group_nvalue(&gr);
}

/* Flood fill the board from a specific position,
   filling empty squares with group (gi->num_groups + 1). */
static void fill(Board *brd, GroupInfo *gi, int r, int c)
{
    Rect *rect;
    struct QueueEntry { int r, c; } queue[100];
    int pos, len, g;

    g = gi->num_groups;
    rect = &gi->bounds[g];
    ((*brd)[r][c]) = g + 1;
    len = pos = 0;
    queue[len++] = (struct QueueEntry) { r, c };
    for (pos = 0; pos < len; ++pos)
    {
        r = queue[pos].r;
        c = queue[pos].c;
        if (r < rect->p.r) rect->p.r = r;
        if (r > rect->q.r) rect->q.r = r;
        if (c < rect->p.c) rect->p.c = c;
        if (c > rect->q.c) rect->q.c = c;
        if (r > 0 && (*brd)[r - 1][c] == 0)
        {
            ((*brd)[r - 1][c]) = g + 1;
            queue[len++] = (struct QueueEntry) { r - 1, c };
        }
        if (r < 9 && (*brd)[r + 1][c] == 0)
        {
            ((*brd)[r + 1][c]) = g + 1;
            queue[len++] = (struct QueueEntry) { r + 1, c };
        }
        if (c > 0 && (*brd)[r][c - 1] == 0)
        {
            ((*brd)[r][c - 1]) = g + 1;
            queue[len++] = (struct QueueEntry) { r, c - 1};
        }
        if (c < 9 && (*brd)[r][c + 1] == 0)
        {
            ((*brd)[r][c + 1]) = g + 1;
            queue[len++] = (struct QueueEntry) { r, c + 1};
        }
    }
    gi->size[g] = len;
}

void analysis_initialize()
{
    /* does nothing right now (but must be called anyway) */
    analyis_initialized = true;
}

void analysis_identify_groups(Board *brd, GroupInfo *gi)
{
    int r, c;

    assert(analyis_initialized);

    gi->num_groups = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*brd)[r][c] == 0)
            {
                gi->first [gi->num_groups] = (Point) { r, c };
                gi->bounds[gi->num_groups] = (Rect) { { r, c }, { r, c } };
                fill(brd, gi, r, c);
                gi->bounds[gi->num_groups].q.r += 1;
                gi->bounds[gi->num_groups].q.c += 1;
                gi->num_groups += 1;
            }
        }
    }
}

void analysis_nim_values(Board *brd, GroupInfo *gi)
{
    int n;

    assert(analyis_initialized);

    for (n = 0; n < gi->num_groups; ++n)
    {
        gi->nval[n] = nvalue(brd, gi, n);
    }
}

int analysis_value_moves(Board *brd_in, Rect *moves, int *values)
{
    int move, num_moves;
    Board brd;
    GroupInfo gi;

    assert(analyis_initialized);

    num_moves = board_list_moves(brd_in, moves);
    for (move = 0; move < num_moves; ++move)
    {
        int group, num_ones, nsum, guessed;

        /* Construct the board after executing the i-th move */
        memcpy(brd, brd_in, sizeof(brd));
        board_flatten(&brd);
        board_fill(&brd, &moves[move], -1);
        analysis_identify_groups(&brd, &gi);
        analysis_nim_values(&brd, &gi);

        /* Determine status of the board by computing its nim-sum */
        num_ones = 0;
        nsum     = 0;
        guessed  = 0;

        for (group = 0; group < gi.num_groups; ++group)
        {
            if (gi.size[group] <= 1) ++num_ones;
            if (gi.nval[group] < 0)
            {
                nsum ^= gi.size[group];
                ++guessed;
            }
            else
            {
                nsum ^= gi.nval[group];
            }
        }

        if (num_ones == gi.num_groups)
        {
            /* Only groups with nim value <= 1 left */
            values[move] = (nsum == 0) ? -1 : +1;
        }
        else
        {
            values[move] = (nsum == 0) ? +1 : -1;
        }

        if (!guessed) values[move] *= 2;
    }

    return num_moves;
}
