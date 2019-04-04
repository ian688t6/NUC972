#ifndef __AVCC_BOX_H__
#define __AVCC_BOX_H__

typedef struct tag_avcc_box {
	box_header_t *boxheader;

	uint8_t configurationVersion;
	uint8_t AVCProfileIndication;
	uint8_t profile_compatibility;
	uint8_t AVCLevelIndication;

	uint8_t lengthSizeMinusOne;

	uint8_t numOfSequenceParameterSets;
	uint16_t sequenceParameterSetLength;
	uint8_t *sequenceParameterSetNALUnit;

	uint8_t numOfPictureParameterSets;
	uint16_t pictureParameterSetLength;
	uint8_t *pictureParameterSetNALUnit;

	uint32_t struct_size;
} avcc_box_t;

int read_avcc_box(mp4_bits_t *bs, avcc_box_t *avccbox);
int write_avcc_box(mp4_bits_t *bs, avcc_box_t *avccbox);
int alloc_struct_avcc_box(avcc_box_t **avccbox);
int free_struct_avcc_box(avcc_box_t *avccbox);

#endif
