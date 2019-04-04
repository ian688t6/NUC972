#ifndef __MP4A_BOX_H__
#define __MP4A_BOX_H__

typedef struct tag_mp4a_box {
	box_header_t *boxheader;
	uint8_t reseved[6];
	uint16_t data_reference_index;

	uint32_t reseved1[2];
	uint16_t chanlecount;
	uint16_t samplesize;
	uint32_t reseved2;
	uint16_t samplerate;
	uint16_t reseved3;

	esds_box_t *esdsbox;

	uint32_t struct_size;
} mp4a_box_t;

int read_mp4a_box(mp4_bits_t *bs, mp4a_box_t *mp4abox);
int write_mp4a_box(mp4_bits_t *bs, mp4a_box_t *mp4abox);
int alloc_struct_mp4a_box(mp4a_box_t **mp4abox);
int free_struct_mp4a_box(mp4a_box_t *mp4abox);

#endif
