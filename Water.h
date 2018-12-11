
#ifndef Water_h
#define Water_h

#include "Node.h"
#include "Window.h"


class Water:public Node
{
private:
    //Water properties.
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
    GLuint shader;
    
    
    
    //Function that defines the bezier patches
    void defineBezierPatches();
    
    //Function that finds the interpolated q point
    glm::vec3 getUBezier(float t, int row);
    
    //Function that finds the interpolated x point from bezier curve
    // represented by four q points
    glm::vec3 getX(float u, float v);
    
    
public:
    Water(int x, int z, GLuint shader);
    ~Water();
    
    void draw(GLuint);
    

    GLuint VBO, VAO, EBO, normalBuffer;
    glm::mat4 toWorld;
    
    //Determine the Water's position in the world.
    int x, z;
    
    //Node functions
    void draw(GLuint shaderProgram, glm::mat4 C);
    void update();
    void shiftAndResizeSphere();
    void updateMinMaxCoordinates(float x, float y, float z);
    
    
    //Define number of vertices (value squared)
    unsigned int numVertices = 300;
    
    //Define the xyz coordinates of the water
    int water_y = 3;
    int water_xz = 150;
    
    
    
    /// Defines the water surface, with control points
    glm::vec3 Points[4][4] = {
        {
            { (2 * water_xz), water_y, (2 * water_xz) },
            { water_xz, water_y, (2 * water_xz) },
            { -water_xz, water_y, (2 * water_xz) },
            {-(2*water_xz), water_y, (2*water_xz) }
        },
        {
            { (2*water_xz), water_y, water_xz },
            {  water_xz, water_y, water_xz },
            { -water_xz, water_y, water_xz },
            {-(2*water_xz), water_y, water_xz }
        },
        {
            { (2*water_xz), water_y, -water_xz },
            {  water_xz, water_y, -water_xz },
            { -water_xz, water_y, -water_xz },
            {-(2*water_xz), water_y, -water_xz }
        },
        {
            { (2*water_xz), water_y,-(2*water_xz) },
            {  water_xz, water_y,-(2*water_xz) },
            { -water_xz, water_y,-(2*water_xz) },
            {-(2*water_xz), water_y, -(2*water_xz) }
        }
    };
    

};

#endif
