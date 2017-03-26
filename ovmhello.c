#include "ovm.c"

// Begin String definitions
struct mystring { _VTABLE_REF; int length; char *chars; }; 
typedef struct mystring *HString;

static struct vtable *String_vt;
static struct object *String;

// String>>#new:
static struct object *String_newp(struct closure *cls, struct object *self, char *chars)
{
	HString clone = (HString)send(vtof(self), s_vtallocate, sizeof(struct mystring));

	clone->length = strlen(chars);
	clone->chars  = strdup(chars);
	return (struct object *)clone;
}

// String>>#length
static struct object *String_length(struct closure *cls, HString self) { return i2oop(self->length); }

// String>>#print
static struct object * String_print(struct closure * cls, HString self)
{
	int i;

	for (i = 0; i < self->length; i++)
		putchar(self->chars[i]);
	return (struct object * )self;
}

// String>>#append
static struct object * String_append(struct closure * cls, HString self, HString arg)
{
	int i, j;
	char charString[self->length + arg->length + 1];	
	for (i = 0; i < self->length; i++)
		charString[i] = self->chars[i];

	for (j = 0; j < arg->length; j++,i++)
	{
		charString[i] = arg->chars[j];
	}
	charString[i] = '\0';
	struct object *tempString = send(String, s_newp, charString);	
	return (struct object * )tempString;
}

// ------------------------ Begin Array definitions
struct array { _VTABLE_REF; int length; struct object **contents; };
typedef struct array *HArray;

static struct vtable *Array_vt;
static struct object *Array;

//Array>>#new:
static struct object *Array_newp(struct closure *cls, struct object *self, int length)
{
	HArray clone = (HArray)send(vtof(self), s_vtallocate, sizeof(struct array));

	clone->length   = length;
	clone->contents = (struct object **)calloc(clone->length, sizeof(struct object *));
	assert(clone->contents);
	return (struct object *)clone;
}

//Array>>#length
static struct object *Array_length(struct closure * cls, HArray self) { return i2oop(self->length); }

//Array>>#at:
static struct object *Array_at(struct closure *cls, HArray self, int ix)
{
	// index starts at 1
	if (0 < ix && ix <= self->length)
		return self->contents[ix-1];
	return 0;
}

//Array>>#at:put:
static struct object *Array_atput(struct closure *cls, HArray self, int ix, struct object *rval)
{
	// index starts at 1
	if (0 < ix && ix <= self->length)
		return self->contents[ix-1] = rval;
	return rval;
}

static struct symbol *s_at;
static struct symbol *s_atput;

int main(int argc, char *argv[])
{
	init_ovm();

	s_at    = (typeof(s_at))   send(Symbol, s_newp, "at:");
	s_atput = (typeof(s_atput))send(Symbol, s_newp, "at:put:");

	printf("Testing String\n");
	String_vt = (typeof(String_vt))send(Object_vt, s_vtdelegate, "String");
	String    = (typeof(String))send((struct object *)String_vt, s_vtallocate, 0);

	assert(vtof(String) == String_vt);

	send(String_vt, s_vtadd_method, s_newp,   (method_t)String_newp);
	send(String_vt, s_vtadd_method, s_length, (method_t)String_length);
	send(String_vt, s_vtadd_method, s_print,  (method_t)String_print);
	//adding concatenation method
	send(String_vt, s_vtadd_method, s_append, (method_t)String_append);	

	struct object *greet = send(String, s_newp, "Object Machine v1.0\n");
	struct object *h     = send(String, s_newp, "hello");
	struct object *sp    = send(String, s_newp, " ");
	struct object *w     = send(String, s_newp, "world");
	struct object *nl    = send(String, s_newp, "\n");

	send(greet, s_print);
	printf("hello length %d\n", oop2i(send(h, s_length)));

	send(h,  s_print);
	send(sp, s_print);
	send(w,  s_print);
	send(nl, s_print);

	//concatenation of h and w
	struct object *newString;
	newString = send(h, s_append, sp);
	newString = send(newString, s_append, w);
	newString = send(newString, s_append, nl);
	//printing concatenated string
	printf("The concatenated String is: ");
	send(newString, s_print);

	printf("Testing Array\n");
	Array_vt  = (typeof(Array_vt)) send(Object_vt, s_vtdelegate, "Array");
	Array     = (typeof(Array))    send((struct object *)Array_vt,  s_vtallocate, 0);

	assert(vtof(Array) == Array_vt);

	send(Array_vt,  s_vtadd_method, s_newp,   (method_t)Array_newp);
	send(Array_vt,  s_vtadd_method, s_length, (method_t)Array_length);
	send(Array_vt,  s_vtadd_method, s_at,     (method_t)Array_at);
	send(Array_vt,  s_vtadd_method, s_atput,  (method_t)Array_atput);

	struct object *line = send(Array, s_newp, 4);

	printf("array elements %d\n", oop2i(send(line, s_length)));
	send(line, s_atput, 1, h);
	send(line, s_atput, 2, sp);
	send(line, s_atput, 3, w); send(line, s_atput, 4, nl);
	for (int i = 1; i <= 4; i++)
		send(send(line, s_at, i), s_print);
	return 0;
}
