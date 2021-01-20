#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CHUNK_SIZE 8

enum BlockType {
	kBlockType_Default = 0,

	kBlockType_Grass,
	kBlockType_Dirt,
	kBlockType_Water,
	kBlockType_Stone,
	kBlockType_Wood,
	kBlockType_Sand,

	kBlockType_NumTypes
};

#endif
