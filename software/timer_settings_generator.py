import math as m

def note(input):
    return 440.0*pow(2, (input - 69.0)/12.0)

strings = []

def printTimer2Settings():
    clk = 20000000.0
    vals = [
        1,
        8,
        32,
        64,
        128,
        256,
        1024,
    ]
    pres = []
    ocrs = []
    for i in range(4*12):
        n = note(i + 60)
        print(n)
        pre = 8

        closest = 100000
        closestH = 0
        closestL = 0
        for i in range(len(vals)):
            for j in range(1, 256):
                f = clk/((j + 1)*vals[i]*2)
                dist = n - f
                if abs(dist) < closest:
                    closest = abs(dist)
                    closestH = i
                    closestL = j

        pres.append(closestH)
        ocrs.append(closestL)
        print(closestH + 1, ",", closestL)
        res = clk/((closestL + 1)*vals[closestH]*2);
        print(res);
        print(1200*(m.log(n/res)/m.log(2)))
        print()


    for i in range(4):
        for j in range(12):
            print(pres[j + i*12] + 1, end=', ')
        print()

    print()
    for i in range(4):
        for j in range(12):
            print(ocrs[j + i*12], end=', ')
        print()

def printTimer0Settings():
    vals = [
        1,
        8,
        32,
        64,
        128,
        256,
        1024,
    ]

    tempi = [
        50,
        70,
        90,
        110,
        130,
        150,
        170,
        190,
    ]
    pres = []
    ocrs = []
    for i in range(len(tempi)):
        n = 1/(60.0/tempi[i]/32.0)
        print(n)

        closest = 100000
        closestH = 0
        closestL = 0
        for i in range(len(vals)):
            for j in range(1, 256):
                f = 16000000/((j + 1)*vals[i]*2)
                dist = n - f
                if abs(dist) < closest:
                    closest = abs(dist)
                    closestH = i
                    closestL = j

        pres.append(closestH)
        ocrs.append(closestL)
        print(closestH + 1, ",", closestL)
        res = 16000000.0/((closestL + 1)*vals[closestH]*2);
        print(res);
        print(1200*(m.log(n/res)/m.log(2)))
        print()

    for i in range(len(pres)):
        print(pres[i] + 1, end=', ')
        # print()

    print()
    for i in range(len(ocrs)):
        print(ocrs[i], end=', ')
        # print()

printTimer2Settings()
