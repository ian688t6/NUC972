#ifndef __STSD_BOX_H__
#define __STSD_BOX_H__

typedef struct tag_stsd_box {
	fullbox_header_t *boxheader;
	uint32_t entry_count;
	mp4a_box_t *mp4abox;
	avc1_box_t *avc1box;
	sowt_box_t *sowtbox;

	uint32_t struct_size;
} stsd_box_t;

int read_stsd_box(mp4_bits_t *bs, stsd_box_t * stsdbox);
int write_stsd_box(mp4_bits_t *bs, stsd_box_t *stsdbox);
int alloc_struct_stsd_box(stsd_box_t **stsdbox);
int free_struct_stsd_box(stsd_box_t *stsdbox);

#endif
