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

static xmlNodePtr analyze_board(Board *b)
{
    xmlNodePtr anaNode;

    anaNode = xmlNewNode(NULL, BAD_CAST("analysis"));
    (void)b; /* TODO */
    return anaNode;
}

int main(int argc, char *argv[])
{
    char *descr;
    Board board;
    xmlDocPtr doc;
    xmlNodePtr gameNode, boardNode, analysisNode, node;

    if (argc != 2)
    {
        printf("usage: analyze <game.xml>\n");
        exit(1);
    }

    LIBXML_TEST_VERSION

    /* Parse input file */
    xmlInitParser();
    doc = xmlReadFile(argv[1], NULL, 0);
    if (doc == NULL)
    {
        fprintf(stderr, "Could not read/parse input file.\n");
        exit(1);
    }
    xmlCleanupParser();

    /* Process input data */
    gameNode = xmlDocGetRootElement(doc);
    if (strcmp((char*)gameNode->name, "game") != 0)
    {
        fprintf(stderr, "Root element should be <game>\n");
        exit(1);
    }
    boardNode = NULL;
    for (node = gameNode->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            if (strcmp((char*)node->name, "board") == 0)
            {
                if (boardNode != NULL)
                {
                    fprintf(stderr, "Multiple Mboard> elements found.\n");
                    exit(1);
                }
                boardNode = node;
            }
            if (strcmp((char*)node->name, "analysis") == 0)
            {
                xmlUnlinkNode(node);
                xmlFreeNode(node);
            }
        }
    }
    if (boardNode == NULL)
    {
        fprintf(stderr, "No <board> element found.\n");
        exit(1);
    }
    if ( boardNode->children == NULL ||
         boardNode->children != boardNode->last ||
         boardNode->children->type != XML_TEXT_NODE )
    {
        fprintf(stderr, "<board> should contain only text.\n");
        exit(1);
    }
    descr = (char*)xmlNodeGetContent(boardNode->children);

    /* Decode board */
    if (!board_decode_full(&board, descr))
    {
        fprintf(stderr, "Cannot decode full board description: %s\n", descr);
        exit(1);
    }

    /* Add analysis to data */
    analysisNode = analyze_board(&board);
    xmlAddChild(gameNode, analysisNode);

    /* Write output document */
    xmlDocDump(stdout, doc);
    xmlFreeDoc(doc);

    return 0;
}
