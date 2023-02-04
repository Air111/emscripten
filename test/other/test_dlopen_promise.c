#include <emscripten/emscripten.h>
#include <assert.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>

int mydata = 0;

em_promise_result_t on_fullfilled(void **result, void* data, void *handle) {
  assert(data == &mydata);
  printf("onsuccess\n");
  int* foo = (int*)dlsym(handle, "foo");
  assert(foo);
  printf("foo = %d\n", *foo);
  assert(*foo == 42);
  exit(0);
}

em_promise_result_t on_rejected(void **result, void* data, void *value) {
  assert(data == &mydata);
  printf("onerror %s\n", dlerror());
  return EM_PROMISE_FULFILL;
}

int main() {
  em_promise_t inner = emscripten_dlopen_promise("libside.so", RTLD_NOW);
  em_promise_t outer = emscripten_promise_then(outer, on_fullfilled, on_rejected, &mydata);
  emscripten_promise_destroy(outer);
  printf("returning from main\n");
  return 99;
}
