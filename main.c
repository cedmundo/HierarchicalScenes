#include <raylib.h>
#include <raymath.h>
#include <flecs.h>

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800
#define WINDOW_TITLE "HierarchicalScenes"

typedef struct {
    Transform local;
    Matrix world;
} ObjectTransform;

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

void UpdateObjectTransforms(ecs_iter_t *it) {
    ObjectTransform *parents = ecs_field(it, ObjectTransform, 1);
    ObjectTransform *objects = ecs_field(it, ObjectTransform, 2);
    for (int i = 0; i < it->count; i++) {
        Vector3 translation = objects[i].local.translation;
        Vector3 scale = objects[i].local.scale;
        Matrix r = QuaternionToMatrix(objects[i].local.rotation);

        Matrix s = MatrixScale(scale.x, scale.y, scale.z);
        Matrix t = MatrixTranslate(translation.x, translation.y, translation.z);
        Matrix st = MatrixMultiply(s, t);
        Matrix str = MatrixMultiply(r, st);
        objects[i].world = MatrixMultiply(str, parents[i].world);
    }
}

void DrawModels(ecs_iter_t *it) {
    Model *models = ecs_field(it, Model, 1);
    ObjectTransform *transforms = ecs_field(it, ObjectTransform, 2);
    for (int i = 0; i < it->count; i++) {
        models[i].transform = transforms[i].world;
        DrawModel(models[i], Vector3Zero(), 1.0f, WHITE);
    }
}

void UpdateRotatingA(ecs_iter_t *it) {
    ObjectTransform *transforms = ecs_field(it, ObjectTransform, 1);
    for (int i = 0; i < it->count; i++) {
        Quaternion base = transforms[i].local.rotation;
        Quaternion incr = QuaternionFromEuler(0.0f, DEG2RAD * 50.0f * GetFrameTime(), 0.0f);
        transforms[i].local.rotation = QuaternionMultiply(base, incr);
    }
}

void UpdateRotatingB(ecs_iter_t *it) {
    ObjectTransform *transforms = ecs_field(it, ObjectTransform, 1);
    for (int i = 0; i < it->count; i++) {
        Quaternion base = transforms[i].local.rotation;
        Quaternion incr = QuaternionFromEuler(DEG2RAD * 50.0f * GetFrameTime(), 0.0f, 0.0f);
        transforms[i].local.rotation = QuaternionMultiply(base, incr);
    }
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
    ECS_COMPONENT(world, Model);
    ECS_COMPONENT(world, ObjectTransform);

    // TAGS
    ECS_TAG(world, ActiveCamera);
    ECS_TAG(world, RotateA);
    ECS_TAG(world, RotateB);

    // RELATIONSHIPS
    // SINGLETONS
    // SYSTEMS
    ECS_SYSTEM(world, PreUpdate, EcsPreFrame);
    ECS_SYSTEM(world, PreActiveCamera, EcsPreFrame, ActiveCamera, Camera3D);
    ECS_SYSTEM(world, PostActiveCamera, EcsPostFrame, ActiveCamera);
    ECS_SYSTEM(world, PostUpdate, EcsPostFrame);

    ecs_system_init(world, &(ecs_system_desc_t) {
            .entity = ecs_entity(world, { .name = "ObjectTransform", .add = {ecs_dependson(EcsOnUpdate)}}),
            .query.filter.expr = "ObjectTransform(parent|cascade), ObjectTransform",
            .callback = UpdateObjectTransforms,
    });
    ECS_SYSTEM(world, DrawModels, EcsOnUpdate, Model, ObjectTransform);
    ECS_SYSTEM(world, UpdateRotatingA, EcsOnUpdate, ObjectTransform, RotateA);
    ECS_SYSTEM(world, UpdateRotatingB, EcsOnUpdate, ObjectTransform, RotateB);

    // OBSERVERS
    // ENTITIES
    ecs_entity_t camera_entity = ecs_entity(world, {});
    ecs_set(world, camera_entity, Camera3D, {
        .position = (Vector3) {10, 10, 10},
        .target = (Vector3) {0, 0, 0},
        .up = (Vector3) {0, 1.0f, 0},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE,
    });
    ecs_add(world, camera_entity, ActiveCamera);

    ecs_entity_t root_entity = ecs_entity(world, {});
    ecs_set(world, root_entity, ObjectTransform, {
        .world = MatrixTranslate(0, 0, 0),
        .local = {
                .scale = Vector3One(),
                .translation = Vector3Zero(),
                .rotation = QuaternionIdentity(),
        },
    });

    ecs_entity_t parent_0 = ecs_entity(world, {});
    ecs_add_pair(world, parent_0, EcsChildOf, root_entity);
    ecs_add(world, parent_0, RotateA);
    ecs_set(world, parent_0, ObjectTransform, {
        .world = MatrixTranslate(0, 0, 0),
        .local = {
                .scale = Vector3One(),
                .translation = Vector3Zero(),
                .rotation = QuaternionIdentity(),
        },
    });

    ecs_entity_t parent_1 = ecs_entity(world, {});
    ecs_add_pair(world, parent_1, EcsChildOf, parent_0);
    ecs_add(world, parent_1, RotateB);
    ecs_set(world, parent_1, ObjectTransform, {
        .world = MatrixTranslate(0, 0, 0),
        .local = {
                .scale = Vector3One(),
                .translation = Vector3Zero(),
                .rotation = QuaternionIdentity(),
        },
    });

    ecs_entity_t maxwell_entity = ecs_entity(world, {});
    ecs_add_pair(world, maxwell_entity, EcsChildOf, parent_1);
    ecs_add(world, maxwell_entity, RotateA);
    ecs_set(world, maxwell_entity, ObjectTransform, {
        .world = MatrixIdentity(),
        .local = {
                .scale = Vector3Scale(Vector3One(), 0.1f),
                .translation = Vector3Zero(),
                .rotation = QuaternionFromEuler(DEG2RAD * -90.0f, 0.0f, 0.0f),
        }
    });
    Model maxwell_model = LoadModel("assets/maxwell_cat.gltf");
    ecs_set_ptr(world, maxwell_entity, Model, &maxwell_model);

    ecs_app_run(world, &(ecs_app_desc_t) {
            .target_fps = 60,
            .enable_monitor = true,
            .enable_rest = true,
    });

    UnloadModel(maxwell_model);
    CloseWindow();
    return 0;
}
