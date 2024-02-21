#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "raylib.h"

#define AMOUNT_OF_POINTS 5000
#define WINDOW_X 1600
#define WINDOW_Y 900
#define RANGE 800
#define CUBE_SIZE 2.0f

typedef struct _Coordinate {
    int x;
    int y;
} Coordinate;

typedef struct _Point {
    Vector3 coord_3d;
    Coordinate* image_coords;
    int amount;
} Point;

typedef struct _Cluster {
    int amount;
    int size;
    Point** cluster_points;
    Vector3 centroid;
} Cluster;

void add_coord_to_point(Point* point, Coordinate coord) {
    Coordinate* coord_arr = (Coordinate*)realloc(point->image_coords, sizeof(Coordinate) * (point->amount + 1));
    coord_arr[point->amount] = coord;
    point->image_coords = coord_arr;
    point->amount++;
}

Vector3 calculate_new_centroid(Cluster* cluster) {
    float new_x = 0; // sketchy, maybe use double?
    float new_y = 0;
    float new_z = 0;
    for (int i = 0; i < cluster->amount; i++) {
        new_x += cluster->cluster_points[i]->coord_3d.x;
        new_y += cluster->cluster_points[i]->coord_3d.y;
        new_z += cluster->cluster_points[i]->coord_3d.z;
    }

    new_x /= cluster->amount;
    new_y /= cluster->amount;
    new_z /= cluster->amount;

    Vector3 new_centroid = {new_x, new_y, new_z};
    return new_centroid;
}

void add_point_to_cluster(Point* point, Cluster* cluster) {
    if (cluster->size <= cluster->amount) {
        cluster->size *= 2;
        cluster->cluster_points = (Point**)realloc(
            cluster->cluster_points, 
            cluster->size * sizeof(Point*)
        );
    }
    cluster->cluster_points[cluster->amount] = point;
    cluster->amount++;
}

double distance_between_points(Vector3 a, Vector3 b) {
    return (double)(b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y) + (b.z - a.z)*(b.z - a.z);
}

Cluster* find_nearest_cluster(Point* point, Cluster* clusters, int k) {
    double min_distance = distance_between_points(point->coord_3d, clusters[0].centroid);
    int min_dist_index = 0;

    for (int i = 1; i < k; i++) {
        double distance = distance_between_points(point->coord_3d, clusters[i].centroid);
        if (distance < min_distance) {
            min_distance = distance;
            min_dist_index = i;
        }
    }

    return &clusters[min_dist_index];
}

Vector3* init_centroids(Point* points, int amount, int k) {
    if (amount < k) exit(1);

    int* chosen = LoadRandomSequence(k, 0, amount-1);
    Vector3* centoids = (Vector3*)malloc(sizeof(Vector3) * k);
    for (int i = 0; i < k; i++) {
        centoids[i] = points[chosen[i]].coord_3d;
    }

    return centoids;
}

Cluster init_empty_cluster(Vector3 centroid) {
    Point** points_array = (Point**)malloc(sizeof(Point*) * 100);
    Cluster cluster = {
        .amount = 0,
        .size = 100,
        .cluster_points = points_array,
        .centroid = centroid,
    };

    return cluster;
}

Cluster* init_empty_clusters(Vector3* centroids, int k) {
    Cluster* clusters = (Cluster*)malloc(sizeof(Cluster) * k);
    for (int i = 0; i < k; i++) {
        clusters[i] = init_empty_cluster(centroids[i]);
    }

    return clusters;
}

void free_clusters(Cluster* clusters, int k) {
    for (int i = 0; i < k; i++) {
        free(clusters[i].cluster_points);
    }
    free(clusters);
}

void draw_points(Point* points, int amount) { 
    for (int i = 0; i < amount; i++)
    {
        Color color = { (unsigned char)floor(points[i].coord_3d.x), (unsigned char)floor(points[i].coord_3d.y), (unsigned char)floor(points[i].coord_3d.z), 255 };
        DrawCube(points[i].coord_3d, CUBE_SIZE, CUBE_SIZE, CUBE_SIZE, color);
    }
}

void draw_centroids(Vector3* centroids, int k) { 
    for (int i = 0; i < k; i++)
    {
        DrawCube(centroids[i], CUBE_SIZE*2, CUBE_SIZE*2, CUBE_SIZE*2, WHITE);
    }
}

Color vector3_to_color(Vector3 v) {
    Color color = { (unsigned char)floor(v.x), (unsigned char)floor(v.y), (unsigned char)floor(v.z), 255 };
    return color;
}

void draw_clusters(Cluster* clusters, int k) { 
    for (int i = 0; i < k; i++) {
        Color centroid_color = vector3_to_color(clusters[i].centroid);
        for (int j = 0; j < clusters[i].amount; j++) {
            DrawCube(clusters[i].cluster_points[j]->coord_3d, CUBE_SIZE, CUBE_SIZE, CUBE_SIZE, centroid_color);
        }
    }   
}


int main(int argc, char** argv) {
    printf("Program: %s\n", argv[0]);
    printf("Arg count: %d\n", argc);

    if (argc < 3) {
        printf("Not enough arguments given\n");
        printf("Required: kmeans <cluster count> <image path> <?output name>\n");
        return 0;
    }

    int cluster_count = atoi(argv[1]);
    if (cluster_count <= 0) {
        printf("Cluster count must be greater than 0\n");
        return 0;
    }
    #define AMOUNT_OF_CLUSTERS cluster_count // fix this fucktard ass solution lazy shit
    char* image_path = argv[2];

    char* output_name = NULL;
    if (argc > 3) {
        output_name = argv[3];
    } else {
        output_name = "result.png";
    }

    Image image = LoadImage(image_path);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    int point_count = 0;
    Point* points = (Point*)malloc(sizeof(Point) * image.width * image.height); // this loader fucking sucks and is outrageously slow. Fix it.
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            Color color = GetPixelColor(&((Color*)image.data)[x + (y * image.width)], PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            Vector3 v_color = { (float)color.r, (float)color.g, (float)color.b };

            bool exists = false;
            for (int i = 0; i < point_count; i++) {
                if (points[i].coord_3d.x == v_color.x && points[i].coord_3d.y == v_color.y && points[i].coord_3d.z == v_color.z) {
                    exists = true;
                    add_coord_to_point(&points[i], (Coordinate){ x, y });
                    break;
                }
            }

            if(!exists) {
                Coordinate* coord_2d = (Coordinate*)malloc(sizeof(Coordinate));
                coord_2d[0] = (Coordinate){ x, y };
                points[point_count] = (Point){
                    .coord_3d = v_color,
                    .image_coords = coord_2d,
                    .amount = 1,
                }; 
                point_count++;
            }
        }
    }

    points = (Point*)realloc(points, sizeof(Point) * point_count);

    printf("Unique colors=%d\n", point_count);
    printf("Total pixel count=%d\n", image.width * image.height);


    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetRandomSeed(time(NULL));
    InitWindow(WINDOW_X, WINDOW_Y, "K-means clustering in 3D"); // GetRandomValue seed is created with initwindow

    Vector3* centroids = init_centroids(points, point_count, AMOUNT_OF_CLUSTERS);
    Cluster* clusters = NULL;

    float camera_theta = 0.0f;
    float camera_phi = 0.0f;
    float camera_mag = CUBE_SIZE*10;
    float camera_mag_velocity = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        camera_mag += camera_mag_velocity;
        camera_mag_velocity += GetMouseWheelMove() * CUBE_SIZE * 2;
        camera_mag_velocity *= 0.9;

        if (camera_mag < 1.0f) camera_mag = 1.0f;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            camera_phi += delta.x*0.01;
            camera_theta += delta.y*0.01;
        }


        if (IsKeyPressed(KEY_N)) {
            clusters = init_empty_clusters(centroids, AMOUNT_OF_CLUSTERS);
            for (int i = 0; i < point_count; i++) {
                Cluster* nearest_cluster = find_nearest_cluster(&points[i], clusters, AMOUNT_OF_CLUSTERS);
                add_point_to_cluster(&points[i], nearest_cluster);
            }

            for (int i = 0; i < AMOUNT_OF_CLUSTERS; i++) {
                centroids[i] = calculate_new_centroid(&clusters[i]);
            }
        }

        if (IsKeyPressed(KEY_R)) {
            free(centroids);
            if (clusters != NULL) free_clusters(clusters, AMOUNT_OF_CLUSTERS);
            clusters = NULL;
            centroids = init_centroids(points, point_count, AMOUNT_OF_CLUSTERS);
        }

        if (IsKeyPressed(KEY_O)) {
            Color* output_data = (Color*)malloc(sizeof(Color) * image.width * image.height);
            int test_count = 0;
            for (int k = 0; k < AMOUNT_OF_CLUSTERS; k++) {
                Color cluster_color = vector3_to_color(clusters[k].centroid);
                printf("Cluster %d: color: r=%d, g=%d, b=%d, a=%d\n", k, cluster_color.r, cluster_color.g, cluster_color.b, cluster_color.a);

                for (int p = 0; p < clusters[k].amount; p++) {
                    Point* current_p = clusters[k].cluster_points[p];

                    for (int c_ind = 0; c_ind < current_p->amount; c_ind++) {
                        test_count++;
                        Coordinate coord = current_p->image_coords[c_ind];
                        output_data[coord.x + (coord.y * image.width) ] = cluster_color;
                    }
                }
            }

            Image result = {
                .data = output_data,
                .height = image.height,
                .width = image.width,
                .mipmaps = image.mipmaps,
                .format = image.format,
            };

            printf("Image created\n info: h=%d, w=%d, f=%d\n", result.height, result.width, result.format);
            printf("Original image: h=%d, w=%d, f=%d\n", image.height, image.width, image.format);
            printf("Actual pixel count=%d\n", test_count);
            printf("Expected pixel count=%d\n", image.width * image.height);
            if (!ExportImage(result, output_name)) {
                printf("Failed to export image\n");
                return 1;
            }
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
                if (clusters == NULL) draw_points(points, point_count);
                else draw_clusters(clusters, AMOUNT_OF_CLUSTERS);
                draw_centroids(centroids, AMOUNT_OF_CLUSTERS);
            EndMode3D();
        EndDrawing();
    }

    UnloadImage(image);
    free(centroids);
    free(points);
    free_clusters(clusters, AMOUNT_OF_CLUSTERS);

    return 0;
}
