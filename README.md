# Harbol

## Introduction

**Harbol** is a collection of data structures and miscellaneous libraries, similar in nature to C++'s Boost, STL, and GNOME's GLib; it is meant to be a smaller and more lightweight collection of data structures, code systems, and convenience software.


### Features

* Variant type - supports any type of values and their type IDs.
* C++-style String type.
* Dynamic/Static Array (can be used as either a dynamic array (aka vector) or as a static fat array.)
* Ordered Hash Table.
* Byte Buffer.
* Tuple type - convertible to structs, can also be packed.
* Memory Pool - accomodates any size.
* Object Pool - like the memory pool but for fixed size data/objects.
* N-ary Tree.
* JSON-like Key-Value Configuration File Parser - allows retrieving data from keys through python-style pathing.
* Plugin Manager - designed to be wrapped around to provide an easy-to-setup plugin API and plugin SDK.
* Fixed Size floating-point types.
* Double Ended Queue (Deque).
* Lexing tools for C/Golang style numbers and strings.


### Future

* C compiler/interpreter
* reg allocator -> https://github.com/bytecodealliance/regalloc2
You have to store instructions from all basic blocks in one continuous array
^ necessary for linear scan regalloc, cause you need index -> instruction mapping 

* HTML Generator


## Usage

```c
#include "harbol.h"

int main(const int argc, char *argv[])
{
	bool result = false;
	struct HarbolString str = harbol_string_make("my initial string!", &result);
	if( !result ) {
		printf("str failed to initialize\n");
		return 1;
	}
	harbol_string_add_cstr(&str, "and another string concatenated!");
	harbol_string_clear(&str);
	
	struct HarbolArray vec = harbol_array_make(ARRAY_DEFAULT_SIZE, &result);
	if( !result ) {
		printf("vec failed to initialize\n");
		return 1;
	}
	harbol_array_insert(&vec, &( float32_t ){2.f}, sizeof(float32_t));
	harbol_array_insert(&vec, &( float32_t ){3.f}, sizeof(float32_t));
	harbol_array_insert(&vec, &( float32_t ){4.f}, sizeof(float32_t));
	const float32_t f = *( const float32_t* )harbol_vector_get(&vec, 1);
	harbol_array_clear(&vec);
	
	struct HarbolMap *ptrmap = harbol_map_new(8);
	if( ptrmap==NULL ) {
		printf("hash table failed to initialize\n");
		return 1;
	}
	harbol_map_insert(ptrmap, "style", sizeof "style", &( float64_t ){2.3553}, sizeof(float64_t));
	harbol_map_insert(ptrmap, "border", sizeof "border", &( float64_t ){12.995}, sizeof(float64_t));
	harbol_map_free(&ptrmap);
}
```

## Contributing

To submit a patch, first file an issue and/or present a pull request.

## Help

If you need help or have any question, make an issue on the github repository.
Simply drop a message or your question and you'll be reached in no time!

## Installation

### Requirements

C99 compliant compiler and libc implementation with stdlib.h, stdio.h, and stddef.h.

### Building

To build the library, simply run `make harbol_static` which will make the static library version of libharbol.
for a shared library version, run `make harbol_shared`.

To clean up the `.o` files, run `make clean`.

To build a debug version of the library, run `make debug`.

### Testing

For testing code changes or additions, simply run `make test` which will build test executables within each library folder.
After that, run `make run_test` to execute ALL compiled tests for each library component.

Running `make clean` _WILL_ delete the test executables and their result outputs.

## Credits

* Kevin Yonan - main developer of Harbol.


## Contact

I can be contacted at kevyonan@gmail.com; No soliciting or spam.


## License

This project is licensed under Apache License.
