#include "labelatlas.h"

LabelAtlas::LabelAtlas() :
    fbo(new QOpenGLFramebufferObject(0, 20, GL_TEXTURE_2D))
{

}

void LabelAtlas::bind()
{
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
}
