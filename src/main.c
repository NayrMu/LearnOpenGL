#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "stb_image.h"
#include "Shader.h"
#include "vMaths.h"
#include "randGen.h"
#include "Voxel.h"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
double calculateFPS(double elapsedTime, int *frameCount);
void loadVectors(int axisSize, int offsetX, int offsetZ, struct chunk* chunk, struct v4* transform);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int p[512];

struct chunk chunk1;
unsigned int seed = 3125123;
#define chunkSize 4096
int axisSize = 16;
struct v4 transforms[chunkSize];
int worldWidth = 15;
int worldRad;
struct chunk chunks[225];
struct v4 allTransforms[225][chunkSize];


struct v3 cameraPos = {8.0f, 50.0f, 8.0f};
struct v3 cameraFront = {0.0f, 0.0f, -1.0f};
struct v3 cameraTarget = {0.0f, 0.0f, 0.0f};
struct v3 lookDirection;
struct v3 xAxis = {1.0f, 0.0f, 0.0f};
struct v3 yAxis = {0.0f, 1.0f, 0.0f};
struct v3 zAxis = {0.0f, 0.0f, 1.0f};
struct v3 lookVec;

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

float currentTime;
float elapsedTime;
float lastTime;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char *vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "\n"
    "out vec2 TexCoord;\n"
    "\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";
const char *fragmentShaderSouce = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec2 TexCoord;\n"
    "\n"
    "uniform sampler2D ourTexture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n";

int main() {
  initPerlin(p);
  
  int worldSize = (worldWidth * worldWidth);
  int worldRad = worldWidth / 2;
  for (int i = 0; i < worldSize; ++i) {
    chunks[i] = chunk1;
    for (int j = 0; j < chunkSize; ++j) {
      allTransforms[i][j] = transforms[j];
    }
  }
  

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
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
  
  glfwSetInputMode(window, GLFW_REPEAT, GLFW_FALSE);
  // Variables for FPS calculation
  float startTime = glfwGetTime();
  lastTime = startTime;
  int frameCount = 0;

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    return -1;
  }

  // build and compile our shader program
  // ------------------------------------
  Shader shader = shader_create(vertexShaderSource, fragmentShaderSouce);
  
  
  

  

  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f
  };
  
  
  
  
  int gridSize = worldWidth; // Adjust this according to your requirements

  for (int row = 0; row < gridSize; row++) {
      for (int col = 0; col < gridSize; col++) {
          int offsetX = (col -worldRad) * axisSize;  // Set the x-offset based on column
          int offsetZ = (row -worldRad) * axisSize;  // Set the z-offset based on row

          // Load vectors for the current chunk with the calculated offsets
          loadVectors(axisSize, offsetX, offsetZ, &chunks[row * gridSize + col], &allTransforms[row * gridSize + col][0]);
      }
  }

  unsigned int num1 = NoiseGen(1000, 9999, 1234567);
  unsigned int num2 = NoiseGen(1000, 9999, 9964921);
  unsigned int num3 = NoiseGen(1000, 9999, 92345678);
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Set up VBO for instance data
  /*unsigned int instanceVBO;
  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(instanceTransforms), instanceTransforms, GL_STATIC_DRAW);

  // Set up VAO for instance data
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1); // This makes sure the attribute advances once per instance*/

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other VAOs requires a
  // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
  // VBOs) when it's not directly necessary. glBindVertexArray(0);

  // bind the VAO (it was already bound, but just to demonstrate): seeing as we
  // only have a single VAO we can just bind it beforehand before rendering the
  // respective triangle; this is another approach.
  glBindVertexArray(VAO);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("C:\\Users\\rkmun\\source\\repos\\LearnOpenGL\\src\\grass.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture\n%s", stbi_failure_reason());
  }
  stbi_image_free(data);

  unsigned int textureDirt;
  glGenTextures(1, &textureDirt);
  glBindTexture(GL_TEXTURE_2D, textureDirt);
  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  unsigned char *dataDirt =
      stbi_load("C:\\Users\\rkmun\\source\\repos\\LearnOpenGL\\src\\dirt.jpg", &width, &height, &nrChannels, 0);
  if (dataDirt) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, dataDirt);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture\n%s", stbi_failure_reason());
  }
  stbi_image_free(dataDirt);

  unsigned int textureStone;
  glGenTextures(1, &textureStone);
  glBindTexture(GL_TEXTURE_2D, textureStone);
  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  unsigned char *dataStone =
      stbi_load("C:\\Users\\rkmun\\source\\repos\\LearnOpenGL\\src\\stone.jpg", &width, &height, &nrChannels, 0);
  if (dataStone) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, dataStone);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture\n%s", stbi_failure_reason());
  }
  stbi_image_free(dataStone);

  unsigned int textureIron;
  glGenTextures(1, &textureIron);
  glBindTexture(GL_TEXTURE_2D, textureIron);
  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  unsigned char *dataIron =
      stbi_load("C:\\Users\\rkmun\\source\\repos\\LearnOpenGL\\src\\iron.jpg", &width, &height, &nrChannels, 0);
  if (dataIron) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, dataIron);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture\n%s", stbi_failure_reason());
  }
  stbi_image_free(dataIron);

  

  
  struct mat4 projection = makeIdentityMatrix();
    makeProjection(45.0f, 0.1f, 100.0f, &projection, (float)SCR_WIDTH,
                   (float)SCR_HEIGHT);
  
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    
    
    // input
    // -----
    processInput(window);
    

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);\
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < worldSize; i++) {
      int chunkOffsetX = chunks[i].offsetX / axisSize;
      int playerOffsetX = ((int)(cameraPos.x / axisSize));
      int chunkOffsetZ = chunks[i].offsetZ / axisSize;
      int playerOffsetZ = ((int)(cameraPos.z / axisSize));
      int newOffsetX;
      int newOffsetZ;
      if (abs(playerOffsetX - chunkOffsetX) > worldRad) {
        if (chunkOffsetX > (int)(cameraPos.x / axisSize)) {
          newOffsetX = ((int)(cameraPos.x / axisSize) - worldRad) * axisSize;
        }
        else {
          newOffsetX = (playerOffsetX + worldRad) * axisSize;
        }
        loadVectors(axisSize, newOffsetX, chunks[i].offsetZ, &chunks[i], &allTransforms[i][0]);
      }
      else if (abs(playerOffsetZ - chunkOffsetZ) > worldRad) {
        if (chunkOffsetZ > (int)(cameraPos.z / axisSize)) {
          newOffsetZ = ((int)(cameraPos.z / axisSize) - worldRad) * axisSize;
        }
        else {
          newOffsetZ = (playerOffsetZ + worldRad) * axisSize;
        }
        loadVectors(axisSize, chunks[i].offsetX, newOffsetZ, &chunks[i], &allTransforms[i][0]);
      }
      
    }
    

    // bind Texture
    
    
    struct mat4 view = makeIdentityMatrix();
    
    addVectors(cameraPos, cameraFront, &lookVec);
    makeLookAtMatrix(cameraPos, lookVec, yAxis, &view);
    
    int viewLoc = glGetUniformLocation(shader.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &view.m);
    int projectionLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, &projection.m);
    
    // render container
    shader_use(&shader);
    
    
    glBindVertexArray(VAO);
    for (int i = 0; i < worldSize; i++) {
      for(unsigned int j = 0; j < chunkSize; j++)
      {
        if (allTransforms[i][j].w != -1) {
          if (allTransforms[i][j].w == 1) {
            glBindTexture(GL_TEXTURE_2D, texture);
          }
          else if (allTransforms[i][j].w == 2) {
            glBindTexture(GL_TEXTURE_2D, textureDirt);
          }
          else if (allTransforms[i][j].w == 3) {
            glBindTexture(GL_TEXTURE_2D, textureStone);
          }
          else if (allTransforms[i][j].w == 4) {
            glBindTexture(GL_TEXTURE_2D, textureIron);
          }
          struct mat4 model = makeIdentityMatrix();
            
          struct v3 renderVector = {allTransforms[i][j].x + chunks[i].offsetX, allTransforms[i][j].y , allTransforms[i][j].z + chunks[i].offsetZ};
          translate(renderVector, &model);
          int modelLoc = glGetUniformLocation(shader.ID, "model");
          
          glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &model.m);

          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      }
    }




    frameCount++;

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapInterval(0);
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Calculate elapsed time
    currentTime = glfwGetTime();
    elapsedTime = currentTime - lastTime;

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;  
    

    // Update FPS every second
    if (elapsedTime >= 1.0) {
      double fps = calculateFPS(elapsedTime, &frameCount);
      printf("FPS: %.2f\n", fps);

      // Reset counters
      lastTime = currentTime;
    }
  }

  // Calculate average FPS
  double endTime = glfwGetTime();
  double totalElapsedTime = endTime - startTime;
  double averageFPS = calculateFPS(totalElapsedTime, &frameCount);

  // Print average FPS
  printf("Average FPS: %.2f\n", averageFPS);

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(&shader);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(window, 1);
  }
  float cameraSpeed = 20.0f * deltaTime; // adjust accordingly
  struct v3 vecA;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    VscalarMulitply(cameraSpeed, cameraFront, &vecA);
    addVectors(cameraPos, vecA, &cameraPos);
  }
    //normalize3d(&cameraPos);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    VscalarMulitply(cameraSpeed, cameraFront, &vecA);
    subtractVectors(cameraPos, vecA, &cameraPos);
  }
    //normalize3d(&cameraPos);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    crossProduct(cameraFront, yAxis, &vecA);
    normalize3d(&vecA);
    VscalarMulitply(cameraSpeed, vecA, &vecA);
    subtractVectors(cameraPos, vecA, &cameraPos);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    crossProduct(cameraFront, yAxis, &vecA);
    normalize3d(&vecA);
    VscalarMulitply(cameraSpeed, vecA, &vecA);
    addVectors(cameraPos, vecA, &cameraPos);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    cameraSpeed = 2.5f * deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    crossProduct(cameraFront, xAxis, &vecA);
    normalize3d(&vecA);
    VscalarMulitply(cameraSpeed, vecA, &vecA);
    subtractVectors(cameraPos, vecA, &cameraPos);
  }
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    
    initPerlin(p);
    int gridSize = worldWidth; // Adjust this according to your requirements

    for (int row = 0; row < gridSize; row++) {
      for (int col = 0; col < gridSize; col++) {
        int offsetX = (col -worldRad) * axisSize;  // Set the x-offset based on column
        int offsetZ = (row -worldRad) * axisSize;  // Set the z-offset based on row

        // Load vectors for the current chunk with the calculated offsets
        loadVectors(axisSize, offsetX, offsetZ, &chunks[row * gridSize + col], &allTransforms[row * gridSize + col][0]);
      }
    }
  }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    struct v3 direction;
    direction.x = cos(yaw * (3.14159 / 180)) * cos(pitch * (3.14159 / 180));
    direction.y = sin(pitch * (3.14159 / 180));
    direction.z = sin(yaw * (3.14159 / 180)) * cos(pitch * (3.14159 / 180));
    normalize3d(&direction);
    cameraFront = direction;
}  
// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

double calculateFPS(double elapsedTime, int *frameCount) {
  double fps = (*frameCount) / elapsedTime;
  *frameCount = 0;
  return fps;
}

void loadVectors(int axisSize, int offsetX, int offsetZ, struct chunk* chunk, struct v4* transform) {
  unsigned int seed;
  chunk->offsetX = offsetX;
  chunk->offsetZ = offsetZ;
  newSeed(&seed);
  createN3dArray(axisSize, seed, chunk, offsetX, offsetZ, p);
  int counter = 0;
  for (int i = 0; i < axisSize; i++) {
    for (int j = 0; j < axisSize; j++) {
      for (int k = 0; k < axisSize; k++) {
        struct v4 currentVector;
        currentVector.x = (float)i - 0.5f;
        currentVector.z = (float)j - 0.5f;
        currentVector.y = (float)k - 0.5f;
        currentVector.w = -1.0f;
        transform[counter] = currentVector;
        if (chunk->m[i][j][k] == 2) {
          transform[counter].w = 2.0f;
        }
        else if (chunk->m[i][j][k] == 3) {
          transform[counter].w = 3.0f;
        }
        else if (chunk->m[i][j][k] == 4) {
          transform[counter].w = 4.0f;
        }
        else if (chunk->m[i][j][k] == 1) {
          transform[counter].w = 1.0f;
        }
        counter++;
      }
    }
  }
}