#ifndef __CTTS_BOX_H__
#define __CTTS_BOX_H__

typedef struct tag_ctts_box {
	fullbox_header_t *boxheader;

	uint32_t entry_count;
	void *sample_count;
	void *sample_offset;

	uint32_t struct_size;
} ctts_box_t;

int read_ctts_box(mp4_bits_t *bs, ctts_box_t *cttsbox);
int write_ctts_box(mp4_bits_t *bs, ctts_box_t *cttsbox);
int alloc_struct_ctts_box(ctts_box_t **cttsbox);
int free_struct_ctts_box(ctts_box_t *cttsbox);

#endif
