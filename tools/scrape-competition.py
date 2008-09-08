#!/usr/bin/env python

import os
import re
import sys
from urllib import urlopen
from xml.dom.ext import Print
import xml.dom

def parsePoint(pt):
    return ("ABCDEFGHIJH".index(pt[0]), "abcdefgijh".index(pt[1]))

def pillars_to_permutation(points):
    'Converts a list of points (as strings of the form Xx) to a permutation'

    cols = range(len(points))
    res = 0
    for n in range(len(cols)):
        if points[n][0] <> n: raise "unordered points"
        i = cols.index(points[n][1])
        res = res*len(cols) + i
        cols[i:i+1] = []
    return res + 1

def extract_game(page):

    board = [ [ 0 for c in range(10) ] for r in range(10) ]

    # Expected keys:
    #   pillars
    #   red
    #   blue
    #   moves
    #   result
    #   resultDesc
    params = {}
    for key,value in re.findall('<param name=\\\\"(.*?)\\\\" value=\\\\"(.*?)\\\\', page):
        params[key] = value

    doc = xml.dom.getDOMImplementation().createDocument(None, "game", None)
    gameElem = doc.firstChild

    # Add player 1 name
    name1Elem = doc.createElement('name')
    name1Elem.appendChild(doc.createTextNode(params['red']))
    player1Elem = doc.createElement('player1')
    player1Elem.appendChild(name1Elem)
    gameElem.appendChild(player1Elem)

    # Add player 2 name
    name2Elem = doc.createElement('name')
    name2Elem.appendChild(doc.createTextNode(params['blue']))
    player2Elem = doc.createElement('player2')
    player2Elem.appendChild(name2Elem)
    gameElem.appendChild(player2Elem)

    # Add pillars
    pillarsElem = doc.createElement('pillars')
    pillars = params['pillars'].split(',')
    pillars.sort()
    perm = pillars_to_permutation(map(parsePoint, pillars))
    pillarsElem.setAttribute('perm', str(perm))
    for point in pillars:
        pointElem = doc.createElement('point')
        pointElem.appendChild(doc.createTextNode(point))
        pillarsElem.appendChild(pointElem)
        (r,c) = parsePoint(point)
        board[r][c] = -1
    gameElem.appendChild(pillarsElem)

    # Add moves
    moves = params['moves'].split(',')
    movesElem = doc.createElement('moves')
    movesElem.setAttribute("count", str(len(moves)))
    for i in range(len(moves)):
        rect = moves[i]
        rectElem = doc.createElement('rect')
        rectElem.appendChild(doc.createTextNode(rect))
        movesElem.appendChild(rectElem)
        if rect[0] == '!': rect = rect[1:]
        ((r1,c1), (r2,c2)) = (parsePoint(rect[0:2]), parsePoint(rect[2:4]))
        for r in range(r1, r2 + 1):
            for c in range(c1, c2 + 1):
                board[r][c] = i + 1
    gameElem.appendChild(movesElem)

    board = ''.join( '%02X'%(board[r][c]&255) for r in range(10) for c in range(10) )
    boardElem = doc.createElement('board')
    boardElem.appendChild(doc.createTextNode(board))
    gameElem.appendChild(boardElem)

    score1, score2 = params['result'].split('-')
    winner = 1 + (int(score1) < int(score2))
    resultElem = doc.createElement('result')
    resultElem.setAttribute('score1', score1)
    resultElem.setAttribute('score2', score2)
    resultElem.setAttribute('winner', str(winner))
    resultElem.appendChild(doc.createTextNode(params['resultDesc']))
    gameElem.appendChild(resultElem)

    return doc


base_url = 'http://www.codecup.nl/'

if len(sys.argv) <> 2:
    print 'usage: %s <comp>' % sys.argv[0]
else:
    comp = sys.argv[1]

comp_page = urlopen(base_url + 'competition.php?comp=' + comp).read()
for cr in re.findall('competitionround.php[?]cr=(\d+)', comp_page):
    round_page = urlopen(base_url + 'competitionround.php?cr=' + cr).read()
    for ga in re.findall('showgame.php[?]ga=(\d+)', round_page):
        game_page = urlopen(base_url + 'showgame.php?ga=' + ga).read()
        game = extract_game(game_page)
        path = '-'.join([comp, cr, ga, 'game.xml'])
        Print(game, file(path, 'wt'))
