#ifndef __HMHD_BOX_H__
#define __HMHD_BOX_H__

typedef struct tag_hmhd_box {
	fullbox_header_t *boxheader;
	uint16_t maxPDUsize;
	uint16_t avgPDUsize;
	uint32_t maxbitrate;
	uint32_t avgbitrate;
	uint32_t reserved;

	uint32_t struct_size;
} hmhd_box_t;

int read_hmhd_box(mp4_bits_t *bs, hmhd_box_t *hmhdbox);
int write_hmhd_box(mp4_bits_t *bs, hmhd_box_t *hmhdbox);
int alloc_struct_hmhd_box(hmhd_box_t **hmhdbox);
int free_struct_hmhd_box(hmhd_box_t * hmhdbox);

#endif
