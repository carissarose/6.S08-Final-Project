from random import shuffle, randrange

def make_maze(width = 16, height = 10):
    #records if cell has been visited
    #creates 8 lists of 12 0s and one 1, and 1 list of 13 1s
    vis = []
    for i in range(height):
        vis += [[0] * width + [1]]
    vis += [[1] * (width + 1)]
    #creates 8 lists of vertical lines, and 1 empty list
    ver = []
    for j in range(height):
        ver += [["|  "] * width + ['|']]
    ver += [[]]
    #print(ver)
    #creates 9 lists of +s
    hor = []
    for k in range(height + 1):
        hor += [["+--"] * width + ['+']]
    #print(hor)
    

    def walk(x, y):
        #check that this cell has been visited
        vis[y][x] = 1
        #create diamond out of the points
        diamond = [(x - 1, y), (x, y + 1), (x + 1, y), (x, y - 1)]
        #shuffle the coordinates
        shuffle(diamond)
        
        for (a, b) in diamond:
            #if this cell has been visited, continue
            if vis[b][a]:
                continue
            #otherwise, if a == x, 
            elif a == x:
                hor[max(y, b)][x] = "+  "
            elif b == y:
                ver[y][max(x, a)] = "   "               
            walk(a, b)
            
    #call walk function, passing two random, valid x,y values
    walk(randrange(width), randrange(height))

    maze_list = []
    counter1 = 0
    counter2 = 0
    #create maze into a list of lists
    for i in range(18):
        if i % 2 == 0:
            maze_list += [hor[counter1]]
            counter1 += 1
        if i % 2 == 1:
            maze_list += [ver[counter2]]
            counter2 += 1

    temp_list = []

    #randomly place chests as 'o's into the maze where valid
    counter = 0
    while counter < 10:
        y_pos = randrange(17)
        x_pos = randrange(12)
        tempstring = ''
        temp_list = []
        tempstring = maze_list[y_pos][x_pos]
        for n in range(len(tempstring)):
            temp_list += tempstring[n]
        if temp_list[0] == '|':
            if temp_list[1] == ' ':
                temp_list[1] = 'o'
                tempstring = "".join(temp_list)
                maze_list[y_pos][x_pos] = tempstring
                counter += 1


    hor_str = ''
    for i in range(len(hor)):
        hor_str += ''.join(hor[i])
        hor_str += 'XX'

    ver_str = ''
    for i in range(len(ver)):
        ver_str += ''.join(ver[i])
        ver_str += 'XX'

    #create maze with horizontal and vertical coordinate information
    maze = zip(hor, ver)
    mazeString = ""
    for (a, b) in maze:
        mazeString += ''.join(a + ['XX'] + b + ['XX'])
                          
    print(mazeString)
        


make_maze()

#rosettacode

