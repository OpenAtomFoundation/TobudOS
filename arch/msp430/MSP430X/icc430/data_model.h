
#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#if __DATA_MODEL__ == __DATA_MODEL_SMALL__
	#define pushm_x pushm.w
	#define popm_x  popm.w
	#define push_x  push.w
	#define pop_x   pop.w
	#define mov_x   mov.w
	#define cmp_x   cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_MEDIUM__
	#define pushm_x pushm.a
	#define popm_x  popm.a
	#define push_x  pushx.a
	#define pop_x   popx.a
	#define mov_x   mov.w
	#define cmp_x   cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_LARGE__
	#define pushm_x pushm.a
	#define popm_x  popm.a
	#define push_x  pushx.a
	#define pop_x   popx.a
	#define mov_x   movx.a
	#define cmp_x   cmpx.a
#endif

#ifndef pushm_x
	#error The assembler options must define one of the following symbols: __DATA_MODEL_SMALL__, __DATA_MODEL_MEDIUM__, or __DATA_MODEL_LARGE__
#endif

#endif /* DATA_MODEL_H */

