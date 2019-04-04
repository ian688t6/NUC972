#include "mp4.h"

int read_dref_box(mp4_bits_t *bs, dref_box_t *drefbox)
{
	fullbox_header_t *boxhead;
	url_box_t *urlbox;

	drefbox->struct_size = drefbox->boxheader->struct_size;

	drefbox->entry_count = mp4_bs_read_u32(bs);
	drefbox->struct_size += 4;

	alloc_struct_fullbox_header(&boxhead);
	read_fullbox_header(bs, boxhead);

	alloc_struct_url_box(&urlbox);
	memcpy(urlbox->boxheader, boxhead, sizeof(fullbox_header_t));
	drefbox->struct_size += read_url_box(bs, urlbox);
	drefbox->urlbox = urlbox;

	free_struct_fullbox_header(boxhead);

	return drefbox->struct_size;
}

int write_dref_box(mp4_bits_t *bs, dref_box_t *drefbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	drefbox->struct_size = write_fullbox_header(bs, drefbox->boxheader);

	mp4_bs_write_u32(bs, drefbox->entry_count);
	drefbox->struct_size += 4;

	if (drefbox->urlbox) {
		drefbox->struct_size += write_url_box(bs, drefbox->urlbox);
	}

	drefbox->boxheader->size = drefbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, drefbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return drefbox->struct_size;
}

int alloc_struct_dref_box(dref_box_t **drefbox)
{
	dref_box_t *drefbox_t = *drefbox;
	if ((drefbox_t = (dref_box_t *)calloc(1, sizeof(dref_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_fullbox_header(&(drefbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*drefbox = drefbox_t;
	return 1;
}

int free_struct_dref_box(dref_box_t *drefbox)
{
	if (drefbox) {
		if (drefbox->boxheader) {
			free_struct_fullbox_header(drefbox->boxheader);
		}
		free(drefbox);
		drefbox = NULL;
	}
	return 1;
}
