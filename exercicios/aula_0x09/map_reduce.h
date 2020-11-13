#ifndef __MAP_REDUCE_H
#define __MAP_REDUCE_H

void **mapear(void **itens, void *(*funcao)(void *), int nr_itens);

#endif
