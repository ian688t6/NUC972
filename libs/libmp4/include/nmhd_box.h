#ifndef __NMHD_BOX_H__
#define __NMHD_BOX_H__

typedef struct tag_nmhd_box {
	fullbox_header_t *boxheader;
	uint32_t struct_size;
} nmhd_box_t;

int read_nmhd_box(mp4_bits_t *bs, nmhd_box_t *nmhdbox);
int write_nmhd_box(mp4_bits_t *bs, nmhd_box_t *nmhdbox);
int alloc_struct_nmhd_box(nmhd_box_t **nmhdbox);
int free_struct_nmhd_box(nmhd_box_t *nmhdbox);

#endif
