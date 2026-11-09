#include <iostream>
#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Gothic");

    #pragma region imgui
    rlImGuiSetup(true);

    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.FontGlobalScale = 1.5;
    #pragma endregion

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        #pragma region imgui
        rlImGuiBegin();
        
        // docking 
        ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        ImGui::PopStyleColor(2);

        #pragma endregion

        DrawText("Welcome to the colony", 190, 200, 20, RED);



        #pragma region imgui windows
        ImGui::Begin("Test");

        ImGui::Text("Hello");
        ImGui::Button("button");

        if(ImGui::Button("button")) {
            std::cout << "Text\n";
        }

        if(ImGui::Button("button2")) {
            std::cout << "different button\n";
        }

        ImGui::End();

        ImGui::Begin("second window");

        ImGui::Text("text");
        ImGui::Separator();
        ImGui::NewLine();
        static float a = 0;
        ImGui::SliderFloat("slider", &a, 0, 1);

        ImGui::End();
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

	return 0;
}