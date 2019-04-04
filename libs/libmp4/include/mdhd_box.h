#ifndef __MDHD_BOX_H__
#define __MDHD_BOX_H__

typedef struct tag_mdhd_box {
	fullbox_header_t *boxheader;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;
	uint16_t language;
	uint16_t pre_defined;

	uint32_t struct_size;
} mdhd_box_t;

int read_mdhd_box(mp4_bits_t *bs, mdhd_box_t *mdhdbox);
int write_mdhd_box(mp4_bits_t *bs, mdhd_box_t *mdhdbox);
int alloc_struct_mdhd_box(mdhd_box_t **mdhdbox);
int free_struct_mdhd_box(mdhd_box_t *mdhdbox);

#endif
