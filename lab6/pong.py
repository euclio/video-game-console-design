#!/usr/bin/python

# from http://trevorappleton.blogspot.com/2014/04/writing-pong-using-python-and-pygame.html

from __future__ import print_function

import json
import socket
import sys

import pygame, sys
from pygame.locals import *

# Number of frames per second
# Change this value to speed up or slow down your game
FPS = 20

#Global Variables to be used through our program

WINDOWWIDTH = 400
WINDOWHEIGHT = 300
LINETHICKNESS = 10
PADDLESIZE = 50
PADDLEOFFSET = 20

# Set up the colours
BLACK     = (0  ,0  ,0  )
WHITE     = (255,255,255)
RED       = (255,  0,  0)
GREEN     = (0  ,255,  0)
BLUE      = (0  ,  0,255)
YELLOW    = (255,255,  0)

LOCALHOST = '127.0.0.1'
PORT = 9999

# Change the color of the game based on the number of fingers present.
FINGER_COLORS = {
    0 : BLACK,
    1 : RED,
    2 : BLUE,
    3 : GREEN,
    4 : YELLOW,
    5 : WHITE
}

#Draws the arena the game will be played in. 
def drawArena(color):
    DISPLAYSURF.fill((0,0,0))
    #Draw outline of arena
    pygame.draw.rect(DISPLAYSURF, color, ((0,0),(WINDOWWIDTH,WINDOWHEIGHT)), LINETHICKNESS*2)
    #Draw centre line
    pygame.draw.line(DISPLAYSURF, color, ((WINDOWWIDTH/2),0),((WINDOWWIDTH/2),WINDOWHEIGHT), (LINETHICKNESS/4))


#Draws the paddle
def drawPaddle(color, paddle):
    #Stops paddle moving too low
    if paddle.bottom > WINDOWHEIGHT - LINETHICKNESS:
        paddle.bottom = WINDOWHEIGHT - LINETHICKNESS
    #Stops paddle moving too high
    elif paddle.top < LINETHICKNESS:
        paddle.top = LINETHICKNESS
    #Draws paddle
    pygame.draw.rect(DISPLAYSURF, color, paddle)


#draws the ball
def drawBall(color, ball):
    pygame.draw.rect(DISPLAYSURF, color, ball)

#moves the ball returns new position
def moveBall(ball, ballDirX, ballDirY):
    ball.x += ballDirX
    ball.y += ballDirY
    return ball

#Checks for a collision with a wall, and 'bounces' ball off it.
#Returns new direction
def checkEdgeCollision(ball, ballDirX, ballDirY):
    if ball.top == (LINETHICKNESS) or ball.bottom == (WINDOWHEIGHT - LINETHICKNESS):
        ballDirY = ballDirY * -1
    if ball.left == (LINETHICKNESS) or ball.right == (WINDOWWIDTH - LINETHICKNESS):
        ballDirX = ballDirX * -1
    return ballDirX, ballDirY

#Checks is the ball has hit a paddle, and 'bounces' ball off it.     
def checkHitBall(ball, paddle1, paddle2, ballDirX):
    if ballDirX == -1 and paddle1.right == ball.left and paddle1.top < ball.top and paddle1.bottom > ball.bottom:
        return -1
    elif ballDirX == 1 and paddle2.left == ball.right and paddle2.top < ball.top and paddle2.bottom > ball.bottom:
        return -1
    else: return 1

#Checks to see if a point has been scored returns new score
def checkPointScored(paddle1, ball, score, ballDirX):
    #reset points if left wall is hit
    if ball.left == LINETHICKNESS: 
        return 0
    #1 point for hitting the ball
    elif ballDirX == -1 and paddle1.right == ball.left and paddle1.top < ball.top and paddle1.bottom > ball.bottom:
        score += 1
        return score
    #5 points for beating the other paddle
    elif ball.right == WINDOWWIDTH - LINETHICKNESS:
        score += 5
        return score
    #if no points scored, return score unchanged
    else: return score

#Artificial Intelligence of computer player 
def artificialIntelligence(ball, ballDirX, paddle2):
    #If ball is moving away from paddle, center bat
    if ballDirX == -1:
        if paddle2.centery < (WINDOWHEIGHT/2):
            paddle2.y += 1
        elif paddle2.centery > (WINDOWHEIGHT/2):
            paddle2.y -= 1
    #if ball moving towards bat, track its movement. 
    elif ballDirX == 1:
        if paddle2.centery < ball.centery:
            paddle2.y += 1
        else:
            paddle2.y -=1
    return paddle2

#Displays the current score on the screen
def displayScore(score):
    resultSurf = BASICFONT.render('Score = %s' %(score), True, WHITE)
    resultRect = resultSurf.get_rect()
    resultRect.topleft = (WINDOWWIDTH - 150, 25)
    DISPLAYSURF.blit(resultSurf, resultRect)

def displayPaused(paused):
    if not paused:
        return

    resultSurf = BASICFONT.render('PAUSED', True, WHITE)
    resultRect = resultSurf.get_rect()
    resultRect.center = (WINDOWWIDTH / 2, WINDOWHEIGHT / 2)
    DISPLAYSURF.blit(resultSurf, resultRect)

#Main function
def main():
    pygame.init()
    global DISPLAYSURF
    ##Font information
    global BASICFONT, BASICFONTSIZE
    BASICFONTSIZE = 20
    BASICFONT = pygame.font.Font('freesansbold.ttf', BASICFONTSIZE)

    FPSCLOCK = pygame.time.Clock()
    DISPLAYSURF = pygame.display.set_mode((WINDOWWIDTH,WINDOWHEIGHT)) 
    pygame.display.set_caption('Pong')

    #Initiate variable and set starting positions
    #any future changes made within rectangles
    ballX = WINDOWWIDTH/2 - LINETHICKNESS/2
    ballY = WINDOWHEIGHT/2 - LINETHICKNESS/2
    playerOnePosition = (WINDOWHEIGHT - PADDLESIZE) /2
    playerTwoPosition = (WINDOWHEIGHT - PADDLESIZE) /2
    score = 0

    #Keeps track of ball direction
    ballDirX = -1 ## -1 = left 1 = right
    ballDirY = -1 ## -1 = up 1 = down

    #Creates Rectangles for ball and paddles.
    paddle1 = pygame.Rect(PADDLEOFFSET,playerOnePosition, LINETHICKNESS,PADDLESIZE)
    paddle2 = pygame.Rect(WINDOWWIDTH - PADDLEOFFSET - LINETHICKNESS, playerTwoPosition, LINETHICKNESS,PADDLESIZE)
    ball = pygame.Rect(ballX, ballY, LINETHICKNESS, LINETHICKNESS)

    #Draws the starting position of the Arena
    drawArena(WHITE)
    drawPaddle(WHITE, paddle1)
    drawPaddle(WHITE, paddle2)
    drawBall(WHITE, ball)

    pygame.mouse.set_visible(0) # make cursor invisible

    sock = socket.socket()
    try:
        sock.connect((LOCALHOST, PORT))
    except Exception as e:
        print("ERROR: Could not connect to {}:{}".format(LOCALHOST, PORT),
                file=sys.stderr)
        sys.exit(1)

    # Keeps track of any yet-unused data from the socket
    last_data = ""
    paused = False
    color = WHITE


    while True: #main game loop
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()

        # Read from the socket to determine how much we should move.
        data = last_data + sock.recv(4096)
        hand_data = None

        # We only care about the most recent data, so if we get multiple
        # JSON objects from the socket then we only use the most recent one.
        if len(data) > 3:
            packet_size = int(data[:3])
            while len(data[3:]) > packet_size:
                raw_json = data[3:3+packet_size]
                hand_data = json.loads(raw_json)
                data = data[3 + packet_size:]

        last_data = data

        if hand_data:
            # Move the paddle based on hand position
            paddle1.y = hand_data['y']

            num_fingers = hand_data['fingers']
            color = FINGER_COLORS[num_fingers]

            # Pause if there is no hand present
            paused = not hand_data['isHand'] or num_fingers == 0

        drawArena(color)
        drawPaddle(color, paddle1)
        drawPaddle(color, paddle2)
        drawBall(color, ball)

        if not paused:
            ball = moveBall(ball, ballDirX, ballDirY)
            ballDirX, ballDirY = checkEdgeCollision(ball, ballDirX, ballDirY)
            score = checkPointScored(paddle1, ball, score, ballDirX)
            ballDirX = ballDirX * checkHitBall(ball, paddle1, paddle2, ballDirX)
            paddle2 = artificialIntelligence (ball, ballDirX, paddle2)

        displayScore(score)
        displayPaused(paused)

        pygame.display.update()
        FPSCLOCK.tick(FPS)

if __name__=='__main__':
    main()
