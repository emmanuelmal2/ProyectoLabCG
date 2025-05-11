#include <iostream>
#include <stdlib.h>
#include <vector>
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM: OpenGL Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model loading classes
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>

// Functions
bool Start();
bool Update();

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Globals
GLFWwindow* window;
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Camara
Camera camera(glm::vec3(5.0f, 4.0f, 10.0f));


// Mouse control
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float elapsedTime = 0.0f;

// Shaders
Shader* mLightsShader;
Shader* cubemapShader;
Shader* fresnelShader;
Shader* dynamicShader;

// Models
Model* lightDummy;
Model* material_mate;
Model* material_metalico;
Model* material_plastico;
Model* material_translucido;
AnimatedModel* astronauta;
Model* satelite;
Model* estacionDentro;
Model* nave;
//Model* controles;
//Model* silla;

// Cubemap
CubeMap* mainCubeMap;

// Lights
std::vector<Light> gLights;

// Materials
Material material01;

// Light uniform setters
void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value);
void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value);
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value);
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value);

int main()
{
    if (!Start())
        return -1;

    while (!glfwWindowShouldClose(window))
    {
        if (!Update())
            break;
    }

    glfwTerminate();
    return 0;
}

bool Start() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Laboratorio - Estacion Espacial", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
    cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");
    fresnelShader = new Shader("shaders/11_fresnel.vs", "shaders/11_fresnel.fs");
    dynamicShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");
    dynamicShader->setBonesIDs(MAX_RIGGING_BONES);

    // Load models
    lightDummy = new Model("models/IllumModels/lightDummy.fbx");
    material_translucido = new Model("models/IllumModels/material_translucido.fbx");
    material_metalico = new Model("models/IllumModels/material_metalico.fbx");
    material_plastico = new Model("models/IllumModels/material_plastico.fbx");
    astronauta = new AnimatedModel("models/IllumModels/astronauta.fbx");
    satelite = new Model("models/IllumModels/satellite.fbx");
    estacionDentro = new Model("models/IllumModels/EstacionDentro.fbx");
    //controles = new Model("models/IllumModels/Controles.fbx");
    //silla = new Model("models/IllumModels/Silla.fbx");
    nave = new Model("models/IllumModels/ESTACIONESPACIAL.fbx");

    // Load cubemap
    vector<std::string> faces
    {
        "textures/cubemap/01/px.png",
        "textures/cubemap/01/nx.png",
        "textures/cubemap/01/py.png",
        "textures/cubemap/01/ny.png",
        "textures/cubemap/01/pz.png",
        "textures/cubemap/01/nz.png"
    };
    mainCubeMap = new CubeMap();
    mainCubeMap->loadCubemap(faces);

    // Configure lights
    Light light01; //Luz de la escena
    light01.Position = glm::vec3(5.0f, 15.0f, -9.0f);
    light01.Color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    light01.Power = glm::vec4(25.0f, 25.0f, 25.0f, 1.0f);
    gLights.push_back(light01);

    Light light02; // Luz alarma
    light02.Position = glm::vec3(10.0f, 2.0f, -15.0f);
    light02.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    light02.Power = glm::vec4(10.0f, 10.0f, 10.1f, 1.0f);
    gLights.push_back(light02);


    /*Light light03;
    light03.Position = glm::vec3(5.0f, 2.0f, -5.0f);
    light03.Color = glm::vec4(0.0f, 0.0f, 0.2f, 1.0f);
    light03.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
    gLights.push_back(light03);

    Light light04;
    light04.Position = glm::vec3(-5.0f, 2.0f, -5.0f);
    light04.Color = glm::vec4(0.2f, 0.2f, 0.0f, 1.0f);
    light04.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
    gLights.push_back(light04);*/


    // Configure materials
    material01.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);   // Mayor base de iluminación
    material01.diffuse = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f); // Buena reflexión difusa
    material01.specular = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);    // Mate = poca especularidad
    material01.transparency = 1.0f;
    return true;
}

bool Update() {
    // Timing
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    elapsedTime += deltaTime;

    // Parpadeo de luz de alarma (por ejemplo cada 0.5 segundos)
    float blinkingInterval = 0.5f; // 0.5 segundos
    float timeMod = fmod(currentFrame, blinkingInterval * 2.0f); // Ciclo de 1s

    if (gLights.size() > 1) { // Asegura que existe la luz de alarma
        if (timeMod < blinkingInterval) {
            gLights[1].Power = glm::vec4(10.0f); // Encendida
        }
        else {
            gLights[1].Power = glm::vec4(0.0f);  // Apagada
        }
    }


    if (elapsedTime > 1.0f / 30.0f) {
        elapsedTime = 0.0f;
    }

    // Input
    processInput(window);

    // Clear screen
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup projection and view
    glm::mat4 projection;
    glm::mat4 view;
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
    view = camera.GetViewMatrix();

    // Draw cubemap background
    {
        mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
    }


    // Materiales mate y plásticos con Phong shading
    {
        mLightsShader->use();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mLightsShader->setMat4("projection", projection);
        mLightsShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        mLightsShader->setMat4("model", model);
        // Configure lights
        mLightsShader->setInt("numLights", (int)gLights.size());
        for (size_t i = 0; i < gLights.size(); ++i) {
            // No transformar la posición de la luz aquí, manteniéndola en el espacio global
            SetLightUniformVec3(mLightsShader, "Position", i, gLights[i].Position);
            SetLightUniformVec3(mLightsShader, "Direction", i, gLights[i].Direction);
            SetLightUniformVec4(mLightsShader, "Color", i, gLights[i].Color);
            SetLightUniformVec4(mLightsShader, "Power", i, gLights[i].Power);
            SetLightUniformInt(mLightsShader, "alphaIndex", i, gLights[i].alphaIndex);
            SetLightUniformFloat(mLightsShader, "distance", i, gLights[i].distance);
        }

        mLightsShader->setVec3("eye", camera.Position);

        // Set material properties
        mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
        mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
        mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
        mLightsShader->setFloat("transparency", material01.transparency);


    }

    // Dibujar materiales metálicos y translucidos con Fresnel shading (diferente refraccion de luz)
    {
        fresnelShader->use();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        fresnelShader->setMat4("projection", projection);
        fresnelShader->setMat4("view", view);

        // Enlazar texturas para Fresnel
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material_metalico->getFirstDiffuseTextureID());
        glBindTexture(GL_TEXTURE_2D, material_translucido->getFirstDiffuseTextureID());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mainCubeMap->getID());

        fresnelShader->setInt("diffuseMap", 0);
        fresnelShader->setInt("skybox", 1);

        // Draw parte interna de la nave
        glm::mat4 estacionModel = glm::mat4(1.0f);
        estacionModel = glm::translate(estacionModel, glm::vec3(10.0f, 0.0f, -30.0f)); // Ajusta si no se ve
        estacionModel = glm::rotate(estacionModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        estacionModel = glm::scale(estacionModel, glm::vec3(2.2f, 2.2f, 2.2f)); // Escala sugerida según Blender
        fresnelShader->setMat4("model", estacionModel);
        fresnelShader->setVec3("cameraPosition", camera.Position);
        fresnelShader->setFloat("mRefractionRatio", 1.0f / 1.003f); // Aire
        fresnelShader->setFloat("_Bias", -0.2f);
        fresnelShader->setFloat("_Scale", 0.15f);
        fresnelShader->setFloat("_Power", 1.0f);
        fresnelShader->setFloat("uAlpha", 1.0f); // Opaco
        estacionDentro->Draw(*fresnelShader);

        /*
        // Controles de la nave
        glm::mat4 controlesModel = glm::mat4(1.0f);
        controlesModel = glm::translate(controlesModel, glm::vec3(10.0f, 0.0f, -30.0f)); // Ajusta si no se ve
        controlesModel = glm::rotate(controlesModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        controlesModel = glm::scale(controlesModel, glm::vec3(2.0f, 2.0f, 10.0f)); // Escala sugerida según Blender


        fresnelShader->setMat4("model", controlesModel);
        fresnelShader->setVec3("cameraPosition", camera.Position);
        fresnelShader->setFloat("mRefractionRatio", 1.0f / 1.003f); // Aire
        fresnelShader->setFloat("_Bias", -0.2f);
        fresnelShader->setFloat("_Scale", 0.15f);
        fresnelShader->setFloat("_Power", 1.0f);
        fresnelShader->setFloat("uAlpha", 1.0f); // Opaco
        controles->Draw(*fresnelShader);
        */
        /*
        glm::mat4 sillaModel = glm::mat4(1.0f);
        sillaModel = glm::translate(sillaModel, glm::vec3(10.0f, 10.0f, -30.0f)); // Ajusta si no se ve
        sillaModel = glm::rotate(sillaModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        sillaModel = glm::rotate(sillaModel, glm::radians(180.0f), glm::vec3(0, 1, 0));
        sillaModel = glm::scale(sillaModel, glm::vec3(1.0f, 1.0f, 1.0f)); // Escala sugerida según Blender
        fresnelShader->setMat4("model", sillaModel);
        fresnelShader->setVec3("cameraPosition", camera.Position);
        fresnelShader->setFloat("mRefractionRatio", 1.0f / 1.003f); // Aire
        fresnelShader->setFloat("_Bias", -0.2f);
        fresnelShader->setFloat("_Scale", 0.15f);
        fresnelShader->setFloat("_Power", 1.0f);
        fresnelShader->setFloat("uAlpha", 1.0f); // Opaco
        silla->Draw(*fresnelShader);
        */

        // Draw parte externa de la nave
        glm::mat4 naveModel = glm::mat4(1.0f);
        naveModel = glm::translate(naveModel, glm::vec3(10.0f, 0.0f, -15.0f)); // Ajusta si no se ve
        naveModel = glm::scale(naveModel, glm::vec3(0.05f) * 1.0f); // Escala sugerida según Blender
        fresnelShader->setMat4("model", naveModel);
        fresnelShader->setVec3("cameraPosition", camera.Position);
        fresnelShader->setFloat("mRefractionRatio", 1.0f / 1.003f); // Aire
        fresnelShader->setFloat("_Bias", -0.2f);
        fresnelShader->setFloat("_Scale", 0.15f);
        fresnelShader->setFloat("_Power", 1.0f);
        fresnelShader->setFloat("uAlpha", 1.0f); // Opaco
        nave->Draw(*fresnelShader);


        // Draw animated character
        glm::mat4 sateliteModel = glm::mat4(1.0f);
        sateliteModel = glm::translate(sateliteModel, glm::vec3(50.0f, 0.0f, -15.0f)); // Ajusta si no se ve
        sateliteModel = glm::scale(sateliteModel, glm::vec3(0.01f)); // Escala sugerida según Blender
        fresnelShader->setMat4("model", sateliteModel);
        fresnelShader->setVec3("cameraPosition", camera.Position);
        fresnelShader->setFloat("mRefractionRatio", 1.0f / 1.003f); // Aire
        fresnelShader->setFloat("_Bias", -0.2f);
        fresnelShader->setFloat("_Scale", 0.15f);
        fresnelShader->setFloat("_Power", 1.0f);
        fresnelShader->setFloat("uAlpha", 1.0f); // Opaco
        satelite->Draw(*fresnelShader);


        // Draw animated character
        {
            astronauta->UpdateAnimation(deltaTime);
            dynamicShader->use();
            dynamicShader->setMat4("projection", projection);
            dynamicShader->setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(3.0f, 0.0f, -3.0f));
            model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
            dynamicShader->setMat4("model", model);
            dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, astronauta->gBones);
            astronauta->Draw(*dynamicShader);
        }


    }

    // Draw light indicators
    {
        Shader basicShader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");
        basicShader.use();
        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        glm::mat4 model;
        for (size_t i = 0; i < gLights.size(); ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, gLights[i].Position);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            basicShader.setMat4("model", model);
            lightDummy->Draw(basicShader);
        }
    }

    glUseProgram(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    return true;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Polygon modes
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

// Light uniform setters implementation
void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();
    shader->setInt(uniformName.c_str(), value);
}

void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();
    shader->setFloat(uniformName.c_str(), value);
}

void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();
    shader->setVec4(uniformName.c_str(), value);
}

void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();
    shader->setVec3(uniformName.c_str(), value);
}