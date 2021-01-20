#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <cmath>
#include <cstdlib>

#include "chunk.h"
#include "../global.h"

#include "textures.c"

std::vector<class Chunk*> chunk_slot;

static const int transparent[16] = {2, 0, 0, 0, 1, 0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0};
static const char *blocknames[16] = {
	"air", "dirt", "topsoil", "grass", "leaves", "wood", "stone", "sand",
	"water", "glass", "brick", "ore", "woodrings", "white", "black", "x-y"
};

Chunk::Chunk(): cellules(CX + 2, std::vector<std::vector<int>>(CY + 2, std::vector<int>(CZ + 2, -1))) {
	memset(blocks_, 0, sizeof blocks_);
	left = right = below = above = front = back = 0;
	lastused = now;
	slot = 0;
	changed = true;
	initialized = false;
	noised = false;

}

Chunk::Chunk(int x, int y, int z): ax(x), ay(y), az(z), cellules(CX + 2, std::vector<std::vector<int>>(CY + 2, std::vector<int>(CZ + 2, -1))) {
	memset(blocks_, 0, sizeof blocks_);
	left = right = below = above = front = back = 0;
	lastused = now;
	slot = 0;
	changed = true;
	initialized = false;
	noised = false;
}

Chunk::~Chunk() {}

uint8_t Chunk::Get(int x, int y, int z) const {
	if(x < 0)
		return left ? left->blocks_[x + CX][y][z] : 0;
	if(x >= CX)
		return right ? right->blocks_[x - CX][y][z] : 0;
	if(y < 0)
		return below ? below->blocks_[x][y + CY][z] : 0;
	if(y >= CY)
		return above ? above->blocks_[x][y - CY][z] : 0;
	if(z < 0)
		return front ? front->blocks_[x][y][z + CZ] : 0;
	if(z >= CZ)
		return back ? back->blocks_[x][y][z - CZ] : 0;
	return blocks_[x][y][z];
}

bool Chunk::IsBlocked(int x1, int y1, int z1, int x2, int y2, int z2) {
	// Invisible blocks are always "blocked"
	if(!blocks_[x1][y1][z1])
		return true;

	// Leaves don't block any other block, including themselves
	if(transparent[Get(x2, y2, z2)] == 1)
		return false;

	// Non-transparent blocks always block line of sight
	if(!transparent[Get(x2, y2, z2)])
		return true;

	// Otherwise, LOS is only blocked by block if the same transparency type
	return transparent[Get(x2, y2, z2)] == transparent[blocks_[x1][y1][z1]];
}

void Chunk::Set(int x, int y, int z, uint8_t type) {
	// If coordinates are outside this chunk, find the right one.
	if(x < 0) {
		if(left)
			left->Set(x + CX, y, z, type);
		return;
	}
	if(x >= CX) {
		if(right)
			right->Set(x - CX, y, z, type);
		return;
	}
	if(y < 0) {
		if(below)
			below->Set(x, y + CY, z, type);
		return;
	}
	if(y >= CY) {
		if(above)
			above->Set(x, y - CY, z, type);
		return;
	}
	if(z < 0) {
		if(front)
			front->Set(x, y, z + CZ, type);
		return;
	}
	if(z >= CZ) {
		if(back)
			back->Set(x, y, z - CZ, type);
		return;
	}

	// Change the block
	blocks_[x][y][z] = type;
	changed = true;

	// When updating blocks at the edge of this chunk,
	// visibility of blocks in the neighbouring chunk might change.
	if(x == 0 && left)
		left->changed = true;
	if(x == CX - 1 && right)
		right->changed = true;
	if(y == 0 && below)
		below->changed = true;
	if(y == CY - 1 && above)
		above->changed = true;
	if(z == 0 && front)
		front->changed = true;
	if(z == CZ - 1 && back)
		back->changed = true;
}

float Chunk::noise2d(float x, float y, int seed, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;

	for(int i = 0; i < octaves; i++) {
		sum += strength * glm::simplex(glm::vec2(x+SCX*seed, y+SCZ*seed) * scale);
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}

void Chunk::Noise(int seed) {
	if(noised)
		return;
	else
		noised = true;

	for(int x = 0; x < CX; x++) {
		for(int z = 0; z < CZ; z++) {
			// Land height
			float n = noise2d((x + ax * CX) / 256.0, (z + az * CZ) / 256.0, seed, 5, 0.5) / 2;
			
			int h = (int)(n * CY * SCY / 2);

			// Land blocks
			for(int y = 0; y < CY; y++) {

				
				// Are we above "ground" level?
				if(y + ay * CY >= h ) {
					// If we are not yet up to sea level, fill with water blocks
					if(y + ay * CY < SEALEVEL) {
						blocks_[x][y][z] = 8;
						continue;
					// Otherwise, we are in the air
					} else {
						
						// A tree!
						if(Get(x, y - 1, z) == 3 && (rand() & 0xff) == 0) {
							// Trunk
							h = (rand() & 0x3) + 3;
							for(int i = 0; i < h; i++)
								Set(x, y + i, z, 5);

							// Leaves
							for(int ix = -3; ix <= 3; ix++)
								for(int iy = -3; iy <= 3; iy++)
									for(int iz = -3; iz <= 3; iz++)
										if(ix * ix + iy * iy + iz * iz < 8 + (rand() & 1) && !Get(x+ix, y + h + iy, z + iz))
											Set(x + ix, y + h + iy, z + iz, 4);
						}
						
						break;
					}
				}
				blocks_[x][y][z] = (h < SEALEVEL || y + ay * CY < h - 1) ? 1 : 3;
			}
		}
	}
	changed = true;
}

void Chunk::Update() {
	byte4* vertex = new byte4[CX * CY * CZ * 18];
	int i = 0;
	int merged = 0;
	bool vis = false;;

	// View from negative x

	for(int x = CX - 1; x >= 0; x--) {
		for(int y = 0; y < CY; y++) {
			for(int z = 0; z < CZ; z++) {
				// Line of sight blocked?
				if(IsBlocked(x, y, z, x - 1, y, z)) {
					vis = false;
					continue;
				}

				uint8_t top = blocks_[x][y][z];
				uint8_t bottom = blocks_[x][y][z];
				uint8_t side = blocks_[x][y][z];

				// Grass block has dirt sides and bottom
				if(top == 3) {
					bottom = 1;
					side = 2;
				// Wood blocks have rings on top and bottom
				} else if(top == 5) {
					top = bottom = 12;
				}

				// Same block as previous one? Extend it.
				if(vis && z != 0 && blocks_[x][y][z] == blocks_[x][y][z - 1]) {
					vertex[i - 5] = byte4(x, y, z + 1, side);
					vertex[i - 2] = byte4(x, y, z + 1, side);
					vertex[i - 1] = byte4(x, y + 1, z + 1, side);
					merged++;
				// Otherwise, add a new quad.
				} else {
					vertex[i++] = byte4(x, y, z, side);
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
				}
				
				vis = true;
			}
		}
	}

	// View from positive x

	for(int x = 0; x < CX; x++) {
		for(int y = 0; y < CY; y++) {
			for(int z = 0; z < CZ; z++) {
				if(IsBlocked(x, y, z, x + 1, y, z)) {
					vis = false;
					continue;
				}

				uint8_t top = blocks_[x][y][z];
				uint8_t bottom = blocks_[x][y][z];
				uint8_t side = blocks_[x][y][z];

				if(top == 3) {
					bottom = 1;
					side = 2;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && z != 0 && blocks_[x][y][z] == blocks_[x][y][z - 1]) {
					vertex[i - 4] = byte4(x + 1, y, z + 1, side);
					vertex[i - 2] = byte4(x + 1, y + 1, z + 1, side);
					vertex[i - 1] = byte4(x + 1, y, z + 1, side);
					merged++;
				} else {
					vertex[i++] = byte4(x + 1, y, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
				}
				vis = true;
			}
		}
	}

	// View from negative y

	for(int x = 0; x < CX; x++) {
		for(int y = CY - 1; y >= 0; y--) {
			for(int z = 0; z < CZ; z++) {
				if(IsBlocked(x, y, z, x, y - 1, z)) {
					vis = false;
					continue;
				}

				uint8_t top = blocks_[x][y][z];
				uint8_t bottom = blocks_[x][y][z];

				if(top == 3) {
					bottom = 1;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && z != 0 && blocks_[x][y][z] == blocks_[x][y][z - 1]) {
					vertex[i - 4] = byte4(x, y, z + 1, bottom + 128);
					vertex[i - 2] = byte4(x + 1, y, z + 1, bottom + 128);
					vertex[i - 1] = byte4(x, y, z + 1, bottom + 128);
					merged++;
				} else {
					vertex[i++] = byte4(x, y, z, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z, bottom + 128);
					vertex[i++] = byte4(x, y, z + 1, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z, bottom + 128);
					vertex[i++] = byte4(x + 1, y, z + 1, bottom + 128);
					vertex[i++] = byte4(x, y, z + 1, bottom + 128);
				}
				vis = true;
			}
		}
	}

	// View from positive y

	for(int x = 0; x < CX; x++) {
		for(int y = 0; y < CY; y++) {
			for(int z = 0; z < CZ; z++) {
				if(IsBlocked(x, y, z, x, y + 1, z)) {
					vis = false;
					continue;
				}

				uint8_t top = blocks_[x][y][z];
				uint8_t bottom = blocks_[x][y][z];

				if(top == 3) {
					bottom = 1;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && z != 0 && blocks_[x][y][z] == blocks_[x][y][z - 1]) {
					vertex[i - 5] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i - 2] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i - 1] = byte4(x + 1, y + 1, z + 1, top + 128);
					merged++;
				} else {
					vertex[i++] = byte4(x, y + 1, z, top + 128);
					vertex[i++] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z, top + 128);
					vertex[i++] = byte4(x, y + 1, z + 1, top + 128);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, top + 128);
				}
				vis = true;
			}
		}
	}

	// View from negative z

	for(int x = 0; x < CX; x++) {
		for(int z = CZ - 1; z >= 0; z--) {
			for(int y = 0; y < CY; y++) {
				if(IsBlocked(x, y, z, x, y, z - 1)) {
					vis = false;
					continue;
				}

				uint8_t top = blocks_[x][y][z];
				uint8_t bottom = blocks_[x][y][z];
				uint8_t side = blocks_[x][y][z];

				if(top == 3) {
					bottom = 1;
					side = 2;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && y != 0 && blocks_[x][y][z] == blocks_[x][y - 1][z]) {
					vertex[i - 5] = byte4(x, y + 1, z, side);
					vertex[i - 3] = byte4(x, y + 1, z, side);
					vertex[i - 2] = byte4(x + 1, y + 1, z, side);
					merged++;
				} else {
					vertex[i++] = byte4(x, y, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z, side);
					vertex[i++] = byte4(x, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y + 1, z, side);
					vertex[i++] = byte4(x + 1, y, z, side);
				}
				vis = true;
			}
		}
	}

	// View from positive z

	for(int x = 0; x < CX; x++) {
		for(int z = 0; z < CZ; z++) {
			for(int y = 0; y < CY; y++) {
				if(IsBlocked(x, y, z, x, y, z + 1)) {
					vis = false;
					continue;
				}

				uint8_t top = blocks_[x][y][z];
				uint8_t bottom = blocks_[x][y][z];
				uint8_t side = blocks_[x][y][z];

				if(top == 3) {
					bottom = 1;
					side = 2;
				} else if(top == 5) {
					top = bottom = 12;
				}

				if(vis && y != 0 && blocks_[x][y][z] == blocks_[x][y - 1][z]) {
					vertex[i - 4] = byte4(x, y + 1, z + 1, side);
					vertex[i - 3] = byte4(x, y + 1, z + 1, side);
					vertex[i - 1] = byte4(x + 1, y + 1, z + 1, side);
					merged++;
				} else {
					vertex[i++] = byte4(x, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
					vertex[i++] = byte4(x, y + 1, z + 1, side);
					vertex[i++] = byte4(x + 1, y, z + 1, side);
					vertex[i++] = byte4(x + 1, y + 1, z + 1, side);
				}
				vis = true;
			}
		}
	}

	changed = false;
	elements = i;

	// If this chunk is empty, no need to allocate a chunk slot.
	if(!elements)
		return;

	// If we don't have an active slot, find one
	if(chunk_slot[slot] != this) {
		int lru = 0;
		for(int i = 0; i < CHUNKSLOTS; i++) {
			// If there is an empty slot, use it
			if(!chunk_slot[i]) {
				lru = i;
				break;
			}
			// Otherwise try to find the least recently used slot
			if(chunk_slot[i]->lastused < chunk_slot[lru]->lastused)
				lru = i;
		}

		// If the slot is empty, create a new VBO
		if(!chunk_slot[lru]) {
			glGenBuffers(1, &vbo);
		// Otherwise, steal it from the previous slot owner
		} else {
			vbo = chunk_slot[lru]->vbo;
			chunk_slot[lru]->changed = true;
		}

		slot = lru;
		chunk_slot[slot] = this;
	}

	// Upload vertices

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, i * sizeof *vertex, vertex, GL_STATIC_DRAW);
}

void Chunk::Render() {
	if(changed)
		Update();

	lastused = now;

	if(!elements)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, elements);
}

SuperChunk::SuperChunk(int seed_, int render_distance): chunk_(render_distance, TwoDimensions(render_distance, OneDimension(render_distance)))
{
	index_x = 1;
	index_z = 1;
	seed = seed_;
	
	SCX = render_distance;
	SCY = 10;
	SCZ = render_distance;

	for (int i = 0; i < SCX*SCY*SCZ; i++)
		chunk_slot.push_back(0);

	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++)
				chunk_[x][y][z] = new Chunk(x-SCX/2, y-SCY/2, z-SCZ/2);
				
	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++) {
				if(x > 0)
					chunk_[x][y][z]->left = chunk_[x-1][y][z];
				if(x < SCX-1)
					chunk_[x][y][z]->right = chunk_[x+1][y][z];
				if(y > 0)
					chunk_[x][y][z]->below = chunk_[x][y-1][z];
				if(x < SCY-1)
					chunk_[x][y][z]->above = chunk_[x][y+1][z];
				if(z > 0)
					chunk_[x][y][z]->front = chunk_[x][y][z-1];
				if(z < SCZ-1)
					chunk_[x][y][z]->back = chunk_[x][y][z+1];
			}
}

SuperChunk::~SuperChunk()
{
}

uint8_t SuperChunk::Get(int x, int y, int z) const {
	int cx = (x + CX * (SCX / 2)) / CX;
	int cy = (y + CY * (SCY / 2)) / CY;
	int cz = (z + CZ * (SCZ / 2)) / CZ;

	if(cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz <= 0 || cz >= SCZ)
		return 0;

	return chunk_[cx][cy][cz]->Get(x & (CX - 1), y & (CY - 1), z & (CZ - 1));
}

void SuperChunk::Set(int x, int y, int z, uint8_t type) {
	int cx = (x + CX * (SCX / 2)) / CX;
	int cy = (y + CY * (SCY / 2)) / CY;
	int cz = (z + CZ * (SCZ / 2)) / CZ;

	if(cx < 0 || cx >= SCX || cy < 0 || cy >= SCY || cz <= 0 || cz >= SCZ)
		return;

	chunk_[cx][cy][cz]->Set(x & (CX - 1), y & (CY - 1), z & (CZ - 1), type);
}

void SuperChunk::Update()
{
	if ((position.z > index_z * CZ*BLOCK_SIZE)) {
		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ-1; z++) 
				{
					chunk_[x][y][z] = chunk_[x][y][z + 1];
				}

		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++) {
					chunk_[x][y][SCZ-1] = new Chunk(index_x-1 + x - (SCX/2), y - (SCY/2), index_z-1 + (SCZ/2));
				}
		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++) {
					if (x > 0)
						chunk_[x][y][z]->left = chunk_[x - 1][y][z];
					if (x < SCX - 1)
						chunk_[x][y][z]->right = chunk_[x + 1][y][z];
					if (y > 0)
						chunk_[x][y][z]->below = chunk_[x][y - 1][z];
					if (x < SCY - 1)
						chunk_[x][y][z]->above = chunk_[x][y + 1][z];
					if (z > 0)
						chunk_[x][y][z]->front = chunk_[x][y][z - 1];
					if (z < SCZ - 1)
						chunk_[x][y][z]->back = chunk_[x][y][z + 1];
				}
		index_z++;
	}
	
	else if ((position.z < (index_z-2) * CZ * BLOCK_SIZE)) {

		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = SCZ-1; z > 0; z--)
				{
					chunk_[x][y][z] = chunk_[x][y][z-1];
				}

		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++) {
				chunk_[x][y][0] = new Chunk(index_x - 1 + x - (SCX / 2), y - (SCY / 2), index_z - 1 - (SCZ / 2));
			}

		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++) {
				
			}
		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++) {
					if (x > 0)
						chunk_[x][y][z]->left = chunk_[x - 1][y][z];
					if (x < SCX - 1)
						chunk_[x][y][z]->right = chunk_[x + 1][y][z];
					if (y > 0)
						chunk_[x][y][z]->below = chunk_[x][y - 1][z];
					if (x < SCY - 1)
						chunk_[x][y][z]->above = chunk_[x][y + 1][z];
					if (z > 0)
						chunk_[x][y][z]->front = chunk_[x][y][z - 1];
					if (z < SCZ - 1)
						chunk_[x][y][z]->back = chunk_[x][y][z + 1];
				}
		index_z--;
	}
	

	if ((position.x > index_x * CX * BLOCK_SIZE)) {

		for (int x = 0; x < SCX-1; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++)
				{
					chunk_[x][y][z] = chunk_[x+1][y][z];
				}
		for (int y = 0; y < SCY; y++)
			for (int z = 0; z < SCZ; z++) {
				chunk_[SCX-1][y][z] = new Chunk(index_x - 1 + (SCX / 2), y - (SCY / 2), index_z - 1 + z - (SCZ / 2));
			}

		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++) {
					if (x > 0)
						chunk_[x][y][z]->left = chunk_[x - 1][y][z];
					if (x < SCX - 1)
						chunk_[x][y][z]->right = chunk_[x + 1][y][z];
					if (y > 0)
						chunk_[x][y][z]->below = chunk_[x][y - 1][z];
					if (x < SCY - 1)
						chunk_[x][y][z]->above = chunk_[x][y + 1][z];
					if (z > 0)
						chunk_[x][y][z]->front = chunk_[x][y][z - 1];
					if (z < SCZ - 1)
						chunk_[x][y][z]->back = chunk_[x][y][z + 1];
				}
		index_x++;
	}

	else if ((position.x < (index_x - 2) * CX * BLOCK_SIZE)) {

		for (int x = SCX-1; x > 0; x--)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++)
				{
					chunk_[x][y][z] = chunk_[x-1][y][z];
				}
		for (int y = 0; y < SCY; y++)
			for (int z = 0; z < SCZ; z++) {
				chunk_[0][y][z] = new Chunk(index_x - 1 - (SCX / 2), y - (SCY / 2), index_z - 1 + z - (SCZ / 2));
			}

		for (int x = 0; x < SCX; x++)
			for (int y = 0; y < SCY; y++)
				for (int z = 0; z < SCZ; z++) {
					if (x > 0)
						chunk_[x][y][z]->left = chunk_[x - 1][y][z];
					if (x < SCX - 1)
						chunk_[x][y][z]->right = chunk_[x + 1][y][z];
					if (y > 0)
						chunk_[x][y][z]->below = chunk_[x][y - 1][z];
					if (x < SCY - 1)
						chunk_[x][y][z]->above = chunk_[x][y + 1][z];
					if (z > 0)
						chunk_[x][y][z]->front = chunk_[x][y][z - 1];
					if (z < SCZ - 1)
						chunk_[x][y][z]->back = chunk_[x][y][z + 1];
				}
		index_x--;
	}
}

void SuperChunk::Render(const glm::mat4 &pv) {
	float ud = INFINITY;
	int ux = -1;
	int uy = -1;
	int uz = -1;

	for(int x = 0; x < SCX; x++) {
		for(int y = 0; y < SCY; y++) {
			for(int z = 0; z < SCZ; z++) {
				glm::mat4  model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk_[x][y][z]->ax * CX, chunk_[x][y][y]->ay * CY, chunk_[x][y][z]->az * CZ));

				glm::mat4 mvp = pv * model;

				// Is this chunk on the screen?
				glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);

				float d = glm::length(center);
				center.x /= center.w;
				center.y /= center.w;
				
				// If it is behind the camera, don't bother drawing it
				if(center.z < -CY / 2)
					continue;

				// If it is outside the screen, don't bother drawing it
				if(fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
					continue;
					
				// If this chunck is not initialized, skip it
				if(!chunk_[x][y][z]->initialized) {
					// But if it is the closest to the camera, mark it for initialization
					if(ux < 0 || d < ud) {
						ud = d;
						ux = x;
						uy = y;
						uz = z;
					}
					continue;
				}

				global_shader->SetMat4("mvp", mvp);

				chunk_[x][y][z]->Render();
			}
		}
	}

	if(ux >= 0) {
		chunk_[ux][uy][uz]->Noise(seed);
		if(chunk_[ux][uy][uz]->left)
			chunk_[ux][uy][uz]->left->Noise(seed);
		if(chunk_[ux][uy][uz]->right)
			chunk_[ux][uy][uz]->right->Noise(seed);
		if(chunk_[ux][uy][uz]->below)
			chunk_[ux][uy][uz]->below->Noise(seed);
		if(chunk_[ux][uy][uz]->above)
			chunk_[ux][uy][uz]->above->Noise(seed);
		if(chunk_[ux][uy][uz]->front)
			chunk_[ux][uy][uz]->front->Noise(seed);
		if(chunk_[ux][uy][uz]->back)
			chunk_[ux][uy][uz]->back->Noise(seed);
		chunk_[ux][uy][uz]->initialized = true;
	}
}

void update_vectors() {
	forward.x = sinf(angle.x);
	forward.y = 0;
	forward.z = cosf(angle.x);

	right.x = -cosf(angle.x);
	right.y = 0;
	right.z = sinf(angle.x);

	lookat.x = sinf(angle.x) * cosf(angle.y);
	lookat.y = sinf(angle.y);
	lookat.z = cosf(angle.x) * cosf(angle.y);

	up = glm::cross(right, lookat);
}

int SuperChunk::init_resorces() {

	attribute_coord = global_shader->GetAttrib("coord");
	uniform_mvp = global_shader->GetUniform("mvp");

	if(attribute_coord == -1 || uniform_mvp == -1)
		return 0;

	// Create and upload the texture

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures.width, textures.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures.pixel_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Create the world

//	world = new superchunk;

	position = glm::vec3(0, BLOCK_SIZE*CY*SCY/2 + 1, 0);
	angle = glm::vec3(0, -1, 0);
	update_vectors();

	// OpenGL Settings that do not change while running this program
	global_shader->Use();
//	glUniform1i(uniform_texture, 0);
	global_shader->SetInt("texture", 0);
	glEnable(GL_CULL_FACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_NEAREST_MIPMAP_LINEAR if you want to use mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPolygonOffset(1, 1);

	// Attribute array coord
	glEnableVertexAttribArray(attribute_coord);

	return 1;
}


