#pragma once

// Project
#include "../includes/common_classes/OpenGLWindow.h"

class OpenGLWindow018 : public OpenGLWindow
{
public:
    void initializeScene() override;
    void renderScene() override;
    void updateScene() override;
    void releaseScene() override;
};