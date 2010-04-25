#include "record.h"

#include <malloc.h>
#include <memory.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

static const int MAX_RECORDS_PER_FILE = 100;
static const char KEY_TERMINATOR = ':';
static const char RECORD_TERMINATOR = '%';
static const char MAX_TOKEN_SIZE = 120;

RecordNode* NewRecordNode()
{
	RecordNode* n = (RecordNode*)malloc(sizeof(RecordNode));
	n->next = 0;
	return n;
}

FieldNode* NewFieldNode()
{
	FieldNode* n = (FieldNode*)malloc(sizeof(FieldNode));
	n->next = 0;
	return n;
}

RecordNode* record_read(FILE* file_in)
{
	RecordNode* root = NULL;
	RecordNode* r = NULL;
	FieldNode* f = NULL;
	
	char c;
	int reading_key = TRUE;

	char token[MAX_TOKEN_SIZE];
	char* t = token;

	while ((c = getc(file_in)) != EOF)
	{
		if (reading_key)
		{
			// key token is complete
			if (c == KEY_TERMINATOR)
			{
				// Create a new field
				if (f)
				{
					// append to list
					f->next = NewFieldNode();
					f = f->next;
				}
				else
				{
					// create new list
					f = NewFieldNode();
					if (!r)
					{
						// create the FIRST record.
						root = NewRecordNode();
						r = root;
					}
					r->fields = f;
				}

				// copy token into f->key
				*(t++) = 0;
				f->key = (char*)malloc(sizeof(token)+1);
				strcpy(f->key, token);

				// begin reading the value token
				t = token;
				reading_key = FALSE;
			}
			// record is complete
			else if (c == RECORD_TERMINATOR)
			{
				// create a new record
				r->next = NewRecordNode();
				r = r->next;
				f = NULL;

				// skip newline
				getc(file_in);
			}
			else
				*(t++) = c;
		}
		else
		{
			// value token is complete
			if (c == '\n')
			{
				// copy token into f->value
				*(t++) = 0;
				f->value = (char*)malloc(sizeof(token)+1);
				strcpy(f->value, token);

				// begin reading the next key
				t = token;
				reading_key = TRUE;
			}
			else
				*(t++) = c;
		}
	}
	return root;
}

void record_free(RecordNode* node)
{
	RecordNode* r = node, *prev_r;
	while (r)
	{
		FieldNode* f = r->fields, *prev_f;
		while(f)
		{
			free(f->key);
			free(f->value);
			prev_f = f;
			f = f->next;
			free(prev_f);
		}
		prev_r = r;
		r = r->next;
		free(prev_r);
	}
}

// Returns the n'th record in the records file.
const RecordNode* record_by_index(const RecordNode* node, int n)
{
	const RecordNode* r = node;
	int i;
	for (i = 0; i < n; ++i, r = r->next);
	return r;
}

const char* record_lookup_value(const RecordNode* node, const char* key)
{
	FieldNode* f = node->fields;
	while (f)
	{
		if (strcmp(f->key, key) == 0)
			return f->value;
		f = f->next;
	}
	return NULL;
}
