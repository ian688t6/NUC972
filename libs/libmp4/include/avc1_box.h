#ifndef __AVC1_BOX_H__
#define __AVC1_BOX_H__

typedef struct tag_avc1_box {
	box_header_t *boxheader;
	uint8_t reserved[6];
	uint16_t data_reference_index;

	uint16_t pre_defined;
	uint16_t reserved1;
	uint32_t pre_defined1[3];
	uint16_t width;
	uint16_t height;
	uint32_t horizresolution;
	uint32_t vertresolution;
	uint32_t reserved2;
	uint8_t compressorname[32];
	uint16_t frame_count;
	uint16_t depth;
	uint16_t pre_defined2;

	avcc_box_t *avccbox;

	uint32_t struct_size;
} avc1_box_t;

int read_avc1_box(mp4_bits_t *bs, avc1_box_t *avc1box);
int write_avc1_box(mp4_bits_t *bs, avc1_box_t *avc1box);
int alloc_struct_avc1_box(avc1_box_t **avc1box);
int free_struct_avc1_box(avc1_box_t *avc1box);

#endif
