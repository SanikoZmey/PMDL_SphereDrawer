#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <math.h>

#include "Renderer/ShaderProgram.h"
#include "ResourceMan/ResourceManager.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define PI 3.14159265358

/*  
    Some useful constants for configuring render window, radius of a sphere to draw on, camera vertical shift, 
    orthogonal projection, and mouse sensetivity
*/
const int WIDTH = 1920, HEIGHT = 1080;
const double S_RADIUS = 0.7f;
const double camera_y_shift = 1.0f;
const float scale = 1.9;
const float aspect = (float)(WIDTH) / (float)(HEIGHT);
const float MOUSE_SENS = 0.07f;

/* Camera initial configurations */
glm::vec3 cameraPos = glm::vec3(0.0f, camera_y_shift, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); 

/* Global variables for mouse first callback, Euler angles, camera movement, and field of view*/
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = (float)WIDTH / 2.0;
float lastY = (float)HEIGHT / 2.0;
float fov = 60.0f;

/* Global variables for estimating FPS */
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

/* Some variables-indicators*/
bool firstMouse = true, left_pressed = false, p_pressed = false, save_render = false, to_clear = false;
 
/* Functions/callbacks declarations*/
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn); 
void processInput(GLFWwindow *window, std::vector<std::vector<double>>& p_to_draw);

void project_points(std::vector<std::vector<double>>& strips);
void fill_strip_vector(std::vector<std::vector<double>>& strips);
void saveImage(const char* filepath, GLFWwindow* window);

int main(int argc, char** argv)
{       
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a "Windowed full screen" window and its OpenGL context */
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "It's kinda nice to be here)", monitor, NULL);
    if (!window)
    {
        std::cout << "Failed to create a window(((\n";
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current and set all callbacks*/
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGL())
    {
        std::cout << "Can't load GLAD!\n";
        return -1;
    }
    
    std::cout << GLVersion.major << "." << GLVersion.minor << "\n";
    
    /* Some OpenGL configuration: Z-test, line smoothness, and line width */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(3.0f);

    /* Target box vertex coordinates & color atributes*/
    float vertices[] = {
        // Coordinates          // Colors
        -0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.2f, 0.6f, 0.3f,
        -0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.5f, 0.5f, 0.5f,
         0.5f,  0.5f,  0.5f,    0.5f, 0.5f, 0.5f,
        -0.5f,  0.5f,  0.5f,    0.7f, 0.3f, 0.1f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,    0.7f, 0.3f, 0.1f,
        -0.5f,  0.5f, -0.5f,    0.2f, 0.6f, 0.3f,    
        -0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.7f, 0.3f, 0.1f,

         0.5f,  0.5f,  0.5f,    0.5f, 0.5f, 0.5f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,  
         0.5f, -0.5f,  0.5f,    0.f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,    0.2f, 0.6f, 0.3f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.5f, 0.5f, 0.5f,
         0.5f,  0.5f,  0.5f,    0.5f, 0.5f, 0.5f,
        -0.5f,  0.5f,  0.5f,    0.7f, 0.3f, 0.1f,
        -0.5f,  0.5f, -0.5f,    0.2f, 0.6f, 0.3f
    };

    {
        /* Creating resource manager to work with all the resources(shaders for now)*/
        ResourceManager resourceManager(argv[0]);
        auto defaultShaderProgram = resourceManager.loadShaderProgram("DefaultShaderProgram", "resources/shaders/vertexShader.vs", "resources/shaders/fragmentShader.fs");

        /* Creating and configuring VBO and VAO to */
        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        /* Creating vectors for storing drawings and their backup*/
        std::vector<std::vector<double>> points_to_draw, t_points_to_draw;
        std::vector<std::vector<double>>& user_vertices = points_to_draw;
        fill_strip_vector(points_to_draw);

        /* Variables for counting number of saved projections and indicator of the mode of rendering(sphere, cylinder, and plane)*/
        int num_of_saved_projections = -1, render_num = 0;
        
        /* Some pre-check code that can auto-generate problems if configuration is bad =)*/
        std::string path = "../renders/", render_name = "";
        if(!std::filesystem::is_empty(path)){
            for (const auto & entry : std::filesystem::directory_iterator(path)){
                render_name = entry.path().string();
                render_name.erase(0, 17);
                render_num = std::stoi(render_name.erase(render_name.find('.'), 4));
                if(render_num > num_of_saved_projections){
                    num_of_saved_projections = render_num;
                }
            } 
        }

        /* Initial projection is perspective */
        glm::mat4 projection_t = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        /* Code for debuging(FPS metric) */
        // int fps_to_show_counter = 0;
        // double fps_sum_to_avg = 0.0f;
        
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window)) 
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            /* Code for debuging(FPS metric) */
            // if(fps_to_show_counter % 50 == 0){
            //     std::cout << "FPS:" << fps_sum_to_avg / 50 << "\n";
            //     fps_sum_to_avg = 0.0f;
            // }           
            // fps_sum_to_avg += 1.0f / deltaTime;
            // fps_to_show_counter++;

            /* Processing input */
            processInput(window, points_to_draw);

            /* Clearing the buffer */
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* Choosing the program to render */
            defaultShaderProgram->use();           

            /* If it is needed to clear the drawing and reset camera position and orientation */
            if(to_clear){
                t_points_to_draw.clear();
                points_to_draw.clear();
                fill_strip_vector(points_to_draw);
                cameraPos = glm::vec3(0.0f, camera_y_shift, 0.0f);
                cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

                projection_t = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
                to_clear = save_render = false;
            }

            if(save_render && points_to_draw[1].size() != 0){
                t_points_to_draw = points_to_draw;

                /* Transforming the coordinates of the vertices from "sphere" to "cylinder" view*/
                for (int strip_num = 1; strip_num < t_points_to_draw.size(); ++strip_num){
                    std::vector<double>* strip = t_points_to_draw.data() + strip_num;
                    for(int v_num = 0; v_num < strip->size(); v_num += 6){
                        double x = (*strip)[v_num], y = (*strip)[v_num + 1] - camera_y_shift, z = (*strip)[v_num + 2];
                        double xzPlaneDistance = glm::distance(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(x, 0.0f, z));
                        double y_angle = glm::degrees(glm::asin(y / S_RADIUS));

                        (*strip)[v_num] = x / xzPlaneDistance * S_RADIUS;
                        (*strip)[v_num + 1] = PI * S_RADIUS / 2 * y_angle / 90 + camera_y_shift;
                        (*strip)[v_num + 2] = z / xzPlaneDistance * S_RADIUS;
                    }
                }

                for (int strip_num = 1; strip_num < t_points_to_draw.size(); ++strip_num){
                    std::vector<double>* strip = t_points_to_draw.data() + strip_num;
                    for(int v_num = 6; v_num < strip->size(); v_num += 6){
                        double x_curr = (*strip)[v_num], x_prev = (*strip)[v_num - 6], y_curr = (*strip)[v_num + 1], y_prev = (*strip)[v_num - 5];;
                        double z_curr = (*strip)[v_num + 2], z_prev = (*strip)[v_num - 4];
                        int x_curr_sgn = int(std::signbit(x_curr)), x_prev_sgn = int(std::signbit(x_prev));
                        if(z_curr > 0.00001f && x_curr_sgn + x_prev_sgn == 1){
                            double z_inter = -x_curr * (z_prev - z_curr) / (x_prev - x_curr) + z_curr;
                            double y_inter = -x_curr * (y_prev - y_curr) / (x_prev - x_curr) + y_curr;

                            if(x_prev_sgn){
                                strip->insert(strip->begin() + v_num, -0.000001f);
                            }
                            else{
                                strip->insert(strip->begin() + v_num, 0.000001f);
                            }
                            strip->insert(strip->begin() + v_num + 1, y_inter);
                            strip->insert(strip->begin() + v_num + 2, z_inter);
                            strip->insert(strip->begin() + v_num + 3, (*strip)[v_num - 3]);
                            strip->insert(strip->begin() + v_num + 4, (*strip)[v_num - 2]);
                            strip->insert(strip->begin() + v_num + 5, (*strip)[v_num - 1]);
                            
                            std::vector<double> strip_piece(strip->begin() + v_num, strip->end());
                            strip_piece[0] *= -1.0f;

                            while(strip->size() != v_num + 6){
                                strip->pop_back();
                            }
                            t_points_to_draw.insert(t_points_to_draw.begin() + strip_num + 1, strip_piece);
                        }                           
                    }
                }
                
                /* At this moment all vertices are preprocessed and can be projected on a plane*/
                project_points(t_points_to_draw);
                
                /* Configuring and applying orthogonal projection to "take a photo" of a drawing */
                projection_t = glm::ortho(-aspect * scale, aspect * scale, -scale, scale, 0.0f, 1.0f);
                cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

                user_vertices = t_points_to_draw;
            }

            /* Creating transformaiton matrices: model->world, world->view, view->crop&projection */
            glm::mat4 world_t = glm::mat4(1.0f); 
            glm::mat4 view_t = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            
            /* "Moving" the target box a little bit further from the camera*/
            world_t = glm::translate(world_t, glm::vec3(0.0f, 0.4f, -3.0f));

            defaultShaderProgram->setMat4("world_t", world_t);
            defaultShaderProgram->setMat4("view_t", view_t);
            defaultShaderProgram->setMat4("projection_t", projection_t);

            /* Binding vertex and atribute buffers and confiuring them to work with box model */
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

            /* Render target box */
            glDrawArrays(GL_TRIANGLES, 0, 36);

            /* Reset model->world transformation matrix for rendering drown lines */
            defaultShaderProgram->setMat4("world_t", glm::mat4(1.0f));

            bool floor_render = true;

            /* Loop over the all... things to render (floor and line pieces) */
            for (std::vector<double> strip : user_vertices){
                if(strip.size() != 0){
                    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * strip.size(), strip.data(), GL_DYNAMIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6*sizeof(double), (void*)0);
                    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6*sizeof(double), (void*)(3*sizeof(double)));

                    if(floor_render){
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, strip.size() / 6);
                        floor_render = false;
                    }
                    else{
                        glDrawArrays(GL_LINE_STRIP, 0, strip.size() / 6);
                    } 
                }
            }

            if(save_render)
            {
                /* Name(path) of the rendered image*/
                std::string render_name = "../renders/render" + std::to_string(++num_of_saved_projections) + ".png";
                
                saveImage(render_name.c_str(), window);
                
                to_clear = true;
            }

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= MOUSE_SENS;
    yoffset *= MOUSE_SENS;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
} 

void saveImage(const char* filepath, GLFWwindow* window) {
 int width, height;
 glfwGetFramebufferSize(window, &width, &height);
 GLsizei nrChannels = 3;
 GLsizei stride = nrChannels * width;
 GLsizei bufferSize = stride * height;
 std::vector<char> buffer(bufferSize);
 glPixelStorei(GL_PACK_ALIGNMENT, 4);
 glReadBuffer(GL_BACK);
 glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
 stbi_flip_vertically_on_write(true);
 stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
}

void fill_strip_vector(std::vector<std::vector<double>>& strips){
    /* Just add the floor to a scene for more comfortable orientation in the environment*/
    std::vector<double> floor{
         10.0f, -0.1f,  10.0f,      0.0f, 0.0f, 0.0f,
         10.0f, -0.1f, -10.0f,      0.5f, 0.5f, 0.5f,
        -10.0f, -0.1f, -10.0f,      1.0f, 1.0f, 1.0f,
        -10.0f, -0.1f,  10.0f,      0.5f, 0.5f, 0.5f,
         10.0f, -0.1f,  10.0f,      0.0f, 0.0f, 0.0f
    }, strip;
    strips.push_back(floor);
    strips.push_back(strip);
}

void project_points(std::vector<std::vector<double>>& strips){
    /* Unfold the surface of a cylinder */
    for (int strip_num = 1; strip_num < strips.size(); ++strip_num){
        std::vector<double>* strip = strips.data() + strip_num;
        for(int v_num = 0; v_num < strip->size(); v_num += 6){
            double x = (*strip)[v_num], z = (*strip)[v_num + 2], z_asin = glm::asin(-z / S_RADIUS);
            if(!std::signbit(x)){
                (*strip)[v_num] = PI / 2 - z_asin;
            }
            else if(x != 0.0f){
                (*strip)[v_num] = -PI / 2 + z_asin;
            }
            (*strip)[v_num + 2] = -S_RADIUS;                          
        }
    } 
}

void processInput(GLFWwindow *window, std::vector<std::vector<double>>& p_to_draw){
    /* Esc - exit the program*/
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        // saveImage("render.png", window);
        glfwSetWindowShouldClose(window, true);
    }
    /* Left mouse button - draw*/
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        bool push = true;
        double x_pos, y_pos, z_pos, dir_lenght;

        left_pressed = true;
        dir_lenght = glm::length(cameraFront);
        x_pos = cameraPos.x + cameraFront.x / dir_lenght * S_RADIUS;
        y_pos = cameraPos.y + cameraFront.y / dir_lenght * S_RADIUS;
        z_pos = cameraPos.z + cameraFront.z / dir_lenght * S_RADIUS; 
        std::vector<double>& last_strip = p_to_draw[p_to_draw.size() - 1];
        if(!last_strip.empty()){
            push = false;
            double last_x, last_y, last_z;
            last_x = last_strip[last_strip.size() - 6];
            last_y = last_strip[last_strip.size() - 5];
            last_z = last_strip[last_strip.size() - 4];
            /* Calculating distance to not to spam same vertices in the strip */
            double distance = glm::distance(glm::vec3(x_pos, y_pos, z_pos), glm::vec3(last_x, last_y, last_z));
            if (distance > 0.0001f){
                push = true;
            }
        }
        if(push){
            last_strip.push_back(x_pos); // X
            last_strip.push_back(y_pos); // Y
            last_strip.push_back(z_pos); // Z

            last_strip.push_back(1.f); // R
            last_strip.push_back(1.0f); // G
            last_strip.push_back(1.0f); // B
        }
    }
    /* If the left mouse button was released - create a new line strip */
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && left_pressed){
        left_pressed = false;
        std::vector<double> new_strip;
        p_to_draw.push_back(new_strip);
    }
    /* If P button was pressed and released - change perspective(mode - will be changed)*/
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
       p_pressed = true; 
    }
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && p_pressed){
       p_pressed = false;
       save_render = true; 
    }
    /* If C button pressed - reset the scene*/
    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
        to_clear = true;
    }
    
    /* Speed of the camera movement depends not on the computer performance... But on... FPS? */
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}