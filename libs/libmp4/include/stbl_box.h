#ifndef __STBL_BOX_H__
#define __STBL_BOX_H__

typedef struct tag_stbl_box {
	box_header_t *boxheader;
	ctts_box_t *cttsbox;
	stts_box_t *sttsbox;
	stss_box_t *stssbox;
	stsc_box_t *stscbox;
	stsz_box_t *stszbox;
	stco_box_t *stcobox;
	stsd_box_t *stsdbox;

	uint32_t struct_size;
} stbl_box_t;

int read_stbl_box(mp4_bits_t *bs, stbl_box_t *stblbox);
int write_stbl_box(mp4_bits_t *bs, stbl_box_t *stblbox);
int alloc_struct_stbl_box(stbl_box_t **stblbox);
int free_struct_stbl_box(stbl_box_t *stblbox);

#endif
