#ifndef __STSC_BOX_H__
#define __STSC_BOX_H__

typedef struct tag_stsc_box {
	fullbox_header_t *boxheader;
	uint32_t entry_count;
	uint32_t *first_chunk;
	uint32_t *samples_per_chunk;
	uint32_t *sample_description_index;

	uint32_t struct_size;
} stsc_box_t;

int read_stsc_box(mp4_bits_t *bs, stsc_box_t *stscbox);
int write_stsc_box(mp4_bits_t *bs, stsc_box_t *stscbox);
int alloc_struct_stsc_box(stsc_box_t **stscbox);
int free_struct_stsc_box(stsc_box_t *stscbox);

#endif
