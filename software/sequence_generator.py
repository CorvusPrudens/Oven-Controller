# val = 65000
# tempnum = [0, 0, 0, 0, 0, 0]
# remainder = val
# quotient = 0
# divisor = 0
# place = 0
#
# for i in range(5, 0, -1):
#     divisor = pow(10, i);
#     tempnum[5 - i] = ord('0') + remainder//divisor;
#     if ((place & 64) == 0 and tempnum[5 - i] != ord('0')):
#         place = 64 | (5 - i);
#     remainder = remainder % divisor
#
# tempnum[5] = ord('0') + remainder
# place &= 0b10111111
#
# for i in range(0, 6 - place):
#     print(chr(tempnum[i + place]))
#
# print(place)

# def note(input):
#     return int(440.0*pow(2, (input - 69.0)/12.0))
#
# for i in range(4):
#     for j in range(12):
#         print(note(12*5 + (i*12 + j)), end=', ')
#     print()



def toNotes(table, tempoIdx, articulation):
    artics = {
        'legato': 0,
        'detache': 1,
    }
    octOff = 0
    warning = False
    tooLong = False
    tooidx = 0
    booboo = 0
    notes = {
        'c': 0 + octOff*12,
        'd': 2 + octOff*12,
        'e': 4 + octOff*12,
        'f': 5 + octOff*12,
        'g': 7 + octOff*12,
        'a': 9 + octOff*12,
        'b': 11 + octOff*12,
        'r': -1,
    }
    mods = {
        'n': 0,
        'f': -1,
        'b': -1,
        's': 1
    }
    strings = []
    for i in range(int(len(table)/2)):
        toks = table[i*2].split(" ")
        #print(toks)
        note = 0
        octmod = 0
        oct = 0
        if len(toks) > 1:
            if len(toks[0]) > 1:
                note = notes[toks[0][0]] + mods[toks[0][1]]
            else:
                note = notes[toks[0]]
            #print(note)
            oct = int(toks[1]) - 4
            #print(oct, toks[1], note)

        tok = table[i*2].split(" ")
        if tok[0] == 'r':
            note = 255
            oct = 0

        if note + oct*12 < octOff*12 or note + oct*12 > octOff*12 + 4*12 and tok[0] != 'r':
            warning = True
            booboo = i

        if table[i*2 + 1]*8 > 255:
            tooLong = True
            tooidx = i

        strings.append("{}, {},".format(note + oct*12, table[i*2 + 1]))

    print(artics[articulation], end=', \n')
    print(tempoIdx, end=', \n')
    print(len(strings), end=',\n')
    for string in strings:
        print(string)

    if len(strings)*2 + 3 > 255:
        print("Warning: sequence is too long! ({} bytes!)".format(len(strings)*2 + 3))
    if warning:
        print("Warning: \"{}, {}\" (element {}) is outside range!".format(table[booboo*2], table[booboo*2 + 1], booboo + 1))
    if tooLong:
        print("Warning: the duration \"{}, {}\" (element {}) is outside range!".format(table[tooidx*2], table[tooidx*2 + 1], tooidx + 1))


mario = [
    #intro
    'e 5', 2,
    'e 5', 2,
    'r', 2,
    'e 5', 2,
    'r', 2,
    'c 5', 2,
    'e 5', 2,
    'r 2', 2,

    'g 5', 2,
    'r', 6,
    'g 4', 2,
    'r', 6,

    #phrase 1
    'c 5', 4,
    'r', 2,
    'g 4', 4,
    'r', 2,
    'e 4', 4,

    'r', 2,
    'a 4', 2,
    'r', 2,
    'b 4', 4,
    'bb 4', 2,
    'a 4', 4,

    'g 4', 3,
    'e 5', 3,
    'g 5', 2,
    'a 5', 4,
    'f 5', 2,
    'g 5', 2,

    'r', 2,
    'e 5', 4,
    'c 5', 2,
    'd 5', 2,
    'b 4', 4,
    'r', 2,

    #phrase 1 repeat 2
    'c 5', 4,
    'r', 2,
    'g 4', 4,
    'r', 2,
    'e 4', 4,

    'r', 2,
    'a 4', 2,
    'r', 2,
    'b 4', 4,
    'bb 4', 2,
    'a 4', 4,

    'g 4', 3,
    'e 5', 3,
    'g 5', 2,
    'a 5', 4,
    'f 5', 2,
    'g 5', 2,

    'r', 2,
    'e 5', 4,
    'c 5', 2,
    'd 5', 2,
    'b 4', 4,
    'r', 2,

    # Phrase 2
    'r', 4,
    'g 5', 2,
    'fs 5', 2,
    'f 5', 2,
    'ds 5', 4,
    'e 5', 2,

    'r', 2,
    'gs 4', 2,
    'a 4', 2,
    'c 5', 2,
    'r', 2,
    'a 4', 2,
    'c 5', 2,
    'd 5', 2,

    'r', 4,
    'g 5', 2,
    'fs 5', 2,
    'f 5', 2,
    'ds 5', 4,
    'e 5', 2,

    'r', 2,
    'c 6', 2,
    'r', 2,
    'c 6', 2,
    'c 6', 2,
    'r', 6,

    'r', 4,
    'g 5', 2,
    'fs 5', 2,
    'f 5', 2,
    'ds 5', 4,
    'e 5', 2,

    'r', 2,
    'gs 4', 2,
    'a 4', 2,
    'c 5', 2,
    'r', 2,
    'a 4', 2,
    'c 5', 2,
    'd 5', 2,

    'r', 4,
    'eb 5', 4,
    'r', 2,
    'd 5', 4,
    'r', 2,
    'c 5', 4,
    'r', 12,
]

zelda = [
    # Phrase 1
    'a 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'g 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'fs 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'g 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,



    'g 5', 2,
    'c 5', 2,
    'a 4', 2,
    'f 4', 2,

    'f 5', 2,
    'c 5', 2,
    'a 4', 2,
    'f 4', 2,

    'e 5', 2,
    'c 5', 2,
    'a 4', 2,
    'f 4', 2,

    'f 5', 2,
    'c 5', 2,
    'a 4', 2,
    'f 4', 2,


    'f 5', 2,
    'bb 4', 2,
    'g 4', 2,
    'e 4', 2,

    'e 5', 2,
    'bb 4', 2,
    'g 4', 2,
    'e 4', 2,

    'eb 5', 2,
    'bb 4', 2,
    'g 4', 2,
    'e 4', 2,

    'e 5', 2,
    'bb 4', 2,
    'g 4', 2,
    'e 4', 2,


    'e 5', 2,
    'a 4', 2,
    'f 4', 2,
    'd 4', 2,

    'd 5', 2,
    'a 4', 2,
    'f 4', 2,
    'd 4', 2,

    'cs 5', 2,
    'a 4', 2,
    'f 4', 2,
    'd 4', 2,

    'd 5', 2,
    'a 4', 2,
    'f 4', 2,
    'd 4', 2,

    # Phrase 2
    'a 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'g 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'fs 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'g 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,


    'bb 5', 2,
    'eb 5', 2,
    'c 5', 2,
    'a 4', 2,

    'a 5', 2,
    'eb 5', 2,
    'c 5', 2,
    'a 4', 2,

    'gs 5', 2,
    'eb 5', 2,
    'c 5', 2,
    'a 4', 2,

    'a 5', 2,
    'eb 5', 2,
    'c 5', 2,
    'a 4', 2,



    'c 6', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'bb 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'a 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,

    'bb 5', 2,
    'd 5', 2,
    'bb 4', 2,
    'g 4', 2,



    'a 5', 2,
    'bb 4', 2,
    'g 4', 2,
    'e 4', 2,

    'g 5', 2,
    'bb 4', 2,
    'g 4', 2,
    'e 4', 2,

    # 'f 5', 2,
    # 'bb 4', 2,
    # 'g 4', 2,
    # 'e 4', 2,
    #
    # 'e 5', 2,
    # 'bb 4', 2,
    # 'g 4', 2,
    # 'e 4', 2,

]

ff7 = [
    'd 5', 4,
    'b 4', 2,
    'g 4', 2,
    'e 4', 2
]

ff7vict = [
    'c 5', 2,
    'c 5', 2,
    'c 5', 2,
    'c 5', 6,
    'ab 4', 6,
    'bb 4', 6,
    'c 5', 4,
    'bb 4', 2,
    'c 5', 12,
    'r', 6,
]

ff7myst = [
    'c 6', 4,
    'g 5', 4,
    'b 5', 8,
    'r', 16,
    'bb 5', 4,
    'f 5', 4,
    'a 5', 4,
    'g 5', 8,
    'r', 16,
]

mariovict = [
    'g 4', 2,
    'c 5', 2,
    'e 5', 2,
    'g 5', 2,
    'c 6', 2,
    'e 6', 2,
    'g 6', 6,
    'e 6', 6,

    'ab 4', 2,
    'c 5', 2,
    'eb 5', 2,
    'ab 5', 2,
    'c 6', 2,
    'eb 6', 2,
    'ab 6', 6,
    'eb 6', 6,

    'bb 4', 2,
    'd 5', 2,
    'f 5', 2,
    'bb 5', 2,
    'd 6', 2,
    'f 6', 2,
    'bb 6', 6,
    'bb 6', 2,
    'bb 6', 2,
    'bb 6', 2,

    'c 7', 14,
    'r', 10,
]

# toNotes(mariovict, 3, 'detache')
# toNotes(ff7vict, 2, 'detache')
# toNotes(ff7myst, 1, 'legato')
toNotes(mario, 2, 'detache')
