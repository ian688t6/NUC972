#include "mp4.h"

int read_url_box(mp4_bits_t *bs, url_box_t *urlbox)
{
	urlbox->struct_size = urlbox->boxheader->struct_size;
	return urlbox->struct_size;
}

int write_url_box(mp4_bits_t *bs, url_box_t *urlbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	urlbox->struct_size = write_fullbox_header(bs, urlbox->boxheader);

	urlbox->boxheader->size = urlbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, urlbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return urlbox->struct_size;
}

int alloc_struct_url_box(url_box_t **urlbox)
{
	url_box_t *urlbox_t = *urlbox;
	if ((urlbox_t = (url_box_t *)calloc(1, sizeof(url_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_fullbox_header(&(urlbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*urlbox = urlbox_t;
	return 1;
}

int free_struct_url_box(url_box_t *urlbox)
{
	if (urlbox) {
		if (urlbox->boxheader) {
			free_struct_fullbox_header(urlbox->boxheader);
		}
		free(urlbox);
		urlbox = NULL;
	}
	return 1;
}
