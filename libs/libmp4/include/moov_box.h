#ifndef __MOOV_BOX_H__
#define __MOOV_BOX_H__

typedef struct tag_moov_box {
	box_header_t *boxheader;
	mvhd_box_t *mvhdbox;
//	udat_box_t *udtabox;
	trak_box_t *soun_trakbox;
	trak_box_t *vide_trakbox;
	uint32_t struct_size;
} moov_box_t;

int read_moov_box(mp4_bits_t *bs, moov_box_t *moovbox);
int write_moov_box(mp4_bits_t *bs, moov_box_t *moovbox);
int alloc_struct_moov_box(moov_box_t **moovbox);
int free_struct_moov_box(moov_box_t *moovbox);

#endif
