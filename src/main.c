#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <math.h>
#include "stb_image.h"
#include "Shader.h"
#include "vMaths.h"
#include "randGen.h"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
double calculateFPS(double elapsedTime, int *frameCount);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
  

  // Variables for FPS calculation
  double startTime = glfwGetTime();
  double lastTime = startTime;
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
  float worldMat[2][2][2] = {
    {{1.0f, 1.0f},
     {1.0f, 1.0f},},
     {{1.0f, 0.0f},
     {0.0f, 0.0f},},
  };
  int worldSize = 8;
  int axisSize = 2;
  struct v4 transforms[worldSize];
  int counter = 0;
  for (int i = 0; i < axisSize; i++) {
    for (int j = 0; j < axisSize; j++) {
      for (int k = 0; k < axisSize; k++) {
        struct v4 currentVector;
        currentVector.y = (float)i - 0.5f;
        currentVector.z = (float)j - 0.5f;
        currentVector.x = (float)k - 0.5f;
        currentVector.w = 1.0f;
        if (worldMat[i][j][k] != 1.0f) {
          currentVector.w = -1.0f; 
        }
        transforms[counter] = currentVector;
        counter++;
      }
    }
  }

  printf("%f %f %f\n%ff %f %f\n %f %f %f", transforms[0].x, transforms[0].y, transforms[0].z, transforms[1].x, transforms[1].y, transforms[1].z, transforms[2].x, transforms[2].y, transforms[2].z);
  unsigned int num1 = NoiseGen(1000, 9999, 1234567);
  unsigned int num2 = NoiseGen(1000, 9999, 9964921);
  unsigned int num3 = NoiseGen(1000, 9999, 92345678);
  printf("%i %i %i", num1, num2, num3);
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
      stbi_load("C:\\Users\\rkmun\\source\\repos\\LearnOpenGL\\src\\wall.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture\n%s", stbi_failure_reason());
  }
  stbi_image_free(data);

  struct v3 xAxis = {1.0f, 0.0f, 0.0f};
  struct v3 yAxis = {0.0f, 1.0f, 0.0f};
  struct v3 zAxis = {0.0f, 0.0f, 1.0f};

  
  
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
    


    // bind Texture
    glBindTexture(GL_TEXTURE_2D, texture);
    

    

    struct quaternion cameraQuat;
    

    struct mat4 view = makeIdentityMatrix();
    struct v3 cameraVec = {0.0f, -2.0f, -10.0f};
    translate(cameraVec, &view);

    struct mat4 projection = makeIdentityMatrix();
    makeProjection(45.0f, 0.1f, 100.0f, &projection, (float)SCR_WIDTH,
                   (float)SCR_HEIGHT);

    int viewLoc = glGetUniformLocation(shader.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &view.m);
    int projectionLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, &projection.m);
    
    // render container
    shader_use(&shader);
    
    
    glBindVertexArray(VAO);
    
    for(unsigned int i = 0; i < worldSize; i++)
    {
      if (transforms[i].w != -1) {
        struct mat4 model = makeIdentityMatrix();
        axisAngleToQuaternion(&xAxis, (30.0f * (3.14 / 180)), &cameraQuat);
        
        struct v3 renderVector = {transforms[i].x, transforms[i].y, transforms[i].z};
        translate(renderVector, &model);
        quatRot(cameraQuat, &model);
        int modelLoc = glGetUniformLocation(shader.ID, "model");
        
        glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &model.m);

        glDrawArrays(GL_TRIANGLES, 0, 36);
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
    double currentTime = glfwGetTime();
    double elapsedTime = currentTime - lastTime;

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
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
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