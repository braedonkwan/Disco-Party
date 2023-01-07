#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "include/tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "include/shader.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
std::vector<float> loadmesh(const char *filepath);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "A3", NULL, NULL);
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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader shader("src/vertex_shader", "src/fragment_shader");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> floor;
    floor = loadmesh("assets/floor.obj");

    std::vector<float> bucket;
    bucket = loadmesh("assets/bucket.obj");

    std::vector<float> timmy;
    timmy = loadmesh("assets/timmy.obj");

    // first, configure the mesh's VAO (and VBO)
    unsigned int VBO, floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, floor.size() * sizeof(float), &floor[0], GL_STATIC_DRAW);
    glBindVertexArray(floorVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int bucketVAO;
    glGenVertexArrays(1, &bucketVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, bucket.size() * sizeof(float), &bucket[0], GL_STATIC_DRAW);
    glBindVertexArray(bucketVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int timmyVAO;
    glGenVertexArrays(1, &timmyVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, timmy.size() * sizeof(float), &timmy[0], GL_STATIC_DRAW);
    glBindVertexArray(timmyVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int floorTexture = loadTexture("assets/floor.jpg");
    unsigned int bucketTexture = loadTexture("assets/bucket.jpg");
    unsigned int timmyTexture = loadTexture("assets/timmy.png");

    // shader configuration
    // --------------------
    shader.use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(60.0), 4.0 / 3.0, 0.1, 1000.0);
    glm::mat4 view = glm::lookAt(glm::vec3(50.0, 100.0, 200.0), glm::vec3(0.0, 80.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0);
    shader.setMat4("model", model);

    // light properties 1
    glm::vec3 dir1 = glm::vec3(50.0, -200.0, -50.0);
    shader.setVec3("light1.position", 0.0, 200.0, 0.0);
    shader.setVec3("light1.direction", dir1);
    shader.setVec3("light1.ambient", 0.2, 0.2, 0.2);
    shader.setVec3("light1.diffuse", 1.0, 0.0, 0.0);
    shader.setFloat("light1.cutOff", glm::cos(M_PI / 6.0));
    shader.setFloat("light1.constant", 1.0);
    shader.setFloat("light1.linear", 0.000035);
    shader.setFloat("light1.quadratic", 0.000044);

    // light properties 2
    glm::vec3 dir2 = glm::vec3(-50.0, -200.0, -50.0);
    shader.setVec3("light2.position", 0.0, 200.0, 0.0);
    shader.setVec3("light2.direction", dir2);
    shader.setVec3("light2.ambient", 0.2, 0.2, 0.2);
    shader.setVec3("light2.diffuse", 0.0, 1.0, 0.0);
    shader.setFloat("light2.cutOff", glm::cos(M_PI / 6.0));
    shader.setFloat("light2.constant", 1.0);
    shader.setFloat("light2.linear", 0.000035);
    shader.setFloat("light2.quadratic", 0.000044);

    // light properties 3
    glm::vec3 dir3 = glm::vec3(0.0, -200.0, 50.0);
    shader.setVec3("light3.position", 0.0, 200.0, 0.0);
    shader.setVec3("light3.direction", dir3);
    shader.setVec3("light3.ambient", 0.2, 0.2, 0.2);
    shader.setVec3("light3.diffuse", 0.0, 0.0, 1.0);
    shader.setFloat("light3.cutOff", glm::cos(M_PI / 6.0));
    shader.setFloat("light3.constant", 1.0);
    shader.setFloat("light3.linear", 0.000035);
    shader.setFloat("light3.quadratic", 0.000044);

    float theta = 0.0;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.3, 0.4, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        shader.use();

        glm::mat4 rotationMatrix = glm::mat4(1.0);
        glm::vec4 rotationVector = glm::vec4(dir1, 1.0);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(theta), glm::vec3(0.0, 1.0, 0.0));
        rotationVector = rotationMatrix * rotationVector;
        shader.setVec3("light1.direction", rotationVector[0], rotationVector[1], rotationVector[2]);

        rotationVector = glm::vec4(dir2, 1.0);
        rotationVector = rotationMatrix * rotationVector;
        shader.setVec3("light2.direction", rotationVector[0], rotationVector[1], rotationVector[2]);

        rotationVector = glm::vec4(dir3, 1.0);
        rotationVector = rotationMatrix * rotationVector;
        shader.setVec3("light3.direction", rotationVector[0], rotationVector[1], rotationVector[2]);

        // bind texture
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        // render the mesh
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, floor.size());

        // bind texture
        glBindTexture(GL_TEXTURE_2D, bucketTexture);

        // render the mesh
        glBindVertexArray(bucketVAO);
        glDrawArrays(GL_TRIANGLES, 0, bucket.size());

        // bind texture
        glBindTexture(GL_TEXTURE_2D, timmyTexture);

        // render the mesh
        glBindVertexArray(timmyVAO);
        glDrawArrays(GL_TRIANGLES, 0, timmy.size());

        theta += 0.05;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &bucketVAO);
    glDeleteVertexArrays(1, &timmyVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

std::vector<float> loadmesh(const char *filepath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    // tinyobj load obj
    std::string warn, err;
    bool bTriangulate = true;
    bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                 filepath, nullptr, bTriangulate);
    if (!bSuc)
    {
        std::cout << "tinyobj error: " << err.c_str() << std::endl;
        std::vector<float> empty;
        return empty;
    }

    std::vector<float> vertices;
    for (auto id : shapes[0].mesh.indices)
    {
        int vid = id.vertex_index;
        int nid = id.normal_index;
        int tid = id.texcoord_index;
        vertices.push_back(attrib.vertices[vid * 3]);
        vertices.push_back(attrib.vertices[vid * 3 + 1]);
        vertices.push_back(attrib.vertices[vid * 3 + 2]);
        vertices.push_back(attrib.normals[nid * 3]);
        vertices.push_back(attrib.normals[nid * 3 + 1]);
        vertices.push_back(attrib.normals[nid * 3 + 2]);
        vertices.push_back(attrib.texcoords[tid * 2]);
        vertices.push_back(attrib.texcoords[tid * 2 + 1]);
    }
    return vertices;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}