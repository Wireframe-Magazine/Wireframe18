# 3D Monster Maze
import pgzrun
WIDTH = 600
HEIGHT = 600
maze = [[1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 0, 1, 0, 0, 0, 1],
        [1, 0, 0, 0, 0, 1, 0, 1, 0, 1],
        [1, 1, 0, 1, 0, 0, 0, 0, 0, 1],
        [1, 1, 0, 1, 0, 1, 0, 1, 0, 1],
        [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
        [1, 1, 0, 1, 0, 1, 1, 0, 1, 1],
        [1, 1, 0, 1, 0, 1, 1, 0, 1, 1],
        [1, 1, 0, 0, 0, 0, 0, 0, 0, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]]
playerX = 1
playerY = 4
playerDir = 2
dirX = [-1,0,1,0]
dirY = [0,1,0,-1]

def draw():
    screen.fill((255, 255, 255))
    screen.blit("back", (0, 0))
    drawMaze()
    
def update():
    pass

def on_key_down(key):
    global playerX, playerY, playerDir
    if key.name == "UP":
        newX = playerX + dirX[playerDir]
        newY = playerY + dirY[playerDir]
        if maze[newX][newY] == 0:
            playerX = newX
            playerY = newY
    if key.name == "DOWN":
        newX = playerX - dirX[playerDir]
        newY = playerY - dirY[playerDir]
        if maze[newX][newY] == 0:
            playerX = newX
            playerY = newY
    if key.name == "LEFT":
        playerDir -=1
        if playerDir < 0: playerDir = 3
    if key.name == "RIGHT":
        playerDir +=1
        if playerDir > 3: playerDir = 0

def drawMaze():
    dm = 1
    if(playerDir == 1 or playerDir == 3): dm=-1
    for l in range(4,-1,-1):
        x = playerX + (l*dirX[playerDir])
        y = playerY + (l*dirY[playerDir])
        if(x>=0 and x<10 and y>=0 and y<10):
            xl = x + (dirY[playerDir] * dm)
            yl = y + (dirX[playerDir] * dm)
            if(maze[xl][yl] == 1):
                screen.blit("left"+str(l), (0, 0))
            xr = x - (dirY[playerDir] * dm)
            yr = y - (dirX[playerDir] * dm)
            if(maze[xr][yr] == 1):
                screen.blit("right"+str(l), (0, 0))
            if(maze[x][y] == 1):
                screen.blit("mid"+str(l), (0, 0))
                
pgzrun.go()
