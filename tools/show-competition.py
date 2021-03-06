#!/usr/bin/env python
import os, sys
from xml.dom import minidom
from xml import xpath

def cmp_filenames(a, b):
    x = a.split('-')
    y = b.split('-')
    for i in range(min(len(x), len(y))):
        if len(x[i]) != len(y[i]): return len(x[i]) - len(y[i])
        if x[i] < y[i]: return -1
        if x[i] > y[i]: return +1
    if len(x) != len(y): return len(x) - len(y)
    return 0

def show(id, contestants, swiss_rounds, finalists):
    game = 0

    player_score = {}
    player_won = {}
    player_lost = {}
    for filename in sorted(os.listdir('.'), cmp_filenames):
        if not filename.startswith('%d-' % id): continue
        game += 1
        if game <= swiss_rounds*contestants: continue

        doc = minidom.parse(filename)
        name1 = xpath.Evaluate('/game/player1/name/text()', doc)[0].data
        name2 = xpath.Evaluate('/game/player2/name/text()', doc)[0].data
        result = xpath.Evaluate('/game/result', doc)[0]
        score1 = int(result.attributes['score1'].value)
        score2 = int(result.attributes['score2'].value)
        for name,score in [ (name1,score1), (name2, score2) ]:
            if name not in player_score:
                player_score[name] = player_won[name] = player_lost[name] = 0
            if score == max(score1, score2): player_won[name]  += 1
            if score == min(score1, score2): player_lost[name] += 1
            player_score[name] += score
    ranking = [(score,name) for name,score in player_score.iteritems()]
    ranking.sort(reverse = True)
    for score,name in ranking:
        print score, name, player_won[name], player_lost[name]


if __name__ == '__main__':
    if len(sys.argv) <> 2:
        print 'Usage: show-competition <id>'
        sys.exit(0)
    arg_id = int(sys.argv[1])
    doc = minidom.parse("competitions.xml")
    for competition in xpath.Evaluate('//competition', doc):
        id, contestants, swiss_rounds, finalists = [
            int(competition.attributes[key].value) for key in
            ('id', 'contestants', 'swiss-rounds', 'finalists') ]
        if id == arg_id:
            show(id, contestants, swiss_rounds, finalists)

