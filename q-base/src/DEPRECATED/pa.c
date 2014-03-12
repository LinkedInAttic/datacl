#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "constants.h"
#include "macros.h"

#undef DEBUG

int g_n_routes;
int g_n_trace;
int g_trace_x[1024];
int g_trace_y[1024];

int 
read_input(
	   int *ptr_nX,
	   int *ptr_nY,
	   unsigned char **ptr_data
	   )
{
  int status = 0;
  int nX, nY; unsigned char *data = NULL;
  int itemp;
  int k = 0;
  fscanf(stdin, "%d", &nX); 
  fscanf(stdin, "%d", &nY); 
  data = (unsigned char *)malloc(nX * nY * sizeof(unsigned char));
  return_if_malloc_failed(data);
  for ( int i = 0; i < nX; i++ ) { 
    for ( int j = 0; j < nY; j++ ) { 
      fscanf(stdin, "%d", &itemp);
      data[k++] = itemp;
    }
  }

  *ptr_nX = nX;
  *ptr_nY = nY;
  *ptr_data = data;
 BYE:
  return(status);
}

void
pr_input(
	 int nX, 
	 int nY, 
	 int n_mine,
	 unsigned char *data
	 )
{
  int k = 0;
  for ( int j = 0; j < nY; j++ ) { 
    for ( int i = 0; i < nX; i++ ) { 
      fprintf(stderr, "%d ", (int)data[k]);
      k++;
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\nNumber of my cells = %d \n", n_mine);
}

bool
is_move_ok(
	   int x,
	   int nX,
	   int y,
	   int nY,
	   int curr_total,
	   int n_mine,
	   unsigned char *data,
	   unsigned char *history,
	   unsigned int last_loc
	   )
{
  int loc, cloc, bloc;
  unsigned char uctemp;
  /* Is it outside grid? */
  if ( ( x < 0 ) || ( y < 0 ) || ( x >= nX ) || ( y >= nY ) ) { return false; }
  loc = y * nX + x;
  /* Did you just come from here */
  if ( loc == last_loc ) { 
    return false;
  }
  /* Did you reach destination too quickly? */
  if ( ( data[loc] == 3 ) && ( curr_total != n_mine ) ) { return false; }
  /* Is it somebody else's spot? */
  if ( data[loc] == 1 ) { return false; }
  /* Is it origin? */
  if ( data[loc] == 2 ) { return false; }
  /* Have you been there before? */
  cloc = loc / 8 ; /* TODO Bit shift */
  bloc = loc - ( cloc * 8 ); /* TODO: Bit shift */
  uctemp = 1 << bloc;
  if ( ( history[cloc] & uctemp ) != 0 ) {
    return false;
  }
  return true;
}
int
pr_g_trace(
    )
{
  fprintf(stderr, "------------------\n");
  for ( int i = 0; i < g_n_trace; i++ ) { 
    fprintf(stderr, "(%d, %d) \n",g_trace_x[i],g_trace_y[i]);
  }
  fprintf(stderr, "------------------\n");
}

int
search(
       int x,
       int nX,
       int y,
       int nY,
       int curr_total,
       int n_mine,
       unsigned char *data,
       unsigned char *history,
       int n_history,
       unsigned int last_loc
       )
{
  bool is_ok;
  int newx, newy, new_curr_total, chk_sum;
  int loc, cloc, bloc;
  unsigned char uctemp, mask;
  char pr_str[8];
  /* Debugging check */
  chk_sum = 0;
  for ( int i = 0; i < n_history; i++ ) {
    uctemp = history[i]; mask = 1;
    for ( int j = 0; j < 8; j++ ) { 
      chk_sum += ( uctemp & 0x00000001 );
      uctemp = uctemp >> 1;
    }
  }
  if ( chk_sum != curr_total ) { 
    fprintf(stderr, "ERROR: chk_sum = %d, curr_total = %d\n", chk_sum,
	curr_total);
  }

  /* Generate next move. 4 possibilities to consider */
  // fprintf(stderr, "Loc = (%d, %d), Total = %d\n", x, y, curr_total);
  /* Move right */
  for ( int moves = 0; moves < 4; moves++ ) {
    switch ( moves ) { 
    case 0 : /* move right */
      newx = x + 1;
      newy = y;
      break;
    case 1 : /* move left */
      newx = x - 1;
      newy = y;
      break;
    case 2 : /* move down */
      newx = x;
      newy = y + 1;
      break;
    case 3 : /* move up */
      newx = x;
      newy = y - 1;
      break;
    default : 
      return -1;
      break;
    }
    is_ok = is_move_ok(newx, nX, newy, nY, curr_total, n_mine, data, 
		       history, last_loc);
    /* Check whether done */
    if ( ( is_ok ) && ( curr_total == n_mine ) ) {
      fprintf(stderr, "ROUTE FOUND %d!!! \n", g_n_routes);
      g_n_routes++;
      // pr_g_trace();
      /* Undo the history */
      g_n_trace--;
      loc = y * nX + x;
      cloc = loc / 8 ; /* TODO Bit shift */
      bloc = loc - ( cloc * 8 ); /* TODO: Bit shift */
      uctemp = 1 << bloc;
      uctemp = ~uctemp;
      history[cloc] &= uctemp;
      return 0;
    }
    /*
      if ( is_ok ) {
      strcpy(pr_str, "");
      fprintf(stderr, "Move from (%d, %d) to (%d, %d) is %s okay \n",
      x, y, newx, newy, pr_str);
      }
    */
    if ( is_ok ) { 
      g_trace_x[g_n_trace] = newx;
      g_trace_y[g_n_trace] = newy;
      g_n_trace++;
      new_curr_total = curr_total + 1; /* one more accounted for */
      /* update history */
      loc = newy * nX + newx;
      cloc = loc / 8 ; /* TODO Bit shift */
      bloc = loc - ( cloc * 8 ); /* TODO: Bit shift */
      uctemp = 1 << bloc;
      history[cloc] |= uctemp;
      search(newx, nX, newy, nY, new_curr_total, n_mine, data, history,
	     n_history, last_loc);
    }
    else {
      /* WHAT DO WE DO HERE ? */
    }
  }
  /* Backtrack and Undo the history */
  g_n_trace--;
  loc = y * nX + x;
  cloc = loc / 8 ; /* TODO Bit shift */
  bloc = loc - ( cloc * 8 ); /* TODO: Bit shift */
  uctemp = 1 << bloc;
  uctemp = ~uctemp;
  history[cloc] &= uctemp;
}




int
main()
{
  int status = 0;
  int nX, nY, n_mine = 0, last_loc = -1; unsigned char *data = NULL;
  unsigned char *history = NULL; /* bit mask for grid */ int n_history = 0;
  int k, start_x, start_y; bool is_start_found = true;
  /* Read the input. */
  status = read_input(&nX, &nY, &data);
  cBYE(status);
  n_history = ( ( nX * nY ) / 8 ) + 1;
  history = (unsigned char *)malloc(n_history * sizeof(unsigned char));
  return_if_malloc_failed(history);
  for ( int i = 0; i < n_history; i++ ) { 
    history[i] = 0;
  }
  for ( int i = 0; i < nX * nY; i++ ) { 
    if ( data[i] == 0 ) { 
      n_mine++;
    }
  }
  k = 0;
  start_x = start_y = -1;
  for ( int j = 0; j < nY; j++ ) { 
    for ( int i = 0; i < nX; i++ ) { 
      if ( data[k] == 2 ) {  
	start_x = i;
	start_y = j;
	is_start_found = true;
	break;
      }
      k++;
    }
    if ( is_start_found ) { break; }
  }
  if ( ( start_x < 0 ) || ( start_y < 0 ) ) { go_BYE(-1); }
#ifdef DEBUG
  pr_input(nX, nY, n_mine, data);
#endif
  g_n_routes = 0;
  g_n_trace = 0;
  g_trace_x[g_n_trace] = start_x;
  g_trace_y[g_n_trace] = start_y;
  g_n_trace++;
  search(start_x, nX, start_y, nY, 0, n_mine, data, history, n_history, 
	 last_loc);

 BYE:
  free_if_non_null(history);
  free_if_non_null(data);
  return(status);
}

