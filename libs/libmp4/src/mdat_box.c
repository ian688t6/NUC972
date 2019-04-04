#include "mp4.h"
#include <unistd.h>

int read_mdat_box(mp4_bits_t *bs, mdat_box_t *mdatbox)
{
	mdatbox->struct_size = mdatbox->boxheader->struct_size;
	mdatbox->data_size = mdatbox->boxheader->size - mdatbox->boxheader->struct_size;
	if (mdatbox->data_size > 0) {
		if ((mdatbox->data = (unsigned char *)calloc(mdatbox->data_size, sizeof(char))) == NULL) {
			mp4_loge("calloc Failed ");
			return 0;
		}
		mp4_bs_read_data(bs, (char *)mdatbox->data, mdatbox->data_size);
		mdatbox->struct_size += mdatbox->data_size;
	}

	return mdatbox->struct_size;
}

int write_mdat_box_head(mp4_bits_t *bs, mdat_box_t *mdatbox)
{
	mdatbox->head_pos = mp4_bs_get_position(bs);

	mdatbox->struct_size = write_box_header(bs, mdatbox->boxheader);
	mdatbox->data_size = 0;

	return mdatbox->struct_size;
}

int write_mdat_box_data(mp4_bits_t *bs, mdat_box_t *mdatbox, uint8_t *data, uint32_t data_len)
{
	write(bs->fd, data, data_len);
	mdatbox->data_size += data_len;
	mdatbox->struct_size += data_len;

	return data_len;
}

int write_mdat_box_tail(mp4_bits_t *bs, mdat_box_t *mdatbox)
{
	int i;
	int count;
	char *data;
	long tail_pos;

	mdatbox->boxheader->size = mdatbox->struct_size;
	tail_pos = lseek(bs->fd, 0, SEEK_CUR);
	lseek(bs->fd, mdatbox->head_pos, SEEK_SET);

	count = sizeof(mdatbox->boxheader->size);
	data = (char *)&mdatbox->boxheader->size;
	for (i = 0; i < count; i++)
		write(bs->fd, &data[count - i - 1], 1);
	lseek(bs->fd, tail_pos, SEEK_SET);

	return mdatbox->struct_size;
}

int alloc_struct_mdat_box(mdat_box_t **mdatbox)
{
	mdat_box_t *mdatbox_t = *mdatbox;

	if ((mdatbox_t = (mdat_box_t *)calloc(1, sizeof(mdat_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(mdatbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*mdatbox = mdatbox_t;
	return 1;
}

int free_struct_mdat_box(mdat_box_t *mdatbox)
{
	if (mdatbox) {
		if (free_struct_box_header(mdatbox->boxheader)) {
			if (mdatbox->data) {
				free(mdatbox->data);
				mdatbox->data = NULL;
			}
		}
		free(mdatbox);
		mdatbox = NULL;
	}

	return 1;
}
