#ifndef __URL_BOX_H__
#define __URL_BOX_H__

typedef struct tag_url_box {
	fullbox_header_t *boxheader;

	uint32_t struct_size;
} url_box_t;

int read_url_box(mp4_bits_t *bs, url_box_t *urlbox);
int write_url_box(mp4_bits_t *bs, url_box_t *urlbox);
int alloc_struct_url_box(url_box_t **urlbox);
int free_struct_url_box(url_box_t *urlbox);

#endif /* __URL_BOX_H__ */
