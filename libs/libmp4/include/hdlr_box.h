#ifndef __HDLR_BOX_H__
#define __HDLR_BOX_H__

typedef struct tag_hdlr_box {
	fullbox_header_t *boxheader;
	uint32_t pre_defined;
	uint32_t handler_type;
	uint32_t reserved[3];
	uint8_t *name;
	uint32_t name_len;

	uint32_t struct_size;
} hdlr_box_t;

int read_hdlr_box(mp4_bits_t *bs, hdlr_box_t *hdlrbox);
int write_hdlr_box(mp4_bits_t *bs, hdlr_box_t *hdlrbox);
int alloc_struct_hdlr_box(hdlr_box_t **hdlrbox);
int free_struct_hdlr_box(hdlr_box_t *hdlrbox);

#endif
