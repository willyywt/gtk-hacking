#include "sap84data.h"

static const size_t MAX_BUF = 1000;

static char* readline(char* buf, FILE* file) {
	return fgets(buf, MAX_BUF, file);
}

Joint Joint_add(Joint a, Joint b) {
	Joint result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}
Joint Joint_multiply(Joint a, double n) {
	Joint result;
	result.x = a.x * n;
	result.y = a.y * n;
	return result;
}
Joint Joint_minus(Joint a, Joint b) {
	return Joint_add(a, Joint_multiply(b, -1.0));
}
Joint Joint_average(Joint a, Joint b) {
	return Joint_multiply(Joint_add(a, b), 0.5);
}
Joint Joint_average4(Joint a, Joint b, Joint c, Joint d) {
	return Joint_average(Joint_average(a, b), Joint_average(c, d));
}
double Joint_area(Joint O, Joint A, Joint B) {
	Joint OA = Joint_minus(A, O);
	Joint OB = Joint_minus(B, O);
	return (OA.x * OB.y - OA.y * OB.x) / 2;
}
void Joint_cpy(Joint* dst, const Joint* src) {
	dst->x = src->x;
	dst->y = src->y;
}
void Joint_swap(Joint* a, Joint* b) {
	Joint temp;
	Joint_cpy(&temp, a);
	Joint_cpy(a, b);
	Joint_cpy(b, &temp);
}
bool Joint_CCL_check(Joint a, Joint b, Joint c, Joint d) {
	return Joint_area(a, b, c) > 0 && Joint_area(b, c, d) > 0 && Joint_area(c, d, a) > 0 && Joint_area(d, a, b) > 0;
}
void Joint_CCL(Joint* a, Joint* b, Joint* c, Joint* d) {
	bool Sabc_positive = Joint_area(*a, *b, *c) > 0;
	bool Sabd_positive = Joint_area(*a, *b, *d) > 0;
	if (Sabc_positive ^ Sabd_positive) {
		// ab, cd are diagnoals of the Quadrilateral
		// Reorder to one of acbd or adbc
		if (Sabc_positive) {
			// adbc
			assert(Sabc_positive == true);
			assert(Sabd_positive == false);
			Joint temp;
			Joint_cpy(&temp, d);
			Joint_cpy(d, c);
			Joint_cpy(c, b);
			Joint_cpy(b, &temp);
		}
		else {
			// acbd
			assert(Sabc_positive == false);
			assert(Sabd_positive == true);
			Joint_swap(b, c);
		}
	}
	else {
		// ab, cd are two opposite sides of the Quadrilateral
		bool Sbcd_positive = Joint_area(*b, *c, *d) > 0;
		if (Sbcd_positive == Sabc_positive) {
			//abcd or adcb
			if (Sbcd_positive) {
				//abcd
				assert(Sbcd_positive == true);
				assert(Sabc_positive == true);
				;
			}
			else {
				//adcb
				assert(Sbcd_positive == false);
				assert(Sabc_positive == false);
				Joint_swap(b, d);
			}
		}
		else {
			//abdc or acdb
			if (Sbcd_positive) {
				//acdb
				assert(Sbcd_positive == true);
				assert(Sabc_positive == false);
				Joint temp;
				Joint_cpy(&temp, b);
				Joint_cpy(b, c);
				Joint_cpy(c, d);
				Joint_cpy(d, &temp);
			}
			else {
				//abdc
				assert(Sbcd_positive == false);
				assert(Sabc_positive == true);
				Joint_swap(c, d);
			}
		}
	}
	assert(Joint_CCL_check(*a, *b, *c, *d));
}

#define ERROR_RETURN(_line_count, _char_count, msg) do { \
	*error = new SAP84Data_error(); \
	(*error)->line_count = (_line_count); \
	(*error)->char_count = (_char_count) + 1; \
	(*error)->message = (msg); \
	delete data; \
	return nullptr; \
} while (0)


SAP84Data* SAP84_read(FILE* file, SAP84Data_error** error) {
	SAP84Data* data = new SAP84Data();

	char buf[MAX_BUF];
	size_t line_count = 1;
	size_t temp_char_count;
	int read_index = 0;
	int previous_line_count = 1;

	// Read joints
	if (readline(buf, file) == nullptr)
		ERROR_RETURN(0, 0, "Unexpected read failure");
	if (strstr(buf, "JOINT") == nullptr)
		ERROR_RETURN(1, 0, "The first line must start with \"JOINT\"");
	while (true) {
		// Read one line
		if (readline(buf, file) == nullptr)
			ERROR_RETURN(line_count, 0, "Unexpected read failure");
		line_count++;

		// Break when a line only consists of '\n' (End reading joints)
		if (strlen(buf) <= 1 || 
				(strlen(buf) == 2 && *buf == '\r'))
			break;

		char* tempbuf = buf;
		tempbuf += HW_JumpSpace(tempbuf);

		// Read and check index number
		if ((temp_char_count = HW_ScanInt(tempbuf, &read_index)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Expect index number of joint");
		if(read_index < 0 || read_index != line_count - previous_line_count)
			ERROR_RETURN(line_count, tempbuf - buf, "Joint index number is not consecutive");
		tempbuf += temp_char_count;

		if ((temp_char_count = HW_JumpSpace(tempbuf)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Expect space after joint index");
		tempbuf += temp_char_count;
		
		// Jump "C="
		if (tempbuf[0] != 'C' || tempbuf[1] != '=')
			ERROR_RETURN(line_count, tempbuf - buf, "Expect \"C=\"");
		tempbuf += 2;
		
		// The space after "C=" is optional
		tempbuf += HW_JumpSpace(tempbuf);

		// Ignore the double after "C="
		if ((temp_char_count = HW_ScanDouble(tempbuf, NULL)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Expect a double number after \"C=\"");
		tempbuf += temp_char_count;

		if ((temp_char_count = HW_JumpSpace(tempbuf)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Expect space after \"C=xxx\"");
		tempbuf += temp_char_count;

		// Start to read point x and y coordinate
		Joint joint;
		if ((temp_char_count = HW_ScanDouble(tempbuf, &joint.x)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Cannot read x coordinate");
		tempbuf += temp_char_count;

		if ((temp_char_count = HW_JumpSpace(tempbuf)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Expect space after x coordinate");
		tempbuf += temp_char_count;

		if ((temp_char_count = HW_ScanDouble(tempbuf, &joint.y)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Cannot read y coordinate");
		tempbuf += temp_char_count;

		// Finally, add the line's joint to data->joints
		data->joints.push_back(joint);
	}

	// Read plane
	if (readline(buf, file) == nullptr)
		ERROR_RETURN(line_count, 0, "Unexpected read failure");
	if (strstr(buf, "PLANE") == nullptr)
		ERROR_RETURN(line_count, 0, "The Plane section must start with \"PLANE\"");
	line_count++;
	previous_line_count = line_count;
	while (true) {
		// Read one line
		// Do not ERROR_RETURN when the ending '\n' is missing
		if (readline(buf, file) == nullptr)
			break;
		line_count++;

		// Break when a line only consists of '\n' (End reading joints)
		if (strlen(buf) <= 1)
			break;

		char* tempbuf = buf;
		tempbuf += HW_JumpSpace(tempbuf);

		// Read and check index number
		if ((temp_char_count = HW_ScanInt(tempbuf, &read_index)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Expect index number of plane");
		if (read_index < 0 || read_index != line_count - previous_line_count)
			ERROR_RETURN(line_count, tempbuf - buf, "Plane index number is not consecutive");
		tempbuf += temp_char_count;

		if ((temp_char_count = HW_JumpSpace(tempbuf)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Expect space after plane index");
		tempbuf += temp_char_count;

		// Jump "Q="
		if (tempbuf[0] != 'Q' || tempbuf[1] != '=')
			ERROR_RETURN(line_count, tempbuf - buf, "Expect \"Q=\"");
		tempbuf += 2;

		// The space after "Q=" is optional
		tempbuf += HW_JumpSpace(tempbuf);

		// read joints of this plane
		Plane plane;
		if ((temp_char_count = HW_ScanInt(tempbuf, &plane.a)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Cannot read 1st point of plane");
		if (plane.a > data->joints.size() || plane.a < 1)
			ERROR_RETURN(line_count, tempbuf - buf, "1st point of plane has invalid index");
		tempbuf += temp_char_count;
		if (tempbuf[0] != ',')
			ERROR_RETURN(line_count, tempbuf - buf, "Expcect ',' after reading 1st point of plane");
		tempbuf += 1;

		if ((temp_char_count = HW_ScanInt(tempbuf, &plane.b)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Cannot read 2nd point of plane");
		if (plane.b > data->joints.size() || plane.b < 1)
			ERROR_RETURN(line_count, tempbuf - buf, "2nd point of plane has invalid index");
		tempbuf += temp_char_count;
		if (tempbuf[0] != ',')
			ERROR_RETURN(line_count, tempbuf - buf, "Expcect ',' after reading 2nd point of plane");
		tempbuf += 1;

		if ((temp_char_count = HW_ScanInt(tempbuf, &plane.c)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Cannot read 3rd point of plane");
		if (plane.c > data->joints.size() || plane.c < 1)
			ERROR_RETURN(line_count, tempbuf - buf, "3rd point of plane has invalid index");
		tempbuf += temp_char_count;
		if (tempbuf[0] != ',')
			ERROR_RETURN(line_count, tempbuf - buf, "Expcect ',' after reading 3rd point of plane");
		tempbuf += 1;

		if ((temp_char_count = HW_ScanInt(tempbuf, &plane.d)) == 0)
			ERROR_RETURN(line_count, tempbuf - buf, "Cannot read 4th point of plane");
		if (plane.d > data->joints.size() || plane.d < 1)
			ERROR_RETURN(line_count, tempbuf - buf, "4th point of plane has invalid index");
		tempbuf += temp_char_count;

		// Check joint index
		

		// Finally, push plane to data->planes
		data->planes.push_back(plane);
	}
	return data;
}

void SAP84_delete(SAP84Data* data) {
	delete data;
}
