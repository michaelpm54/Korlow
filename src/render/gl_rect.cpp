#include <glm/gtc/type_ptr.hpp>

#include "render/gl_rect.h"

void rect_init(Rect *rect)
{
    glGenVertexArrays(1, &rect->vao);
    glGenBuffers(1, &rect->vbo);

    // clang-format off

	GLfloat vertices[] =
	{
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 0.0f, 0.0f, 1.0f, /* uv */ 1.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 0.0f,
	};

    // clang-format on

    glBindVertexArray(rect->vao);
    glBindBuffer(GL_ARRAY_BUFFER, rect->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6, vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 4));
    glBindVertexArray(GL_NONE);
}

void rect_free(Rect *rect)
{
    glDeleteVertexArrays(1, &rect->vao);
    glDeleteBuffers(1, &rect->vbo);
}

void rect_draw(Rect *rect, GLuint texture, GLuint program, const glm::mat4 &projection, const glm::mat4 &scale)
{
    glUseProgram(program);
    glBindVertexArray(rect->vao);

    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(scale));

    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(GL_NONE);
    glUseProgram(GL_NONE);
}
