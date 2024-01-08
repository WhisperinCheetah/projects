#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "raylib.h"

#define AMOUNT_OF_POINTS 5000
#define AMOUNT_OF_CLUSTERS 20
#define WINDOW_X 1600
#define WINDOW_Y 900
#define RANGE 800
#define CUBE_SIZE 10.0f
#define MOUSE_SPEED CUBE_SIZE
#define UNIFORM true
#define CENTER_CENTROIDS false

typedef struct _Cluster {
    int amount;
    int size;
    Vector3* cluster_points;
} Cluster;

double distance_between_points(Vector3, Vector3);
int find_nearest_centroid(Vector3, Vector3*, int);
Vector3 calculate_new_centroid(Cluster);
Vector3* generate_points(int, int, int, int);
Vector3* generate_uniform_points(int, int);
Vector3* init_centroids(Vector3*, int, int);
Vector3* init_centroids_center(int);
Cluster init_empty_cluster();
Cluster* init_empty_clusters(int);
void add_point_to_cluster(Vector3, int, Cluster*);
void draw_points(Vector3*, int);
void draw_centroids(Vector3*, int);
void draw_clusters(Cluster*, int);
void print_points(Vector3*, int);
void free_clusters(Cluster* clusters, int k);


int main() {
    InitWindow(WINDOW_X, WINDOW_Y, "K-means clustering in 3D"); // GetRandomValue seed is created with initwindow
    SetRandomSeed(time(NULL));

    Vector3* points;
    if (UNIFORM) points = generate_uniform_points(AMOUNT_OF_POINTS, RANGE);
    else points = generate_points(AMOUNT_OF_CLUSTERS, AMOUNT_OF_POINTS, WINDOW_X, WINDOW_Y);

    Vector3* centroids;
    if (CENTER_CENTROIDS) centroids = init_centroids_center(AMOUNT_OF_CLUSTERS);
    else centroids = init_centroids(points, AMOUNT_OF_POINTS, AMOUNT_OF_CLUSTERS);
    Cluster* clusters = NULL;

    float camera_theta = 0.0f;
    float camera_phi = 0.0f;
    float camera_mag = CUBE_SIZE*10;
    float camera_mag_velocity = 0.0f;

    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        camera_mag += camera_mag_velocity;
        camera_mag_velocity += GetMouseWheelMove() * CUBE_SIZE * 2;
        camera_mag_velocity *= 0.9;

        if (camera_mag < 1.0f) camera_mag = 1.0f;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float dt = GetFrameTime();
            Vector2 delta = GetMouseDelta();
            camera_phi += delta.x*0.01;
            camera_theta += delta.y*0.01;
        }


        if (IsKeyPressed(KEY_N)) {
            clusters = init_empty_clusters(AMOUNT_OF_CLUSTERS);
            for (int i = 0; i < AMOUNT_OF_POINTS; i++) {
                int nearest_centroid = find_nearest_centroid(points[i], centroids, AMOUNT_OF_CLUSTERS);
                add_point_to_cluster(points[i], nearest_centroid, clusters);
            }

            for (int i = 0; i < AMOUNT_OF_CLUSTERS; i++) {
                centroids[i] = calculate_new_centroid(clusters[i]);
            }
        }

        if (IsKeyPressed(KEY_H)) {
            free(centroids);
            if (clusters != NULL) free_clusters(clusters, AMOUNT_OF_CLUSTERS);

            if (CENTER_CENTROIDS) centroids = init_centroids_center(AMOUNT_OF_CLUSTERS);
            else centroids = init_centroids(points, AMOUNT_OF_POINTS, AMOUNT_OF_CLUSTERS);

            clusters = NULL;
        }

        if (IsKeyPressed(KEY_R)) {
            free(centroids);
            free(points);
            if (clusters != NULL) free_clusters(clusters, AMOUNT_OF_CLUSTERS);

            SetRandomSeed(time(NULL));

            if (UNIFORM) points = generate_uniform_points(AMOUNT_OF_POINTS, RANGE);
            else points = generate_points(AMOUNT_OF_CLUSTERS, AMOUNT_OF_POINTS, WINDOW_X, WINDOW_Y);

            if (CENTER_CENTROIDS) centroids = init_centroids_center(AMOUNT_OF_CLUSTERS);
            else centroids = init_centroids(points, AMOUNT_OF_POINTS, AMOUNT_OF_CLUSTERS);

            clusters = NULL;
        }

        Camera3D camera = { 
            .position = {
                .x = camera_mag * sin(camera_theta) * cos(camera_phi),
                .y = camera_mag * sin(camera_theta) * sin(camera_theta),
                .z = camera_mag * cos(camera_theta),
            },
            .target = { 0.0f, 0.0f, 0.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .fovy = 90.0f,
            .projection = CAMERA_PERSPECTIVE,
        };

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
                if (clusters == NULL) draw_points(points, AMOUNT_OF_POINTS);
                else draw_clusters(clusters, AMOUNT_OF_CLUSTERS);
                draw_centroids(centroids, AMOUNT_OF_CLUSTERS);
            EndMode3D();
        EndDrawing();
    }

    free(centroids);
    free(points);
    free_clusters(clusters, AMOUNT_OF_CLUSTERS);

    return 0;
}

void free_clusters(Cluster* clusters, int k) {
    for (int i = 0; i < k; i++) {
        free(clusters[i].cluster_points);
    }
    free(clusters);
}

Vector3 calculate_new_centroid(Cluster cluster) {
    float new_x = 0;
    float new_y = 0;
    float new_z = 0;
    for (int i = 0; i < cluster.amount; i++) {
        new_x += cluster.cluster_points[i].x;
        new_y += cluster.cluster_points[i].y;
        new_z += cluster.cluster_points[i].z;
    }

    new_x /= cluster.amount;
    new_y /= cluster.amount;
    new_z /= cluster.amount;

    Vector3 new_centroid = {new_x, new_y, new_z};
    return new_centroid;
}

void add_point_to_cluster(Vector3 point, int centroid_index, Cluster* clusters) {
    clusters[centroid_index].cluster_points[clusters[centroid_index].amount] = point;
    clusters[centroid_index].amount++;
}

int find_nearest_centroid(Vector3 point, Vector3* centroids, int k) {
    double min_distance = distance_between_points(point, centroids[0]);
    int min_dist_index = 0;

    for (int i = 1; i < k; i++) {
        double distance = distance_between_points(point, centroids[i]);
        if (distance < min_distance) {
            min_distance = distance;
            min_dist_index = i;
        }
    }

    return min_dist_index;
}

double distance_between_points(Vector3 a, Vector3 b) {
    return (double)(b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y) + (b.z - a.z)*(b.z - a.z);
}

Vector3* init_centroids(Vector3* points, int amount, int k) {
    if (amount < k) return NULL;

    int* chosen = LoadRandomSequence(k, 0, amount-1);
    Vector3* centoids = (Vector3*)malloc(sizeof(Vector3) * k);
    for (int i = 0; i < k; i++) {
        centoids[i] = points[chosen[i]];
    }

    return centoids;
}

Vector3* init_centroids_center(int k) { // TODO
    Vector3* centoids = (Vector3*)malloc(sizeof(Vector3) * k);
    for (int i = 0; i < k; i++) {
        Vector3 point = { (WINDOW_X / 2) + i * 5, (WINDOW_Y / 2) + i * 5};
        centoids[i] = point;
    }
    return centoids;
}

Cluster* init_empty_clusters(int k) {
    Cluster* clusters = (Cluster*)malloc(sizeof(Cluster) * k);
    for (int i = 0; i < k; i++) {
        clusters[i] = init_empty_cluster();
    }

    return clusters;
}

Cluster init_empty_cluster() {
    Vector3* points_array = (Vector3*)malloc(sizeof(Vector3) * AMOUNT_OF_POINTS);
    Cluster cluster = {
        .amount = 0,
        .size = AMOUNT_OF_POINTS,
        .cluster_points = points_array
    };

    return cluster;
}

void draw_points(Vector3* points, int amount) { 
    for (int i = 0; i < amount; i++)
    {
        DrawCube(points[i], CUBE_SIZE, CUBE_SIZE, CUBE_SIZE, MAGENTA);
    }
}

void draw_centroids(Vector3* centroids, int k) { 
    for (int i = 0; i < k; i++)
    {
        DrawCube(centroids[i], CUBE_SIZE*2, CUBE_SIZE*2, CUBE_SIZE*2, WHITE);
    }
}

void draw_clusters(Cluster* clusters, int k) { 
    Color colors[] = {
         LIGHTGRAY
        ,DARKBLUE
        ,YELLOW
        ,PINK
        ,RED
        ,MAROON
        ,GRAY
        ,DARKGRAY
        ,GOLD
        ,ORANGE
        ,GREEN
        ,LIME
        ,DARKGREEN
        ,SKYBLUE
        ,BLUE
        ,PURPLE
        ,VIOLET
        ,DARKPURPLE
        ,BEIGE
        ,BROWN
        ,DARKBROWN
        ,WHITE
        ,MAGENTA
        ,RAYWHITE
    };
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < clusters[i].amount; j++) {
            DrawCube(clusters[i].cluster_points[j], CUBE_SIZE, CUBE_SIZE, CUBE_SIZE, colors[i%22]);
        }
    }
}

Vector3* generate_points(int k, int amount, int rangeX, int rangeY) { // TODO
    Vector3* array = (Vector3*)malloc(sizeof(Vector3) * amount);

    int index = 0;
    for (int j = 0; j < k; j++) {
        int c_x = GetRandomValue(100, rangeX - 100);
        int c_y = GetRandomValue(100, rangeY - 100);

        double radius = 150.0;

        for (int i = 0; i < amount/k; i++) {
            double dist = sqrt(((double)GetRandomValue(0, 1000)) / 1000.0);
            double r = radius * dist * dist;
            double theta = (((double)GetRandomValue(0, 1000)) / 1000.0) * 2 * (double)PI;

            int x = c_x + (int)floor(r * cos(theta));
            int y = c_y + (int)floor(r * sin(theta));
            Vector3 point = {x, y};
            array[index] = point;
            index++;
        }
    }

    return array;
}

Vector3* generate_uniform_points(int amount, int range) {
    Vector3* array = (Vector3*)malloc(sizeof(Vector3) * amount);

    for (int i = 0; i < amount; i++) {
        Vector3 point = {0};

        point.x = (float)GetRandomValue(-range, range);
        point.y = (float)GetRandomValue(-range, range);
        point.z = (float)GetRandomValue(-range, range);

        array[i] = point;
    }

    return array;
}

void print_points(Vector3* points, int amount) {
    for (int i = 0; i < amount; i++) {
        printf("Point: x=%f, y=%f, z=%f\n", points[i].x, points[i].y, points[i].z);
    }
}