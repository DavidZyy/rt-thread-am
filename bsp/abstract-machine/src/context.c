#include <am.h>
#include <klib.h>
#include <rtthread.h>

static Context* ev_handler(Event e, Context *c) {
  switch (e.event) {
    case EVENT_YIELD: {
      Context **to   = (Context **) c->gpr[11]; // a1
      Context **from = (Context **) c->gpr[10]; // a0
      // printf("Context a0 is: %p\n", c->gpr[10]);
      // printf("Context a1 is: %p\n", c->gpr[11]);
      c = *to;
      *from = c;
      // printf("Context a1 is: %p\n", c);
      // printf("mepc is: %p\n", c->mepc);
      // assert(0);
      break;
    }
    default: printf("Unhandled event ID = %d\n", e.event); assert(0);
  }
  return c;
}

void __am_cte_init() {
  cte_init(ev_handler);
}

// to is the addr of the Context*, not the Context* ?
void rt_hw_context_switch_to(rt_ubase_t to) {
  // printf("to is:%p\n", *((u_int32_t *)to));
  // for(int i=0; i<16; i++)
  // printf("mepc is: %p\n", ((Context *)to)->gpr[i]);
  // uint32_t addr = *((u_int32_t *)to);

  asm volatile("mv a1, %0" :: "r"(to)); // Move the value of 'to' to register a0
  yield();
  // assert(0);
}

void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to) {
  // printf("from is: %p, to is:%p\n", (void *)from, (void *)to);
  asm volatile("mv a0, %0" :: "r"(from)); // Move the value of 'from' to register a0
  asm volatile("mv a1, %0" :: "r"(to)); // Move the value of 'to' to register a1
  yield();
  // assert(0);
}

void rt_hw_context_switch_interrupt(void *context, rt_ubase_t from, rt_ubase_t to, struct rt_thread *to_thread) {
  assert(0);
}

void wrapper_func(void *tentry, void *parameter, void *texit) {
  // printf("in wrapper_func, tentry is: %p, parameter is: %p, texit is: %p\n", tentry, parameter, texit);
  // Cast the tentry to the appropriate function pointer type
  void (*entry_func)(void*) = (void(*)(void*))tentry;
  void (*exit_func)() = (void(*)())texit;

  // Call the entry_func with the parameter
  entry_func(parameter);
  exit_func();
}

rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit) {
  // printf("stack_addr before align is:%p\n", stack_addr);
  uintptr_t size = sizeof(uintptr_t)-1;
  uintptr_t a = (uintptr_t)stack_addr & ~size;
  stack_addr  = (rt_uint8_t *)a;
  // printf("size is: %d\n", size);

  void *para[3];
  para[0] = tentry;
  para[1] = parameter;
  para[2] = texit;
  // printf("stack_addr is:%p\n", stack_addr);
  // printf("para : %p\n", para);
  Context *p = kcontext((Area){NULL, stack_addr}, (void(*)(void*))wrapper_func, para);
  // Context *p = kcontext((Area){NULL, stack_addr}, (void(*)(void*))wrapper_func, tentry);
  // printf("p is:%p\n", p);
//   printf("tentry is: %p, parameter is: %p, texit is: %p\n", tentry, parameter, texit);
// 
//   printf("\n");
  // assert(0);
  return (rt_uint8_t *)p;
  // return (rt_uint8_t *)0xf;
  // return NULL;
}
