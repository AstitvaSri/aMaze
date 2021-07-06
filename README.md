## aMaze  (Quick Guide)

## Controls

Movement : Arrow Keys (↑, ↓ , ←, →)
Camera Zoom-in : X
Camera Zoom-out : Z

## On-screen Indicators

Health bar (Cyan), Power bar (Yellow), Power Cells (Yellow rotating boxes)

## Power-Ups

Speed boost(Green) : Increases the speed for 10 seconds
Health boost(Cyan) : Increases the health
Break Walls(Blue) : Grants ability to break maze walls for 10 seconds
Power coins(Yellow) : Collect 20 coins to recharge the light.
Battery (Grey) : Recharges the light instantly.

## Obstacles

Avoid collision with maze walls, it will result in ​ **speed drop​** for 10 seconds as well as
**heath drop.**
Other obstacles are represented by ​ **“wobbling” floors​**. These floors may contain
any arbitrary obstacle among the following:

1. Rotates Maze walls continuously for 15 seconds, thereby increasing the
    chances of collision.
2. Scale up the cube size, thereby increasing the chances of collision.
3. Health drop.
4. All power coins drop.

## Trade-Off

Some power-ups might be on top of wobbling floors. You have to be careful while making
this trade-off because the floor contains any random obstacle. For example, if you are
running out of health and power and you see a battery over a wobbling floor. If you take
that battery there’s a chance that you might end up dying. Or might get your power
recharged over the cost of a few coins. Use this trade-off for your benefit.

![alt text](https://github.com/AstitvaSri/aMaze/blob/master/img/amaze1.png)
![alt text](https://github.com/AstitvaSri/aMaze/blob/master/img/amaze2.png)


### --------------------- INSTRUCTIONS TO COMPILE AND RUN ------------------------

#### (For UBUNTU)

1. Setting up GLFW:

Download GLFW source code from here : https://github.com/glfw/glfw (If you don't already have it)

Unzip, navigate to extracted folder and run the following commands in terminal:

mkdir build

cd build

cmake .. (install required libraries if error occurs)

make

sudo make install

2. Copy the folders present in "include files" directory to your "/user/local/include/".

3. Make sure you have OpenGL and its dependencies installed. (Most of them come pre-installed with linux)

4. Go to "aMaze" directory and compile "aMaze.cpp" from terminal : 

g++ aMaze.cpp  glad.c -o aMaze -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lstdc++fs

5. ./aMaze




