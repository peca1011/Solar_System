/*
OpenGL coordinate system (right-handed)
positive X axis points right
positive Y axis points up
positive Z axis points "outside" the screen


                              Y
                              |
                              |
                              |__X
                             /
                            /
                           /
                          Z
*/

// Std. Includes
#include <string>

// Loader estensioni OpenGL
// http://glad.dav1d.de/
// THIS IS OPTIONAL AND NOT REQUIRED, ONLY USE THIS IF YOU DON'T WANT GLAD TO INCLUDE windows.h
// GLAD will include windows.h for APIENTRY if it was not previously defined.
// Make sure you have the correct definition for APIENTRY for platforms which define _WIN32 but don't use __stdcall
#ifdef _WIN32
    #define APIENTRY __stdcall
#endif

#include <glad/glad.h>

// GLFW library to create window and to manage I/O
#include <glfw/glfw3.h>

// another check related to OpenGL loader
// confirm that GLAD didn't include windows.h
#ifdef WINDOWS
    #error windows.h was included!
#endif

// classes developed during lab lectures to manage shaders, to load models, and for FPS camera
#include <utils/shader.h>
#include <utils/model.h>
#include <utils/camera.h>

// we load the GLM classes used in the application
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

// we include the library for images loading
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

// number of lights in the scene
#define NR_LIGHTS 1

// dimensions of application's window
GLuint screenWidth = 1200, screenHeight = 900;

// callback functions for keyboard and mouse events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// if one of the WASD keys is pressed, we call the corresponding method of the Camera class
void apply_camera_movements();

// index of the current shader subroutine (= 0 in the beginning)
GLuint current_subroutine = 0;
// a vector for all the shader subroutines names used and swapped in the application
vector<std::string> shaders;

// the name of the subroutines are searched in the shaders, and placed in the shaders vector (to allow shaders swapping)
void SetupShader(int shader_program);



// print on console the name of current shader subroutine
void PrintCurrentShader(int subroutine);

// load the 6 images from disk and create an OpenGL cubemap
GLint LoadTextureCube(string path);

// load image from disk and create an OpenGL texture
GLint LoadTexture(const char* path);

// we initialize an array of booleans for each keyboard key
bool keys[1024];

// we need to store the previous mouse position to calculate the offset with the current frame
GLfloat lastX, lastY;
// when rendering the first frame, we do not have a "previous state" for the mouse, so we need to manage this situation
bool firstMouse = true;

// parameters for time calculation (for animations)
GLfloat deltaTime = 5.0f;
GLfloat lastFrame = 0.0f;


// rotation angle on Y axis
GLfloat orientationYSun, orientationYMercury, orientationYVenus, orientationYEarth, orientationYMars, orientationYJupiter, orientationYSaturn, orientationYUranus, orientationYNeptune= 0.0f;
// rotation speed on Y axis
GLfloat spin_speedSun = 2.0f;
GLfloat spin_speedMercury = 4.0f;
GLfloat spin_speedVenus = 3.5f;
GLfloat spin_speedEarth = 3.0f;
GLfloat spin_speedMars = 2.5f;
GLfloat spin_speedJupiter = 2.0f;
GLfloat spin_speedSaturn = 1.5f;
GLfloat spin_speedUranus = 1.0f;
GLfloat spin_speedNeptune = 0.5f;

// Declare the rotation angles for each planet's own rotation
GLfloat rotationYMercury;
GLfloat rotationYVenus;
GLfloat rotationYEarth;
GLfloat rotationYMars;
GLfloat rotationYJupiter;
GLfloat rotationYSaturn;
GLfloat rotationYUranus;
GLfloat rotationYNeptune;

GLfloat angleSun = 0.0f;
GLfloat angleMercury = 0.0f;
GLfloat angleVenus = 0.0f;
GLfloat angleEarth = 90.0f;
GLfloat angleMars = 0.0f;
GLfloat angleJupiter = 0.0f;
GLfloat angleSaturn = 0.0f;
GLfloat angleUranus = 0.0f;
GLfloat angleNeptune = 0.0f;

// Declare the rotation speeds for each planet's own rotation
GLfloat rotation_speedMercury = 5.0f;
GLfloat rotation_speedVenus = 3.0f;
GLfloat rotation_speedEarth = 30.0f;
GLfloat rotation_speedMars = 0.5f;
GLfloat rotation_speedJupiter = 2.5f;
GLfloat rotation_speedSaturn = 2.0f;
GLfloat rotation_speedUranus = 1.5f;
GLfloat rotation_speedNeptune = 1.0f;

GLfloat orbitRadiusMercury = 2.5f;
GLfloat orbitRadiusVenus = 4.5f;
GLfloat orbitRadiusEarth = 6.5f; 
GLfloat orbitRadiusMars = 9.0f;  
GLfloat orbitRadiusJupiter = 15.0f;
GLfloat orbitRadiusSaturn = 20.0f; 
GLfloat orbitRadiusUranus = 25.0f;
GLfloat orbitRadiusNeptune = 30.0f;
// boolean to start/stop animated rotation on Y angle
GLboolean spinning = GL_TRUE;

// boolean to activate/deactivate wireframe rendering
GLboolean wireframe = GL_FALSE;

// we create a camera. We pass the initial position as a paramenter to the constructor. The last boolean tells if we want a camera "anchored" to the ground
Camera camera(glm::vec3(0.0f, 0.0f, 7.0f), GL_TRUE);


// specular and ambient components
GLfloat specularColor[] = {1.0,1.0,1.0};
GLfloat ambientColor[] = {1.0,1.0,1.0};

// weights for the diffusive, specular and ambient components
GLfloat Kd = 0.8f;
GLfloat Ks = 0.5f;
GLfloat Ka = 0.01f;
// shininess coefficient for Blinn-Phong shader
GLfloat shininess = 25.0f;

// roughness index for GGX shader
GLfloat alpha = 0.2f;
// Fresnel reflectance at 0 degree (Schlik's approximation)
GLfloat F0 = 0.9f;

// ratio between refraction indices (Fresnel shader) of air (1.00) and glass (1.52)
GLfloat Eta = 1.0f/1.52f;
// exponent for Fresnel equation
// = 5 -> "physically correct" value
// < 5 -> technically not physically correct, but it gives more "artistic" results
GLfloat mFresnelPower = 5.0f;

// texture unit for the cube map
GLuint textureCube;
GLuint textureSun;

// vector for the textures IDs
vector<GLint> textureID;

// UV repetitions
GLfloat repeat = 1.0f;



/////////////////// MAIN function ///////////////////////
int main()
{
  // Initialization of OpenGL context using GLFW
  glfwInit();
  // We set OpenGL specifications required for this application
  // In this case: 4.1 Core
  // If not supported by your graphics HW, the context will not be created and the application will close
  // N.B.) creating GLAD code to load extensions, try to take into account the specifications and any extensions you want to use,
  // in relation also to the values indicated in these GLFW commands
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // we set if the window is resizable
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // we create the application's window
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "try", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // we put in relation the window and the callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // we disable the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD tries to load the context set by GLFW
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // we define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // we enable Z test
    glEnable(GL_DEPTH_TEST);

    //the "clear" color for the frame buffer
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // we create the Shader Program used for the environment map
    Shader skybox_shader("skybox.vert", "skybox.frag");
    Shader sun_shader("sun.vert","sun.frag");
    // we create the Shader Program used for objects (which presents different subroutines we can switch)
    Shader illumination_shader = Shader("illumination_models_ML.vert", "illumination_models_ML.frag");   
    // we parse the Shader Program to search for the number and names of the subroutines.
    // the names are placed in the shaders vector
    SetupShader(illumination_shader.Program);
   // we print on console the name of the first subroutine used
    PrintCurrentShader(current_subroutine);
    
   
    // we load the cube map (we pass the path to the folder containing the 6 views)
    textureCube = LoadTextureCube("../../textures/cube/ProjectCubeMap/");


    // we load the model(s)
    Model cubeModel("../../models/cube.obj"); // used for the environment map
    Model sunModel("../../models/sun.obj");
    Model mercuryModel("../../models/mercury.obj");
    Model venusModel("../../models/venus.obj");
    Model earthModel("../../models/sphere.obj");
    Model marsModel("../../models/sphere.obj");
    Model jupiterModel("../../models/sphere.obj");
    Model saturnModel("../../models/saturn.obj");
    Model uranusModel("../../models/sphere.obj");
    Model neptuneModel("../../models/sphere.obj");

    textureID.push_back(LoadTexture("../../textures/sun/suns.jpg"));
    textureID.push_back(LoadTexture("../../textures/mercury/mercury.jpg"));
    textureID.push_back(LoadTexture("../../textures/venus/venus.jpg"));
    textureID.push_back(LoadTexture("../../textures/earth/earth1.jpg"));
    textureID.push_back(LoadTexture("../../textures/mars.jpg"));
    textureID.push_back(LoadTexture("../../textures/jupiter.jpg"));
    textureID.push_back(LoadTexture("../../textures/saturn.jpg"));
    textureID.push_back(LoadTexture("../../textures/uranus.jpg"));
    textureID.push_back(LoadTexture("../../textures/neptune.jpg"));


    // Projection matrix: FOV angle, aspect ratio, near and far planes
    glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 10000.0f);

    // View matrix: the camera moves, so we just set to indentity now
    glm::mat4 view = glm::mat4(1.0f);

    // Model and Normal transformation matrices for the object in the scene: we set to identity
    glm::mat4 sunModelMatrix = glm::mat4(1.0f);
    glm::mat3 sunNormalMatrix = glm::mat3(1.0f);

    glm::mat4 mercuryModelMatrix = glm::mat4(1.0f);
    glm::mat3 mercuryNormalMatrix = glm::mat3(1.0f);

    glm::mat4 venusModelMatrix = glm::mat4(1.0f);
    glm::mat3 venusNormalMatrix = glm::mat3(1.0f);

    glm::mat4 earthModelMatrix = glm::mat4(1.0f);
    glm::mat3 earthNormalMatrix = glm::mat3(1.0f);

    glm::mat4 marsModelMatrix = glm::mat4(1.0f);
    glm::mat3 marsNormalMatrix = glm::mat3(1.0f);

    glm::mat4 jupiterModelMatrix = glm::mat4(1.0f);
    glm::mat3 jupiterNormalMatrix = glm::mat3(1.0f);

    glm::mat4 saturnModelMatrix = glm::mat4(1.0f);
    glm::mat3 saturnNormalMatrix = glm::mat3(1.0f);

    glm::mat4 uranusModelMatrix = glm::mat4(1.0f);
    glm::mat3 uranusNormalMatrix = glm::mat3(1.0f);

    glm::mat4 neptuneModelMatrix = glm::mat4(1.0f);
    glm::mat3 neptuneNormalMatrix = glm::mat3(1.0f);
    
    // Light and view positions
    glm::vec3 lightPos(10.0f, 10.0f, 10.0f); // Assuming the sun is at origin
    glm::vec3 viewPos = camera.Position;  // Use your camera's position

   // Rendering loop: this code is executed at each frame
    while(!glfwWindowShouldClose(window))
    { 
        
        // we determine the time passed from the beginning
        // and we calculate time difference between current frame rendering and the previous one
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check is an I/O event is happening
        glfwPollEvents();
        // we apply FPS camera movements
        apply_camera_movements();
        // View matrix (=camera): position, view direction, camera "up" vector
        view = camera.GetViewMatrix();

        // we "clear" the frame and z buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // we set the rendering mode
        if (wireframe)
            // Draw in wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // if animated rotation is activated, than we increment the rotation angle using delta time and the rotation speed parameter
        if (spinning)
          //for rotation around itself
          orientationYSun+= (deltaTime*spin_speedSun);
          orientationYMercury += (deltaTime * spin_speedMercury);
          orientationYVenus += (deltaTime * spin_speedVenus);
          orientationYEarth += (deltaTime * spin_speedEarth);
          orientationYMars += (deltaTime * spin_speedMars);
          orientationYJupiter += (deltaTime * spin_speedJupiter);
          orientationYSaturn += (deltaTime * spin_speedSaturn);
          orientationYUranus += (deltaTime * spin_speedUranus);
          orientationYNeptune += (deltaTime * spin_speedNeptune);
          //rotation around sun
          angleMercury = currentFrame * spin_speedMercury;
          angleVenus = currentFrame * spin_speedVenus;
          angleEarth = spin_speedEarth * currentFrame;
          angleMars = currentFrame * spin_speedMars;
          angleJupiter = currentFrame * spin_speedJupiter;
          angleSaturn = currentFrame * spin_speedSaturn;
          angleUranus = currentFrame * spin_speedUranus;
          angleNeptune = currentFrame * spin_speedNeptune;

          // Update the rotation angles for planets' own rotations
          rotationYMercury += (deltaTime * rotation_speedMercury);
          rotationYVenus += (deltaTime * rotation_speedVenus);
          rotationYEarth += (deltaTime * rotation_speedEarth);
          rotationYMars += (deltaTime * rotation_speedMars);
          rotationYJupiter += (deltaTime * rotation_speedJupiter);
          rotationYSaturn += (deltaTime * rotation_speedSaturn);
          rotationYUranus += (deltaTime * rotation_speedUranus);
          rotationYNeptune += (deltaTime * rotation_speedNeptune);

        illumination_shader.Use();
          
        /////////////////// PLANE ////////////////////////////////////////////////
   

    // We search inside the Shader Program the name of the subroutine, and we get the numerical index
    GLuint index = glGetSubroutineIndex(illumination_shader.Program, GL_FRAGMENT_SHADER, "BlinnPhong_ML_TX");
    // We activate the subroutine using the index (this is where shaders swapping happens)
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index);

    // We determine the position in the Shader Program of the uniform variables
    GLint lightPosLocation = glGetUniformLocation(illumination_shader.Program, "lightPos");
    GLint textureLocation = glGetUniformLocation(illumination_shader.Program, "tex");
    GLint textureLoc = glGetUniformLocation(sun_shader.Program, "tex");
    GLint repeatLocation = glGetUniformLocation(illumination_shader.Program, "repeat");
    GLint repeatLoc = glGetUniformLocation(sun_shader.Program, "repeat");

    GLint matAmbientLocation = glGetUniformLocation(illumination_shader.Program, "ambientColor");
    GLint matSpecularLocation = glGetUniformLocation(illumination_shader.Program, "specularColor");
    GLint kaLocation = glGetUniformLocation(illumination_shader.Program, "Ka");
    GLint kdLocation = glGetUniformLocation(illumination_shader.Program, "Kd");
    GLint ksLocation = glGetUniformLocation(illumination_shader.Program, "Ks");
    GLint shineLocation = glGetUniformLocation(illumination_shader.Program, "shininess");
    GLint alphaLocation = glGetUniformLocation(illumination_shader.Program, "alpha");
    GLint f0Location = glGetUniformLocation(illumination_shader.Program, "F0");

    glUniform3fv(lightPosLocation, 1, glm::value_ptr(lightPos));
       ////////////////// OBJECT ////////////////////////////////////////////////
        // we activate the cube map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);


        // we pass projection and view matrices to the Shader Program
       glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
       glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));


    //////////SUN///////////
        sun_shader.Use();
       
        
        
        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[0]);

        // Set transformation matrices for the sun
        glUniformMatrix4fv(glGetUniformLocation(sun_shader.Program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(sun_shader.Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform1i(textureLoc, 0);
        glUniform1f(repeatLoc, 1.0f);

        sunModelMatrix = glm::mat4(1.0f);
        sunNormalMatrix = glm::mat3(1.0f);
        
        sunModelMatrix = glm::rotate(sunModelMatrix, glm::radians(orientationYSun), glm::vec3(0.0f, 1.0f, 0.0f));
        sunModelMatrix = glm::scale(sunModelMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
        
        glUniformMatrix4fv(glGetUniformLocation(sun_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(sunModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(sun_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(sunNormalMatrix));
   
        sunModel.Draw();

       /////////////MERCURY////////////
        illumination_shader.Use();
        // Activate the texture with id 1, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[1]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);

        mercuryModelMatrix = glm::mat4(1.0f);
        mercuryNormalMatrix = glm::mat3(1.0f);
        mercuryModelMatrix = glm::rotate(mercuryModelMatrix, glm::radians(orientationYMercury), glm::vec3(0.0f, 1.0f, 0.0f)); // Planet's own rotation
        mercuryModelMatrix = glm::translate(mercuryModelMatrix, glm::vec3(orbitRadiusMercury, 0.0f, 0.0f)); // move mervury to Orbital radius
        mercuryModelMatrix = glm::rotate(mercuryModelMatrix, glm::radians(angleMercury), glm::vec3(0.0f, 1.0f, 0.0f));// orient orbit angle around the sun
        mercuryModelMatrix = glm::scale(mercuryModelMatrix, glm::vec3(0.1596f, 0.1596f, 0.1596f)); // Scale relative to the sun
        mercuryNormalMatrix = glm::inverseTranspose(glm::mat3(view * mercuryModelMatrix));

        
        // Set the transformation matrices for the shader
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(mercuryModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(mercuryNormalMatrix));

        // Draw the Mercury model
        mercuryModel.Draw();

       /////////////VENUS////////////
        // Activate the texture with id 1, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[2]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);

        // Venus transformation
        venusModelMatrix = glm::mat4(1.0f);
        venusNormalMatrix = glm::mat3(1.0f);
        venusModelMatrix = glm::rotate(venusModelMatrix, glm::radians(orientationYVenus), glm::vec3(0.0f, 1.0f, 0.0f));//orient orbit
        venusModelMatrix = glm::translate(venusModelMatrix, glm::vec3(orbitRadiusVenus, 0.0f, 0.0f)); //move venus to its orbit
        venusModelMatrix = glm::rotate(venusModelMatrix, glm::radians(angleVenus), glm::vec3(0.0f, 1.0f, 0.0f)); // Planet's own rotation
        venusModelMatrix = glm::scale(venusModelMatrix, glm::vec3(0.399f, 0.399f, 0.399f));
        venusNormalMatrix = glm::inverseTranspose(glm::mat3(view * venusModelMatrix));
        
        // Set the transformation matrices for the shader
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(venusModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(venusNormalMatrix));

        
        // Draw the Venus model
        venusModel.Draw();

         /////////////EARTH////////////
        // Activate the texture with id 1, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[3]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);

        // Earth transformation
        earthModelMatrix = glm::mat4(1.0f);
        earthNormalMatrix = glm::mat3(1.0f);
        
        earthModelMatrix = glm::rotate(earthModelMatrix, glm::radians(orientationYEarth), glm::vec3(0.0f, 1.0f, 0.0f)); //orient orbit angle around the sun
        earthModelMatrix = glm::translate(earthModelMatrix, glm::vec3(orbitRadiusEarth, 0.0f, 0.0f)); //move earth to orbit of radius
        earthModelMatrix = glm::rotate(earthModelMatrix, glm::radians(angleEarth), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around itself       
        earthModelMatrix = glm::scale(earthModelMatrix, glm::vec3(0.42f, 0.42f, 0.42f)); // Scale relative to the sun
        earthNormalMatrix = glm::inverseTranspose(glm::mat3(view * earthModelMatrix));

        
        // Set the transformation matrices for the shader
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(earthModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(earthNormalMatrix));

        
        // Draw the Earth model
        earthModel.Draw();


        /////////////MARS////////////
        // Activate the texture with id 1, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[4]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);

        // Mars transformation
        marsModelMatrix = glm::mat4(1.0f);
        marsNormalMatrix = glm::mat3(1.0f); 
        marsModelMatrix = glm::rotate(marsModelMatrix, glm::radians(orientationYMars), glm::vec3(0.0f, 1.0f, 0.0f));
        marsModelMatrix = glm::translate(marsModelMatrix, glm::vec3(orbitRadiusMars, 0.0f, 0.0f));
        marsModelMatrix = glm::rotate(marsModelMatrix, angleMars, glm::vec3(0.0f, 1.0f, 0.0f)); // Planet's own rotation
        marsModelMatrix = glm::scale(marsModelMatrix, glm::vec3(0.2226f, 0.2226f, 0.2226f)); // Scale relative to the sun
        marsNormalMatrix = glm::inverseTranspose(glm::mat3(view * marsModelMatrix));

        
        // Set the transformation matrices for the shader
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(marsModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(marsNormalMatrix));

        // Draw the Mars model
        marsModel.Draw();


        /////////////JUPITER////////////
        // Activate the texture with id 1, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[5]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);

        // Jupiter transformation
        jupiterModelMatrix = glm::mat4(1.0f);
        jupiterNormalMatrix = glm::mat3(1.0f);
        jupiterModelMatrix = glm::rotate(jupiterModelMatrix, glm::radians(orientationYJupiter), glm::vec3(0.0f, 1.0f, 0.0f));
        jupiterModelMatrix = glm::translate(jupiterModelMatrix, glm::vec3(orbitRadiusJupiter, 0.0f, 0.0f));
        jupiterModelMatrix = glm::rotate(jupiterModelMatrix, angleJupiter, glm::vec3(0.0f, 1.0f, 0.0f)); // Planet's own rotation
        jupiterModelMatrix = glm::scale(jupiterModelMatrix, glm::vec3(0.90f, 0.9f, 0.9f)); // Scale relative to the sun
        jupiterNormalMatrix = glm::inverseTranspose(glm::mat3(view * jupiterModelMatrix));

        
        // Set the transformation matrices for the shader
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(jupiterModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(jupiterNormalMatrix));

        
        // Draw the Jupiter model
        jupiterModel.Draw();


        /////////////SATURN///////////
        // Activate the texture with id 1, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[6]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);
        // Saturn transformation
        
        saturnModelMatrix = glm::mat4(1.0f);
        saturnNormalMatrix = glm::mat3(1.0f);
        saturnModelMatrix = glm::rotate(saturnModelMatrix,glm::radians(90.0f),glm::vec3(1.0f, 0.0f, 0.0f)); 
        saturnModelMatrix = glm::rotate(saturnModelMatrix, glm::radians(orientationYSaturn), glm::vec3(0.0f, 0.0f, -1.0f)); //orient orbit angle around the sun
        saturnModelMatrix = glm::translate(saturnModelMatrix, glm::vec3(orbitRadiusSaturn, 0.0f, 0.0f)); //move planet to orbit radius
        saturnModelMatrix = glm::rotate(saturnModelMatrix, angleSaturn, glm::vec3(0.0f, 0.0f, 1.0f)); //rotate around itself
        saturnModelMatrix = glm::scale(saturnModelMatrix, glm::vec3(0.004f,0.004f,0.004f)); // Scale relative to the sun
        saturnNormalMatrix = glm::inverseTranspose(glm::mat3(view * saturnModelMatrix));
        

        // Set the transformation matrices for the shader
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(saturnModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(saturnNormalMatrix));
        
        
        // Draw the Saturn model
        saturnModel.Draw();




         /////////////URANUS////////////
        // Activate the texture with id 7, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[7]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);

        // Uranus transformation
        uranusModelMatrix = glm::mat4(1.0f);
        uranusNormalMatrix = glm::mat3(1.0f);
        uranusModelMatrix = glm::rotate(uranusModelMatrix, glm::radians(orientationYUranus), glm::vec3(0.0f, 1.0f, 0.0f));
        uranusModelMatrix = glm::translate(uranusModelMatrix, glm::vec3(orbitRadiusUranus, 0.0f, 0.0f)); //move uranus to orbit of radius
        uranusModelMatrix = glm::rotate(uranusModelMatrix, angleUranus, glm::vec3(0.0f, 1.0f, 0.0f)); // Planet's own rotation
        uranusModelMatrix = glm::scale(uranusModelMatrix, glm::vec3(0.70f, 0.7f, 0.7f)); // Scale relative to the sun
        uranusNormalMatrix = glm::inverseTranspose(glm::mat3(view * uranusModelMatrix));

        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(uranusModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(uranusNormalMatrix));

        // Draw the Uranus model
        uranusModel.Draw();


         /////////////NEPTUNE////////////
        // Activate the texture with id 8, and bind the id to our loaded texture data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[8]);

        // Set uniform variables for the shader
        glUniform1f(kaLocation, Ka);
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, 1.0f);

        // Neptune transformation
        neptuneModelMatrix = glm::mat4(1.0f);
        neptuneNormalMatrix = glm::mat3(1.0f);
        neptuneModelMatrix = glm::rotate(neptuneModelMatrix, glm::radians(orientationYNeptune), glm::vec3(0.0f, 1.0f, 0.0f)); //orient orbit angle around the sun
        neptuneModelMatrix = glm::translate(neptuneModelMatrix, glm::vec3(orbitRadiusNeptune, 0.0f, 0.0f)); // move neptune to the orbit of the radius
        neptuneModelMatrix = glm::rotate(neptuneModelMatrix, angleNeptune, glm::vec3(0.0f, 1.0f, 0.0f)); // Routate around itself
        neptuneModelMatrix = glm::scale(neptuneModelMatrix, glm::vec3(0.65f, 0.65f, 0.65f)); // Scale relative to the sun
        neptuneNormalMatrix = glm::inverseTranspose(glm::mat3(view * neptuneModelMatrix));

        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(neptuneModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(illumination_shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(neptuneNormalMatrix));

        // Activate or deactivate the texture
        glUniform1i(glGetUniformLocation(illumination_shader.Program, "useTexture"), true);

        // Draw the Neptune model
        neptuneModel.Draw();


        /////////////////// SKYBOX ////////////////////////////////////////////////
        // we use the cube to attach the 6 textures of the environment map.
        // we render it after all the other objects, in order to avoid the depth tests as much as possible.
        // we will set, in the vertex shader for the skybox, all the values to the maximum depth. Thus, the environment map is rendered only where there are no other objects in the image (so, only on the background).
        //Thus, we set the depth test to GL_LEQUAL, in order to let the fragments of the background pass the depth test (because they have the maximum depth possible, and the default setting is GL_LESS)
        glDepthFunc(GL_LEQUAL);
        skybox_shader.Use();
        
         // we pass projection and view matrices to the Shader Program of the skybox
        glUniformMatrix4fv(glGetUniformLocation(skybox_shader.Program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
        // to have the background fixed during camera movements, we have to remove the translations from the view matrix
        // thus, we consider only the top-left submatrix, and we create a new 4x4 matrix
        view = glm::mat4(glm::mat3(view)); // Remove any translation component of the view matrix
        glUniformMatrix4fv(glGetUniformLocation(skybox_shader.Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));

        // we determine the position in the Shader Program of the uniform variables
        textureLocation = glGetUniformLocation(skybox_shader.Program, "tCube");
        // we assign the value to the uniform variable
        glUniform1i(textureLocation, 0);

        // we render the cube with the environment map
        cubeModel.Draw();
        // we set again the depth test to the default operation for the next frame
        glDepthFunc(GL_LESS);

        // Swapping back and front buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    illumination_shader.Delete();
    sun_shader.Delete();
    // when I exit from the graphics loop, it is because the application is closing
    // we delete the Shader Program
    skybox_shader.Delete();
    // we close and delete the created context
    glfwTerminate();
    return 0;
}

///////////////////////////////////////////
// load one side of the cubemap, passing the name of the file and the side of the corresponding OpenGL cubemap
void LoadTextureCubeSide(string path, string side_image, GLuint side_name)
{
    int w, h;
    unsigned char* image;
    string fullname;

    // full name and path of the side of the cubemap
    fullname = path + side_image;
    // we load the image file
    image = stbi_load(fullname.c_str(), &w, &h, 0, STBI_rgb);
    if (image == nullptr)
        std::cout << "Failed to load texture!" << std::endl;
    // we set the image file as one of the side of the cubemap (passed as a parameter)
    glTexImage2D(side_name, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    // we free the memory once we have created an OpenGL texture
    stbi_image_free(image);
}


//////////////////////////////////////////
// we load the 6 images from disk and we create an OpenGL cube map
GLint LoadTextureCube(string path)
{
    GLuint textureImage;

    // we create and activate the OpenGL cubemap texture
    glGenTextures(1, &textureImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureImage);

    // we load and set the 6 images corresponding to the 6 views of the cubemap
    // we use as convention that the names of the 6 images are "posx, negx, posy, negy, posz, negz", placed at the path passed as parameter
    // we load the images individually and we assign them to the correct sides of the cube map
    LoadTextureCubeSide(path, std::string("nx.png"), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    LoadTextureCubeSide(path, std::string("ny.png"), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    LoadTextureCubeSide(path, std::string("nz.png"), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    LoadTextureCubeSide(path, std::string("px.png"), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    LoadTextureCubeSide(path, std::string("pz.png"), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    LoadTextureCubeSide(path, std::string("py.png"), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    // we set the filtering for minification and magnification
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // we set how to consider the texture coordinates outside [0,1] range
    // in this case we have a cube map, so
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // we set the binding to 0 once we have finished
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureImage;

}

///////////////////////////////////////////
// The function parses the content of the Shader Program, searches for the Subroutine type names,
// the subroutines implemented for each type, print the names of the subroutines on the terminal, and add the names of
// the subroutines to the shaders vector, which is used for the shaders swapping
void SetupShader(int program)
{
    int maxSub,maxSubU,countActiveSU;
    GLchar name[256];
    int len, numCompS;

    // global parameters about the Subroutines parameters of the system
    glGetIntegerv(GL_MAX_SUBROUTINES, &maxSub);
    glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubU);
    std::cout << "Max Subroutines:" << maxSub << " - Max Subroutine Uniforms:" << maxSubU << std::endl;

    // get the number of Subroutine uniforms (only for the Fragment shader, due to the nature of the exercise)
    // it is possible to add similar calls also for the Vertex shader
    glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &countActiveSU);

    // print info for every Subroutine uniform
    for (int i = 0; i < countActiveSU; i++) {

        // get the name of the Subroutine uniform (in this example, we have only one)
        glGetActiveSubroutineUniformName(program, GL_FRAGMENT_SHADER, i, 256, &len, name);
        // print index and name of the Subroutine uniform
        std::cout << "Subroutine Uniform: " << i << " - name: " << name << std::endl;

        // get the number of subroutines
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);

        // get the indices of the active subroutines info and write into the array s
        int *s =  new int[numCompS];
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_COMPATIBLE_SUBROUTINES, s);
        std::cout << "Compatible Subroutines:" << std::endl;

        // for each index, get the name of the subroutines, print info, and save the name in the shaders vector
        for (int j=0; j < numCompS; ++j) {
            glGetActiveSubroutineName(program, GL_FRAGMENT_SHADER, s[j], 256, &len, name);
            std::cout << "\t" << s[j] << " - " << name << "\n";
            shaders.push_back(name);
        }
        std::cout << std::endl;

        delete[] s;
    }
}



GLint LoadTexture(const char* path)
{
    GLuint textureImage;
    int w, h, channels;
    unsigned char* image;
    image = stbi_load(path, &w, &h, &channels, STBI_rgb);

    if (image == nullptr)
        std::cout << "Failed to load texture!" << std::endl;

    glGenTextures(1, &textureImage);
    glBindTexture(GL_TEXTURE_2D, textureImage);
    // 3 channels = RGB ; 4 channel = RGBA
    if (channels==3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (channels==4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    // we set how to consider UVs outside [0,1] range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // we set the filtering for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // we free the memory once we have created an OpenGL texture
    stbi_image_free(image);

    // we set the binding to 0 once we have finished
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureImage;

}

//////////////////////////////////////////
// we print on console the name of the currently used shader subroutine
void PrintCurrentShader(int subroutine)
{
    std::cout << "Current shader subroutine: " << shaders[subroutine]  << std::endl;
}

//////////////////////////////////////////
// callback for keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    GLuint new_subroutine;

    // if ESC is pressed, we close the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // if P is pressed, we start/stop the animated rotation of models
    if(key == GLFW_KEY_P && action == GLFW_PRESS)
        spinning=!spinning;

    // if L is pressed, we activate/deactivate wireframe rendering of models
    if(key == GLFW_KEY_L && action == GLFW_PRESS)
        wireframe=!wireframe;

    // pressing a key number, we change the shader applied to the models
    // if the key is between 1 and 9, we proceed and check if the pressed key corresponds to
    // a valid subroutine
    if((key >= GLFW_KEY_1 && key <= GLFW_KEY_9) && action == GLFW_PRESS)
    {
        // "1" to "9" -> ASCII codes from 49 to 59
        // we subtract 48 (= ASCII CODE of "0") to have integers from 1 to 9
        // we subtract 1 to have indices from 0 to 8
        new_subroutine = (key-'0'-1);
        // if the new index is valid ( = there is a subroutine with that index in the shaders vector),
        // we change the value of the current_subroutine variable
        // NB: we can just check if the new index is in the range between 0 and the size of the shaders vector,
        // avoiding to use the std::find function on the vector
        if (new_subroutine<shaders.size())
        {
            current_subroutine = new_subroutine;
            PrintCurrentShader(current_subroutine);
        }
    }

    // we keep trace of the pressed keys
    // with this method, we can manage 2 keys pressed at the same time:
    // many I/O managers often consider only 1 key pressed at the time (the first pressed, until it is released)
    // using a boolean array, we can then check and manage all the keys pressed at the same time
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

//////////////////////////////////////////
// If one of the WASD keys is pressed, the camera is moved accordingly (the code is in utils/camera.h)
void apply_camera_movements()
{
    // if a single WASD key is pressed, then we will apply the full value of velocity v in the corresponding direction.
    // However, if two keys are pressed together in order to move diagonally (W+D, W+A, S+D, S+A), 
    // then the camera will apply a compensation factor to the velocities applied in the single directions, 
    // in order to have the full v applied in the diagonal direction  
    // the XOR on A and D is to avoid the application of a wrong attenuation in the case W+A+D or S+A+D are pressed together.  
    GLboolean diagonal_movement = (keys[GLFW_KEY_W] ^ keys[GLFW_KEY_S]) && (keys[GLFW_KEY_A] ^ keys[GLFW_KEY_D]); 
    camera.SetMovementCompensation(diagonal_movement);
    
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

//////////////////////////////////////////

  // callback for mouse events
  void mouse_callback(GLFWwindow* window, double xpos, double ypos)
  {
        // we move the camera view following the mouse cursor
        // we calculate the offset of the mouse cursor from the position in the last frame
        // when rendering the first frame, we do not have a "previous state" for the mouse, so we set the previous state equal to the initial values (thus, the offset will be = 0)
        if(firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        // offset of mouse cursor position
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos;

        // the new position will be the previous one for the next frame
        lastX = xpos;
        lastY = ypos;

        // we pass the offset to the Camera class instance in order to update the rendering
        camera.ProcessMouseMovement(xoffset, yoffset);

    }