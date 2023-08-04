#ifndef MAINGUI_H
#define MAINGUI_H

#include "Source.h"
#include "meshGeneration.h"

ImFont* font1;
ImFont* font2;
ImFont* font3;
ImFont* font4;

ImGuiWindowFlags window_flags_default;
ImGuiWindowFlags window_flags_transparent = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

void styleInitialization() {

    // imgui initialization and customizing the color scheme
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
	
    char* fontPath = concatenateStrings(sourceDirectory, "/Resources/fonts/mainfont.ttf");
	
    font1 = io.Fonts->AddFontFromFileTTF(fontPath, 24.0f, nullptr); // normal font
    font2 = io.Fonts->AddFontFromFileTTF(fontPath, 12.0f, nullptr); // smaller font
    font3 = io.Fonts->AddFontFromFileTTF(fontPath, 30.0f, nullptr); // larger font
    font4 = io.Fonts->AddFontFromFileTTF(fontPath, 6.0f, nullptr); // smallest font

    // ------------------------------------------------------------------------

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };

    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.38f, 0.38f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.28f, 0.28f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };

    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_PlotLines] = ImVec4(1.f, 1.f, 1.f, 1.0f);

    // ------------------------------------------------------------------------

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 0.0;
    style.TabRounding = 0.0;
    style.ScrollbarRounding = 0.0;
    style.TabBorderSize = 1.0;
    style.FramePadding = ImVec2(8.0, 3.0);

    style.ItemSpacing = ImVec2(15.0, 4.0);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
}

bool centerButton(const char* label, float alignment = 0.5f) // from ocornut
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);
}

void smallSeperator() {
    ImGui::PushFont(font4);
    ImGui::Text("");
    ImGui::Separator();
    ImGui::Text("");
    ImGui::PopFont();
}

void debugWindow(ImVec2 windowSize){
    ImGui::Begin("Debug Window");

    ImGui::Text("Framerate: %i", (int)ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::Text("(ms/frame : %f)", deltaTime * 1000);

    ImGui::Text("Vertices: %i", terrainMesh.vertices.size());
    ImGui::Text("Indices: %i", terrainMesh.indices.size());
    ImGui::SameLine();
    ImGui::Text("(Triangles: %i)", terrainMesh.indices.size() / 3);

    ImGui::Text("");
    ImGui::Text("Camera Position: %f, %f, %f", cameraPos.x, cameraPos.y, cameraPos.z);
    
    ImGui::Text("Rendering Time: ");
    ImGui::SameLine();
    if (renderingTime != -1.0f) {
        ImGui::Text("%f", renderingTime);
    }
    else {
        ImGui::Text("N/A");
    }
    
    smallSeperator();

    // ------------------------------------------------------------------------
    //performance graph

    static float values[25] = {};
    static int values_offset = 0;
    static double refresh_time = 0.1;

    if (refresh_time == 0.0)
        refresh_time = ImGui::GetTime();

    while (refresh_time < ImGui::GetTime()) {
        values[values_offset] = (int)ImGui::GetIO().Framerate;
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
        refresh_time += 1.0f / 60.0f;
    }

    float average = 0.0f;

    for (int n = 0; n < IM_ARRAYSIZE(values); n++)
        average += values[n];
    average /= (float)IM_ARRAYSIZE(values);

    char overlay[32];
    sprintf_s(overlay, "Average: %f", average); //writing out fps average above graph

    ImGui::Text("FPS Graph:");

    if (ImGui::BeginChild("Graph", ImVec2(windowSize.x * 0.95, windowSize.y * 0.4), false, window_flags_default)) {
        ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, rollingMaximum, rollingMinimum, ImVec2(windowSize.x * 0.95, windowSize.y * 0.4));
        
        ImGui::EndChild();
    }
    
    ImGui::End();
}

void interactiveUI() {

    ImVec2 windowSize = ImVec2(screenWidth * 0.3, screenHeight * 0.95);

    if (!imguiInit) {
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(ImVec2(screenWidth - windowSize.x - screenWidth * 0.01, screenHeight * 0.0125));
    }

    windowSize = ImVec2(screenWidth * 0.325, screenHeight * 0.45);

    if (!imguiInit) {
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(ImVec2(screenWidth * 0.01, screenHeight * 0.01));
    }

    debugWindow(windowSize);

    // ------------------------------------------------------------------------

    ImGui::Begin("Terrain Parameters", nullptr, window_flags_default);

    static float lightPosVec3[3] = { lightPos.x, lightPos.y, lightPos.z };
    ImGui::InputFloat3("Light Position", lightPosVec3);
    lightPos = glm::vec3(lightPosVec3[0], lightPosVec3[1], lightPosVec3[2]);

    static float terrainSize[2] = { gridX, gridY };
    ImGui::InputFloat2("Terrain Size", terrainSize);
    gridX = (int)terrainSize[0];
    gridY = (int)terrainSize[1];

    static float noiseOffsetSlider[2] = { noiseOffset.x, noiseOffset.y };
    ImGui::DragFloat2("Offset", noiseOffsetSlider, 0.1f, -500.0f, 500.0f);
    noiseOffset = glm::vec2(noiseOffsetSlider[0], noiseOffsetSlider[1]);

    ImGui::Text("");

    ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 7.5f);
    ImGui::SliderFloat("Frequency", &frequency, 0.0f, 20.0f);
    ImGui::SliderFloat("Lacunarity", &lacunarity, 0.0f, 5.0f);
    ImGui::SliderFloat("Persistence", &persistence, 0.0f, 2.0f);
    ImGui::SliderInt("Octaves", &octaves, 0, 20);
    
    ImGui::Text("");

    ImGui::DragFloat("Noise Scale", &noiseScale, 0.01f, 0.01f, 100.0f);
    ImGui::DragFloat("Mesh Scale", &meshScale, 0.01f, 0.01f, 10.0f);

    ImGui::Text("");

    ImGui::Checkbox("Specular Enabled", &isSpecular);

    if (isSpecular) {
        ImGui::DragFloat("Specular Strength", &specularStrength, 0.01f, 0.0f, 1.0f);
    }

    ImGui::DragFloat("Ambient Strength", &ambientStrength, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Sun Strength", &sunIntensity, 0.01f, 0.0f, 10.0f);

    static float sunDirVec3[3] = { sunDir.x, sunDir.y, sunDir.z };
    ImGui::DragFloat3("Sun Direction", sunDirVec3, 0.01f, 0.0f, 1.0f);
    sunDir = glm::vec3(sunDirVec3[0], sunDirVec3[1], sunDirVec3[2]);

    static float sunColorVec3[3] = { sunColor.x * 255, sunColor.y * 255, sunColor.z * 255 };
    ImGui::DragFloat3("Sun Color", sunColorVec3, 0.01f, 0.0f, 255.0f);

    for (int i = 0; i < 3; i++) {
        sunColorVec3[i] = glm::clamp(sunColorVec3[i], 0.0f, 255.0f);
    }

    sunColor = glm::vec3(sunColorVec3[0] / 255.0f, sunColorVec3[1] / 255.0f, sunColorVec3[2] / 255.0f);

    static float skyColorVec3[3] = { skyColor.x * 255, skyColor.y * 255, skyColor.z * 255};
    ImGui::DragFloat3("Sky Color", skyColorVec3, 1.0f, 0.0f, 255.0f);

    for (int i = 0; i < 3; i++) {
        skyColorVec3[i] = glm::clamp(skyColorVec3[i], 0.0f, 255.0f);
    }

    skyColor = glm::vec3(skyColorVec3[0] / 255.0f, skyColorVec3[1] / 255.0f, skyColorVec3[2] / 255.0f);

    ImGui::Text("");

    ImGui::InputInt("Seed", &seed);
    seed = std::clamp(seed, -10000, 10000);

    if (ImGui::Button("Random Seed")) {
        seed = std::rand() % 20001 - 10000; 
    }

    ImGui::Separator();

    if (centerButton("Generate / Update Mesh", 0.5f)) {
        beforeTime = glfwGetTime();
        terrainMesh = updateMesh(terrainMesh);
    }

    imguiInit = true;
    ImGui::End();
}

#endif