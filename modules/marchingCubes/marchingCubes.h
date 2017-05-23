/*
 * marchingCubes.h
 *
 *  Created on: May 23, 2017
 *      Author: nelaturi
 */

#ifndef MARCHINGCUBES_H_
#define MARCHINGCUBES_H_



uint3 gridSizeLog2 = make_uint3(5, 5, 5);
uint3 gridSizeShift;
uint3 gridSize;
uint3 gridSizeMask;

float3 voxelSize;
uint numVoxels = 0;
uint maxVerts = 0;
uint activeVoxels = 0;
uint totalVerts = 0;

float isoValue = 0.2f;
float dIsoValue = 0.005f;

// device data
GLuint posVbo, normalVbo;
GLint gl_Shader;
struct cudaGraphicsResource *cuda_posvbo_resource, *cuda_normalvbo_resource; // handles OpenGL-CUDA exchange

float4 *d_pos = 0, *d_normal = 0;

uchar *d_volume = 0;
uint *d_voxelVerts = 0;
uint *d_voxelVertsScan = 0;
uint *d_voxelOccupied = 0;
uint *d_voxelOccupiedScan = 0;
uint *d_compVoxelArray;

// tables
uint *d_numVertsTable = 0;
uint *d_edgeTable = 0;
uint *d_triTable = 0;

// mouse controls
int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float3 rotate = make_float3(0.0, 0.0, 0.0);
float3 translate = make_float3(0.0, 0.0, -3.0);

// toggles
bool wireframe = false;
bool animate = true;
bool lighting = true;
bool render = true;
bool compute = true;


#define MAX_EPSILON_ERROR 5.0f
#define REFRESH_DELAY     10 //ms


// constants
const unsigned int window_width = 512;
const unsigned int window_height = 512;


extern "C" void
launch_classifyVoxel(dim3 grid, dim3 threads, uint *voxelVerts,
        uint *voxelOccupied, uchar *volume, uint3 gridSize, uint3 gridSizeShift,
        uint3 gridSizeMask, uint numVoxels, float3 voxelSize, float isoValue);

extern "C" void
launch_compactVoxels(dim3 grid, dim3 threads, uint *compactedVoxelArray,
        uint *voxelOccupied, uint *voxelOccupiedScan, uint numVoxels);

extern "C" void
launch_generateTriangles(dim3 grid, dim3 threads, float4 *pos, float4 *norm,
        uint *compactedVoxelArray, uint *numVertsScanned, uint3 gridSize,
        uint3 gridSizeShift, uint3 gridSizeMask, float3 voxelSize,
        float isoValue, uint activeVoxels, uint maxVerts);

extern "C" void
launch_generateTriangles2(dim3 grid, dim3 threads, float4 *pos, float4 *norm,
        uint *compactedVoxelArray, uint *numVertsScanned, uchar *volume,
        uint3 gridSize, uint3 gridSizeShift, uint3 gridSizeMask,
        float3 voxelSize, float isoValue, uint activeVoxels, uint maxVerts);

extern "C" void allocateTextures(uint **d_edgeTable, uint **d_triTable,
        uint **d_numVertsTable);
extern "C" void bindVolumeTexture(uchar *d_volume);
extern "C" void ThrustScanWrapper(unsigned int *output, unsigned int *input,
        unsigned int numElements);



// Auto-Verification Code
const int frameCheckNumber = 4;
int fpsCount = 0;        // FPS count for averaging
int fpsLimit = 1;        // FPS limit for sampling
int g_Index = 0;
unsigned int frameCount = 0;
bool g_bValidate = false;

int *pArgc = NULL;
char **pArgv = NULL;

// forward declarations
void runGraphicsTest(int argc, char **argv);
void runAutoTest(int argc, char **argv);
void initMC(int argc, char **argv);
void computeIsosurface();
void dumpFile(void *dData, int data_bytes, const char *file_name);

template<class T>
void dumpBuffer(T *d_buffer, int nelements, int size_element);

void cleanup();

bool initGL(int *argc, char **argv);
void createVBO(GLuint *vbo, unsigned int size);
void deleteVBO(GLuint *vbo, struct cudaGraphicsResource **cuda_resource);

void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle();
void reshape(int w, int h);

void mainMenu(int i);

#define EPSILON 5.0f
#define THRESHOLD 0.30f

#endif /* MARCHINGCUBES_H_ */
