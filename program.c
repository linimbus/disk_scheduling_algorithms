#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum saFlag { SA_FCFS = 1, SA_SSTF = 2, SA_CSCAN = 3, SA_LOOK = 4 };

#define HEAD_DIR_UP 1
#define HEAD_DIR_DOWN 0

#define SECT_MAX 199
#define SECT_MIN 0

struct sectorRequests {
  int secter;
  int atime;
  int done;
};

int requestCnt = 0;
struct sectorRequests requestList[10000];

int compare(const void *a, const void *b) {
  struct sectorRequests *aa = (struct sectorRequests *)a;
  struct sectorRequests *bb = (struct sectorRequests *)b;
  if (aa->atime > bb->atime) {
    return 1;
  } else if (aa->atime == bb->atime) {
    return 0;
  }
  return -1;
}

void sortByatime(struct sectorRequests *list, int cnt) {
  qsort(list, cnt, sizeof(struct sectorRequests), compare);
}

int sectorAbs(int a, int b) {
  if (a > b) {
    return a - b;
  }
  return b - a;
}

void orderRequestProc(struct sectorRequests *list, int cnt, int secter, int dir,
                      int *mov, int *time) {
  int totalmov = 0;
  int totaltime = 0;

  for (int i = 0; i < cnt; i++) {
    int newdir = HEAD_DIR_DOWN;

    if (list[i].secter > secter) {
      newdir = HEAD_DIR_UP;
    }

    if (newdir != dir) {
      totaltime += 5;
      dir = newdir;
    }

    int move = sectorAbs(list[i].secter, secter);
    fprintf(stdout, "%d mov %d\n", list[i].secter, move);

    totalmov += move;
    totaltime += move / 10;

    secter = list[i].secter;
    list[i].done = 1;
  }

  *mov += totalmov;
  *time += totaltime;
}

// Scheduling according to the time sequence of process requests to access the
// disk
void requestFCFS(struct sectorRequests *list, int cnt, int secter, int dir) {
  int totalmov = 0;
  int totaltime = 0;

  sortByatime(list, cnt);

  orderRequestProc(list, cnt, secter, dir, &totalmov, &totaltime);

  fprintf(stdout, "TotalMov %d, TotalTime %d\n", totalmov, totaltime);
}

struct sectorRequests *findClosest(struct sectorRequests *list, int cnt,
                                   int secter) {
  int distance = SECT_MAX + 1;
  struct sectorRequests *find = NULL;
  for (int i = 0; i < cnt; i++) {
    if (list[i].done) {
      continue;
    }
    int temp = sectorAbs(list[i].secter, secter);
    if (distance > temp) {
      find = &list[i];
      distance = temp;
    }
  }
  return find;
}

// The required to be accessed is the closest
int requestSSTF(struct sectorRequests *list, int cnt, int secter, int dir) {
  int totalmov = 0;
  int totaltime = 0;

  for (;;) {
    struct sectorRequests *find = findClosest(list, cnt, secter);
    if (find == NULL) {
      break;
    }

    int newdir = HEAD_DIR_DOWN;
    if (find->secter > secter) {
      newdir = HEAD_DIR_UP;
    }

    if (newdir != dir) {
      totaltime += 5;
      dir = newdir;
    }

    int move = sectorAbs(find->secter, secter);

    totalmov += move;
    totaltime += move / 10;

    secter = find->secter;
    find->done = 1;
  }

  fprintf(stdout, "TotalMov %d, TotalTime %d\n", totalmov, totaltime);
}

int compareBySector(const void *a, const void *b) {
  struct sectorRequests *aa = (struct sectorRequests *)a;
  struct sectorRequests *bb = (struct sectorRequests *)b;
  if (aa->secter > bb->secter) {
    return 1;
  } else if (aa->secter == bb->secter) {
    return 0;
  }
  return -1;
}

void sortBySector(struct sectorRequests *list, int cnt) {
  qsort(list, cnt, sizeof(struct sectorRequests), compareBySector);
}

int compareBySector2(const void *a, const void *b) {
  struct sectorRequests *aa = (struct sectorRequests *)a;
  struct sectorRequests *bb = (struct sectorRequests *)b;
  if (aa->secter < bb->secter) {
    return 1;
  } else if (aa->secter == bb->secter) {
    return 0;
  }
  return -1;
}

void sortBySector2(struct sectorRequests *list, int cnt) {
  qsort(list, cnt, sizeof(struct sectorRequests), compareBySector2);
}

// CSCAN: Sort all requests greater than the current position; Sort all requests
// less than the current position;
int requestLargerCnt = 0;
struct sectorRequests requestLargerList[10000];
int requestSmallCnt = 0;
struct sectorRequests requestSmallList[10000];

void filterLargerReq(struct sectorRequests *list, int cnt, int secter) {
  for (int i = 0; i < cnt; i++) {
    if (list[i].secter > secter) {
      int idx = requestLargerCnt;
      requestLargerList[idx].atime = list[i].atime;
      requestLargerList[idx].secter = list[i].secter;
      requestLargerCnt++;
    }
  }
}

void filterSmallReq(struct sectorRequests *list, int cnt, int secter) {
  for (int i = 0; i < cnt; i++) {
    if (list[i].secter < secter) {
      int idx = requestSmallCnt;
      requestSmallList[idx].atime = list[i].atime;
      requestSmallList[idx].secter = list[i].secter;
      requestSmallCnt++;
    }
  }
}

int requestCSCAN(struct sectorRequests *list, int cnt, int secter, int dir) {
  filterLargerReq(list, cnt, secter);
  filterSmallReq(list, cnt, secter);

  // c-scan : start at one end
  requestLargerList[requestLargerCnt++].secter = SECT_MAX;
  sortBySector(requestLargerList, requestLargerCnt);

  requestSmallList[requestSmallCnt++].secter = SECT_MIN;
  sortBySector(requestSmallList, requestSmallCnt);

  int totalmov = 0;
  int totaltime = 0;

  if (requestLargerCnt > 0) {
    orderRequestProc(requestLargerList, requestLargerCnt, secter, dir,
                     &totalmov, &totaltime);
    secter = requestLargerList[requestLargerCnt - 1].secter;
  }

  orderRequestProc(requestSmallList, requestSmallCnt, secter, HEAD_DIR_UP,
                   &totalmov, &totaltime);
  fprintf(stdout, "TotalMov %d, TotalTime %d\n", totalmov, totaltime);
}

int requestLOOK(struct sectorRequests *list, int cnt, int secter, int dir) {
  filterLargerReq(list, cnt, secter);
  filterSmallReq(list, cnt, secter);

  sortBySector(requestLargerList, requestLargerCnt);
  sortBySector2(requestSmallList, requestSmallCnt);

  int totalmov = 0;
  int totaltime = 0;

  if (requestLargerCnt > 0) {
    orderRequestProc(requestLargerList, requestLargerCnt, secter, dir,
                     &totalmov, &totaltime);
    secter = requestLargerList[requestLargerCnt - 1].secter;
  }

  orderRequestProc(requestSmallList, requestSmallCnt, secter, HEAD_DIR_UP,
                   &totalmov, &totaltime);
  fprintf(stdout, "TotalMov %d, TotalTime %d\n", totalmov, totaltime);
}

int parseFlag(char *name) {
  if (strcmp(name, "FCFS") == 0) {
    return SA_FCFS;
  } else if (strcmp(name, "SSTF") == 0) {
    return SA_SSTF;
  } else if (strcmp(name, "CSCAN") == 0) {
    return SA_CSCAN;
  } else if (strcmp(name, "LOOK") == 0) {
    return SA_LOOK;
  }
  fprintf(stderr, "Unknown AS Name: %s\n", name);
  exit(-1);
}

int parseDir(char *name) {
  if (strcmp(name, "ao") == 0) {
    return HEAD_DIR_UP;
  } else if (strcmp(name, "do") == 0) {
    return HEAD_DIR_DOWN;
  }
  fprintf(stderr, "Unknown Dir: %s\n", name);
  exit(-1);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: <FCFS|SSTF|CSCAN|LOOK> <0~9999> <ao/do>\n");
    return -1;
  }

  int runASFlag = parseFlag(argv[1]);
  int initSecter = 0;

  if (1 != sscanf(argv[2], "%d", &initSecter)) {
    fprintf(stderr, "Unknown Secter: %s\n", argv[2]);
    exit(-1);
  }

  int initDir = parseDir(argv[3]);

  fprintf(stdout, "ASFLAG %d, Secter %d, Dir %d\n", runASFlag, initSecter,
          initDir);

  memset(requestList, 0, sizeof(requestList));

  char buff[128];
  while (1) {
    char *ret = fgets(buff, sizeof(buff), stdin);
    if (ret == 0 || strcmp(buff, "\n") == 0) {
      break;
    }
    int secter, atime;
    int cnt = sscanf(buff, "%d %d", &secter, &atime);
    if (cnt != 2) {
      fprintf(stderr, "Unknown Secter and Arrival time: %s\n", buff);
      exit(-1);
    }
    if (secter > SECT_MAX || secter < SECT_MIN) {
      fprintf(stderr, "Unknown Secter [0-9999]: %d\n", secter);
      exit(-1);
    }
    int idx = requestCnt;
    requestList[idx].atime = atime;
    requestList[idx].secter = secter;
    requestCnt++;
    // fprintf(stdout, "%d %d\n", secter, atime);
  }

  switch (runASFlag) {
  case SA_FCFS: {
    requestFCFS(requestList, requestCnt, initSecter, initDir);
  } break;
  case SA_SSTF: {
    requestSSTF(requestList, requestCnt, initSecter, initDir);
  } break;
  case SA_CSCAN: {
    requestCSCAN(requestList, requestCnt, initSecter, initDir);
  } break;
  case SA_LOOK: {
    requestLOOK(requestList, requestCnt, initSecter, initDir);
  } break;
  }

  return 0;
}
