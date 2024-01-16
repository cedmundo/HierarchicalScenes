#include <raylib.h>
#include <flecs.h>
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500
#define WINDOW_TITLE "HierarchicalScenes"

void PreUpdate(ecs_iter_t *it) {
    (void) it;
    BeginDrawing();
    ClearBackground(BLACK);
}

void PostUpdate(ecs_iter_t *it) {
    EndDrawing();
    if (WindowShouldClose()) {
        ecs_quit(it->world);
    }
}

void DrawGUI(ecs_iter_t *it) {
    (void) it;
    DrawText("hello world", 100, 100, 12, WHITE);
}

int main(int argc, char **argv) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetWindowMonitor(0);

    // IMPORTS
    ECS_IMPORT(world, FlecsRest);
    ECS_IMPORT(world, FlecsMonitor);

    // COMPONENTS
    // TAGS
    // RELATIONSHIPS
    // SINGLETONS
    // SYSTEMS
    ECS_SYSTEM(world, PreUpdate, EcsPreFrame);
    ECS_SYSTEM(world, PostUpdate, EcsPostFrame);
    ECS_SYSTEM(world, DrawGUI, EcsOnUpdate);

    // OBSERVERS
    // ENTITIES
    ecs_app_run(world, &(ecs_app_desc_t) {
        .target_fps = 60,
        .enable_monitor = true,
        .enable_rest = true,
    });

    CloseWindow();
    return 0;
}
