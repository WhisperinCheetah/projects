#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "raylib.h"

#define AMOUNT_OF_POINTS 20000
#define AMOUNT_OF_CLUSTERS 10
#define WINDOW_X 1200
#define WINDOW_Y 800
#define UNIFORM false
#define CENTER_CENTROIDS false

typedef struct _Vector2Int {
    int x;
    int y;
} Vector2Int;

typedef struct _Cluster {
    int amount;
    int size;
    Vector2Int* cluster_points;
} Cluster;

long distance_between_points(Vector2Int, Vector2Int);
int find_nearest_centroid(Vector2Int, Vector2Int*, int);
Vector2Int calculate_new_centroid(Cluster);
Vector2Int* generate_points(int, int, int, int);
Vector2Int* generate_uniform_points(int, int, int);
Vector2Int* init_centroids(Vector2Int*, int, int);
Vector2Int* init_centroids_center(int);
Cluster init_empty_cluster();
Cluster* init_empty_clusters(int);
void add_point_to_cluster(Vector2Int, int, Cluster*);
void draw_points(Vector2Int*, int);
void draw_centroids(Vector2Int*, int);
void draw_clusters(Cluster*, int);
void print_points(Vector2Int*, int);
void free_clusters(Cluster* clusters, int k);


int main() {
    InitWindow(WINDOW_X, WINDOW_Y, "K-means clustering"); // GetRandomValue seed is created with initwindow
    SetRandomSeed(time(NULL));

    Vector2Int* points;
    if (UNIFORM) points = generate_uniform_points(AMOUNT_OF_POINTS, WINDOW_X, WINDOW_Y);
    else points = generate_points(AMOUNT_OF_CLUSTERS, AMOUNT_OF_POINTS, WINDOW_X, WINDOW_Y);

    Vector2Int* centroids;
    if (CENTER_CENTROIDS) centroids = init_centroids_center(AMOUNT_OF_CLUSTERS);
    else centroids = init_centroids(points, AMOUNT_OF_POINTS, AMOUNT_OF_CLUSTERS);
    Cluster* clusters = NULL; // clusters are of size 100, overflows not detected.

    while (!WindowShouldClose()) {
        BeginDrawing();
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

                if (UNIFORM) points = generate_uniform_points(AMOUNT_OF_POINTS, WINDOW_X, WINDOW_Y);
                else points = generate_points(AMOUNT_OF_CLUSTERS, AMOUNT_OF_POINTS, WINDOW_X, WINDOW_Y);

                if (CENTER_CENTROIDS) centroids = init_centroids_center(AMOUNT_OF_CLUSTERS);
                else centroids = init_centroids(points, AMOUNT_OF_POINTS, AMOUNT_OF_CLUSTERS);

                clusters = NULL;
            }

            Color color = {0, 0, 0, 1};
            ClearBackground(color);
            if (clusters == NULL) draw_points(points, AMOUNT_OF_POINTS);
            else draw_clusters(clusters, AMOUNT_OF_CLUSTERS);
            draw_centroids(centroids, AMOUNT_OF_CLUSTERS);
        EndDrawing();
    }
    CloseWindow();

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

Vector2Int calculate_new_centroid(Cluster cluster) {
    int new_x = 0;
    int new_y = 0;
    for (int i = 0; i < cluster.amount; i++) {
        new_x += cluster.cluster_points[i].x;
        new_y += cluster.cluster_points[i].y;
    }

    new_x /= cluster.amount;
    new_y /= cluster.amount;

    Vector2Int new_centroid = {new_x, new_y};
    return new_centroid;
}

void add_point_to_cluster(Vector2Int point, int centroid_index, Cluster* clusters) {
    clusters[centroid_index].cluster_points[clusters[centroid_index].amount] = point;
    clusters[centroid_index].amount++;
}

int find_nearest_centroid(Vector2Int point, Vector2Int* centroids, int k) {
    long min_distance = distance_between_points(point, centroids[0]);
    int min_dist_index = 0;

    for (int i = 1; i < k; i++) {
        long distance = distance_between_points(point, centroids[i]);
        if (distance < min_distance) {
            min_distance = distance;
            min_dist_index = i;
        }
    }

    return min_dist_index;
}

long distance_between_points(Vector2Int a, Vector2Int b) {
    return (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y);
}

Vector2Int* init_centroids(Vector2Int* points, int amount, int k) {
    if (amount < k) return NULL;

    int* chosen = LoadRandomSequence(k, 0, amount-1);
    Vector2Int* centoids = (Vector2Int*)malloc(sizeof(Vector2Int) * k);
    for (int i = 0; i < k; i++) {
        centoids[i] = points[chosen[i]];
    }

    return centoids;
}

Vector2Int* init_centroids_center(int k) {
    Vector2Int* centoids = (Vector2Int*)malloc(sizeof(Vector2Int) * k);
    for (int i = 0; i < k; i++) {
        Vector2Int point = { (WINDOW_X / 2) + i * 5, (WINDOW_Y / 2) + i * 5};
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
    Vector2Int* points_array = (Vector2Int*)malloc(sizeof(Vector2Int) * AMOUNT_OF_POINTS);
    Cluster cluster = {
        .amount = 0,
        .size = AMOUNT_OF_POINTS,
        .cluster_points = points_array
    };

    return cluster;
}

void draw_points(Vector2Int* points, int amount) {
    for (int i = 0; i < amount; i++)
    {
        if (points[i].x > 0 && points[i].x < WINDOW_X && points[i].y > 0 && points[i].y < WINDOW_Y) {
            DrawCircle(points[i].x, points[i].y, 3.0, MAGENTA); 
        }
    }
}

void draw_centroids(Vector2Int* centroids, int k) {
    for (int i = 0; i < k; i++)
    {
        DrawCircle(centroids[i].x, centroids[i].y, 10.0, WHITE);
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
            DrawCircle(clusters[i].cluster_points[j].x, clusters[i].cluster_points[j].y, 3.0, colors[i%22]);
        }
    }
}

Vector2Int* generate_points(int k, int amount, int rangeX, int rangeY) {
    Vector2Int* array = (Vector2Int*)malloc(sizeof(Vector2Int) * amount);

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
            Vector2Int point = {x, y};
            array[index] = point;
            index++;
        }
    }

    return array;
}

Vector2Int* generate_uniform_points(int amount, int rangeX, int rangeY) {
    Vector2Int* array = (Vector2Int*)malloc(sizeof(Vector2Int) * amount);

    for (int i = 0; i < amount; i++) {
        int x = GetRandomValue(0, rangeX);
        int y = GetRandomValue(0, rangeY);
        Vector2Int point = {x, y};
        array[i] = point;
    }

    return array;
}

void print_points(Vector2Int* points, int amount) {
    for (int i = 0; i < amount; i++) {
        printf("Point: x=%d, y=%d\n", points[i].x, points[i].y);
    }
}