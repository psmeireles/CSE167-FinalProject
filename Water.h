
#ifndef Water_h
#define Water_h

#include "Node.h"
#include "Window.h"

//A struct to hold a control point of the surface.
struct Point {
    float x;
    float y;
    float z;
};

class Water:public Node
{
private:
    //Water properties.
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<int> indices;
    unsigned int level_of_detail;
    //GLSL properties.
    int draw_mode;
    glm::mat4 toWorld;
    GLuint VBO, VAO, EBO, VBONORM;
    //Intialization functions.
    void setupGeometry();
    Point CalculateU(float t, int row);
    Point CalculateV(float t, Point* pnts);
    Point Calculate(float u, float v);
    void setupWater();
    
public:
    Water(int x_d, int z_d, GLuint shader);
    ~Water();
    
    GLuint skyTexture;
    void draw(GLuint);
    
    //To hold our shader
    GLuint shader;
    
    //Determine the Water's position in the world.
    int x, z;
    
    //Node functions
    void draw(GLuint shaderProgram, glm::mat4 C);
    void update();
    void shiftAndResizeSphere();
    void updateMinMaxCoordinates(float x, float y, float z);
    

};

#endif
