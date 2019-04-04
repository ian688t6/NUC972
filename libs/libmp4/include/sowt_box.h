#ifndef __SOWT_BOX_H__
#define __SOWT_BOX_H__

typedef struct tag_sowt_box {
	box_header_t *boxheader;
	uint8_t reseved[6];
	uint16_t data_reference_index;

	uint16_t version;
	uint16_t revision_level;
	uint32_t vendor;
	uint16_t channel_num;
	uint16_t sample_size;
	uint16_t compression_id;
	uint16_t packet_size;
	uint32_t sample_rate;

	uint32_t struct_size;
} sowt_box_t;

int read_sowt_box(mp4_bits_t *bs, sowt_box_t *sowtbox);
int write_sowt_box(mp4_bits_t *bs, sowt_box_t *sowtbox);
int alloc_struct_sowt_box(sowt_box_t **sowtbox);
int free_struct_sowt_box(sowt_box_t *sowtbox);

#endif /* __SOWT_BOX_H__ */
