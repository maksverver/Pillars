#include "Board.h"
#include "Group.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

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


uint32_t hash_data[4][100] = {
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

#define HASH_TABLE_SIZE (3999971)

GroupCacheEntry *group_cache[HASH_TABLE_SIZE];

#if 0
static int nvalue_bruteforce_work(Mask *moves, int *skip, int grp_size)
{
    static NV memo[1<<ANALYSIS_MAX_SIZE];

    int n, nval;
    Mask mask, move;
    uint32_t nvals;

    mask = ((Mask)1<<grp_size);
    do {
        --mask;

        nvals = 0;
        n = 0;
        while ((move = moves[n]) != 0)
        {
            if (mask&move)
            {
                n += skip[n];
            }
            else
            {
                nvals |= (1<<memo[mask|move]);
                n += 1;
            }
        }

        nval = 0;
        while ((nvals & (1 << nval)) != 0) ++nval;
        memo[mask] = nval;
    } while (mask > 0);

    return memo[0];
}

/* Alternative, brute-force implementation of nim-value analysis, which
   does not utilize the global cache. */
static NV group_nvalue_bruteforce(Group *gr)
{
    static Mask moves[MAX_MOVES+1];
    static int skip[MAX_MOVES+1];

    Mask mask, *move;
    int num_moves;
    Rect rect;

    /* Figure out all valid moves */
    num_moves = 0;
    for (rect.p.r = 0; rect.p.r < gr->height; ++rect.p.r)
    {
        for (rect.p.c = 0; rect.p.c < gr->width; ++rect.p.c)
        {
            if (!gr->board[rect.p.r][rect.p.c]) continue;

            for (rect.q.r = rect.p.r + 1; rect.q.r <= gr->height; ++rect.q.r)
            {
                for (rect.q.c = rect.p.c + 1; rect.q.c <= gr->width; ++rect.q.c)
                {
                    int r, c;

                    for (r = rect.p.r; r < rect.q.r; ++r)
                    {
                        for (c = rect.p.c; c < rect.q.c; ++ c)
                        {
                            if (!gr->board[r][c]) goto invalid;
                        }
                    }

                    mask = 0;
                    for (r = 0; r < gr->height; ++r)
                    {
                        for (c = 0; c < gr->width; ++ c)
                        {
                            if (!gr->board[r][c]) continue;
                            mask = 2*mask + ( r >= rect.p.r && r < rect.q.r &&
                                              c >= rect.p.c && c < rect.q.c );
                        }
                    }
                    moves[num_moves++] = mask;
                invalid: continue;
                }
            }
        }
    }
    moves[num_moves] = 0;   /* mark end of moves */

    /* Calculate move skip data */
    for (move = moves; *move != 0; ++move)
    {
        skip[move - moves] = 1;
        while ((move[skip[move - moves]]&*move) == *move) ++skip[move - moves];
    }

    /* Calculate nim value (this takes the most time) */
    return nvalue_bruteforce_work(moves, skip, gr->pop);
}
#endif

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

void group_print(const Group *gr)
{
    int r, c;
    printf("[%dx%d]\n", gr->height, gr->width);
    for (r = 0; r < gr->height; ++r)
    {
        for (c = 0; c < gr->width; ++c)
        {
            fputc(".x"[(gr->rows[r]>>c)&1], stdout);
        }
        fputc('\n', stdout);
    }
}

/* Isolates a connected subgroup of the given group (which is not in standard
   form; i.e. it may contain multiple connected components). The subgroup is
   moved form src to dst (so both arguments are modified!)
*/
int group_isolate(Group *src, int r, int c, Group *dst)
{
    /* flood-fill queue */
    uint8_t q[100][2];
    int qlen, qpos;
    /* bounding rectangle */
    Rect b;

    memset(dst->rows, 0, sizeof(dst->rows));

    assert(GR_GET(src, r, c));
    GR_TOG(src, r, c);
    q[0][0] = r;
    q[0][1] = c;
    qlen = 1;
    for (qpos = 0; qpos < qlen; ++qpos)
    {
        r = q[qpos][0];
        c = q[qpos][1];
        GR_TOG(dst, r, c);

        if (r - 1 >= 0 && GR_GET(src, r - 1, c))
        {
            GR_TOG(src, r - 1, c);
            q[qlen][0] = r - 1;
            q[qlen][1] = c;
            ++qlen;
        }

        if (r + 1 < src->height && GR_GET(src, r + 1, c))
        {
            GR_TOG(src, r + 1, c);
            q[qlen][0] = r + 1;
            q[qlen][1] = c;
            ++qlen;
        }

        if (c && ((src->rows[r] & (1 << (c - 1)))))
        {
            GR_TOG(src, r, c - 1);
            q[qlen][0] = r;
            q[qlen][1] = c - 1;
            ++qlen;
        }

        if (src->rows[r] & (1 << (c + 1)))
        {
            GR_TOG(src, r, c + 1);
            q[qlen][0] = r;
            q[qlen][1] = c + 1;
            ++qlen;
        }
    }

    /* Determine bounding rectangle */
    {
        int mask;

        b.p.r = 0;
        while (!dst->rows[b.p.r]) ++b.p.r;
        b.q.r = src->height - 1;
        while (!dst->rows[b.q.r]) --b.q.r;

        mask = 0;
        for (r = b.p.r; r <= b.q.r; ++r) mask |= dst->rows[r];

        b.p.c = 0;
        while ((mask & (1<<b.p.c)) == 0) ++b.p.c;
        b.q.c = 9;
        while ((mask & (1<<b.q.c)) == 0) --b.q.c;
    }

    dst->height = b.q.r - b.p.r + 1;
    dst->width  = b.q.c - b.p.c + 1;

    /* Translate isolated group to upper-left corner */
    {
        int r;
        for (r = 0; r < dst->height; ++r)
        {
            dst->rows[r] = dst->rows[r + b.p.r] >> b.p.c;
        }
        for ( ; r < 10; ++r) dst->rows[r] = 0;
    }

    return qlen;
}

static void group_hash(Group *gr, GroupId *id)
{
    int r, c, bit;

    id->hash[0] = 16777619*gr->height ^ gr->width;
    id->hash[1] = 0;
    id->hash[2] = 0;
    id->hash[3] = 0;
    for (r = 0; r < gr->height; ++r)
    {
        bit = 1;
        for (c = 0; c < gr->width; ++c)
        {
            if (gr->rows[r]&(1<<c))
            {
                id->hash[0] ^= hash_data[0][10*r + c];
                id->hash[1] ^= hash_data[1][10*r + c];
                id->hash[2] ^= hash_data[2][10*r + c];
                /* id->hash[3] ^= hash_data[3][10*r + c]; */
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
    if (*gce == NULL) group_cache_store(gce, &id, nval);
}

/* Stores all configurations of the given group in the group cache, including
   all equivalent configurations obtained by rotations/reflections. */
static void group_cache_store_all(Group *gr, NV nval)
{
    Group alts[8];
    int n, num_alts;

    alts[0] = *gr;
    num_alts = group_alternatives(alts);
    for (n = 0; n < num_alts; ++n) insert_new_nval(&alts[n], nval);
}

__attribute__((noinline))
static NV group_nvalue_smart(Group *gr)
{
    unsigned nvals;             /* nim-values of reachable states */
    NV nval;                    /* group nim-value (result) */
    Rect m;                     /* move */

    /* Consider all possible moves */
    nvals = 0;
    for (m.p.r = 0; m.p.r < gr->height; ++m.p.r)
    {
        for (m.p.c = 0; m.p.c < gr->width; ++m.p.c)
        {
            if (!GR_GET(gr, m.p.r, m.p.c)) continue;

            for (m.q.r = m.p.r; m.q.r < gr->height; ++m.q.r)
            {
                if (!GR_GET(gr, m.q.r, m.p.c)) break;
                for (m.q.c = m.p.c; m.q.c < gr->width; ++m.q.c)
                {
                    Group ngr, nngr;
                    int r, c;
                    NV nnval;       /* nim-value of new group */

                    /* Check if rectangle is covered completely by fields */
                    for (r = m.p.r; r <= m.q.r; ++r)
                    {
                        for (c = m.p.c; c <= m.q.c; ++c)
                        {
                            if (!GR_GET(gr, r, c)) goto invalid;
                        }
                    }

                    /* Construct new group, with selected rectangle removed */
                    ngr.height = gr->height;
                    ngr.width  = gr->width;
                    for (r = 0; r < m.p.r; ++r) ngr.rows[r] = gr->rows[r];
                    for (; r <= m.q.r; ++r)
                    {
                        ngr.rows[r] = gr->rows[r] ^ (((1<<(m.q.c - m.p.c + 1)) - 1) << m.p.c);
                    }
                    for (; r < gr->height; ++r) ngr.rows[r] = gr->rows[r];

                    /* Split up into groups */
                    nnval = 0;
                    for (r = 0; r < gr->height; ++r)
                    {
                        while (ngr.rows[r] != 0)
                        {
                            c = __builtin_ctz(ngr.rows[r]);
                            nnval ^= group_isolate(&ngr, r, c, &nngr) > 1
                                     ? group_nvalue(&nngr) : 1;

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

    return nval;
}

/* Determine the nim value for a group. */
NV group_nvalue(Group *gr)
{
    GroupCacheEntry **gce;      /* group cache entry */
    GroupId id;                 /* identification of the group */
    NV nval;

    /* Look up in cache */
    /* group_normalize(gr); */
    group_hash(gr, &id);
    gce = group_cache_lookup(&id);
    if (*gce != NULL) return (*gce)->nvalue;

    /* Calculate nim value */
    nval = group_nvalue_smart(gr);

    /* Store in cache */
    /* group_cache_store(gce, &id, nval); */
    group_cache_store_all(gr, nval);

    return nval;
}
