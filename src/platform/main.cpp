#include <iostream>
#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>
#include "imguiThemes.h"

#include <gameMain.h>

int main()
{

    #if PRODUCTION_BUILD == 1 
        SetTraceLogLevel(LOG_NONE); // no raylib log output on the console
    #endif


	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Gothic");

    #pragma region imgui
    rlImGuiSetup(true);

    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.FontGlobalScale = 1.0;
    SetupImGuiAmberYellowStyle();

    // vars
    bool fullscreen = false;
    bool vsync = true;
    bool sound = true;

    int input_int = 0;
    float float_field[3] = { 0.0f, 0.0f, 0.0f };
    float col[3] = { 0.0f, 0.0f, 0.0f };
    float v_slided = 0.0f;
    float col_4[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    #pragma endregion

    if(!initGame()) {
        return 0;
    }

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        SetExitKey(KEY_NULL); // Disable esc from closing window
        SetTargetFPS(240);

        #pragma region imgui
        rlImGuiBegin();
        
        // docking 
        ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        ImGui::PopStyleColor(2);

        #pragma endregion

        if(!updateGame())
        {
            CloseWindow();
        }

        

        #pragma region imgui windows
        // ImGui::Begin("Test");
        
        // if(ImGui::Button("button")) {
        //     std::cout << "Text\n";
        // }

        // if(ImGui::Button("button2")) {
        //     std::cout << "different button\n";
        // }
        
        // // Checkbox
        // ImGui::Checkbox("Fullscreen", &fullscreen);
        // ImGui::Checkbox("VSync", &vsync);
        // ImGui::Checkbox("Sound", &sound);

        // // Radio button

        // ImGui::RadioButton("one", sound);
        // if (ImGui::CollapsingHeader("Tests"))
        // {
        // ImGui::InputInt("Input int Field", &input_int);

        // ImGui::DragFloat3("Drag Float field", float_field);
        // ImGui::VSliderFloat("vslider", ImVec2{30.0f,100.0f}, &v_slided, 0.0f, 20.0f, "%.3f");
        
        // ImGui::ColorEdit3("Color Edit", col);
        // ImGui::ColorPicker4("Color picker", col_4);
        // ImGui::ProgressBar(0.5f, ImVec2{400, 20});
        // ImGui::CollapsingHeader("Header");


        // }

        // if(ImGui::TreeNode("Node"))  {
        //     ImGui::Text("Node Element 1");
        //     ImGui::Text("Node Element 2");
        //     ImGui::Text("Node Element 3");

        //     ImGui::TreePop();
        // }

        // ImGui::BeginChild("Scrollable", ImVec2(300, 200), true);

        // for (int i = 0; i < 50; i++)
        // {
        //     ImGui::Text("Element %d", i);
        // }

        // ImGui::EndChild();

        // ImGui::End();

        // ImGui::Begin("second window");

        // ImGui::Text("text");
        // ImGui::Separator();
        // ImGui::NewLine();
        // static float a = 0;
        // ImGui::SliderFloat("slider", &a, 0, 1);



        // ImGui::End();
        renderImGui();
        #pragma endregion

        #pragma region imgui
        rlImGuiEnd();
        #pragma endregion
        EndDrawing();
    }
    
    #pragma region imgui
    rlImGuiShutdown();
    #pragma endregion
    CloseWindow();

    closeGame();

	return 0;
}