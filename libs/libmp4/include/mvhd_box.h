#ifndef __MVHD_BOX_H__
#define __MVHD_BOX_H__

typedef struct tag_mvhd_box {
	fullbox_header_t *boxheader;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;
	uint32_t rate;
	uint16_t volume;
	uint16_t reserved;
	uint32_t reserved_1[2];
	uint32_t matrix[9];
	uint32_t pre_defined[6];
	uint32_t next_track_ID;
	uint32_t struct_size;
} mvhd_box_t;

int read_mvhd_box(mp4_bits_t *bs, mvhd_box_t * mvhdbox);
int write_mvhd_box(mp4_bits_t *bs, mvhd_box_t *mvhdbox);
int alloc_struct_mvhd_box(mvhd_box_t ** mvhdbox);
int free_struct_mvhd_box(mvhd_box_t * mvhdbox);

#endif
