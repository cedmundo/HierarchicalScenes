#include <raylib.h>
#include <flecs.h>
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800
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

void PreActiveCamera(ecs_iter_t *it) {
    Camera3D *camera = ecs_field(it, Camera3D, 2);
    // Use only first match of active camera
    BeginMode3D(*camera);
    DrawGrid(10, 1.0f);
}

void PostActiveCamera(ecs_iter_t *it) {
    (void) it;
    EndMode3D();
}

int main(int argc, char **argv) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetWindowMonitor(0);

    // IMPORTS
    ECS_IMPORT(world, FlecsRest);
    ECS_IMPORT(world, FlecsMonitor);

    // COMPONENTS
    ECS_COMPONENT(world, Camera3D);

    // TAGS
    ECS_TAG(world, ActiveCamera);

    // RELATIONSHIPS
    // SINGLETONS
    // SYSTEMS
    ECS_SYSTEM(world, PreUpdate, EcsPreFrame);
    ECS_SYSTEM(world, PreActiveCamera, EcsPreFrame, ActiveCamera, Camera3D);
    ECS_SYSTEM(world, PostActiveCamera, EcsPostFrame, ActiveCamera);
    ECS_SYSTEM(world, PostUpdate, EcsPostFrame);

    // OBSERVERS
    // ENTITIES
    ecs_entity_t camera_entity = ecs_entity(world, {});
    ecs_set(world, camera_entity, Camera3D, {
        .position = (Vector3) { 10, 10, 10 },
        .target = (Vector3) { 0, 0, 0},
        .up = (Vector3) { 0, 1.0f, 0 },
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE,
    });
    ecs_add(world, camera_entity, ActiveCamera);

    ecs_app_run(world, &(ecs_app_desc_t) {
        .target_fps = 60,
        .enable_monitor = true,
        .enable_rest = true,
    });

    CloseWindow();
    return 0;
}
