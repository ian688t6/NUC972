#ifndef __STCO_BOX_H__
#define __STCO_BOX_H__

#define MAX_STCO_BOX_CHUNK_OFFSET_SIZE  1024

typedef struct tag_stco_box {
	fullbox_header_t *boxheader;
	uint32_t entry_count;
	uint32_t *chunk_offset;

	uint32_t struct_size;
} stco_box_t;

int read_stco_box(mp4_bits_t *bs, stco_box_t *stcobox);
int write_stco_box(mp4_bits_t *bs, stco_box_t *stcobox);
int alloc_struct_stco_box(stco_box_t **stcobox);
int free_struct_stco_box(stco_box_t *stcobox);

#endif
