#include <SDL.h>
#include <GL/glew.h>
#include <glm.hpp>

#include <stdio.h>
#include <ctime>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

void hourHand(float angle);
void minuteHand(float angle);
void secondHand(float angle);
void drawClock();
void draw(float angle, int type);


const char* vertexShaderSource =
"  #version 330 core                              \n"
"                                                 \n"
"  layout(location = 0) in vec2 position;         \n"
"  layout(location = 1) in vec3 color;           \n"
"  uniform mat3 transform;                        \n"
"                                                 \n"
"  out vec3 the_color;                            \n"
"  void main()                                    \n"
"  {                                              \n"
"                                                 \n"
"      vec3 p = transform * vec3(position, 1.0);     \n"   
"      gl_Position = vec4(p[0], p[1], 0.0, 1.0); \n"
"      the_color = color;                          \n"
"      //gl_Position = vec4(position, 0.0, 1.0);  \n"
"                                                 \n"
"  }                                              \n";


const char* fragmentShaderSource =
"  #version 330 core                              \n"
"  in vec3 the_color;                              \n"
"  out vec4 outColor;                             \n"
"                                                 \n"
"  void main()                                    \n"
"  {                                              \n"
"      outColor = vec4(the_color , 1.0);       \n"
"      //outColor = vec4(1.0, 1.0, 1.0 , 1.0);       \n"
"  }                                              \n";


float test[] = {
   //vertices 1  
   -1, 1, 0,     
   -1, 0, 0,      
    1, 1, 0,     
    1, 0, 0,   

    //color 1
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,
    1, 1, 1,

    //vertices 2    
   -1, 1, 0,     
   -1, 0, 0,     
    1, 1, 0,    
    1, 0, 0,     

    //color 2
    1, 1, 1,
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,

    
};

GLint matrix_id;


int main(int argc, char* argv[])
{
    int SCREEN_WIDTH = 800;
    int SCREEN_HEIGHT = 600;
    // Initializing SDL

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    

    // Creating SDL Window


    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window) {
        printf("Cannot create the sdl window: %s", SDL_GetError());
        return -1;
    }
    // Creating GL context for the window, it is required for any gl functions to work
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        printf("Cannot create OpenGL context, probably the requested version of OpenGL is not supported on your machine.");
        printf("SDL ERROR: %s", SDL_GetError());
        return -1;
    }


    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();


    // Initialize OpenGL


    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


   

   
    // Create vertex buffer object, this is a buffer in gpu memory
    GLuint vbo;
    glGenBuffers(1, &vbo); // Generate 1 buffer

    // make my buffer, the active buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Copy data from main memory to GPU memory
    // The last parameter in the glBufferData, determines how we plan to use data 
    // GL_STATIC_DRAW   : The vertex data will be uploaded once and drawn many times
    // GL_DYNAMIC_DRAW  : The vertex data will be created once, changed from time to time, but drawn many times.
    // GL_STREAM_DRAW   : The vertex data will be uploaded once and drawn once.
    glBufferData(GL_ARRAY_BUFFER, sizeof(test), test, GL_STATIC_DRAW);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        printf("Cannot compile vertex shader!");
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        printf("Compilation log: %s", buffer);
        return -1;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        printf("Cannot compile fragment shader!");
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        printf("Compilation log: %s", buffer);
        return -1;
    }

    // Linking vertex and fragment shaders to each other, and create a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);

    // check if program linked correctly!
    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        printf("Cannot link the program shader!");
        char buffer[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
        printf("Compilation log: %s", buffer);
        return -1;
    }


    // Making the current program the active one, only one program can be active in any given time
    glUseProgram(shaderProgram);

    // Retrive a reference to position attribute in vertex shader program
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");

    // specify the mapping from out buffer into position variable in vertex shader, it also attaches the active vbo into program
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

     
   
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void *)(12 * sizeof(float)));
    glEnableVertexAttribArray(colAttrib);


    SDL_Event windowEvent;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLint matrix_id = glGetUniformLocation(shaderProgram, "transform");
  
    while (1)
    {

        //Time
        float hour, minute, second;
        time_t t = time(0);
        struct tm buf;
        localtime_s(&buf, &t);
        hour = buf.tm_hour;
        minute = buf.tm_min;
        second = buf.tm_sec;
        

        //std::cout << hour << std::endl;
        //std::cout << minute << std::endl;
        //std::cout << second << std::endl;

        if (SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT) break;
            if (windowEvent.type == SDL_KEYUP &&
                windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
        }

        //The second and minute hands move 6 degrees every second/minute 
        //The hour hand should move 30 degrees every hour but that would involve a jump
        //Therefore we convert the current time in minutes into hours to find the angle 
        //Here 2.0f * M_PI represents 360 degrees in rad
        // (-) for clockwise rotation
        float second_angle  =  -second /60.0f * 2.0f * M_PI;
        float minute_angle =   -minute /60.0f * 2.0f * M_PI; 
        float hour_angle = -(hour / 12.0f + minute / (60.0f * 12)) * 2.0f * M_PI;

        //std::cout << hour_angle << std::endl;
        //std::cout << minute_angle << std::endl;
        //std::cout << second_angle << std::endl;


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw methods
        hourHand(hour_angle);
        minuteHand(minute_angle);
        secondHand(second_angle);
        drawClock();
       
        //sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
       
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

void hourHand(float angle) {

    glm::mat3 translate = glm::transpose(glm::mat3(1, 0, 0,
                                                   0, 1, 0,
                                                   0, 0, 0));

    glm::mat3 rotate = glm::transpose(glm::mat3(cos(angle), -sin(angle), 0,
                                               sin(angle), cos(angle), 0,
                                                0, 0, 1));

    glm::mat3 scale = glm::transpose(glm::mat3(0.05, 0, 0,
                                               0, 0.3 ,0,
                                               0, 0, 0));

    //general transformation 
    glm::mat3 transform = translate * rotate * scale;

    glUniformMatrix3fv(matrix_id, 1, GL_FALSE, &transform[0][0]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawArrays(GL_TRIANGLES, 1, 3);

}

void minuteHand(float angle) {

    glm::mat3 translate = glm::transpose(glm::mat3(1, 0, 0,
        0, 1, 0,
        0, 0, 0));

    glm::mat3 rotate = glm::transpose(glm::mat3(cos(angle), -sin(angle), 0,
        sin(angle), cos(angle), 0,
        0, 0, 1));

    glm::mat3 scale = glm::transpose(glm::mat3(0.05, 0, 0,
        0, 0.5, 0,
        0, 0, 0));

    //general transformation 
    glm::mat3 transform = translate * rotate * scale;

    glUniformMatrix3fv(matrix_id, 1, GL_FALSE, &transform[0][0]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawArrays(GL_TRIANGLES, 1, 3);

}

void secondHand(float angle) {
    glm::mat3 translate = glm::transpose(glm::mat3(1, 0, 0,
        0, 1, 0,
        0, 0, 0));

    glm::mat3 rotate = glm::transpose(glm::mat3(cos(angle), -sin(angle), 0,
        sin(angle), cos(angle), 0,
        0, 0, 1));

    glm::mat3 scale = glm::transpose(glm::mat3(0.01, 0, 0,
        0, 0.8, 0,
        0, 0, 0));

    //general transformation 
    glm::mat3 transform = translate * rotate * scale;

    glUniformMatrix3fv(matrix_id, 1, GL_FALSE, &transform[0][0]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawArrays(GL_TRIANGLES, 1, 3);
    
   

}

void drawClock() {
    float angle;
        

    for (int i = 0; i < 60; i++) {
        //5 or 15
        //5 for each 5th minute 
        //15 for each 15th minute  
        if ((i % 15) == 0) {
            angle = -i / 30.0f * M_PI;
            draw(angle, 1);
        }
        else {
            angle = -i / 30.0f * M_PI;
            draw(angle, 2);
        }
    }

}

void draw(float angle, int type) {
    glm::mat3 scale;

    glm::mat3 translate = glm::transpose(glm::mat3(1, 0, 0,
                                                   0, 1, 0,
                                                   0, 0, 0));

    glm::mat3 rotate = glm::transpose(glm::mat3(cos(angle), -sin(angle), 0,
                                                sin(angle), cos(angle), 0,
                                                0, 0, 1));


    //different scale matrix based on type of indicator
    if (type == 1) {
            scale = glm::transpose(glm::mat3(0.05, 0, 0,
                                             0, 0.3, 0.6,
                                             0, 0, 0));
    }
    else if (type == 2) {
            scale = glm::transpose(glm::mat3(0.01, 0, 0,
                                             0, 0.1, 0.8,
                                             0, 0, 1));
    }
   
    //general transformation 
    glm::mat3 transform = translate * rotate * scale;

    glUniformMatrix3fv(matrix_id, 1, GL_FALSE, &transform[0][0]);

    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);


}
