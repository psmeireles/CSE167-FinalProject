#include <stdio.h>
#include "Water.h"


Water::Water(int x, int z, GLuint shader)
{
    this->shader = shader;
    
    this->toWorld = glm::mat4(1.0f);
    
    //TODO this is where translation is done to the world; remove and add it to Transform?
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
    
    this->toWorld = translate*this->toWorld;
    
    //Define the bezier patches
    this->defineBezierPatches();
    
    //Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &normalBuffer);
    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);
    
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_DYNAMIC_DRAW);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
}



/* Deconstructor to safely delete when finished. */
Water::~Water()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &EBO);
}



// Define the Bezier surfaces.
void Water::defineBezierPatches()
{
    // Defining verticies, normals set to default "up" for now
    for (unsigned int i = 0; i < this->numVertices; ++i) {
        
        //Get a scalar u value u[0..1]
        float u = (float)i / (this->numVertices - 1);
        
        //Loop for the parametric v value, per section.
        for (unsigned int j = 0; j < this->numVertices; ++j) {
            
            //Get a scalar v value v[0..1]
            float v = (float)j / (this->numVertices - 1);
            
            //Get the point x(u,v) on the bezier curve
            glm::vec3 xOfuv = getX(u, v);
            
            //Add x(u,v) to list of vertices
            this->vertices.push_back(xOfuv);
            
            // set all normals to facing y direction
            // Fix later if time to implement refraction/ reflection correctly...
            this->normals.push_back((glm::vec3(0.0, 1.0, 0.0)));
        }
    }
    
    
    // Defining indices
    for (unsigned int x = 0; x < this->numVertices - 1; x++)
    {
        for (unsigned int z = 0; z < this->numVertices - 1; z++)
        {
            int topLeft = (x*this->numVertices) + z;
            int topRight = ((x + 1)*this->numVertices) + z;
            int bottomLeft = topLeft + 1;
            int bottomRight = topRight + 1;
            //Push back to indices.
            this->indices.push_back(topLeft);
            this->indices.push_back(bottomLeft);
            this->indices.push_back(topRight);
            this->indices.push_back(topRight);
            this->indices.push_back(bottomLeft);
            this->indices.push_back(bottomRight);
        }
    }
    
} // end of defineBezierPatches()



/* Perform Bezier Surface calculation. */
glm::vec3 Water::getX(float u, float v)
{
    glm::vec3 xOfuv;
    
    float t = v;
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    
    //First, evaluate 4 curves in the 'u' direction. The points will be stored in a temporary array.
    glm::vec3 q[4];
    
    // Evaluate four u-diretion Bezier curves at scalar value u[0..1]
    q[0] = getUBezier(u, 0);
    q[1] = getUBezier(u, 1);
    q[2] = getUBezier(u, 2);
    q[3] = getUBezier(u, 3);
    
    /* Use linear interpoloation to get our point x from the v-direction  */
    // Evaluate it at v[0..1]
    xOfuv.x = q[0].x * t2 * t + q[1].x * 3.0f * t2 * one_minus_t + q[2].x * 3.0f * t * one_minus_t2 + q[3].x * one_minus_t * one_minus_t2;
    xOfuv.y = q[0].y * t2 * t + q[1].y * 3.0f * t2 * one_minus_t + q[2].y * 3.0f * t * one_minus_t2 + q[3].y * one_minus_t * one_minus_t2;
    xOfuv.z = q[0].z * t2 * t + q[1].z * 3.0f * t2 * one_minus_t + q[2].z * 3.0f * t * one_minus_t2 + q[3].z * one_minus_t * one_minus_t2;
    
    //Return the adjusted point.
    return xOfuv;
} // end of getX



/* Use linear interpolation to get our point q from the u-direction */
glm::vec3 Water::getUBezier(float t, int row)
{
    glm::vec3 q;

    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    
    // Get the value q from interopolating points along u
    q.x = Points[row][0].x * t2 * t + Points[row][1].x * 3.0f * t2 * one_minus_t + Points[row][2].x * 3.0f * t * one_minus_t2 + Points[row][3].x * one_minus_t * one_minus_t2;
    q.y = Points[row][0].y * t2 * t + Points[row][1].y * 3.0f * t2 * one_minus_t + Points[row][2].y * 3.0f * t * one_minus_t2 + Points[row][3].y * one_minus_t * one_minus_t2;
    q.z = Points[row][0].z * t2 * t + Points[row][1].z * 3.0f * t2 * one_minus_t + Points[row][2].z * 3.0f * t * one_minus_t2 + Points[row][3].z * one_minus_t * one_minus_t2;
    return q;
} //end of getUBezier



/* Draw the water mesh. */
void Water::draw(GLuint shaderProgram)
{
    //Get the current time.
    float time = (float)glfwGetTime();
    
    GLuint uProjection = glGetUniformLocation(shader, "projection");
    GLuint uModel = glGetUniformLocation(shader, "model");
    GLuint uView = glGetUniformLocation(shader, "view");
    
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
    
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camPos.x, Window::camPos.y, Window::camPos.z);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //Bind for drawing.
    glBindVertexArray(VAO);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
    glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //Set it back to fill.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



void Water::draw(GLuint shaderProgram, glm::mat4 C) {
    
    glUseProgram(shader);
    
    // Calculate the combination of the model and view (camera inverse) matrices
    //glm::mat4 model = glm::inverse(Window::V)*C*toWorld;
    //glm::mat4 view = Window::V;
    
    //Get the current time.
    float time = (float)glfwGetTime();
    
    GLuint uProjection = glGetUniformLocation(shader, "projection");
    GLuint uModel = glGetUniformLocation(shader, "model");
    GLuint uView = glGetUniformLocation(shader, "view");
    
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camPos.x, Window::camPos.y, Window::camPos.z);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Now draw the OBJObject. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
    // Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, indices[0], indices.size());
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
    
    //Set it back to fill.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
}



void Water::update(){
    
}



void Water::shiftAndResizeSphere(){
    
}



void Water::updateMinMaxCoordinates(float x, float y, float z){
    
}
