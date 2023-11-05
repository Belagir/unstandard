
#include <ustd/strings.h>

i32 string_cmp(const string *str1, const string *str2)
{
	size_t pos = { 0 };
	size_t limit = min(str1->length, str2->length);
	i32 result = { 0 };

	while ((result == 0) && (pos < limit)) {
		result = (str1->data[pos] > str2->data[pos]) - (str1->data[pos] < str2->data[pos]);
		pos += 1;
	}

	if (result != 0) {
		return result;
	}

	if (str1->length == str2->length) {
		return result;
	} else if (str1->length > str2->length) {
		return 1;
	} else {
		return -1;
	}
}
