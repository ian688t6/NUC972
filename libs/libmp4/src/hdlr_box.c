#include "mp4.h"

int read_hdlr_box(mp4_bits_t *bs, hdlr_box_t *hdlrbox)
{
	hdlrbox->struct_size = hdlrbox->boxheader->struct_size;

	hdlrbox->pre_defined = mp4_bs_read_u32(bs);
	hdlrbox->handler_type = mp4_bs_read_u32(bs);
	hdlrbox->reserved[0] = mp4_bs_read_u32(bs);
	hdlrbox->reserved[1] = mp4_bs_read_u32(bs);
	hdlrbox->reserved[2] = mp4_bs_read_u32(bs);
	hdlrbox->struct_size += 20;

	hdlrbox->name_len = hdlrbox->boxheader->size - hdlrbox->struct_size;
	hdlrbox->name = (uint8_t *)calloc(hdlrbox->name_len, sizeof(uint8_t));
	mp4_bs_read_data(bs, (char *)hdlrbox->name, hdlrbox->name_len);
	hdlrbox->struct_size += hdlrbox->name_len;

	return hdlrbox->struct_size;
}

int write_hdlr_box(mp4_bits_t *bs, hdlr_box_t *hdlrbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	hdlrbox->struct_size = write_fullbox_header(bs, hdlrbox->boxheader);

	mp4_bs_write_u32(bs, hdlrbox->pre_defined);
	mp4_bs_write_u32(bs, hdlrbox->handler_type);
	mp4_bs_write_u32(bs, hdlrbox->reserved[0]);
	mp4_bs_write_u32(bs, hdlrbox->reserved[1]);
	mp4_bs_write_u32(bs, hdlrbox->reserved[2]);
	hdlrbox->struct_size += 20;

	mp4_bs_write_data(bs, (char *)hdlrbox->name, hdlrbox->name_len);
	hdlrbox->struct_size += hdlrbox->name_len;

	hdlrbox->boxheader->size = hdlrbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, hdlrbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return hdlrbox->struct_size;
}

int alloc_struct_hdlr_box(hdlr_box_t **hdlrbox)
{
	hdlr_box_t * hdlrbox_t = *hdlrbox;
	if ((hdlrbox_t = (hdlr_box_t *)calloc(1, sizeof(hdlr_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_fullbox_header(&(hdlrbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*hdlrbox = hdlrbox_t;
	return 1;
}

int free_struct_hdlr_box(hdlr_box_t * hdlrbox)
{
	if (hdlrbox) {
		if (hdlrbox->boxheader) {
			free_struct_fullbox_header(hdlrbox->boxheader);
		}
		if (hdlrbox->name) {
			free(hdlrbox->name);
			hdlrbox->name = NULL;
		}

		free(hdlrbox);
		hdlrbox = NULL;
	}
	return 1;
}
