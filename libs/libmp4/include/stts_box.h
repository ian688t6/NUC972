#ifndef __STTS_BOX_H__
#define __STTS_BOX_H__

typedef struct tag_stts_box {
	fullbox_header_t *boxheader;
	uint32_t entry_count;
	uint32_t *sample_count;
	uint32_t *sample_delta;

	uint32_t struct_size;
} stts_box_t;

int read_stts_box(mp4_bits_t *bs, stts_box_t *sttsbox);
int write_stts_box(mp4_bits_t *bs, stts_box_t *sttsbox);
int alloc_struct_stts_box(stts_box_t **sttsbox);
int free_struct_stts_box(stts_box_t *sttsbox);

#endif
