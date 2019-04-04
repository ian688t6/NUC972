#ifndef __TKHD_BOX_H__
#define __TKHD_BOX_H__

typedef struct tag_tkhd_box {
	fullbox_header_t *boxheader;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t track_ID;
	uint32_t reserved;
	uint64_t duration;
	uint32_t reserved_1[2];
	uint16_t layer;
	uint16_t alternate_group;
	uint16_t volume;
	uint16_t reserved_2;
	uint32_t matrix[9];
	uint32_t width;
	uint32_t height;

	uint32_t struct_size;
} tkhd_box_t;

int read_tkhd_box(mp4_bits_t *bs, tkhd_box_t *tkhdbox);
int write_tkhd_box(mp4_bits_t *bs, tkhd_box_t *tkhdbox);
int alloc_struct_tkhd_box(tkhd_box_t **tkhdbox);
int free_struct_tkhd_box(tkhd_box_t *tkhdbox);

#endif
