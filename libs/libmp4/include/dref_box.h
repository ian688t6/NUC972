#ifndef __DREF_BOX_H__
#define __DREF_BOX_H__

typedef struct tag_dref_box {
	fullbox_header_t *boxheader;

	uint32_t entry_count;
	url_box_t *urlbox;

	uint32_t struct_size;
} dref_box_t;

int read_dref_box(mp4_bits_t *bs, dref_box_t *drefbox);
int write_dref_box(mp4_bits_t *bs, dref_box_t *drefbox);
int alloc_struct_dref_box(dref_box_t **drefbox);
int free_struct_dref_box(dref_box_t *drefbox);

#endif /* __DREF_BOX_H__ */
