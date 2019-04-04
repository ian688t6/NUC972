#ifndef __Dinf_BOX_H__
#define __Dinf_BOX_H__

typedef struct tag_dinf_box {
	box_header_t *boxheader;

	dref_box_t *drefbox;

	uint32_t struct_size;
} dinf_box_t;

int read_dinf_box(mp4_bits_t *bs, dinf_box_t *dinfbox);
int write_dinf_box(mp4_bits_t *bs, dinf_box_t *dinfbox);
int alloc_struct_dinf_box(dinf_box_t **dinfbox);
int free_struct_dinf_box(dinf_box_t *dinfbox);

#endif /* __Dinf_BOX_H__ */
