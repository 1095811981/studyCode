#include "cache.h"
#include "json-writer.h"

static const char *expect_obj1 = "{\"a\":\"abc\",\"b\":42,\"c\":true}";
static const char *expect_obj2 = "{\"a\":-1,\"b\":2147483647,\"c\":0}";
static const char *expect_obj3 = "{\"a\":0,\"b\":4294967295,\"c\":18446744073709551615}";
static const char *expect_obj4 = "{\"t\":true,\"f\":false,\"n\":null}";
static const char *expect_obj5 = "{\"abc\\tdef\":\"abc\\\\def\"}";
static const char *expect_obj6 = "{\"a\":3.14}";

static const char *pretty_obj1 = ("{\n"
				  "  \"a\": \"abc\",\n"
				  "  \"b\": 42,\n"
				  "  \"c\": true\n"
				  "}");
static const char *pretty_obj2 = ("{\n"
				  "  \"a\": -1,\n"
				  "  \"b\": 2147483647,\n"
				  "  \"c\": 0\n"
				  "}");
static const char *pretty_obj3 = ("{\n"
				  "  \"a\": 0,\n"
				  "  \"b\": 4294967295,\n"
				  "  \"c\": 18446744073709551615\n"
				  "}");
static const char *pretty_obj4 = ("{\n"
				  "  \"t\": true,\n"
				  "  \"f\": false,\n"
				  "  \"n\": null\n"
				  "}");

static struct json_writer obj1 = JSON_WRITER_INIT;
static struct json_writer obj2 = JSON_WRITER_INIT;
static struct json_writer obj3 = JSON_WRITER_INIT;
static struct json_writer obj4 = JSON_WRITER_INIT;
static struct json_writer obj5 = JSON_WRITER_INIT;
static struct json_writer obj6 = JSON_WRITER_INIT;

static void make_obj1(int pretty)
{
	jw_object_begin(&obj1, pretty);
	{
		jw_object_string(&obj1, "a", "abc");
		jw_object_int(&obj1, "b", 42);
		jw_object_true(&obj1, "c");
	}
	jw_end(&obj1);
}

static void make_obj2(int pretty)
{
	jw_object_begin(&obj2, pretty);
	{
		jw_object_int(&obj2, "a", -1);
		jw_object_int(&obj2, "b", 0x7fffffff);
		jw_object_int(&obj2, "c", 0);
	}
	jw_end(&obj2);
}

static void make_obj3(int pretty)
{
	jw_object_begin(&obj3, pretty);
	{
		jw_object_uint64(&obj3, "a", 0);
		jw_object_uint64(&obj3, "b", 0xffffffff);
		jw_object_uint64(&obj3, "c", 0xffffffffffffffffULL);
	}
	jw_end(&obj3);
}

static void make_obj4(int pretty)
{
	jw_object_begin(&obj4, pretty);
	{
		jw_object_true(&obj4, "t");
		jw_object_false(&obj4, "f");
		jw_object_null(&obj4, "n");
	}
	jw_end(&obj4);
}

static void make_obj5(int pretty)
{
	jw_object_begin(&obj5, pretty);
	{
		jw_object_string(&obj5, "abc" "\x09" "def", "abc" "\\" "def");
	}
	jw_end(&obj5);
}

static void make_obj6(int pretty)
{
	jw_object_begin(&obj6, pretty);
	{
		jw_object_double(&obj6, "a", 2, 3.14159);
	}
	jw_end(&obj6);
}

static const char *expect_arr1 = "[\"abc\",42,true]";
static const char *expect_arr2 = "[-1,2147483647,0]";
static const char *expect_arr3 = "[0,4294967295,18446744073709551615]";
static const char *expect_arr4 = "[true,false,null]";

static const char *pretty_arr1 = ("[\n"
				  "  \"abc\",\n"
				  "  42,\n"
				  "  true\n"
				  "]");
static const char *pretty_arr2 = ("[\n"
				  "  -1,\n"
				  "  2147483647,\n"
				  "  0\n"
				  "]");
static const char *pretty_arr3 = ("[\n"
				  "  0,\n"
				  "  4294967295,\n"
				  "  18446744073709551615\n"
				  "]");
static const char *pretty_arr4 = ("[\n"
				  "  true,\n"
				  "  false,\n"
				  "  null\n"
				  "]");

static struct json_writer arr1 = JSON_WRITER_INIT;
static struct json_writer arr2 = JSON_WRITER_INIT;
static struct json_writer arr3 = JSON_WRITER_INIT;
static struct json_writer arr4 = JSON_WRITER_INIT;

static void make_arr1(int pretty)
{
	jw_array_begin(&arr1, pretty);
	{
		jw_array_string(&arr1, "abc");
		jw_array_int(&arr1, 42);
		jw_array_true(&arr1);
	}
	jw_end(&arr1);
}

static void make_arr2(int pretty)
{
	jw_array_begin(&arr2, pretty);
	{
		jw_array_int(&arr2, -1);
		jw_array_int(&arr2, 0x7fffffff);
		jw_array_int(&arr2, 0);
	}
	jw_end(&arr2);
}

static void make_arr3(int pretty)
{
	jw_array_begin(&arr3, pretty);
	{
		jw_array_uint64(&arr3, 0);
		jw_array_uint64(&arr3, 0xffffffff);
		jw_array_uint64(&arr3, 0xffffffffffffffffULL);
	}
	jw_end(&arr3);
}

static void make_arr4(int pretty)
{
	jw_array_begin(&arr4, pretty);
	{
		jw_array_true(&arr4);
		jw_array_false(&arr4);
		jw_array_null(&arr4);
	}
	jw_end(&arr4);
}

static char *expect_nest1 =
	"{\"obj1\":{\"a\":\"abc\",\"b\":42,\"c\":true},\"arr1\":[\"abc\",42,true]}";

static struct json_writer nest1 = JSON_WRITER_INIT;

static void make_nest1(int pretty)
{
	jw_object_begin(&nest1, pretty);
	{
		jw_object_sub_jw(&nest1, "obj1", &obj1);
		jw_object_sub_jw(&nest1, "arr1", &arr1);
	}
	jw_end(&nest1);
}

static char *expect_inline1 =
	"{\"obj1\":{\"a\":\"abc\",\"b\":42,\"c\":true},\"arr1\":[\"abc\",42,true]}";

static char *pretty_inline1 =
	("{\n"
	 "  \"obj1\": {\n"
	 "    \"a\": \"abc\",\n"
	 "    \"b\": 42,\n"
	 "    \"c\": true\n"
	 "  },\n"
	 "  \"arr1\": [\n"
	 "    \"abc\",\n"
	 "    42,\n"
	 "    true\n"
	 "  ]\n"
	 "}");

static struct json_writer inline1 = JSON_WRITER_INIT;

static void make_inline1(int pretty)
{
	jw_object_begin(&inline1, pretty);
	{
		jw_object_inline_begin_object(&inline1, "obj1");
		{
			jw_object_string(&inline1, "a", "abc");
			jw_object_int(&inline1, "b", 42);
			jw_object_true(&inline1, "c");
		}
		jw_end(&inline1);
		jw_object_inline_begin_array(&inline1, "arr1");
		{
			jw_array_string(&inline1, "abc");
			jw_array_int(&inline1, 42);
			jw_array_true(&inline1);
		}
		jw_end(&inline1);
	}
	jw_end(&inline1);
}

static char *expect_inline2 =
	"[[1,2],[3,4],{\"a\":\"abc\"}]";

static char *pretty_inline2 =
	("[\n"
	 "  [\n"
	 "    1,\n"
	 "    2\n"
	 "  ],\n"
	 "  [\n"
	 "    3,\n"
	 "    4\n"
	 "  ],\n"
	 "  {\n"
	 "    \"a\": \"abc\"\n"
	 "  }\n"
	 "]");

static struct json_writer inline2 = JSON_WRITER_INIT;

static void make_inline2(int pretty)
{
	jw_array_begin(&inline2, pretty);
	{
		jw_array_inline_begin_array(&inline2);
		{
			jw_array_int(&inline2, 1);
			jw_array_int(&inline2, 2);
		}
		jw_end(&inline2);
		jw_array_inline_begin_array(&inline2);
		{
			jw_array_int(&inline2, 3);
			jw_array_int(&inline2, 4);
		}
		jw_end(&inline2);
		jw_array_inline_begin_object(&inline2);
		{
			jw_object_string(&inline2, "a", "abc");
		}
		jw_end(&inline2);
	}
	jw_end(&inline2);
}

/*
 * When super is compact, we expect subs to be compacted (even if originally
 * pretty).
 */
static const char *expect_mixed1 =
	("{\"obj1\":{\"a\":\"abc\",\"b\":42,\"c\":true},"
	 "\"arr1\":[\"abc\",42,true]}");

/*
 * When super is pretty, a compact sub (obj1) is kept compact and a pretty
 * sub (arr1) is re-indented.
 */
static const char *pretty_mixed1 =
	("{\n"
	 "  \"obj1\": {\"a\":\"abc\",\"b\":42,\"c\":true},\n"
	 "  \"arr1\": [\n"
	 "    \"abc\",\n"
	 "    42,\n"
	 "    true\n"
	 "  ]\n"
	 "}");

static struct json_writer mixed1 = JSON_WRITER_INIT;

static void make_mixed1(int pretty)
{
	jw_init(&obj1);
	jw_init(&arr1);

	make_obj1(0); /* obj1 is compact */
	make_arr1(1); /* arr1 is pretty */

	jw_object_begin(&mixed1, pretty);
	{
		jw_object_sub_jw(&mixed1, "obj1", &obj1);
		jw_object_sub_jw(&mixed1, "arr1", &arr1);
	}
	jw_end(&mixed1);
}

static void cmp(const char *test, const struct json_writer *jw, const char *exp)
{
	if (!strcmp(jw->json.buf, exp))
		return;

	printf("error[%s]: observed '%s' expected '%s'\n",
	       test, jw->json.buf, exp);
	exit(1);
}

#define t(v) do { make_##v(0); cmp(#v, &v, expect_##v); } while (0)
#define p(v) do { make_##v(1); cmp(#v, &v, pretty_##v); } while (0)

/*
 * Run some basic regression tests with some known patterns.
 * These tests also demonstrate how to use the jw_ API.
 */
static int unit_tests(void)
{
	/* comptact (canonical) forms */
	t(obj1);
	t(obj2);
	t(obj3);
	t(obj4);
	t(obj5);
	t(obj6);

	t(arr1);
	t(arr2);
	t(arr3);
	t(arr4);

	t(nest1);

	t(inline1);
	t(inline2);

	jw_init(&obj1);
	jw_init(&obj2);
	jw_init(&obj3);
	jw_init(&obj4);

	jw_init(&arr1);
	jw_init(&arr2);
	jw_init(&arr3);
	jw_init(&arr4);

	jw_init(&inline1);
	jw_init(&inline2);

	/* pretty forms */
	p(obj1);
	p(obj2);
	p(obj3);
	p(obj4);

	p(arr1);
	p(arr2);
	p(arr3);
	p(arr4);

	p(inline1);
	p(inline2);

	/* mixed forms */
	t(mixed1);
	jw_init(&mixed1);
	p(mixed1);

	return 0;
}

#define STMT(s) do { s } while (0)

#define PARAM(tok, lbl, p) \
	STMT( if (!(p) || (*(p) == '@')) \
		die("token '%s' requires '%s' parameter, but saw: '%s'", \
		    tok, lbl, p); )

#define KEY() \
	STMT( key = argv[++k]; \
	      PARAM(a_k, "key", key); )

#define VAL() \
	STMT( val = argv[++k]; \
	      PARAM(a_k, "val", val); )

#define PRECISION() \
	STMT( const char *s = argv[++k]; \
	      PARAM(a_k, "precision", s); \
	      precision = strtol(s, &endptr, 10); \
	      if (*endptr || errno == ERANGE) \
		die("invalid '%s' precision: '%s'", a_k, s); )

#define VAL_INT() \
	STMT( VAL(); \
	      v_int = strtol(val, &endptr, 10); \
	      if (*endptr || errno == ERANGE) \
		die("invalid '%s' value: '%s'",	a_k, val); )

#define VAL_UINT64() \
	STMT( VAL(); \
	      v_uint64 = strtoull(val, &endptr, 10); \
	      if (*endptr || errno == ERANGE) \
		die("invalid '%s' value: '%s'",	a_k, val); )

#define VAL_DOUBLE() \
	STMT( VAL(); \
	      v_double = strtod(val, &endptr); \
	      if (*endptr || errno == ERANGE) \
		die("invalid '%s' value: '%s'",	a_k, val); )

static int pretty;

static inline int scripted(int argc, const char **argv)
{
	struct json_writer jw = JSON_WRITER_INIT;
	int k;

	if (!strcmp(argv[0], "@object"))
		jw_object_begin(&jw, pretty);
	else if (!strcmp(argv[0], "@array"))
		jw_array_begin(&jw, pretty);
	else
		die("expected '@object' or '@array': '%s'", argv[0]);

	for (k = 1; k < argc; k++) {
		const char *a_k = argv[k];
		const char *key;
		const char *val;
		char *endptr;
		int precision;
		int v_int;
		uint64_t v_uint64;
		double v_double;

		if (!strcmp(a_k, "@end")) {
			jw_end(&jw);
			continue;
		}

		if (!strcmp(a_k, "@object-string")) {
			KEY();
			VAL();
			jw_object_string(&jw, key, val);
			continue;
		}
		if (!strcmp(a_k, "@object-int")) {
			KEY();
			VAL_INT();
			jw_object_int(&jw, key, v_int);
			continue;
		}
		if (!strcmp(a_k, "@object-uint64")) {
			KEY();
			VAL_UINT64();
			jw_object_uint64(&jw, key, v_uint64);
			continue;
		}
		if (!strcmp(a_k, "@object-double")) {
			KEY();
			PRECISION();
			VAL_DOUBLE();
			jw_object_double(&jw, key, precision, v_double);
			continue;
		}
		if (!strcmp(a_k, "@object-true")) {
			KEY();
			jw_object_true(&jw, key);
			continue;
		}
		if (!strcmp(a_k, "@object-false")) {
			KEY();
			jw_object_false(&jw, key);
			continue;
		}
		if (!strcmp(a_k, "@object-null")) {
			KEY();
			jw_object_null(&jw, key);
			continue;
		}
		if (!strcmp(a_k, "@object-object")) {
			KEY();
			jw_object_inline_begin_object(&jw, key);
			continue;
		}
		if (!strcmp(a_k, "@object-array")) {
			KEY();
			jw_object_inline_begin_array(&jw, key);
			continue;
		}

		if (!strcmp(a_k, "@array-string")) {
			VAL();
			jw_array_string(&jw, val);
			continue;
		}
		if (!strcmp(a_k, "@array-int")) {
			VAL_INT();
			jw_array_int(&jw, v_int);
			continue;
		}
		if (!strcmp(a_k, "@array-uint64")) {
			VAL_UINT64();
			jw_array_uint64(&jw, v_uint64);
			continue;
		}
		if (!strcmp(a_k, "@array-double")) {
			PRECISION();
			VAL_DOUBLE();
			jw_array_double(&jw, precision, v_double);
			continue;
		}
		if (!strcmp(a_k, "@array-true")) {
			jw_array_true(&jw);
			continue;
		}
		if (!strcmp(a_k, "@array-false")) {
			jw_array_false(&jw);
			continue;
		}
		if (!strcmp(a_k, "@array-null")) {
			jw_array_null(&jw);
			continue;
		}
		if (!strcmp(a_k, "@array-object")) {
			jw_array_inline_begin_object(&jw);
			continue;
		}
		if (!strcmp(a_k, "@array-array")) {
			jw_array_inline_begin_array(&jw);
			continue;
		}

		die("unrecognized token: '%s'", a_k);
	}

	if (!jw_is_terminated(&jw))
		die("json not terminated: '%s'", jw.json.buf);

	printf("%s\n", jw.json.buf);

	strbuf_release(&jw.json);
	return 0;
}

static inline int my_usage(void)
{
	die("usage: '-u' | '[-p] @object ... @end' | '[-p] @array ... @end'");
}

int cmd_main(int argc, const char **argv)
{
	if (argc <= 1)
		return my_usage();

	argc--;
	argv++;

	if (argv[0][0] == '-') {
		if (!strcmp(argv[0], "-u") || !strcmp(argv[0], "--unit"))
			return unit_tests();

		if (!strcmp(argv[0], "-p") || !strcmp(argv[0], "--pretty")) {
			pretty = 1;
			argc--;
			argv++;
			if (argc < 1)
				return my_usage();
		}
	}

	return scripted(argc, argv);
}
