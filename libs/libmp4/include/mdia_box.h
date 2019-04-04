#ifndef __MDIA_BOX_H__
#define __MDIA_BOX_H__

typedef struct tag_mdia_box {
	box_header_t *boxheader;
	mdhd_box_t *mdhdbox;
	hdlr_box_t *hdlrbox;
	minf_box_t *minfbox;

	uint32_t struct_size;
} mdia_box_t;

int read_mdia_box(mp4_bits_t *bs, mdia_box_t *mdiabox);
int write_mdia_box(mp4_bits_t *bs, mdia_box_t *mdiabox);
int alloc_struct_mdia_box(mdia_box_t **mdiabox);
int free_struct_mdia_box(mdia_box_t *mdiabox);

#endif
