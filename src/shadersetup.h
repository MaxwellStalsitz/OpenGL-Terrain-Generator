#pragma once

#include "Source.h"
#include "Shader.h"
#include "cameraMovement.h"

void setUpShaderVariables(Shader * shader){

    shader->setBool("isFlat", isFlat);
    shader->setVec3("lightPos", lightPos);
    shader->setVec3("cameraPos", cameraPos);
    shader->setVec3("sunDir", sunDir);
    shader->setVec3("sunColor", sunColor);
    shader->setVec3("skyColor", skyColor);

    shader->setBool("isSpecular", isSpecular);
    shader->setFloat("specularStrength", specularStrength);

    shader->setFloat("sunIntensity", sunIntensity);
    shader->setFloat("ambientStrength", ambientStrength);
}