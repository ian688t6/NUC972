#ifndef __STSS_BOX_H__
#define __STSS_BOX_H__

typedef struct tag_stss_box {
	fullbox_header_t *boxheader;
	uint32_t entry_count;
	uint32_t *sample_number;

	uint32_t struct_size;
} stss_box_t;

int read_stss_box(mp4_bits_t *bs, stss_box_t *stssbox);
int write_stss_box(mp4_bits_t *bs, stss_box_t *stssbox);
int alloc_struct_stss_box(stss_box_t **stssbox);
int free_struct_stss_box(stss_box_t *stssbox);

#endif

