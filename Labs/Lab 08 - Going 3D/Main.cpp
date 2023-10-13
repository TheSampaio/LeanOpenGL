// Pre-compiled headers
#include "PCH.h"

// Project includes
#include "Window.h"
#include "Renderer.h"
#include "Timer.h"

int main()
{
    // Gets all the static instance references
    Window& Window = Window::GetInstance();
    Renderer& Renderer = Renderer::GetInstance();
    Timer& Timer = Timer::GetInstance();

    // Set-ups the window
    Window.SetSize(1360, 768);
    Window.SetTitle("Window | OpenGL");
    Window.SetVerticalSynchronization(true);
    Window.SetBackgroundColour(0.2f, 0.2f, 0.4f);

    // Creates the window
    Window.Create();

    // Vertices's array
    const std::vector<GLfloat> Vertices
    {
        // === Positions       // === Colours         // === UVs      // === Indices
        -0.5f,  0.0f,  0.5f,    1.0f,  1.0f,  1.0f,    0.0f, 0.0f,    //  0
        -0.5f,  0.0f, -0.5f,    1.0f,  1.0f,  1.0f,    5.0f, 0.0f,    //  1
         0.5f,  0.0f, -0.5f,    1.0f,  1.0f,  1.0f,    0.0f, 0.0f,    //  2
         0.5f,  0.0f,  0.5f,    1.0f,  1.0f,  1.0f,    5.0f, 0.0f,    //  3
         0.0f,  0.8f,  0.0f,    1.0f,  1.0f,  1.0f,    2.5f, 5.0f     //  4
    };

    // Indices's dynamic array
    const std::vector<GLuint> Indices
    {
       0, 1, 2,
       0, 2, 3,
       0, 1, 4,
       1, 2, 4,
       2, 3, 4,
       3, 0, 4
    };

    // Creates a shader program using files for the vertex and fragment shaders
    Shader* DefaultProgram = new Shader("DefaultVert.glsl", "DefaultFrag.glsl");

    // Creates all buffer objects
    VAO* VertexArray = new VAO();
    VBO* VertexBuffer = new VBO(Vertices);
    EBO* ElementBuffer = new EBO(Indices);

    // Loads and creates a texture
    Texture* Sandbrick = new Texture("../../Resources/Textures/diffuse-sandbrick-01.png", GL_RGB);

    // Set-ups VAO's layouts
    VertexArray->AttribPointer(0, 3, 8 * sizeof(GLfloat), 0);                   // Position
    VertexArray->AttribPointer(1, 3, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat)); // Colour
    VertexArray->AttribPointer(3, 2, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat)); // UV

    // Unbinds all buffers to avoid bugs
    VertexBuffer->Unbind();
    VertexArray->Unbind();
    ElementBuffer->Unbind();

    // Starts the timer
    Timer.Start();

    // Main loop (Game loop)
    while (!Window.Close())
    {
        // Resets timer and updates delta time
        Timer.Reset();

        // Process all window's events and clear all buffers
        Window.ProcessEvents();
        Window.ClearBuffers();

        {
            // Informs OpenGL which shader program and VAO we want to use
            DefaultProgram->Bind();
            VertexArray->Bind();

            // Creates MVP matrix
            glm::mat4 Model = glm::mat4(1.0f);
            glm::mat4 View = glm::mat4(1.0f);
            glm::mat4 Projection = glm::mat4(1.0f);

            // Set-ups MVP matrix
            Model = glm::rotate(Model, glm::radians(50.0f * Timer.GetDeltaTime()), glm::vec3{ 0.0f, 1.0f, 0.0f });
            View = glm::translate(View, glm::vec3{ 0.0f, -0.25f, -1.2f });
            Projection = glm::perspective(glm::radians(60.0f), static_cast<float>(Window.GetSize()[0]) / static_cast<float>(Window.GetSize()[1]), 0.1f, 100.0f);

            // Send data from CPU to GPU by using uniforms
            Renderer.SetUniformMatrix4fv(*DefaultProgram, "Model", Model);
            Renderer.SetUniformMatrix4fv(*DefaultProgram, "View", View);
            Renderer.SetUniformMatrix4fv(*DefaultProgram, "Projection", Projection);

            // Set-ups texture's uniform and binds the texture
            Renderer.SetUniform1i(*DefaultProgram, "DiffuseSampler", 0);
            Sandbrick->Bind();

            // Draw call command using indices
            Renderer.Draw(Indices);

            // Unbind everything binded to avoid bugs
            Sandbrick->Unbind();
            VertexArray->Unbind();
            DefaultProgram->Unbind();
        }

        // Swaps window's buffers
        Window.SwapBuffers();
    }

    // Deletes what we need anymore
    delete VertexBuffer;
    delete VertexArray;
    delete ElementBuffer;
    delete Sandbrick;
    delete DefaultProgram;

    return 0;
}