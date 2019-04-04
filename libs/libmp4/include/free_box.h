#ifndef __FREE_BOX_H__
#define __FREE_BOX_H__

typedef struct tag_free_box {
	box_header_t *boxheader;
	uint8_t *data;
	uint32_t data_size;

	uint32_t struct_size;
} free_box_t;

int read_free_box(mp4_bits_t *bs, free_box_t *freebox);
int write_free_box(mp4_bits_t *bs, free_box_t *freebox);
int alloc_struct_free_box(free_box_t **freebox);
int free_struct_free_box(free_box_t *freebox);

#endif
