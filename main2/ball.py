X, Y = 40, 40
R = 40

M = [0]*6561

f = open('./circle.txt', 'w')
for y in range(81):
    for x in range(81):
        if (X-x)**2 + (Y-y)**2 <= 40**2:
            #print((X-x)**2 + (Y-y)**2, 40**2)
            M[y*81+x] = 1
    print(M[y*81:y*81+81])
    f.write(str(M[y*81:y*81+81]) + '\r\n')
