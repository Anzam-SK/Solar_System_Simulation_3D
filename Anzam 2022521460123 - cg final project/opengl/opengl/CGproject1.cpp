#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <array>

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Orbit angle to control the rotation of planets around the Sun
float orbitAngle = 0.0f;

// Rotation angles for each planet's own axis
std::array<float, 9> planetRotationAngles = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

// Rotation speeds for each planet's orbit (degrees per "+" key press)
const std::array<float, 9> planetOrbitSpeeds = {
    0.5f,  // Sun (if needed)
    4.74f, // Mercury
    1.62f, // Venus
    0.99f, // Earth
    0.53f, // Mars
    1.08f, // Jupiter
    0.43f, // Saturn
    0.72f, // Uranus
    1.19f  // Neptune
};

// Rotation speeds for each planet's own axis (degrees per "+" key press)
const std::array<float, 9> planetRotationSpeeds = {
    0.5f,  // Sun
    4.74f, // Mercury
    1.62f, // Venus
    0.99f, // Earth
    0.53f, // Mars
    1.08f, // Jupiter
    0.43f, // Saturn
    0.72f, // Uranus
    1.19f  // Neptune
};

// Distances from the Sun for each celestial body (0: Sun, 1: Mercury, ..., 8: Neptune)
const std::array<float, 9> distance_from_sun = {
    0.0f,  // Sun
    3.0f,  // Mercury
    5.0f,  // Venus
    7.0f,  // Earth
    9.0f,  // Mars
    12.0f, // Jupiter
    15.0f, // Saturn
    18.0f, // Uranus
    21.0f  // Neptune
};

// Define planet radii using std::array
const std::array<float, 9> planetRadii = {
    1.0f,  // Sun
    0.2f,  // Mercury
    0.3f,  // Venus
    0.3f,  // Earth
    0.2f,  // Mars
    0.6f,  // Jupiter
    0.5f,  // Saturn
    0.4f,  // Uranus
    0.4f   // Neptune
};

// Define planet names using std::array for better type safety
const std::array<std::string, 9> planetNames = {
    "Sun",
    "Mercury",
    "Venus",
    "Earth",
    "Mars",
    "Jupiter",
    "Saturn",
    "Uranus",
    "Neptune"
};

GLuint textures[9];          // Array to hold textures for Sun and the 8 planets
GLuint backgroundTexture;    // Texture for the Milky Way background

// Currently held object for zooming (-1: None, 0-8: Sun and planets)
int heldObject = -1;

// Global variables for camera control via mouse
float cameraYaw = 0.0f;         // Horizontal angle (left-right)
float cameraPitch = 0.0f;       // Vertical angle (up-down)
float lastMouseX = 0.0f;        // Last mouse X position
float lastMouseY = 0.0f;        // Last mouse Y position
bool isDragging = false;        // Track if mouse is dragging

// Camera distance from the target
float cameraDistance = 30.0f;   // Initial distance

// Function to load a BMP texture with corrected image size calculation
GLuint loadBMPTexture(const char* filename) {
    GLuint texture;
    int width, height;
    unsigned char* data;

    FILE* file;
    errno_t err = fopen_s(&file, filename, "rb"); // Use fopen_s for safety
    if (err != 0 || !file) {
        std::cerr << "Failed to open texture file: " << filename << std::endl;
        return 0;
    }

    unsigned char header[54];
    size_t bytesRead = fread(header, sizeof(unsigned char), 54, file);
    if (bytesRead != 54) {
        std::cerr << "Invalid BMP file: " << filename << std::endl;
        fclose(file);
        return 0;
    }

    // Verify BMP signature
    if (header[0] != 'B' || header[1] != 'M') {
        std::cerr << "Invalid BMP signature: " << filename << std::endl;
        fclose(file);
        return 0;
    }

    // Extract image dimensions
    width = *(int*)&header[18];
    height = *(int*)&header[22];

    // Ensure the BMP is 24-bit
    int bitsPerPixel = *(short*)&header[28];
    if (bitsPerPixel != 24) {
        std::cerr << "Unsupported BMP format (only 24-bit supported): " << filename << std::endl;
        fclose(file);
        return 0;
    }

    // Calculate row size with padding (each row is padded to a multiple of 4 bytes)
    int bytesPerPixel = 3;
    int rowSize = (bytesPerPixel * width + 3) & (~3);
    int imageSize = rowSize * height;

    // Allocate memory for image data
    data = new unsigned char[imageSize];
    if (!data) {
        std::cerr << "Failed to allocate memory for texture: " << filename << std::endl;
        fclose(file);
        return 0;
    }

    // Read image data
    bytesRead = fread(data, sizeof(unsigned char), imageSize, file);
    if (bytesRead != imageSize) {
        std::cerr << "Failed to read image data from BMP: " << filename << std::endl;
        delete[] data;
        fclose(file);
        return 0;
    }
    fclose(file);

    // Convert BGR to RGB
    for (int y = 0; y < height; ++y) {
        for (int i = 0; i < width * bytesPerPixel; i += 3) {
            std::swap(data[y * rowSize + i], data[y * rowSize + i + 2]); // Swap B and R
        }
    }

    // Create a tightly packed buffer for OpenGL
    unsigned char* packedData = new unsigned char[width * height * bytesPerPixel];
    if (!packedData) {
        std::cerr << "Failed to allocate memory for packed texture: " << filename << std::endl;
        delete[] data;
        return 0;
    }

    for (int y = 0; y < height; ++y) {
        memcpy(packedData + y * width * bytesPerPixel, data + y * rowSize, width * bytesPerPixel);
    }

    // Generate OpenGL texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, packedData);

    // Clean up
    delete[] packedData;
    delete[] data;

    std::cout << "Texture loaded successfully: " << filename << std::endl;

    return texture;
}

// Function to draw a textured sphere
void drawTexturedSphere(GLuint texture, float radius, int slices, int stacks) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1.0f, 1.0f, 1.0f); // White color to display texture
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricOrientation(quad, GLU_OUTSIDE); // Ensure correct orientation
    gluSphere(quad, radius, slices, stacks);
    gluDeleteQuadric(quad);

    glDisable(GL_TEXTURE_2D);
}

// Function to draw the Milky Way background
void drawBackground() {
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glColor3f(1.0f, 1.0f, 1.0f); // White color to display the texture
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricOrientation(quad, GLU_INSIDE); // Render the inside of the sphere
    gluSphere(quad, 50.0f, 50, 50);         // Large sphere radius
    gluDeleteQuadric(quad);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Function to draw the Sun
void drawSun() {
    glPushMatrix();
    glRotatef(planetRotationAngles[0], 0.0f, 1.0f, 0.0f); // Rotate on its own axis
    drawTexturedSphere(textures[0], 1.0f, 50, 50);          // Render the textured Sun
    glPopMatrix();
}

// Function to draw Mercury
void drawMercury() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[1], 0.0f, 1.0f, 0.0f); // Orbit rotation
    glTranslatef(distance_from_sun[1], 0.0f, 0.0f);                   // Distance from the Sun
    glRotatef(planetRotationAngles[1], 0.0f, 1.0f, 0.0f);            // Rotate on its own axis
    drawTexturedSphere(textures[1], 0.2f, 20, 20);
    glPopMatrix();
}

// Function to draw Venus
void drawVenus() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[2], 0.0f, 1.0f, 0.0f);
    glTranslatef(distance_from_sun[2], 0.0f, 0.0f);
    glRotatef(planetRotationAngles[2], 0.0f, 1.0f, 0.0f);
    drawTexturedSphere(textures[2], 0.3f, 20, 20);
    glPopMatrix();
}

// Function to draw Earth
void drawEarth() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[3], 0.0f, 1.0f, 0.0f);
    glTranslatef(distance_from_sun[3], 0.0f, 0.0f);
    glRotatef(planetRotationAngles[3], 0.0f, 1.0f, 0.0f);
    drawTexturedSphere(textures[3], 0.3f, 20, 20);
    glPopMatrix();
}

// Function to draw Mars
void drawMars() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[4], 0.0f, 1.0f, 0.0f);
    glTranslatef(distance_from_sun[4], 0.0f, 0.0f);
    glRotatef(planetRotationAngles[4], 0.0f, 1.0f, 0.0f);
    drawTexturedSphere(textures[4], 0.2f, 20, 20);
    glPopMatrix();
}

// Function to draw Jupiter
void drawJupiter() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[5], 0.0f, 1.0f, 0.0f);
    glTranslatef(distance_from_sun[5], 0.0f, 0.0f);
    glRotatef(planetRotationAngles[5], 0.0f, 1.0f, 0.0f);
    drawTexturedSphere(textures[5], 0.6f, 20, 20);
    glPopMatrix();
}

// Function to draw Saturn
void drawSaturn() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[6], 0.0f, 1.0f, 0.0f);
    glTranslatef(distance_from_sun[6], 0.0f, 0.0f);
    glRotatef(planetRotationAngles[6], 0.0f, 1.0f, 0.0f);
    drawTexturedSphere(textures[6], 0.5f, 20, 20);
    glPopMatrix();
}

// Function to draw Uranus
void drawUranus() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[7], 0.0f, 1.0f, 0.0f);
    glTranslatef(distance_from_sun[7], 0.0f, 0.0f);
    glRotatef(planetRotationAngles[7], 0.0f, 1.0f, 0.0f);
    drawTexturedSphere(textures[7], 0.4f, 20, 20);
    glPopMatrix();
}

// Function to draw Neptune
void drawNeptune() {
    glPushMatrix();
    glRotatef(orbitAngle * planetOrbitSpeeds[8], 0.0f, 1.0f, 0.0f);
    glTranslatef(distance_from_sun[8], 0.0f, 0.0f);
    glRotatef(planetRotationAngles[8], 0.0f, 1.0f, 0.0f);
    drawTexturedSphere(textures[8], 0.4f, 20, 20);
    glPopMatrix();
}

// Function to draw orbital paths (optional enhancement)
void drawOrbit(float radius) {
    const int segments = 100;
    glColor3f(1.0f, 1.0f, 1.0f); // White color for orbits
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = radius * cosf(theta); // Use cosf for float
        float z = radius * sinf(theta); // Use sinf for float
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
}

// Helper function to get the current position of a celestial body
void getObjectPosition(int index, float& x, float& z) {
    if (index < 0 || index > 8) {
        x = 0.0f;
        z = 0.0f;
        return;
    }
    float angle = orbitAngle * planetOrbitSpeeds[index];
    float rad = angle * M_PI / 180.0f; // Convert degrees to radians
    x = distance_from_sun[index] * cosf(rad); // Use cosf for float
    z = distance_from_sun[index] * sinf(rad); // Use sinf for float
}

// Function to render bitmap strings (for on-screen instructions)
void renderBitmapString(float x, float y, void* font, const char* string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string);
        string++;
    }
}

// Display function with gluLookAt and on-screen instructions
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float targetX = 0.0f;
    float targetY = 0.0f;
    float targetZ = 0.0f;

    if (heldObject != -1) {
        // Get the position of the held object
        getObjectPosition(heldObject, targetX, targetZ);
    }

    // Calculate camera position in spherical coordinates
    float camX = cameraDistance * cosf(cameraPitch * M_PI / 180.0f) * sinf(cameraYaw * M_PI / 180.0f) + targetX;
    float camY = cameraDistance * sinf(cameraPitch * M_PI / 180.0f) + targetY;
    float camZ = cameraDistance * cosf(cameraPitch * M_PI / 180.0f) * cosf(cameraYaw * M_PI / 180.0f) + targetZ;

    // Define the up vector
    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;

    // Set the camera using gluLookAt
    gluLookAt(camX, camY, camZ, targetX, targetY, targetZ, upX, upY, upZ);

    // Draw the Milky Way background
    drawBackground();

    // Draw orbital paths (optional)
    drawOrbit(3.0f);  // Mercury
    drawOrbit(5.0f);  // Venus
    drawOrbit(7.0f);  // Earth
    drawOrbit(9.0f);  // Mars
    drawOrbit(12.0f); // Jupiter
    drawOrbit(15.0f); // Saturn
    drawOrbit(18.0f); // Uranus
    drawOrbit(21.0f); // Neptune

    // Draw the Sun and planets
    drawSun();
    drawMercury();
    drawVenus();
    drawEarth();
    drawMars();
    drawJupiter();
    drawSaturn();
    drawUranus();
    drawNeptune();

    // Render on-screen instructions
    // Switch to orthographic projection for 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable lighting and textures for text rendering
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Display instructions
    renderBitmapString(10.0f, windowHeight - 20.0f, GLUT_BITMAP_HELVETICA_18, "Controls:");
    renderBitmapString(10.0f, windowHeight - 40.0f, GLUT_BITMAP_HELVETICA_12, "Left Mouse Button + Drag: Rotate Camera");
    renderBitmapString(10.0f, windowHeight - 60.0f, GLUT_BITMAP_HELVETICA_12, "Mouse Wheel: Zoom In/Out");
    renderBitmapString(10.0f, windowHeight - 80.0f, GLUT_BITMAP_HELVETICA_12, "Keys '0'-'8': Select Sun and Planets to zoom");
    renderBitmapString(10.0f, windowHeight - 100.0f, GLUT_BITMAP_HELVETICA_12, "Keys '+' / '-': Planets Movement, anti-clockwise and clockwise");
    renderBitmapString(10.0f, windowHeight - 120.0f, GLUT_BITMAP_HELVETICA_12, "ESC: Exit Program");

    if (heldObject != -1) {
        std::string selectedText = "Selected Object: " + planetNames[heldObject];
        renderBitmapString(10.0f, windowHeight - 140.0f, GLUT_BITMAP_HELVETICA_12, selectedText.c_str());
    }

    // Re-enable lighting and textures
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Restore previous projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

// Reshape function to handle window resizing
void reshape(int w, int h) {
    if (h == 0) h = 1; // Prevent division by zero
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<float>(w) / static_cast<float>(h), 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Cleanup function to delete textures and perform other cleanup tasks
void cleanup() {
    glDeleteTextures(9, textures);
    glDeleteTextures(1, &backgroundTexture);
    std::cout << "Resources cleaned up. Exiting program." << std::endl;
}

// Mouse Button and wheel handler with smaller zoom steps and dynamic min distance
void mouseHandler(int button, int state, int x, int y) {
    // Mouse wheel events
    if (button == 3 && state == GLUT_DOWN) { // Mouse wheel up
        if (heldObject != -1) {
            // Zoom in on the held object
            cameraDistance -= 0.5f; // Smaller zoom step
            // Calculate dynamic minimum distance based on planet radius
            float minDistance = planetRadii[heldObject] * 2.0f;
            if (cameraDistance < minDistance) { // Dynamic minimum zoom in limit
                cameraDistance = minDistance;
            }
        }
        else {
            // General zoom in
            cameraDistance -= 0.5f; // Smaller zoom step
            // Clamp general zoom
            if (cameraDistance < 2.0f) { // Lowered minimum zoom in limit
                cameraDistance = 2.0f;
            }
        }
        glutPostRedisplay();
    }
    else if (button == 4 && state == GLUT_DOWN) { // Mouse wheel down
        if (heldObject != -1) {
            // Zoom out from the held object
            cameraDistance += 0.5f; // Smaller zoom step
            // Clamp zoom level to prevent excessive zooming
            if (cameraDistance > 100.0f) { // Maximum zoom out limit
                cameraDistance = 100.0f;
            }
        }
        else {
            // General zoom out
            cameraDistance += 0.5f; // Smaller zoom step
            // Clamp general zoom
            if (cameraDistance > 100.0f) {
                cameraDistance = 100.0f;
            }
        }
        glutPostRedisplay();
    }
    else if (button == GLUT_LEFT_BUTTON) { // Left mouse button
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = static_cast<float>(x); // Record initial mouse position
            lastMouseY = static_cast<float>(y);
        }
        else if (state == GLUT_UP) {
            isDragging = false; // Stop dragging
        }
    }
}

// Mouse Drag Function
void mouseDrag(int x, int y) {
    if (isDragging) {
        float deltaX = static_cast<float>(x) - lastMouseX; // Calculate horizontal movement
        float deltaY = static_cast<float>(y) - lastMouseY; // Calculate vertical movement

        cameraYaw += deltaX * 0.5f;    // Adjust horizontal angle (sensitivity 0.5)
        cameraPitch += deltaY * 0.5f;  // Adjust vertical angle (sensitivity 0.5)

        // Clamp the pitch to prevent flipping
        if (cameraPitch > 89.0f) cameraPitch = 89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;

        lastMouseX = static_cast<float>(x); // Update last mouse position
        lastMouseY = static_cast<float>(y);

        glutPostRedisplay(); // Request redraw
    }
}

// Keyboard press handler
void keyboardPressHandler(unsigned char key, int x, int y) {
    if (key == 27) { // 27 is the ASCII code for ESC
        std::cout << "ESC key pressed. Exiting the program." << std::endl;
        cleanup(); // Perform cleanup
        exit(0);    // Terminate the program
    }

    if (key >= '0' && key <= '8') {
        heldObject = key - '0'; // Convert char to int
        std::cout << "Held Object Selected: " << heldObject << " (" << planetNames[heldObject] << ")" << std::endl;
    }

    // Handle orbit rotation keys
    if (key == '+' || key == '-') {
        // Orbit rotation
        if (key == '+') {
            orbitAngle += 1.0f; // Increment orbit angle by 1 degree
            planetRotationAngles[0] += planetRotationSpeeds[0]; // Rotate Sun on its axis
        }
        else if (key == '-') {
            orbitAngle -= 1.0f; // Decrement orbit angle by 1 degree
            planetRotationAngles[0] -= planetRotationSpeeds[0]; // Rotate Sun on its axis
        }

        // Ensure rotation angles stay within [0, 360) degrees
        if (orbitAngle >= 360.0f) orbitAngle -= 360.0f;
        if (orbitAngle < 0.0f) orbitAngle += 360.0f;

        if (planetRotationAngles[0] >= 360.0f) planetRotationAngles[0] -= 360.0f;
        if (planetRotationAngles[0] < 0.0f) planetRotationAngles[0] += 360.0f;

        glutPostRedisplay(); // Request redraw
    }
}

// Keyboard release handler
void keyboardReleaseHandler(unsigned char key, int x, int y) {
    if (key >= '0' && key <= '8') {
        std::cout << "Held Object Released: " << (key - '0') << std::endl;
        heldObject = -1; // Reset held object
    }
}

// Initialize OpenGL settings
void initOpenGL() {
    glEnable(GL_DEPTH_TEST);                // Enable depth testing
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Set background to black

    // Enable Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up light parameters
    GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Position the light at the Sun's location
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);    // Position the light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);  // Set ambient light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);  // Set diffuse light
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);// Set specular light

    glEnable(GL_COLOR_MATERIAL);                    // Enable color tracking

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 800.0f / 600.0f, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    // Load textures for Sun and the 8 planets
    std::array<std::string, 9> texturePaths = {
        "texture/sun.bmp",      // textures[0]
        "texture/mercury.bmp",  // textures[1]
        "texture/venus.bmp",    // textures[2]
        "texture/earth.bmp",    // textures[3]
        "texture/mars.bmp",     // textures[4]
        "texture/jupiter.bmp",  // textures[5]
        "texture/saturn.bmp",   // textures[6]
        "texture/uranus.bmp",   // textures[7]
        "texture/neptune.bmp"   // textures[8]
    };

    backgroundTexture = loadBMPTexture("texture/milkyway.bmp");
    if (!backgroundTexture) {
        std::cerr << "Failed to load background texture: texture/milkyway.bmp" << std::endl;
    }

    for (int i = 0; i < 9; ++i) { // Load 9 textures
        textures[i] = loadBMPTexture(texturePaths[i].c_str());
        if (!textures[i]) {
            std::cerr << "Failed to load texture: " << texturePaths[i] << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Solar System with Enhanced Camera Control");

    initOpenGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseHandler);                // Handle mouse wheel and button events
    glutMotionFunc(mouseDrag);                  // Handle mouse drag
    glutKeyboardFunc(keyboardPressHandler);     // Handle key presses
    glutKeyboardUpFunc(keyboardReleaseHandler); // Handle key releases

    glutMainLoop();

    return 0;
}
