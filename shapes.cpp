extern "C"
{
#include "Board.h"
#include "Analysis.h"
}

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <map>
#include <set>
#include <vector>

bool operator< (const Group &a, const Group &b)
{
    if (a.pop != b.pop) return a.pop < b.pop;
    if (a.height != b.height) return a.height < b.height;
    if (a.width != b.width) return a.width < b.width;
    for (int r = 0; r < a.height; ++r)
    {
        for (int c = 0; c < a.width; ++c)
        {
            if (a.board[r][c] != b.board[r][c])
                return a.board[r][c] < b.board[r][c];
        }
    }
    return false;
}

std::map<Group, int> groups;

void add(const Group &g)
{
    Group h = g;
    group_normalize(&h);
    if (groups.find(h) == groups.end()) groups.insert(std::pair<Group,int>(h, groups.size() + 1));
}

int main()
{
    analysis_initialize();

    const int max_fields = 5;

    Group one;
    one.width = one.height = one.pop = 1;
    one.board[0][0] = 1;
    groups.insert(std::pair<Group,int>(one, 1));

    for (int n = 2; n <= max_fields; ++n)
    {
        for (std::map<Group,int>::const_iterator it = groups.begin();
             it != groups.end(); ++it)
        {
            const Group &f = it->first;
            if (f.pop != n - 1) continue;

            Group g;
            g.pop = n;

            /* Add field in the middle. */
            memcpy(g.board, f.board, sizeof(g.board));
            g.height = f.height;
            g.width  = f.width;
            for (int r = 0; r < f.height; ++r)
            {
                for (int c = 0; c < f.width; ++c)
                {
                    if (!g.board[r][c])
                    {
                        g.board[r][c] = 1;
                        add(g);
                        g.board[r][c] = 0;
                    }
                }
            }

            if (f.height < 10)
            {
                /* Add field to the top */
                g.height = f.height + 1;
                g.width  = f.width;
                memset(g.board, 0, sizeof(g.board));
                for (int r = 0; r < f.height; ++r)
                {
                    for (int c = 0; c < f.width; ++c)
                    {
                        g.board[r+1][c] = f.board[r][c];
                    }
                }
                for (int c = 0; c < f.width; ++c)
                {
                    if (f.board[0][c])
                    {
                        g.board[0][c] = 1;
                        add(g);
                        g.board[0][c] = 0;
                    }
                }

                /* Add field to the bottom */
                g.height = f.height + 1;
                g.width  = f.width;
                memset(g.board, 0, sizeof(g.board));
                for (int r = 0; r < f.height; ++r)
                {
                    for (int c = 0; c < f.width; ++c)
                    {
                        g.board[r][c] = f.board[r][c];
                    }
                }
                for (int c = 0; c < f.width; ++c)
                {
                    if (f.board[f.height - 1][c])
                    {
                        g.board[f.height][c] = 1;
                        add(g);
                        g.board[f.height][c] = 0;
                    }
                }
            }

            /* Add field to the left */
            if (g.width < 10)
            {
                g.height = f.height;
                g.width  = f.width + 1;
                memset(g.board, 0, sizeof(g.board));
                for (int r = 0; r < f.height; ++r)
                {
                    for (int c = 0; c < f.width; ++c)
                    {
                        g.board[r][c+1] = f.board[r][c];
                    }
                }
                for (int r = 0; r < f.height; ++r)
                {
                    if (f.board[r][0])
                    {
                        g.board[r][0] = 1;
                        add(g);
                        g.board[r][0] = 0;
                    }
                }

                /* Add field to the right */
                g.height = f.height;
                g.width  = f.width + 1;
                memset(g.board, 0, sizeof(g.board));
                for (int r = 0; r < f.height; ++r)
                {
                    for (int c = 0; c < f.width; ++c)
                    {
                        g.board[r][c] = f.board[r][c];
                    }
                }
                for (int r = 0; r < f.height; ++r)
                {
                    if (f.board[r][f.width - 1])
                    {
                        g.board[r][f.width] = 1;
                        add(g);
                        g.board[r][f.width] = 0;
                    }
                }
            }
        }
    }

    printf("g0 := Misere();\n");

    for (std::map<Group, int>::const_iterator it = groups.begin();
         it != groups.end(); ++it)
    {
        std::set<std::vector<int> > options;
        Board b;
        memcpy(&b, it->first.board, sizeof(b));
        for (int r = 0; r < 10; ++r)
        {
            for (int c = 0; c < 10; ++c)
            {
                b[r][c] = (r < it->first.height && c < it->first.width &&
                           it->first.board[r][c]) ? 0 : -1;
            }
        }
        Rect moves[MAX_MOVES];
        int num_moves = board_list_moves(&b, moves);
        for (int m = 0; m < num_moves; ++m)
        {
            GroupInfo gi;
            std::vector<int> subgroups;
            board_fill(&b, &moves[m], -1);
            analysis_identify_groups(&b, &gi);
            for (int n = 0; n < gi.num_groups; ++n)
            {
                Group g;
                g.height = gi.bounds[n].q.r - gi.bounds[n].p.r;
                g.width  = gi.bounds[n].q.c - gi.bounds[n].p.c;
                g.pop    = gi.size[n];
                for (int r = 0; r < g.height; ++r)
                {
                    for (int c = 0; c < g.width; ++c)
                    {
                        g.board[r][c] = (b[gi.bounds[n].p.r + r][gi.bounds[n].p.c + c] == n + 1);
                    }
                }
                group_normalize(&g);
                assert(groups.find(g) != groups.end());
                subgroups.push_back(groups[g]);
            }
            if (subgroups.empty()) subgroups.push_back(0);
            board_clear(&b);
            board_fill(&b, &moves[m], 0);
            std::sort(subgroups.begin(), subgroups.end());
            options.insert(subgroups);
        }

        /*
        printf("Shape %d: ", it->second);
        group_print((Group*)&(it->first));
        printf("Options:");
        for (std::set<std::vector<int> >::const_iterator it = options.begin();
             it != options.end(); ++it)
        {
            printf(" {");
            for (std::vector<int>::const_iterator it2 = it->begin(); it2 != it->end(); ++it2)
            {
                if (it2 != it->begin()) printf("+");
                printf("%d", *it2);
            }
            printf("}");
        }
        printf("\n");
        */

        printf("g%d := Misere(", it->second);
        for (std::set<std::vector<int> >::const_iterator it = options.begin();
             it != options.end(); ++it)
        {
            if (it != options.begin()) printf(", ");
            for (std::vector<int>::const_iterator it2 = it->begin(); it2 != it->end(); ++it2)
            {
                if (it2 != it->begin()) printf("+");
                printf("g%d", *it2);
            }
        }
        printf(") >> out;\n");
    }

    printf("G := g0");
    for (int n = 1; n <= (int)groups.size(); ++n) printf(" + g%d", n);
    printf(" >> out;\n");
    /* printf("Total: %d\n", (int)groups.size()); */

}
