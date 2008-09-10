/* Tool to analyze Pillars games.
   Input is a game in XML format, and output is the same file augmented with
   an analysis section. (If the input already contains an analysis section, it
   is replaced.)
*/
#include "Board.h"
#include "Analysis.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

int main(int argc, char *argv[])
{
    xmlDocPtr doc;
    xmlNodePtr gameNode;

    if (argc != 2)
    {
        printf("usage: analyze <game.xml>\n");
        exit(1);
    }

    doc = xmlParseFile(argv[2]);
    if (doc == NULL)
    {
        fprintf(stderr, "Could not read/parse input file.\n");
        exit(1);
    }

    gameNode = xmlDocGetRootElement(doc);
    if (strcmp(gameNode->name, "game") != 0)
    {
        fprintf(stderr, "Invalid file format: root element should be <game>\n");
        exit(1);
    }

    return 0;
}
