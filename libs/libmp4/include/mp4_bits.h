#ifndef __MP4_BITS_H__
#define __MP4_BITS_H__

#define USE_FILE_IO_INTERFACE

#ifndef USE_FILE_IO_INTERFACE
#define ONLY_READ_WRITE_USE_FILE_IO
#endif

/*the default size for new streams allocation...*/
#define BS_MEM_BLOCK_ALLOC_SIZE		163840 /* 160k */

enum {
	MP4_BITS_READ,
	MP4_BITS_WRITE,
	MP4_BITS_FILE_READ,
	MP4_BITS_FILE_WRITE,
	/*private mode if we own the buffer */
	MP4_BITS_WRITE_DYN
};

struct mp4_bits {
#ifndef USE_FILE_IO_INTERFACE
	/*original stream data */
	FILE *stream;
#endif
	int fd;

	/*original data */
	char *original;
	/*the size of our buffer */
	uint64_t size;
	/*current position in BYTES */
	uint64_t position;
	/*the byte readen/written */
	uint32_t current;
	/*the number of bits in the current byte */
	uint32_t nbBits;
	/*the bitstream mode */
	uint32_t bsmode;

	void (*end_of_stream)(void *par);
	void *par;
};

typedef struct mp4_bits mp4_bits_t;

mp4_bits_t *mp4_bs_create(char *buffer, uint64_t size, uint32_t mode);
#ifdef USE_FILE_IO_INTERFACE
mp4_bits_t *mp4_bs_create_from_file(int fd, uint32_t mode);
#else
mp4_bits_t *mp4_bs_create_from_file(FILE *f, uint32_t mode);
#endif
void mp4_bs_destroy(mp4_bits_t * bs);
uint32_t mp4_bs_read_int(mp4_bits_t * bs, uint32_t nBits);
uint64_t mp4_bs_read_long_int(mp4_bits_t * bs, uint32_t nBits);
float mp4_bs_read_float(mp4_bits_t * bs);
double mp4_bs_read_double(mp4_bits_t * bs);
uint32_t mp4_bs_read_data(mp4_bits_t * bs, char *data, uint32_t nbBytes);
uint32_t mp4_bs_read_u8(mp4_bits_t * bs);
uint32_t mp4_bs_read_u16(mp4_bits_t * bs);
uint32_t mp4_bs_read_u24(mp4_bits_t * bs);
uint32_t mp4_bs_read_u32(mp4_bits_t * bs);
uint64_t mp4_bs_read_u64(mp4_bits_t * bs);
uint32_t mp4_bs_read_u32_le(mp4_bits_t * bs);
uint16_t mp4_bs_read_u16_le(mp4_bits_t * bs);
void mp4_bs_write_int(mp4_bits_t * bs, int32_t value, int32_t nBits);
void mp4_bs_write_long_int(mp4_bits_t * bs, int64_t value, int32_t nBits);
void mp4_bs_write_float(mp4_bits_t * bs, float value);
void mp4_bs_write_double(mp4_bits_t * bs, double value);
uint32_t mp4_bs_write_data(mp4_bits_t * bs, char *data, uint32_t nbBytes);
void mp4_bs_write_u8(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u16(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u24(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u32(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u64(mp4_bits_t * bs, uint64_t value);
void mp4_bs_write_u32_le(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u16_le(mp4_bits_t * bs, uint32_t value);
void mp4_bs_set_eos_callback(mp4_bits_t * bs, void (*EndOfStream)(void *par), void *par);
uint8_t mp4_bs_align(mp4_bits_t * bs);
uint64_t mp4_bs_available(mp4_bits_t * bs);
void mp4_bs_get_content(mp4_bits_t * bs, char **output, uint32_t * outSize);
void mp4_bs_obtain_content(struct mp4_bits *bs, char **output, uint32_t *outSize);
void mp4_bs_skip_bytes(mp4_bits_t * bs, uint64_t nbBytes);
int mp4_bs_seek(mp4_bits_t * bs, uint64_t offset);
uint32_t mp4_bs_peek_bits(mp4_bits_t * bs, uint32_t numBits, uint32_t byte_offset);
uint8_t mp4_bs_bits_available(mp4_bits_t * bs);
uint64_t mp4_bs_get_position(mp4_bits_t * bs);
uint64_t mp4_bs_get_size(mp4_bits_t * bs);
uint64_t mp4_bs_get_refreshed_size(mp4_bits_t * bs);

/* used by sample read */
uint32_t mp4_bs_pread_data(mp4_bits_t *bs, char *data, uint32_t nbBytes, uint64_t offset);

#endif
