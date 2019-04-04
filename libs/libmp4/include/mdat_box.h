#ifndef __MDAT_BOX_H__
#define __MDAT_BOX_H__

typedef struct tag_mdat_box {
	box_header_t *boxheader;
	uint8_t *data;
	uint32_t data_size;

	uint32_t head_pos;
	uint32_t struct_size;
} mdat_box_t;

int read_mdat_box(mp4_bits_t *bs, mdat_box_t *mdatbox);
int write_mdat_box_head(mp4_bits_t *bs, mdat_box_t *mdatbox);
int write_mdat_box_data(mp4_bits_t *bs, mdat_box_t *mdatbox, uint8_t *data, uint32_t data_len);
int write_mdat_box_tail(mp4_bits_t *bs, mdat_box_t *mdatbox);
int alloc_struct_mdat_box(mdat_box_t **freebox);
int free_struct_mdat_box(mdat_box_t *freebox);

#endif /* __MDAT_BOX_H__ */
