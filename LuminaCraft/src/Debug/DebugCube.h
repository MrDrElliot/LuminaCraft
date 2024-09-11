#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

struct DebugCube
{
    unsigned int VAO, VBO, EBO;

    DebugCube()
    {
        // Define the vertices for a cube
        float vertices[] = {
            // Positions
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f
        };
        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,  // Front face
            4, 5, 6, 6, 7, 4,  // Back face
            0, 1, 7, 7, 6, 5,  // Left face
            1, 2, 6, 6, 7, 1,  // Right face
            2, 3, 5, 5, 6, 2,  // Top face
            0, 3, 4, 4, 5, 3   // Bottom face
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void Draw(const glm::vec3& position, const glm::mat4& view, const glm::mat4& projection)
    {
        // Set up shaders and uniform matrices here
        // e.g., shader.use();
        // shader.setMat4("view", view);
        // shader.setMat4("projection", projection);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        // shader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};
