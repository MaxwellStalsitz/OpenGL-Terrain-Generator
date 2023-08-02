//Maxwell Stalsitz, 2023

//stb_image library, for image loading
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <string>

//source and shader header files, for setting up variables and shader class
#include "Source.h"
#include "Shader.h"
#include "cameraMovement.h"
#include "shaderSetup.h"
#include "input.h"
#include "maingui.h"
#include "perlinNoise.h"
#include "meshGeneration.h"

//window variable
GLFWwindow* window;

Shader* modelShader;

int init() {
    //boilerplate code, initializing glfw and setting the correct versions (modern opengl/core profile)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

    //creating the window with according width and height
    window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Rendering Engine", nullptr /*add glfwGetPrimaryMonitor() here for fullscreen*/, nullptr);

    //fail case for glfw
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //boilerplate code, setting up glfw and the input functions
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //fail case for GLAD library
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //setting application thumbnail
    GLFWimage images[1];

    char* thumbnailPath = concatenateStrings(sourceDirectory, "/Resources/images/enginethumbnail.png");
    images[0].pixels = stbi_load(thumbnailPath, &images[0].width, &images[0].height, nullptr, 4); //rgba channels

    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    char* vertexShaderPath = concatenateStrings(sourceDirectory, "/src/shaders/modelVertex.glsl");
    char* fragmentShaderPath = concatenateStrings(sourceDirectory, "/src/shaders/modelFragment.glsl");

    modelShader = new Shader(vertexShaderPath , fragmentShaderPath);

    delete[] vertexShaderPath;
    delete[] fragmentShaderPath;

    //making sure loaded images are in the proper orientation (flipping them)
    stbi_set_flip_vertically_on_load(true);
    
    //create buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // vertex buffer object
    glGenBuffers(1, &IBO); // indices/index buffer object
    glGenBuffers(1, &NBO); // normal buffer object
    glGenBuffers(1, &CBO); // color buffer object

    //creates mesh struct object along with binding buffer data
    terrainMesh = initializeMesh(gridX, gridY);

    // ------------------------------------------------------------------------

    styleInitialization();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glBindVertexArray(0);

    return 1;
}

void update() {

    float currentFrame = (float)glfwGetTime();
    limitFPS = 1.0 / frameLimit;

    //fixed delta time (because delta time is not constant)
    fixedDeltaTime += (currentFrame - lastFrame) / limitFPS;
    deltaTime = currentFrame - lastFrame;

    lastFrame = currentFrame;

    while (fixedDeltaTime >= 1.0) {
        processInput(window);

        //camera smoothing
        cameraPos = glm::mix(cameraPos, desiredPos, smoothing / 100);

        fixedDeltaTime--;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //prevents camera jerk at start
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    //window refresh
    glm::vec3 backgroundColor = getColor(glm::vec3(98, 157, 217)); //rgb
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //imgui refresh
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (paused)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //getting mouse position
    glfwGetCursorPos(window, &xpo, &ypo);

    modelShader->use();

    glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    modelShader->setMat4("view", view);

    //draw mesh
    // ------------------------------------------------------------------------

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glm::mat4 model = glm::mat4(1.0f);

    modelShader->setMat4("model", model);

    setUpShaderVariables(modelShader);

    glDrawElements(GL_TRIANGLES, terrainMesh.indices.size(), GL_UNSIGNED_INT, nullptr);

    if (paused) {
        interactiveUI();
    }

    // ------------------------------------------------------------------------

    if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

    //rendering imgui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //rendering/refreshing window
    glfwSwapInterval(0); //uncaps fps
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int WinMain()
{
    if (init() != 1)
        return -1;

    modelShader->use();

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    modelShader->setMat4("projection", projection);

    //game loop
    while (!glfwWindowShouldClose(window)) {
        update();
    }

    //imgui shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //buffer and array object deletion
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &CBO);

    //glfw shutdown
    glfwTerminate();
    return 0;
}

void centerText(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

char* concatenateStrings(const char* firstString, const char* secondString) {
    std::string concatenatedString = std::string(firstString) + std::string(secondString);
    char* result = new char[concatenatedString.length() + 1];
    std::strcpy(result, concatenatedString.c_str());
    return result;
}