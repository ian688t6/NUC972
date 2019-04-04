#ifndef __TRAK_BOX_H__
#define __TRAK_BOX_H__

typedef struct tag_trak_box {
	box_header_t *boxheader;
//	udat_box_t *udtabox;
	tkhd_box_t *tkhdbox;
//	tref_box_t *trefbox;
	mdia_box_t *mdiabox;

	uint32_t struct_size;
} trak_box_t;

int read_trak_box(mp4_bits_t *bs, trak_box_t *trakbox);
int write_trak_box(mp4_bits_t *bs, trak_box_t * trakbox);
int alloc_struct_trak_box(trak_box_t **trakbox);
int free_struct_trak_box(trak_box_t *trakbox);

#endif
