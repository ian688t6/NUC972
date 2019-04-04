#ifndef __SMHD_BOX_H__
#define __SMHD_BOX_H__

typedef struct tag_smhd_box {
	fullbox_header_t *boxheader;
	uint16_t balance;
	uint16_t reserved;

	uint32_t struct_size;
} smhd_box_t;

int read_smhd_box(mp4_bits_t *bs, smhd_box_t *smhdbox);
int write_smhd_box(mp4_bits_t *bs, smhd_box_t *smhdbox);
int alloc_struct_smhd_box(smhd_box_t **smhdbox);
int free_struct_smhd_box(smhd_box_t *smhdbox);

#endif
