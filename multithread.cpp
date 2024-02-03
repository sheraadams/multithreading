// library includes
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
// learnopengl resources includes
#include "shader.h"
#include "camera.h"
#include "model.h"
// shapes
#include "objects.h"
#include "texture.h"
#include "geometry.h"
#include "pen_accent.h"
#include "pen_body.h"
#include "pen_clip.h"
#include "pen_point.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <iostream>

template<typename Function, typename... Args>
/* FUNCTIONS */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void updateGUI();
void processAudio();
void renderScene(Shader shader);

int t = 0;
int r = 0;
std::vector<unsigned int> textures;
float x = -.0f;
float y = -1.3f; // we are hiding the box in the desk for now
float z = -3.2f; // we do this setting y-position to -1.3 and the z position to -3.2
float xx = 0.0f;
float yy = 0.0f;
float zz = 0.0f;

/* VARIABLES */
float speed = 45.0f; // light orbiting speed
float direction = -1.0; // used for light orbiting direction
bool lampIsOrbiting = true;
bool Keys[1024], firstMouse = true, onPerspective = true;
float SCR_WIDTH = 1000, SCR_HEIGHT = 900;
float lastX = (float)SCR_WIDTH / 2.0, lastY = (float)SCR_HEIGHT / 2.0;
float lastFrame = 0.0f, deltaTime = 0.0f;
GLfloat xoffset = 0.0f, yoffset = 0.0f;
/* VARIABLES */
bool usingCamera = true;
// initialize the camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

/* FUNCTION TO MAKE THE PROGRAM FULLSCREEN */

int main()
{
    /* GLFW INITIALIZE */
    glfwInit();
    // set the version to be used, I am using version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create the glfw window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shera Adams: Final Project", NULL, NULL);
    // output error message if the window fails to create
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // make the window the current context for the openGL application
    glfwMakeContextCurrent(window);
    // tell glfw to capture the mouse movement within the window and scroll movement within the window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // changed to glfw_cursor_normal form glfw_cursor_disabled
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // load openGL pointers and glad pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Query and print OpenGL version
    const GLubyte* version = glGetString(GL_VERSION);
    if (version) {
        std::cout << "OpenGL Version: " << version << std::endl;
    } else {
        std::cerr << "Failed to get OpenGL version" << std::endl;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark(); // Use the dark theme,
    // Initialize ImGui for GLFW and OpenGL3

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    glEnable(GL_DEPTH_TEST);

    /* LOAD SHADERS FROM FILES */
    Shader lightingShader("specular.vs", "specular.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");



    /* VERTICES */
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,   // this is a modufied cube z axis is my x axis and it is modified to be less tall
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,  // left
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // right
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // back
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  // right
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  // top
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  //bottom
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };


    // light box positions
    glm::vec3 lightPositions[] = {
            glm::vec3(-3.0f, 0.0f, 3.0f),
            glm::vec3(-3.0f, 0.0f, -3.0f),
    };
    // lighting positions for our scene
    glm::vec3 pointLightPositions[] = {
            glm::vec3(2.0f, 1.0f, 2.0f),
            glm::vec3(1.0f, 2.0f, -3.0f),
            glm::vec3(-3.0f, 1.0f, -2.0f),
            glm::vec3(1.0f, 1.0f, 2.0f),
            glm::vec3(-0.0f, 2.0f, -2.0f),
            glm::vec3(2.0f, 2.0f, 3.0f),
            glm::vec3(0.0f, 3.0f, -2.0f),
            glm::vec3(-2.0f, 1.0f, -2.0f),
            glm::vec3(1.0f, 2.0f, -3.0f),
            glm::vec3(2.0f, 1.0f, 2.0f),
            glm::vec3(3.0f, 1.0f, 0.0f),
            glm::vec3(2.0f, 1.0f, 2.0f),
            glm::vec3(2.0f, 1.0f, -2.0f),
            glm::vec3(-1.0f, 2.0f, 2.0f),
            glm::vec3(2.0f, 1.0f, 2.0f),
            glm::vec3(3.0f, 3.0f, -5.0f),
            glm::vec3(0.0f, 2.0f, 2.0f),
            glm::vec3(2.0f, 2.0f, 3.0f),
    };

    Textures texture;
    unsigned int redTexture = texture.loadTexture("resources/textures/class/red.jpg");
    unsigned int blueTexture = texture.loadTexture("resources/textures/class/blue.jpg");
    textures.push_back(redTexture);
    textures.push_back(blueTexture);
    
    // set the shader ints
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    onPerspective = true;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        // directional light
        lightingShader.setVec3("dirLight.direction", 0.2f, 0.0f, 0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        float ambient[] = { 0.5f, 0.5f, 0.5f, 1 };
        float diffuse[] = { 0.8f, 0.8f, 0.8f, 1 };
        float specular[] = { 1.0f, 1.0f, 1.0f, 1 };
        float shininess = 128;
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);


        std::thread sceneThread(renderScene);
        std::thread audioThread(processAudio);
        std::thread guiThread(updateGUI);
        
        sceneThread.join();
        audioThread.join();
        guiThread.join();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    /* DELETE VAOS AND CLEAR MEMORY */
    Objects skybox;
    skybox.clear();
    Objects cube;
    cube.clear();
    Objects box;
    box.clear();
    Objects pyramid;
    pyramid.clear();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // clear textures
    texture.destroyTexture(blueTexture);
    texture.destroyTexture(redTexture);
    glDeleteProgram(lightingShader.ID);
    glDeleteProgram(skyboxShader.ID);


    glfwTerminate();
    return 0;
}

/* PROCESS INPUT */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS))
    {
        if (!usingCamera)
            usingCamera = true;
        else
            usingCamera = false;
    }
}

/* CALLBACKS */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (usingCamera)
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
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// function to increase or decrease camera speed depending on the scroll direction
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
    // increase camera speed
    if (yoffset > 0)
        camera.MovementSpeed += 1.0f;
        // decrease camera speed
    else
        camera.MovementSpeed -= 1.0f;
}


void renderScene(Shader lightingShader) {

    std::this_thread::sleep_for(std::chrono::seconds(2));
  
    Geometry geometry;
    std::vector<GLfloat>vertices = geometry.GetCubeVertices();
    std::vector<GLfloat>compassVertices = geometry.GetCompassVertices();
    std::vector<GLfloat>pyramidVertices = geometry.GetBoxVertices();
    std::vector<GLfloat>boxVertices = geometry.GetBoxVertices();
    std::vector<GLfloat>skyboxVertices = geometry.GetSkyboxVertices();
    std::vector<GLfloat>planeVertices = geometry.GetPlaneVertices();
    
    /* INITIALIZE VARAIBLES */
    glm::mat4 projection, view, model;
    if (onPerspective)
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    if (!onPerspective)
        projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, 1.0f, 100.0f);

    /* SET SHADER */
    view = camera.GetViewMatrix();
    lightingShader.use();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    lightingShader.setMat4("model", model);
    
    /* RENDER SCENE */
    // bind metal texture and use it on the next three objects
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, metalTexture);

    // set model and translate the next three objects down 2 and back 3
    model = glm::mat4(0.5f);
    model = glm::translate(model, glm::vec3(0.0f, -2.005f, -3.80f));

    // draw the penbody shape
    lightingShader.setMat4("model", model);
    PenBody penBody;
    penBody.Draw();

    // bind the ballpoint texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ballpointTexture);


    /* TESTING: ADDING AND MOVING NEW OBJECTS WITH SIMS MODE */
    Objects book1;
    book1.link(sizeof(vertices), vertices);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::scale(model, glm::vec3(xx + 1, yy + 1, zz + 1));
    lightingShader.setMat4("model", model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[r]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    /* RENDER LIGHTS */
    // bind the light uniform buffer object and the lightcube vao
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindVertexArray(lightCubeVAO);
    // set the speed of the light movement
    // iterate through two draw loops binding the light cube vao twice in two different positions


    /* RENDER SKYBOX */
    glDepthFunc(GL_LEQUAL);
    Objects skybox;
    skybox.skybox(sizeof(skyboxVertices), skyboxVertices);
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skybox.bindSkybox();
    glDepthFunc(GL_LESS);

    std::cout << "Scene rendering complete." << std::endl;
}


void processAudio() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Audio processing complete." << std::endl;
}

void updateGUI() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowDemoWindow(); // Show demo window! :)

    // Create an ImGui window
    ImGui::SetNextWindowSize(ImVec2(350, 530));
    ImGui::SetNextWindowPos(ImVec2(560, 470));
    ImGui::Begin("Basic Design Controls");

    /* COLORS  */
    //ImVec4 blueColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); //
    ImVec4 blueColor = ImVec4(1.0f, 1.f, 1.0f, 1.0f); //
    ImVec4 greenColor = ImVec4(0, 1, 0, 1);

    /* CENTER IMGUI */
    float windowWidth = ImGui::GetWindowContentRegionWidth();
    float buttonWidth = ImGui::CalcTextSize("Print Coordinates").x;
    float xPosition = (windowWidth - buttonWidth) * 0.5f;

    /* Position data */
    ImGui::TextColored(blueColor, "Position Controls:");
    // ImGui::TextColored(blueColor, "left, right, up, down, y, z");
    /* sliders */
    ImGui::SliderFloat("X", &x, -4.0f, 10.0f);
    ImGui::SliderFloat("Y", &y, -4.0f, 10.0f);
    ImGui::SliderFloat("Z", &z, -10.0f, 10.0f);
    ImGui::TextColored(ImVec4(0, 1, 0, 1), "x: %.2f   y: %.2f   z: %.2f", x, y, z);
    ImGui::Separator();
    ImGui::Text("");

    /* scale data */
    ImGui::TextColored(blueColor, "Scale Controls:");
    // ImGui::TextColored(blueColor, "j, l, i, k");
    /* sliders */
    ImGui::SliderFloat("XX", &xx, -4.0f, 10.0f);
    ImGui::SliderFloat("YY", &yy, -4.0f, 10.0f);
    ImGui::SliderFloat("ZZ", &zz, -10.0f, 10.0f);
    ImGui::TextColored(ImVec4(0, 1, 0, 1), "xx: %.2f   yy: %.2f   zz: %.2f", xx, yy, zz);
    ImGui::Separator();
    ImGui::Text("");


    /* Dropdown: camera */
    int cameraChoice = usingCamera ? 0 : 1;
    ImGui::Text("Using Camera:");
    const char* cameraChoices[] = { "True (Using Camera)", "False (Not Using Camera)" };
    const int numCameraChoices = sizeof(cameraChoices) / sizeof(cameraChoices[0]);
    // Create a dropdown to select between "True (Using Camera)" and "False (Not Using Camera)"
    if (ImGui::Combo("##CameraCombo", &cameraChoice, cameraChoices, numCameraChoices)) {
        // Update usingCamera based on the selected choice
        usingCamera = (cameraChoice == 0);
    }

    ImGui::Separator();
    ImGui::Text("");

    /* texture slider*/
    ImGui::TextColored(blueColor, "Texture Controls:");
    //ImGui::TextColored(blueColor, "r, t");
    if (ImGui::SliderInt("Texture 1", &t, 0, textures.size() - 1)) {
        // t is  updated via the slider
        if (t < 0) {
            t = textures.size() - 1;
        }
        else if (t >= textures.size()) {
            t = 0;
        }
    }
    
    ImGui::Separator();
    ImGui::Text("");

    /* print coordinates to console button */
    // center the button
    ImGui::SetCursorPosX(xPosition);
    if (ImGui::Button("Print Coordinates"))
    {
        std::cout << "x: " << x << "\ny: " << y << "\nz: " << z << std::endl;
        std::cout << "xx: " << xx << "\nyy: " << yy << "\nzz: " << zz << std::endl;
        std::cout << endl;
    }

    ImGui::End(); // End ImGui window
    // process input throughout the loop
    processInput(window);

    // set the background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    std::cout << "GUI updated." << std::endl;
    
}
template<typename Function, typename... Args>
void runAsync(Function&& func, Args&&... args) {
    std::async(std::launch::async, std::forward<Function>(func), std::forward<Args>(args)...);
}
