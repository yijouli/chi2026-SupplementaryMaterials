#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define E_INVALID_REQUEST -50
#define E_MEMORY_ERROR -25

#define MIN_CHUNK 32
#define MAX(a,b) ((a)>(b)?(a):(b))

typedef struct buffer_st {
	char *allocd;	/* pointer to allocated data */
	size_t max_length;
	size_t length;		/* API: current length */
} buffer_st;

/* Public string related functions */
typedef struct buffer_st *buffer_t;

void buffer_init(buffer_st * str)
{
	str->allocd = NULL;
	str->max_length = 0;
	str->length = 0;
}

void buffer_clear(buffer_st * str)
{
	if (str == NULL || str->allocd == NULL)
		return;
	free(str->allocd);
	str->allocd = NULL;
	str->max_length = 0;
	str->length = 0;
}

/**
 * buffer_append_data:
 * @dest: the buffer to append to
 * @data: the data
 * @data_size: the size of @data
 *
 * Appends the provided @data to the destination buffer.
 *
 * Returns: 0 on success, otherwise a negative error code.
 *
 **/
int buffer_append_data(buffer_t dest, const char *data, size_t data_size)
{
	size_t const tot_len = data_size + dest->length;

	if (data_size == 0)
		return 0;

	if (dest->max_length < tot_len) {
		size_t const new_len = MAX(data_size, MIN_CHUNK) + MAX(dest->max_length,MIN_CHUNK);

		dest->allocd = (char*) realloc(dest->allocd, new_len);
		if (dest->allocd == NULL) {
			return E_MEMORY_ERROR;
		}
		dest->max_length = new_len;
	}
	memcpy(&dest->allocd[dest->length], data, data_size);
	dest->length = tot_len;

	return 0;
}

/**
 * append_bson:
 * @dest: the buffer to append to
 * @data: the data
 * @data_size: the size of @data
 *
 * Appends {"NOBUG":data} as BSON
 * when data = "world"
 * {"NOBUG": "world" } →
 * \x16\x00\x00\x00           // total document size depands on data_size
 * \x02                       // 0x02 = type String
 * NOBUG\x00                  // field name
 * \x06\x00\x00\x00world\x00  // field size(include NULL) + value
 * \x00                       // 0x00 = type EOO ('end of object')
 *
 **/
static int append_bson(buffer_st * buf, const char *data, size_t data_size )
{	
	uint32_t *lengthp =(uint32_t*) &buf->allocd[buf->length];
	buf->length+= sizeof(*lengthp);
	uint32_t cur_length = buf->length;
	uint32_t sz =data_size;
	buffer_append_data(buf,"\x02", 1);
	buffer_append_data(buf,"NOBUG\0", 6); //plus NULL
	
	if (data[data_size-1] != '\0')
	{
		sz+=1;
		buffer_append_data(buf, (const char*)&sz, 4); //append size
		buffer_append_data(buf, data, data_size); // append data
		buffer_append_data(buf, "\0", 1); 
	}
	else
	{
		buffer_append_data(buf, (const char*)&sz, 4); //append size
		buffer_append_data(buf, data, data_size); // append data
	}
	buffer_append_data(buf, "\0", 1); 
	*lengthp = buf->length - cur_length;
	return 0;
}

#define MAX_SIZE 256

char tmp[MAX_SIZE];

int main(){

	int payload_size;
	scanf("%d",&payload_size);
	if (payload_size > MAX_SIZE || payload_size <0)
		exit(1);
	memset(tmp,'A',sizeof(tmp));

	buffer_st buff;
	buffer_init(&buff);
	buffer_append_data(&buff,"BSON:",5);
	append_bson(&buff, tmp, payload_size);
	for (size_t i=0;i<buff.length;++i)
		printf("0x%hhx ", buff.allocd[i]);
	buffer_clear(&buff);
	
	return 0;
}