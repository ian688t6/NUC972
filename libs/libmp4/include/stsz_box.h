#ifndef __STSZ_BOX_H__
#define __STSZ_BOX_H__

typedef struct tag_stsz_box {
	fullbox_header_t *boxheader;
	uint32_t sample_size;
	uint32_t sample_count;
	uint32_t *entry_size;

	uint32_t struct_size;
} stsz_box_t;

int read_stsz_box(mp4_bits_t *bs, stsz_box_t *stszbox);
int write_stsz_box(mp4_bits_t *bs, stsz_box_t *stszbox);
int alloc_struct_stsz_box(stsz_box_t **stszbox);
int free_struct_stsz_box(stsz_box_t *stszbox);

#endif
