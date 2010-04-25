
#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>

struct FieldNode_t
{
	struct FieldNode_t* next;
	char* key;
	char* value;
};
typedef struct FieldNode_t FieldNode;

struct RecordNode_t
{
	struct RecordNode_t* next;
	FieldNode* fields;
};
typedef struct RecordNode_t RecordNode;

// Returns list of records read in from a record file
RecordNode* record_read(FILE* file_in);

// Frees a list of records which was previously created with record_read
void record_free(RecordNode* node);

// gives the n'th record in the records file.
const RecordNode* record_by_index(const RecordNode* node, int n);

// looks up a field value using a string key.
const char* record_lookup_value(const RecordNode* node, const char* key);

#endif
