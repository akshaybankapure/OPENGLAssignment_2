#ifndef CHUNK_H
#define CHUNK_H

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// Size of one chunk in blocks
//#define CX 32
//#define CY 64
//#define CZ 32
#define CX 16
#define CY 32
#define CZ 16
//#define CX 8
//#define CY 16
//#define CZ 8

// Number of chunks in the world
static int SCX = 32;
static int SCY = 10;
static int SCZ = 32;

#define BLOCK_SIZE 0.1f

// Sea level
#define SEALEVEL 4

// Number of VBO slots for chunks
#define CHUNKSLOTS (SCX * SCY * SCZ)

struct byte4 {
	uint8_t x, y, z, w;
	byte4() {}
	byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w): x(x), y(y), z(z), w(w) {}
};

class Chunk {
public:

	Chunk *left, *right, *below, *above, *front, *back;
	bool initialized;
	int ax, ay, az;
	int site[2];
	std::vector<std::vector<std::vector<int>>> cellules;

	Chunk();
	Chunk(int x, int y, int z);

	~Chunk();

	uint8_t Get(int x, int y, int z) const;

	bool IsBlocked(int x1, int y1, int z1, int x2, int y2, int z2);

	void Set(int x, int y, int z, uint8_t type);

	void Update();

	void Render();

	void Noise(int seed);

private:
	// The blocks data
	uint8_t blocks_[CX] [CY] [CZ] ;
	int slot;
	GLuint vbo;
	int elements;
	time_t lastused;
	bool changed;
	bool noised;

	float noise2d(float x, float y, int seed, int octaves, float persistence);
	float noise2d_perlin(float x, float y, int seed, int octaves, float persistence);
	float noise3d_ads(float x, float y, float z, int seed, int octaves, float persistence);
	void cellular_automata(int n);
};

typedef std::vector<Chunk*> OneDimension;
typedef std::vector<std::vector<Chunk*>> TwoDimensions;
typedef std::vector<std::vector<std::vector<Chunk*>>> ThreeDimensions;

class SuperChunk {
public:
	SuperChunk(int seed_, int render_distance);

	~SuperChunk();

	uint8_t Get(int x, int y, int z) const;

	void Set(int x, int y, int z, uint8_t type);

	void Update();

	void Render(const glm::mat4 &pv);

	int init_resorces();



private:
	ThreeDimensions chunk_;// [SCX] [SCY] [SCZ] ;
	int seed;
	int index_x, index_z;
};

void update_vectors();

#endif
