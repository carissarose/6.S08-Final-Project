import _mysql
import cgi
import datetime
from random import shuffle, randrange

exec(open("/var/www/html/student_code/LIBS/s08libs.py").read())
print("Content-type:text/html\r\n\r\n")

method_type = get_method_type()
form = cgi.FieldStorage() #get specified parameters!

#function to randomly generate maze with embedded chests
def make_maze(width, height):
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
    counter = 0
    #randomly place chests as 'o's into the maze where valid
    while counter < 20:
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

    #insert XXs in between each embedded list in the hor list as markers
    hor_str = ''
    for i in range(len(hor)):
        hor_str += ''.join(hor[i])
        hor_str += 'XX'
    #insert XXs in between each embedded list in the ver list as markers
    ver_str = ''
    for i in range(len(ver)):
        ver_str += ''.join(ver[i])
        ver_str += 'XX'

    #create maze with horizontal and vertical coordinate information
    maze = zip(hor, ver)
    mazeString = ""
    for (a, b) in maze:
        mazeString += ''.join(a + ['XX'] + b + ['XX'])
                          
    return mazeString

if method_type == 'POST':
    print("<html>")
    
    #define variable user for the use in query creation below
    if 'kerberos' in form and 'level' in form and 'action' in form:
        kerberos = form['kerberos'].value
        level = form['level'].value
        action = form['action'].value

        #connect to database:
        cnx = _mysql.connect(user='akabay_gadsoncr', passwd='85qNdQQ3', db='akabay_gadsoncr')
        rep = ''
        
        #delete all mazes in mazes database when they're garbage. Helpful for debugging
        if action == 'deletemazes':
            query = ("DELETE FROM mazes")
            cnx.query(query)
            cnx.commit()
        #delete information in scoreboard database when they're garbage. Helpful for debugging
        elif action == 'deleteinfo':
            query = ("DELETE FROM scoreboard")
            cnx.query(query)
            cnx.commit()

        #randomly generate a maze and store it in the mazes database
        elif action == 'createMaze':
            rep = make_maze(12, 8)
            #create a mySQL query and commit to database relevant information for logging maze
            query = ("INSERT INTO mazes (rep) VALUES ('%s')" %(rep)) #logs the maze
            cnx.query(query)
            cnx.commit()

        #store the kerberos, level, and times to database after game is completed
        elif action == 'storeTimes' and 'starttime' in form and 'endtime' in form:
            starttime = form['starttime'].value
            endtime = form['endtime'].value
            #create a mySQL query and commit to database relevant information for logging times
            query = ("INSERT INTO scoreboard (kerberos, level, starttime, endtime) VALUES ('%s','%s','%s','%s')" %(kerberos,level,starttime,endtime)) #logs the times
            cnx.query(query)
            cnx.commit()

            #Do a GET request within this post request so that on the Arduino side, one POST request will return back the best times
            #Create query to database to get top time plays meeting certain criteria (to a user and associated with the site)
            query = ("SELECT *, (endtime - starttime) as timediff FROM scoreboard WHERE level='%s' ORDER BY timediff ASC LIMIT 5" %(level))
            cnx.query(query)
            result = cnx.store_result()
            rows = result.fetch_row(maxrows=0,how=0)

            information = []
            #generate list of information (and take care of unicode issues so everything is a Python String)
            for row in rows:
                information.append([e.decode('utf-8') if type(e) is bytes else e for e in row])

            for items in information:
                timediff = float(items[7])
                timediff = round(timediff, 2)
                print(items[1] + ': ' + str(timediff))

        cnx.close()
        
    print("</html>")

elif method_type == 'GET':
    #you need to define the variable user for use in the query creation below
    if 'level' in form and 'action' in form:
        level = form['level'].value  
        action = form['action'].value

        #connect to database
        cnx = _mysql.connect(user='akabay_gadsoncr', passwd='85qNdQQ3', db='akabay_gadsoncr')
        
        print("<html>")

        #show all mazes in the database for debugging purposes
        if action == 'showallmazes':
            query = ("SELECT * FROM mazes")
            cnx.query(query)
            result = cnx.store_result()
            rows = result.fetch_row(maxrows=0,how=0)
            information = []
            #generate list of information (and take care of unicode issues so everything is a Python String)
            for row in rows:
                information.append([e.decode('utf-8') if type(e) is bytes else e for e in row])
            print(information)

        #get a maze from the database. Note there are two different methods here.
        elif action == 'getMaze':
            #Method 1: do this GET request to simply return a randomly generated maze without storing it in the database first
            '''
            rep = make_maze(12,8)
            print(rep)
            '''
            #Method 2: extract a random maze that already exists in the mazes database
            #Create query to database to get maze
            query = ("SELECT rep FROM mazes")
            cnx.query(query)
            result = cnx.store_result()
            rows = result.fetch_row(maxrows=0,how=0)
            rows = list(rows)
            shuffle(rows)
            if len(rows) > 0:
                strMaze = rows[3][0].decode('utf-8')
                print(strMaze)

        #show all information in the scoreboard database for debugging purposes
        elif action == 'showall':
            query = ("SELECT * FROM scoreboard")
            cnx.query(query)
            result = cnx.store_result()
            rows = result.fetch_row(maxrows=0,how=0)
            information = []
            #generate list of information (and take care of unicode issues so everything is a Python String)
            for row in rows:
                information.append([e.decode('utf-8') if type(e) is bytes else e for e in row])
            print(information)

        #get the five lowest times from the database. This can also be done in the POST request with action = 'storeTimes'
        elif action == 'getTimes':
            #Create query to database to get top time plays meeting certain criteria (to a user and associated with the site)
            query = ("SELECT *, (endtime - starttime) as timediff FROM scoreboard WHERE level='%s' ORDER BY timediff ASC LIMIT 5" %(level))
            cnx.query(query)
            result = cnx.store_result()
            rows = result.fetch_row(maxrows=0,how=0)

            information = []
            #generate list of information (and take care of unicode issues so everything is a Python String)
            for row in rows:
                information.append([e.decode('utf-8') if type(e) is bytes else e for e in row])

            for items in information:
                timediff = float(items[7])
                timediff = round(timediff, 2)
                print(items[1] + ': ' + str(timediff) + '\n')

        cnx.close()
        print("</html>")
