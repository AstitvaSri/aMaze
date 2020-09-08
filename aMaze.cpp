#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <deque>

#include "shader_s.h"

#define V 100 //4*DIM*DIM

#include "djkstra.h"

using namespace std;
#include <bits/stdc++.h> 
#include <unistd.h>
#include <iostream>
#include <map>
#include <time.h>
#include <thread>
#include <experimental/filesystem>

#define DIM 5


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
bool collision_check(GLFWwindow *window,char direction,float pos_x, float pos_z);
void setPowerUpLocations();
void setObstaclesLocations();
void checkPowerUps();
void checkObstacles();

deque <float> chasing_chain;


// settings
unsigned int SCR_WIDTH = 1080;
unsigned int SCR_HEIGHT = 720;

//maze_info
//position range is -DIM to DIM
int maze_dim = DIM; //It will make a 2*DIM x 2*DIM maze (Not DIM x DIM)
int place_cube[2*DIM][2*DIM]; //don't forget to put maze_dimension here
int place_pow[2*DIM][2*DIM];  //here too
int place_obs[2*DIM][2*DIM];  //and here again
int graph[4*DIM*DIM][4*DIM*DIM];


int LOSE = 0;
int WIN = 0;
int display_indicators = 1;

int disable_movement = 0;
int powerups_quantity_factor = 150; //decrease this values for more quantity
int obstacles_quantity_factor = 300; //decrease this values for more quantity

// light properties
int powerup = 0;
float constant_light=1.0f;
float linear_light = 1.0f;
float quadratic_light = 1.0f;

//camera
float y_pos_moving = 4.0f;
float max_zoom_out = 6.f;
float max_zoom_in = 3.0f;
float cam_speed = 0.02f;
glm::vec3 CameraPos = glm::vec3(float(maze_dim),y_pos_moving,float(maze_dim-1));
glm::vec3 CameraTarget = glm::vec3(float(maze_dim),0.0f,float(maze_dim-1));
glm::vec3 CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
glm::mat4 view = glm::lookAt(CameraPos,CameraTarget, CameraUp);

//enemy
int enemy_exist = 1;
glm::vec2 enemy_pos = glm::vec2(float(-maze_dim),float(-maze_dim));
glm::vec2 target_pos = glm::vec2(enemy_pos.x,enemy_pos.y); //initially right
int retarget = 1;
int enemy_direction = 1; //1-Right, 2-Left, 3-Down, 4-Up
float enemy_speed = 0.01f;
int* parent;
glm::mat4 model_enemy;


//light
glm::vec3 diffuse_light = glm::vec3(1.0,1.0,1.0);
glm::vec3 specular_light = glm::vec3(1.0,1.0,1.0);
glm::vec4 player_color = glm::vec4(1.0,1.0,1.0,1.0);

//previous_state of light
float light_decay = 0.0005f;
float diffuse_decay = 0.0008f;
glm::vec3 old_diffuse_light = glm::vec3(0.9,0.9,0.9);
glm::vec3 old_specular_light = glm::vec3(0.0,1.0,0.0);
glm::vec4 old_player_color = glm::vec4(0.0,1.0,0.0,1.0);

//timer
time_t collision_timer = time(NULL);
time_t speedup_timer = time(NULL);
time_t remove_wall_timer = time(NULL);
time_t maze_rotation_timer = time(NULL);
time_t grow_timer = 0;
time_t decay_power_timer = time(NULL);

int render_everything = 1;

//collision parameters
bool ongoing_collision = 0;
float collision_thresh = 0.18f;// 0.18 default, 0.038 during maze_rotation and 0.048 for growing player size at scale 1.3
float grow_old_collision_thresh = collision_thresh;
float maze_old_collision_thresh = collision_thresh;
float scale_rate = 10.0f;
float antenna_rotation = 1000.0f;

//obstacles
int maze_rotation = 0;
int grow = 0;
float player_scale = 1.0f;

//powerups
glm::vec3 coin_color = glm::vec3(1.0,0.5,0.0);
glm::vec3 health_color = glm::vec3(0.0,1.0,1.0);
glm::vec3 speedup_color = glm::vec3(0.0,1.0,0.0);
bool speedup = 0;
glm::vec3 remove_wall_color = glm::vec3(0.0,0.0,1.0);
bool remove_wall = 0;
glm::vec3 battery_color = glm::vec3(0.2,0.2,0.2);
int max_health = 290;
int health = max_health;
int max_power = 290;
int power_bar_value = max_power;
int max_coins = 20;
int coins = 0;



//indicators
float h_bar_z = 1.0f;
float h_bar_x = (float(SCR_WIDTH)/float(SCR_HEIGHT))*h_bar_z;
float p_bar_z = 0.945f;
float p_bar_x = (float(SCR_WIDTH)/float(SCR_HEIGHT))*h_bar_z; //don't write p_bar_z, h_bar_z is correct
float c_bar_z = 0.89;
float c_bar_x = (float(SCR_WIDTH)/float(SCR_HEIGHT))*h_bar_z; //don't write p_bar_z, h_bar_z is correct



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "aMaze", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    Shader mazeShader("shaders/maze_vs", "shaders/maze_fs");
    Shader playerShader("shaders/player_vs", "shaders/player_fs");
    Shader powerupShader("shaders/powerups_vs", "shaders/powerups_fs");
    Shader indiShader("shaders/indicator_vs", "shaders/indicator_fs");

   
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
   float vertices[] = {
     //vertices           //colors            //normals
    -0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f,   0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f,   0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,   0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,   0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,   0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f,   0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,   0.0f,  1.0f,  0.0f
};


float player_vertices[] = {
    -0.3f, -0.3f, -0.3f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f, 
     0.3f, -0.3f, -0.3f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
     0.3f,  0.3f, -0.3f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
     0.3f,  0.3f, -0.3f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
    -0.3f,  0.3f, -0.3f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
    -0.3f, -0.3f, -0.3f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,

    -0.3f, -0.3f,  0.3f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
     0.3f, -0.3f,  0.3f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
     0.3f,  0.3f,  0.3f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
     0.3f,  0.3f,  0.3f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
    -0.3f,  0.3f,  0.3f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
    -0.3f, -0.3f,  0.3f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,

    -0.3f,  0.3f,  0.3f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.3f,  0.3f, -0.3f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.3f, -0.3f, -0.3f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.3f, -0.3f, -0.3f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.3f, -0.3f,  0.3f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.3f,  0.3f,  0.3f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,

     0.3f,  0.3f,  0.3f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.3f,  0.3f, -0.3f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.3f, -0.3f, -0.3f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.3f, -0.3f, -0.3f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.3f, -0.3f,  0.3f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.3f,  0.3f,  0.3f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,

    -0.3f, -0.3f, -0.3f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
     0.3f, -0.3f, -0.3f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
     0.3f, -0.3f,  0.3f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
     0.3f, -0.3f,  0.3f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
    -0.3f, -0.3f,  0.3f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
    -0.3f, -0.3f, -0.3f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,

    -0.3f,  0.3f, -0.3f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
     0.3f,  0.3f, -0.3f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
     0.3f,  0.3f,  0.3f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
     0.3f,  0.3f,  0.3f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
    -0.3f,  0.3f,  0.3f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
    -0.3f,  0.3f, -0.3f,  3.0f,2.0f,1.0f,  0.0f,  1.0f,  0.0f
};


float powerups_vertices[] = {
    -0.1f, -0.1f, -0.1f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f, 
     0.1f, -0.1f, -0.1f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
     0.1f,  0.1f, -0.1f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
     0.1f,  0.1f, -0.1f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
    -0.1f,  0.1f, -0.1f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,
    -0.1f, -0.1f, -0.1f,  1.0f,1.0f,1.0f, 0.0f,  0.0f, -1.0f,

    -0.1f, -0.1f,  0.1f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
     0.1f, -0.1f,  0.1f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
     0.1f,  0.1f,  0.1f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
     0.1f,  0.1f,  0.1f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
    -0.1f,  0.1f,  0.1f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,
    -0.1f, -0.1f,  0.1f,  1.0f,1.0f,1.0f,  0.0f,  0.0f, 1.0f,

    -0.1f,  0.1f,  0.1f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.1f,  0.1f, -0.1f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.1f, -0.1f, -0.1f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.1f, -0.1f, -0.1f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.1f, -0.1f,  0.1f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,
    -0.1f,  0.1f,  0.1f,  1.0f,1.0f,1.0f,-1.0f,  0.0f,  0.0f,

     0.1f,  0.1f,  0.1f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.1f,  0.1f, -0.1f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.1f, -0.1f, -0.1f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.1f, -0.1f, -0.1f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.1f, -0.1f,  0.1f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,
     0.1f,  0.1f,  0.1f,  1.0f,1.0f,1.0f, 1.0f,  0.0f,  0.0f,

    -0.1f, -0.1f, -0.1f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
     0.1f, -0.1f, -0.1f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
     0.1f, -0.1f,  0.1f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
     0.1f, -0.1f,  0.1f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
    -0.1f, -0.1f,  0.1f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,
    -0.1f, -0.1f, -0.1f,  3.0f,2.0f,1.0f, 0.0f, -1.0f,  0.0f,

    -0.1f,  0.1f, -0.1f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
     0.1f,  0.1f, -0.1f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
     0.1f,  0.1f,  0.1f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
     0.1f,  0.1f,  0.1f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
    -0.1f,  0.1f,  0.1f,  3.0f,2.0f,1.0f, 0.0f,  1.0f,  0.0f,
    -0.1f,  0.1f, -0.1f,  3.0f,2.0f,1.0f,  0.0f,  1.0f,  0.0f
};



float obstacle_vertices[] = {
     //vertices           //colors            //normals
    -0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f,   0.0f,  1.0f,  0.0f
};

float indicators_vertices[] = {
     //vertices           //colors            //normals
    -0.02f,  0.02f, -0.02f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.02f,  0.02f, -0.02f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.02f,  0.02f,  0.02f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
     0.02f,  0.02f,  0.02f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
    -0.02f,  0.02f,  0.02f,  1.0f,1.0f,1.0f,  0.0f,  1.0f,  0.0f,
    -0.02f,  0.02f, -0.02f,  1.0f,1.0f,1.0f,   0.0f,  1.0f,  0.0f
};

   

    unsigned int VBO, EBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);


    unsigned int VBO_player, VAO_player;
    glGenVertexArrays(1, &VAO_player);
    glGenBuffers(1, &VBO_player);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_player);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_player);

    glBufferData(GL_ARRAY_BUFFER, sizeof(player_vertices), player_vertices, GL_DYNAMIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int VBO_pow, VAO_pow;
    glGenVertexArrays(1, &VAO_pow);
    glGenBuffers(1, &VBO_pow);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_pow);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pow);

    glBufferData(GL_ARRAY_BUFFER, sizeof(powerups_vertices), powerups_vertices, GL_STATIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int VBO_obs, VAO_obs;
    glGenVertexArrays(1, &VAO_obs);
    glGenBuffers(1, &VBO_obs);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_obs);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_obs);

    glBufferData(GL_ARRAY_BUFFER, sizeof(obstacle_vertices), obstacle_vertices, GL_STATIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int VBO_ind, VAO_ind;
    glGenVertexArrays(1, &VAO_ind);
    glGenBuffers(1, &VBO_ind);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_ind);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ind);

    glBufferData(GL_ARRAY_BUFFER, sizeof(indicators_vertices), indicators_vertices, GL_DYNAMIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    //generating maze

    //set path
    setting_path_in_maze:
    int boundary = maze_dim+1;
    int correct_path[2*maze_dim][2*maze_dim];
    std::memset(correct_path, 0, sizeof(correct_path[0][0]) * 2*maze_dim * 2*maze_dim);
    time_t t1;
    std::srand((unsigned int)time(&t1));
    int count=0;
    int pos_i=-maze_dim;
    int pos_j=-maze_dim;
    correct_path[pos_i+maze_dim][pos_j+maze_dim]=1;
    while(1){
        //finding unvisited neighbors
        if(pos_i==-maze_dim){//bottom-row
            if (pos_j==-maze_dim){//if left-most
                int r = rand()%2;
                if (r==0)pos_i++;//up
                else pos_j++;//right
            }
            else if (pos_j==maze_dim-1){//if right-most
                int r = rand()%2;
                if(r==0)pos_j--;//left
                else pos_i++;//up
            }
            else{//remaining bottom row cells
                int r = rand()%3;
                if(r==0)pos_j--;//left
                else if (r==1)pos_j++;//right)
                else pos_i++;//up
            }
        }

        else if(pos_i==maze_dim-1){//top-row
            if (pos_j==-maze_dim){//if left-most
                int r = rand()%2;
                if (r==0)pos_i--;//down
                else pos_j++;//right
            }
            else if (pos_j==maze_dim-1){//if right-most
                int r = rand()%2;
                if(r==0)pos_j--;//left
                else pos_i--;//down
            }
            else{//remaining top row cells
                int r = rand()%3;
                if(r==0)pos_j--;//left
                else if (r==1)pos_j++;//right)
                else pos_i--;//down
            }
        }

        else{
            int r = rand()%4==0;
            if (r==0)pos_i++;
            else if (r==1)pos_i--;
            else if (r==2)pos_j++;
            else pos_j--;
        }
        correct_path[pos_i+maze_dim][pos_j+maze_dim]=1;
        if (pos_i==maze_dim-1 && pos_j==maze_dim-1)break;
        // if (count>(maze_dim*maze_dim)){cout<<count;goto setting_path_in_maze;}
        count++;
    }
    correct_path[2*maze_dim-1][2*maze_dim-1]=1;


    //placing cubes in maze
    std::memset(place_cube, -1, sizeof(place_cube[0][0]) * 2*maze_dim * 2*maze_dim);
    time_t t2;
    std::srand((unsigned int)time(&t2));
    for(int i = -maze_dim; i < maze_dim; i++){
        for(int j = -maze_dim; j < maze_dim; j++){
            if (correct_path[maze_dim+i][maze_dim+j]==1){
                place_cube[maze_dim+i][maze_dim+j]=0;
            }
            else place_cube[maze_dim+i][maze_dim+j]=int(rand()%2);
        }
    }

    //important functions
    setPowerUpLocations();
    setObstaclesLocations();

   


    // THE RENDER LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOP
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST); 



//=================================================================================

    //graph
    std::memset(graph, 0, sizeof(graph[0][0]) * DIM * DIM);
    for(int i=0;i<2*DIM;i++){
        for(int j=0;j<(((2*DIM))-1);j++){
            if(place_cube[i][j]<=0 && place_cube[i][j+1]<=0){
				int p = (2*DIM*i) + j;
				int q = (2*DIM*i) + j + 1;
                graph[p][q] = 1;
                graph[q][p] = 1;
            }            
        }
    }

    for(int i=0;i<(((2*DIM))-1);i++){
        for(int j=0;j<2*DIM;j++){
            if(place_cube[i][j]==0 && place_cube[i+1][j]==0){
                int p = (2*DIM*i) + j;
				int q = (2*DIM*(i+1)) + j;
                graph[p][q] = 1;
                graph[q][p] = 1;
            }            
        }
    }
    
    if (retarget==1){
        retarget = 0;
        int src = int(  (2*DIM*(maze_dim+round(CameraTarget.x))) + (maze_dim + round(CameraTarget.z))  );
        int dst = int(  (2*DIM*(maze_dim+round(enemy_pos.x))) + (maze_dim + round(enemy_pos.y))  );
        parent = dijkstra(graph, src);

        if (dst-parent[dst]==2*DIM){ enemy_direction=1; target_pos.x = round(enemy_pos.x) - 1.0f; }//left
        else if (dst-parent[dst]==-2*DIM){ enemy_direction=2; target_pos.x = round(enemy_pos.x) + 1.0f; }//right
        else if (dst-parent[dst]==-1){ enemy_direction=3; target_pos.y = round(enemy_pos.y) + 1.0f; }//down
        else if (dst-parent[dst]==1){ enemy_direction=4; target_pos.y = round(enemy_pos.y) - 1.0f; }//up
    }

    if((abs(enemy_pos.x - round(target_pos.x))>0.001) || (abs(enemy_pos.y - round(target_pos.y))>0.001)){
        if(enemy_direction==1) enemy_pos.x = enemy_pos.x - enemy_speed;
        else if(enemy_direction==2) enemy_pos.x = enemy_pos.x + enemy_speed;
        else if(enemy_direction==3) enemy_pos.y = enemy_pos.y + enemy_speed;
        else enemy_pos.y = enemy_pos.y - enemy_speed; 
    }
    else{
        retarget=1;
    }

    if((abs(enemy_pos.x - CameraTarget.x)<0.3) && (abs(enemy_pos.y - CameraTarget.z)<0.3)){
        LOSE = 1;
    }
//=================================================================================
        


        
       
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), float(SCR_WIDTH)/float(SCR_HEIGHT), 0.1f, 100.0f);     
        
        if(render_everything==0){goto RENDER_PLAYER;}
        
        mazeShader.use();     
        mazeShader.setVec3("light.diffuse",diffuse_light);
        mazeShader.setVec3("light.specular", specular_light);
        mazeShader.setFloat("light.constant", constant_light);
        mazeShader.setFloat("light.linear", linear_light);
        mazeShader.setFloat("light.quadratic", quadratic_light);
        mazeShader.setVec3("light.position",CameraTarget);
        mazeShader.setVec3("light.direction", CameraTarget);
        mazeShader.setFloat("light.cutOff", glm::cos(glm::radians(12.0f)));
        mazeShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.f)));
        mazeShader.setVec3("viewPos", CameraPos);
        mazeShader.setFloat("material_shininess",1.0f);
        mazeShader.setMat4("view", view);
        mazeShader.setMat4("projection", projection);

    
        // render the maze
        glBindVertexArray(VAO);
        for(int i = -maze_dim; i < maze_dim; i++){
            for(int j = -maze_dim; j < maze_dim; j++){
                
                if ((place_cube[maze_dim+i][maze_dim+j]>0)){
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(float(i),0.0f,float(j)));
                    if(maze_rotation==1){
                        float angle = (float)glfwGetTime()*200;
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
                    }
                    mazeShader.setMat4("model", model);

                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        //boundaries
        for(int i = -boundary; i < boundary; i++){
            for(int j = -boundary; j < boundary; j++){
                if ( (i==-boundary && j==-boundary+1) || (i==boundary-1 && j==boundary-2) )continue;    

                else if (i==-boundary||i==boundary-1||j==-boundary||j==boundary-1){
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(float(i),0.0f,float(j)));
                    mazeShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

      

        //resetting settings after collision
        if (((collision_timer+10) == time(NULL)) && (ongoing_collision==1)){
            diffuse_light = glm::vec3(old_diffuse_light.x,old_diffuse_light.y,old_diffuse_light.z);
            specular_light = glm::vec3(old_specular_light.x,old_specular_light.y,old_specular_light.z);
            player_color = glm::vec4(old_player_color.x,old_player_color.y,old_player_color.z,1.0f);
            cam_speed = 0.02f;
            scale_rate = 10.0f;
            ongoing_collision = 0;
            powerup = 0;
        }

        //resetting speedup timer
        if (((speedup_timer+10) == time(NULL)) && (speedup==1)){
            if (remove_wall==0){
                diffuse_light = glm::vec3(old_diffuse_light.x,old_diffuse_light.y,old_diffuse_light.z);
                specular_light = glm::vec3(old_specular_light.x,old_specular_light.y,old_specular_light.z);
                player_color = glm::vec4(old_player_color.x,old_player_color.y,old_player_color.z,1.0f);
                powerup = 0;
            }//else it is already blue

            //common
            cam_speed = 0.02f;
            scale_rate = 10.0f;
            ongoing_collision = 0;
            speedup = 0;
        }

        //resetting remove_wall timer
        if (((remove_wall_timer+10) == time(NULL)) && (remove_wall==1)){
            if(speedup==0){
                diffuse_light = glm::vec3(old_diffuse_light.x,old_diffuse_light.y,old_diffuse_light.z);
                specular_light = glm::vec3(old_specular_light.x,old_specular_light.y,old_specular_light.z);
                player_color = glm::vec4(old_player_color.x,old_player_color.y,old_player_color.z,1.0f);
                cam_speed = 0.02f;
                scale_rate = 10.0f;
                powerup = 0;
            }
            else if (speedup==1){//make it green
                diffuse_light = glm::vec3(0.0f,0.0f,1.0f);
                specular_light = glm::vec3(0.0f,0.0f,1.0f);
                player_color = glm::vec4(glm::vec3(0.0f,0.0f,1.0f),1.0f);
            }

            //common
            ongoing_collision = 0;
            remove_wall = 0;
        }


        //resetting maze rotation timer
        if ((maze_rotation_timer+15) == time(NULL)){
            maze_rotation = 0;
            collision_thresh = maze_old_collision_thresh;
        }

        //resetting grow timer
        if ((grow_timer+15) == time(NULL)){
            grow = 0;
            collision_thresh = grow_old_collision_thresh;
            player_scale = 1.0f;
        }
        //to remove collision bugs due to scaling
        if (player_scale == 1.3f) collision_thresh = 0.048f;
        if (player_scale == 1.0f && maze_rotation == 0) collision_thresh = 0.18;
        if (player_scale==1.0f && maze_rotation == 1) collision_thresh = 0.038;


        //WIN
        if(int(round(CameraPos.x))==-(maze_dim+2) && int(round(CameraPos.z))==-maze_dim){
            disable_movement=1;
            player_color = glm::vec4(0.4,1.0,0.4,1.0);
            scale_rate = 10.0f;
            antenna_rotation = 0.0f;
            player_scale = 1.0f;
            powerup = 1;
            display_indicators = 0;
            WIN = 1;       
            render_everything = 0;   
        }

        //LOSE
        if(LOSE==1 || health<=0){
            disable_movement=1;
            player_color = glm::vec4(1.0,0.4,0.4,1.0);
            scale_rate = 10.0f;
            player_scale = 1.0f;
            antenna_rotation = 0.0f;
            powerup = 1;
            display_indicators = 0;
            render_everything = 0;
        }



        //attenuation
        if (powerup==0){
            coin_color = glm::vec3(1.0,0.6,0.0);
            health_color = glm::vec3(0.0,1.0,1.0);
            speedup_color = glm::vec3(0.0,1.0,0.0);
            remove_wall_color = glm::vec3(0.0,0.0,1.0);
            battery_color = glm::vec3(0.2,0.2,0.2);
            constant_light = 1.0f;
            linear_light = 1.0f;
            quadratic_light = 1.0f;
        }
        if ((player_color.x!=player_color.y) && (player_color.x!=player_color.z) && (player_color.y!=player_color.z)){
            coin_color = glm::vec3(1.0,1.0,1.0);
            health_color = glm::vec3(1.0,1.0,1.0);
            speedup_color = glm::vec3(1.0,1.0,1.0);
            remove_wall_color = glm::vec3(1.0,1.0,1.0);
            battery_color = glm::vec3(1.0,1.0,1.0);
            constant_light = 1.0f;
            linear_light = 0.0f;
            quadratic_light = 1.0f;
        }


        //power decay
        if (ongoing_collision==0 && speedup==0 && remove_wall==0 && powerup==0){

            if (decay_power_timer+1 <= time(NULL)){
                if (power_bar_value>0){
                power_bar_value -= 5.0000045f;
                decay_power_timer = time(NULL);        
                }           
            }
            if (diffuse_light.y<=0){
                diffuse_light = glm::vec3(0.0f,0.0f,0.0f);
                if (specular_light.y<=0){
                    specular_light = glm::vec3(0.0f,0.0f,0.0f);
                }
                else{
                    specular_light = glm::vec3(specular_light.x-light_decay,specular_light.y-light_decay,specular_light.z-light_decay);
                }
            }
            else{
                diffuse_light = glm::vec3(diffuse_light.x-diffuse_decay,diffuse_light.y-diffuse_decay,diffuse_light.z-diffuse_decay);
            }
            if (player_color.y>0.1){
                player_color = glm::vec4(player_color.x-diffuse_decay,player_color.y-diffuse_decay,player_color.z-diffuse_decay,1.0f);
            }
        }

        RENDER_PLAYER:
        //render the player
        playerShader.use();
        playerShader.setVec4("playerColor",player_color);
        playerShader.setMat4("view", view);
        playerShader.setMat4("projection", projection);

        glBindVertexArray(VAO_player);

        glm::mat4 model_player = glm::mat4(1.0f);        
        model_player = glm::translate(model_player, glm::vec3(CameraPos.x,0.0f,CameraPos.z));
        model_player = glm::scale(model_player, glm::vec3(player_scale,player_scale,player_scale));
        mazeShader.setMat4("model", model_player);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        if(render_everything==0){goto RENDER_ANTENNA;}


        //render the enemy
        playerShader.use();
        playerShader.setVec4("playerColor",glm::vec4(0.7,0.1,1.0,1.0));
        playerShader.setMat4("view", view);
        playerShader.setMat4("projection", projection);

        glBindVertexArray(VAO_player);

        model_enemy = glm::mat4(1.0f);        
        model_enemy = glm::translate(model_enemy, glm::vec3(enemy_pos.x,0.0f,enemy_pos.y));
        model_enemy = glm::scale(model_enemy, glm::vec3(0.3,0.3,0.3));
        model_enemy = glm::rotate(model_enemy, glm::radians((float)glfwGetTime()*2000), glm::vec3(0.0f, 1.0f, 0.0f));
        mazeShader.setMat4("model", model_enemy);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        RENDER_ANTENNA:

        //rendering powerups
        powerupShader.use();
        powerupShader.setMat4("view", view);
        powerupShader.setMat4("projection", projection);

        powerupShader.setVec3("light.diffuse",diffuse_light);
        powerupShader.setVec3("light.specular", specular_light);
        powerupShader.setFloat("light.constant", 1.0f);
        powerupShader.setFloat("light.linear", 1.0f);
        powerupShader.setFloat("light.quadratic", 0.5f);
        powerupShader.setVec3("light.position",CameraTarget);
        powerupShader.setVec3("light.direction", CameraTarget);
        powerupShader.setFloat("light.cutOff", glm::cos(glm::radians(12.0f)));
        powerupShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.f)));
        powerupShader.setVec3("viewPos", CameraPos);
        powerupShader.setFloat("material_shininess",1.0f);
        
        
        glBindVertexArray(VAO_pow);

        //antenna
        glm::mat4 model_pow = glm::mat4(1.0f);        
        model_pow = glm::translate(model_pow, glm::vec3(CameraPos.x,0.3f,CameraPos.z));
        float scaling_factor = sin(float(glfwGetTime())*scale_rate);
        model_pow = glm::scale(model_pow, glm::vec3(scaling_factor,scaling_factor,scaling_factor));
        model_pow = glm::rotate(model_pow, glm::radians(float(glfwGetTime()*antenna_rotation)), glm::vec3(0.0f, 1.0f, 0.0f));
        powerupShader.setMat4("model", model_pow);
        powerupShader.setVec3("singleColor",glm::vec3(player_color.x,player_color.y,player_color.z));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        if(render_everything==0){goto END;}


        // render the powerups
        glBindVertexArray(VAO_pow);
        for(int i = -maze_dim; i < maze_dim; i++){
            for(int j = -maze_dim; j < maze_dim; j++){
                int val = place_pow[maze_dim+i][maze_dim+j];
                if (val>0 && val<50){
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(float(i),-0.4f,float(j)));

                    //battery
                    if(val<=5){
                        powerupShader.setVec3("singleColor", battery_color);
                    }

                    //health
                    else if(val>3 && val<=6){
                        float angle = (float)glfwGetTime()*20;
                        float scaling_factor = sin(float(glfwGetTime())*10) + 0.5;
                        model = glm::scale(model, glm::vec3(scaling_factor,scaling_factor,scaling_factor));
                        powerupShader.setVec3("singleColor", health_color);
                    }

                    //speedup
                    else if(val>6 && val<=9){
                        float angle = (float)glfwGetTime()*200;
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.3f, -1.0f, 0.5f));
                        float scaling_factor = sin(float(glfwGetTime())*10) ;
                        model = glm::scale(model, glm::vec3(scaling_factor,scaling_factor,scaling_factor));
                        powerupShader.setVec3("singleColor", speedup_color);
                    }

                    //remove wall
                    else if(val>9 && val<=12){
                        float angle = 45.0f;
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
                        angle = (float)glfwGetTime()*200;
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
                        float scaling_factor = sin(float(glfwGetTime())*10) + 1.5;
                        model = glm::scale(model, glm::vec3(scaling_factor,scaling_factor,scaling_factor));
                        powerupShader.setVec3("singleColor", remove_wall_color);
                    }
                    
                    //coins
                    else{
                        float angle = (float)glfwGetTime()*2000;
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
                        powerupShader.setVec3("singleColor", coin_color);
                    }
                    
                    powerupShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }


        //render obstacles
        glBindVertexArray(VAO_obs);
        for(int i = -maze_dim; i < maze_dim; i++){
            for(int j = -maze_dim; j < maze_dim; j++){
                
                if (place_cube[maze_dim+i][maze_dim+j]<=0){
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(float(i),-1.0f,float(j)));
                    if(place_obs[maze_dim+i][maze_dim+j]>=20 || place_cube[maze_dim+i][maze_dim+j]==-1){
                        powerupShader.setVec3("singleColor", glm::vec3(1.0,1.0,1.0));
                    }
                    else{//place obstacles ( range = 1 to 20)
                        float scaling_factor = sin(float(glfwGetTime())*4);
                        model = glm::scale(model, glm::vec3(scaling_factor,1.0f,scaling_factor));
                        powerupShader.setVec3("singleColor", glm::vec3(0.5,0.5,0.5));         
                    }
                    powerupShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }



     if (display_indicators==1){   

        //++++++++++++++++++++++++++++++++++++++++++++++
    

        glDisable(GL_DEPTH_TEST); 

        //render health bar
        glBindVertexArray(VAO_ind);
        indiShader.use();
        indiShader.setVec3("barColor",glm::vec3(0.0,0.7,0.7));
        indiShader.setMat4("view", view);
        indiShader.setMat4("projection", projection);
        float shift_x=0.0f;
        for(int i=0;i<health;i++){
            glm::mat4 model_bar = glm::mat4(1.0f);        
            model_bar = glm::translate(model_bar, glm::vec3(CameraPos.x-h_bar_x+shift_x,y_pos_moving-2.8f,CameraPos.z+h_bar_z));
            indiShader.setMat4("model", model_bar);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            shift_x += 0.01;
        }

        //render power bar
        glBindVertexArray(VAO_ind);
        indiShader.use();
        indiShader.setVec3("barColor",glm::vec3(1.0,0.7,0.0));
        indiShader.setMat4("view", view);
        indiShader.setMat4("projection", projection);
        shift_x=0.0f;
        for(int i=0;i<power_bar_value;i++){
            glm::mat4 model_bar = glm::mat4(1.0f);        
            model_bar = glm::translate(model_bar, glm::vec3(CameraPos.x-p_bar_x+shift_x,y_pos_moving-2.8f,CameraPos.z+p_bar_z));
            indiShader.setMat4("model", model_bar);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            shift_x += 0.01;
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //render coin bar placeholder
        glBindVertexArray(VAO_ind);
        indiShader.use();
        indiShader.setVec3("barColor",glm::vec3(1.0,0.7,0.0));
        indiShader.setMat4("view", view);
        indiShader.setMat4("projection", projection);
        shift_x=0.0f;
        for(int i=0;i<max_coins;i++){
            glm::mat4 model_bar = glm::mat4(1.0f);      
            float angle = (float)glfwGetTime()*2000;
            model_bar = glm::translate(model_bar, glm::vec3(CameraPos.x-c_bar_x+shift_x,y_pos_moving-2.8f,CameraPos.z+c_bar_z));
            model_bar = glm::rotate(model_bar, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));            
            indiShader.setMat4("model", model_bar);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            shift_x += 0.06;
        }


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //render coin bar
        glBindVertexArray(VAO_ind);
        indiShader.use();
        indiShader.setVec3("barColor",glm::vec3(1.0,0.7,0.0));
        indiShader.setMat4("view", view);
        indiShader.setMat4("projection", projection);
        shift_x=0.0f;
        for(int i=0;i<coins;i++){
            glm::mat4 model_bar = glm::mat4(1.0f);      
            float angle = (float)glfwGetTime()*2000;
            model_bar = glm::translate(model_bar, glm::vec3(CameraPos.x-c_bar_x+shift_x,y_pos_moving-2.8f,CameraPos.z+c_bar_z));
            model_bar = glm::rotate(model_bar, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
            indiShader.setMat4("model", model_bar);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            shift_x += 0.06;
        }


        glEnable(GL_DEPTH_TEST); 

        //++++++++++++++++++++++++++++++++++++++++++++++
     } //diplay indicator ends here


        END:


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO_player);
    glDeleteVertexArrays(1, &VAO_pow);
    glDeleteVertexArrays(1, &VAO_obs);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_player);
    glDeleteBuffers(1, &VBO_pow);
    glDeleteBuffers(1, &VBO_obs);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    checkPowerUps();
    checkObstacles();    


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    //smaller view
    if (glfwGetKey(window,GLFW_KEY_C)==GLFW_PRESS){
        CameraPos = glm::vec3(CameraPos.x,4.0f,CameraPos.z);
        CameraTarget = glm::vec3(CameraTarget.x,0.0f,CameraTarget.z);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }

    //zoom-out view
    if (glfwGetKey(window,GLFW_KEY_Z)==GLFW_PRESS){
        if (y_pos_moving<max_zoom_out)y_pos_moving *= 1.01f;
        CameraPos = glm::vec3(CameraPos.x,y_pos_moving,CameraPos.z);
        CameraTarget = glm::vec3(CameraTarget.x,0.0f,CameraTarget.z);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }

     //zoom-in view
    if (glfwGetKey(window,GLFW_KEY_X)==GLFW_PRESS){
        if (y_pos_moving>max_zoom_in)y_pos_moving /= 1.01f;
        CameraPos = glm::vec3(CameraPos.x,y_pos_moving,CameraPos.z);
        CameraTarget = glm::vec3(CameraTarget.x,0.0f,CameraTarget.z);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }

    //full view
    if (glfwGetKey(window,GLFW_KEY_F)==GLFW_PRESS){
        CameraPos = glm::vec3(CameraPos.x,maze_dim*3.0f,CameraPos.z);
        CameraTarget = glm::vec3(CameraTarget.x,0.0f,CameraTarget.z);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }

    
    //camera and player movement
    //RIGHT
    if (glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS && disable_movement==0){

        //collision check
        bool c1 = collision_check(window,'r',CameraPos.x,CameraPos.z);
        bool c2 = collision_check(window,'u',CameraPos.x+0.3f,CameraPos.z);
        bool c3 = collision_check(window,'d',CameraPos.x+0.3f,CameraPos.z);
        if (c1 || c2 || c3){
            if (ongoing_collision==0 && remove_wall==0){
                health = health - 30;
                powerup = 1;
                ongoing_collision = 1;
                cam_speed = 0.005f;
                scale_rate = 2.5f;
                if (speedup==0){
                    old_diffuse_light = glm::vec3(diffuse_light.x,diffuse_light.y,diffuse_light.z);
                    old_specular_light = glm::vec3(specular_light.x,specular_light.y,specular_light.z);
                    old_player_color = glm::vec4(player_color.x,player_color.y,player_color.z,1.0f);
                }
                player_color = glm::vec4(1.0,0.2,0.2,1.0);
                diffuse_light = glm::vec3(0.0,0.0,0.0);
                specular_light = glm::vec3(1.0,0.0,0.0);
                collision_timer = time(NULL);
            }
            else if(remove_wall==1){                
                place_cube[maze_dim+int(round(CameraPos.x))+1][maze_dim+int(round(CameraPos.z))] = -1;
            }
            return;
        }        

        //move
        CameraPos = glm::vec3(CameraPos.x+cam_speed,y_pos_moving,CameraPos.z);
        CameraTarget = glm::vec3(CameraTarget.x+cam_speed,0.0f,CameraTarget.z);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }
    
    //LEFT
    if (glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS && disable_movement==0){

        //collision check
        bool c1 = collision_check(window,'l',CameraPos.x,CameraPos.z);
        bool c2 = collision_check(window,'u',CameraPos.x-0.3f,CameraPos.z);
        bool c3 = collision_check(window,'d',CameraPos.x-0.3f,CameraPos.z);
        if (c1 || c2 || c3){
            if (ongoing_collision==0 && remove_wall==0){
                health = health - 30;
                powerup = 1;
                ongoing_collision = 1;
                cam_speed = 0.005f;
                scale_rate = 2.5f;
                if (speedup==0){
                    old_diffuse_light = glm::vec3(diffuse_light.x,diffuse_light.y,diffuse_light.z);
                    old_specular_light = glm::vec3(specular_light.x,specular_light.y,specular_light.z);
                    old_player_color = glm::vec4(player_color.x,player_color.y,player_color.z,1.0f);
                }
                player_color = glm::vec4(1.0,0.2,0.2,1.0);
                diffuse_light = glm::vec3(0.0,0.0,0.0);
                specular_light = glm::vec3(1.0,0.0,0.0);
                collision_timer = time(NULL);
            }
            else if(remove_wall==1){                
                place_cube[maze_dim+int(round(CameraPos.x))-1][maze_dim+int(round(CameraPos.z))] = -1;
            }
            return;
        }   

        //move
        CameraPos = glm::vec3(CameraPos.x-cam_speed,y_pos_moving,CameraPos.z);
        CameraTarget = glm::vec3(CameraTarget.x-cam_speed,0.0f,CameraTarget.z);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }

    //UP
    if (glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS && disable_movement==0){

        //collision check
        bool c1 = collision_check(window,'u',CameraPos.x,CameraPos.z);
        bool c2 = collision_check(window,'l',CameraPos.x,CameraPos.z-0.3f);
        bool c3 = collision_check(window,'r',CameraPos.x,CameraPos.z-0.3f);
        if (c1 || c2 || c3){
            if (ongoing_collision==0 && remove_wall==0){
                health = health - 30;
                powerup = 1;
                ongoing_collision = 1;
                cam_speed = 0.005f;
                scale_rate = 2.5f;
                if (speedup==0){
                    old_diffuse_light = glm::vec3(diffuse_light.x,diffuse_light.y,diffuse_light.z);
                    old_specular_light = glm::vec3(specular_light.x,specular_light.y,specular_light.z);
                    old_player_color = glm::vec4(player_color.x,player_color.y,player_color.z,1.0f);
                }
                player_color = glm::vec4(1.0,0.2,0.2,1.0);
                diffuse_light = glm::vec3(0.0,0.0,0.0);
                specular_light = glm::vec3(1.0,0.0,0.0);
                collision_timer = time(NULL);
            }
            else if(remove_wall==1){                
                place_cube[maze_dim+int(round(CameraPos.x))][maze_dim+int(round(CameraPos.z))-1] = -1;
            }
            return;
        }   

        //move
        CameraPos = glm::vec3(CameraPos.x,y_pos_moving,CameraPos.z-cam_speed);
        CameraTarget = glm::vec3(CameraTarget.x,0.0f,CameraTarget.z-cam_speed);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }

    //DOWN
    if (glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS && disable_movement==0){

        //collision check
        bool c1 = collision_check(window,'d',CameraPos.x,CameraPos.z);
        bool c2 = collision_check(window,'l',CameraPos.x,CameraPos.z+0.3f);
        bool c3 = collision_check(window,'r',CameraPos.x,CameraPos.z+0.3f);
        if (c1 || c2 || c3){
            if (ongoing_collision==0 && remove_wall==0){
                health = health - 30;
                powerup = 1;
                ongoing_collision = 1;
                cam_speed = 0.005f;
                scale_rate = 2.5f;
                if (speedup==0){
                    old_diffuse_light = glm::vec3(diffuse_light.x,diffuse_light.y,diffuse_light.z);
                    old_specular_light = glm::vec3(specular_light.x,specular_light.y,specular_light.z);
                    old_player_color = glm::vec4(player_color.x,player_color.y,player_color.z,1.0f);
                }
                player_color = glm::vec4(1.0,0.2,0.2,1.0);
                diffuse_light = glm::vec3(0.0,0.0,0.0);
                specular_light = glm::vec3(1.0,0.0,0.0);
                collision_timer = time(NULL);
            }
            else if(remove_wall==1){                
                place_cube[maze_dim+int(round(CameraPos.x))][maze_dim+int(round(CameraPos.z))+1] = -1;
            }
            return;
        }   

        //move
        CameraPos = glm::vec3(CameraPos.x,y_pos_moving,CameraPos.z+cam_speed);
        CameraTarget = glm::vec3(CameraTarget.x,0.0f,CameraTarget.z+cam_speed);
        CameraUp = glm::vec3(0.0f,0.0f,-1.0f);
        view = glm::lookAt(CameraPos,CameraTarget, CameraUp);
    }

}



//collision checker
bool collision_check(GLFWwindow *window,char direction,float pos_x,float pos_z){
    if (direction=='r'){
        float space = -1*(round(pos_x) - pos_x);
        if (space>=collision_thresh){
            if (round(pos_x)>=maze_dim-1){
                return 1;
            }  
            else if ((place_cube[maze_dim+int(round(pos_x))+1][maze_dim+int(round(pos_z))])==1){
                return 1;
            }         
        }
        return 0;
    }

    if (direction=='l'){
        float space = round(pos_x) - pos_x;
        if (round(pos_x)<=-(maze_dim+1+0.3)){
            disable_movement=1;
            return 1;
        }  
        if (space>=collision_thresh){
            if (round(pos_x)<=-maze_dim && round(pos_z)>-maze_dim){
                return 1;
            }  
            else if ((place_cube[maze_dim+int(round(pos_x))-1][maze_dim+int(round(pos_z))])==1){
                return 1;
            }         
        }
        return 0;
    }

    if (direction=='u'){
        float space = round(pos_z) - pos_z;
        if (space>=collision_thresh){
            if (round(pos_z)<=-maze_dim){
                return 1;
            }
            else if(round(pos_x)>=maze_dim){
                return 1;
            }  
            else if ((place_cube[maze_dim+int(round(pos_x))][maze_dim+int(round(pos_z))-1])==1){
                return 1;
            }         
        }
        return 0;
    }

    if (direction=='d'){
        float space = -1*(round(pos_z) - pos_z);
        if (space>=collision_thresh){
            if (round(pos_z)>=maze_dim-1){
                return 1;
            }  
            else if ((place_cube[maze_dim+int(round(pos_x))][maze_dim+int(round(pos_z))+1])==1){
                return 1;
            }         
        }
        return 0;
    }

    //remove this else part in case of bugs (its just to suppress control warning)
    else{
        return 0;
    }

    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    
}


void setObstaclesLocations(){
    time_t t_obs;
    std::memset(place_obs, -1, sizeof(place_obs[0][0]) * 2*maze_dim * 2*maze_dim);
    std::srand((unsigned int)time(&t_obs));
    for(int i = -maze_dim; i < maze_dim; i++){
        for(int j = -maze_dim; j < maze_dim; j++){
            if (place_cube[maze_dim+i][maze_dim+j]==0){
                place_obs[maze_dim+i][maze_dim+j]=rand()%obstacles_quantity_factor; //don't place on -1 and 0
            }
        }
    }

}


void setPowerUpLocations(){
    time_t t_pow;
    std::memset(place_pow, -1, sizeof(place_pow[0][0]) * 2*maze_dim * 2*maze_dim);
    std::srand((unsigned int)time(&t_pow));
    for(int i = -maze_dim; i < maze_dim; i++){
        for(int j = -maze_dim; j < maze_dim; j++){
            if (place_cube[maze_dim+i][maze_dim+j]==0){
                place_pow[maze_dim+i][maze_dim+j]=rand()%powerups_quantity_factor; //don't place on -1 and 0
            }
        }
    }

}

void checkPowerUps(){
        float pos_x = CameraPos.x;
        float pos_z = CameraPos.z;
        int c_x = round(pos_x);
        int c_z = round(pos_z);
        int val = place_pow[maze_dim+c_x][maze_dim+c_z];

        //battery
        if(val>0 && val<=5){
            power_bar_value = max_power;
            if (ongoing_collision==1){
                old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                old_specular_light = glm::vec3(1.0,1.0,1.0);
                old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                diffuse_light = glm::vec3(1.0,0.0,0.0);
                specular_light = glm::vec3(1.0,0.0,0.0);
                player_color = glm::vec4(1.0,0.0,0.0,1.0);
            }
            else if(speedup==1 && remove_wall==0){
                old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                old_specular_light = glm::vec3(1.0,1.0,1.0);
                old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                diffuse_light = glm::vec3(0.0,1.0,0.0);
                specular_light = glm::vec3(0.0,1.0,0.0);
                player_color = glm::vec4(0.0,1.0,0.0,1.0);
            }
            else if(speedup==0 && remove_wall==1){
                old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                old_specular_light = glm::vec3(1.0,1.0,1.0);
                old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                diffuse_light = glm::vec3(0.0,0.0,1.0);
                specular_light = glm::vec3(0.0,0.0,1.0);
                player_color = glm::vec4(0.0,0.0,1.0,1.0);
            }
            else{
                old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                old_specular_light = glm::vec3(1.0,1.0,1.0);
                old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                diffuse_light = glm::vec3(1.0,1.0,1.0);
                specular_light = glm::vec3(1.0,1.0,1.0);
                player_color = glm::vec4(1.0,1.0,1.0,1.0);
            }
        }

        //health
        else if(val>3 && val<=6){
            if (health<max_health) health=health + 30;
        }

        //speedup
        else if(val>6 && val<=9){
            if (ongoing_collision==1){
                powerup = 0;
                ongoing_collision = 0;
                speedup = 0;
                diffuse_light = glm::vec3(old_diffuse_light.x,old_diffuse_light.y,old_diffuse_light.z);
                specular_light = glm::vec3(old_specular_light.z,old_specular_light.y,old_specular_light.z);
                player_color = glm::vec4(old_player_color.x,old_player_color.y,old_player_color.z,1.0f);
                cam_speed = 0.02f;
                scale_rate = 10.0f;
            }
            else if (remove_wall==1){
                speedup = 1;
                cam_speed = 0.04f;
                scale_rate = 20.0f;
                speedup_timer = time(NULL);
                powerup = 1;
            }
            else if(speedup==1){
                speedup_timer = time(NULL);
                powerup = 1;                
                diffuse_light = glm::vec3(0.0,1.0,0.0);
                specular_light = glm::vec3(0.0,1.0,0.0);
                player_color = glm::vec4(0.0,1.0,0.0,1.0);
                cam_speed = 0.04f;
                scale_rate = 20.0f;
            }
            else{
                old_diffuse_light = glm::vec3(diffuse_light.x,diffuse_light.y,diffuse_light.z);
                old_specular_light = glm::vec3(specular_light.z,specular_light.y,specular_light.z);
                old_player_color = glm::vec4(player_color.x,player_color.y,player_color.z,1.0f);
                speedup = 1;
                speedup_timer = time(NULL);
                powerup = 1;
                diffuse_light = glm::vec3(0.0,1.0,0.0);
                specular_light = glm::vec3(0.0,1.0,0.0);
                player_color = glm::vec4(0.0,1.0,0.0,1.0);
                cam_speed = 0.04f;
                scale_rate = 20.0f;
            }
        }

        //remove wall
        else if(val>9 && val<=12){
            if (ongoing_collision==1){
                powerup = 0;
                ongoing_collision = 0;
                diffuse_light = glm::vec3(old_diffuse_light.x,old_diffuse_light.y,old_diffuse_light.z);
                specular_light = glm::vec3(old_specular_light.z,old_specular_light.y,old_specular_light.z);
                player_color = glm::vec4(old_player_color.x,old_player_color.y,old_player_color.z,1.0f);
                cam_speed = 0.02f;
                scale_rate = 10.0f;
            }
            else if(speedup==1){
                remove_wall = 1;
                powerup = 1;
                remove_wall_timer = time(NULL);
                player_color = glm::vec4(0.0,0.0,1.0,1.0f);
                diffuse_light = glm::vec3(0.0,0.0,1.0);
                specular_light = glm::vec3(0.0,0.0,1.0);
            }
            else if(remove_wall==1){
                remove_wall = 1;
                powerup = 1;
                remove_wall_timer = time(NULL);
            }
            else{
                old_diffuse_light = glm::vec3(diffuse_light.x,diffuse_light.y,diffuse_light.z);
                old_specular_light = glm::vec3(specular_light.z,specular_light.y,specular_light.z);
                old_player_color = glm::vec4(player_color.x,player_color.y,player_color.z,1.0f);
                remove_wall = 1;
                powerup = 1;
                remove_wall_timer = time(NULL);
                diffuse_light = glm::vec3(0.0,0.0,1.0);
                specular_light = glm::vec3(0.0,0.0,1.0);
                player_color = glm::vec4(0.0,0.0,1.0,1.0);
            }
        }
        //coins
        else if(val>12 && val <50){
            coins++;
            if (coins==max_coins){
                coins = 0;
                //does what battery can do
                power_bar_value = max_power;
                if (ongoing_collision==1){
                    old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                    old_specular_light = glm::vec3(1.0,1.0,1.0);
                    old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                    diffuse_light = glm::vec3(1.0,0.0,0.0);
                    specular_light = glm::vec3(1.0,0.0,0.0);
                    player_color = glm::vec4(1.0,0.0,0.0,1.0);
                }
                else if(speedup==1 && remove_wall==0){
                    old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                    old_specular_light = glm::vec3(1.0,1.0,1.0);
                    old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                    diffuse_light = glm::vec3(0.0,1.0,0.0);
                    specular_light = glm::vec3(0.0,1.0,0.0);
                    player_color = glm::vec4(0.0,1.0,0.0,1.0);
                }
                else if(speedup==0 && remove_wall==1){
                    old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                    old_specular_light = glm::vec3(1.0,1.0,1.0);
                    old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                    diffuse_light = glm::vec3(0.0,0.0,1.0);
                    specular_light = glm::vec3(0.0,0.0,1.0);
                    player_color = glm::vec4(0.0,0.0,1.0,1.0);
                }
                else{
                    old_diffuse_light = glm::vec3(1.0,1.0,1.0);
                    old_specular_light = glm::vec3(1.0,1.0,1.0);
                    old_player_color = glm::vec4(1.0,1.0,1.0,1.0);
                    diffuse_light = glm::vec3(1.0,1.0,1.0);
                    specular_light = glm::vec3(1.0,1.0,1.0);
                    player_color = glm::vec4(1.0,1.0,1.0,1.0);
                }
            }
        }

        //consuming_powerup
        place_pow[maze_dim+c_x][maze_dim+c_z] = -1;
}





void checkObstacles(){    
    float pos_x = CameraPos.x;
    float pos_z = CameraPos.z;
    int c_x = round(pos_x);
    int c_z = round(pos_z);
    int val = place_obs[maze_dim+c_x][maze_dim+c_z];
    if (val>=1 && val <20){
        //maze rotation
        if (val>=1 && val <5){
            maze_rotation = 1;
            maze_rotation_timer = time(NULL);
            grow_old_collision_thresh = collision_thresh;
            collision_thresh = 0.038f;
        }

        //grow size
        else  if (val>=5 && val <10){
            grow = 1;
            player_scale = 1.3f;
            maze_old_collision_thresh = collision_thresh;
            collision_thresh = 0.048f;
            grow_timer = time(NULL);
        }

        //reduce health
        else if (val>=5 && val <10){
            health = health - 20;
            if (health<=0){
                LOSE = 1;
            }
        }

        //reduce coins
        else{
            coins = 0;
        }
    }
}
